#include "StdAfx.h"
#include "GlbGlobeTerrain.h"
#include "GlbDataSourceFactory.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include <osg\Switch>
#include <osg\AlphaFunc>
#include <osg\CullFace>
#include <osg\MatrixTransform>
#include <osg\Geode>
#include <osg/PolygonMode>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/Material>
#include <osg/LightModel>
#include <osg/Depth>

#include "GlbGlobeMemCtrl.h"
#include "GlbGlobeTiltPhotographyLayer.h"

//#ifdef _DEBUG
//		WCHAR wBuff[128];
//		wsprintf(wBuff,L"visible tile count: %d .\n",visible_cnt);
//		OutputDebugString(wBuff);
//#endif

using namespace GlbGlobe;

CGlbGlobeTerrain::GlbTileOperation::GlbTileOperation(	GLBOperationEnum addordelormodity, 
														GLBOperationObjectEnum operation, 
														osg::Group* p_tile_node,
														double compiletime)
{
	_addordelormodify = addordelormodity;
	_nodeordrawableorvecarrayortexture = operation;
	_p_tile_node = p_tile_node;
	_node_compiletime = compiletime;
}

CGlbGlobeTerrain::GlbTileOperation::GlbTileOperation(GLBOperationEnum addordelormodity, 
													 GLBOperationObjectEnum operation,
													 osg::Geode* p_tile_geode_node, 
													 osg::Drawable* p_drawable,
													 osg::Vec3Array* p_vecarray)
{
	_addordelormodify = addordelormodity;
	_nodeordrawableorvecarrayortexture = operation;
	_p_tile_geode_node = p_tile_geode_node;
	_p_drawable = p_drawable;
	_p_vecarray = p_vecarray;
}

CGlbGlobeTerrain::GlbTileOperation::GlbTileOperation ( GLBOperationEnum addordelormodity, 
														GLBOperationObjectEnum operation,
														osg::Texture* p_texture,
														osg::Image* p_image)
{
	_addordelormodify = addordelormodity;
	_nodeordrawableorvecarrayortexture = operation;
	_p_texture = p_texture;
	_p_image = p_image;
}

CGlbGlobeTerrain::GlbTileOperation::~GlbTileOperation()
{		
	_p_tile_node = NULL;			// 节点
	_p_tile_geode_node = NULL;		///< 叶子节点
	_p_drawable = NULL;				///< 可绘制对象
	_p_vecarray = NULL;				///< 顶点数组
	_p_texture = NULL;
	_p_image = NULL;
}

CGlbGlobeTerrain::CGlbGlobeTerrainCallBack::CGlbGlobeTerrainCallBack(CGlbGlobeTerrain* pGlobeTerrain)
{
	m_p_globe_terrian = pGlobeTerrain;
}

void CGlbGlobeTerrain::CGlbGlobeTerrainCallBack::operator() (osg::Node* node,osg::NodeVisitor* nv)
{			
	if (m_p_globe_terrian && nv->getVisitorType()==osg::NodeVisitor::UPDATE_VISITOR)
	{
		m_p_globe_terrian->UpdateOsgCallBack(node,nv);
	}
	// must continue subgraph traversal.
	traverse(node,nv);		
}
//////////////////////////////////////////////////////////////////////////
CGlbGlobeTerrain::CGlbGlobeTerrain(CGlbGlobe* globe)
{
	mpr_tdataset = NULL;
	mpr_node = new osg::Switch;
	InitializeCriticalSection(&mpr_tDatasetAccessCritical);
	InitializeCriticalSection(&mpr_osgcritical);
	InitializeCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
	//mpr_levelzero_tilesize_degrees = 36.0; // 36度
	mpr_lzts = 36.0;
	mpr_is_camera_underground = false;
	
	mpr_isInitialized = false;
	mpr_visible = true;
	mpr_globe = globe;
	mpr_south				= -90.0;
	mpr_north				= 90.0;
	mpr_east				= 180.0;
	mpr_west				= -180.0;
	mpr_opacity				= 100;	
	mpr_exaggrate			= 1.0;
	EnableWireMode(false);	
	mpr_enablecullface = true;
	mpr_enableUpdate = true;
	//mpr_prebufferchange_time	= -1;
	mpr_bOpacityChanged = false;
	mpr_addOperationCount = 0;
}


CGlbGlobeTerrain::~CGlbGlobeTerrain(void)
{
	Dispose();
	if (mpr_node->getUpdateCallback())
		mpr_node->removeUpdateCallback(mpr_node->getUpdateCallback());
	mpr_node = NULL;
	mpr_tdataset = NULL;
	DeleteCriticalSection(&mpr_tDatasetAccessCritical);
	DeleteCriticalSection(&mpr_osgcritical);
	DeleteCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
}

/****************************************************************/
glbBool CGlbGlobeTerrain::SetTerrainDataset(IGlbTerrainDataset* tdataset)
{
	if (tdataset == mpr_tdataset.get())
		return true;

	if (mpr_globe->GetType()==GLB_GLOBETYPE_FLAT)
	{
		glbref_ptr<CGlbExtent> _extent = (CGlbExtent*)tdataset->GetExtent();
		if(mpr_globe->mpr_sceneobjIdxManager) mpr_globe->mpr_sceneobjIdxManager=NULL;

		mpr_globe->mpr_sceneobjIdxManager = new CGlbGlobeSceneObjIdxManager(L"CreatarGlobe SceneManager",8/*默认树深8级*/,_extent.get());
	}
// 临界区与地形更新互斥
EnterCriticalSection(&mpr_tDatasetAccessCritical);
	if (mpr_isInitialized)
	{
		// 首先清空所有的块
		Dispose();
		mpr_topmost_tiles.clear();
		// 更改dataset
		mpr_tdataset = tdataset;
		// 重新初始化
		Initialize();
	}
	else
		mpr_tdataset = tdataset;

LeaveCriticalSection(&mpr_tDatasetAccessCritical);
	return true;
}

IGlbTerrainDataset* CGlbGlobeTerrain::GetTerrainDataset()
{
	return mpr_tdataset.get();
}
glbBool CGlbGlobeTerrain::Initialize()
{
	if (mpr_globe==NULL)
		return false;

	if (mpr_isInitialized)//只初始化一次
		return true;
	if (mpr_tdataset==NULL)
	{

		return false;
	}

	// 启动alpha测试，过滤alpha=0的区域 
	mpr_node->getOrCreateStateSet()->setAttributeAndModes( new osg::AlphaFunc( osg::AlphaFunc::GREATER, 0.01f ) , osg::StateAttribute::ON);
	// 默认禁用混合
	mpr_node->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OFF);
	mpr_node->getOrCreateStateSet()->setRenderingHint( osg::StateSet::OPAQUE_BIN );
	// 默认启用深度测试 !!!???不加不行呀
	mpr_node->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	// 关闭光照
	mpr_node->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

	// 设置场景类型
	mpr_globe_type = mpr_globe->GetType();

	std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers;
	std::vector<glbref_ptr<CGlbGlobeDemLayer>> demlays;
	std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs; 
	std::vector<glbref_ptr<CGlbGlobeRObject>> mTerrainobjs;	

	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{// 创建5*10个tile的第0级基础球
		// 打开背面剔除 cullface		
		if (mpr_enablecullface)
			mpr_node->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::BACK),osg::StateAttribute::ON);
		else
			mpr_node->getOrCreateStateSet()->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);

		glbInt32 latitude_cnt = 180.0 / mpr_lzts + 0.5;
		glbInt32 longitude_cnt = 360.0 / mpr_lzts + 0.5;

		mpr_levelzero_tiles_columns = longitude_cnt;
		mpr_levelzero_tiles_rows = latitude_cnt;
		// 测试：先用两个块测
		//latitude_cnt = 4;
		//longitude_cnt = 9;
		// end 测试
		for(glbInt32 curRow = 0/*3*/; curRow < latitude_cnt; curRow++)	
		{
			for(glbInt32 curCol = 0/*7*/; curCol < longitude_cnt; curCol++)
			{
				glbInt32 key =  curRow * longitude_cnt + curCol;
				CGlbGlobeTerrainTile* qt = NULL;			

				glbDouble west = -180.0f + curCol * mpr_lzts;
				if (west > mpr_east)//180.0)
					continue;

				glbDouble east = west + mpr_lzts;
				if (east < mpr_west)//-180.0)
					continue;

				//glbDouble south = -90.0f + curRow * mpr_lzts;
				//if (south > mpr_north)//90.0)
				//	continue;

				//glbDouble north = south + mpr_lzts;
				//if (north < mpr_south)//-90.0)
				//	continue;	

				glbDouble north = 90.0 - curRow * mpr_lzts;
				if (north < mpr_south)//-90.0)
					continue;	

				glbDouble south = north - mpr_lzts;
				if (north < mpr_south)//-90.0)
					continue;			

				qt = new CGlbGlobeTerrainTile(this,0,south,north,west,east,0,0,NULL);	
				qt->Initialize(domlayers,demlays,terrainobjs,mTerrainobjs,true);
				mpr_topmost_tiles[key] = qt;
			}
		}

	}
	else if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
	{
		// 关闭背面剔除 cullface		
		//mpr_p_terrian_node->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace,osg::StateAttribute::OFF);

		// 获取地形图层相关属性	
		glbInt32 startTileColumn,startTileRow,endTileColumn,endTileRow;
		const CGlbExtent* terrain_extent=NULL;

		if(mpr_tdataset)
		{				
			terrain_extent = mpr_tdataset->GetExtent();
			terrain_extent->GetMin(&mpr_west,&mpr_south,0L);
			terrain_extent->GetMax(&mpr_east,&mpr_north,0L);

			mpr_tdataset->GetDomExtentCoverTiles(terrain_extent,0,startTileColumn,startTileRow,endTileColumn,endTileRow);
		
			mpr_levelzero_tiles_columns = endTileColumn - startTileColumn + 1;
			mpr_levelzero_tiles_rows = endTileRow - startTileRow + 1;	
			
			CGlbPixelBlock* p_dompixelblock=mpr_tdataset->CreateDomPixelBlock();			
			CGlbPixelBlock* p_dempixelblock=mpr_tdataset->CreateDemPixelBlock();
			double minx,maxx,miny,maxy,minz,maxz;

			//startTileColumn = 2; endTileColumn = 3;
			//startTileRow = 0; endTileRow = 1;
			for (glbInt32 i = startTileColumn; i <= endTileColumn; i++)
				for(glbInt32 j = startTileRow; j <= endTileRow; j++)
				{
					glbBool r1 = mpr_tdataset->ReadDom(0,i,j,p_dompixelblock);
					glbBool r2 = mpr_tdataset->ReadDem(0,i,j,p_dempixelblock);
					if (r1)
					{// 此块存在 ， 生成对应的terraintile ,加入到mpr_topmost_tiles数组中						
						CGlbExtent* blockextent = (CGlbExtent* )p_dompixelblock->GetExtent();
						if (blockextent==NULL)
						{
							GlbLogWOutput(GLB_LOGTYPE_ERR,L"平面地形初始化时出现地形块外包为空的情况！\r\n");
							return false;
						}
						blockextent->Get(&minx,&maxx,&miny,&maxy,&minz,&maxz);
						if (r2)
						{
							CGlbExtent* blockextent2 = (CGlbExtent*) p_dempixelblock->GetExtent();
							blockextent2->Get(0,0,0,0,&minz,&maxz);
							blockextent->Set(minx,maxx,miny,maxy,minz,maxz);
						}
						//CGlbGlobeTerrainTile* qt = new CGlbGlobeTerrainTile(this,0,j,i,*blockextent,NULL);
						CGlbGlobeTerrainTile* qt = new CGlbGlobeTerrainTile(this,0,miny,maxy,minx,maxx,minz,maxz,NULL);
						qt->SetPosition(j,i);
						glbInt32 tile_index = j * mpr_levelzero_tiles_columns + i;		
						mpr_topmost_tiles[tile_index] = qt;
						qt->Initialize(domlayers,demlays,terrainobjs,mTerrainobjs,true);						
					}
				}

			if (p_dompixelblock)
				delete p_dompixelblock;
			if (p_dempixelblock)
				delete p_dempixelblock;
		}		
	}	

	//mpr_p_terrian_node->setDataVariance(osg::Object::DYNAMIC);
	// 将Terrain_node挂到globe的组节点下(在CGlbGlobe::Initialized()中已经挂上了)
	if (mpr_node->getUpdateCallback()==NULL)
		mpr_node->setUpdateCallback(new CGlbGlobeTerrainCallBack(this));
	mpr_isInitialized = true;

	return true;
}
glbBool CGlbGlobeTerrain::IsInitialized()
{
	return mpr_isInitialized;
}

void CGlbGlobeTerrain::Update( std::vector<glbref_ptr<CGlbGlobeDomLayer>>  &domlayers,
								std::vector<glbref_ptr<CGlbGlobeDemLayer>> &demlayers,
								std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainobjs, 
								std::vector<glbref_ptr<CGlbGlobeRObject>>  &mTerrainobjs)
{
	if (!mpr_isInitialized || !mpr_visible || !mpr_enableUpdate)
		return;

//#ifdef _DEBUG
//	WCHAR wBuff[128];	
//	wsprintf(wBuff,L"CGlbGlobeTerrain::Update demlayers count: %d, mTerrainobjs count :%d\n",demlayers.size(),mTerrainobjs.size());
//	OutputDebugString(wBuff);
//#endif

// 临界区与地形更新互斥
EnterCriticalSection(&mpr_tDatasetAccessCritical);

	std::vector<glbref_ptr<CGlbGlobeRObject>> digholeObjs;
	// 倾斜摄影图层更新
	if (mpr_tilt_photography_layers.size()>0)
	{
		std::vector<glbref_ptr<CGlbGlobeLayer>>::iterator itr = mpr_tilt_photography_layers.begin();
		while(itr != mpr_tilt_photography_layers.end())
		{
			CGlbGlobeTiltPhotographyLayer* tiltLayer = dynamic_cast<CGlbGlobeTiltPhotographyLayer*>((*itr).get());
			if (tiltLayer && tiltLayer->IsShow())
			{				
				CGlbGlobeRObject* obj = dynamic_cast<CGlbGlobeRObject*>(tiltLayer->GetAssociateObject());
				if (obj)
				{
					digholeObjs.push_back(obj);
				}									
			}
			++itr;
		}
	}
	if (digholeObjs.size()>0)
	{// 添加内部挖洞对象
		for(size_t k = 0; k < digholeObjs.size(); k++)
		{
			terrainobjs.push_back(digholeObjs.at(k));
		}		
	}

	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
		UpdateGlobeTerrain(domlayers,demlayers,terrainobjs,mTerrainobjs);
	else if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
		UpdateFlatTerrain(domlayers,demlayers,terrainobjs,mTerrainobjs);
// 临界区与地形更新互斥
LeaveCriticalSection(&mpr_tDatasetAccessCritical);


	if (mpr_bOpacityChanged)
	{// 地形透明度修改
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
		std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
		while(_itr !=mpr_topmost_tiles.end())
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			_itr->second->FilterOnTerrainObjects(terrainobjs,childtobjs);		
			_itr->second->SetOpacity(domlayers,childtobjs,mpr_opacity);
			++_itr;
		}
		mpr_bOpacityChanged = false;
	}

}
void CGlbGlobeTerrain::Dispose()
{
	if (!mpr_isInitialized)
		return;

	std::map<glbInt32,glbref_ptr<CGlbGlobeTerrainTile>>::iterator itr = mpr_topmost_tiles.begin();
	while(itr != mpr_topmost_tiles.end())
	{
		itr->second->Dispose(true,true);	
		itr->second=NULL;
		itr++;
	}
	//mpr_topmost_tiles.clear();
	mpr_isInitialized = false;
}
void CGlbGlobeTerrain::AddOperation(GlbTileOperation opera)
{
	EnterCriticalSection(&mpr_osgcritical);	
	mpr_operations.push_back(opera);
	LeaveCriticalSection(&mpr_osgcritical);

	if (opera._addordelormodify == GLB_OPENERAION_ADD && opera._nodeordrawableorvecarrayortexture==GLB_NODE)
		mpr_addOperationCount++;
}

void CGlbGlobeTerrain::AddPackageOperation(GlbTileOperation opera)
{
	mpr_packageOperations.push_back(opera);
}

glbBool CGlbGlobeTerrain::IsUpdateNeedTerminate()
{
	if (mpr_addOperationCount > 3)
		return true;
	return false;
}
glbBool CGlbGlobeTerrain::ScreenToTerrainCoordinate(glbInt32 x,glbInt32 y,
								glbDouble &lonOrx,glbDouble &latOry,glbDouble &altOrz)
{
	CGlbGlobeView* p_globeview = mpr_globe->GetView();
	if (p_globeview==NULL)
		return false;
	if (mpr_opacity < 5) // 当透明度为0-4时,相当于地形是完全透明状态，此时不与地形求交
		return false;
	osg::Vec3d geostart(x,y,0);
	osg::Vec3d geoend(x,y,1);
	p_globeview->ScreenToWorld(geostart.x(),geostart.y(),geostart.z());
	p_globeview->ScreenToWorld(geoend.x(),geoend.y(),geoend.z());
	// 如果geostart，geoend是无效值
	if (geostart.valid()==false || geoend.valid()==false)
	{
		return false;
	}

	osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(geostart, geoend);
	osgUtil::IntersectionVisitor iv( lsi.get() );
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
		lsi->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
	}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		lsi->setIntersectionLimit(osgUtil::Intersector::NO_LIMIT);
	}

	// 如果倾斜摄影图层存在,优先考虑与倾斜摄影图层求交
	if (mpr_tilt_photography_layers.size()>0)
	{
		std::vector<glbref_ptr<CGlbGlobeLayer>>::iterator itr = mpr_tilt_photography_layers.begin();
		while(itr != mpr_tilt_photography_layers.end())
		{
			CGlbGlobeTiltPhotographyLayer* tiltLayer = dynamic_cast<CGlbGlobeTiltPhotographyLayer*>((*itr).get());
			if (tiltLayer && tiltLayer->IsShow())
			{				
				osg::Node* tiltNode = tiltLayer->GetOsgNode();	

				EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
				tiltNode->accept( iv );		
				LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	

				osgUtil::LineSegmentIntersector::Intersections& results = lsi->getIntersections();
				if (!results.empty() )
				{
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
						osg::Vec3d wgscoord = results.begin()->getWorldIntersectPoint();
						osg::Vec3d geodetic;
						g_ellipsmodel->convertXYZToLatLongHeight(
							wgscoord.x(),     wgscoord.y(),     wgscoord.z(),
							geodetic.y(), geodetic.x(), geodetic.z() );		
						lonOrx = osg::RadiansToDegrees(geodetic.x());// longitude
						latOry = osg::RadiansToDegrees(geodetic.y());// latitude
						altOrz = geodetic.z();						// height
					}		
					else if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
					{
						bool bInExtent = false;
						osgUtil::LineSegmentIntersector::Intersections::iterator itr = results.begin();
						while(itr != results.end())
						{
							osg::Vec3d wgscoord = itr->getWorldIntersectPoint();
							if ( (wgscoord.x()-mpr_west)*(wgscoord.x()-mpr_east) <= 0 && 
								(wgscoord.y()-mpr_south)*(wgscoord.y()-mpr_north) <= 0)
							{// 交点在地形有效区域
								lonOrx = wgscoord.x();
								latOry = wgscoord.y();
								altOrz = wgscoord.z();
								bInExtent = true;
								break;
							}
							itr++;
						}
						// 交点都不在有效区域
						if (bInExtent==false)
							return false;
					}
					return true;			 
				}
			}
			++itr;
		}
	}

	// 没有倾斜摄影图层或与倾斜摄影图层无交点时
	EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
	mpr_node->accept( iv );		
	LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);

	osgUtil::LineSegmentIntersector::Intersections& results = lsi->getIntersections();
	if (!results.empty() )
	{
		if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
			osg::Vec3d wgscoord = results.begin()->getWorldIntersectPoint();
			osg::Vec3d geodetic;
			g_ellipsmodel->convertXYZToLatLongHeight(
				wgscoord.x(),     wgscoord.y(),     wgscoord.z(),
				geodetic.y(), geodetic.x(), geodetic.z() );		
			lonOrx = osg::RadiansToDegrees(geodetic.x());// longitude
			latOry = osg::RadiansToDegrees(geodetic.y());// latitude
			altOrz = geodetic.z();						// height
		}		
		else if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
		{
			bool bInExtent = false;
			osgUtil::LineSegmentIntersector::Intersections::iterator itr = results.begin();
			while(itr != results.end())
			{
				osg::Vec3d wgscoord = itr->getWorldIntersectPoint();
				if ( (wgscoord.x()-mpr_west)*(wgscoord.x()-mpr_east) <= 0 && 
					(wgscoord.y()-mpr_south)*(wgscoord.y()-mpr_north) <= 0)
				{// 交点在地形有效区域
					lonOrx = wgscoord.x();
					latOry = wgscoord.y();
					altOrz = wgscoord.z();
					bInExtent = true;
					break;
				}
				itr++;
			}
			// 交点都不在有效区域
			if (bInExtent==false)
				return false;
		}
		return true;			 
	}
	return false;
}

glbBool CGlbGlobeTerrain::IsRayInterTerrain(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos)
{
	if (mpr_visible==false)//地形隐藏，无交点
		return false;

	ln_dir.normalize();
	//double r = osg::WGS_84_RADIUS_EQUATOR;
	osg::Vec3d geostart = ln_pt1;
	osg::Vec3d geoend = ln_pt1 + ln_dir*ln_pt1.length();
	if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
	{
		osg::Vec3d _center ((mpr_west+mpr_east)*0.5,(mpr_south+mpr_north)*0.5,0);
		double dis = (ln_pt1-_center).length();
		geoend = ln_pt1 + ln_dir*dis*5;
	}

	osgUtil::LineSegmentIntersector* lsi = new osgUtil::LineSegmentIntersector(geostart, geoend);
	osgUtil::IntersectionVisitor iv( lsi );
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
		lsi->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
	}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		lsi->setIntersectionLimit(osgUtil::Intersector::NO_LIMIT);
	}

	EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	
	mpr_node->accept( iv );		
	LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	

	osgUtil::LineSegmentIntersector::Intersections& results = lsi->getIntersections();
	if (!results.empty() )
	{
		InterPos = results.begin()->getWorldIntersectPoint();
		if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
		{
			if ( (InterPos.x()-mpr_west)*(InterPos.x()-mpr_east) > 0 || 
				(InterPos.y()-mpr_south)*(InterPos.y()-mpr_north) > 0)
			{// 交点不在地形有效区域
				return false;
			}
		}
		osg::Vec3d dir = InterPos - ln_pt1;
		dir.normalize();
		if (dir * ln_dir > 0)//同向	
			return true;			 
	}
	return false;
}

HRESULT CGlbGlobeTerrain::UpdateOsgCallBack(osg::Node* node,osg::NodeVisitor* nv)
{
	osg::Group* _p_terrain_node = dynamic_cast<osg::Group*>(node);
	if (_p_terrain_node == NULL)
		return E_FAIL;	

	osg::Timer_t startFrameTick = osg::Timer::instance()->tick();	

	size_t tilescnt = mpr_operations_buffer.size();
	if (tilescnt > 0)
	{			
		//CGlbSysLog::WriteLog(L"CGlbGlobeTerrain::UpdateRendererNode() lock pre.\n");		
		//double t = nv->getFrameStamp()->getSimulationTime();
		double timeUseToCompile = 0.0;
		EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);

		std::vector<GlbTileOperation>::iterator _itr = mpr_operations_buffer.begin();
		while(_itr!=mpr_operations_buffer.end())
		{	
			if (_itr->_addordelormodify==GLB_OPENERAION_ADD)//add
			{
				if (_itr->_nodeordrawableorvecarrayortexture==GLB_NODE)//node
				{				
					if (_itr->_p_tile_node && _p_terrain_node->containsNode(_itr->_p_tile_node.get())==false)
					{
						timeUseToCompile += _itr->_node_compiletime;
						_p_terrain_node->addChild(_itr->_p_tile_node);
					}
				}
				else if (_itr->_nodeordrawableorvecarrayortexture==GLB_DRAWABLE)//drawable
				{
					if (_itr->_p_tile_geode_node && _itr->_p_tile_geode_node->containsDrawable(_itr->_p_drawable.get())==false)
						_itr->_p_tile_geode_node->addDrawable(_itr->_p_drawable);							
				}
			}
			else if (_itr->_addordelormodify==GLB_OPENERAION_REMOVE)//delete
			{
				if (_itr->_nodeordrawableorvecarrayortexture==GLB_NODE)//node
				{
					if (_itr->_p_tile_node)
						_p_terrain_node->removeChild(_itr->_p_tile_node);
				}
				else if (_itr->_nodeordrawableorvecarrayortexture==GLB_DRAWABLE)//drawable
				{
					if (_itr->_p_tile_geode_node)
						_itr->_p_tile_geode_node->removeDrawable(_itr->_p_drawable);															
				}
			}
			else if (_itr->_addordelormodify==GLB_OPENERAION_MODIFY)//modify
			{
				if (_itr->_nodeordrawableorvecarrayortexture==GLB_VECARRAY)//vecarray
				{
					if (_itr->_p_drawable && _itr->_nodeordrawableorvecarrayortexture)
					{
						osg::Geometry* pGeo = dynamic_cast<osg::Geometry*>(_itr->_p_drawable.get());
						if (pGeo)
						{
							pGeo->setVertexArray(_itr->_p_vecarray);
							pGeo->dirtyDisplayList();
						}
					}
				}
				else if (_itr->_nodeordrawableorvecarrayortexture==GLB_TEXTURE)//texture
				{
					if (_itr->_p_texture && _itr->_p_image)
					{
						(_itr->_p_texture)->setImage(0,_itr->_p_image);		
						(_itr->_p_texture)->dirtyTextureObject();
					}
				}
			}
			_itr++;		
		}
		mpr_operations_buffer.clear();

		LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);

		// 输出需要添加的节点数量, 看是否一次加入很多节点，导致了帧冲击 2016.9.14
		//#ifdef _DEBUG
		//WCHAR wBuff[128];
		//swprintf(wBuff, L"add tile count: %d , use time %lf.\n",int(timeUseToCompile/0.002), timeUseToCompile);
		//OutputDebugString(wBuff);
		//#endif

		CGlbGlobeMemCtrl::GetInstance()->IncrementTimeToCompileCost(timeUseToCompile);
	}

	// 处理需要打包处理的地形修改操作
	tilescnt = mpr_packageOperations_buffer.size();
	if (tilescnt > 0)
	{
		EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);

		std::vector<GlbTileOperation>::iterator _itr = mpr_packageOperations_buffer.begin();
		while(_itr!=mpr_packageOperations_buffer.end())
		{	
			if (_itr->_addordelormodify==GLB_OPENERAION_ADD)//add
			{
				if (_itr->_nodeordrawableorvecarrayortexture==GLB_NODE)//node
				{				
					if (_itr->_p_tile_node && _p_terrain_node->containsNode(_itr->_p_tile_node.get())==false)
					{
						//timeUseToCompile += _itr->_node_compiletime;
						_p_terrain_node->addChild(_itr->_p_tile_node);
					}
				}
				else if (_itr->_nodeordrawableorvecarrayortexture==GLB_DRAWABLE)//drawable
				{
					if (_itr->_p_tile_geode_node && _itr->_p_tile_geode_node->containsDrawable(_itr->_p_drawable.get())==false)
						_itr->_p_tile_geode_node->addDrawable(_itr->_p_drawable);							
				}
			}
			else if (_itr->_addordelormodify==GLB_OPENERAION_REMOVE)//delete
			{
				if (_itr->_nodeordrawableorvecarrayortexture==GLB_NODE)//node
				{
					if (_itr->_p_tile_node)
						_p_terrain_node->removeChild(_itr->_p_tile_node);
				}
				else if (_itr->_nodeordrawableorvecarrayortexture==GLB_DRAWABLE)//drawable
				{
					if (_itr->_p_tile_geode_node)
						_itr->_p_tile_geode_node->removeDrawable(_itr->_p_drawable);															
				}
			}
			else if (_itr->_addordelormodify==GLB_OPENERAION_MODIFY)//modify
			{
				if (_itr->_nodeordrawableorvecarrayortexture==GLB_VECARRAY)//vecarray
				{
					if (_itr->_p_drawable && _itr->_nodeordrawableorvecarrayortexture)
					{
						osg::Geometry* pGeo = dynamic_cast<osg::Geometry*>(_itr->_p_drawable.get());
						if (pGeo)
						{
							pGeo->setVertexArray(_itr->_p_vecarray);
							pGeo->dirtyDisplayList();
						}
					}
				}
				else if (_itr->_nodeordrawableorvecarrayortexture==GLB_TEXTURE)//texture
				{
					if (_itr->_p_texture && _itr->_p_image)
					{
						(_itr->_p_texture)->setImage(0,_itr->_p_image);		
						(_itr->_p_texture)->dirtyTextureObject();
					}
				}
			}
			_itr++;
		}
		mpr_packageOperations_buffer.clear();

		LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
	}

	osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
	double frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);

	return S_OK;
}
void CGlbGlobeTerrain::RefreshTexture(	std::vector<CGlbExtent>                    &objDirtyExts,
										std::vector<CGlbExtent>                    &domDirtyExts,
										std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,                   
										std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainObjs )
{

	std::vector<glbref_ptr<CGlbGlobeRObject>> digholeObjs;
	// 倾斜摄影图层更新
	if (mpr_tilt_photography_layers.size()>0)
	{
		std::vector<glbref_ptr<CGlbGlobeLayer>>::iterator itr = mpr_tilt_photography_layers.begin();
		while(itr != mpr_tilt_photography_layers.end())
		{
			CGlbGlobeTiltPhotographyLayer* tiltLayer = dynamic_cast<CGlbGlobeTiltPhotographyLayer*>((*itr).get());
			if (tiltLayer && tiltLayer->IsShow())
			{
				tiltLayer->RefreshTexture(objDirtyExts,domDirtyExts,domlayers,terrainObjs);
				CGlbGlobeRObject* obj = dynamic_cast<CGlbGlobeRObject*>(tiltLayer->GetAssociateObject());
				if (obj)
				{
					digholeObjs.push_back(obj);
				}					
			}
			++itr;
		}
	}
	if (digholeObjs.size()>0)
	{// 添加内部挖洞对象
		for(size_t k = 0; k < digholeObjs.size(); k++)
		{
			terrainObjs.push_back(digholeObjs.at(k));
		}		
	}

GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
#if 0
	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
	while(_itr !=mpr_topmost_tiles.end())
	{		
		std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
		std::vector<CGlbExtent> childDirtyExts;
		_itr->second->FilterOnTerrainObjects(terrainObjs,childtobjs);
		_itr->second->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
		_itr->second->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,terrainObjs);
		
		_itr++;
	}	
#else // 优化刷新过程,先刷新显示的tile
	// 第一次遍历 ： 只处理那些node在场景树上的tile
	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
	while(_itr !=mpr_topmost_tiles.end())
	{		
		std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
		std::vector<CGlbExtent> childDirtyExts;
		_itr->second->FilterOnTerrainObjects(terrainObjs,childtobjs);
		_itr->second->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
		_itr->second->RefreshTexture_Step1(childDirtyExts,domDirtyExts,domlayers,terrainObjs);

		_itr++;
	}	
	// 第二次遍历 ： 处理那些node不在场景树上的tile
	_itr = mpr_topmost_tiles.begin();
	while(_itr !=mpr_topmost_tiles.end())
	{		
		std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
		std::vector<CGlbExtent> childDirtyExts;
		_itr->second->FilterOnTerrainObjects(terrainObjs,childtobjs);
		_itr->second->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
		_itr->second->RefreshTexture_Step2(childDirtyExts,domDirtyExts,domlayers,terrainObjs);

		_itr++;
	}
#endif
}
void CGlbGlobeTerrain::RefreshDem(  std::vector<CGlbExtent>                      &mtobjDirtyExts,
									std::vector<CGlbExtent>                      &demDirtyExts,
									std::vector<glbref_ptr<CGlbGlobeDemLayer>>   &demlayers,                   
									std::vector<glbref_ptr<CGlbGlobeRObject>>    &mterrainObjs )
{	
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);		
	//CGlbGlobeTerrain::GlbTileOperationPackage operationPackage;

	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
	while(_itr !=mpr_topmost_tiles.end())
	{
		CGlbExtent* tile_extent = _itr->second->GetExtent();
		glbBool isInter = IsIntersect(tile_extent,mtobjDirtyExts);
		if (!isInter)
		{
			isInter = IsIntersect(tile_extent,demDirtyExts);
		}

		if (isInter)
			_itr->second->RefreshDem(mtobjDirtyExts,demDirtyExts,demlayers,mterrainObjs);		
		_itr++;
	}

	// 重新更新边界顶点
	_itr = mpr_topmost_tiles.begin();
	while(_itr !=mpr_topmost_tiles.end())
	{
		CGlbExtent* tile_extent = _itr->second->GetExtent();
		glbBool isInter = IsIntersect(tile_extent,mtobjDirtyExts);
		if (!isInter)
		{
			isInter = IsIntersect(tile_extent,demDirtyExts);
		}

		if (isInter)
			_itr->second->UpdateEdgeGeometryVertexArray();		
		_itr++;
	}

	if (mpr_packageOperations.size()>0)
	{
		EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	
		mpr_packageOperations_buffer = mpr_packageOperations;		
		LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	
		mpr_packageOperations.clear();			
	}
//LeaveCriticalSection(&mpr_critical);

}

glbBool CGlbGlobeTerrain::SetOpacity(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,                   
										std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainObjs,
										glbInt32 opacity )
{
	if (mpr_opacity==opacity)
		return true;

GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
	mpr_opacity = opacity;
#if 1
	// 这种方式存在缺陷： 贴地绘制的矢量的透明度会被地形透明度影响，即当地形半透明时，贴地绘制的矢量也将被强制半透明
	mpr_bOpacityChanged = true;
#else
	// 倾斜摄影图层要通知它更新透明度
	std::vector<glbref_ptr<CGlbGlobeRObject>> digholeObjs;
	if (mpr_tilt_photography_layers.size()>0)
	{
		std::vector<glbref_ptr<CGlbGlobeLayer>>::iterator itr = mpr_tilt_photography_layers.begin();
		while(itr != mpr_tilt_photography_layers.end())
		{
			CGlbGlobeTiltPhotographyLayer* tiltLayer = dynamic_cast<CGlbGlobeTiltPhotographyLayer*>((*itr).get());
			if (tiltLayer && tiltLayer->IsShow())
			{
				tiltLayer->SetOpacity(opacity);
				CGlbGlobeRObject* obj = dynamic_cast<CGlbGlobeRObject*>(tiltLayer->GetAssociateObject());
				if (obj)
				{
					digholeObjs.push_back(obj);
				}			
			}
			++itr;
		}
	}
	if (digholeObjs.size()>0)
	{// 添加内部挖洞对象
		for(size_t k = 0; k < digholeObjs.size(); k++)		
			terrainObjs.push_back(digholeObjs.at(k));				
	}
	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
	while(_itr !=mpr_topmost_tiles.end())
	{
		std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
		_itr->second->FilterOnTerrainObjects(terrainObjs,childtobjs);		
		_itr->second->SetOpacity(domlayers,childtobjs,opacity);
		++_itr;
	}
#endif
	//OutputDebugString(L"SetOpacity begin .... \n");
	// 2015.10.13 不需要迭代实现，直接从混合方程可以实现
	osg::StateSet* ss = mpr_node->getOrCreateStateSet();
	osg::ref_ptr<osg::BlendFunc> pBlendFun = NULL;
	osg::ref_ptr<osg::Depth> depth = NULL;
	if (mpr_opacity<100)
	{// 透明			
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);	
		ss->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
		pBlendFun = dynamic_cast<osg::BlendFunc*>(ss->getAttribute(osg::StateAttribute::BLENDFUNC));
		if(!pBlendFun.valid())
		{
			pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
			ss->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}
		if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
		depth = dynamic_cast<osg::Depth *>(ss->getAttribute(osg::StateAttribute::DEPTH));
		if(!depth.valid())
		{
			depth = new osg::Depth;
			depth->setWriteMask(false);
			ss->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
		}
		else
		{
			if(depth->getWriteMask())
				depth->setWriteMask(false);
		}
	}
	else
	{
		ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
		if (osg::StateSet::OPAQUE_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );	
		depth = dynamic_cast<osg::Depth *>(ss->getAttribute(osg::StateAttribute::DEPTH));
		if(!depth.valid())
		{
			depth = new osg::Depth;
			depth->setWriteMask(true);
			ss->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
		}
		else
		{
			if(!depth->getWriteMask())
				depth->setWriteMask(true);
		}
	}
	//OutputDebugString(L"	SetOpacity end .... \n");
	// // 地形块要求最后绘制【所以渲染顺序要比默认值大1 透明绘制为10，不透明度的默认值为0】2015.5.6 
	// // 以下代码移到UpdateOsgCallback()中以避免状态频繁改变导致osg崩溃 2015.11.5
	//osg::StateSet* ss = mpr_node->getOrCreateStateSet();
	//if (mpr_opacity<100)
	//{// 透明			
	//	ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
	//	if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
	//		ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );	//ss->setRenderBinDetails(11,"DepthSortedBin"); 

	//	osg::BlendColor* bc = dynamic_cast<osg::BlendColor*>(ss->getAttribute(osg::StateAttribute::BLENDCOLOR));		
	//	if (!bc)
	//	{
	//		bc=new osg::BlendColor(osg::Vec4(1.0f,1.0f,1.0f,(float)opacity/ 100.0));  
	//		ss->setAttributeAndModes(bc,osg::StateAttribute::ON);
	//	}
	//	else
	//	{
	//		bc->setConstantColor(osg::Vec4(1.0f,1.0f,1.0f,(float)opacity/ 100.0));
	//	}
	//	osg::BlendFunc* bf = dynamic_cast<osg::BlendFunc*>(ss->getAttribute(osg::StateAttribute::BLENDFUNC));
	//	if (!bf)
	//	{
	//		bf = new osg::BlendFunc();						
	//		bf->setFunction(osg::BlendFunc::CONSTANT_ALPHA,osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA,osg::BlendFunc::CONSTANT_ALPHA,osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
	//		ss->setAttributeAndModes(bf,osg::StateAttribute::ON);
	//	}		
	//}
	//else
	//{		
	//		
	//	osg::BlendColor* bc = dynamic_cast<osg::BlendColor*>(ss->getAttribute(osg::StateAttribute::BLENDCOLOR));
	//	if (bc)
	//		ss->removeAttribute(bc);
	//	osg::BlendFunc* bf = dynamic_cast<osg::BlendFunc*>(ss->getAttribute(osg::StateAttribute::BLENDFUNC));
	//	if (bf)
	//		ss->removeAttribute(bf);
	//	
	//	ss->setMode(GL_BLEND, osg::StateAttribute::OFF );
	//	if (osg::StateSet::OPAQUE_BIN != ss->getRenderingHint())
	//		ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		//ss->setRenderBinDetails( 1, "RenderBin");
	//}

	return true;
}
glbInt32 CGlbGlobeTerrain::GetOpacity()
{
	return mpr_opacity;
}
glbBool CGlbGlobeTerrain::SetExaggrate(glbDouble exaggrate)
{// 思路：修改每个地形块的matrix矩阵的缩放系数，按地心点(0,0,0)与地形块中心点的矢量的比例进行 放缩
	if (mpr_exaggrate==exaggrate)
		return true;
	mpr_exaggrate = exaggrate;

// 临界区与地形更新互斥
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
	// 递归更新opacity
	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
	while(_itr !=mpr_topmost_tiles.end())
	{	
		_itr->second->SetExaggrate(exaggrate);		
		_itr++;
	}

	// 重新更新边界顶点
	_itr = mpr_topmost_tiles.begin();
	while(_itr !=mpr_topmost_tiles.end())
	{
		_itr->second->UpdateEdgeGeometryVertexArray();		
		_itr++;
	}
//LeaveCriticalSection(&mpr_critical);

	if (mpr_packageOperations.size()>0)
	{
		EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	
		mpr_packageOperations_buffer = mpr_packageOperations;		
		LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	
		mpr_packageOperations.clear();			
	}

	return true;
}
glbDouble CGlbGlobeTerrain::GetExaggrate()
{
	return mpr_exaggrate;
}
glbBool CGlbGlobeTerrain::SetVisible(glbBool visible)
{
	if (mpr_visible==visible)
		return true;
	mpr_visible = visible;
	if (mpr_node)
	{
		osg::Switch* pSW = dynamic_cast<osg::Switch*>(mpr_node.get());
		if (pSW)
		{
			if (mpr_visible)
				pSW->setAllChildrenOn();
			else
				pSW->setAllChildrenOff();
		}
	}
	return true;
}
glbBool CGlbGlobeTerrain::GetVisible()
{
	return mpr_visible;
}
osg::Node* CGlbGlobeTerrain::GetNode()
{
	return mpr_node.get();
}
glbDouble CGlbGlobeTerrain::ComputeOnTerrainDistance(glbDouble xOrLonStart, glbDouble yOrLatStart, glbDouble xOrLonEnd, glbDouble yOrLatEnd, glbDouble step)
{
	double dis = (xOrLonEnd-xOrLonStart)*(xOrLonEnd-xOrLonStart) + (yOrLatEnd-yOrLatStart)*(yOrLatEnd-yOrLatStart);
	int cnt = floor(dis / step);
	std::vector<osg::Vec3d> points;
	glbDouble zStart = GetElevationAt(xOrLonStart,yOrLatStart);
	points.push_back(osg::Vec3d(xOrLonStart,yOrLatStart,zStart));
	
	osg::Vec2d dir = osg::Vec2d(xOrLonEnd-xOrLonStart,yOrLatEnd-yOrLatEnd);
	dir.normalize();

	// 临界区与地形更新互斥
	EnterCriticalSection(&mpr_tDatasetAccessCritical);
	for (int k = 1; k < cnt; k++)
	{
		double lonOrX = xOrLonStart + dir.x()*step;
		double latOrY = yOrLatStart + dir.y()*step;

		glbDouble elevation = 0;
		glbBool res = GetElevationAtFromTiltPhotographLayer(lonOrX,latOrY,elevation);
		if (res==false)
		{// 不在倾斜摄影图层范围内
			if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
				glbInt32 curRow = GetRowFromLatitude(latOrY,mpr_lzts);
				glbInt32 curCol = GetColFromLongitude(lonOrX,mpr_lzts);

				glbInt32 latitude_cnt = 180.0 / mpr_lzts + 0.5;
				glbInt32 longitude_cnt = 360.0 / mpr_lzts + 0.5;

				glbInt32 key = (curRow * longitude_cnt) + curCol;

				//EnterCriticalSection(&mpr_update_criticalsection);
				std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.find(key);
				if (_itr != mpr_topmost_tiles.end())
				{		
					elevation = _itr->second->GetElevationAt(lonOrX,latOrY);
				}
				//LeaveCriticalSection(&mpr_update_criticalsection);
			}
			else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
				//EnterCriticalSection(&mpr_update_criticalsection);
				glbDouble east,west,south,north,minAlt,maxAlt;
				std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
				while(_itr !=mpr_topmost_tiles.end())
				{
					_itr->second->GetRange(east,west,south,north,minAlt,maxAlt);
					if ((lonOrX-east)*(lonOrX-west)<=0 && (latOrY-south)*(latOrY-north)<=0)
					{
						elevation = _itr->second->GetElevationAt(lonOrX,latOrY);
						break;
					}
					_itr++;
				}
				//LeaveCriticalSection(&mpr_update_criticalsection);
			}
		}	
		points.push_back(osg::Vec3d(lonOrX,latOrY,elevation));		
	}
	LeaveCriticalSection(&mpr_tDatasetAccessCritical);	
	glbDouble zEnd = GetElevationAt(xOrLonEnd,yOrLatEnd);		
	points.push_back(osg::Vec3d(xOrLonEnd,yOrLatEnd,zEnd));

	// 计算距离
	dis = 0;
	osg::Vec3d v1,v2;
	for (size_t k = 1; k < points.size(); k++)
	{
		v1 = points.at(k-1);
		v2 = points.at(k);
		if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v1.y()),osg::DegreesToRadians(v1.x()),v1.z(),v1.x(),v1.y(),v1.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v2.y()),osg::DegreesToRadians(v2.x()),v2.z(),v2.x(),v2.y(),v2.z());
		}
		
		double l = (v2 - v1).length();
		dis += l;
	}

	return dis;
}
glbDouble CGlbGlobeTerrain::GetElevationAt(glbDouble lonOrX, glbDouble latOrY)
{
// 临界区与地形更新互斥
EnterCriticalSection(&mpr_tDatasetAccessCritical);
	glbDouble elevation = 0;
	glbBool res = GetElevationAtFromTiltPhotographLayer(lonOrX,latOrY,elevation);
	if (res==false)
	{// 不在倾斜摄影图层范围内
		if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
			glbInt32 curRow = GetRowFromLatitude(latOrY,mpr_lzts);
			glbInt32 curCol = GetColFromLongitude(lonOrX,mpr_lzts);

			glbInt32 latitude_cnt = 180.0 / mpr_lzts + 0.5;
			glbInt32 longitude_cnt = 360.0 / mpr_lzts + 0.5;

			glbInt32 key = (curRow * longitude_cnt) + curCol;

			//EnterCriticalSection(&mpr_update_criticalsection);
			std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.find(key);
			if (_itr != mpr_topmost_tiles.end())
			{		
				elevation = _itr->second->GetElevationAt(lonOrX,latOrY);
			}
			//LeaveCriticalSection(&mpr_update_criticalsection);
		}
		else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
			//EnterCriticalSection(&mpr_update_criticalsection);
			glbDouble east,west,south,north,minAlt,maxAlt;
			std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
			while(_itr !=mpr_topmost_tiles.end())
			{
				_itr->second->GetRange(east,west,south,north,minAlt,maxAlt);
				if ((lonOrX-east)*(lonOrX-west)<=0 && (latOrY-south)*(latOrY-north)<=0)
				{
					elevation = _itr->second->GetElevationAt(lonOrX,latOrY);
					break;
				}
				_itr++;
			}
			//LeaveCriticalSection(&mpr_update_criticalsection);
		}
	}	
LeaveCriticalSection(&mpr_tDatasetAccessCritical);
	return elevation;
}
void CGlbGlobeTerrain::NotifyCameraIsUnderground(glbBool underground)
{
	if (mpr_is_camera_underground==underground)
		return;
	mpr_is_camera_underground = underground;

	// 计算相对地面高度
	osg::CullFace* pCF = dynamic_cast<osg::CullFace*>(mpr_node->getOrCreateStateSet()->getAttribute(osg::StateAttribute::CULLFACE));
	if (pCF && mpr_enablecullface)
	{
		if (pCF->getMode()==osg::CullFace::FRONT && !mpr_is_camera_underground){
			pCF->setMode(osg::CullFace::BACK);
		}
		if (pCF->getMode()==osg::CullFace::BACK && mpr_is_camera_underground){
			pCF->setMode(osg::CullFace::FRONT);
		}
	}
}
glbBool CGlbGlobeTerrain::GetExtent(glbDouble &west, glbDouble &east, glbDouble &south , glbDouble &north)
{
	west = mpr_west;
	east = mpr_east;
	south = mpr_south;
	north = mpr_north;
	return true;
}
glbBool CGlbGlobeTerrain::ComputeVisibleExtents(CGlbExtent& extent_one, CGlbExtent& extent_two, glbDouble min_southDegree, glbDouble max_norhtDegree)
{
	if (!mpr_globe || mpr_globe->GetView()==NULL)
		return false;
	//OutputDebugString(L"ComputeVisibleExtent begin. ............\n");
	/*
		经验证，此种递归算法得到的可见区域在GLB_FLAT模式下可以覆盖屏幕，基本是正确的。
		在GLB_GLOBE模式下还有问题。尤其是经度方向放大太多出现了 [-180,180] - 需要进一步测试
		优化方向：在CGlbGlobeTerrain::Update()过程中同时计算可见区域..... 未完待续 马林 2013.8.22
	*/
	//
// 临界区与地形更新互斥
EnterCriticalSection(&mpr_tDatasetAccessCritical);

	osg::Camera* p_osgcamera = mpr_globe->GetView()->GetOsgCamera();
	osg::Matrixd _modelView = p_osgcamera->getViewMatrix();
	osg::Matrixd _projection = p_osgcamera->getProjectionMatrix();
	osg::Polytope cv;
	cv.setToUnitFrustum();
	cv.transformProvidingInverse((_modelView)*(_projection));

	osg::BoundingBox visiblebox1;
	osg::BoundingBox visiblebox2;

	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		glbDouble west,east,south,north;
		west = 180;
		east = -180;
		south = 90;
		north = -90;
		glbDouble w,e,s,n,mz,Mz;

		std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
		while(_itr != mpr_topmost_tiles.end())
		{
			if (_itr->second)
			{			
				_itr->second->GetRange(e,w,s,n,mz,Mz);
				if (n <= min_southDegree || s >= max_norhtDegree)
				{
					_itr++;
					continue;
				}
				else
				{
					if (_itr->second->IsVisible())
					{
						south = min(south,s);
						north = max(north,n);
						osg::BoundingBox bb = (_itr->second)->GetVisibleExtent(cv,min_southDegree,max_norhtDegree);
						if (bb.valid()) 
						{// 合并可见区域
							//if (bb.yMax() > max_norhtDegree)
							//{
							//	bb.set(bb.xMin(),bb.yMin(),bb.zMin(),bb.xMax(),max_norhtDegree,bb.zMax());
							//}
							//if (bb.yMin() < min_southDegree)
							//{
							//	bb.set(bb.xMin(),min_southDegree,bb.zMin(),bb.xMax(),bb.yMax(),bb.zMax());
							//}
							//if (bb.yMin()>min_southDegree && bb.yMax()<max_norhtDegree)
							{// 南北极极圈之间
								if (e<0)
								{
									visiblebox1.expandBy(bb);
								}
								else
									visiblebox2.expandBy(bb);
							}
						}
					}	
				}			
			}					
			_itr++;
		}
	}
	else if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
	{
		std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
		while(_itr != mpr_topmost_tiles.end())
		{
			if (_itr->second && _itr->second->IsVisible())
			{		
				osg::BoundingBox bb = (_itr->second)->GetVisibleExtent(cv,min_southDegree,max_norhtDegree);
				if (bb.valid()) 
					visiblebox1.expandBy(bb);
			}
			_itr++;
		}
	}

	//#ifdef _DEBUG
	//		WCHAR wBuff[128];
	//		wsprintf(wBuff,L"visible tile count: %d .\n",visible_cnt);
	//		OutputDebugString(wBuff);
	//#endif

	glbBool  bres = false;
	if (visiblebox1.valid() && visiblebox2.valid())
	{
		if (visiblebox1.xMax()==visiblebox2.xMin())
		{
			osg::BoundingBox visiblebox;

			visiblebox.expandBy(visiblebox1);
			visiblebox.expandBy(visiblebox2);

			extent_one.Set(visiblebox.xMin(),visiblebox.xMax(),visiblebox.yMin(),visiblebox.yMax(),visiblebox.zMin(),visiblebox.zMax());
			bres = true; 
			//mpr_p_visible_extent->Set(visiblebox.xMin(),visiblebox.xMax(),visiblebox.yMin(),visiblebox.yMax(),visiblebox.zMin(),visiblebox.zMax());
		}
		else
		{
			extent_one.Set(visiblebox1.xMin(),visiblebox1.xMax(),visiblebox1.yMin(),visiblebox1.yMax(),visiblebox1.zMin(),visiblebox1.zMax());
			extent_two.Set(visiblebox2.xMin(),visiblebox2.xMax(),visiblebox2.yMin(),visiblebox2.yMax(),visiblebox2.zMin(),visiblebox2.zMax());
			bres = 2; 
			//mpr_p_visible_extent->Set(visiblebox1.xMin(),visiblebox1.xMax(),visiblebox1.yMin(),visiblebox1.yMax(),visiblebox1.zMin(),visiblebox1.zMax());
			//mpr_p_visible_extent2->Set(visiblebox2.xMin(),visiblebox2.xMax(),visiblebox2.yMin(),visiblebox2.yMax(),visiblebox2.zMin(),visiblebox2.zMax());				
		}			
	}
	else if (visiblebox1.valid())
	{
		extent_one.Set(visiblebox1.xMin(),visiblebox1.xMax(),visiblebox1.yMin(),visiblebox1.yMax(),visiblebox1.zMin(),visiblebox1.zMax());
		bres = true; 

		//mpr_p_visible_extent->Set(visiblebox1.xMin(),visiblebox1.xMax(),visiblebox1.yMin(),visiblebox1.yMax(),visiblebox1.zMin(),visiblebox1.zMax());
	}
	else if (visiblebox2.valid())
	{
		extent_one.Set(visiblebox2.xMin(),visiblebox2.xMax(),visiblebox2.yMin(),visiblebox2.yMax(),visiblebox2.zMin(),visiblebox2.zMax());
		bres = true; 

		//mpr_p_visible_extent->Set(visiblebox2.xMin(),visiblebox2.xMax(),visiblebox2.yMin(),visiblebox2.yMax(),visiblebox2.zMin(),visiblebox2.zMax());
	}	
//LeaveCriticalSection(&mpr_critical);	
LeaveCriticalSection(&mpr_tDatasetAccessCritical);
	return bres;
}
void CGlbGlobeTerrain::EnableCullFace(glbBool enablecull)
{
	mpr_enablecullface = enablecull;
}
void CGlbGlobeTerrain::EnableUpdate(glbBool enableupdate)
{
	mpr_enableUpdate = enableupdate;
}
glbBool CGlbGlobeTerrain::IsUpdateEnable()
{
	return mpr_enableUpdate;
}
CGlbGlobeView* CGlbGlobeTerrain::GetView()
{
	if (!mpr_globe)
		return NULL;
	return mpr_globe->GetView();
}
glbInt32 CGlbGlobeTerrain::GetLevelZeroTileSizeOfDegrees()
{
	return 36;
}
glbInt32 CGlbGlobeTerrain::GetRowFromLatitude(glbDouble latitude, glbDouble tileSize)
{
	if (latitude > 90.0)
		latitude = 90.0;
	if (latitude < -90.0)
		latitude = -90.0;
	//return (glbInt32)(fmod(abs(-90.0 - latitude),180)/tileSize);
	return (glbInt32)(fmod(abs(latitude - 90.0),180)/tileSize);
}
glbInt32 CGlbGlobeTerrain::GetColFromLongitude(glbDouble longitude, glbDouble tileSize)
{
	if (longitude > 180.0)
		longitude = - 360.0 + longitude;
	if (longitude < -180.0)
		longitude =  360.0 + longitude;
	return (glbInt32)(fmod(abs(-180.0 - longitude),360)/tileSize);
}

glbBool CGlbGlobeTerrain::IsIntersect(CGlbExtent* src_extent, std::vector<CGlbExtent>& target_extents)
{
	if (src_extent==NULL)return false;
	if (target_extents.size()<=0) return false;

	glbDouble src_minX,src_maxX,src_minY,src_maxY;
	src_extent->Get(&src_minX,&src_maxX,&src_minY,&src_maxY);
	glbDouble minX,maxX,minY,maxY;
	for(size_t i = 0; i < target_extents.size(); i++)
	{		
		target_extents.at(i).Get(&minX,&maxX,&minY,&maxY);
		glbDouble mx = max(minX,src_minX);
		glbDouble Mx = min(maxX,src_maxX);
		glbDouble my = max(minY,src_minY);
		glbDouble My = min(maxY,src_maxY);
		if ((mx <= Mx) && (my <= My))		
			return true;
	}
	return false;
}

void CGlbGlobeTerrain::UpdateGlobeTerrain(std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
						std::vector<glbref_ptr<CGlbGlobeDemLayer>> demlayers,
						std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs, 
						std::vector<glbref_ptr<CGlbGlobeRObject>> mTerrainobjs)
{
	if (mpr_operations_buffer.size() > 0)
	{// 上一帧结果尚未处理完毕，直接返回。
		return;
	}	

	//{// 使用最小时间间隔更新逻辑，避免帧冲击		
	//	osg::Timer_t timeTick = osg::Timer::instance()->tick();
	//	double deltatime = osg::Timer::instance()->delta_m(mpr_prebufferchange_time, timeTick);//毫秒（千分之一秒）
	//	double time_interval_fazhi = 500.0;//时间间隔阀值为500毫秒
	//	if (deltatime < time_interval_fazhi)
	//	{// 距离上一次更新小于time_interval_fazhi时，不需要更新
	//		return;
	//	}
	//}
	osg::Vec3d camerapos;
	glbDouble lat,lon;
	mpr_globe->GetView()->GetCameraPos(camerapos);
	//g_ellipsmodel->convertXYZToLatLongHeight(camerapos.x(),camerapos.y(),camerapos.z(),lat,lon,hei);
	
	glbDouble ViewRange;
	if(camerapos.z() >= osg::WGS_84_RADIUS_EQUATOR)
		ViewRange = osg::DegreesToRadians(180.0);
	else
		ViewRange = asin(camerapos.z() / osg::WGS_84_RADIUS_EQUATOR) * 2;

	lon =camerapos.x();
	lat = camerapos.y();
	if (mpr_lzts < 180)
	{
		glbDouble vrd = osg::RadiansToDegrees(ViewRange);
		glbDouble latitudeMax = lat + vrd;
		glbDouble latitudeMin = lat - vrd;
		glbDouble longitudeMax = lon + vrd;
		glbDouble longitudeMin = lon - vrd;
		if (latitudeMax < -90.0 || latitudeMin > 90.0 || longitudeMax < -180.0 || longitudeMin > 180.0)
			return;
	}

	if (ViewRange * 0.5f > osg::DegreesToRadians(3.5f * mpr_lzts))
	{// 就目前的最远距离，永远不会进入此逻辑	
		std::map<glbInt32,glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr=mpr_topmost_tiles.begin();
		while(_itr!=mpr_topmost_tiles.end())
		{
			if (_itr->second)
			{
				(_itr->second)->Dispose(false,false);
			}
			++_itr;
		}
		return;
	}	
	
	osg::Timer_t startFrameTick = osg::Timer::instance()->tick();	
	//if (true)	
	//{
	//	//GlbLogWOutput(GLB_LOGTYPE_INFO,L"**** UpdateGlobeTerrain begin.\n");	
	//	LOG("**** UpdateGlobeTerrain begin.");
	//}

	// 卸载不可见tile
	RemoveInvisibleTiles();

	//if(true)
	//{// 测试时间
	//	osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
	//	double frameTime = osg::Timer::instance()->delta_m(startFrameTick, endFrameTick);
	//	GlbLogWOutput(GLB_LOGTYPE_INFO,L"**** RemoveInvisibleTiles using time %lf\n",frameTime);
	//}

	//startFrameTick = osg::Timer::instance()->tick();

	//0级地形块集合mpr_lztiles， 按到相机距离排序后的地形块集合
	std::map<glbDouble,glbref_ptr<CGlbGlobeTerrainTile>> sorted_topmost_tiles;
	glbBool res = GetSortedTopmostTiles(mpr_topmost_tiles,camerapos,sorted_topmost_tiles);
	std::map<glbDouble,glbref_ptr<CGlbGlobeTerrainTile>>::iterator itr = sorted_topmost_tiles.begin();
	while(itr != sorted_topmost_tiles.end())
	{
		if (itr->second)
			itr->second->Update(domlayers,demlayers,terrainobjs,mTerrainobjs);	
		itr++;
	}

	//if(true)
	//{// 测试时间
	//	osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
	//	double frameTime = osg::Timer::instance()->delta_m(startFrameTick, endFrameTick);
	//	GlbLogWOutput(GLB_LOGTYPE_INFO,L"**** tiles Update using time %lf\n",frameTime);
	//}

	//startFrameTick = osg::Timer::instance()->tick();

	// 生成块间接缝边
	itr = sorted_topmost_tiles.begin();
	while(itr != sorted_topmost_tiles.end())
	{
		if (itr->second)
			itr->second->Merge();	
		itr++;
	}

	//if(true)
	//{// 测试时间
	//	osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
	//	double frameTime = osg::Timer::instance()->delta_m(startFrameTick, endFrameTick);
	//	GlbLogWOutput(GLB_LOGTYPE_INFO,L"**** tiles merge using time %lf\n",frameTime);
	//}
	
	//std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
	//glbInt32 _invisibleCnt = 0;
	//while(_itr != mpr_topmost_tiles.end())
	//{
	//	if (_itr->second && _itr->second->IsVisible())
	//	{			
	//		(_itr->second)->Update(domlayers,demlayers,terrainobjs,mTerrainobjs);							
	//	}					
	//	_itr++;
	//}
	//LeaveCriticalSection(&mpr_critical);

	//// 生成块间接边
	//_itr = mpr_topmost_tiles.begin();
	//while(_itr != mpr_topmost_tiles.end())
	//{
	//	if (_itr->second && _itr->second->IsVisible())
	//	{				
	//		(_itr->second)->Merge();			
	//	}			
	//	_itr++;
	//}//*/	

	// update 完毕 将mpr_modified_tiles_buffer0 拷贝到 mpr_modified_tiles_buffer1
	size_t tilescnt = mpr_operations.size();
	if (tilescnt > 0 )
	{		
		EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	
		mpr_operations_buffer = mpr_operations;
		//mpr_globe->mpr_TerrainNodeUpdate = true;
		LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
		//mpr_prebufferchange_time = osg::Timer::instance()->tick();	
		mpr_operations.clear();			
	}	

	//if (true)
	//{
	//	osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
	//	double frameTime = osg::Timer::instance()->delta_m(startFrameTick, endFrameTick);
	//	//GlbLogWOutput(GLB_LOGTYPE_INFO,L"**** UpdateGlobeTerrain end %lf.\n",frameTime);
	//	LOG2("**** UpdateGlobeTerrain end ,use time .",frameTime);
	//}
}
void CGlbGlobeTerrain::UpdateFlatTerrain(std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
										std::vector<glbref_ptr<CGlbGlobeDemLayer>> demlayers,
										std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs, 
										std::vector<glbref_ptr<CGlbGlobeRObject>> mTerrainobjs)
{
	if (mpr_operations_buffer.size() > 0)
	{// 上一帧结果尚未处理完毕，直接返回。
		return;
	}	

	// 优化
	if (mpr_opacity < 5) // 当透明度为0-4时,相当于地面是完全透明状态，此时地面不再更新，提高渲染速度
		return;

	osg::Timer_t startFrameTick = osg::Timer::instance()->tick();	
	// 1. 计算可见区域[用视锥体]
	//osg::ref_ptr<osg::Vec3Array> p_visibleregion = ComputeVisibleExtent();
	//if (p_visibleregion==NULL)
	//	return;
	// 采用视锥体精确计算可见 contains()

	//EnterCriticalSection(&mpr_update_criticalsection);
	// 顶级tile不释放
	RemoveInvisibleTiles();

	//osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
	//glbDouble frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
	//startFrameTick = osg::Timer::instance()->tick();	

	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
	while(_itr != mpr_topmost_tiles.end())
	{
		if (_itr->second)
		{			
			if (_itr->second->IsVisible())
			{				
				(_itr->second)->Update(domlayers,demlayers,terrainobjs,mTerrainobjs);
			}	
		}					
		_itr++;
	}
	//LeaveCriticalSection(&mpr_update_criticalsection);

	//endFrameTick = osg::Timer::instance()->tick();
	//glbDouble frameTime_1 = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
	//startFrameTick = osg::Timer::instance()->tick();	

	// 地形tile缝边
	_itr = mpr_topmost_tiles.begin();
	while(_itr != mpr_topmost_tiles.end())
	{
		if (_itr->second)
		{					
			(_itr->second)->Merge();					
		}					
		_itr++;
	}
	// end tile缝边

	//endFrameTick = osg::Timer::instance()->tick();
	//glbDouble frameTime_2 = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
	//startFrameTick = osg::Timer::instance()->tick();	
	// update 完毕 将mpr_modified_tiles_buffer0 拷贝到 mpr_modified_tiles_buffer1
	glbInt32 tilescnt = mpr_operations.size();
	if (tilescnt > 0 )
	{
		EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	
		mpr_operations_buffer = mpr_operations;		
		LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);		
		mpr_addOperationCount = 0;
		mpr_operations.clear();	
	}	
}

void CGlbGlobeTerrain::RemoveInvisibleTiles()
{
	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.begin();
	glbInt32 _invisibleCnt = 0;	
	while(_itr != mpr_topmost_tiles.end())
	{
		if (_itr->second)
		{
			if (_itr->second->IsVisible()==false)
			{
				//(_itr->second)->Dispose(FALSE,TRUE);
				(_itr->second)->Dispose(false,false);
				_invisibleCnt++;
			}
		}			
		_itr++;
	}
}

CGlbGlobeTerrainTile* CGlbGlobeTerrain::GetTerrainLevelZeroTile(glbInt32 key)
{
	std::map<glbInt32,glbref_ptr<CGlbGlobeTerrainTile>>::iterator itr_find = mpr_topmost_tiles.find(key);
	if (itr_find != mpr_topmost_tiles.end())
	{
		return (itr_find->second).get();		
	}
	return NULL;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrain::GetTopmostNorthNeibourTile(CGlbGlobeTerrainTile* p_tile)
{
	CGlbGlobeTerrainTile* ptopmost_tile=p_tile;
	while(ptopmost_tile->GetParentTile())
	{
		ptopmost_tile = ptopmost_tile->GetParentTile();
	}
	glbInt32 tile_row,tile_column;
	ptopmost_tile->GetPosition(tile_row,tile_column);
	glbInt32 northtile_row = tile_row + 1;
	glbInt32 northtile_column = tile_column;

	glbInt32 neibour_index = (tile_row + 1)*mpr_levelzero_tiles_columns + tile_column;
	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.find(neibour_index);
	if (_itr != mpr_topmost_tiles.end())
	{
		return (_itr->second).get();
	}
	return NULL;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrain::GetTopmostSouthNeibourTile(CGlbGlobeTerrainTile* p_tile)
{
	CGlbGlobeTerrainTile* ptopmost_tile=p_tile;
	while(ptopmost_tile->GetParentTile())
	{
		ptopmost_tile = ptopmost_tile->GetParentTile();
	}
	glbInt32 tile_row,tile_column;
	ptopmost_tile->GetPosition(tile_row,tile_column);
	glbInt32 southtile_row = tile_row - 1;
	glbInt32 southtile_column = tile_column;

	glbInt32 neibour_index = (tile_row - 1)*mpr_levelzero_tiles_columns + tile_column;
	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.find(neibour_index);
	if (_itr != mpr_topmost_tiles.end())
	{
		return (_itr->second).get();
	}
	return NULL;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrain::GetTopmostWestNeibourTile(CGlbGlobeTerrainTile* p_tile)
{
	CGlbGlobeTerrainTile* ptopmost_tile=p_tile;
	while(ptopmost_tile->GetParentTile())
	{
		ptopmost_tile = ptopmost_tile->GetParentTile();
	}
	glbInt32 tile_row,tile_column;
	ptopmost_tile->GetPosition(tile_row,tile_column);
	glbInt32 westtile_row = tile_row;
	glbInt32 westtile_column = tile_column - 1;

	glbInt32 neibour_index = tile_row*mpr_levelzero_tiles_columns + tile_column-1;
	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.find(neibour_index);
	if (_itr != mpr_topmost_tiles.end())
	{
		return (_itr->second).get();
	}
	return NULL;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrain::GetTopmostEastNeibourTile(CGlbGlobeTerrainTile* p_tile)
{
	CGlbGlobeTerrainTile* ptopmost_tile=p_tile;
	while(ptopmost_tile->GetParentTile())
	{
		ptopmost_tile = ptopmost_tile->GetParentTile();
	}
	glbInt32 tile_row,tile_column;
	ptopmost_tile->GetPosition(tile_row,tile_column);
	glbInt32 easttile_row = tile_row;
	glbInt32 easttile_column = tile_column + 1;

	glbInt32 neibour_index = tile_row*mpr_levelzero_tiles_columns + tile_column+1;
	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = mpr_topmost_tiles.find(neibour_index);
	if (_itr != mpr_topmost_tiles.end())
	{
		return (_itr->second).get();
	}
	return NULL;
}

void CGlbGlobeTerrain::EnableWireMode(glbBool wiremode)
{
	if (mpr_isLinemode==wiremode)
		return;

	mpr_isLinemode = wiremode;
	if (mpr_isLinemode)
	{// 线框模式绘制地形
		osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
		polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		mpr_node->getOrCreateStateSet()->setAttribute( polygonMode.get(), osg::StateAttribute::ON );
	}
	else
	{
		osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
		polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
		mpr_node->getOrCreateStateSet()->setAttribute( polygonMode.get(), osg::StateAttribute::ON );
		//mpr_p_terrian_node->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
	}	
}

glbBool CGlbGlobeTerrain::IsWireMode()
{
	return mpr_isLinemode;
}

glbBool CGlbGlobeTerrain::Load(xmlNodePtr node,const glbWChar* prjPath)
{
	xmlChar* szKey = NULL;
	IGlbDataSource* dsource = NULL;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"DatasourceID")))
	{		
		szKey = xmlNodeGetContent(node);
		glbInt32 dsid;
		sscanf((char*)szKey,"%d",&dsid);	
		xmlFree(szKey);
		CGlbDataSourceFactory* factory = CGlbDataSourceFactory::GetInstance();
		dsource = factory->GetDataSourceByID(dsid);
		node = node->next;
	}			

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"DatasetName")))
	{		
		szKey = xmlNodeGetContent(node);		
		if (dsource)
		{
			char* str = u2g((char*)szKey);
			CGlbString glbstr = str;
			CGlbWString datsetname = glbstr.ToWString();	
			free(str);
			IGlbDataset* dataset = dsource->GetDatasetByName(datsetname.c_str());
			IGlbTerrainDataset* tDataset = dynamic_cast<IGlbTerrainDataset*>(dataset);
			if (tDataset)
				mpr_globe->SetTerrainDataset(tDataset);
			else
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"Load TerrianDataset file error, Check the file path is right ? \r\n");
				GlbLogWOutput(GLB_LOGTYPE_ERR,(glbWChar*)datsetname.c_str());	
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"\r\n");	
			}
		}
		xmlFree(szKey);
		node = node->next;
	}			

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_opacity);	
		xmlFree(szKey);
		node = node->next;
	}			

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Exaggrate")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_exaggrate);
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Visible")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%c",&mpr_visible);	
		xmlFree(szKey);
		node = node->next;
	}	
	return true;
}

glbBool CGlbGlobeTerrain::Save(xmlNodePtr node,const glbWChar* prjPath)
{	
	IGlbDataSource *p_datasource = (IGlbDataSource *)mpr_tdataset->GetDataSource();
	glbInt32 dsid = p_datasource->GetID();
	
	char str[64];
	sprintf(str,"%d",dsid);
	xmlNewTextChild(node, NULL, BAD_CAST "DatasourceID", BAD_CAST str);
	
	const glbWChar* datasetname = mpr_tdataset->GetName();
	CGlbWString dsnameString = datasetname;
	char* szOut = g2u((char*)dsnameString.ToString().c_str());	
	xmlNewTextChild(node, NULL, BAD_CAST "DatasetName", BAD_CAST szOut);
	free(szOut);

	sprintf(str,"%d",mpr_opacity);
	xmlNewTextChild(node, NULL, BAD_CAST "Opacity", BAD_CAST str);
	sprintf(str,"%lf",mpr_exaggrate);
	xmlNewTextChild(node, NULL, BAD_CAST "Exaggrate", BAD_CAST str);
	sprintf(str,"%c",mpr_visible);
	xmlNewTextChild(node, NULL, BAD_CAST "Visible", BAD_CAST str);

	return true;
}

void CGlbGlobeTerrain::AddTiltPhotographLayer(CGlbGlobeLayer* layer)
{
// 共用mpr_osgcritical临界区
EnterCriticalSection(&mpr_osgcritical);
	CGlbGlobeTiltPhotographyLayer* tiltLayer = dynamic_cast<CGlbGlobeTiltPhotographyLayer*>(layer);
	if (tiltLayer)
	{
		mpr_tilt_photography_layers.push_back(layer);	
	}
LeaveCriticalSection(&mpr_osgcritical);
}

glbBool CGlbGlobeTerrain::RemoveTiltPhotographLayer(CGlbGlobeLayer* layer)
{
	glbBool res = false;
EnterCriticalSection(&mpr_osgcritical);
	std::vector<glbref_ptr<CGlbGlobeLayer>>::iterator itr = mpr_tilt_photography_layers.begin();
	while(itr != mpr_tilt_photography_layers.end())
	{
		if ((*itr)->GetId()==layer->GetId())
		{
			itr = mpr_tilt_photography_layers.erase(itr);
			res = true;
			break;
		}
		++itr;
	}
LeaveCriticalSection(&mpr_osgcritical);
	return res;
}

glbBool CGlbGlobeTerrain::GetSortedTopmostTiles(std::map<glbInt32,glbref_ptr<CGlbGlobeTerrainTile>> tiles, osg::Vec3d camerapos, std::map<glbDouble,glbref_ptr<CGlbGlobeTerrainTile>>& sorted_tiles)
{
	glbDouble east,west,south,north,minAlt,maxAlt;
	std::map<glbInt32, glbref_ptr<CGlbGlobeTerrainTile>>::iterator _itr = tiles.begin();
	glbInt32 _invisibleCnt = 0;
	while(_itr != tiles.end())
	{
		if (_itr->second && _itr->second->IsVisible())
		{			
			_itr->second->GetRange(east,west,south,north,minAlt,maxAlt);						

			glbDouble centerLon = (east + west)*0.5;
			glbDouble centerLat = (south + north)*0.5;
			glbDouble dist = sqrt((camerapos.x()-centerLon)*(camerapos.x()-centerLon) + (camerapos.y()-centerLat)*(camerapos.y()-centerLat));

			sorted_tiles[dist] = _itr->second;
			
 		}					
		_itr++;
	}

	return true;
}

glbBool CGlbGlobeTerrain::GetElevationAtFromTiltPhotographLayer(glbDouble lonOrX, glbDouble latOrY, glbDouble &elevtion)
{
	if (!mpr_tdataset)
		return false;
	glbBool res = false;
	CGlbExtent* bound = const_cast<CGlbExtent*>(mpr_tdataset->GetExtent());
	glbDouble Tminx,Tmaxx,Tminy,Tmaxy,Tminz,Tmaxz;
	bound->Get(&Tminx,&Tmaxx,&Tminy,&Tmaxy,&Tminz,&Tmaxz);	
	if (mpr_tilt_photography_layers.size()>0)
	{
		glbDouble minX,maxX,minY,maxY;
EnterCriticalSection(&mpr_osgcritical);
		std::vector<glbref_ptr<CGlbGlobeLayer>> tempLayers = mpr_tilt_photography_layers;
LeaveCriticalSection(&mpr_osgcritical);

		std::vector<glbref_ptr<CGlbGlobeLayer>>::iterator itr = tempLayers.begin();
		while(itr != tempLayers.end())
		{
			glbref_ptr<CGlbExtent> ext = (*itr)->GetBound(false);
			if (ext)
			{
				ext->Get(&minX,&maxX,&minY,&maxY);
				if (lonOrX>= minX && lonOrX <= maxX && latOrY >= minY && latOrY <= maxY)
				{// 计算求交
					osg::Vec3d geostart(lonOrX,latOrY,Tmaxz+bound->GetZLengh()*0.1);
					osg::Vec3d geoend(lonOrX,latOrY,Tminz-bound->GetZLengh()*0.1);
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
					{
						g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(latOrY),osg::DegreesToRadians(lonOrX),10000.0,geostart.x(),geostart.y(),geostart.z());
						g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(latOrY),osg::DegreesToRadians(lonOrX),-500.0,geoend.x(),geoend.y(),geoend.z());
					}					

					osgUtil::LineSegmentIntersector* lsi = new osgUtil::LineSegmentIntersector(geostart, geoend);
					osgUtil::IntersectionVisitor iv( lsi );
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
						lsi->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
					}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
						lsi->setIntersectionLimit(osgUtil::Intersector::NO_LIMIT);
					}

					EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
					(*itr)->GetOsgNode()->accept( iv );		
					LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	

					osgUtil::LineSegmentIntersector::Intersections& results = lsi->getIntersections();
					if (!results.empty() )
					{
						osg::Vec3d InterPos = results.begin()->getWorldIntersectPoint();
						double lon = InterPos.x();
						double lat = InterPos.y();
						double hei = InterPos.z();
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
						{
							g_ellipsmodel->convertXYZToLatLongHeight(InterPos.x(),InterPos.y(),InterPos.z(),lat,lon,hei);
							lon = osg::RadiansToDegrees(lon);
							lat = osg::RadiansToDegrees(lat);
						}
							
						if (lon >= minX && lon <= maxX && lat >= minY && lat <= maxY )
						{// 交点在图层范围内
							elevtion = hei;
							res = true;
							break;
						}						
					}
				}
			}			
			++itr;
		}

	}
	return res;
}