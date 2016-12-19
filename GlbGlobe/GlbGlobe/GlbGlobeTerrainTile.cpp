#include "StdAfx.h"
#include "GlbGlobeTerrainTile.h"
#include "GlbGlobeTerrain.h"
#include "GlbGlobeView.h"
#include "GlbGlobeTerrainTileRequestTask.h"
#include "GlbGlobeTerrainModify.h"
#include <osg/CullFace>
#include <osgText/Text>
#include <osgDB/WriteFile>
#include <osg/GraphicsCostEstimator>
#include <osgUtil/CullVisitor>
#include <sstream> 
#include "geos.h"

#define FAZHI 0.00000001

using namespace GlbGlobe;

CGlbGlobeTerrainTile::CGlbGlobeTerrainTileCullCallBack::CGlbGlobeTerrainTileCullCallBack(const osg::BoundingBox& bbox,glbInt32 level)
{
	mpr_bbox = bbox;
	mpr_level = level;
}

CGlbGlobeTerrainTile::CGlbGlobeTerrainTileCullCallBack::~CGlbGlobeTerrainTileCullCallBack()
{
	
}

void CGlbGlobeTerrainTile::CGlbGlobeTerrainTileCullCallBack::operator() (osg::Node* node,osg::NodeVisitor* nv)
{			
	osgUtil::CullVisitor* cullVisitor = static_cast<osgUtil::CullVisitor*>(nv);
	//osgUtil::RenderStage* renderStage = cullVisitor->getCurrentRenderStage();
	//const osg::Viewport* viewport = renderStage->getViewport();	
	bool bVisible = true;	
	{
		osg::Vec3 eye = cullVisitor->getEyePoint();
		double vdis = sqrt(eye.length2()-osg::WGS_84_RADIUS_EQUATOR*osg::WGS_84_RADIUS_EQUATOR);
		double dis = (eye-mpr_bbox.center()).length() - mpr_bbox.radius();
		if (dis > vdis)// 超出可见距离必定不可见
			bVisible = false;		
	}
	
	if (bVisible==false)			
		return;
	
	if (mpr_level>0)
	{
		const osg::Matrix& _modelView = *(cullVisitor->getModelViewMatrix());	
		const osg::Matrix& _projection = *(cullVisitor->getProjectionMatrix());
		osg::Polytope cv;
		cv.setToUnitFrustum();
		cv.transformProvidingInverse((_modelView)*(_projection));	
		// 判断视锥体是否包含至少部分boundbox
		if (cv.contains(mpr_bbox)==false)
			bVisible = false;
	}
	//if (mpr_p_terraintile && mpr_p_terraintile->IsVisible()==false)
	if (bVisible==false)			
		return;

	// must continue subgraph traversal.
	traverse(node,nv);		
}

CGlbGlobeTerrainTile::CGlbGlobeTerrainTile(CGlbGlobeTerrain* terrain,glbInt32 level,
					glbDouble south,glbDouble north,glbDouble west,glbDouble east,
					glbDouble minAlt,glbDouble maxAlt,CGlbGlobeTerrainTile* parent)
{
	mpr_extent = new CGlbExtent();
	mpr_extent->Set(west,east,south,north,minAlt,maxAlt);
	mpr_terrain = terrain;
	mpr_level = level;
	mpr_south = south;
	mpr_north = north;
	mpr_east = east;
	mpr_west = west;
	mpr_minalt = minAlt;
	mpr_maxalt = maxAlt;
	mpr_center_longitude = (mpr_east + mpr_west) * 0.5;
	mpr_center_latitude = (mpr_north + mpr_south) * 0.5;
	mpr_parent_tile = parent;
	mpr_image = NULL;
	mpr_overlayImage = NULL;
	mpr_origin_image = NULL;
	mpr_demdata = NULL;
	mpr_overlaydemdata = NULL;
	mpr_demsize = 0;
	mpr_isLoadingData = false;
	mpr_isinitialized = false;
	mpr_isDataLoaded = false;
	mpr_longitude_span = fabs(mpr_east - mpr_west) ;
	mpr_latitude_span = fabs(mpr_north - mpr_south) ;
	mpr_globe_type = mpr_terrain->mpr_globe_type;

	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		mpr_row = GetRowFromLatitude(/*south*/north, north - south);
		mpr_column = GetColFromLongitude(west, north - south);

		mpr_center_longitude = osg::DegreesToRadians((mpr_east + mpr_west) * 0.5) ;
		mpr_center_latitude = osg::DegreesToRadians((mpr_north + mpr_south) * 0.5) ;
	}	

	mpr_p_north_easthalf_geom = NULL;
	mpr_p_north_westhalf_geom = NULL;
	mpr_p_south_easthalf_geom = NULL;
	mpr_p_south_westhalf_geom = NULL;
	mpr_p_east_northhalf_geom = NULL;
	mpr_p_east_southhalf_geom = NULL;
	mpr_p_west_northhalf_geom = NULL;
	mpr_p_west_southhalf_geom = NULL;
	mpr_p_north_easthalf_vertexarray = NULL;
	mpr_p_north_westhalf_vertexarray = NULL;
	mpr_p_south_easthalf_vertexarray = NULL;
	mpr_p_south_westhalf_vertexarray = NULL;
	mpr_p_east_northhalf_vertexarray = NULL;
	mpr_p_east_southhalf_vertexarray = NULL;
	mpr_p_west_northhalf_vertexarray = NULL;
	mpr_p_west_southhalf_vertexarray = NULL;

	mpr_p_center_westhalf_vertexarray = NULL;
	mpr_p_center_easthalf_vertexarray = NULL;
	mpr_p_center_northhalf_vertexarray = NULL;
	mpr_p_center_southhalf_vertexarray = NULL;

	mpr_p_north_easthalf_texcoords = NULL;
	mpr_p_north_westhalf_texcoords = NULL;
	mpr_p_south_easthalf_texcoords = NULL;
	mpr_p_south_westhalf_texcoords = NULL;
	mpr_p_east_northhalf_texcoords = NULL;
	mpr_p_east_southhalf_texcoords = NULL;
	mpr_p_west_northhalf_texcoords = NULL;
	mpr_p_west_southhalf_texcoords = NULL;

	mpr_p_geode = NULL;
	mpr_p_northWestgeom = NULL;
	mpr_p_northEastgeom = NULL;
	mpr_p_southEastgeom = NULL;
	mpr_p_southWestgeom = NULL;

	mpr_terrainTask = NULL;

	ComputeBoundBox();

	mpr_opacity = -1;

	mpr_textureRefreshed = false;
}

CGlbGlobeTerrainTile::~CGlbGlobeTerrainTile(void)
{	
	mpr_extent = NULL;
	//mpr_parent_tile = NULL;
	//mpr_nwchild = NULL;
	//mpr_nechild = NULL;
	//mpr_swchild = NULL;
	//mpr_sechild = NULL;

	if (mpr_node && mpr_node->getCullCallback()!=NULL)
	{// 清除拣选回调
		mpr_node->setCullCallback(NULL);
	}
	//mpr_node = NULL;
	//mpr_p_geode = NULL;
	//mpr_p_osgImg = NULL;
	//mpr_texture = NULL;

	if (mpr_image)
		delete [] mpr_image;
	if (mpr_overlayImage)
		delete [] mpr_overlayImage;
	if (mpr_origin_image)
		delete [] mpr_origin_image;
	if (mpr_demdata)
		delete [] mpr_demdata;
	if (mpr_overlaydemdata)
		delete [] mpr_overlaydemdata;
}

glbBool CGlbGlobeTerrainTile::IsInitialized()
{
	return mpr_isinitialized;
}

void CGlbGlobeTerrainTile::Update(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
									std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
									std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs, 
									std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs)
{
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		//if (mpr_terrain->mpr_is_camera_underground)
		//	UpdateGlobe(domlayers,demlayers,terrainobjs,mTerrainobjs);
		//else
			UpdateGlobe_Pro(domlayers,demlayers,terrainobjs,mTerrainobjs);
	}
	else if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
		UpdateFlat(domlayers,demlayers,terrainobjs,mTerrainobjs);
}

void CGlbGlobeTerrainTile::UpdateGlobe(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
										std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
										std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs, 
										std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs)
{
	CGlbGlobeView* p_globeview = mpr_terrain->GetView();
	if (!p_globeview)
		return;

//#ifdef _DEBUG
//	WCHAR wBuff[128];
//	wsprintf(wBuff,L"	Tile UpdateGlobe begin. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
//	OutputDebugString(wBuff);
//#endif

	glbDouble tileSize = mpr_north - mpr_south;	
	glbFloat TileDrawDistance = 3.5f;
	glbFloat TileDrawSpread = 2.9f;

	osg::Vec3d camerpos;
	p_globeview->GetCameraPos(camerpos);
	glbDouble altitude = camerpos.z();

	// 笛卡尔转经纬度(弧度)
	glbDouble neareast_latitude,nearest_longitude;
	osg::Vec3d focuspos;
	p_globeview->GetFocusPos(focuspos);	
	nearest_longitude = osg::DegreesToRadians(focuspos.x());
	neareast_latitude = osg::DegreesToRadians(focuspos.y());

	//osg::Vec3d nearestpos = mpr_terrain->mpr_nearestpos;
	//nearest_longitude = nearestpos.x();//osg::DegreesToRadians(nearestpos.x());
	//neareast_latitude = nearestpos.y();//osg::DegreesToRadians(nearestpos.y());	 

	glbDouble ViewRange;
	//if(_distance >= osg::WGS_84_RADIUS_EQUATOR)//时间：2013.2.2，改distance为altitude，目前看效果无太大变化
	if(altitude >= osg::WGS_84_RADIUS_EQUATOR)
		ViewRange = osg::DegreesToRadians(180.0);
	else
		//ViewRange = asin(_distance / osg::WGS_84_RADIUS_EQUATOR) * 2;
		ViewRange = asin(altitude / osg::WGS_84_RADIUS_EQUATOR) * 1.5;//2;
	glbBool flag1 = false;
	glbBool flag2 = false;
	glbDouble K = 0.85; //0.73
	glbDouble sizeScale = 0.75;//0.825;
	glbDouble spericalDis = SphericalDistance(mpr_center_latitude, mpr_center_longitude, neareast_latitude, nearest_longitude);
	{				
		if(spericalDis  < osg::DegreesToRadians(TileDrawSpread * tileSize * sizeScale))
			flag1 = true;
		if(spericalDis  < osg::DegreesToRadians(TileDrawSpread * tileSize * K * sizeScale))
			flag2 = true;
	}
	//规律 : *2可使正视情况下最大等级由20级增加到21级
	// [tileRadians控制最大分级数，越大则分级越多，2倍多分一级....依次类推]
	glbDouble tileRadians = osg::DegreesToRadians(TileDrawDistance * tileSize ) * 2; 
	glbBool _bVisible = IsVisible();
	if (!mpr_isinitialized)
	{		
		if (ViewRange * 0.5f < tileRadians && flag1 && _bVisible)
		{// 加载本块数据!!!! 纹理和实际dem高程数据
			Initialize(domlayers,demlayers,terrainobjs,mTerrainobjs);			
		}
	}
	if (mpr_isinitialized)
	{
		if (ViewRange < tileRadians	&& flag2 && _bVisible)
		{
			if (mpr_nechild == NULL || mpr_nwchild == NULL || 
				mpr_sechild == NULL || mpr_swchild == NULL)
			{
				ComputeChildren();
			}
			if (mpr_nechild != NULL)
			{
				mpr_nechild->Update(domlayers,demlayers,terrainobjs,mTerrainobjs);		
				if ( mpr_nechild->IsInitialized() )
				{// remove drawable
					if (mpr_p_geode->containsDrawable(mpr_p_northEastgeom.get()))
					{
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northEastgeom.get());
						mpr_terrain->AddOperation(_opera);								
					}
				}
			}

			if (mpr_nwchild != NULL)
			{
				mpr_nwchild->Update(domlayers,demlayers,terrainobjs,mTerrainobjs);
				if (mpr_nwchild->IsInitialized())
				{// remove drawable
					if (mpr_p_geode->containsDrawable(mpr_p_northWestgeom.get()))
					{
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northWestgeom.get());
						mpr_terrain->AddOperation(_opera);								
					}
				}
			}

			if (mpr_sechild != NULL)
			{
				mpr_sechild->Update(domlayers,demlayers,terrainobjs,mTerrainobjs);		
				if (mpr_sechild->IsInitialized())
				{// remove drawable
					if (mpr_p_geode->containsDrawable(mpr_p_southEastgeom.get()))
					{
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southEastgeom.get());
						mpr_terrain->AddOperation(_opera);			
					}
				}
			}

			if (mpr_swchild != NULL)
			{
				mpr_swchild->Update(domlayers,demlayers,terrainobjs,mTerrainobjs);
				if (mpr_swchild->IsInitialized())
				{// remove drawable
					if (mpr_p_geode->containsDrawable(mpr_p_southWestgeom.get()))
					{	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southWestgeom.get());
						mpr_terrain->AddOperation(_opera);		
					}
				}
			}
		}
		else
		{
			if (mpr_nwchild != NULL && mpr_nwchild->IsInitialized())
			{						
				mpr_nwchild->Dispose();		
				//delete mpr_p_northwest_childtile;
				mpr_nwchild = NULL;
				if (mpr_p_geode->containsDrawable(mpr_p_northWestgeom.get())==false)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northWestgeom.get());
					mpr_terrain->AddOperation(_opera);											
				}									
			}

			if (mpr_nechild != NULL && mpr_nechild->IsInitialized())
			{					
				mpr_nechild->Dispose();
				//delete mpr_p_northeast_childtile;
				mpr_nechild = NULL;
				if (mpr_p_geode->containsDrawable(mpr_p_northEastgeom.get())==false)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northEastgeom.get());
					mpr_terrain->AddOperation(_opera);											
				}	
			}

			if (mpr_sechild != NULL  && mpr_sechild->IsInitialized())
			{								
				mpr_sechild->Dispose();
				//delete mpr_p_southeast_childtile;
				mpr_sechild = NULL;
				if (mpr_p_geode->containsDrawable(mpr_p_southEastgeom.get())==false)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southEastgeom.get());
					mpr_terrain->AddOperation(_opera);	
				}
			}

			if (mpr_swchild != NULL  && mpr_swchild->IsInitialized())
			{		
				mpr_swchild->Dispose();	
				//delete mpr_p_southwest_childtile;
				mpr_swchild = NULL;
				if (mpr_p_geode->containsDrawable(mpr_p_southWestgeom.get())==false)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southWestgeom.get());
					mpr_terrain->AddOperation(_opera);	
				}					
			}
		}
	}
//#ifdef _DEBUG	
//	wsprintf(wBuff,L"	Tile UpdateGlobe end. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
//	OutputDebugString(wBuff);
//#endif
}

void CGlbGlobeTerrainTile::UpdateGlobe_Pro(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
											std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
											std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs, 
											std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs)
{
	//char buff[256];
	//if (true)
	//{	
	//	//GlbLogWOutput(GLB_LOGTYPE_INFO,L"**Tile Update begin. [Level: %d , row: %d, col: %d]\r\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//	sprintf(buff,"**Tile Update begin. [Level: %d , row: %d, col: %d].",mpr_level,(long)mpr_row,(long)mpr_column);
	//	LOG(buff);
	//}
	//WCHAR wBuff[128];
	//wsprintf(wBuff,L"Tile Update begin. [Level: %d , row: %d, col: %d]\r\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//GlbLogWOutput(GLB_LOGTYPE_INFO,wBuff);

	//if (mpr_level==0 && mpr_p_geode)
	//{
	//	mpr_p_geode->setNodeMask(0x1);
	//}

	glbBool _bVisible = IsVisible();

	if (!mpr_isinitialized)
	{	
		if (_bVisible){			
			// 加载本块数据!!!! 纹理和实际dem高程数据
			Initialize(domlayers,demlayers,terrainobjs,mTerrainobjs);		
		}
	}
	if(mpr_isinitialized)
	{
		glbBool _bSplit = IsTileSplit();		
		//if (false)
		//{
		//	sprintf(buff,"***Tile UpdateGlobe_Pro. [Level: %d , row: %d, col: %d] [w: %lf,e: %lf,s: %lf,n: %lf]isVisible:%d isSplit:%d.",mpr_level,(long)mpr_row,(long)mpr_column,mpr_west,mpr_east,mpr_south,mpr_north,_bVisible,_bSplit);
		//	LOG(buff);
		//}

		if (_bVisible && _bSplit)
		{// 分裂
			if (mpr_nechild == NULL || mpr_nwchild == NULL || 
				mpr_sechild == NULL || mpr_swchild == NULL)
			{
				ComputeChildren();
			}
			// 根据距离相机由近到远距离排序四个子块
			std::map<glbDouble,glbref_ptr<CGlbGlobeTerrainTile>> sorted_tiles;
			GetSortedChildTiles(sorted_tiles);
			// end 排序
			std::map<glbDouble,glbref_ptr<CGlbGlobeTerrainTile>>::iterator itr = sorted_tiles.begin();
			while(itr!=sorted_tiles.end())
			{
				if (itr->second)
				{
					/*
						贴地形对象，只检索子块对相应范围的对象往下传递！！！！
					*/
					std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
					itr->second->FilterOnTerrainObjects(terrainobjs,childtobjs);

					itr->second->Update(domlayers,demlayers,childtobjs,mTerrainobjs);		
					if (itr->second->IsInitialized() )
					{// remove drawable
						GlbGlobeChildLocationEnum locationEnum = itr->second->GetTileLocation();
						switch (locationEnum)
						{
						case GLB_NORTHEAST:
							if (mpr_p_northEastgeom->getNumParents()>0 /*&& mpr_p_geode->containsDrawable(mpr_p_northEastgeom.get())*/)
							{
								CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northEastgeom.get());
								mpr_terrain->AddOperation(_opera);								
							}
							break;
						case GLB_NORTHWEST:
							if (mpr_p_northWestgeom->getNumParents()>0 /*&& mpr_p_geode->containsDrawable(mpr_p_northWestgeom.get())*/)
							{
								CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northWestgeom.get());
								mpr_terrain->AddOperation(_opera);								
							}
							break;
						case GLB_SOUTHEAST:
							if (mpr_p_southEastgeom->getNumParents()>0 /*&& mpr_p_geode->containsDrawable(mpr_p_southEastgeom.get())*/)
							{
								CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southEastgeom.get());
								mpr_terrain->AddOperation(_opera);			
							}
							break;
						case GlB_SOUTHWEST:
							if (mpr_p_southWestgeom->getNumParents()>0 /*&& mpr_p_geode->containsDrawable(mpr_p_southWestgeom.get())*/)
							{	
								CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southWestgeom.get());
								mpr_terrain->AddOperation(_opera);		
							}
							break;
						}
					}
				}
				itr++;
			}	
		}
		// 合并
		else 
		{
			//if (false)
			//{
			//	sprintf(buff,"**** [Level: %d , row: %d, col: %d] _bVisible: %d , _bSplit: %d. ",mpr_level,(long)mpr_row,(long)mpr_column,_bVisible,_bSplit);
			//	LOG(buff);
			//}

			if (mpr_nwchild != NULL && mpr_nwchild->IsInitialized())
			{						
				mpr_nwchild->Dispose();		
				//delete mpr_p_northwest_childtile;
				mpr_nwchild = NULL;
				if (mpr_p_northWestgeom->getNumParents()<=0 /*&& mpr_p_geode->containsDrawable(mpr_p_northWestgeom.get())==false*/)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northWestgeom.get());
					mpr_terrain->AddOperation(_opera);											
				}									
			}

			if (mpr_nechild != NULL && mpr_nechild->IsInitialized())
			{					
				mpr_nechild->Dispose();
				//delete mpr_p_northeast_childtile;
				mpr_nechild = NULL;
				if (mpr_p_northEastgeom->getNumParents()<=0 /*&& mpr_p_geode->containsDrawable(mpr_p_northEastgeom.get())==false*/)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northEastgeom.get());
					mpr_terrain->AddOperation(_opera);											
				}	
			}

			if (mpr_sechild != NULL && mpr_sechild->IsInitialized())
			{								
				mpr_sechild->Dispose();
				//delete mpr_p_southeast_childtile;
				mpr_sechild = NULL;
				if (mpr_p_southEastgeom->getNumParents()<=0/* && mpr_p_geode->containsDrawable(mpr_p_southEastgeom.get())==false*/)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southEastgeom.get());
					mpr_terrain->AddOperation(_opera);	
				}
			}

			if (mpr_swchild != NULL && mpr_swchild->IsInitialized())
			{		
				mpr_swchild->Dispose();	
				//delete mpr_p_southwest_childtile;
				mpr_swchild = NULL;
				if (mpr_p_southWestgeom->getNumParents()<=0/* && mpr_p_geode->containsDrawable(mpr_p_southWestgeom.get())==false*/)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southWestgeom.get());
					mpr_terrain->AddOperation(_opera);	
				}					
			}
				
		}
	}
}

void CGlbGlobeTerrainTile::UpdateFlat(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
										std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
										std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs, 
										std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs)
{
	// 判断是否需要中止更新 2016.9.14
	if (mpr_terrain->IsUpdateNeedTerminate())
		return;

	glbBool _bVisible = IsVisible();
	if (!mpr_isinitialized)
	{	
		if (_bVisible){			
			// 加载本块数据!!!! 纹理和实际dem高程数据
			Initialize(domlayers,demlayers,terrainobjs,mTerrainobjs);		
		}
	}
	if(mpr_isinitialized)
	{
		glbBool _bSplit = IsTileSplit();		
		if (_bVisible && _bSplit)
		{
			if (mpr_nechild == NULL || mpr_nwchild == NULL || 
				mpr_sechild == NULL || mpr_swchild == NULL)
			{
				ComputeChildren();
			}
			if (mpr_nechild != NULL)
			{
				std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
				mpr_nechild->FilterOnTerrainObjects(terrainobjs,childtobjs);
				mpr_nechild->Update(domlayers,demlayers,childtobjs,mTerrainobjs);		
				if ( mpr_nechild->IsInitialized() )
				{// remove drawable
					if (mpr_p_northEastgeom->getNumParents()>0/*&& mpr_p_geode->containsDrawable(mpr_p_northEastgeom.get())*/)
					{
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northEastgeom.get());
						mpr_terrain->AddOperation(_opera);								
					}
				}
			}

			if (mpr_nwchild != NULL)
			{
				std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
				mpr_nwchild->FilterOnTerrainObjects(terrainobjs,childtobjs);
				mpr_nwchild->Update(domlayers,demlayers,childtobjs,mTerrainobjs);
				if (mpr_nwchild->IsInitialized())
				{// remove drawable
					if (mpr_p_northWestgeom->getNumParents()>0 /*&& mpr_p_geode->containsDrawable(mpr_p_northWestgeom.get())*/)
					{
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northWestgeom.get());
						mpr_terrain->AddOperation(_opera);								
					}
				}
			}

			if (mpr_sechild != NULL)
			{
				std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
				mpr_sechild->FilterOnTerrainObjects(terrainobjs,childtobjs);
				mpr_sechild->Update(domlayers,demlayers,childtobjs,mTerrainobjs);		
				if (mpr_sechild->IsInitialized())
				{// remove drawable
					if (mpr_p_southEastgeom->getNumParents()>0 /*&& mpr_p_geode->containsDrawable(mpr_p_southEastgeom.get())*/)
					{
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southEastgeom.get());
						mpr_terrain->AddOperation(_opera);			
					}
				}
			}

			if (mpr_swchild != NULL)
			{
				std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
				mpr_swchild->FilterOnTerrainObjects(terrainobjs,childtobjs);
				mpr_swchild->Update(domlayers,demlayers,childtobjs,mTerrainobjs);
				if (mpr_swchild->IsInitialized())
				{// remove drawable
					if (mpr_p_southWestgeom->getNumParents()>0 /* && mpr_p_geode->containsDrawable(mpr_p_southWestgeom.get())*/)
					{	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southWestgeom.get());
						mpr_terrain->AddOperation(_opera);		
					}
				}
			}
		}
		else
		{
			if (mpr_nwchild != NULL && mpr_nwchild->IsInitialized())
			{						
				mpr_nwchild->Dispose();		
				//delete mpr_p_northwest_childtile;
				mpr_nwchild = NULL;
				if (mpr_p_northWestgeom->getNumParents()<=0 /*&& mpr_p_geode->containsDrawable(mpr_p_northWestgeom.get())==false*/)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northWestgeom.get());
					mpr_terrain->AddOperation(_opera);											
				}									
			}

			if (mpr_nechild != NULL && mpr_nechild->IsInitialized())
			{					
				mpr_nechild->Dispose();
				//delete mpr_p_northeast_childtile;
				mpr_nechild = NULL;
				if (mpr_p_northEastgeom->getNumParents()<=0 /* && mpr_p_geode->containsDrawable(mpr_p_northEastgeom.get())==false*/)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northEastgeom.get());
					mpr_terrain->AddOperation(_opera);											
				}	
			}

			if (mpr_sechild != NULL && mpr_sechild->IsInitialized())
			{								
				mpr_sechild->Dispose();
				//delete mpr_p_southeast_childtile;
				mpr_sechild = NULL;
				if (mpr_p_southEastgeom->getNumParents()<=0 /*&& mpr_p_geode->containsDrawable(mpr_p_southEastgeom.get())==false*/)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southEastgeom.get());
					mpr_terrain->AddOperation(_opera);	
				}
			}

			if (mpr_swchild != NULL && mpr_swchild->IsInitialized())
			{		
				mpr_swchild->Dispose();	
				//delete mpr_p_southwest_childtile;
				mpr_swchild = NULL;
				if (mpr_p_southWestgeom->getNumParents()<=0/* && mpr_p_geode->containsDrawable(mpr_p_southWestgeom.get())==false*/)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southWestgeom.get());
					mpr_terrain->AddOperation(_opera);	
				}					
			}
		}
	}
}

void CGlbGlobeTerrainTile::LoadData(std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
									std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
									std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs, 
									std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs)
{// 地形块初始化时调用的线程加载数据接口

	if (mpr_isDataLoaded)
		return;

	//char buff[256];
	osg::Timer_t startFrameTick = osg::Timer::instance()->tick();
	//if (false)
	//{	
	//	//LOG_FUNC_BEGIN
	//	sprintf(buff,"	**Tile LoadData begin. [Level: %d , row: %d, col: %d]",mpr_level,(long)mpr_row,(long)mpr_column);
	//	LOG(buff);
	//	//GlbLogWOutput(GLB_LOGTYPE_INFO,L"**Tile LoadData begin. [Level: %d , row: %d, col: %d]\r\n",mpr_level,(long)mpr_row,(long)mpr_column);		
	//}

	// 1. 读取地形块的dom和dem数据
	glbBool res = LoadTileData();
	//if (false)
	//{
	//	osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
	//	double frameTime = osg::Timer::instance()->delta_m(startFrameTick, endFrameTick);
	//	//GlbLogWOutput(GLB_LOGTYPE_INFO,L"**Tile LoadData end. [Level: %d , row: %d, col: %d]  %lf .\r\n",mpr_level,(long)mpr_row,(long)mpr_column,frameTime);	
	//	sprintf(buff,"	**Tile LoadTileData [Level: %d , row: %d, col: %d] use time %lf.",mpr_level,(long)mpr_row,(long)mpr_column,frameTime);
	//	LOG(buff);
	//}
	// 2. 读取domlayers,terrainobjs的数据
	glbBool res2 = LoadOverlayTextureData(domlayers,terrainobjs);
	// 3. 读取demlayers，mterrainobjs的数据
	glbBool res3 = LoadOverlayDemData(demlayers,mTerrainobjs);

	// 4. 设置标志
	mpr_isLoadingData = false;
	mpr_isDataLoaded = true;

	//if (false)
	//{
	//	osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
	//	double frameTime = osg::Timer::instance()->delta_m(startFrameTick, endFrameTick);
	//	//GlbLogWOutput(GLB_LOGTYPE_INFO,L"**Tile LoadData end. [Level: %d , row: %d, col: %d]  %lf .\r\n",mpr_level,(long)mpr_row,(long)mpr_column,frameTime);	
	//	sprintf(buff,"	**Tile LoadData end. [Level: %d , row: %d, col: %d] terObj %d use time %lf.",mpr_level,(long)mpr_row,(long)mpr_column,(long)terrainobjs.size(),frameTime);
	//	LOG(buff);
	//	//LOG_FUNC_END
	//}
}

glbBool CGlbGlobeTerrainTile::LoadTileData()
{
// 进入数据集读取临界区
EnterCriticalSection(&(mpr_terrain->mpr_tDatasetAccessCritical));
	CGlbPixelBlock* p_dompixelblock = mpr_terrain->GetTerrainDataset()->CreateDomPixelBlock();
	glbBool accessdomsuccess = mpr_terrain->GetTerrainDataset()->ReadDom(mpr_level,mpr_column,mpr_row,p_dompixelblock);
	CGlbPixelBlock* p_dempixelblock = mpr_terrain->GetTerrainDataset()->CreateDemPixelBlock();
	glbBool accessdemsuccess = mpr_terrain->GetTerrainDataset()->ReadDem(mpr_level,mpr_column,mpr_row,p_dempixelblock);
	//if(mpr_level == 2 && mpr_row == 2 && mpr_column == -3)
	//{
	//	CGlbWString ssp = CGlbWString::FormatString(L"D:\\testDem.txt");			
	//	p_dempixelblock->ExportToText(ssp.c_str());
	//}
	if(accessdemsuccess == false && mpr_level == 0)
	{
		glbByte* _pdemdata = NULL;
		glbByte* _pmaskdata= NULL;
		glbInt32 columns,rows;
		p_dempixelblock->GetSize(columns,rows);
		p_dempixelblock->GetPixelData((glbByte**)(&_pdemdata));
		p_dempixelblock->GetMaskData((glbByte**)(&_pmaskdata));
		memset(_pdemdata,0,columns*rows*sizeof(glbByte));
		memset(_pmaskdata,255,columns*rows*sizeof(glbByte));
		accessdemsuccess = true;
	}
// 离开数据集读取临界区
LeaveCriticalSection(&(mpr_terrain->mpr_tDatasetAccessCritical));

	if (accessdomsuccess)
	{// 读取dom数据成功				
		glbInt32 bandcnt = p_dompixelblock->GetBandCount();
		glbInt32 pixelrows, pixelcolumns;
		p_dompixelblock->GetSize(pixelcolumns,pixelrows);
		glbByte* p_domdata = NULL;
		p_dompixelblock->GetPixelData(&p_domdata);
		if (pixelrows<=0 || pixelcolumns <= 0 || bandcnt < 0 || p_domdata == NULL)
		{	
			if (p_dompixelblock)
				delete p_dompixelblock;
			return false;				
		}
		GLenum pixelFormat = GL_RGBA;
		switch(bandcnt)
		{
		case 1:	pixelFormat = GL_LUMINANCE;	break;
		case 2:	pixelFormat = GL_LUMINANCE_ALPHA;	break;
		case 3:	pixelFormat = GL_RGB;	break;
		case 4:/* 默认值RGBA*/break;
		}	
		GlbPixelTypeEnum pixeltype = p_dompixelblock->GetPixelType();
		switch(pixeltype)
		{
		case GLB_PIXELTYPE_BYTE:
			{// 当前只处理BYTE图像
				mpr_p_osgImg = new osg::Image;
				mpr_p_osgImg->allocateImage(pixelcolumns,pixelrows,1,pixelFormat,GL_UNSIGNED_BYTE);

				// 拷贝dom数据
				unsigned int imagesize = mpr_p_osgImg->getImageSizeInBytes();
				mpr_image = new glbByte[imagesize];
				memcpy(mpr_image,p_domdata,imagesize);
				mpr_origin_image = new glbByte[imagesize];
				memcpy(mpr_origin_image,mpr_image,imagesize);	
				{// 根据地面不透明度修改mpr_image的Alpha分量 2014.10.30
					glbByte* ppMask = NULL;//mask数据【255表示有效数】
					p_dompixelblock->GetMaskData(&ppMask);

					glbInt32 _rows,_columns;
					p_dompixelblock->GetSize(_columns,_rows);
					
					glbInt32 tOpacity = mpr_terrain->GetOpacity() * 255 / 100;
					if (tOpacity<255)
					{
						void* _val=NULL;
						for (glbInt32 i = 0; i < _rows; i++)
							for(glbInt32 j = 0; j < _columns; j++)
							{
								glbInt32 idx = i*_columns+j;
								if (ppMask[idx]==255)
								{// 有效值[255]时，可以替换源dom数据
									mpr_image[4*idx+3] = tOpacity;
								}
							}
					}					
					//if (tOpacity<255)
					//{
					//	for (glbInt32 kk = 0; kk < pixelcolumns*pixelrows; kk++)
					//	{
					//		mpr_image[kk*4+3] = tOpacity;
					//	}
					//}
					mpr_opacity = tOpacity;
				}							
				memcpy(mpr_p_osgImg->data(),mpr_image,imagesize);
				mpr_imagesize = pixelrows;
			}
			break;
		}
	}
	else
	{
		glbInt32 domcolumns,domrows;
		glbByte* parentdomdata=NULL;
		mpr_parent_tile->GetDomdata(&parentdomdata,domcolumns,domrows);		
		
		if (parentdomdata)
		{// 加密纹理数据,加密一倍【ok!! 2014.6.10】
			mpr_image = MultifyDomSection(parentdomdata,domcolumns,domrows,mpr_location_enum);
			mpr_imagesize = domcolumns;

			mpr_p_osgImg = new osg::Image;
			GLenum pixelFormat = GL_RGBA;
			mpr_p_osgImg->allocateImage(domcolumns,domrows,1,pixelFormat,GL_UNSIGNED_BYTE);
			unsigned int imagesize = mpr_p_osgImg->getImageSizeInBytes();
			memcpy(mpr_p_osgImg->data(),mpr_image,imagesize);

			mpr_origin_image = new glbByte[imagesize];
			memcpy(mpr_origin_image,mpr_image,imagesize);	
		}
	}

	if (accessdemsuccess)
	{// 能读到dem数据成功
		//CGlbExtent* p_extent = (CGlbExtent*)p_dempixelblock->GetExtent();
		//p_extent->Get(0L,0L,0L,0L,&mpr_minalt,&mpr_maxalt);
		//if (mpr_extent)
		//{// 更新外包
		//	mpr_extent->Set(mpr_west,mpr_east,mpr_south,mpr_north,mpr_minalt,mpr_minalt);
		//}

		glbFloat* _pdemdata = NULL;
		glbInt32 bandcnt = p_dempixelblock->GetBandCount();
		glbInt32 columns,rows;
		p_dempixelblock->GetSize(columns,rows);
		p_dempixelblock->GetPixelData((glbByte**)(&_pdemdata));
		GlbPixelTypeEnum pixeltype = p_dempixelblock->GetPixelType();
		if (columns<=0 || rows <= 0 || bandcnt < 0 || _pdemdata == NULL)
		{			
			if (p_dempixelblock)
				delete p_dempixelblock;
			return false;
		}

		glbInt32 totaldatasize = 0;
		switch(pixeltype)
		{
		case GLB_PIXELTYPE_FLOAT32:
			{// 当前只处理FLOAT数据
				mpr_demdata = new float[columns*rows];
				totaldatasize = columns*rows*sizeof(glbFloat);
				memcpy(mpr_demdata,_pdemdata,totaldatasize);		
			}
			break;
		}		
		if (mpr_demdata)
		{// 更改mpr_vertex_count的值
			mpr_vertex_count = columns - columns%2; 
			mpr_demsize = columns;
		}
	}
	else
	{// 没有读取到本级的dem，从取上一级的demdata,加密一倍使用.	
		glbInt32 demcolumns,demrows;
		glbFloat* parentdemdata=NULL;
		mpr_parent_tile->GetDemdata(&parentdemdata,demcolumns,demrows);		
		if (parentdemdata)
		{// 加密高程点,加密一倍
			mpr_demdata = MultifyDemSection(parentdemdata,demcolumns,demrows,mpr_location_enum);
			mpr_vertex_count = demcolumns - demcolumns%2; 
			mpr_demsize = demcolumns;
		}
	}
	
	// 释放pixelblock
	if (p_dompixelblock)
		delete p_dompixelblock;
	if (p_dempixelblock)
		delete p_dempixelblock;

	if (accessdemsuccess || accessdomsuccess)
		return true;
	return false;
}

glbBool CGlbGlobeTerrainTile::LoadOverlayTextureData(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
														std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs)
{
	//1. 遍历domlayers，合并tile的底图和domlayers，生成mpr_overlayImage
	std::vector<glbref_ptr<CGlbGlobeDomLayer>>::iterator _itr = domlayers.begin();
	while(_itr != domlayers.end())
	{
		CGlbGlobeDomLayer* p_domlayer = dynamic_cast<CGlbGlobeDomLayer*>((*_itr).get());
		if (p_domlayer && p_domlayer->IsShow())
		{
			glbBool bInter = IsDomlayerInterTile(p_domlayer);
			if (!bInter)
			{// 没有交集，转入下一个图层
				_itr++;
				continue;
			}

			glbBool bHasPyramid = p_domlayer->HasPyramid();			
			glbInt32 opacity = p_domlayer->GetOpacity();
			CGlbPixelBlock* p_pixelblock=NULL;
			p_domlayer->CreatePixelBlock(&p_pixelblock);
			glbBool bsucc =false;			
			if (bHasPyramid)
			{// 有金子塔	
				glbInt32 minLevel,maxLevel;
				p_domlayer->GetPyramidLevels(&minLevel,&maxLevel);
				if (mpr_level>maxLevel)
				{// tile等级比domlayer的最大等级都大					
					bsucc = InterpolateDomLayerData(p_domlayer,maxLevel,p_pixelblock);
				}
				else// 加载跟当前地形块tile等级相同的，行列号则通过范围来确定
					bsucc = p_domlayer->ReadData(mpr_level,mpr_row,mpr_column,p_pixelblock);				
			}
			else
			{// 没有金字塔
				bsucc = p_domlayer->ReadDataEx(mpr_extent.get(),p_pixelblock);
			}

			// 将p_pixelblock数据+mpr_image -> mpr_overlayImage
			if (bsucc)
			{// agg进行转换,将一个layer的image混合到tile的源image上
				unsigned int imagesize = mpr_p_osgImg->getImageSizeInBytes();
				if (!mpr_overlayImage)									
					mpr_overlayImage = new glbByte[imagesize];									
				memcpy(mpr_overlayImage,mpr_image,imagesize);
				// agg混合p_pixelblock的数据到mpr_overlayImage上				
				glbByte* p_blockdata=NULL;
				p_pixelblock->GetPixelData(&p_blockdata);
				// 修改BlendImage使其适应RGBA,RGB,LUMINNT,LUMNIT_ALPHA
				glbBool res = BlendImage(mpr_overlayImage,p_blockdata,mpr_imagesize,mpr_imagesize,opacity,true);
				//{// 测试混合效果	2014.6.14   ml	!!!!!!!!			
				//	if (mpr_overlayImage)			
				//		memcpy(mpr_p_osgImg->data(),mpr_overlayImage,imagesize);	
				//	std::string filename("D:\\overlayimage.tif");
				//	osgDB::writeImageFile(*mpr_p_osgImg.get(), filename);	
				//}				
				// end混合
			}
			
			if (p_pixelblock)
				delete p_pixelblock;	
		}
		++_itr;
	}

	// 2. 拷贝mpr_overlayImage或mpr_image中的数据到mpr_p_osgImg
	osg::Image* p_newOsgImg = new osg::Image;
	p_newOsgImg->allocateImage(mpr_p_osgImg->s(),mpr_p_osgImg->t(),mpr_p_osgImg->r(),mpr_p_osgImg->getPixelFormat(),GL_UNSIGNED_BYTE);

	unsigned int totalbytes = mpr_p_osgImg->getImageStepInBytes();
	if (mpr_overlayImage)			
		memcpy(p_newOsgImg->data(),mpr_overlayImage,totalbytes);		
	else if (mpr_image)	
		memcpy(p_newOsgImg->data(),mpr_image,totalbytes);	

	//3. 遍历terrainobjs，在mpr_overlayImage上叠加矢量对象， 生成_pFinalImage
	std::vector<glbref_ptr<CGlbGlobeRObject>>::iterator _itrobj = terrainobjs.begin();
	while(_itrobj!=terrainobjs.end())
	{
		(*_itrobj)->DrawToImage(p_newOsgImg->data(),p_newOsgImg->s(),p_newOsgImg->t(),*mpr_extent.get());	
		++_itrobj;
	}
	mpr_p_osgImg = p_newOsgImg;

	return false;
}

glbBool CGlbGlobeTerrainTile::LoadOverlayDemData(	std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
													std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs)
{
//#ifdef _DEBUG
//	WCHAR wBuff[128];
//	wsprintf(wBuff,L"	LoadOverlayDemData begin. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
//	OutputDebugString(wBuff);
//	wsprintf(wBuff,L"		demlayers count: %d, mTerrainobjs count :%d\n",demlayers.size(),mTerrainobjs.size());
//	OutputDebugString(wBuff);
//#endif

	if (demlayers.size()<=0 && mTerrainobjs.size()<=0)
	{// 没有图层和地形修改对象的话不用mpr_overlaydemdata
		if (mpr_overlaydemdata)
		{
			delete [] mpr_overlaydemdata;
			mpr_overlaydemdata = NULL;
		}
		return false;
	}

	glbBool _overlayflag = false;
	if (mpr_overlaydemdata)
	{// 如果overlaydemdata已经存在，重新初始化其值		
		memcpy(mpr_overlaydemdata,mpr_demdata,mpr_demsize*mpr_demsize*sizeof(glbFloat));
	}

	// 1. 遍历demlayers,用其dem数据替换mpr_demdata数据
	std::vector<glbref_ptr<CGlbGlobeDemLayer>>::iterator _itr = demlayers.begin();
	while(_itr != demlayers.end())
	{
		CGlbGlobeDemLayer* p_demlayer = dynamic_cast<CGlbGlobeDemLayer*>((*_itr).get());
		if (p_demlayer && p_demlayer->IsShow())
		{
			glbBool bInter = IsDemlayerInterTile(p_demlayer);
			if (!bInter)
			{// 没有交集，转入下一个图层
				_itr++;
				continue;
			}

			if (!mpr_overlaydemdata)
			{// 如果没有创建overlaydemdata，创建之并将demdata赋值给它
				mpr_overlaydemdata = new glbFloat[mpr_demsize*mpr_demsize];
				memcpy(mpr_overlaydemdata,mpr_demdata,mpr_demsize*mpr_demsize*sizeof(glbFloat));
			}

			glbBool bHasPyramid = p_demlayer->HasPyramid();			
			CGlbPixelBlock* p_pixelblock=NULL;
			p_demlayer->CreatePixelBlock(&p_pixelblock);
			glbBool bsucc =false;			
			glbInt32 minLevel,maxLevel;
			if (bHasPyramid)
			{// 有金子塔					
				p_demlayer->GetPyramidLevels(&minLevel,&maxLevel);
				if (mpr_level>maxLevel)
				{// tile等级比domlayer的最大等级都大					
					bsucc = InterpolateDemLayerData(p_demlayer,maxLevel,p_pixelblock);
				}
				else// 加载跟当前地形块tile等级相同的，行列号则通过范围来确定
					bsucc = p_demlayer->ReadData(mpr_level,mpr_row,mpr_column,p_pixelblock);				
			}
			else
			{// 没有金字塔
				bsucc = p_demlayer->ReadDataEx(mpr_extent.get(),p_pixelblock);
			}
		
			glbBool isAccumulationMode = p_demlayer->IsAccumulationMode();
			if (bsucc)
			{// 替换mpr_demdata数据				
				glbByte* ppMask = NULL;//mask数据【255表示有效数】
				p_pixelblock->GetMaskData(&ppMask);
				glbInt32 _rows,_columns;
				p_pixelblock->GetSize(_columns,_rows);
				glbFloat pVal;
				void* _val=NULL;
				for (glbInt32 i = 0; i < _rows; i++)
					for(glbInt32 j = 0; j < _columns; j++)
					{
						glbInt32 idx = i*_columns+j;
						if (ppMask[idx]==255)
						{// 有效值[255]时，可以替换源高程数据
							// 获取dem图层块数据
							p_pixelblock->GetPixelVal(0,j,i,&_val);							
							pVal = *((glbFloat*)_val);	
							
							if (isAccumulationMode)//累加模式累加高程
								mpr_overlaydemdata[idx] += pVal;
							else//overlay模式 替换源数据
								mpr_overlaydemdata[idx] = pVal;//pVal*10.0+1000;//pVal;		

							_overlayflag = true;
						}
					}
			}

			if (p_pixelblock)
				delete p_pixelblock;	
		}
		++_itr;
	}

	// 2. 遍历mTerrainobjs,用其dem数据替换mpr_demdata数据
	glbDouble minx,miny,maxx,maxy;
	mpr_extent->Get(&minx,&maxx,&miny,&maxy);

	std::vector<glbref_ptr<CGlbGlobeRObject>>::iterator _itrobj = mTerrainobjs.begin();
	while(_itrobj!=mTerrainobjs.end())                  
	{
		CGlbGlobeTerrainModify* tm = dynamic_cast<CGlbGlobeTerrainModify*>((*_itrobj).get());
		if (tm)
		{
			CGlbLine* region = tm->GetRegion();
			glbDouble rminx,rmaxx,rminy,rmaxy;
			CGlbExtent* regionExtent = const_cast<CGlbExtent*>(region->GetExtent());
			regionExtent->Get(&rminx,&rmaxx,&rminy,&rmaxy);

			glbDouble mx = max(minx,rminx);
			glbDouble my = max(miny,rminy);
			glbDouble Mx = min(maxx,rmaxx);
			glbDouble My = min(maxy,rmaxy);
			if (mx<Mx && my<My)
			{// 区域有交集
				if (!mpr_overlaydemdata)
				{// 如果没有创建overlaydemdata，创建之并将demdata赋值给它
					mpr_overlaydemdata = new glbFloat[mpr_demsize*mpr_demsize];
					memcpy(mpr_overlaydemdata,mpr_demdata,mpr_demsize*mpr_demsize*sizeof(glbFloat));
				}
				glbBool overwrite = tm->ModifyDem(mpr_overlaydemdata,mpr_demsize,mpr_extent.get());
				if (overwrite)
					_overlayflag = true;
			}			
		}		
		_itrobj++;
	}

//#ifdef _DEBUG	
//	wsprintf(wBuff,L"	LoadOverlayDemData end. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
//	OutputDebugString(wBuff);
//#endif
	return _overlayflag;
}

osg::ref_ptr<osg::Group> CGlbGlobeTerrainTile::CreateElevatedMesh()
{
	osg::ref_ptr<osg::Vec3Array> northWestVertices = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> southWestVertices = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> northEastVertices = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> southEastVertices = new osg::Vec3Array();

	osg::ref_ptr<osg::Vec2Array> northWestToords = new osg::Vec2Array();
	osg::ref_ptr<osg::Vec2Array> southWestToords = new osg::Vec2Array();
	osg::ref_ptr<osg::Vec2Array> northEastToords = new osg::Vec2Array();
	osg::ref_ptr<osg::Vec2Array> southEastToords = new osg::Vec2Array();

	osg::ref_ptr<osg::Vec3Array> northWestNormals = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> southWestNormals = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> northEastNormals = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> southEastNormals = new osg::Vec3Array();

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array();
	osg::Vec4 clr = AdjustTileColor(mpr_level);	
	colorArray->push_back(clr);

	 
	osg::ref_ptr<osg::Vec3dArray> p_north_easthalf_vertexarray = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> p_north_westhalf_vertexarray = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> p_south_easthalf_vertexarray = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> p_south_westhalf_vertexarray = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> p_east_northhalf_vertexarray = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> p_east_southhalf_vertexarray = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> p_west_northhalf_vertexarray = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> p_west_southhalf_vertexarray = new osg::Vec3dArray;

	osg::ref_ptr<osg::Vec3dArray> p_center_westhalf_vertexarray = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> p_center_easthalf_vertexarray = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> p_center_northhalf_vertexarray = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> p_center_southhalf_vertexarray = new osg::Vec3dArray;

	osg::ref_ptr<osg::Vec2Array> p_north_easthalf_texcoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> p_north_westhalf_texcoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> p_south_easthalf_texcoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> p_south_westhalf_texcoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> p_east_northhalf_texcoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> p_east_southhalf_texcoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> p_west_northhalf_texcoords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> p_west_southhalf_texcoords = new osg::Vec2Array;

	glbDouble layerRadius = osg::WGS_84_RADIUS_EQUATOR;
	//double scaleFactor = (mpr_vertex_count%2==0) ? 1.0 / (double)mpr_vertex_count : 1.0 / (double)(mpr_vertex_count-1);
	glbDouble scaleFactor = 1.0 / (glbDouble)mpr_vertex_count;
	glbInt32 thisVertexCount = mpr_vertex_count / 2 + (mpr_vertex_count % 2);
	glbInt32 thisVertexCountPlus1 = (thisVertexCount%2 == 0) ? thisVertexCount + 1 : thisVertexCount;

	northWestVertices->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	southWestVertices->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	northEastVertices->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	southEastVertices->reserve(thisVertexCountPlus1*thisVertexCountPlus1);

	northWestToords->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	southWestToords->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	northEastToords->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	southEastToords->reserve(thisVertexCountPlus1*thisVertexCountPlus1);

	northWestNormals->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	southWestNormals->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	northEastNormals->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	southEastNormals->reserve(thisVertexCountPlus1*thisVertexCountPlus1);

	p_north_easthalf_vertexarray->reserve(thisVertexCountPlus1);
	p_north_westhalf_vertexarray->reserve(thisVertexCountPlus1);
	p_south_easthalf_vertexarray->reserve(thisVertexCountPlus1);
	p_south_westhalf_vertexarray->reserve(thisVertexCountPlus1);
	p_east_northhalf_vertexarray->reserve(thisVertexCountPlus1);
	p_east_southhalf_vertexarray->reserve(thisVertexCountPlus1);
	p_west_northhalf_vertexarray->reserve(thisVertexCountPlus1);
	p_west_southhalf_vertexarray->reserve(thisVertexCountPlus1);
	p_center_westhalf_vertexarray->reserve(thisVertexCountPlus1);
	p_center_easthalf_vertexarray->reserve(thisVertexCountPlus1);
	p_center_northhalf_vertexarray->reserve(thisVertexCountPlus1);
	p_center_southhalf_vertexarray->reserve(thisVertexCountPlus1);

	p_north_easthalf_texcoords->reserve(thisVertexCountPlus1);
	p_north_westhalf_texcoords->reserve(thisVertexCountPlus1);
	p_south_easthalf_texcoords->reserve(thisVertexCountPlus1);
	p_south_westhalf_texcoords->reserve(thisVertexCountPlus1);
	p_east_northhalf_texcoords->reserve(thisVertexCountPlus1);
	p_east_southhalf_texcoords->reserve(thisVertexCountPlus1);
	p_west_northhalf_texcoords->reserve(thisVertexCountPlus1);
	p_west_southhalf_texcoords->reserve(thisVertexCountPlus1);

	glbInt32 totalVertexCount = thisVertexCountPlus1 * thisVertexCountPlus1;
	const glbDouble Degrees2Radians = osg::PI / 180.0;

	// Cache western sin/cos of longitude values
	glbDouble* sinLon = new glbDouble[thisVertexCountPlus1];
	glbDouble* cosLon = new glbDouble[thisVertexCountPlus1];

	glbInt32 i = 0;
	glbDouble angle = mpr_west * Degrees2Radians;
	glbDouble deltaAngle = scaleFactor * mpr_longitude_span * Degrees2Radians;	
	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = mpr_west * Degrees2Radians + i * deltaAngle;
		sinLon[i] = angle;
		cosLon[i] = angle;
	}

	glbFloat* _pDemdata = NULL;
	if (mpr_overlaydemdata)
		_pDemdata = mpr_overlaydemdata;
	else
		_pDemdata = mpr_demdata;

	glbDouble exaggrate = mpr_terrain->GetExaggrate();

	glbFloat _geoheight = GetHeightData(_pDemdata, thisVertexCountPlus1-1, thisVertexCountPlus1-1, GLB_NORTHWEST)*exaggrate;

	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
		g_ellipsmodel->convertLatLongHeightToXYZ(
			mpr_center_latitude , mpr_center_longitude,_geoheight,//0.0,
			mpr_local_origin.x(), mpr_local_origin.y(), mpr_local_origin.z() );
	}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		mpr_local_origin.set(0.5*(mpr_west+mpr_east), 0.5*(mpr_south+mpr_north),_geoheight);
	}

	glbDouble deltaX = scaleFactor * (mpr_east - mpr_west);
	glbDouble deltaY = -scaleFactor * (mpr_north - mpr_south);

	glbDouble _minAlt = 999999999.9;
	glbDouble _maxAlt = -999999999.9;

	deltaAngle = -scaleFactor * mpr_latitude_span * Degrees2Radians;
	osg::Vec3d geocentric;
	glbDouble geoheight;
	osg::Vec2 geotexcoord;
	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = mpr_north * Degrees2Radians + i * deltaAngle;
		glbDouble sinLat = sin(angle);
		glbDouble radCosLat = cos(angle) * layerRadius;		
		for (glbInt32 j = 0; j < thisVertexCountPlus1; j++)
		{			
			geoheight = GetHeightData(_pDemdata, j, i, GLB_NORTHWEST)*exaggrate;
			if (_minAlt>geoheight)
				_minAlt = geoheight;
			if (_maxAlt<geoheight)
				_maxAlt = geoheight;

			if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
				g_ellipsmodel->convertLatLongHeightToXYZ(
					angle , cosLon[j], geoheight/*0.0*/,
					geocentric.x(), geocentric.y(), geocentric.z() );
				//northWestVertices->push_back(geocentric);
				northWestVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));									
				geotexcoord.set(j * scaleFactor, i * scaleFactor);
				northWestToords->push_back(geotexcoord);
				geocentric.normalize();
				northWestNormals->push_back(osg::Vec3f(geocentric));
				// 记录一下
				geocentric.set(angle,cosLon[j],geoheight);
			}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
				geocentric.set( mpr_west + j * deltaX, mpr_north + i * deltaY, geoheight);				
				northWestVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));									
				geotexcoord.set(j * scaleFactor , i * scaleFactor);
				northWestToords->push_back(geotexcoord);
				northWestNormals->push_back(osg::Vec3f(0,0,1));
				// 记录一下
				geocentric.set( mpr_north + i * deltaY,mpr_west + j * deltaX,geoheight);
			}

			if (i == 0){
				//mpr_p_north_westhalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));
				p_north_westhalf_vertexarray->push_back(geocentric);
				p_north_westhalf_texcoords->push_back(geotexcoord);
			}
			if (j == 0){
				//mpr_p_west_northhalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));	
				p_west_northhalf_vertexarray->push_back(geocentric);
				p_west_northhalf_texcoords->push_back(geotexcoord);
			}	
			if ( i == thisVertexCountPlus1-1){
				//mpr_p_center_westhalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));		
				p_center_westhalf_vertexarray->push_back(geocentric);		
			}
		}
	}
	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = 0.5 * (mpr_north + mpr_south) * Degrees2Radians + i * deltaAngle;
		glbDouble sinLat = sin(angle);
		glbDouble radCosLat = cos(angle) * layerRadius;

		for (glbInt32 j = 0; j < thisVertexCountPlus1; j++)
		{	
			geoheight = GetHeightData(_pDemdata, j, i, GlB_SOUTHWEST)*exaggrate;
			if (_minAlt>geoheight)
				_minAlt = geoheight;
			if (_maxAlt<geoheight)
				_maxAlt = geoheight;

			if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
				g_ellipsmodel->convertLatLongHeightToXYZ(
					angle , cosLon[j], geoheight/*0.0*/,
					geocentric.x(), geocentric.y(), geocentric.z() );
				//southWestVertices->push_back(geocentric);
				southWestVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));

				geotexcoord.set(j * scaleFactor,(i + thisVertexCount) * scaleFactor);
				//geotexcoord.set(u_offset + j * scaleFactor * u_scale,v_offset + (i + thisVertexCount) * scaleFactor * v_scale);
				//southWestToords->push_back(osg::Vec2(j * scaleFactor, (i + thisVertexCount) * scaleFactor));
				southWestToords->push_back(geotexcoord);
				geocentric.normalize();
				southWestNormals->push_back(osg::Vec3f(geocentric));
				// 记录一下
				geocentric.set(angle,cosLon[j],geoheight);
			}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
				geocentric.set( mpr_west + j * deltaX, 0.5*(mpr_north+mpr_south) + i * deltaY, geoheight);				
				southWestVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));
				geotexcoord.set(j * scaleFactor,(i + thisVertexCount) * scaleFactor);
				southWestToords->push_back(geotexcoord);
				//southWestNormals?
				southWestNormals->push_back(osg::Vec3f(0,0,1));
				// 记录一下
				geocentric.set( 0.5*(mpr_north+mpr_south) + i * deltaY, mpr_west + j * deltaX,  geoheight);
			}

			if(i == thisVertexCountPlus1-1){
				//mpr_p_south_westhalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));
				p_south_westhalf_vertexarray->push_back(geocentric);
				p_south_westhalf_texcoords->push_back(geotexcoord);
			}			
			if(j == 0){
				//mpr_p_west_southhalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));	
				p_west_southhalf_vertexarray->push_back(geocentric);	
				p_west_southhalf_texcoords->push_back(geotexcoord);
			}	
			if ( j == thisVertexCountPlus1-1){
				//mpr_p_center_southhalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));
				p_center_southhalf_vertexarray->push_back(geocentric);
			}
		}
	}

	deltaAngle = scaleFactor * mpr_longitude_span * Degrees2Radians;
	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = 0.5 * (mpr_west + mpr_east) * Degrees2Radians + i * deltaAngle;
		sinLon[i] = angle;
		cosLon[i] = angle;
	}

	deltaAngle = -scaleFactor * mpr_latitude_span * Degrees2Radians;
	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = mpr_north * Degrees2Radians + i * deltaAngle;
		glbDouble sinLat = sin(angle);
		glbDouble radCosLat = cos(angle) * layerRadius;

		for (glbInt32 j = 0; j < thisVertexCountPlus1; j++)
		{		
			geoheight = GetHeightData(_pDemdata, j, i, GLB_NORTHEAST)*exaggrate;
			if (_minAlt>geoheight)
				_minAlt = geoheight;
			if (_maxAlt<geoheight)
				_maxAlt = geoheight;

			if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
				g_ellipsmodel->convertLatLongHeightToXYZ(
					angle , cosLon[j], geoheight/*0.0*/,
					geocentric.x(), geocentric.y(), geocentric.z() );
				//northEastVertices->push_back(geocentric);
				northEastVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));

				geotexcoord.set((j + thisVertexCount) * scaleFactor ,i * scaleFactor );
				northEastToords->push_back(geotexcoord);
				geocentric.normalize();
				northEastNormals->push_back(osg::Vec3f(geocentric));
				// 记录一下
				geocentric.set(angle,cosLon[j],geoheight);
			}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
				geocentric.set( 0.5 * (mpr_west + mpr_east) + j * deltaX, mpr_north + i * deltaY, geoheight);				
				northEastVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));
				geotexcoord.set((j + thisVertexCount) * scaleFactor ,i * scaleFactor );
				northEastToords->push_back(geotexcoord);
				northEastNormals->push_back(osg::Vec3f(0,0,1));
				// 记录一下
				geocentric.set( mpr_north + i * deltaY, 0.5 * (mpr_west + mpr_east) + j * deltaX,  geoheight);
			}

			if(i == 0){
				//mpr_p_north_easthalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));
				p_north_easthalf_vertexarray->push_back(geocentric);
				p_north_easthalf_texcoords->push_back(geotexcoord);
			}
			if(j == thisVertexCountPlus1-1){
				//mpr_p_east_northhalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));
				p_east_northhalf_vertexarray->push_back(geocentric);
				p_east_northhalf_texcoords->push_back(geotexcoord);
			}
			if(j == 0)
			{
				//mpr_p_center_northhalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));
				p_center_northhalf_vertexarray->push_back(geocentric);
			}
		}
	}

	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = 0.5 * (mpr_north + mpr_south) * Degrees2Radians + i * deltaAngle;
		glbDouble sinLat = sin(angle);
		glbDouble radCosLat = cos(angle) * layerRadius;

		for (glbInt32 j = 0; j < thisVertexCountPlus1; j++)
		{			
			geoheight = GetHeightData(_pDemdata, j, i, GLB_SOUTHEAST)*exaggrate;
			if (_minAlt>geoheight)
				_minAlt = geoheight;
			if (_maxAlt<geoheight)
				_maxAlt = geoheight;

			if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
				g_ellipsmodel->convertLatLongHeightToXYZ(
					angle , cosLon[j], geoheight/*0.0*/,
					geocentric.x(), geocentric.y(), geocentric.z() );
				//southEastVertices->push_back(geocentric);
				southEastVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));	

				geotexcoord.set((j + thisVertexCount) * scaleFactor , (i + thisVertexCount) * scaleFactor);
				//geotexcoord.set(u_offset + (j + thisVertexCount) * scaleFactor * u_scale, v_offset + (i + thisVertexCount) * scaleFactor * v_scale);
				//southEastToords->push_back(osg::Vec2((j + thisVertexCount) * scaleFactor , (i + thisVertexCount) * scaleFactor));
				southEastToords->push_back( geotexcoord );
				geocentric.normalize();
				southEastNormals->push_back(osg::Vec3f(geocentric));
				// 记录一下
				geocentric.set(angle,cosLon[j],geoheight);
			}else if( mpr_globe_type == GLB_GLOBETYPE_FLAT){
				geocentric.set( 0.5 * (mpr_west + mpr_east) + j * deltaX, 0.5 * (mpr_north+mpr_south) + i * deltaY, geoheight);				
				southEastVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));
				geotexcoord.set( (j + thisVertexCount) * scaleFactor , (i + thisVertexCount) * scaleFactor );
				southEastToords->push_back( geotexcoord );			
				southEastNormals->push_back(osg::Vec3f(0,0,1));
				// 记录一下
				geocentric.set( 0.5 * (mpr_north+mpr_south) + i * deltaY, 0.5 * (mpr_west + mpr_east) + j * deltaX,  geoheight);
			}

			if(i == thisVertexCountPlus1-1){
				//mpr_p_south_easthalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));
				p_south_easthalf_vertexarray->push_back(geocentric);
				p_south_easthalf_texcoords->push_back(geotexcoord);
			}
			if(j == thisVertexCountPlus1-1){
				//mpr_p_east_southhalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));
				p_east_southhalf_vertexarray->push_back(geocentric);
				p_east_southhalf_texcoords->push_back(geotexcoord);
			}
			if (i == 0){
				//mpr_p_center_easthalf_vertexarray->push_back(osg::Vec3d(angle,cosLon[j],geoheight));	
				p_center_easthalf_vertexarray->push_back(geocentric);		
			}
		}
	}

	// 更新高度的最大最小值
	mpr_minalt = _minAlt;
	mpr_maxalt = _maxAlt;
	if (mpr_extent)
	{// 更新外包
		mpr_extent->Set(mpr_west,mpr_east,mpr_south,mpr_north,mpr_minalt,mpr_maxalt);
	}
	// 重新计算boundbox
	ComputeBoundBox();

	// 释放内存空间
	if (sinLon)	delete[]sinLon;
	if (cosLon)	delete[] cosLon;

	mpr_p_north_easthalf_vertexarray = p_north_easthalf_vertexarray;
	mpr_p_north_westhalf_vertexarray = p_north_westhalf_vertexarray;
	mpr_p_south_easthalf_vertexarray = p_south_easthalf_vertexarray;
	mpr_p_south_westhalf_vertexarray = p_south_westhalf_vertexarray;
	mpr_p_east_northhalf_vertexarray = p_east_northhalf_vertexarray;
	mpr_p_east_southhalf_vertexarray = p_east_southhalf_vertexarray;
	mpr_p_west_northhalf_vertexarray = p_west_northhalf_vertexarray;
	mpr_p_west_southhalf_vertexarray = p_west_southhalf_vertexarray;

	mpr_p_center_westhalf_vertexarray = p_center_westhalf_vertexarray;
	mpr_p_center_easthalf_vertexarray = p_center_easthalf_vertexarray;
	mpr_p_center_northhalf_vertexarray = p_center_northhalf_vertexarray;
	mpr_p_center_southhalf_vertexarray = p_center_southhalf_vertexarray;

	mpr_p_north_easthalf_texcoords = p_north_easthalf_texcoords;
	mpr_p_north_westhalf_texcoords = p_north_westhalf_texcoords;
	mpr_p_south_easthalf_texcoords = p_south_easthalf_texcoords;
	mpr_p_south_westhalf_texcoords = p_south_westhalf_texcoords;
	mpr_p_east_northhalf_texcoords = p_east_northhalf_texcoords;
	mpr_p_east_southhalf_texcoords = p_east_southhalf_texcoords;
	mpr_p_west_northhalf_texcoords = p_west_northhalf_texcoords;
	mpr_p_west_southhalf_texcoords = p_west_southhalf_texcoords;

	mpr_p_northWestgeom = new osg::Geometry();
	mpr_p_southWestgeom = new osg::Geometry();
	mpr_p_northEastgeom = new osg::Geometry();
	mpr_p_southEastgeom = new osg::Geometry();
	//mpr_p_northWestgeom->setDataVariance(osg::Object::DYNAMIC);
	//mpr_p_southWestgeom->setDataVariance(osg::Object::DYNAMIC);
	//mpr_p_northEastgeom->setDataVariance(osg::Object::DYNAMIC);
	//mpr_p_southEastgeom->setDataVariance(osg::Object::DYNAMIC);

#if 0
	for (glbInt32 row = 0; row < thisVertexCountPlus1-1; row++)
	{	
		osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
		pIndexArray->reserve((thisVertexCountPlus1-1)*2);
		for (glbInt32 col = 0; col < thisVertexCountPlus1; col++)
		{
			pIndexArray->push_back(row*thisVertexCountPlus1+col);
			pIndexArray->push_back((row+1)*thisVertexCountPlus1+col);
		}
		osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,pIndexArray->size(),&pIndexArray->front());
		mpr_p_northWestgeom->addPrimitiveSet(drawElements);
		mpr_p_southWestgeom->addPrimitiveSet(drawElements);
		mpr_p_northEastgeom->addPrimitiveSet(drawElements);
		mpr_p_southEastgeom->addPrimitiveSet(drawElements);
	}
#else
	osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
	pIndexArray->reserve((thisVertexCountPlus1-1)*thisVertexCountPlus1*2);
	for (glbInt32 row = 0; row < thisVertexCountPlus1-1; row++)
	{		
		if (row%2==0)
		{
			for (glbInt32 col = 0; col < thisVertexCountPlus1; col++)
			{
				pIndexArray->push_back(row*thisVertexCountPlus1+col);
				pIndexArray->push_back((row+1)*thisVertexCountPlus1+col);
			}
			//pIndexArray->push_back(row*thisVertexCountPlus1+thisVertexCountPlus1-1);
			//pIndexArray->push_back((row+1)*thisVertexCountPlus1+thisVertexCountPlus1-1);

			pIndexArray->push_back((row+1)*thisVertexCountPlus1+thisVertexCountPlus1-1);
			pIndexArray->push_back((row+1)*thisVertexCountPlus1+thisVertexCountPlus1-1);
		}
		else
		{
			for (glbInt32 col = thisVertexCountPlus1-1; col >=0 ; col--)
			{			
				pIndexArray->push_back((row+1)*thisVertexCountPlus1+col);
				pIndexArray->push_back(row*thisVertexCountPlus1+col);												
			}
			//pIndexArray->push_back((row+1)*thisVertexCountPlus1);
			//pIndexArray->push_back(row*thisVertexCountPlus1);		
			pIndexArray->push_back((row+1)*thisVertexCountPlus1);		
			pIndexArray->push_back((row+1)*thisVertexCountPlus1);	
		}		
	}
	osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,pIndexArray->size(),&pIndexArray->front());
	mpr_p_northWestgeom->addPrimitiveSet(drawElements);
	mpr_p_southWestgeom->addPrimitiveSet(drawElements);
	mpr_p_northEastgeom->addPrimitiveSet(drawElements);
	mpr_p_southEastgeom->addPrimitiveSet(drawElements);
#endif

	mpr_p_northWestgeom->setName("northwestgeom");
	mpr_p_southWestgeom->setName("southwestgeom");
	mpr_p_northEastgeom->setName("northeastgeom");
	mpr_p_southEastgeom->setName("southeastgeom");

	mpr_p_northWestgeom->setVertexArray(northWestVertices.get());
	mpr_p_northWestgeom->setTexCoordArray(0,northWestToords.get());
	mpr_p_northWestgeom->setNormalArray(northWestNormals.get());
	mpr_p_northWestgeom->setColorArray(colorArray.get());
	mpr_p_northWestgeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	mpr_p_northEastgeom->setVertexArray(northEastVertices.get());
	mpr_p_northEastgeom->setTexCoordArray(0,northEastToords.get());
	mpr_p_northEastgeom->setNormalArray(northEastNormals.get());
	mpr_p_northEastgeom->setColorArray(colorArray.get());
	mpr_p_northEastgeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	mpr_p_southWestgeom->setVertexArray(southWestVertices.get());
	mpr_p_southWestgeom->setTexCoordArray(0,southWestToords.get());
	mpr_p_southWestgeom->setNormalArray(southWestNormals.get());
	mpr_p_southWestgeom->setColorArray(colorArray.get());
	mpr_p_southWestgeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	mpr_p_southEastgeom->setVertexArray(southEastVertices.get());
	mpr_p_southEastgeom->setTexCoordArray(0,southEastToords.get());
	mpr_p_southEastgeom->setNormalArray(southEastNormals.get());
	mpr_p_southEastgeom->setColorArray(colorArray.get());
	mpr_p_southEastgeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	if (mpr_p_osgImg)
	{
		mpr_texture = new osg::Texture2D;		
		mpr_texture->setImage(mpr_p_osgImg.get());

		mpr_texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR);
		mpr_texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
		//mpr_p_texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
		mpr_texture->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
		mpr_texture->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);

		mpr_texture->setMaxAnisotropy(8.0);

		mpr_texture->setResizeNonPowerOfTwoHint(true);
		mpr_p_northWestgeom->getOrCreateStateSet()->setTextureAttributeAndModes(0,mpr_texture.get());	
		mpr_p_southWestgeom->getOrCreateStateSet()->setTextureAttributeAndModes(0,mpr_texture.get());	
		mpr_p_northEastgeom->getOrCreateStateSet()->setTextureAttributeAndModes(0,mpr_texture.get());	
		mpr_p_southEastgeom->getOrCreateStateSet()->setTextureAttributeAndModes(0,mpr_texture.get());	
	}	

	mpr_p_geode = new osg::Geode();
	mpr_p_geode->addDrawable(mpr_p_northWestgeom.get());
	mpr_p_geode->addDrawable(mpr_p_southWestgeom.get());
	mpr_p_geode->addDrawable(mpr_p_northEastgeom.get());
	mpr_p_geode->addDrawable(mpr_p_southEastgeom.get());	

	{// 测试 ，在块的中心偏左边界位置（左边界起始绘制）输出level，row, column		
		/*
		if (mpr_level >= 0)
		{			
			float geoheight = GetHeightData(_pDemdata, 0, 0, GLB_SOUTHEAST)*exaggrate + 5;
			osg::Vec3d local;
			if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsmodel->convertLatLongHeightToXYZ(
					mpr_center_latitude , mpr_center_longitude,geoheight,
					local.x(), local.y(), local.z() );
			}
			else if( mpr_globe_type==GLB_GLOBETYPE_FLAT)
			{
				local.set(0.5*(mpr_west+mpr_east), 0.5*(mpr_south+mpr_north),geoheight+3.0);
			}

			osg::ref_ptr<osgText::Text> p_text = new osgText::Text;

			p_text->setColor(osg::Vec4(0,1.0f,1.0f,1.0f));
			wchar_t strTitle[64];
			wsprintf(strTitle,L"l:%d,r:%d,c:%d",mpr_level,(long)mpr_row,(long)mpr_column);

			osg::ref_ptr<osgText::Font> font = osgText::readFontFile("simkai.ttf");
			p_text->setFont(font.get());
			p_text->setText(strTitle);
			p_text->setFontResolution(10,10); 
			p_text->setPosition(local-mpr_local_origin);
			p_text->setAxisAlignment(osgText::Text::SCREEN);
			p_text->setCharacterSizeMode(osgText::Text::CharacterSizeMode::SCREEN_COORDS);
			//p_text->setCharacterSizeMode(osgText::Text::OBJECT_COORDS);
			p_text->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);
			p_text->setCharacterSize(20);	
			p_text->setAlignment(osgText::Text::LEFT_BOTTOM);	

			//p_text->setDrawMode(osgText::Text::TEXT|osgText::Text::ALIGNMENT|osgText::Text::BOUNDINGBOX);
			mpr_p_geode->addDrawable(p_text.get());
		}//*/
	}

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
	osg::Matrixd matrixlocalOrigin;
	matrixlocalOrigin.makeTranslate(mpr_local_origin);
	mt->setMatrix(matrixlocalOrigin);
	mt->addChild(mpr_p_geode.get());

	osg::ref_ptr<osg::Group> grp = new osg::Group();
	grp->addChild(mt.get());
	// 添加节点剪裁回调
	//if (mpr_level>0)
	{		
		//grp->setCullCallback(new CGlbGlobeTerrainTileCullCallBack(mpr_boundbox,mpr_level));
	}
	return grp;
}

void CGlbGlobeTerrainTile::Dispose(glbBool dispos_direct ,glbBool dispose_level_zero_tile)
{
	//if (false)
	//{
	//	char buff[256];
	//	sprintf(buff,"****Tile Dispose. [Level: %d , row: %d, col: %d].",mpr_level,(long)mpr_row,(long)mpr_column);
	//	LOG(buff);
	//}

	if (mpr_nechild != NULL)
	{
		mpr_nechild->Dispose();
		//delete mpr_p_northeast_childtile;
		mpr_nechild = NULL;
	}
	if (mpr_nwchild != NULL)
	{		
		mpr_nwchild->Dispose();
		//delete mpr_p_northwest_childtile;
		mpr_nwchild = NULL;
	}
	if (mpr_sechild != NULL)
	{	
		mpr_sechild->Dispose();
		//delete mpr_p_southeast_childtile;
		mpr_sechild = NULL;
	}
	if (mpr_swchild != NULL)
	{
		mpr_swchild->Dispose();
		//delete mpr_p_southwest_childtile;
		mpr_swchild = NULL;
	}

	if (mpr_terrainTask != NULL)
	{
		mpr_terrain->mpr_globe->mpr_taskmanager->ReomveTerrainTask(mpr_terrainTask.get());
		mpr_terrainTask = NULL;
	}	

	if (dispose_level_zero_tile)
	{// 可以删除0级的tile的node
		if (dispos_direct)
		{
			osg::Group* p_tnode = dynamic_cast<osg::Group*>(mpr_terrain->GetNode());
			if (p_tnode)
				p_tnode->removeChild(mpr_node.get());

		}
		else
		{
			CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_NODE,mpr_node.get());
			mpr_terrain->AddOperation(_opera);
		}
		mpr_isinitialized = false;
	}
	else
	{
		if (mpr_level > 0)
		{
			if (mpr_isinitialized)
			{
				if (dispos_direct)
				{
					osg::Group* p_tnode = dynamic_cast<osg::Group*>(mpr_terrain->GetNode());
					if (p_tnode)
						p_tnode->removeChild(mpr_node.get());
				}
				else
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_NODE,mpr_node.get());
					mpr_terrain->AddOperation(_opera);
					//#ifdef _DEBUG
					//					WCHAR wBuff[128];
					//					wsprintf(wBuff,L"	Remove Tile. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
					//					OutputDebugString(wBuff);
					//#endif
				}
			}

			mpr_isinitialized = false;
		}
		else
		{
			if (mpr_p_geode)
			{
				if (mpr_p_geode->containsDrawable(mpr_p_northEastgeom.get())==false)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northEastgeom.get());
					mpr_terrain->AddOperation(_opera);					
				}

				if (mpr_p_geode->containsDrawable(mpr_p_northWestgeom.get())==false)
				{// add drawabl	
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_northWestgeom.get());
					mpr_terrain->AddOperation(_opera);		
				}

				if (mpr_p_geode->containsDrawable(mpr_p_southEastgeom.get())==false)
				{// add drawabl	
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southEastgeom.get());
					mpr_terrain->AddOperation(_opera);		
				}

				if (mpr_p_geode->containsDrawable(mpr_p_southWestgeom.get())==false)
				{// add drawabl
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_southWestgeom.get());
					mpr_terrain->AddOperation(_opera);		
				}				
			}			
		}
	}
}
void CGlbGlobeTerrainTile::RefreshTexture(	std::vector<CGlbExtent>						&objDirtyExts,
											std::vector<CGlbExtent>						&domDirtyExts,
											std::vector<glbref_ptr<CGlbGlobeDomLayer>>  &domlayers,                   
											std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainObjs)
{
	if (mpr_isDataLoaded==false)
		return;
	//1. 分别计算objDirtyExts和domDirtyExts区域与当前tile范围是否有交集
	glbBool _tobjDirty=false;
	glbBool _domlayerDirty = false;
	_tobjDirty = objDirtyExts.size()>0 ? true : false;
	_domlayerDirty = domDirtyExts.size()>0 ? true : false;

	//glbDouble minX,maxX,minY,maxY;
	//std::vector<CGlbExtent>::iterator itr = domDirtyExts.begin();
	//while(itr!=domDirtyExts.end())
	//{
	//	itr->Get(&minX,&maxX,&minY,&maxY);
	//	glbDouble mx = max(minX,mpr_west);
	//	glbDouble Mx = min(maxX,mpr_east);
	//	glbDouble my = max(minY,mpr_south);
	//	glbDouble My = min(maxY,mpr_north);

	//	if ((mx < Mx) && (my < My))
	//	{
	//		_domlayerDirty = true;
	//		break;
	//	}
	//	itr++;
	//}

	//itr = objDirtyExts.begin();
	//while(itr!=objDirtyExts.end())
	//{
	//	itr->Get(&minX,&maxX,&minY,&maxY);
	//	glbDouble mx = max(minX,mpr_west);
	//	glbDouble Mx = min(maxX,mpr_east);
	//	glbDouble my = max(minY,mpr_south);
	//	glbDouble My = min(maxY,mpr_north);

	//	if ((mx < Mx) && (my < My))
	//	{
	//		_tobjDirty = true;	
	//		break;
	//	}		
	//	itr++;
	//}

	if (_tobjDirty || _domlayerDirty)
	{// 刷新区域与地形块区域有交集

		// 1. 递归刷新子块
		glbref_ptr<CGlbGlobeTerrainTile> p_northwestchild = GetNorthWestChild();
		if (p_northwestchild )
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_northwestchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_northwestchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_northwestchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);
		}

		glbref_ptr<CGlbGlobeTerrainTile> p_northeastchild = GetNorthEastChild();
		if (p_northeastchild )
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_northeastchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_northeastchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_northeastchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);
		}

		glbref_ptr<CGlbGlobeTerrainTile> p_southwestchild = GetSouthWestChild();	
		if (p_southwestchild)
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_southwestchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_southwestchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_southwestchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);
		}

		glbref_ptr<CGlbGlobeTerrainTile> p_southeastchild = GetSouthEastChild();
		if (p_southeastchild )
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_southeastchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_southeastchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_southeastchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);	
		}

		// 2. 创建刷新任务，重新生成mpr_texutre
		//CGlbGlobeRefreshTileTextureTask* task = new CGlbGlobeRefreshTileTextureTask(this,domlayers,terrainObjs,_domlayerDirty);
		//mpr_terrain->mpr_globe->mpr_taskmanager->AddTerrainTask(task);
		LoadDomData(domlayers,terrainObjs,_domlayerDirty);
	}
}

void CGlbGlobeTerrainTile::LoadDomData(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
										std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs,
										glbBool domrefresh)
{
	if (mpr_p_osgImg==NULL) return;

	unsigned int imagesize = mpr_p_osgImg->getImageSizeInBytes();
	//1. 如果需要刷新domlayer，则重新生成mpr_overlayimage	
	if (domrefresh)
	{
		// 重新填充mpr_overlayImage的内容
		if (!mpr_overlayImage)		
			mpr_overlayImage = new glbByte[imagesize];
		memcpy(mpr_overlayImage,mpr_image,imagesize);	

		std::vector<glbref_ptr<CGlbGlobeDomLayer>>::iterator _itr = domlayers.begin();
		while(_itr != domlayers.end())
		{
			CGlbGlobeDomLayer* p_domlayer = dynamic_cast<CGlbGlobeDomLayer*>((*_itr).get());
			if (p_domlayer && p_domlayer->IsShow())
			{
				glbBool bInter = IsDomlayerInterTile(p_domlayer);
				if (!bInter)
				{// 没有交集，转入下一个图层
					_itr++;
					continue;
				}

				glbBool bHasPyramid = p_domlayer->HasPyramid();			
				glbInt32 opacity = p_domlayer->GetOpacity();
				CGlbPixelBlock* p_pixelblock=NULL;
				p_domlayer->CreatePixelBlock(&p_pixelblock);
				glbBool bsucc =false;			
				if (bHasPyramid)
				{// 有金子塔	
					glbInt32 minLevel,maxLevel;
					p_domlayer->GetPyramidLevels(&minLevel,&maxLevel);
					if (mpr_level>maxLevel)
					{// tile等级比domlayer的最大等级都大					
						bsucc = InterpolateDomLayerData(p_domlayer,maxLevel,p_pixelblock);
					}
					else// 加载跟当前地形块tile等级相同的，行列号则通过范围来确定
					{
						bsucc = p_domlayer->ReadData(mpr_level,mpr_row,mpr_column,p_pixelblock);
						//if(mpr_level == 9 && mpr_row == 53775 && mpr_column == 542667)
						//{
						//	CGlbWString path = L"D:/test1.tiff";
						//	p_pixelblock->ExportToTiff(path.c_str());
						//}
						//if(mpr_level == 9 && mpr_row == 53774 && mpr_column == 542667)
						//{
						//	CGlbWString path = L"D:/test2.tiff";
						//	p_pixelblock->ExportToTiff(path.c_str());
						//}
					}
				}
				else
				{// 没有金字塔
					bsucc = p_domlayer->ReadDataEx(mpr_extent.get(),p_pixelblock);
				}

				// 将p_pixelblock数据+mpr_image -> mpr_overlayImage
				if (bsucc)
				{// agg进行转换,将一个layer的image混合到tile的源image上
					unsigned int imagesize = mpr_p_osgImg->getImageSizeInBytes();											
					memcpy(mpr_overlayImage,mpr_image,imagesize);
					// agg混合p_pixelblock的数据到mpr_overlayImage上				
					glbByte* p_blockdata=NULL;
					p_pixelblock->GetPixelData(&p_blockdata);
					// 修改BlendImage使其适应RGBA,RGB,LUMINNT,LUMNIT_ALPHA
					glbBool res = BlendImage(mpr_overlayImage,p_blockdata,mpr_imagesize,mpr_imagesize,opacity,true);
				}

				if (p_pixelblock)
					delete [] p_pixelblock;	
			}
			++_itr;
		}
	}
	// 2. 拷贝mpr_overlayImage或mpr_image到mpr_p_osgImg中
	osg::Image* p_newOsgImg = new osg::Image;
	p_newOsgImg->allocateImage(mpr_p_osgImg->s(),mpr_p_osgImg->t(),mpr_p_osgImg->r(),mpr_p_osgImg->getPixelFormat(),GL_UNSIGNED_BYTE);

	if (mpr_overlayImage)
		memcpy(p_newOsgImg->data(),mpr_overlayImage,imagesize);	
	else if (mpr_image)
		memcpy(p_newOsgImg->data(),mpr_image,imagesize);	

	//3. 遍历terrainobjs，在mpr_overlayImage上叠加矢量对象， 生成_pFinalImage
	std::vector<glbref_ptr<CGlbGlobeRObject>>::iterator _itrobj = terrainobjs.begin();
	while(_itrobj!=terrainobjs.end())
	{
		(*_itrobj)->DrawToImage(p_newOsgImg->data(),p_newOsgImg->s(),p_newOsgImg->t(),*mpr_extent.get());	
		++_itrobj;
	}

	//4. 将mpr_p_osgImg设置给mpr_textrue,刷新纹理
	//mpr_p_osgImg->dirty();
	//if (mpr_texture)
	//{
	//	mpr_texture->setImage(0,mpr_p_osgImg);		
	//	mpr_texture->dirtyTextureObject();
	//}	

	//{
	//	static int num = 0;
	//	if (mpr_level == 0 && mpr_row == 1 && mpr_column==8)
	//	{
	//		char buff[128];
	//		sprintf(buff,"image%d",num);
	//		std::string strName = std::string("f:\\1\\") + buff + std::string(".tif");

	//		osgDB::writeImageFile(*mpr_p_osgImg.get(),strName);
	//		num++;
	//	}
	//}
	CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_TEXTURE,mpr_texture.get(),p_newOsgImg);
	mpr_terrain->AddOperation(_opera);

	mpr_p_osgImg = p_newOsgImg;
}

void CGlbGlobeTerrainTile::RefreshDem(	std::vector<CGlbExtent>						&mtobjDirtyExts,
										std::vector<CGlbExtent>						&demDirtyExts,
										std::vector<glbref_ptr<CGlbGlobeDemLayer>>  &demlayers,                   
										std::vector<glbref_ptr<CGlbGlobeRObject>>	&mterrainObjs
										/*CGlbGlobeTerrain::GlbTileOperationPackage	&operationPackage*/)
{
	if (!mpr_isinitialized)
	{// 没有初始化清空接边所产生的边界顶点数组
		mpr_p_north_easthalf_vertexarray=NULL;
		mpr_p_north_westhalf_vertexarray=NULL;
		mpr_p_south_easthalf_vertexarray=NULL;
		mpr_p_south_westhalf_vertexarray=NULL;
		mpr_p_east_northhalf_vertexarray=NULL;
		mpr_p_east_southhalf_vertexarray=NULL;
		mpr_p_west_northhalf_vertexarray=NULL;
		mpr_p_west_southhalf_vertexarray=NULL;
		mpr_p_center_westhalf_vertexarray=NULL;
		mpr_p_center_easthalf_vertexarray=NULL;
		mpr_p_center_northhalf_vertexarray=NULL;
		mpr_p_center_southhalf_vertexarray=NULL;		

		if (mpr_isDataLoaded)
		{// 如果此时已经加载完dem数据，更新dem数据
			glbBool bSucc = LoadOverlayDemData(demlayers,mterrainObjs);
			return;
		}
		return;
	}

	if (mpr_isDataLoaded==false)
	{// 如果没有加载数据，返回
		return;
	}
	
	//1. 分别计算objDirtyExts和domDirtyExts区域与当前tile范围是否有交集
	glbBool _mtobjDirty=false;
	glbBool _demlayerDirty = false;

	glbDouble minX,maxX,minY,maxY;
	std::vector<CGlbExtent>::iterator itr = demDirtyExts.begin();
	while(itr!=demDirtyExts.end())
	{
		itr->Get(&minX,&maxX,&minY,&maxY);
		glbDouble mx = max(minX,mpr_west);
		glbDouble Mx = min(maxX,mpr_east);
		glbDouble my = max(minY,mpr_south);
		glbDouble My = min(maxY,mpr_north);

		if ((mx <= Mx) && (my <= My))
		{
			_demlayerDirty = true;	break;
		}
		itr++;
	}

	itr = mtobjDirtyExts.begin();
	while(itr!=mtobjDirtyExts.end())
	{
		itr->Get(&minX,&maxX,&minY,&maxY);
		glbDouble mx = max(minX,mpr_west);
		glbDouble Mx = min(maxX,mpr_east);
		glbDouble my = max(minY,mpr_south);
		glbDouble My = min(maxY,mpr_north);

		if ((mx <= Mx) && (my <= My))
		{
			_mtobjDirty = true;	break;
		}		
		itr++;
	}

	if (_mtobjDirty || _demlayerDirty)
	{// 刷新区域与地形块区域有交集 - 自下而上刷新
		LoadDemData(demlayers,mterrainObjs);

		// 1. 递归刷新子块
		glbref_ptr<CGlbGlobeTerrainTile> p_northwestchild = GetNorthWestChild();
		if (p_northwestchild)
			p_northwestchild->RefreshDem(mtobjDirtyExts,demDirtyExts,demlayers,mterrainObjs);	

		glbref_ptr<CGlbGlobeTerrainTile> p_northeastchild = GetNorthEastChild();
		if (p_northeastchild)
			p_northeastchild->RefreshDem(mtobjDirtyExts,demDirtyExts,demlayers,mterrainObjs);	

		glbref_ptr<CGlbGlobeTerrainTile> p_southwestchild = GetSouthWestChild();	
		if (p_southwestchild)
			p_southwestchild->RefreshDem(mtobjDirtyExts,demDirtyExts,demlayers,mterrainObjs);	

		glbref_ptr<CGlbGlobeTerrainTile> p_southeastchild = GetSouthEastChild();
		if (p_southeastchild)
			p_southeastchild->RefreshDem(mtobjDirtyExts,demDirtyExts,demlayers,mterrainObjs);			

		// 2. 创建刷新任务，重新生成mpr_texutre
		//CGlbGlobeRefreshTileDemTask* task = new CGlbGlobeRefreshTileDemTask(this,demlayers,mterrainObjs);
		//mpr_terrain->mpr_globe->mpr_taskmanager->AddTerrainTask(task);

		//LoadDemData(demlayers,mterrainObjs);
	}
}

void CGlbGlobeTerrainTile::RebuildElevatedMesh()
{
	//1. 更新mpr_p_northWestgeom,mpr_p_northEastgeom,mpr_p_southEastgeom,mpr_p_southWestgeom 4个geometry的顶点数组
	osg::ref_ptr<osg::Vec3Array> northWestVertices = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> southWestVertices = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> northEastVertices = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> southEastVertices = new osg::Vec3Array();

	glbDouble layerRadius = osg::WGS_84_RADIUS_EQUATOR;
	glbDouble scaleFactor = 1.0 / (glbDouble)mpr_vertex_count;
	glbInt32 thisVertexCount = mpr_vertex_count / 2 + (mpr_vertex_count % 2);
	glbInt32 thisVertexCountPlus1 = (thisVertexCount%2 == 0) ? thisVertexCount + 1 : thisVertexCount;

	glbInt32 totalVertexCount = thisVertexCountPlus1 * thisVertexCountPlus1;
	const glbDouble Degrees2Radians = osg::PI / 180.0;

	northWestVertices->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	southWestVertices->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	northEastVertices->reserve(thisVertexCountPlus1*thisVertexCountPlus1);
	southEastVertices->reserve(thisVertexCountPlus1*thisVertexCountPlus1);

	// Cache western sin/cos of longitude values
	glbDouble* sinLon = new glbDouble[thisVertexCountPlus1];
	glbDouble* cosLon = new glbDouble[thisVertexCountPlus1];

	glbInt32 i = 0;
	glbDouble angle = mpr_west * Degrees2Radians;
	glbDouble deltaAngle = scaleFactor * mpr_longitude_span * Degrees2Radians;	
	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = mpr_west * Degrees2Radians + i * deltaAngle;
		sinLon[i] = angle;
		cosLon[i] = angle;
	}
	glbFloat geoheight;
	glbDouble deltaX = scaleFactor * (mpr_east - mpr_west);
	glbDouble deltaY = -scaleFactor * (mpr_north - mpr_south);

	glbDouble _minAlt = 999999999.9;
	glbDouble _maxAlt = -999999999.9;

	// 清空旧的边界数据
	mpr_p_north_easthalf_vertexarray->clear();
	mpr_p_north_westhalf_vertexarray->clear();
	mpr_p_south_easthalf_vertexarray->clear();
	mpr_p_south_westhalf_vertexarray->clear();
	mpr_p_east_northhalf_vertexarray->clear();
	mpr_p_east_southhalf_vertexarray->clear();
	mpr_p_west_northhalf_vertexarray->clear();
	mpr_p_west_southhalf_vertexarray->clear();
	mpr_p_center_westhalf_vertexarray->clear();
	mpr_p_center_easthalf_vertexarray->clear();
	mpr_p_center_northhalf_vertexarray->clear();
	mpr_p_center_southhalf_vertexarray->clear();

	glbFloat* _pDemdata = NULL;
	if (mpr_overlaydemdata)
		_pDemdata = mpr_overlaydemdata;
	else
		_pDemdata = mpr_demdata;

	glbDouble exaggrate = mpr_terrain->GetExaggrate();

	//glbFloat _geoheight = GetHeightData(_pDemdata, thisVertexCountPlus1-1, thisVertexCountPlus1-1, GLB_NORTHWEST) * exaggrate;
	//if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
	//	g_ellipsmodel->convertLatLongHeightToXYZ(
	//		mpr_center_latitude , mpr_center_longitude,_geoheight,//0.0,
	//		mpr_local_origin.x(), mpr_local_origin.y(), mpr_local_origin.z() );
	//}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
	//	mpr_local_origin.set(0.5*(mpr_west+mpr_east), 0.5*(mpr_south+mpr_north),_geoheight);
	//}	

	deltaAngle = -scaleFactor * mpr_latitude_span * Degrees2Radians;
	osg::Vec3d geocentric;
	osg::Vec2 geotexcoord;
	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = mpr_north * Degrees2Radians + i * deltaAngle;
		glbDouble sinLat = sin(angle);
		glbDouble radCosLat = cos(angle) * layerRadius;		
		for (glbInt32 j = 0; j < thisVertexCountPlus1; j++)
		{			
			geoheight = GetHeightData(_pDemdata, j, i, GLB_NORTHWEST) * exaggrate;
			if (_minAlt>geoheight)
				_minAlt = geoheight;
			if (_maxAlt<geoheight)
				_maxAlt = geoheight;

			if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
				g_ellipsmodel->convertLatLongHeightToXYZ(
					angle , cosLon[j], geoheight/*0.0*/,
					geocentric.x(), geocentric.y(), geocentric.z() );
				northWestVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));			
				// 记录一下
				geocentric.set(angle,cosLon[j],geoheight);
			}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
				geocentric.set( mpr_west + j * deltaX, mpr_north + i * deltaY, geoheight);				
				northWestVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));	
				// 记录一下
				geocentric.set( mpr_north + i * deltaY, mpr_west + j * deltaX,  geoheight);
			}

			if (i == 0){
				mpr_p_north_westhalf_vertexarray->push_back(geocentric);
			}
			if (j == 0){
				mpr_p_west_northhalf_vertexarray->push_back(geocentric);
			}	
			if ( i == thisVertexCountPlus1-1){
				mpr_p_center_westhalf_vertexarray->push_back(geocentric);		
			}
		}
	}
	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = 0.5 * (mpr_north + mpr_south) * Degrees2Radians + i * deltaAngle;
		glbDouble sinLat = sin(angle);
		glbDouble radCosLat = cos(angle) * layerRadius;

		for (glbInt32 j = 0; j < thisVertexCountPlus1; j++)
		{	
			geoheight = GetHeightData(_pDemdata, j, i, GlB_SOUTHWEST) * exaggrate;
			if (_minAlt>geoheight)
				_minAlt = geoheight;
			if (_maxAlt<geoheight)
				_maxAlt = geoheight;

			if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
				g_ellipsmodel->convertLatLongHeightToXYZ(
					angle , cosLon[j], geoheight/*0.0*/,
					geocentric.x(), geocentric.y(), geocentric.z() );
				//southWestVertices->push_back(geocentric);
				southWestVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));				
				// 记录一下
				geocentric.set(angle,cosLon[j],geoheight);
			}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
				geocentric.set( mpr_west + j * deltaX, 0.5*(mpr_north+mpr_south) + i * deltaY, geoheight);			
				southWestVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));
				// 记录一下
				geocentric.set(0.5*(mpr_north+mpr_south) + i * deltaY, mpr_west + j * deltaX,  geoheight);
			}

			if(i == thisVertexCountPlus1-1){
				mpr_p_south_westhalf_vertexarray->push_back(geocentric);
			}			
			if(j == 0){	
				mpr_p_west_southhalf_vertexarray->push_back(geocentric);					
			}	
			if ( j == thisVertexCountPlus1-1){
				mpr_p_center_southhalf_vertexarray->push_back(geocentric);
			}
		}
	}

	deltaAngle = scaleFactor * mpr_longitude_span * Degrees2Radians;
	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = 0.5 * (mpr_west + mpr_east) * Degrees2Radians + i * deltaAngle;
		sinLon[i] = angle;
		cosLon[i] = angle;
	}

	deltaAngle = -scaleFactor * mpr_latitude_span * Degrees2Radians;
	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = mpr_north * Degrees2Radians + i * deltaAngle;
		glbDouble sinLat = sin(angle);
		glbDouble radCosLat = cos(angle) * layerRadius;

		for (glbInt32 j = 0; j < thisVertexCountPlus1; j++)
		{		
			geoheight = GetHeightData(_pDemdata, j, i, GLB_NORTHEAST) * exaggrate;
			if (_minAlt>geoheight)
				_minAlt = geoheight;
			if (_maxAlt<geoheight)
				_maxAlt = geoheight;

			if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
				g_ellipsmodel->convertLatLongHeightToXYZ(
					angle , cosLon[j], geoheight/*0.0*/,
					geocentric.x(), geocentric.y(), geocentric.z() );
				northEastVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));				
				//记录一下
				geocentric.set(angle,cosLon[j],geoheight);
			}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
				geocentric.set( 0.5 * (mpr_west + mpr_east) + j * deltaX, mpr_north + i * deltaY, geoheight);				
				northEastVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));	
				//记录一下
				geocentric.set( mpr_north + i * deltaY, 0.5 * (mpr_west + mpr_east) + j * deltaX, geoheight);
			}

			if(i == 0){
				mpr_p_north_easthalf_vertexarray->push_back(geocentric);				
			}
			if(j == thisVertexCountPlus1-1){				
				mpr_p_east_northhalf_vertexarray->push_back(geocentric);
			}
			if(j == 0)
			{				
				mpr_p_center_northhalf_vertexarray->push_back(geocentric);
			}
		}
	}

	for (i = 0; i < thisVertexCountPlus1; i++)
	{
		angle = 0.5 * (mpr_north + mpr_south) * Degrees2Radians + i * deltaAngle;
		glbDouble sinLat = sin(angle);
		glbDouble radCosLat = cos(angle) * layerRadius;

		for (glbInt32 j = 0; j < thisVertexCountPlus1; j++)
		{			
			geoheight = GetHeightData(_pDemdata, j, i, GLB_SOUTHEAST) * exaggrate;
			if (_minAlt>geoheight)
				_minAlt = geoheight;
			if (_maxAlt<geoheight)
				_maxAlt = geoheight;

			if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
				g_ellipsmodel->convertLatLongHeightToXYZ(
					angle , cosLon[j], geoheight/*0.0*/,
					geocentric.x(), geocentric.y(), geocentric.z() );
				southEastVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));	
				// 记录一下
				geocentric.set(angle,cosLon[j],geoheight);
			}else if( mpr_globe_type == GLB_GLOBETYPE_FLAT){
				geocentric.set( 0.5 * (mpr_west + mpr_east) + j * deltaX, 0.5 * (mpr_north+mpr_south) + i * deltaY, geoheight);				
				southEastVertices->push_back(osg::Vec3f(geocentric - mpr_local_origin));		
				// 记录一下
				geocentric.set(0.5 * (mpr_north+mpr_south) + i * deltaY, 0.5 * (mpr_west + mpr_east) + j * deltaX,  geoheight);
			}

			if(i == thisVertexCountPlus1-1){
				mpr_p_south_easthalf_vertexarray->push_back(geocentric);			
			}
			if(j == thisVertexCountPlus1-1){			
				mpr_p_east_southhalf_vertexarray->push_back(geocentric);
			}
			if (i == 0){				
				mpr_p_center_easthalf_vertexarray->push_back(geocentric);		
			}
		}
	}

	// 更新高度的最大最小值
	mpr_minalt = _minAlt;
	mpr_maxalt = _maxAlt;
	if (mpr_extent)
	{// 更新外包
		mpr_extent->Set(mpr_west,mpr_east,mpr_south,mpr_north,mpr_minalt,mpr_maxalt);
	}

	// 释放内存空间
	if (sinLon)	delete [] sinLon;
	if (cosLon)	delete [] cosLon;
#if 0
	mpr_p_northWestgeom->setVertexArray(northWestVertices.get());
	mpr_p_northEastgeom->setVertexArray(northEastVertices.get());
	mpr_p_southWestgeom->setVertexArray(southWestVertices.get());
	mpr_p_southEastgeom->setVertexArray(southEastVertices.get());
#else
	// 添加修改vecarray
	CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_northWestgeom.get(),northWestVertices.get());
	mpr_terrain->AddPackageOperation(_opera);
	CGlbGlobeTerrain::GlbTileOperation _opera2(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_northEastgeom.get(),northEastVertices.get());
	mpr_terrain->AddPackageOperation(_opera2);
	CGlbGlobeTerrain::GlbTileOperation _opera3(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_southWestgeom.get(),southWestVertices.get());
	mpr_terrain->AddPackageOperation(_opera3);
	CGlbGlobeTerrain::GlbTileOperation _opera4(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_southEastgeom.get(),southEastVertices.get());
	mpr_terrain->AddPackageOperation(_opera4);
#endif
	// 行列号标注
	//for (size_t k = 0 ; k < mpr_p_geode->getNumDrawables(); k++)
	//{
	//	osgText::Text* p_text = dynamic_cast<osgText::Text*>(mpr_p_geode->getDrawable(k));
	//	if (p_text)
	//	{
	//		float geoheight = GetHeightData(_pDemdata, 0, 0, GLB_SOUTHEAST)*exaggrate + 5;
	//		osg::Vec3d local;
	//		if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	//		{
	//			g_ellipsmodel->convertLatLongHeightToXYZ(
	//				mpr_center_latitude , mpr_center_longitude,geoheight,
	//				local.x(), local.y(), local.z() );
	//		}
	//		else if( mpr_globe_type==GLB_GLOBETYPE_FLAT)
	//		{
	//			local.set(0.5*(mpr_west+mpr_east), 0.5*(mpr_south+mpr_north),geoheight+3.0);
	//		}

	//		p_text->setPosition(local-mpr_local_origin);
	//	}
	//}
	
}

void CGlbGlobeTerrainTile::LoadDemData(	std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
										std::vector<glbref_ptr<CGlbGlobeRObject>>	&mterrainobjs)
{
	//1. 加载数据并更新mpr_demdata数据
	glbBool bSucc = LoadOverlayDemData(demlayers,mterrainobjs);

	// 2. 重新生成4个子geometry
	RebuildElevatedMesh();	
}

void CGlbGlobeTerrainTile::Merge()
{
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
		MergeGlobe();
	else // if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
		MergeFlat();
}
void CGlbGlobeTerrainTile::MergeGlobe()
{
	CGlbGlobeTerrainTile* p_north_tile = NULL;
	CGlbGlobeTerrainTile* p_south_tile = NULL;
	CGlbGlobeTerrainTile* p_west_tile = NULL;
	CGlbGlobeTerrainTile* p_east_tile = NULL;
	//#ifdef _DEBUG
	//	WCHAR wBuff[128];
	//	wsprintf(wBuff,L"Merge begin. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//	CGlbSysLog::WriteLog(wBuff);
	//	_swprintf(wBuff,L"south:%.3f, north: %.3f, west: %.3f, east: %.3f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//	CGlbSysLog::WriteLog(wBuff);	
	//#endif

	if (mpr_nwchild && mpr_nwchild->IsInitialized())
	{
		mpr_nwchild->Merge();
		RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);
		RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);	
	}
	else if (mpr_isinitialized)
	{	// 首先处理north_edge_west_section
		//		获取其北边相邻的tile
		p_north_tile = GetNorthDirectionTile();
		if (p_north_tile)
		{// 只处理相邻tile极差<=1的情况
			glbInt32 north_tile_level = p_north_tile->GetLevel();
			if (p_north_tile->IsInitialized()==false)
				north_tile_level--;

			if ( (north_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 北边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;	
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-north_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_north_westhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_WESTSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_NORTHEDGE_WESTSECTION);
					if (mpr_p_north_westhalf_geom){
						mpr_p_north_westhalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_westhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}								

				if ( mpr_p_north_westhalf_geom && mpr_p_north_westhalf_geom->getName() != sname )
				{// 接边tile等级发生改变，修正接边顶点坐标 2013.2.21 竟然有这种情况，晕!!!!
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_WESTSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_NORTHEDGE_WESTSECTION);	
					mpr_p_north_westhalf_geom->setName(sname);	
				}

				if ( !bAddgeom && mpr_p_north_westhalf_geom && mpr_p_north_westhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_north_westhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_westhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}

				//{// 移除北边块p_north_tile的接边
				//	glbDouble e,w,s,n,m;
				//	p_north_tile->GetRange(e,w,s,n,m,m);
				//	glbDouble cx = (w+e)*0.5;
				//	glbDouble _centerX = (mpr_west+mpr_east)*0.5;
				//	if (cx > _centerX)
				//		p_north_tile->RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);
				//	else if(cx < _centerX)
				//		p_north_tile->RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);
				//	else if (cx == _centerX)
				//	{
				//		p_north_tile->RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);
				//		p_north_tile->RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);
				//	}
				//}
			}		
			else if (north_tile_level-1==mpr_level)
			{// 北边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);	
			}
			else if (north_tile_level == mpr_level)
			{// 北边的tile等级与本级相同[包含高于本级的情况]
				if (p_north_tile->GetSouthWestChild() && p_north_tile->GetSouthWestChild()->IsInitialized())
				{// 北边tile有西南子块，说明北边tile的实际级数要高于本块级数 : 情况B
					// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
					RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);	
				}
				else
				{// 北边tile没有西南子块，说明北边的tile在西北位置等级与本级相同 : 情况A
					// 处于"下方"位置，要考虑接边问题，符合“补边规则a”
					//osg::ref_ptr<osg::Vec3Array> p_neighbour_edgeVertexArray = new osg::Vec3Array;
					osg::Vec3dArray* p_neighbour_edgeVertexArray = NULL;
					if (IsEdgeNeedMerge(this,p_north_tile,GLB_NORTHEDGE_WESTSECTION,p_neighbour_edgeVertexArray))
					{// 根据高程信息判断是否需要补边
						if (mpr_p_geode->containsDrawable(mpr_p_north_westhalf_geom.get())==false)
						{
							BuildEdgeGeometry(p_neighbour_edgeVertexArray,GLB_NORTHEDGE_WESTSECTION);
							mpr_p_north_westhalf_geom->setName("A");
							CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_westhalf_geom.get());
							mpr_terrain->AddOperation(_opera);
						}
						else
						{
							ModifyEdgeGeometry(p_neighbour_edgeVertexArray,GLB_NORTHEDGE_WESTSECTION);	
							mpr_p_north_westhalf_geom->setName("A");
						}										
					}
					else
					{// 不需要接边
						RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);	
					}
				}	
			}
			else
			{// 不需要接边
				RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);	
			}
		}

		// 其次处理west_edge_north_section
		p_west_tile = GetWestDirectionTile();
		if (p_west_tile)
		{
			glbInt32 west_tile_level = p_west_tile->GetLevel();
			if (p_west_tile->IsInitialized()==false)
				west_tile_level--;

			if ( (west_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 西边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-west_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_west_northhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_WESTEDGE_NORTHSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_WESTEDGE_NORTHSECTION);						
					if (mpr_p_west_northhalf_geom /*&& mpr_p_west_northhalf_geom->getName()==""*/){
						mpr_p_west_northhalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_northhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}					

				if ( mpr_p_west_northhalf_geom && mpr_p_west_northhalf_geom->getName() != sname )
				{// 接边tile等级发生改变，修正接边顶点坐标 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_WESTEDGE_NORTHSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_WESTEDGE_NORTHSECTION);	
					mpr_p_west_northhalf_geom->setName(sname);	
				}
				if (!bAddgeom && mpr_p_west_northhalf_geom && mpr_p_west_northhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_west_northhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_northhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}

				//{// 移除西边块p_west_tile的接边
				//	glbDouble e,w,s,n,m;
				//	p_west_tile->GetRange(e,w,s,n,m,m);
				//	glbDouble cy = (s+n)*0.5;
				//	glbDouble _centerY = (mpr_south+mpr_north)*0.5;
				//	if (cy > _centerY)
				//		p_west_tile->RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);
				//	else if (cy < _centerY)
				//		p_west_tile->RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);
				//	else if (cy == _centerY)
				//	{
				//		p_west_tile->RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);
				//		p_west_tile->RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);
				//	}
				//}
			}			
			else if (west_tile_level-1==mpr_level)
			{// 西边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);	
			}
			else if (west_tile_level == mpr_level)
			{//西边的tile等级与本级相同[包含高于本级的情况]
				if (p_west_tile->GetNorthEastChild() && p_west_tile->GetNorthEastChild()->IsInitialized())
				{// 西边tile有东北子块，说明北边tile的实际级数要高于本块级数 : 情况B
					// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
					RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);	
				}
				else
				{// 西边tile没有东北子块，说明西边的tile在东北位置等级与本级相同 : 情况A
					// 处于"右边"位置，要考虑接边问题，符合“补边规则a”					
					osg::Vec3dArray* p_neighbour_edgeVertexArray = NULL;
					if (IsEdgeNeedMerge(this,p_west_tile,GLB_WESTEDGE_NORTHSECTION,p_neighbour_edgeVertexArray))
					{// 根据高程信息判断是否需要补边
						if (mpr_p_geode->containsDrawable(mpr_p_west_northhalf_geom.get())==false)
						{
							BuildEdgeGeometry(p_neighbour_edgeVertexArray,GLB_WESTEDGE_NORTHSECTION);
							mpr_p_west_northhalf_geom->setName("A");
							CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_northhalf_geom.get());
							mpr_terrain->AddOperation(_opera);
						}
						else
						{
							ModifyEdgeGeometry(p_neighbour_edgeVertexArray,GLB_WESTEDGE_NORTHSECTION);
							mpr_p_west_northhalf_geom->setName("A");
						}						
					}
					else
					{// 不需要接边
						RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);	
					}
				}
			}
			else
			{// 不需要接边
				RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);	
			}
		}		
	}	

	if (mpr_nechild && mpr_nechild->IsInitialized())
	{
		mpr_nechild->Merge();
		RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);
		RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);	
	}
	else if (mpr_isinitialized)
	{//		首先处理GLB_NORTH_EDGE_EAST_SECTION
		if (p_north_tile==NULL)
			p_north_tile = GetNorthDirectionTile();
		if (p_north_tile)
		{// 只处理相邻tile极差<=1的情况
			glbInt32 north_tile_level = p_north_tile->GetLevel();
			if (p_north_tile->IsInitialized()==false)
				north_tile_level--;

			if ( (north_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 北边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-north_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_north_easthalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_EASTSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_NORTHEDGE_EASTSECTION);	
					if (mpr_p_north_easthalf_geom /*&& mpr_p_north_easthalf_geom->getName()==""*/){
						mpr_p_north_easthalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_easthalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}					

				if ( mpr_p_north_easthalf_geom && mpr_p_north_easthalf_geom->getName() != sname )
				{// 接边tile等级发生改变，修正接边顶点坐标 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_EASTSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_NORTHEDGE_EASTSECTION);	
					mpr_p_north_easthalf_geom->setName(sname);	
				}
				if (!bAddgeom && mpr_p_north_easthalf_geom && mpr_p_north_easthalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_north_easthalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_easthalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}

				//{// 移除北边块p_north_tile的接边
				//	glbDouble e,w,s,n,m;
				//	p_north_tile->GetRange(e,w,s,n,m,m);
				//	glbDouble cx = (w+e)*0.5;
				//	glbDouble _centerX = (mpr_west+mpr_east)*0.5;
				//	if (cx > _centerX)
				//		p_north_tile->RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);
				//	else if (cx < _centerX)
				//		p_north_tile->RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);
				//	else if (cx == _centerX)
				//	{
				//		p_north_tile->RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);
				//		p_north_tile->RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);
				//	}
				//}
			}			
			else if (north_tile_level-1==mpr_level)
			{// 北边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);	
			}
			else if (north_tile_level == mpr_level)
			{// 北边的tile等级与本级相同[包含高于本级的情况]
				if (p_north_tile->GetSouthEastChild() && p_north_tile->GetSouthEastChild()->IsInitialized())
				{// 北边tile有东南子块，说明北边tile的实际级数要高于本块级数 : 情况B
					// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
					RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);	
				}
				else
				{// 北边tile没有东南子块，说明北边的tile等级与本级相同 : 情况A
					// 处于"下方"位置，要考虑接边问题，符合“补边规则a”
					//osg::ref_ptr<osg::Vec3Array> p_neighbour_edgeVertexArray = new osg::Vec3Array;
					osg::Vec3dArray* p_neighbour_edgeVertexArray = NULL;
					if (IsEdgeNeedMerge(this,p_north_tile,GLB_NORTHEDGE_EASTSECTION,p_neighbour_edgeVertexArray))
					{// 根据高程信息判断是否需要补边
						if (mpr_p_geode->containsDrawable(mpr_p_north_easthalf_geom.get())==false)
						{
							BuildEdgeGeometry(p_neighbour_edgeVertexArray,GLB_NORTHEDGE_EASTSECTION);
							mpr_p_north_easthalf_geom->setName("A");
							CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_easthalf_geom.get());
							mpr_terrain->AddOperation(_opera);
						}
						else
						{
							ModifyEdgeGeometry(p_neighbour_edgeVertexArray,GLB_NORTHEDGE_EASTSECTION);
							mpr_p_north_easthalf_geom->setName("A");
						}						
					}
					else
					{// 不需要接边
						RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);	
					}
				}
			}
			else 
			{
				RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);	
			}
		}

		// 其次处理east_edge_north_section
		p_east_tile = GetEastDirectionTile();
		if (p_east_tile)
		{
			glbInt32 east_tile_level = p_east_tile->GetLevel();
			if (p_east_tile->IsInitialized()==false)
				east_tile_level--;

			if ( (east_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 东边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-east_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_east_northhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_EASTEDGE_NORTHSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_EASTEDGE_NORTHSECTION);	
					if (mpr_p_east_northhalf_geom /*&& mpr_p_east_northhalf_geom->getName()==""*/){
						mpr_p_east_northhalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_east_northhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}								

				if (mpr_p_east_northhalf_geom && mpr_p_east_northhalf_geom->getName() != sname)				
				{// 接边的另一边tile等级变了 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_EASTEDGE_NORTHSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_EASTEDGE_NORTHSECTION);	
					mpr_p_east_northhalf_geom->setName(sname);	
				}		

				if (!bAddgeom && mpr_p_east_northhalf_geom && mpr_p_east_northhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_east_northhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_east_northhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}

				//{// 移除东边块p_east_tile的接边
				//	glbDouble e,w,s,n,m;
				//	p_east_tile->GetRange(e,w,s,n,m,m);
				//	glbDouble cy = (s+n)*0.5;
				//	glbDouble _centerY = (mpr_south+mpr_north)*0.5;
				//	if (cy > _centerY)
				//		p_east_tile->RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);
				//	else if (cy < _centerY)
				//		p_east_tile->RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);
				//	else if (cy == _centerY)
				//	{
				//		p_east_tile->RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);
				//		p_east_tile->RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);
				//	}
				//}
			}		
			else if (east_tile_level-1==mpr_level)
			{// 东边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);	
			}
			else if (east_tile_level == mpr_level)
			{// 东边的tile等级与本级相同[包含高于本级的情况]
				//if (p_east_tile->GetNorthWestChild() && p_east_tile->GetNorthWestChild()->IsInitialized())
				//{// 东边tile有西北子块，说明东边tile的实际级数要高于本块级数 : 情况B
				//	// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
				//	RemoveEdgeGeometry(GLB_EAST_EDGE_NORTH_SECTION);	
				//}
				//else
				//{// 东边tile没有西北子块，说明东边的tile等级与本级相同 : 情况A
				//	// 处于"左边"位置，符合“补边规则a” ，无论IsEdgeNeedMerge返回什么值都不需要要考虑接边问题					
				//	RemoveEdgeGeometry(GLB_EAST_EDGE_NORTH_SECTION);						
				//}
				RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);	
			}
			else
			{// 东边的tile等级 > 本级 对方负责补边
				RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);	
			}
		}
	}

	if (mpr_swchild && mpr_swchild->IsInitialized())
	{
		mpr_swchild->Merge();
		RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);
		RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);		
	}
	else if (mpr_isinitialized)
	{	// 首先处理GLB_SOUTH_EDGE_WEST_SECTION
		p_south_tile = GetSouthDirectionTile();
		if (p_south_tile)
		{// 只处理相邻tile极差<=1的情况
			glbInt32 south_tile_level = p_south_tile->GetLevel();
			if (p_south_tile->IsInitialized()==false)
				south_tile_level--;

			if ( (south_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 南边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-south_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_south_westhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_SOUTHEDGE_WESTSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_SOUTHEDGE_WESTSECTION);	
					if (mpr_p_south_westhalf_geom ){
						mpr_p_south_westhalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_south_westhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}					

				if ( mpr_p_south_westhalf_geom && mpr_p_south_westhalf_geom->getName() != sname )
				{// 接边的另一边tile等级变了 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_SOUTHEDGE_WESTSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_SOUTHEDGE_WESTSECTION);
					mpr_p_south_westhalf_geom->setName(sname);	

				}

				if (!bAddgeom && mpr_p_south_westhalf_geom && mpr_p_south_westhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_south_westhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_south_westhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}

				//{// 移除南边块p_south_tile的接边
				//	glbDouble e,w,s,n,m;
				//	p_south_tile->GetRange(e,w,s,n,m,m);
				//	glbDouble cx = (w+e)*0.5;
				//	glbDouble _centerX = (mpr_west+mpr_east)*0.5;
				//	if (cx > _centerX)
				//		p_south_tile->RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);
				//	else if (cx < _centerX)
				//		p_south_tile->RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);
				//	else if (cx == _centerX)
				//	{
				//		p_south_tile->RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);
				//		p_south_tile->RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);
				//	}
				//}
			}		
			else if (south_tile_level-1==mpr_level)
			{// 南边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);	
			}
			else if (south_tile_level == mpr_level)
			{
				RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);	
			}
			else{
				RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);	
			}
		}


		// 其次处理GLB_WEST_EDGE_SOUTH_SECTION
		if (p_west_tile==NULL)
			p_west_tile = GetWestDirectionTile();

		if (p_west_tile)
		{
			glbInt32 west_tile_level = p_west_tile->GetLevel();
			if (p_west_tile->IsInitialized()==false)
				west_tile_level--;

			if ( (west_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 西边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-west_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_west_southhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_WESTEDGE_SOUTHSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_WESTEDGE_SOUTHSECTION);	
					if (mpr_p_west_southhalf_geom /*&& mpr_p_west_southhalf_geom->getName()==""*/){
						mpr_p_west_southhalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_southhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}								

				if ( mpr_p_west_southhalf_geom && mpr_p_west_southhalf_geom->getName() != sname )
				{// 接边的另一边tile等级变了 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_WESTEDGE_SOUTHSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_WESTEDGE_SOUTHSECTION);	
					mpr_p_west_southhalf_geom->setName(sname);	
				}

				if (!bAddgeom && mpr_p_west_southhalf_geom && mpr_p_west_southhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_west_southhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_southhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}

				//{// 移除西边块p_west_tile的接边
				//	glbDouble e,w,s,n,m;
				//	p_west_tile->GetRange(e,w,s,n,m,m);
				//	glbDouble cy = (s+n)*0.5;
				//	glbDouble _centerY = (mpr_south+mpr_north)*0.5;
				//	if (cy > _centerY)
				//		p_west_tile->RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);
				//	else if (cy < _centerY)
				//		p_west_tile->RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);
				//	else if (cy == _centerY)
				//	{
				//		p_west_tile->RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);
				//		p_west_tile->RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);
				//	}
				//}
			}			
			else if (west_tile_level-1==mpr_level)
			{// 西边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);	
			}
			else if (west_tile_level == mpr_level)
			{//西边的tile等级与本级相同[包含高于本级的情况]
				if (p_west_tile->GetSouthEastChild() && p_west_tile->GetSouthEastChild()->IsInitialized())
				{// 西边tile有东南子块，说明西边tile的实际级数要高于本块级数 : 情况B
					// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
					RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);	
				}
				else
				{// 西边tile没有东南子块，说明西边的tile在东南位置等级与本级相同 : 情况A
					// 处于"右边"位置，要考虑接边问题，符合“补边规则a”
					//osg::ref_ptr<osg::Vec3Array> p_neighbour_edgeVertexArray = new osg::Vec3Array;
					osg::Vec3dArray* p_neighbour_edgeVertexArray = NULL;
					if (IsEdgeNeedMerge(this,p_west_tile,GLB_WESTEDGE_SOUTHSECTION,p_neighbour_edgeVertexArray))
					{// 根据高程信息判断是否需要补边
						if (mpr_p_geode->containsDrawable(mpr_p_west_southhalf_geom.get())==false)
						{
							BuildEdgeGeometry(p_neighbour_edgeVertexArray,GLB_WESTEDGE_SOUTHSECTION);
							mpr_p_west_southhalf_geom->setName("A");
							CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_southhalf_geom.get());
							mpr_terrain->AddOperation(_opera);
						}
						else
						{							
							ModifyEdgeGeometry(p_neighbour_edgeVertexArray,GLB_WESTEDGE_SOUTHSECTION);
							mpr_p_west_southhalf_geom->setName("A");							
						}						
					}
					else
					{// 不需要接边
						RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);	
					}
				}				
			}
			else
			{
				RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);	
			}
		}
	}

	if (mpr_sechild && mpr_sechild->IsInitialized())
	{
		mpr_sechild->Merge();
		RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);
		RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);	
	}
	else if (mpr_isinitialized)
	{	// 首先处理GLB_SOUTH_EDGE_EAST_SECTION
		if (p_south_tile==NULL)
			p_south_tile = GetSouthDirectionTile();
		if (p_south_tile)
		{// 只处理相邻tile极差<=1的情况
			glbInt32 south_tile_level = p_south_tile->GetLevel();
			if (p_south_tile->IsInitialized()==false)
				south_tile_level--;

			if ( (south_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 南边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-south_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_south_easthalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_SOUTHEDGE_EASTSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_SOUTHEDGE_EASTSECTION);	
					if (mpr_p_south_easthalf_geom /*&& mpr_p_south_easthalf_geom->getName()==""*/){
						mpr_p_south_easthalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_south_easthalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}					

				if ( mpr_p_south_easthalf_geom && mpr_p_south_easthalf_geom->getName() != sname )
				{// 接边的另一边tile等级变了 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_SOUTHEDGE_EASTSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_SOUTHEDGE_EASTSECTION);	
					mpr_p_south_easthalf_geom->setName(sname);	
				}

				if (!bAddgeom && mpr_p_south_easthalf_geom && mpr_p_south_easthalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_south_easthalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_south_easthalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}

				//{// 移除南边块p_south_tile的接边
				//	glbDouble e,w,s,n,m;
				//	p_south_tile->GetRange(e,w,s,n,m,m);
				//	glbDouble cx = (w+e)*0.5;
				//	glbDouble _centerX = (mpr_west+mpr_east)*0.5;
				//	if (cx > _centerX)
				//		p_south_tile->RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);
				//	else if (cx < _centerX)
				//		p_south_tile->RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);
				//	else if (cx == _centerX)
				//	{
				//		p_south_tile->RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);
				//		p_south_tile->RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);
				//	}
				//}
			}		
			else if (south_tile_level-1==mpr_level)
			{// 南边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);	
			}
			else if (south_tile_level == mpr_level)
			{
				RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);				
			}
			else{
				RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);
			}			
		}

		// 其次处理GLB_EAST_EDGE_SOUTH_SECTION
		if (p_east_tile==NULL)
			p_east_tile = GetEastDirectionTile();
		if (p_east_tile)
		{
			glbInt32 east_tile_level = p_east_tile->GetLevel();
			if (p_east_tile->IsInitialized()==false)
				east_tile_level--;

			if ( (east_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 东边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-east_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_east_southhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_EASTEDGE_SOUTHSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_EASTEDGE_SOUTHSECTION);	
					if (mpr_p_east_southhalf_geom /*&& mpr_p_east_southhalf_geom->getName()==""*/){
						mpr_p_east_southhalf_geom->setName(sname);	

						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_east_southhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}					

				if (mpr_p_east_southhalf_geom && mpr_p_east_southhalf_geom->getName() != sname)				
				{// // 接边的另一边tile等级变了 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_EASTEDGE_SOUTHSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_EASTEDGE_SOUTHSECTION);	
					mpr_p_east_southhalf_geom->setName(sname);	
				}		

				if (!bAddgeom && mpr_p_east_southhalf_geom && mpr_p_east_southhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_east_southhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_east_southhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}

				//{// 移除东边块p_east_tile的接边
				//	glbDouble e,w,s,n,m;
				//	p_east_tile->GetRange(e,w,s,n,m,m);
				//	glbDouble cy = (s+n)*0.5;
				//	glbDouble _centerY = (mpr_south+mpr_north)*0.5;
				//	if (cy > _centerY)
				//		p_east_tile->RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);
				//	else if (cy < _centerY)
				//		p_east_tile->RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);
				//	else if (cy == _centerY)
				//	{
				//		p_east_tile->RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);
				//		p_east_tile->RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);
				//	}
				//}
			}			
			else if (east_tile_level-1==mpr_level)
			{// 东边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);	
			}
			else if (east_tile_level == mpr_level)
			{// 东边的tile等级与本级相同[包含高于本级的情况]
				//if (p_east_tile->GetNorthWestChild() && p_east_tile->GetNorthWestChild()->IsInitialized())
				//{// 东边tile有西北子块，说明东边tile的实际级数要高于本块级数 : 情况B
				//	// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
				//	RemoveEdgeGeometry(GLB_EAST_EDGE_NORTH_SECTION);	
				//}
				//else
				//{// 东边tile没有西北子块，说明东边的tile等级与本级相同 : 情况A
				//	// 处于"左边"位置，符合“补边规则a” ，无论IsEdgeNeedMerge返回什么值都不需要要考虑接边问题					
				//	RemoveEdgeGeometry(GLB_EAST_EDGE_NORTH_SECTION);						
				//}
				RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);				
			}
			else
			{
				RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);
			}
		}
	}	
}
void CGlbGlobeTerrainTile::MergeFlat()
{
	// 不可见的tile不需要做接边处理
	if (IsVisible()==false)
		return;

	// 补边规则：A.同级之间验证相同经纬度高程值是否一致，不一致时补边,处于 下方或右边 （south,east）的tile补边
	//			 B.不同级之间只处理相差1级的情况，由level较大的tile负责补边。
	//			 C.不同级且相差超过1级的情况，由level较大的tile负责补边（中间过程会出现）
	CGlbGlobeTerrainTile* p_north_tile = NULL;
	CGlbGlobeTerrainTile* p_south_tile = NULL;
	CGlbGlobeTerrainTile* p_west_tile = NULL;
	CGlbGlobeTerrainTile* p_east_tile = NULL;
	//osg::ref_ptr<osg::EllipsoidModel> ellips = new osg::EllipsoidModel(osg::WGS_84_RADIUS_EQUATOR,osg::WGS_84_RADIUS_POLAR);

	//#ifdef _DEBUG
	//	WCHAR wBuff[128];
	//	wsprintf(wBuff,L"Merge begin. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//	CGlbSysLog::WriteLog(wBuff);
	//	_swprintf(wBuff,L"south:%.3f, north: %.3f, west: %.3f, east: %.3f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//	CGlbSysLog::WriteLog(wBuff);	
	//#endif

	if (mpr_nwchild && mpr_nwchild->IsInitialized())
	{
		mpr_nwchild->MergeFlat();
		RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);
		RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);	
	}
	else if (mpr_isinitialized)
	{	// 首先处理north_edge_west_section
		//		获取其北边相邻的tile
		p_north_tile = GetNorthDirectionTileOnFlatTerrain();
		if (p_north_tile)
		{// 只处理相邻tile极差<=1的情况
			glbInt32 north_tile_level = p_north_tile->GetLevel();
			if (p_north_tile->IsInitialized()==false && north_tile_level>0)
				north_tile_level--;

			if ( (north_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 北边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;	
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-north_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_north_westhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_WESTSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_NORTHEDGE_WESTSECTION);
					if (mpr_p_north_westhalf_geom){
						mpr_p_north_westhalf_geom->setName(sname);
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_westhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}								

				if ( mpr_p_north_westhalf_geom && mpr_p_north_westhalf_geom->getName() != sname )
				{// 接边tile等级发生改变，修正接边顶点坐标 2013.2.21 竟然有这种情况，晕!!!!
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_WESTSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_NORTHEDGE_WESTSECTION);	
					mpr_p_north_westhalf_geom->setName(sname);	
				}

				if ( !bAddgeom && mpr_p_north_westhalf_geom && mpr_p_north_westhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_north_westhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_westhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}
			}		
			else if (north_tile_level-1==mpr_level)
			{// 北边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);	
			}
			else if (north_tile_level == mpr_level)
			{// 北边的tile等级与本级相同[包含高于本级的情况]
				if (p_north_tile->GetSouthWestChild() && p_north_tile->GetSouthWestChild()->IsInitialized())
				{// 北边tile有西南子块，说明北边tile的实际级数要高于本块级数 : 情况B
					// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
					RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);	
				}
				else
				{// 北边tile没有西南子块，说明北边的tile在西北位置等级与本级相同 : 情况A
					// 处于"下方"位置，要考虑接边问题，符合“补边规则a”
					//osg::ref_ptr<osg::Vec3Array> p_neighbour_edgeVertexArray = new osg::Vec3Array;
					osg::Vec3dArray* p_neighbour_edgeVertexArray = NULL;
					if (IsEdgeNeedMerge(this,p_north_tile,GLB_NORTHEDGE_WESTSECTION,p_neighbour_edgeVertexArray))
					{// 根据高程信息判断是否需要补边
						if (mpr_p_geode->containsDrawable(mpr_p_north_westhalf_geom.get())==false)
						{
							BuildEdgeGeometry(p_neighbour_edgeVertexArray,GLB_NORTHEDGE_WESTSECTION);
							mpr_p_north_westhalf_geom->setName("A");
							CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_westhalf_geom.get());
							mpr_terrain->AddOperation(_opera);
						}
						else
						{
							ModifyEdgeGeometry(p_neighbour_edgeVertexArray,GLB_NORTHEDGE_WESTSECTION);	
							mpr_p_north_westhalf_geom->setName("A");
						}										
					}
					else
					{// 不需要接边
						RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);	
					}
				}
			}
			else
			{// 不需要接边
				RemoveEdgeGeometry(GLB_NORTHEDGE_WESTSECTION);	
			}
		}

		// 其次处理west_edge_north_section
		p_west_tile = GetWestDirectionTileOnFlatTerrain();
		if (p_west_tile)
		{
			glbInt32 west_tile_level = p_west_tile->GetLevel();
			if (p_west_tile->IsInitialized()==false && west_tile_level>0)
				west_tile_level--;

			if ( (west_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 西边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-west_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_west_northhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_WESTEDGE_NORTHSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_WESTEDGE_NORTHSECTION);						
					if (mpr_p_west_northhalf_geom /*&& mpr_p_west_northhalf_geom->getName()==""*/){
						mpr_p_west_northhalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_northhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}					

				if ( mpr_p_west_northhalf_geom && mpr_p_west_northhalf_geom->getName() != sname )
				{// 接边tile等级发生改变，修正接边顶点坐标 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_WESTEDGE_NORTHSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_WESTEDGE_NORTHSECTION);	
					mpr_p_west_northhalf_geom->setName(sname);	
				}
				if (!bAddgeom && mpr_p_west_northhalf_geom && mpr_p_west_northhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_west_northhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_northhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}
			}			
			else if (west_tile_level-1==mpr_level)
			{// 西边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);	
			}
			else if (west_tile_level == mpr_level)
			{//西边的tile等级与本级相同[包含高于本级的情况]
				if (p_west_tile->GetNorthEastChild() && p_west_tile->GetNorthEastChild()->IsInitialized())
				{// 西边tile有东北子块，说明北边tile的实际级数要高于本块级数 : 情况B
					// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
					RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);	
				}
				else
				{// 西边tile没有东北子块，说明西边的tile在东北位置等级与本级相同 : 情况A
					// 处于"右边"位置，要考虑接边问题，符合“补边规则a”					
					osg::Vec3dArray* p_neighbour_edgeVertexArray = NULL;
					if (IsEdgeNeedMerge(this,p_west_tile,GLB_WESTEDGE_NORTHSECTION,p_neighbour_edgeVertexArray))
					{// 根据高程信息判断是否需要补边
						if (mpr_p_geode->containsDrawable(mpr_p_west_northhalf_geom.get())==false)
						{
							BuildEdgeGeometry(p_neighbour_edgeVertexArray,GLB_WESTEDGE_NORTHSECTION);
							mpr_p_west_northhalf_geom->setName("A");
							CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_northhalf_geom.get());
							mpr_terrain->AddOperation(_opera);
						}
						else
						{
							ModifyEdgeGeometry(p_neighbour_edgeVertexArray,GLB_WESTEDGE_NORTHSECTION);
							mpr_p_west_northhalf_geom->setName("A");
						}						
					}
					else
					{// 不需要接边
						RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);	
					}
				}
			}
			else
			{// 不需要接边
				RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);	
			}
		}
	}	

	if (mpr_nechild && mpr_nechild->IsInitialized())
	{
		mpr_nechild->MergeFlat();
		RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);
		RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);	
	}
	else if (mpr_isinitialized)
	{//		首先处理GLB_NORTH_EDGE_EAST_SECTION
		if (p_north_tile==NULL)
			p_north_tile = GetNorthDirectionTileOnFlatTerrain();
		if (p_north_tile)
		{// 只处理相邻tile极差<=1的情况
			glbInt32 north_tile_level = p_north_tile->GetLevel();
			if (p_north_tile->IsInitialized()==false && north_tile_level>0)
				north_tile_level--;

			if ( (north_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 北边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-north_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_north_easthalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_EASTSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_NORTHEDGE_EASTSECTION);	
					if (mpr_p_north_easthalf_geom /*&& mpr_p_north_easthalf_geom->getName()==""*/){
						mpr_p_north_easthalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_easthalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}					

				if ( mpr_p_north_easthalf_geom && mpr_p_north_easthalf_geom->getName() != sname )
				{// 接边tile等级发生改变，修正接边顶点坐标 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_EASTSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_NORTHEDGE_EASTSECTION);	
					mpr_p_north_easthalf_geom->setName(sname);	
				}
				if (!bAddgeom && mpr_p_north_easthalf_geom && mpr_p_north_easthalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_north_easthalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_easthalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}
			}			
			else if (north_tile_level-1==mpr_level)
			{// 北边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);	
			}
			else if (north_tile_level == mpr_level)
			{// 北边的tile等级与本级相同[包含高于本级的情况]
				if (p_north_tile->GetSouthEastChild() && p_north_tile->GetSouthEastChild()->IsInitialized())
				{// 北边tile有东南子块，说明北边tile的实际级数要高于本块级数 : 情况B
					// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
					RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);	
				}
				else
				{// 北边tile没有东南子块，说明北边的tile等级与本级相同 : 情况A
					// 处于"下方"位置，要考虑接边问题，符合“补边规则a”
					//osg::ref_ptr<osg::Vec3Array> p_neighbour_edgeVertexArray = new osg::Vec3Array;
					osg::Vec3dArray* p_neighbour_edgeVertexArray = NULL;
					if (IsEdgeNeedMerge(this,p_north_tile,GLB_NORTHEDGE_EASTSECTION,p_neighbour_edgeVertexArray))
					{// 根据高程信息判断是否需要补边
						if (mpr_p_geode->containsDrawable(mpr_p_north_easthalf_geom.get())==false)
						{
							BuildEdgeGeometry(p_neighbour_edgeVertexArray,GLB_NORTHEDGE_EASTSECTION);
							mpr_p_north_easthalf_geom->setName("A");
							CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_easthalf_geom.get());
							mpr_terrain->AddOperation(_opera);
						}
						else
						{
							ModifyEdgeGeometry(p_neighbour_edgeVertexArray,GLB_NORTHEDGE_EASTSECTION);
							mpr_p_north_easthalf_geom->setName("A");
						}						
					}
					else
					{// 不需要接边
						RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);	
					}
				}
			}
			else 
			{
				RemoveEdgeGeometry(GLB_NORTHEDGE_EASTSECTION);	
			}
		}

		// 其次处理east_edge_north_section
		p_east_tile = GetEastDirectionTileOnFlatTerrain();
		if (p_east_tile)
		{
			glbInt32 east_tile_level = p_east_tile->GetLevel();
			if (p_east_tile->IsInitialized()==false && east_tile_level>0)
				east_tile_level--;

			if ( (east_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 东边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-east_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_east_northhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_EASTEDGE_NORTHSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_EASTEDGE_NORTHSECTION);	
					if (mpr_p_east_northhalf_geom /*&& mpr_p_east_northhalf_geom->getName()==""*/){
						mpr_p_east_northhalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_east_northhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}								

				if (mpr_p_east_northhalf_geom && mpr_p_east_northhalf_geom->getName() != sname)				
				{// 接边的另一边tile等级变了 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_EASTEDGE_NORTHSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_EASTEDGE_NORTHSECTION);	
					mpr_p_east_northhalf_geom->setName(sname);	
				}		

				if (!bAddgeom && mpr_p_east_northhalf_geom && mpr_p_east_northhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_east_northhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_east_northhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}
			}		
			else if (east_tile_level-1==mpr_level)
			{// 东边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);	
			}
			else if (east_tile_level == mpr_level)
			{// 东边的tile等级与本级相同[包含高于本级的情况]
				//if (p_east_tile->GetNorthWestChild() && p_east_tile->GetNorthWestChild()->IsInitialized())
				//{// 东边tile有西北子块，说明东边tile的实际级数要高于本块级数 : 情况B
				//	// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
				//	RemoveEdgeGeometry(GLB_EAST_EDGE_NORTH_SECTION);	
				//}
				//else
				//{// 东边tile没有西北子块，说明东边的tile等级与本级相同 : 情况A
				//	// 处于"左边"位置，符合“补边规则a” ，无论IsEdgeNeedMerge返回什么值都不需要要考虑接边问题					
				//	RemoveEdgeGeometry(GLB_EAST_EDGE_NORTH_SECTION);						
				//}
				RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);	
			}
			else
			{
				RemoveEdgeGeometry(GLB_EASTEDGE_NORTHSECTION);	
			}
		}
	}

	if (mpr_swchild && mpr_swchild->IsInitialized())
	{
		mpr_swchild->MergeFlat();
		RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);
		RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);		
	}
	else if (mpr_isinitialized)
	{	// 首先处理GLB_SOUTH_EDGE_WEST_SECTION
		p_south_tile = GetSouthDirectionTileOnFlatTerrain();
		if (p_south_tile)
		{// 只处理相邻tile极差<=1的情况
			glbInt32 south_tile_level = p_south_tile->GetLevel();
			if (p_south_tile->IsInitialized()==false && south_tile_level>0)
				south_tile_level--;

			if ( (south_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 南边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-south_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_south_westhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_SOUTHEDGE_WESTSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_SOUTHEDGE_WESTSECTION);	
					if (mpr_p_south_westhalf_geom ){
						mpr_p_south_westhalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_south_westhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}					

				if ( mpr_p_south_westhalf_geom && mpr_p_south_westhalf_geom->getName() != sname )
				{// 接边的另一边tile等级变了 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_SOUTHEDGE_WESTSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_SOUTHEDGE_WESTSECTION);
					mpr_p_south_westhalf_geom->setName(sname);	
				}

				if (!bAddgeom && mpr_p_south_westhalf_geom && mpr_p_south_westhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_south_westhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_south_westhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}
			}		
			else if (south_tile_level-1==mpr_level)
			{// 南边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);	
			}
			else if (south_tile_level == mpr_level){
				RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);	
			}
			else{
				RemoveEdgeGeometry(GLB_SOUTHEDGE_WESTSECTION);	
			}
		}

		// 其次处理GLB_WEST_EDGE_SOUTH_SECTION
		if (p_west_tile==NULL)
			p_west_tile = GetWestDirectionTileOnFlatTerrain();

		if (p_west_tile)
		{
			glbInt32 west_tile_level = p_west_tile->GetLevel();
			if (p_west_tile->IsInitialized()==false && west_tile_level>0)
				west_tile_level--;

			if ( (west_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 西边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-west_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_west_southhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_WESTEDGE_SOUTHSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_WESTEDGE_SOUTHSECTION);	
					if (mpr_p_west_southhalf_geom /*&& mpr_p_west_southhalf_geom->getName()==""*/){
						mpr_p_west_southhalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_southhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}								

				if ( mpr_p_west_southhalf_geom && mpr_p_west_southhalf_geom->getName() != sname )
				{// 接边的另一边tile等级变了 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_WESTEDGE_SOUTHSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_WESTEDGE_SOUTHSECTION);	
					mpr_p_west_southhalf_geom->setName(sname);	
				}

				if (!bAddgeom && mpr_p_west_southhalf_geom && mpr_p_west_southhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_west_southhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_southhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}
			}			
			else if (west_tile_level-1==mpr_level)
			{// 西边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);	
			}
			else if (west_tile_level == mpr_level)
			{//西边的tile等级与本级相同[包含高于本级的情况]
				if (p_west_tile->GetSouthEastChild() && p_west_tile->GetSouthEastChild()->IsInitialized())
				{// 西边tile有东南子块，说明西边tile的实际级数要高于本块级数 : 情况B
					// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
					RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);	
				}
				else
				{// 西边tile没有东南子块，说明西边的tile在东南位置等级与本级相同 : 情况A
					// 处于"右边"位置，要考虑接边问题，符合“补边规则a”
					//osg::ref_ptr<osg::Vec3Array> p_neighbour_edgeVertexArray = new osg::Vec3Array;
					osg::Vec3dArray* p_neighbour_edgeVertexArray = NULL;
					if (IsEdgeNeedMerge(this,p_west_tile,GLB_WESTEDGE_SOUTHSECTION,p_neighbour_edgeVertexArray))
					{// 根据高程信息判断是否需要补边
						if (mpr_p_geode->containsDrawable(mpr_p_west_southhalf_geom.get())==false)
						{
							BuildEdgeGeometry(p_neighbour_edgeVertexArray,GLB_WESTEDGE_SOUTHSECTION);
							mpr_p_west_southhalf_geom->setName("A");
							CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_southhalf_geom.get());
							mpr_terrain->AddOperation(_opera);
						}
						else
						{							
							ModifyEdgeGeometry(p_neighbour_edgeVertexArray,GLB_WESTEDGE_SOUTHSECTION);
							mpr_p_west_southhalf_geom->setName("A");							
						}						
					}
					else
					{// 不需要接边
						RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);	
					}
				}
			}
			else
			{
				RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);	
			}
		}
	}

	if (mpr_sechild && mpr_sechild->IsInitialized())
	{
		mpr_sechild->MergeFlat();
		RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);
		RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);	
	}
	else if (mpr_isinitialized)
	{	// 首先处理GLB_SOUTH_EDGE_EAST_SECTION
		if (p_south_tile==NULL)
			p_south_tile = GetSouthDirectionTileOnFlatTerrain();
		if (p_south_tile)
		{// 只处理相邻tile极差<=1的情况
			glbInt32 south_tile_level = p_south_tile->GetLevel();
			if (p_south_tile->IsInitialized()==false && south_tile_level>0)
				south_tile_level--;

			if ( (south_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 南边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-south_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_south_easthalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_SOUTHEDGE_EASTSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_SOUTHEDGE_EASTSECTION);	
					if (mpr_p_south_easthalf_geom /*&& mpr_p_south_easthalf_geom->getName()==""*/){
						mpr_p_south_easthalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_south_easthalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}					

				if ( mpr_p_south_easthalf_geom && mpr_p_south_easthalf_geom->getName() != sname )
				{// 接边的另一边tile等级变了 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_SOUTHEDGE_EASTSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_SOUTHEDGE_EASTSECTION);	
					mpr_p_south_easthalf_geom->setName(sname);	
				}

				if (!bAddgeom && mpr_p_south_easthalf_geom && mpr_p_south_easthalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_south_easthalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_south_easthalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}
			}		
			else if (south_tile_level-1==mpr_level)
			{// 南边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);	
			}
			else if (south_tile_level == mpr_level){
				RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);	
			}
			else{
				RemoveEdgeGeometry(GLB_SOUTHEDGE_EASTSECTION);
			}			
		}

		// 其次处理GLB_EAST_EDGE_SOUTH_SECTION
		if (p_east_tile==NULL)
			p_east_tile = GetEastDirectionTileOnFlatTerrain();
		if (p_east_tile)
		{
			glbInt32 east_tile_level = p_east_tile->GetLevel();
			if (p_east_tile->IsInitialized()==false && east_tile_level>0)
				east_tile_level--;

			if ( (east_tile_level-mpr_level) <= -1)
			{// 中间过程可能会进入这里，需要处理，否则移动过程中会有缝隙出现。
				// 东边的tile等级比本级小2级或3级：符合"补边规则c" 情况C, 需要补边，由本块负责补边.
				bool bAddgeom = false;
				std::ostringstream   ostr;   //   include   <sstream>   
				ostr  <<  "C"  <<  (mpr_level-east_tile_level);  
				std::string sname = ostr.str();

				if (mpr_p_geode->containsDrawable(mpr_p_east_southhalf_geom.get())==false)
				{// 补边面片没有挂上
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_EASTEDGE_SOUTHSECTION);
					BuildEdgeGeometry(p_edgeVertexArray.get(),GLB_EASTEDGE_SOUTHSECTION);	
					if (mpr_p_east_southhalf_geom /*&& mpr_p_east_southhalf_geom->getName()==""*/){
						mpr_p_east_southhalf_geom->setName(sname);	
						CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_east_southhalf_geom.get());
						mpr_terrain->AddOperation(_opera);
						bAddgeom = true;
					}				
				}					

				if (mpr_p_east_southhalf_geom && mpr_p_east_southhalf_geom->getName() != sname)				
				{// // 接边的另一边tile等级变了 2013.2.21
					osg::ref_ptr<osg::Vec3dArray> p_edgeVertexArray = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_EASTEDGE_SOUTHSECTION);
					ModifyEdgeGeometry(p_edgeVertexArray.get(),GLB_EASTEDGE_SOUTHSECTION);	
					mpr_p_east_southhalf_geom->setName(sname);	
				}		

				if (!bAddgeom && mpr_p_east_southhalf_geom && mpr_p_east_southhalf_geom->getName() == sname && mpr_p_geode->containsDrawable(mpr_p_east_southhalf_geom.get())==false)
				{
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_east_southhalf_geom.get());
					mpr_terrain->AddOperation(_opera);
				}
			}			
			else if (east_tile_level-1==mpr_level)
			{// 东边的tile等级比本级大一级 : 符合"补边规则b" 情况B, 需要补边，由对方块负责补边.
				RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);	
			}
			else if (east_tile_level == mpr_level)
			{// 东边的tile等级与本级相同[包含高于本级的情况]
				//if (p_east_tile->GetNorthWestChild() && p_east_tile->GetNorthWestChild()->IsInitialized())
				//{// 东边tile有西北子块，说明东边tile的实际级数要高于本块级数 : 情况B
				//	// -本级级数低于邻居tile级数，由邻居tile负责补边, 如果存在补边面片，移除之
				//	RemoveEdgeGeometry(GLB_EAST_EDGE_NORTH_SECTION);	
				//}
				//else
				//{// 东边tile没有西北子块，说明东边的tile等级与本级相同 : 情况A
				//	// 处于"左边"位置，符合“补边规则a” ，无论IsEdgeNeedMerge返回什么值都不需要要考虑接边问题					
				//	RemoveEdgeGeometry(GLB_EAST_EDGE_NORTH_SECTION);						
				//}
				RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);	
			}
			else
			{
				RemoveEdgeGeometry(GLB_EASTEDGE_SOUTHSECTION);
			}
		}
	}	
}
glbDouble CGlbGlobeTerrainTile::GetElevationAt(glbDouble lonOrX, glbDouble latOrY)
{
	if (!mpr_isinitialized)
		return 0.0;

	GlbGlobeChildLocationEnum location = GetLocationFromLongitudeLatitude(lonOrX,latOrY);
	switch(location)
	{
	case GLB_NORTHWEST:
		{
			glbref_ptr<CGlbGlobeTerrainTile> p_northwestchild = GetNorthWestChild();
			if (p_northwestchild && p_northwestchild->IsInitialized()){				
					return p_northwestchild->GetElevationAt(lonOrX,latOrY);
			}
		}
		break;
	case GLB_NORTHEAST:
		{
			glbref_ptr<CGlbGlobeTerrainTile> p_northeastchild = GetNorthEastChild();
			if (p_northeastchild && p_northeastchild->IsInitialized()){				
					return p_northeastchild->GetElevationAt(lonOrX,latOrY);
			}
		}
		break;
	case GlB_SOUTHWEST:
		{
			glbref_ptr<CGlbGlobeTerrainTile> p_southwestchild = GetSouthWestChild();
			if (p_southwestchild && p_southwestchild->IsInitialized()){				
					return p_southwestchild->GetElevationAt(lonOrX,latOrY);
			}
		}		
		break;
	case GLB_SOUTHEAST:
		{
			glbref_ptr<CGlbGlobeTerrainTile> p_southeastchild = GetSouthEastChild();
			if (p_southeastchild && p_southeastchild->IsInitialized()){
					return p_southeastchild->GetElevationAt(lonOrX,latOrY);
			}
		}
		break;
	}

	glbDouble latitude_span = (mpr_north - mpr_south) / mpr_vertex_count;
	glbDouble longitude_span = (mpr_east - mpr_west) / mpr_vertex_count;
	glbDouble d_row = (mpr_north - latOrY) / latitude_span;
	double d_col = (lonOrX - mpr_west) / longitude_span; 
	glbInt32 row = (glbInt32)d_row;
	glbInt32 col = (glbInt32)d_col;
	if (row >= mpr_demsize-1) 
		row = mpr_demsize-2;
	if (col >= mpr_demsize-1) 
		col = mpr_demsize-2;

	glbFloat* _pDemdata = NULL;
	if (mpr_overlaydemdata)
		_pDemdata = mpr_overlaydemdata;
	else
		_pDemdata = mpr_demdata;

	glbDouble exaggrate = mpr_terrain->GetExaggrate();
	glbFloat h1 = _pDemdata[row*mpr_demsize+col] * exaggrate;
	glbFloat h2 = _pDemdata[(row+1)*mpr_demsize+col] * exaggrate;
	glbFloat h3 = _pDemdata[row*mpr_demsize+col+1] * exaggrate;
	glbFloat h4 = _pDemdata[(row+1)*mpr_demsize+col+1] * exaggrate;

	osg::Vec3d v1(mpr_west+col*longitude_span,mpr_north-row*latitude_span,h1);
	osg::Vec3d v2(mpr_west+col*longitude_span,mpr_north-(row+1)*latitude_span,h2);
	osg::Vec3d v3(mpr_west+(col+1)*longitude_span,mpr_north-row*latitude_span,h3);
	osg::Vec3d v4(mpr_west+(col+1)*longitude_span,mpr_north-(row+1)*latitude_span,h4);

	glbDouble elevation=0.0;
	osg::Vec3d n;	

	glbDouble theta = atan((latOrY-v2.y()) / (lonOrX-v2.x()));
	if (theta >= osg::DegreesToRadians(45.0))
	{// v1,v2,v3
		n = (v1-v2)^(v3-v2);			
	}
	else
	{// v2,v4,v3
		n = (v4-v2)^(v3-v2);			
	}
	if (fabs(n.z()) > FAZHI)
	{
		elevation = v2.z() + (-n.x()*(lonOrX-v2.x()) - n.y()*(latOrY-v2.y())) / n.z();
	}
	else
		elevation = h2;

	return elevation;
}

osg::BoundingBox CGlbGlobeTerrainTile::GetVisibleExtent(osg::Polytope cv,glbDouble min_southDegree, glbDouble max_norhtDegree)
{
	glbDouble w,e,s,n;
	mpr_terrain->GetExtent(w,e,s,n);
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		s = min_southDegree;
		n = max_norhtDegree;
	}

	if (mpr_east <= w || mpr_west >= e || mpr_south >= n || mpr_north <= s)
	{// 非地形有效区域，返回无效bbox
		osg::BoundingBox _bb;
		return _bb;		 
	}	

	std::vector<osg::Vec3d> pointvec;
	osg::BoundingBox bb;	
	osg::Vec3d corners[8];
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_north),osg::DegreesToRadians(mpr_west),mpr_minalt,corners[0].x(),corners[0].y(),corners[0].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_south),osg::DegreesToRadians(mpr_west),mpr_minalt,corners[1].x(),corners[1].y(),corners[1].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_south),osg::DegreesToRadians(mpr_east),mpr_minalt,corners[2].x(),corners[2].y(),corners[2].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_north),osg::DegreesToRadians(mpr_east),mpr_minalt,corners[3].x(),corners[3].y(),corners[3].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_north),osg::DegreesToRadians(mpr_west),mpr_maxalt,corners[4].x(),corners[4].y(),corners[4].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_south),osg::DegreesToRadians(mpr_west),mpr_maxalt,corners[5].x(),corners[5].y(),corners[5].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_south),osg::DegreesToRadians(mpr_east),mpr_maxalt,corners[6].x(),corners[6].y(),corners[6].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_north),osg::DegreesToRadians(mpr_east),mpr_maxalt,corners[7].x(),corners[7].y(),corners[7].z());
	}
	else
	{
		corners[0].set(mpr_west,mpr_north,mpr_minalt);
		corners[1].set(mpr_west,mpr_south,mpr_minalt);
		corners[2].set(mpr_east,mpr_north,mpr_minalt);
		corners[3].set(mpr_east,mpr_south,mpr_minalt);
		corners[4].set(mpr_west,mpr_north,mpr_maxalt);
		corners[5].set(mpr_west,mpr_south,mpr_maxalt);
		corners[6].set(mpr_east,mpr_north,mpr_maxalt);
		corners[7].set(mpr_east,mpr_south,mpr_maxalt);	
	}	

	for (long i = 0; i < 8; i++){		
		bb.expandBy(corners[i]);		
	}

	osg::BoundingBox _bb;
	// 判断boundBox是否都在视锥体内
	bool bAllIn = cv.containsAllOf(bb);	
	if (bAllIn )//&& bFaceCircle)
	{// 8个点都在视域范围内，则不需要细分子节点		
		_bb.set(mpr_west,mpr_south,mpr_minalt,mpr_east,mpr_north,mpr_maxalt);
		return _bb;
	}
	bool bPartIn = cv.contains(bb);
	if ( bPartIn )//&& bFaceCircle)
	{// 部分在
		glbDouble CenterLat = 0.5f * (mpr_south + mpr_north);
		glbDouble CenterLon = 0.5f * (mpr_east + mpr_west);

		glbInt32 childLevel = mpr_level+1;
		glbInt32 r2 = mpr_row*2;
		glbInt32 c2 = mpr_column*2;

		osg::BoundingBox northeast_bb;
		glbref_ptr<CGlbGlobeTerrainTile> nechild = mpr_nechild;
		if (nechild && nechild->IsInitialized()){
			northeast_bb = nechild->GetVisibleExtent(cv,min_southDegree,max_norhtDegree);
		}
		else{			
			northeast_bb = AAA(CenterLat, mpr_north, CenterLon, mpr_east,mpr_minalt,mpr_maxalt,childLevel,r2+1,c2+1,cv,min_southDegree,max_norhtDegree);	// GLB_NORTHEAST
		}

		osg::BoundingBox northwest_bb;
		glbref_ptr<CGlbGlobeTerrainTile> nwchild = mpr_nwchild;
		if (nwchild && nwchild->IsInitialized()){
			northwest_bb = nwchild->GetVisibleExtent(cv,min_southDegree,max_norhtDegree);
		}else{
			northwest_bb = AAA(CenterLat, mpr_north, mpr_west, CenterLon,mpr_minalt,mpr_maxalt,childLevel,r2+1,c2,cv,min_southDegree,max_norhtDegree);  // GLB_NORTHWEST
		}

		osg::BoundingBox southwest_bb;
		glbref_ptr<CGlbGlobeTerrainTile> swchild = mpr_swchild;
		if (swchild  && swchild->IsInitialized()){
			southwest_bb = swchild->GetVisibleExtent(cv,min_southDegree,max_norhtDegree);
		}
		else{
			southwest_bb = AAA(mpr_south, CenterLat, mpr_west, CenterLon,mpr_minalt,mpr_maxalt,childLevel,r2,c2,cv,min_southDegree,max_norhtDegree); // GLB_SOUTHWEST
		}

		osg::BoundingBox southeast_bb;
		glbref_ptr<CGlbGlobeTerrainTile> sechild = mpr_sechild;
		if (sechild && sechild->IsInitialized()){
			southeast_bb = sechild->GetVisibleExtent(cv,min_southDegree,max_norhtDegree);
		}
		else{
			southeast_bb = AAA(mpr_south, CenterLat, CenterLon, mpr_east,mpr_minalt,mpr_maxalt,childLevel,r2,c2+1,cv,min_southDegree,max_norhtDegree); // GLB_SoUTHEAST
		}

		if (northeast_bb.valid())
			_bb.expandBy(northeast_bb);
		if (northwest_bb.valid())
			_bb.expandBy(northwest_bb);
		if (southeast_bb.valid())
			_bb.expandBy(southeast_bb);
		if (southwest_bb.valid())
			_bb.expandBy(southwest_bb);	
	}
	else
	{// 全不在			
	}
	return _bb;
}
glbInt32 CGlbGlobeTerrainTile::GetLevel()
{
	return mpr_level;
}
void CGlbGlobeTerrainTile::GetPosition(glbInt32& row, glbInt32& column)
{
	row = mpr_row;
	column = mpr_column;
}

glbBool CGlbGlobeTerrainTile::SetPosition(glbInt32 row, glbInt32 column)
{
	mpr_row = row;
	mpr_column = column;
	return true;
}
void CGlbGlobeTerrainTile::GetRange(glbDouble& east, glbDouble& west, glbDouble& south, glbDouble& north, glbDouble& minAlt, glbDouble& maxAlt)
{
	east = mpr_east;
	west = mpr_west;
	south = mpr_south;
	north = mpr_north;
	minAlt = mpr_minalt;
	maxAlt = mpr_maxalt;
}

CGlbExtent* CGlbGlobeTerrainTile::GetExtent()
{
	return mpr_extent.get();
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetParentTile()
{
	return mpr_parent_tile.get();
}
glbBool CGlbGlobeTerrainTile::IsVisible()
{	
	if (!mpr_terrain) return false;
	CGlbGlobeView* p_globeview = mpr_terrain->mpr_globe->GetView();
	if (!p_globeview) 
		return false;
	
	glbBool bVisible = false;
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{// Globe地形		
		if (mpr_level==0)
		{
			osg::Vec3d camerapos,eye;
			p_globeview->GetCameraPos(camerapos);

			if ( (camerapos.x()-mpr_west)*(camerapos.x()-mpr_east)<=0 &&
				 (camerapos.y()-mpr_south)*(camerapos.y()-mpr_north)<=0 )
				 bVisible = true;
			if (!bVisible)
			{
				if (p_globeview->contains(mpr_boundbox))
					bVisible = true;
			}

			//double alt = camerapos.z();
			//if (alt<0)
			//	alt = -alt;

			//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(camerapos.y()),osg::DegreesToRadians(camerapos.x()),camerapos.z(),
			//											eye.x(),eye.y(),eye.z());

			//double vdis = sqrt(eye.length2()-osg::WGS_84_RADIUS_EQUATOR*osg::WGS_84_RADIUS_EQUATOR);
			//double dis = (eye-mpr_boundbox.center()).length() - mpr_boundbox.radius();		

			//// 相机距离地形块距离超出可见距离，则必定不可见
			//if (dis > vdis)
			//	return false;

			bVisible = true;
		}
		else
		{			
			// 2014.1.10 改为用视锥判断可见性 
			if (p_globeview->contains(mpr_boundbox))
				bVisible = true;		
		}		
	}
	else if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
	{// Flat地形
		glbDouble w,e,s,n;
		mpr_terrain->GetExtent(w,e,s,n);
		if (mpr_east <= w || mpr_west >= e || mpr_south >= n || mpr_north <= s)
		{
			bVisible = false;
		}
		else if (p_globeview->contains(mpr_boundbox))
			bVisible = true;		
	}
	return bVisible;
}
GlbGlobeChildLocationEnum CGlbGlobeTerrainTile::GetTileLocation()
{
	return mpr_location_enum;
}
osg::Texture2D* CGlbGlobeTerrainTile::GetTexture()
{
	return mpr_texture.get();
}
void CGlbGlobeTerrainTile::GetDemdata(glbFloat** p_data, glbInt32 &dem_columns, glbInt32 &dem_rows)
{
	//if (mpr_overlaydemdata)
	//	*p_data = mpr_overlaydemdata;
	//else
	*p_data = mpr_demdata;
	dem_columns = mpr_demsize;
	dem_rows = mpr_demsize;
}
void CGlbGlobeTerrainTile::GetDomdata(glbByte** p_data, glbInt32 &dom_columns, glbInt32 &dom_rows)
{
	//if (mpr_overlayImage)
	//	*p_data = mpr_overlayImage;
	//else
	*p_data = mpr_image;
	dom_columns = mpr_imagesize;
	dom_rows = mpr_imagesize;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetNorthEastChild()
{
	return mpr_nechild.get();
}
CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetNorthWestChild()
{
	return mpr_nwchild.get();
}
CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetSouthEastChild()
{
	return mpr_sechild.get();
}
CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetSouthWestChild()
{
	return mpr_swchild.get();
}
osg::Vec3dArray* CGlbGlobeTerrainTile::GetEdgeVertexArray(GlbTileEdgeEnum edge)
{
	glbInt32 k = 0;
	osg::Vec3dArray* p_parent_edge_half = NULL;
	switch(edge)
	{
	case GLB_WESTEDGE_NORTHSECTION:
		{
			if (!mpr_p_west_northhalf_vertexarray)	{				// mpr_isInitialized == FALSE,本身没有数据，从上级获取
				mpr_p_west_northhalf_vertexarray = new osg::Vec3dArray;
				switch(mpr_location_enum)
				{
				case GLB_NORTHWEST:{
					osg::Vec3dArray* p_parent_west_northhalf = mpr_parent_tile->GetEdgeVertexArray(GLB_WESTEDGE_NORTHSECTION);
					for (k = 0; k < (glbInt32)p_parent_west_northhalf->size()/2; k++){
						mpr_p_west_northhalf_vertexarray->push_back(p_parent_west_northhalf->at(k));
						mpr_p_west_northhalf_vertexarray->push_back((p_parent_west_northhalf->at(k)+p_parent_west_northhalf->at(k+1))*0.5);
					}
					mpr_p_west_northhalf_vertexarray->push_back(p_parent_west_northhalf->at((glbInt32)p_parent_west_northhalf->size()/2));
					}
					break;
				case GLB_NORTHEAST:{
					osg::Vec3dArray* p_parent_center_northhalf = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_NORTHSECTION);
					for (k = 0; k < (glbInt32)p_parent_center_northhalf->size()/2; k++){
						mpr_p_west_northhalf_vertexarray->push_back(p_parent_center_northhalf->at(k));
						mpr_p_west_northhalf_vertexarray->push_back((p_parent_center_northhalf->at(k)+p_parent_center_northhalf->at(k+1))*0.5);
					}
					mpr_p_west_northhalf_vertexarray->push_back(p_parent_center_northhalf->at((long)p_parent_center_northhalf->size()/2));
					}
					break;
				case GlB_SOUTHWEST:{
					osg::Vec3dArray* p_parent_west_northhalf = mpr_parent_tile->GetEdgeVertexArray(GLB_WESTEDGE_SOUTHSECTION);
					for (k = 0; k < (glbInt32)p_parent_west_northhalf->size()/2; k++){
						mpr_p_west_northhalf_vertexarray->push_back(p_parent_west_northhalf->at(k));
						mpr_p_west_northhalf_vertexarray->push_back((p_parent_west_northhalf->at(k)+p_parent_west_northhalf->at(k+1))*0.5);
					}
					mpr_p_west_northhalf_vertexarray->push_back(p_parent_west_northhalf->at((glbInt32)p_parent_west_northhalf->size()/2));
					}
					break;
				case GLB_SOUTHEAST:{
					osg::Vec3dArray* p_parent_center_northhalf = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_SOUTHSECTION);
					for (k = 0; k < (glbInt32)p_parent_center_northhalf->size()/2; k++){
						mpr_p_west_northhalf_vertexarray->push_back(p_parent_center_northhalf->at(k));
						mpr_p_west_northhalf_vertexarray->push_back((p_parent_center_northhalf->at(k)+p_parent_center_northhalf->at(k+1))*0.5);
					}
					mpr_p_west_northhalf_vertexarray->push_back(p_parent_center_northhalf->at((glbInt32)p_parent_center_northhalf->size()/2));
					}
					break;
				}
			}
			return  mpr_p_west_northhalf_vertexarray.get();		
		}		
	case GLB_WESTEDGE_SOUTHSECTION:
		{
			if (!mpr_p_west_southhalf_vertexarray){
				mpr_p_west_southhalf_vertexarray = new osg::Vec3dArray;
				switch(mpr_location_enum)
				{
				case GLB_NORTHWEST:{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_WESTEDGE_NORTHSECTION);
					for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
						mpr_p_west_southhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_west_southhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_west_southhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GLB_NORTHEAST:{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_NORTHSECTION);
					for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
						mpr_p_west_southhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_west_southhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_west_southhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GlB_SOUTHWEST:{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_WESTEDGE_SOUTHSECTION);
					for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
						mpr_p_west_southhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_west_southhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_west_southhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GLB_SOUTHEAST:{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_SOUTHSECTION);
					for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
						mpr_p_west_southhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_west_southhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_west_southhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				}
			}
			return mpr_p_west_southhalf_vertexarray.get();
		}		
	case GLB_EASTEDGE_NORTHSECTION:
		{		
			if (!mpr_p_east_northhalf_vertexarray){			
				mpr_p_east_northhalf_vertexarray = new osg::Vec3dArray;
				switch(mpr_location_enum)
				{
				case GLB_NORTHWEST:	{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_NORTHSECTION);
					for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
						mpr_p_east_northhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_east_northhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_east_northhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				case GLB_NORTHEAST:{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_EASTEDGE_NORTHSECTION);
					for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
						mpr_p_east_northhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_east_northhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_east_northhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				case GlB_SOUTHWEST:{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_SOUTHSECTION);
					for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
						mpr_p_east_northhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_east_northhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_east_northhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				case GLB_SOUTHEAST:{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_EASTEDGE_SOUTHSECTION);
					for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
						mpr_p_east_northhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_east_northhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_east_northhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				}
			}		
			return mpr_p_east_northhalf_vertexarray.get();
		}		
	case GLB_EASTEDGE_SOUTHSECTION:
		{
			if (!mpr_p_east_southhalf_vertexarray){
				mpr_p_east_southhalf_vertexarray = new osg::Vec3dArray;
				switch(mpr_location_enum)
				{
				case GLB_NORTHWEST:	{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_NORTHSECTION);
					for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
						mpr_p_east_southhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_east_southhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_east_southhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GLB_NORTHEAST:{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_EASTEDGE_NORTHSECTION);
					for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
						mpr_p_east_southhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_east_southhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_east_southhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GlB_SOUTHWEST:{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_SOUTHSECTION);
					for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
						mpr_p_east_southhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_east_southhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_east_southhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GLB_SOUTHEAST:{
					p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_EASTEDGE_SOUTHSECTION);
					for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
						mpr_p_east_southhalf_vertexarray->push_back(p_parent_edge_half->at(k));
						mpr_p_east_southhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
					}
					mpr_p_east_southhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				}
			}
			return mpr_p_east_southhalf_vertexarray.get();
		}		
	case GLB_NORTHEDGE_EASTSECTION:
		{
			if (!mpr_p_north_easthalf_vertexarray){
				mpr_p_north_easthalf_vertexarray = new osg::Vec3dArray;
				switch(mpr_location_enum)
				{
				case GLB_NORTHWEST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_NORTHEDGE_WESTSECTION);
						for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
							mpr_p_north_easthalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_north_easthalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_north_easthalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GLB_NORTHEAST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_NORTHEDGE_EASTSECTION);
						for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
							mpr_p_north_easthalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_north_easthalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_north_easthalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GlB_SOUTHWEST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_WESTSECTION);
						for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
							mpr_p_north_easthalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_north_easthalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_north_easthalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GLB_SOUTHEAST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_EASTSECTION);
						for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
							mpr_p_north_easthalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_north_easthalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_north_easthalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				}
			}
			return mpr_p_north_easthalf_vertexarray.get();
		}		
	case GLB_NORTHEDGE_WESTSECTION:
		{
			if (!mpr_p_north_westhalf_vertexarray){
				mpr_p_north_westhalf_vertexarray = new osg::Vec3dArray;
				switch(mpr_location_enum)
				{
				case GLB_NORTHWEST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_NORTHEDGE_WESTSECTION);
						for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
							mpr_p_north_westhalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_north_westhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_north_westhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				case GLB_NORTHEAST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_NORTHEDGE_EASTSECTION);
						for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
							mpr_p_north_westhalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_north_westhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_north_westhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				case GlB_SOUTHWEST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_WESTSECTION);
						for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
							mpr_p_north_westhalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_north_westhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_north_westhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				case GLB_SOUTHEAST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_EASTSECTION);
						for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
							mpr_p_north_westhalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_north_westhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_north_westhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				}
			}
			return mpr_p_north_westhalf_vertexarray.get();
		}		
	case GLB_SOUTHEDGE_EASTSECTION:
		{
			if (!mpr_p_south_easthalf_vertexarray){
				mpr_p_south_easthalf_vertexarray = new osg::Vec3dArray;
				switch(mpr_location_enum)
				{
				case GLB_NORTHWEST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_WESTSECTION);
						for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
							mpr_p_south_easthalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_south_easthalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_south_easthalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GLB_NORTHEAST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_EASTSECTION);
						for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
							mpr_p_south_easthalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_south_easthalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_south_easthalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GlB_SOUTHWEST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_WESTSECTION);
						for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
							mpr_p_south_easthalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_south_easthalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_south_easthalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				case GLB_SOUTHEAST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_EASTSECTION);
						for (k = (glbInt32)p_parent_edge_half->size()/2; k < (glbInt32)p_parent_edge_half->size()-1; k++){
							mpr_p_south_easthalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_south_easthalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_south_easthalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()-1));
					}
					break;
				}
			}
			return mpr_p_south_easthalf_vertexarray.get();
		}		
	case GLB_SOUTHEDGE_WESTSECTION:
		{
			if (!mpr_p_south_westhalf_vertexarray){
				mpr_p_south_westhalf_vertexarray = new osg::Vec3dArray;
				switch(mpr_location_enum)
				{
				case GLB_NORTHWEST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_WESTSECTION);
						for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
							mpr_p_south_westhalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_south_westhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_south_westhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				case GLB_NORTHEAST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_CENTEREDGE_EASTSECTION);
						for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
							mpr_p_south_westhalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_south_westhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_south_westhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				case GlB_SOUTHWEST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_WESTSECTION);
						for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
							mpr_p_south_westhalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_south_westhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_south_westhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				case GLB_SOUTHEAST:
					{
						p_parent_edge_half = mpr_parent_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_EASTSECTION);
						for (k = 0; k < (glbInt32)p_parent_edge_half->size()/2; k++){
							mpr_p_south_westhalf_vertexarray->push_back(p_parent_edge_half->at(k));
							mpr_p_south_westhalf_vertexarray->push_back((p_parent_edge_half->at(k)+p_parent_edge_half->at(k+1))*0.5);
						}
						mpr_p_south_westhalf_vertexarray->push_back(p_parent_edge_half->at((glbInt32)p_parent_edge_half->size()/2));
					}
					break;
				}
			}
			return mpr_p_south_westhalf_vertexarray.get();	
		}		
	case GLB_CENTEREDGE_WESTSECTION:
		return mpr_p_center_westhalf_vertexarray.get();
	case GLB_CENTEREDGE_EASTSECTION:
		return mpr_p_center_easthalf_vertexarray.get();
	case GLB_CENTEREDGE_NORTHSECTION:
		return mpr_p_center_northhalf_vertexarray.get();
	case GLB_CENTEREDGE_SOUTHSECTION:
		return mpr_p_center_southhalf_vertexarray.get();
	}
	return NULL;
}
glbBool CGlbGlobeTerrainTile::SetOpacity(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,                   
											std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainObjs,
											glbInt32 opacity)
{
	//WCHAR wBuff[128];
	//if (true)
	//{		
	//  swprintf(wBuff,L"  ****SetOpacity %d . [Level: %d , row: %d, col: %d , isInit: %d , isDataLoading: %d, isDataLoaded: %d]\n",opacity,mpr_level,mpr_row,mpr_column, mpr_isinitialized, mpr_isLoadingData,mpr_isDataLoaded)
	////wsprintf(wBuff,L"  ****SetOpacity %d . [Level: %d , row: %d, col: %d , isInit: %d , isDataLoading: %d, isDataLoaded: %d]\n",opacity,mpr_level,mpr_row,mpr_column, mpr_isinitialized, mpr_isLoadingData,mpr_isDataLoaded);
	//	OutputDebugString(wBuff);
	//}	

	
	if (mpr_isDataLoaded==false)		
		return true;	

	if (mpr_opacity==opacity)
		return true;

	mpr_opacity = opacity;
	
	glbref_ptr<CGlbGlobeTerrainTile> p_northwestchild = GetNorthWestChild();
	if (p_northwestchild /*&& p_northwestchild->IsInitialized()*/)
	{
		std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
		p_northwestchild->FilterOnTerrainObjects(terrainObjs,childtobjs);		
		p_northwestchild->SetOpacity(domlayers,childtobjs,opacity);
	}

	glbref_ptr<CGlbGlobeTerrainTile> p_northeastchild = GetNorthEastChild();
	if (p_northeastchild /*&& p_northeastchild->IsInitialized()*/)
	{		
		std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
		p_northeastchild->FilterOnTerrainObjects(terrainObjs,childtobjs);	
		p_northeastchild->SetOpacity(domlayers,childtobjs,opacity);
	}
	
	glbref_ptr<CGlbGlobeTerrainTile> p_southwestchild = GetSouthWestChild();	
	if (p_southwestchild /*&& p_southwestchild->IsInitialized()*/)
	{
		std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
		p_southwestchild->FilterOnTerrainObjects(terrainObjs,childtobjs);	
		p_southwestchild->SetOpacity(domlayers,childtobjs,opacity);		
	}

	glbref_ptr<CGlbGlobeTerrainTile> p_southeastchild = GetSouthEastChild();
	if (p_southeastchild/* && p_southeastchild->IsInitialized()*/)
	{
		std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
		p_southeastchild->FilterOnTerrainObjects(terrainObjs,childtobjs);	
		p_southeastchild->SetOpacity(domlayers,childtobjs,opacity);	
	}
	
	if (mpr_p_osgImg)
	{// 修改image的alpha通道值
		unsigned int imagesize = mpr_p_osgImg->getImageSizeInBytes();
		BYTE alpha = opacity * 255 / 100;

		if (mpr_image)
		{
#if 0
EnterCriticalSection(&(mpr_terrain->mpr_tDatasetAccessCritical));
			CGlbPixelBlock* p_dompixelblock = mpr_terrain->GetTerrainDataset()->CreateDomPixelBlock();
			glbBool accessdomsuccess = mpr_terrain->GetTerrainDataset()->ReadDom(mpr_level,mpr_column,mpr_row,p_dompixelblock);
LeaveCriticalSection(&(mpr_terrain->mpr_tDatasetAccessCritical));
			glbByte* ppMask = NULL;//mask数据【255表示有效数】
			p_dompixelblock->GetMaskData(&ppMask);

			glbInt32 _rows,_columns;
			p_dompixelblock->GetSize(_columns,_rows);
			void* _val=NULL;
			for (glbInt32 i = 0; i < _rows; i++)
				for(glbInt32 j = 0; j < _columns; j++)
				{
					glbInt32 idx = i*_columns+j;
					if (ppMask[idx]==255)
					{// 有效值[255]时，可以替换源dom数据
						mpr_image[4*idx+3] = alpha;
					}
				}

			if (p_dompixelblock)
				delete p_dompixelblock;
#else
			for (glbInt32 k =0 ; k < mpr_imagesize*mpr_imagesize; k++)
				mpr_image[4*k+3] = (mpr_origin_image[4*k+3] < 5) ? 0 : alpha;
#endif
		}

		// 重新生成mpr_overlayImage
		bool bNeedLoadDomLayer = false;		
		for (size_t kk = 0; kk < domlayers.size(); kk++)
		{
			glbref_ptr<CGlbGlobeDomLayer> domlayer = domlayers.at(kk);
			glbref_ptr<CGlbExtent> ext = domlayer->GetBound(false);

			glbref_ptr<CGlbExtent> interExt = ext->Intersect(*mpr_extent.get(),false);
			if (interExt)
			{
				bNeedLoadDomLayer = true;
				break;
			}
		}

		if (terrainObjs.size()>0 || bNeedLoadDomLayer)
		{
			glbBool res = LoadOverlayTextureData(domlayers,terrainObjs);
		}
		else
		{
			memcpy(mpr_p_osgImg->data(),mpr_image,imagesize);	
		}
		
		// 将mpr_p_osgImg设置给mpr_textrue,刷新纹理
		//mpr_p_osgImg->dirty();
		//if (mpr_texture)
		//{
		//	mpr_texture->setImage(0,mpr_p_osgImg);		
		//	mpr_texture->dirtyTextureObject();
		//}
		CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_TEXTURE,mpr_texture.get(),mpr_p_osgImg.get());
		mpr_terrain->AddOperation(_opera);
	}	
	
	//// 修改colorarry的alpha
	//float alpha = opacity / 100.0f;	
	//size_t numDrawable = mpr_p_geode->getNumDrawables();
	//for (size_t j = 0; j < numDrawable; j++)
	//{
	//	osg::Geometry* p_geomtry = dynamic_cast<osg::Geometry*> (mpr_p_geode->getDrawable(j));
	//	if (p_geomtry)
	//	{
	//		osg::Vec4Array* p_colorarray = dynamic_cast<osg::Vec4Array*> (p_geomtry->getColorArray());
	//		if (p_colorarray==NULL) continue;
	//		for (long k = 0; k < long(p_colorarray->size()); k++)
	//		{
	//			osg::Vec4 clr = p_colorarray->at(k);
	//			p_colorarray->at(k).set(clr.r(),clr.g(),clr.b(),alpha);
	//		}					
	//		p_geomtry->setColorArray(p_colorarray);
	//		//p_geomtry->dirtyDisplayList();
	//	}
	//}

	return true;
}

glbBool CGlbGlobeTerrainTile::SetExaggrate(glbDouble exaggrate)
{
	if (!mpr_isinitialized)
	{// 没有初始化清空接边所产生的边界顶点数组
		mpr_p_north_easthalf_vertexarray=NULL;
		mpr_p_north_westhalf_vertexarray=NULL;
		mpr_p_south_easthalf_vertexarray=NULL;
		mpr_p_south_westhalf_vertexarray=NULL;
		mpr_p_east_northhalf_vertexarray=NULL;
		mpr_p_east_southhalf_vertexarray=NULL;
		mpr_p_west_northhalf_vertexarray=NULL;
		mpr_p_west_southhalf_vertexarray=NULL;
		mpr_p_center_westhalf_vertexarray=NULL;
		mpr_p_center_easthalf_vertexarray=NULL;
		mpr_p_center_northhalf_vertexarray=NULL;
		mpr_p_center_southhalf_vertexarray=NULL;		
		return false;
	}

	glbref_ptr<CGlbGlobeTerrainTile> p_northwestchild = GetNorthWestChild();
	if (p_northwestchild)
		p_northwestchild->SetExaggrate(exaggrate);

	glbref_ptr<CGlbGlobeTerrainTile> p_northeastchild = GetNorthEastChild();
	if (p_northeastchild)
		p_northeastchild->SetExaggrate(exaggrate);

	glbref_ptr<CGlbGlobeTerrainTile> p_southwestchild = GetSouthWestChild();	
	if (p_southwestchild)
		p_southwestchild->SetExaggrate(exaggrate);

	glbref_ptr<CGlbGlobeTerrainTile> p_southeastchild = GetSouthEastChild();
	if (p_southeastchild)
		p_southeastchild->SetExaggrate(exaggrate);

	// 算法 : 修改geometry中的顶点坐标
	glbInt32 thisVertexCount = mpr_vertex_count / 2 + (mpr_vertex_count % 2);
	glbInt32 thisVertexCountPlus1 = (thisVertexCount%2 == 0) ? thisVertexCount + 1 : thisVertexCount;	
	glbFloat* _pDemdata = NULL;
	if (mpr_overlaydemdata)
		_pDemdata = mpr_overlaydemdata;
	else
		_pDemdata = mpr_demdata;

	glbFloat _geoheight = GetHeightData(_pDemdata, thisVertexCountPlus1-1, thisVertexCountPlus1-1, GLB_NORTHWEST) * exaggrate;

	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
		g_ellipsmodel->convertLatLongHeightToXYZ(
			mpr_center_latitude , mpr_center_longitude,_geoheight,//0.0,
			mpr_local_origin.x(), mpr_local_origin.y(), mpr_local_origin.z() );
	}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		mpr_local_origin.set(0.5*(mpr_west+mpr_east), 0.5*(mpr_south+mpr_north),_geoheight);
	}

	// 2. 重新生成4个子geometry
	RebuildElevatedMesh();

	// 3. 重设偏移矩阵
	if (mpr_node->getNumChildren()>0)
	{
		osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(mpr_node->getChild(0));
		if (mt)
		{
			osg::Matrixd matrixlocalOrigin;
			matrixlocalOrigin.makeTranslate(mpr_local_origin);
			mt->setMatrix(matrixlocalOrigin);
		}
	}

	return true;
}

CGlbGlobeTerrain* CGlbGlobeTerrainTile::GetTerrain()
{
	return mpr_terrain;
}
osg::BoundingBox CGlbGlobeTerrainTile::AAA(glbDouble childSouth,glbDouble childNorth,glbDouble childWest,glbDouble childEast, glbDouble childMinHei, glbDouble childMaxHei,glbInt32 level, glbInt32 row, glbInt32 column, osg::Polytope cv,glbDouble min_southDegree, glbDouble max_norhtDegree,glbInt32 loopdepth)
{
	//#ifdef _DEBUG	
	//	WCHAR wBuff[128];
	//	wsprintf(wBuff,L"  AAA begin. [Level: %d , row: %d, col: %d]\n",level,row,column);
	//	OutputDebugString(wBuff);
	//	//CGlbSysLog::WriteLog(wBuff);
	//	//_swprintf(wBuff,L"south:%.3f, north: %.3f, west: %.3f, east: %.3f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//	//CGlbSysLog::WriteLog(wBuff);	
	//#endif
	osg::BoundingBox _bb;
	glbDouble w,e,s,n;
	mpr_terrain->GetExtent(w,e,s,n);
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		s = min_southDegree;
		n = max_norhtDegree;
	}
	if (childEast <= w || childWest >= e || childSouth >= n || childNorth <= s)
	{// 非地形有效区域，返回无效bbox
		return _bb;		 
	}

	if (loopdepth<=0)
	//if (loopdepth<=0 && level>7)//并且至少要分裂到14级
	{// 剩余迭代次数<=0 
		_bb.set(childWest,childSouth,childMinHei,childEast,childNorth,childMaxHei);
		return _bb;
	}
	osg::BoundingBox bb;	

	osg::Vec3d corners[8];
	std::vector<osg::Vec3d> pointvec;
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(childNorth),osg::DegreesToRadians(childWest),childMinHei,corners[0].x(),corners[0].y(),corners[0].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(childSouth),osg::DegreesToRadians(childWest),childMinHei,corners[1].x(),corners[1].y(),corners[1].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(childSouth),osg::DegreesToRadians(childEast),childMinHei,corners[2].x(),corners[2].y(),corners[2].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(childNorth),osg::DegreesToRadians(childEast),childMinHei,corners[3].x(),corners[3].y(),corners[3].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(childNorth),osg::DegreesToRadians(childWest),childMaxHei,corners[4].x(),corners[4].y(),corners[4].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(childSouth),osg::DegreesToRadians(childWest),childMaxHei,corners[5].x(),corners[5].y(),corners[5].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(childSouth),osg::DegreesToRadians(childEast),childMaxHei,corners[6].x(),corners[6].y(),corners[6].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(childNorth),osg::DegreesToRadians(childEast),childMaxHei,corners[7].x(),corners[7].y(),corners[7].z());	
	}
	else
	{
		corners[0].set(childWest,childNorth,childMinHei);
		corners[1].set(childWest,childSouth,childMinHei);
		corners[2].set(childEast,childNorth,childMinHei);
		corners[3].set(childEast,childSouth,childMinHei);
		corners[4].set(childWest,childNorth,childMaxHei);
		corners[5].set(childWest,childSouth,childMaxHei);
		corners[6].set(childEast,childNorth,childMaxHei);
		corners[7].set(childEast,childSouth,childMaxHei);		
	}	

	//CGlbGlobeCamera* _p_camara = mpr_p_globe_terrain->GetCamera();
	//osg::Vec3d _eye = _p_camara->GetPosition();
	//bool bFaceCircle=false;
	for (long i = 0; i < 8; i++){		
		bb.expandBy(corners[i]);		
		//if (!bFaceCircle){
		//	if ((corners[i] * (_eye-corners[i]) >0))
		//		bFaceCircle = true;
		//}
	}


	bool bAllIn = cv.containsAllOf(bb);
	if (bAllIn)// && bFaceCircle)
	{// 8个点都在视域范围内，则不需要细分子节点		
		bb.set(childWest,childSouth,childMinHei,childEast,childNorth,childMaxHei);
		return bb;
	}
	else if (cv.contains(bb))// && bFaceCircle)
	{// 部分在
		double CenterLat = 0.5f * (childSouth + childNorth);
		double CenterLon = 0.5f * (childEast + childWest);

		glbInt32 childLevel = level+1;

		glbInt32 r2 = row*2;
		glbInt32 c2 = column*2;

		long next_loop_depth = loopdepth - 1;

		osg::BoundingBox northeast_bb;				
		northeast_bb = AAA(CenterLat, childNorth, CenterLon, childEast,childMinHei,childMaxHei,childLevel,r2+1,c2+1,cv,min_southDegree,max_norhtDegree,next_loop_depth);	// GLB_NORTHEAST

		osg::BoundingBox northwest_bb;		
		northwest_bb = AAA(CenterLat, childNorth, childWest, CenterLon,childMinHei,childMaxHei,childLevel,r2+1,c2,cv,min_southDegree,max_norhtDegree,next_loop_depth);  // GLB_NORTHWEST

		osg::BoundingBox southwest_bb;		
		southwest_bb = AAA(childSouth, CenterLat, childWest, CenterLon,childMinHei,childMaxHei,childLevel,r2,c2,cv,min_southDegree,max_norhtDegree,next_loop_depth); // GLB_SOUTHWEST

		osg::BoundingBox southeast_bb;		
		southeast_bb = AAA(childSouth, CenterLat, CenterLon, childEast,childMinHei,childMaxHei,childLevel,r2,c2+1,cv,min_southDegree,max_norhtDegree,next_loop_depth); // GLB_SoUTHEAST

		osg::BoundingBox _bb;
		if (northeast_bb.valid())
			_bb.expandBy(northeast_bb);
		if (northwest_bb.valid())
			_bb.expandBy(northwest_bb);
		if (southeast_bb.valid())
			_bb.expandBy(southeast_bb);
		if (southwest_bb.valid())
			_bb.expandBy(southwest_bb);
		return _bb;
	}
	else
	{// 全不在		
		return _bb;
	}	
	return _bb;
}

glbDouble CGlbGlobeTerrainTile::SphericalDistance(glbDouble latA, glbDouble lonA, glbDouble latB, glbDouble lonB)
{
	glbDouble radLatA = latA;
	glbDouble radLatB = latB;
	glbDouble radLonA = lonA;
	glbDouble radLonB = lonB;

	return acos(
		cos(radLatA)*cos(radLatB)*cos(radLonA-radLonB)+
		sin(radLatA)*sin(radLatB));
}

void CGlbGlobeTerrainTile::Initialize(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
										std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
										std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs, 
										std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs,
										glbBool	load_direct)
{// 地形块初始化
	if (mpr_isinitialized)
		return;

	if (load_direct)
	{
		mpr_isLoadingData = true;
		LoadData(domlayers,demlayers,terrainobjs,mTerrainobjs);
		// 生成地形块的节点及子节点geometry，赋值texture
		mpr_node = CreateElevatedMesh();

		osg::Group* p_terrainnode = dynamic_cast<osg::Group*>(mpr_terrain->GetNode());
		p_terrainnode->addChild(mpr_node);
		mpr_isinitialized = true;
	}
	else
	{		
		if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
		{// 两极位置只更新到第3级 （4.5度）2012.12.14 //北极圈 66.5度
			glbDouble polarLimit = 85.4; //87.74; // 第4级
			if (fabs(mpr_south) > polarLimit  &&  fabs(mpr_north) > polarLimit)
				return;	
		}	

		if (mpr_isDataLoaded)
		{// 数据已经读取完毕
			osg::Timer_t startFrameTick = osg::Timer::instance()->tick();

			// 生成地形块的节点及子节点geometry，赋值texture
			mpr_node = CreateElevatedMesh();
			// 预先计算bound，节省updatetravel中的时间
			mpr_node->getBound();

			// 计算draw过程中编译mpr_node中的gemery和texture生成显示列表需要的时间(毫秒)-计算耗费时间同时发现有崩溃问题，废弃不用！
			//osg::ref_ptr<osg::GraphicsCostEstimator> gce = new osg::GraphicsCostEstimator;
			//osg::CostPair compileCost = gce->estimateCompileCost(mpr_node.get());
			//double compileNodeTime = compileCost.first;
			double compileNodeTime = 0.002; //2毫秒的渲染时间

			// 生成一个挂操作
			CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_ADD,CGlbGlobeTerrain::GLB_NODE,mpr_node,compileNodeTime);
			mpr_terrain->AddOperation(_opera);	

			mpr_isinitialized = true;

			//if (false)
			//{// 	
			//	osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
			//	double frameTime = osg::Timer::instance()->delta_m(startFrameTick, endFrameTick);

			//	char buff[256];
			//	sprintf(buff,"**Tile initialized CreateElevatedMesh end. [Level: %d , row: %d, col: %d] use time %lf.",mpr_level,(long)mpr_row,(long)mpr_column,frameTime);
			//	LOG(buff);
			//	//GlbLogWOutput(GLB_LOGTYPE_INFO,L"**Tile initialized. [Level: %d , row: %d, col: %d]\r\n",mpr_level,(long)mpr_row,(long)mpr_column);
			//}
		}	
		else if (!mpr_isLoadingData)
		{// 没有读数据
			mpr_isLoadingData = true;
//#ifdef _DEBUG
//			WCHAR wBuff[128];
//			wsprintf(wBuff,L"	Initialize(). [Level: %d , row: %d, col: %d, mTerrainobjs :%d]\n",mpr_level,(long)mpr_row,(long)mpr_column,mTerrainobjs.size());
//			OutputDebugString(wBuff);		
//#endif
			// 创建一个加载任务			
			CGlbGlobeTerrainTileRequestTask* task = new CGlbGlobeTerrainTileRequestTask(this,domlayers,demlayers,terrainobjs,mTerrainobjs);
			mpr_terrain->mpr_globe->mpr_taskmanager->AddTerrainTask(task);

			mpr_terrainTask = task;
			//if (false)
			//{// 	
			//	char buff[256];
			//	sprintf(buff,"**Tile initialized Create LoadTask. [Level: %d , row: %d, col: %d].",mpr_level,(long)mpr_row,(long)mpr_column);
			//	LOG(buff);
			//	//GlbLogWOutput(GLB_LOGTYPE_INFO,L"**Tile initialized. [Level: %d , row: %d, col: %d]\r\n",mpr_level,(long)mpr_row,(long)mpr_column);
			//}
		}
	}
}

glbInt32 CGlbGlobeTerrainTile::GetRowFromLatitude(glbDouble latitude, glbDouble tileSize)
{
	if (latitude > 90.0)
		latitude = 90.0;
	if (latitude < -90.0)
		latitude = -90.0;	
	//return (glbInt32)(fmod(abs(-90.0 - latitude),180)/tileSize);
	return (glbInt32)(fmod(abs(latitude - 90.0),180)/tileSize);
}

glbInt32 CGlbGlobeTerrainTile::GetColFromLongitude(glbDouble longitude, glbDouble tileSize)
{
	if (longitude > 180.0)
		longitude = -360.0 + longitude;
	if (longitude < -180.0)
		longitude =  360.0 + longitude;
	return (glbInt32)(fmod(abs(-180.0 - longitude),360)/tileSize);
}

void CGlbGlobeTerrainTile::ComputeChildren()
{
	if (mpr_level + 1 > MAX_LEVEL)//不能超过最大等级
		return;
	glbDouble CenterLat = 0.5f * (mpr_south + mpr_north);
	glbDouble CenterLon = 0.5f * (mpr_east + mpr_west);

	if (mpr_nwchild== NULL)
	{	
		mpr_nwchild = new CGlbGlobeTerrainTile(mpr_terrain,mpr_level+1,CenterLat,mpr_north,mpr_west,CenterLon,mpr_minalt,mpr_maxalt,this);
		if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
			mpr_nwchild->SetPosition(mpr_row*2+1,mpr_column*2);		
		mpr_nwchild->mpr_location_enum = GLB_NORTHWEST;			
	}
	if (mpr_nechild == NULL)
	{
		mpr_nechild = new CGlbGlobeTerrainTile(mpr_terrain,mpr_level+1,CenterLat,mpr_north,CenterLon,mpr_east,mpr_minalt,mpr_maxalt,this);
		if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
			mpr_nechild->SetPosition(mpr_row*2+1,mpr_column*2+1);
		mpr_nechild->mpr_location_enum = GLB_NORTHEAST;
	}
	if (mpr_swchild == NULL)
	{
		mpr_swchild = new CGlbGlobeTerrainTile(mpr_terrain,mpr_level+1,mpr_south,CenterLat,mpr_west,CenterLon,mpr_minalt,mpr_maxalt,this);
		if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
			mpr_swchild->SetPosition(mpr_row*2,mpr_column*2);
		mpr_swchild->mpr_location_enum = GlB_SOUTHWEST;
	}
	if (mpr_sechild == NULL)
	{
		mpr_sechild = new CGlbGlobeTerrainTile(mpr_terrain,mpr_level+1,mpr_south,CenterLat,CenterLon,mpr_east,mpr_minalt,mpr_maxalt,this);
		if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
			mpr_sechild->SetPosition(mpr_row*2,mpr_column*2+1);
		mpr_sechild->mpr_location_enum = GLB_SOUTHEAST;		
	}
}

glbFloat* CGlbGlobeTerrainTile::MultifyDemSection(glbFloat* p_data, glbInt32 dem_columns, glbInt32 dem_rows, GlbGlobeChildLocationEnum location)
{	// dem_columns==dem_rows==17 - 插值算法
	glbFloat * p_new_data = new glbFloat[dem_rows*dem_columns];
	memset(p_new_data,0,sizeof(glbFloat)*dem_rows*dem_columns);

	glbInt32 dem_half_rows = dem_rows / 2 + (dem_rows % 2);		//9
	glbInt32 dem_half_columns = dem_columns / 2 + (dem_columns % 2); // 9

	// step 1 : 写真实数据
	for (glbInt32 i = 0 ; i < dem_half_rows; i++)
		for(glbInt32 j = 0; j < dem_half_columns; j++)
		{			
			switch(location)
			{
			case GLB_NORTHWEST:	//西北
				{
					p_new_data[2*i*dem_columns+j*2] = p_data[i*dem_columns+j];		
					if (j < dem_half_columns-1){
						p_new_data[2*i*dem_columns+j*2+1] = (p_data[i*dem_columns+j] + p_data[i*dem_columns+j+1]) *0.5;	
					}				
				}
				break;
			case GLB_NORTHEAST:	//东北			
				{
					p_new_data[2*i*dem_columns+j*2] = p_data[i*dem_columns+j+dem_half_columns-1];	
					if (j < dem_half_columns-1){
						p_new_data[2*i*dem_columns+j*2+1] = (p_data[i*dem_columns+j+dem_half_columns-1] + p_data[i*dem_columns+j+dem_half_columns]) *0.5;	
					}
				}							
				break;
			case GlB_SOUTHWEST: //西南
				{
					p_new_data[2*i*dem_columns+j*2] = p_data[(i+dem_half_rows-1)*dem_columns+j];
					if (j < dem_half_columns-1){
						p_new_data[2*i*dem_columns+j*2+1] = (p_data[(i+dem_half_rows-1)*dem_columns+j] + p_data[(i+dem_half_rows-1)*dem_columns+j+1]) *0.5;	
					}
				}								
				break;
			case GLB_SOUTHEAST: //东南
				{
					p_new_data[2*i*dem_columns+j*2] = p_data[(i+dem_half_rows-1)*dem_columns+j+dem_half_columns-1];
					if (j < dem_half_columns-1){
						p_new_data[2*i*dem_columns+j*2+1] = (p_data[(i+dem_half_rows-1)*dem_columns+j+dem_half_columns-1] + p_data[(i+dem_half_rows-1)*dem_columns+j+dem_half_columns]) *0.5;	
					}
				}				
				break;
			}
		}
	// step 2 ： 插值中间数据	
	for (glbInt32 i = 1 ; i < dem_rows; i+=2)
		for(glbInt32 j = 0; j < dem_columns; j++)
		{			
			p_new_data[i*dem_columns+j] = (p_new_data[(i-1)*dem_columns+j] + p_new_data[(i+1)*dem_columns+j]) * 0.5;
		}
	return p_new_data;
}

glbByte* CGlbGlobeTerrainTile::MultifyDomSection(glbByte* p_data, glbInt32 dom_columns, glbInt32 dom_rows, GlbGlobeChildLocationEnum location)
{// domcolumn == domrow = 256   rgba模式 4个字节
	glbByte* p_newdata = new glbByte[dom_rows*dom_columns*4];
#if 0	
	glbInt32 half_rows = dom_rows / 2 + (dom_rows % 2);		//128
	glbInt32 half_columns = dom_columns / 2 + (dom_columns % 2); // 128

	osg::ref_ptr<osg::Image> image = new osg::Image();
	GLenum pixelFormat = GL_RGBA;	
	image->allocateImage(dom_columns,dom_rows,1,pixelFormat,GL_UNSIGNED_BYTE);
	unsigned int imagesize = image->getImageSizeInBytes();
	memcpy(image->data(),p_data,imagesize);

	// 放大2倍
	image->scaleImage(dom_columns*2,dom_rows*2,1);
	int m,n;
	switch(location)
	{
	case GLB_NORTHWEST:	//西北
		m = n = 0;
		break;
	case GLB_NORTHEAST: //东北
		{
			m  = 0;
			n = 1;
		}
		break;
	case GlB_SOUTHWEST: //西南
		{
			m = 1;
			n = 0;
		}
		break;
	case GLB_SOUTHEAST: //东南
		m = n = 1;		
		break;
	}

	osg::ref_ptr<osg::Image> outputimage = new osg::Image();
	outputimage->allocateImage(dom_columns,dom_rows,1,pixelFormat,GL_UNSIGNED_BYTE);

	int start_col = n*dom_columns;
	int start_row = m*dom_rows;
	for (int j = 0; j < dom_rows; j++)// 高度
	{		
		unsigned char* sourcedata = image->data(start_col, start_row+j);
		unsigned char* destdata = outputimage->data(0, j);
		memcpy(destdata,sourcedata,dom_columns*4);				 
	}

	//imagesize = outputimage->getImageSizeInBytes();
	memcpy(p_newdata,outputimage->data(),imagesize);

	int i;
	// p_newdata边界数据恢复处理- 避免出现条纹
	switch(location)
	{
	case GLB_NORTHWEST:	//西北
		{
			for (i = 0; i < half_rows; i++)
			{
				int d_loc = ((i*2)*dom_columns)*4;
				int s_loc = (i*dom_columns)*4;
				//p_newdata[d_loc] = p_data[s_loc];
				//p_newdata[d_loc+1] = p_data[s_loc+1];
				//p_newdata[d_loc+2] = p_data[s_loc+2];
				//p_newdata[d_loc+3] = p_data[s_loc+3];
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));

				d_loc = ((i*2+1)*dom_columns)*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));
			}

			for (i = 0; i < half_columns; i++ )
			{
				int d_loc = (2*i)*4;
				int s_loc = i*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));

				d_loc = (2*i+1)*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));
			}
		}
		break;
	case GLB_NORTHEAST: //东北
		{ 
			for (i = 0; i < half_rows; i++)
			{
				int d_loc = (2*i*dom_columns + (dom_columns-1))*4;
				int s_loc = (i*dom_columns + (dom_columns-1))*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));

				d_loc = ((2*i+1)*dom_columns + (dom_columns-1))*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));
			}

			for (i = 0; i < half_columns; i++ )
			{
				int d_loc = 2*i*4;
				int s_loc = (i + half_columns)*4 ;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));

				d_loc = (2*i+1)*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));
			}
		}
		break;
	case GlB_SOUTHWEST: //西南
		{
			for (i = 0; i < half_rows; i++)
			{
				int d_loc = (2*i*dom_columns)*4;
				int s_loc = ((i+half_rows)*dom_columns)*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));

				d_loc = ((i*2+1)*dom_columns)*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));
			}

			for (i = 0; i < half_columns; i++ )
			{
				int d_loc = (dom_columns*(dom_rows-1)+i*2)*4;
				int s_loc = (dom_columns*(dom_rows-1)+i)*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));

				d_loc = (dom_columns*(dom_rows-1)+i*2+1)*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));
			}
		}
		break;
	case GLB_SOUTHEAST: //东南
		{
			for (i = 0; i < half_rows; i++)
			{
				int d_loc = (2*i*dom_columns + (dom_columns-1))*4;
				int s_loc = ((i+half_rows)*dom_columns + (dom_columns-1))*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));

				d_loc = ((2*i+1)*dom_columns + (dom_columns-1))*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));
			}

			for (i = 0; i < half_columns; i++ )
			{
				int d_loc = (dom_columns*(dom_rows-1)+i*2)*4;
				int s_loc = (dom_columns*(dom_rows-1)+half_columns+i)*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));

				d_loc = (dom_columns*(dom_rows-1)+ i*2+1)*4;
				memcpy(&p_newdata[d_loc],&p_data[s_loc],4*sizeof(unsigned char));
			}
		}		
		break;
	}
#else	
	int totoalsize = dom_rows*dom_columns*4;
	memset(p_newdata,0,totoalsize);
	glbInt32 half_rows = dom_rows / 2 + (dom_rows % 2);		//128
	glbInt32 half_columns = dom_columns / 2 + (dom_columns % 2); // 128
	// step 1 : 写真实数据
	for (glbInt32 i = 0 ; i < half_rows; i++)
		for(glbInt32 j = 0; j < half_columns; j++)
		{
			switch(location)
			{
			case GLB_NORTHWEST:	//西北
				{
					int d_loc = (i*2*dom_columns+j*2)*4;
					int s_loc = (i*dom_columns+j)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = (i*2*dom_columns+j*2+1)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = ((i*2+1)*dom_columns+j*2)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = ((i*2+1)*dom_columns+j*2+1)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];
				}
				break;
			case GLB_NORTHEAST:	//东北			
				{
					int d_loc = (i*2*dom_columns+j*2)*4;
					int s_loc = (i*dom_columns+j+half_columns)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = (i*2*dom_columns+j*2+1)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = ((i*2+1)*dom_columns+j*2)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = ((i*2+1)*dom_columns+j*2+1)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

				}							
				break;
			case GlB_SOUTHWEST: //西南
				{
					int d_loc = (i*2*dom_columns+j*2)*4;
					int s_loc = ((i+half_rows)*dom_columns+j)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = (i*2*dom_columns+j*2+1)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = ((i*2+1)*dom_columns+j*2)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = ((i*2+1)*dom_columns+j*2+1)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

				}								
				break;
			case GLB_SOUTHEAST: //东南
				{
					int d_loc = (i*2*dom_columns+j*2)*4;
					int s_loc = ((i+half_rows)*dom_columns+j+half_columns)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = (i*2*dom_columns+j*2+1)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = ((i*2+1)*dom_columns+j*2)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

					d_loc = ((i*2+1)*dom_columns+j*2+1)*4;
					p_newdata[d_loc] = p_data[s_loc];
					p_newdata[d_loc+1] = p_data[s_loc+1];
					p_newdata[d_loc+2] = p_data[s_loc+2];
					p_newdata[d_loc+3] = p_data[s_loc+3];

				}				
				break;
			}
		}
#endif
	return p_newdata;	

}

glbFloat CGlbGlobeTerrainTile::GetHeightData(glbFloat* p_heightdatas, glbInt32 column, glbInt32 row, GlbGlobeChildLocationEnum location)
{// 左上角为数据块起始位置
	if (p_heightdatas==NULL)
		return 0.0f;
	glbFloat height_data = 0.0f;

	glbInt32 thisVertexCount = mpr_vertex_count / 2 + (mpr_vertex_count % 2);
	//long thisVertexCountPlus1 = thisVertexCount + 1;
	glbInt32 thisVertexCountPlus1 = (thisVertexCount%2 == 0) ? thisVertexCount + 1 : thisVertexCount;
	glbInt32 total_columns =thisVertexCount*2+1;

	glbInt32 data_column = column;
	glbInt32 data_row = row;
	switch(location)
	{
	case GLB_NORTHWEST:// 西北		
		break;
	case GLB_NORTHEAST:// 东北		
		data_column = thisVertexCountPlus1 - 1 + column;				
		break; 
	case GlB_SOUTHWEST:// 西南		
		data_row = thisVertexCountPlus1 - 1 + row;
		break;
	case GLB_SOUTHEAST:// 东南		
		{
			data_column = thisVertexCountPlus1 - 1 + column;	
			data_row = thisVertexCountPlus1 - 1 + row;
		}			
		break;
	default:
		return 0.0f;
	}
	glbInt32 _index = data_row *  total_columns + data_column;
	//if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
	//	height_data = p_heightdatas[_index];
	//}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
	//	height_data = p_heightdatas[_index];
	//}
	
	height_data = p_heightdatas[_index] ;	
	return height_data;
}

osg::Vec4 CGlbGlobeTerrainTile::AdjustTileColor(long level)
{
	float opacity = mpr_terrain->GetOpacity() / 100.0f;
	if (opacity>0.99){
		opacity = 1.0f;
	}

	glbBool wire_render_mode = mpr_terrain->IsWireMode();

	osg::Vec4 clr(1,1,1,1);	
	return clr;

	if (!wire_render_mode)
	{
		return clr;
	}
	else
	{
		switch(level)
		{
		case 0:		clr.set(1,1,1,1);		break;
		case 1:		clr.set(0.5,0,0,1);		break;
		case 2:		clr.set(0,0.5,0,1);		break;
		case 3:		clr.set(0,0,0.5,1);		break;
		case 4:		clr.set(0.5,0.5,0,1);	break;
		case 5:		clr.set(0,0.5,0.5,1);	break;
		case 6:		clr.set(0.5,0,0.5,1);	break;
		case 7:		clr.set(1,0,0,1);		break;
		case 8:		clr.set(0,1,0,1);		break;
		case 9:		clr.set(0,0,1,1);		break;
		case 10:	clr.set(1,1,0,1);		break;
		case 11:	clr.set(1,0,1,1);		break;
		case 12:	clr.set(0,1,1,1);		break;
		case 13:	clr.set(1,1,1,1);		break;
		case 14:	clr.set(0,1,0,1);		break;
		case 15:	clr.set(0.2,0,1,1);		break;
		case 16:	clr.set(0,0.5,0.7,1);	break;
		case 17:	clr.set(0.4,0.5,0.7,1);	break;
		case 18:	clr.set(1,0.4,0.3,1);	break;
		case 19:	clr.set(0,0.7,0.2,1);	break;
		case 20:	clr.set(0.6,0.7,0.7,1);	break;
		case 21:	clr.set(0.4,0.2,0.7,1);	break;
		case 22:	clr.set(0.0,0.7,0.3,1);	break;
		case 23:	clr.set(1.0,0.7,0.2,1);	break;
		case 24:	clr.set(0.6,0.7,1.0,1);	break;
		case 25:	clr.set(1.0,1.0,1.0,1);	break;
		}
	}
	return clr;
}

void CGlbGlobeTerrainTile::WorldToScreen(glbDouble &x, glbDouble &y, glbDouble &z)
{//	世界-〉屏幕：	osgUtil::SceneView::projectObjectIntoWindow	或者自己运算	
	osg::Vec3d pos(x,y,z);
	osg::Camera* p_camera = mpr_terrain->mpr_globe->GetView()->GetOsgCamera();

	osg::Matrixd VPW = p_camera->getViewMatrix() *
		p_camera->getProjectionMatrix() *
		p_camera->getViewport()->computeWindowMatrix();
	osg::Vec3d window = pos * VPW;
	x = window.x();
	y = p_camera->getViewport()->height() - window.y();
	z = window.z();
}

#define  TEST_RESOLUTION_RADIO		1		//设置测试分辨率为1个像素
#define  TILE_TEXTURE_SIZE			256		//tile用的纹理尺寸256*256
#define  SPLIT_FAZHI				2		//2

glbBool CGlbGlobeTerrainTile::IsTileSplit()
{
	// 对于分辨率测试，首先计算瓦片四个角点的目标分辨率，如果四个角点的目标分辨率数值都大于或等于瓦片所处
	// 金字塔层的分辨率数值，则表示该瓦片满足分辨率要求，否则需要测试其四个分支。
	// 假设被测试瓦片的分辨率为2，由于瓦片左上角点目标分辨率数值小于2，则必须对瓦片进行细分.
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		glbDouble averageAlt = (mpr_minalt+mpr_maxalt)*0.5;
		glbDouble x0,y0,x1,y1;
		osg::Vec3d vec1,vec2,vec3;

#if 1		
		int testPassCnt=0;
		//// 测试中心竖线
		//x0 = x1 = (mpr_west+mpr_east)*0.5;
		//y0 = mpr_north;
		//y1 = mpr_south;
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		//WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		//WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		//double screenLen1 = (vec1-vec2).length();
		//if (screenLen1 > TILE_TEXTURE_SIZE*SPLIT_FAZHI)
		//{// 像素放大显示，需要细分		
		//	testPassCnt++;
		//}	

		//// 测试中心横线
		//y0 = y1 = (mpr_south+mpr_north)*0.5;
		//x0 = mpr_west;
		//x1 = mpr_east;
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		//WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		//WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		//screenLen1 = (vec1-vec2).length();
		//if (screenLen1 > TILE_TEXTURE_SIZE*SPLIT_FAZHI)
		//{// 像素放大显示，需要细分		
		//	testPassCnt++;
		//}	

		// 测试左边界线
		x0 = x1 = mpr_west;
		y0 = mpr_north;
		y1 = mpr_south;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		vec3 = vec1-vec2;
		double screenLen1 = sqrt(vec3.x()*vec3.x()+vec3.y()*vec3.y());
		if (screenLen1 > TILE_TEXTURE_SIZE*SPLIT_FAZHI)
		{// 像素放大显示，需要细分		
			testPassCnt++;
		}	

		// 测试下边界
		y0 = y1 = mpr_south;
		x0 = mpr_west;
		x1 = mpr_east;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		vec3 = vec1-vec2;
		screenLen1 = sqrt(vec3.x()*vec3.x()+vec3.y()*vec3.y());
		if (screenLen1 > TILE_TEXTURE_SIZE*SPLIT_FAZHI)
		{// 像素放大显示，需要细分		
			testPassCnt++;
		}	

		// 测试右边界线
		x0 = x1 = mpr_east;
		y0 = mpr_north;
		y1 = mpr_south;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		vec3 = vec1-vec2;
		screenLen1 = sqrt(vec3.x()*vec3.x()+vec3.y()*vec3.y());
		if (screenLen1 > TILE_TEXTURE_SIZE*SPLIT_FAZHI)
		{// 像素放大显示，需要细分			
			testPassCnt++;
		}	

		// 测试上边界
		y0 = y1 = mpr_north;
		x0 = mpr_west;
		x1 = mpr_east;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		vec3 = vec1-vec2;
		screenLen1 = sqrt(vec3.x()*vec3.x()+vec3.y()*vec3.y());
		if (screenLen1 > TILE_TEXTURE_SIZE*SPLIT_FAZHI)
		{// 像素放大显示，需要细分		
			testPassCnt++;
		}	

		if (testPassCnt>1)		
			return true;
#else
		// 测试西北角点
		x0 = x1 = mpr_west;
		y0 = mpr_north + (mpr_north - mpr_south)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		y1 = mpr_north - (mpr_north - mpr_south)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;

		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		double screenLen1 = (vec1-vec2).length();
		if (screenLen1 > TEST_RESOLUTION_RADIO*SPLIT_FAZHI)
		{// 像素放大显示，需要细分
			return true;
		}	

		// 测试西南角点
		x0 = mpr_west;
		y0 = y1 = mpr_south;	
		x1 = x0 + (mpr_east-mpr_west)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		double screenLen2 = (vec1-vec2).length();
		if (screenLen2 > TEST_RESOLUTION_RADIO*SPLIT_FAZHI)
		{// 像素放大显示，需要细分
			return true;
		}

		// 测试东南角点
		x0 = x1 = mpr_east;
		y0 = mpr_south;
		y1 = y0 + (mpr_north-mpr_south)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		double screenLen3 = (vec1-vec2).length();
		if (screenLen3 > TEST_RESOLUTION_RADIO*SPLIT_FAZHI)
		{// 像素放大显示，需要细分
			return true;
		}


		// 测试东北角点
		x0 = mpr_east;
		y0 = y1 = mpr_north;
		x1 = x0 - (mpr_east-mpr_west)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		double screenLen4 = (vec1-vec2).length();
		if (screenLen4 > TEST_RESOLUTION_RADIO*SPLIT_FAZHI)
		{// 像素放大显示，需要细分
			return true;
		}
	
		// 测试地形块中心点
		x0 = (mpr_east+mpr_west)*0.5;
		y0 = y1 = (mpr_north+mpr_south)*0.5;
		x1 = x0 - (mpr_east-mpr_west)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		double screenLen9 = (vec1-vec2).length();
		if (screenLen9 > TEST_RESOLUTION_RADIO*SPLIT_FAZHI)
		{// 像素放大显示，需要细分
			return true;
		}	
#endif
	}
	else if(mpr_globe_type==GLB_GLOBETYPE_FLAT)
	{
		glbDouble xmin,xmax,ymin,ymax,zmin,zmax;
		mpr_extent->Get(&xmin,&xmax,&ymin,&ymax,&zmin,&zmax);
		glbDouble averagez = (zmin+zmax)*0.5f;

		glbDouble x0,y0,z0,x1,y1,z1;

		// 测试左上角点
		x0 = x1 = xmin;
		y0 = ymax;
		z0 = z1 = averagez;	
		y1 = y0 - (ymax-ymin)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		WorldToScreen(x0,y0,z0);
		WorldToScreen(x1,y1,z1);
		double screenLen1 = sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) );
		if (screenLen1 > TEST_RESOLUTION_RADIO)
		{// 像素放大显示，需要细分
			return true;
		}

		// 测试左下角点
		x0 = xmin;
		y0 = y1 = ymin;
		z0 = z1 = averagez;	
		x1 = x0 + (xmax-xmin)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		WorldToScreen(x0,y0,z0);
		WorldToScreen(x1,y1,z1);
		double screenLen2 = sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) );
		if (screenLen2 > TEST_RESOLUTION_RADIO)
		{// 像素放大显示，需要细分
			return true;
		}

		// 测试右下角点
		x0 = x1 = xmax;
		y0 = ymin;
		z0 = z1 = averagez;	
		y1 = y0 + (ymax-ymin)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		WorldToScreen(x0,y0,z0);
		WorldToScreen(x1,y1,z1);
		double screenLen3 = sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) );
		if (screenLen3 > TEST_RESOLUTION_RADIO)
		{// 像素放大显示，需要细分
			return true;
		}

		// 测试右上角点
		x0 = xmax;
		y0 = y1 = ymax;
		z0 = z1 = averagez;	
		x1 = x0 - (xmax-xmin)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		WorldToScreen(x0,y0,z0);
		WorldToScreen(x1,y1,z1);
		double screenLen4 = sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) );
		if (screenLen4 > TEST_RESOLUTION_RADIO)
		{// 像素放大显示，需要细分
			return true;
		}
	}
	return false;
}

void CGlbGlobeTerrainTile::BuildEdgeGeometry(osg::Vec3dArray* p_neighbour_edge_vertex_array,GlbTileEdgeEnum edge)
{
	if (p_neighbour_edge_vertex_array==NULL)
		return;

	osg::ref_ptr<osg::Texture2D> p_texture = mpr_texture;

	glbFloat exaggeration = 0;//10000;//4000;//0;//400;
	glbFloat exaggeration1 = 30000;

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array();
	osg::Vec4 clr = AdjustTileColor(mpr_level);	
	colorArray->push_back(clr);

	//osg::ref_ptr<osg::EllipsoidModel> ellips = new osg::EllipsoidModel(osg::WGS_84_RADIUS_EQUATOR,osg::WGS_84_RADIUS_POLAR);
	switch(edge)
	{
	case GLB_NORTHEDGE_WESTSECTION:
		{
			if (mpr_p_north_westhalf_geom==NULL)
			{
				mpr_p_north_westhalf_geom = new osg::Geometry;
				mpr_p_north_westhalf_geom->setDataVariance(osg::Object::DYNAMIC);
			}
			else
				mpr_p_north_westhalf_geom->removePrimitiveSet(0,mpr_p_north_westhalf_geom->getNumPrimitiveSets());			
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> p_VertexTexCoords = new osg::Vec2Array;
			
			mpr_p_north_westhalf_geom->setColorArray(colorArray.get());
			p_VertexArray->reserve(mpr_p_north_westhalf_vertexarray->size()*2);
			p_VertexTexCoords->reserve(mpr_p_north_westhalf_texcoords->size()*2);	
			osg::Vec3d point;
			osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
			pIndexArray->reserve(mpr_p_north_westhalf_vertexarray->size()*2);
			for (long k = 0; k < (long)mpr_p_north_westhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_north_westhalf_vertexarray->at(k).x(),mpr_p_north_westhalf_vertexarray->at(k).y(),mpr_p_north_westhalf_vertexarray->at(k).z(),					
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_north_westhalf_vertexarray->at(k);
					point.set(mpr_p_north_westhalf_vertexarray->at(k).y(),mpr_p_north_westhalf_vertexarray->at(k).x(),mpr_p_north_westhalf_vertexarray->at(k).z()+exaggeration);
				}

				p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));
				//p_VertexArray->push_back(mpr_p_north_westhalf_vertexarray->at(k) - mpr_local_origin);

/*				ellips->convertLatLongHeightToXYZ(
					p_neighbour_edge_vertex_array->at(k).x(),p_neighbour_edge_vertex_array->at(k).y(),p_neighbour_edge_vertex_array->at(k).z() + exaggeration,						
					point.x(), point.y(), point.z() );
				p_VertexArray->push_back(point - mpr_local_origin);			*/	
				//p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin  + _exaggeration);
				p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));

				// 纹理coord
				p_VertexTexCoords->push_back(mpr_p_north_westhalf_texcoords->at(k));
				p_VertexTexCoords->push_back(mpr_p_north_westhalf_texcoords->at(k));
				pIndexArray->push_back(2*k);
				pIndexArray->push_back(2*k+1);
			}

			mpr_p_north_westhalf_geom->setVertexArray(p_VertexArray.get());
			mpr_p_north_westhalf_geom->setTexCoordArray(0,p_VertexTexCoords.get());
			mpr_p_north_westhalf_geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,p_texture.get());	
			osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,pIndexArray->size(),&pIndexArray->front());
			mpr_p_north_westhalf_geom->addPrimitiveSet(drawElements.get());	
			//mpr_p_north_westhalf_geom->addPrimitiveSet( new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP,pIndexArray->size(),&pIndexArray->front()));
			mpr_p_north_westhalf_geom->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		}
		break;
	case GLB_NORTHEDGE_EASTSECTION:
		{
			if (mpr_p_north_easthalf_geom==NULL)
			{
				mpr_p_north_easthalf_geom = new osg::Geometry;
				mpr_p_north_easthalf_geom->setDataVariance(osg::Object::DYNAMIC);
			}
			else
				mpr_p_north_easthalf_geom->removePrimitiveSet(0,mpr_p_north_easthalf_geom->getNumPrimitiveSets());
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> p_VertexTexCoords = new osg::Vec2Array;
			
			mpr_p_north_easthalf_geom->setColorArray(colorArray.get());

			p_VertexArray->reserve(mpr_p_north_easthalf_vertexarray->size()*2);
			p_VertexTexCoords->reserve(mpr_p_north_easthalf_texcoords->size()*2);

			osg::Vec3d point;
			osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
			pIndexArray->reserve(mpr_p_north_easthalf_vertexarray->size()*2);
			for (long k = 0; k < (long)mpr_p_north_easthalf_vertexarray->size(); k++)
			{				
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_north_easthalf_vertexarray->at(k).x(),mpr_p_north_easthalf_vertexarray->at(k).y(),mpr_p_north_easthalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_north_easthalf_vertexarray->at(k);
					point.set(mpr_p_north_easthalf_vertexarray->at(k).y(),mpr_p_north_easthalf_vertexarray->at(k).x(),mpr_p_north_easthalf_vertexarray->at(k).z()+exaggeration);
				}
				p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));		
			
				p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));

				// 纹理coord
				p_VertexTexCoords->push_back(mpr_p_north_easthalf_texcoords->at(k));
				p_VertexTexCoords->push_back(mpr_p_north_easthalf_texcoords->at(k));
				pIndexArray->push_back(2*k);
				pIndexArray->push_back(2*k+1);
			}
			mpr_p_north_easthalf_geom->setVertexArray(p_VertexArray.get());
			mpr_p_north_easthalf_geom->setTexCoordArray(0,p_VertexTexCoords.get());
			mpr_p_north_easthalf_geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,p_texture.get());	
			osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,pIndexArray->size(),&pIndexArray->front());			
			mpr_p_north_easthalf_geom->addPrimitiveSet(drawElements.get());		
			//mpr_p_north_easthalf_geom->addPrimitiveSet( new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP,pIndexArray->size(),&pIndexArray->front()));
			mpr_p_north_easthalf_geom->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
				
		}
		break;
	case GLB_SOUTHEDGE_WESTSECTION:
		{
			if (mpr_p_south_westhalf_geom==NULL)
			{
				mpr_p_south_westhalf_geom = new osg::Geometry;
				mpr_p_south_westhalf_geom->setDataVariance(osg::Object::DYNAMIC);
			}
			else
				mpr_p_south_westhalf_geom->removePrimitiveSet(0,mpr_p_south_westhalf_geom->getNumPrimitiveSets());
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> p_VertexTexCoords = new osg::Vec2Array;
			
			mpr_p_south_westhalf_geom->setColorArray(colorArray.get());

			p_VertexArray->reserve(mpr_p_south_westhalf_vertexarray->size()*2);
			p_VertexTexCoords->reserve(mpr_p_south_westhalf_texcoords->size()*2);

			osg::Vec3d point;
			osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
			pIndexArray->reserve(mpr_p_south_westhalf_vertexarray->size()*2);
			for (long k = 0; k < (long)mpr_p_south_westhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_south_westhalf_vertexarray->at(k).x(),mpr_p_south_westhalf_vertexarray->at(k).y(),mpr_p_south_westhalf_vertexarray->at(k).z() ,
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_south_westhalf_vertexarray->at(k);
					point.set(mpr_p_south_westhalf_vertexarray->at(k).y(),mpr_p_south_westhalf_vertexarray->at(k).x(),mpr_p_south_westhalf_vertexarray->at(k).z()+exaggeration);
				}

				p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));				

				//ellips->convertLatLongHeightToXYZ(
				//	p_neighbour_edge_vertex_array->at(k).x(),p_neighbour_edge_vertex_array->at(k).y(),p_neighbour_edge_vertex_array->at(k).z() + exaggeration,
				//	point.x(), point.y(), point.z() );
				//p_VertexArray->push_back(point - mpr_local_origin);
				//p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin  + _exaggeration);
				p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));

				// 纹理coord
				p_VertexTexCoords->push_back(mpr_p_south_westhalf_texcoords->at(k));
				p_VertexTexCoords->push_back(mpr_p_south_westhalf_texcoords->at(k));
				pIndexArray->push_back(2*k);
				pIndexArray->push_back(2*k+1);
			}
			mpr_p_south_westhalf_geom->setVertexArray(p_VertexArray.get());
			mpr_p_south_westhalf_geom->setTexCoordArray(0,p_VertexTexCoords.get());
			mpr_p_south_westhalf_geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,p_texture.get());	
			osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,pIndexArray->size(),&pIndexArray->front());
			mpr_p_south_westhalf_geom->addPrimitiveSet(drawElements.get());	
			//mpr_p_south_westhalf_geom->addPrimitiveSet( new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP,pIndexArray->size(),&pIndexArray->front()));
			mpr_p_south_westhalf_geom->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		}
		break;
	case GLB_SOUTHEDGE_EASTSECTION:
		{
			if (mpr_p_south_easthalf_geom==NULL)
			{
				mpr_p_south_easthalf_geom = new osg::Geometry;
				mpr_p_south_easthalf_geom->setDataVariance(osg::Object::DYNAMIC);
			}
			else
				mpr_p_south_easthalf_geom->removePrimitiveSet(0,mpr_p_south_easthalf_geom->getNumPrimitiveSets());
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> p_VertexTexCoords = new osg::Vec2Array;
			
			mpr_p_south_easthalf_geom->setColorArray(colorArray.get());

			p_VertexArray->reserve(mpr_p_south_easthalf_vertexarray->size()*2);
			p_VertexTexCoords->reserve(mpr_p_south_easthalf_texcoords->size()*2);

			osg::Vec3d point;
			osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
			pIndexArray->reserve(mpr_p_south_easthalf_vertexarray->size()*2);
			for (long k = 0; k < (long)mpr_p_south_easthalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_south_easthalf_vertexarray->at(k).x(),mpr_p_south_easthalf_vertexarray->at(k).y(),mpr_p_south_easthalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){				
					point.set(mpr_p_south_easthalf_vertexarray->at(k).y(),mpr_p_south_easthalf_vertexarray->at(k).x(),mpr_p_south_easthalf_vertexarray->at(k).z()+exaggeration);
				}
				p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));

				//ellips->convertLatLongHeightToXYZ(
				//	p_neighbour_edge_vertex_array->at(k).x(),p_neighbour_edge_vertex_array->at(k).y(),p_neighbour_edge_vertex_array->at(k).z() + exaggeration,
				//	point.x(), point.y(), point.z() );
				//p_VertexArray->push_back(point - mpr_local_origin);
				//p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin  + _exaggeration);
				p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));

				// 纹理coord
				p_VertexTexCoords->push_back(mpr_p_south_easthalf_texcoords->at(k));
				p_VertexTexCoords->push_back(mpr_p_south_easthalf_texcoords->at(k));
				pIndexArray->push_back(2*k);
				pIndexArray->push_back(2*k+1);
			}
			mpr_p_south_easthalf_geom->setVertexArray(p_VertexArray.get());
			mpr_p_south_easthalf_geom->setTexCoordArray(0,p_VertexTexCoords.get());
			mpr_p_south_easthalf_geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,p_texture.get());	
			osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,pIndexArray->size(),&pIndexArray->front());
			mpr_p_south_easthalf_geom->addPrimitiveSet(drawElements.get());	
			//mpr_p_south_easthalf_geom->addPrimitiveSet( new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP,pIndexArray->size(),&pIndexArray->front()));
			mpr_p_south_easthalf_geom->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		}
		break;
	case GLB_WESTEDGE_NORTHSECTION:
		{
			if (mpr_p_west_northhalf_geom==NULL)
			{
				mpr_p_west_northhalf_geom = new osg::Geometry;
				mpr_p_west_northhalf_geom->setDataVariance(osg::Object::DYNAMIC);
			}
			else
				mpr_p_west_northhalf_geom->removePrimitiveSet(0,mpr_p_west_northhalf_geom->getNumPrimitiveSets());
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> p_VertexTexCoords = new osg::Vec2Array;
		
			mpr_p_west_northhalf_geom->setColorArray(colorArray.get());

			p_VertexArray->reserve(mpr_p_west_northhalf_vertexarray->size()*2);
			p_VertexTexCoords->reserve(mpr_p_west_northhalf_texcoords->size()*2);

			osg::Vec3d point;
			osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
			pIndexArray->reserve(mpr_p_west_northhalf_vertexarray->size()*2);
			for (long k = 0; k < (long)mpr_p_west_northhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_west_northhalf_vertexarray->at(k).x(),mpr_p_west_northhalf_vertexarray->at(k).y(),mpr_p_west_northhalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_west_northhalf_vertexarray->at(k);
					point.set(mpr_p_west_northhalf_vertexarray->at(k).y(),mpr_p_west_northhalf_vertexarray->at(k).x(),mpr_p_west_northhalf_vertexarray->at(k).z());
				}
				p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));

				//ellips->convertLatLongHeightToXYZ(
				//	p_neighbour_edge_vertex_array->at(k).x(),p_neighbour_edge_vertex_array->at(k).y(),p_neighbour_edge_vertex_array->at(k).z() + exaggeration,
				//	point.x(), point.y(), point.z() );
				//p_VertexArray->push_back(point - mpr_local_origin);
				//p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin  + _exaggeration);
				p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));

				// 纹理coord
				p_VertexTexCoords->push_back(mpr_p_west_northhalf_texcoords->at(k));
				p_VertexTexCoords->push_back(mpr_p_west_northhalf_texcoords->at(k));
				pIndexArray->push_back(2*k);
				pIndexArray->push_back(2*k+1);
			}
			mpr_p_west_northhalf_geom->setVertexArray(p_VertexArray.get());
			mpr_p_west_northhalf_geom->setTexCoordArray(0,p_VertexTexCoords.get());
			mpr_p_west_northhalf_geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,p_texture.get());	
			osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,pIndexArray->size(),&pIndexArray->front());
			mpr_p_west_northhalf_geom->addPrimitiveSet(drawElements.get());		
			//mpr_p_west_northhalf_geom->addPrimitiveSet( new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP,pIndexArray->size(),&pIndexArray->front()));
			mpr_p_west_northhalf_geom->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		}
		break;
	case GLB_WESTEDGE_SOUTHSECTION:
		{
			if (mpr_p_west_southhalf_geom==NULL)
			{
				mpr_p_west_southhalf_geom = new osg::Geometry;
				mpr_p_west_southhalf_geom->setDataVariance(osg::Object::DYNAMIC);
			}
			else
				mpr_p_west_southhalf_geom->removePrimitiveSet(0,mpr_p_west_southhalf_geom->getNumPrimitiveSets());
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> p_VertexTexCoords = new osg::Vec2Array;
			
			mpr_p_west_southhalf_geom->setColorArray(colorArray.get());
			p_VertexArray->reserve(mpr_p_west_southhalf_vertexarray->size()*2);
			p_VertexTexCoords->reserve(mpr_p_west_southhalf_texcoords->size()*2);

			osg::Vec3d point;
			osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
			pIndexArray->reserve(mpr_p_west_southhalf_vertexarray->size()*2);
			for (long k = 0; k < (long)mpr_p_west_southhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_west_southhalf_vertexarray->at(k).x(),mpr_p_west_southhalf_vertexarray->at(k).y(),mpr_p_west_southhalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_west_southhalf_vertexarray->at(k);
					point.set(mpr_p_west_southhalf_vertexarray->at(k).y(),mpr_p_west_southhalf_vertexarray->at(k).x(),mpr_p_west_southhalf_vertexarray->at(k).z());
				}
				p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));

				//ellips->convertLatLongHeightToXYZ(
				//	p_neighbour_edge_vertex_array->at(k).x(),p_neighbour_edge_vertex_array->at(k).y(),p_neighbour_edge_vertex_array->at(k).z() + exaggeration,
				//	point.x(), point.y(), point.z() );
				//p_VertexArray->push_back(point - mpr_local_origin);
				//p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin  + _exaggeration);
				p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));

				// 纹理coord
				p_VertexTexCoords->push_back(mpr_p_west_southhalf_texcoords->at(k));
				p_VertexTexCoords->push_back(mpr_p_west_southhalf_texcoords->at(k));
				pIndexArray->push_back(2*k);
				pIndexArray->push_back(2*k+1);
			}
			mpr_p_west_southhalf_geom->setVertexArray(p_VertexArray.get());
			mpr_p_west_southhalf_geom->setTexCoordArray(0,p_VertexTexCoords.get());
			mpr_p_west_southhalf_geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,p_texture.get());	
			osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,pIndexArray->size(),&pIndexArray->front());
			mpr_p_west_southhalf_geom->addPrimitiveSet(drawElements.get());	
			//mpr_p_west_southhalf_geom->addPrimitiveSet( new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP,pIndexArray->size(),&pIndexArray->front()));
			mpr_p_west_southhalf_geom->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		}
		break;
	case GLB_EASTEDGE_NORTHSECTION:
		{
			if (mpr_p_east_northhalf_geom==NULL)
			{
				mpr_p_east_northhalf_geom = new osg::Geometry;
				mpr_p_east_northhalf_geom->setDataVariance(osg::Object::DYNAMIC);
			}
			else
				mpr_p_east_northhalf_geom->removePrimitiveSet(0,mpr_p_east_northhalf_geom->getNumPrimitiveSets());
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> p_VertexTexCoords = new osg::Vec2Array;
		
			mpr_p_east_northhalf_geom->setColorArray(colorArray.get());

			p_VertexArray->reserve(mpr_p_east_northhalf_vertexarray->size()*2);
			p_VertexTexCoords->reserve(mpr_p_east_northhalf_texcoords->size()*2);

			osg::Vec3d point;
			osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
			pIndexArray->reserve(mpr_p_east_northhalf_vertexarray->size()*2);
			for (long k = 0; k < (long)mpr_p_east_northhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_east_northhalf_vertexarray->at(k).x(),mpr_p_east_northhalf_vertexarray->at(k).y(),mpr_p_east_northhalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_east_northhalf_vertexarray->at(k);
					point.set(mpr_p_east_northhalf_vertexarray->at(k).y(),mpr_p_east_northhalf_vertexarray->at(k).x(),mpr_p_east_northhalf_vertexarray->at(k).z());
				}
				p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));	

				//ellips->convertLatLongHeightToXYZ(
				//	p_neighbour_edge_vertex_array->at(k).x(),p_neighbour_edge_vertex_array->at(k).y(),p_neighbour_edge_vertex_array->at(k).z() + exaggeration,
				//	point.x(), point.y(), point.z() );
				//p_VertexArray->push_back(point - mpr_local_origin);
				//p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin  + _exaggeration);
				p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));

				// 纹理coord
				p_VertexTexCoords->push_back(mpr_p_east_northhalf_texcoords->at(k));
				p_VertexTexCoords->push_back(mpr_p_east_northhalf_texcoords->at(k));
				pIndexArray->push_back(2*k);
				pIndexArray->push_back(2*k+1);
			}
			mpr_p_east_northhalf_geom->setVertexArray(p_VertexArray.get());
			mpr_p_east_northhalf_geom->setTexCoordArray(0,p_VertexTexCoords.get());
			mpr_p_east_northhalf_geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,p_texture.get());	
			osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,pIndexArray->size(),&pIndexArray->front());
			mpr_p_east_northhalf_geom->addPrimitiveSet(drawElements.get());	
			//mpr_p_east_northhalf_geom->addPrimitiveSet( new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP,pIndexArray->size(),&pIndexArray->front()));
			mpr_p_east_northhalf_geom->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		}
		break;
	case GLB_EASTEDGE_SOUTHSECTION:
		{
			if (mpr_p_east_southhalf_geom==NULL)
			{
				mpr_p_east_southhalf_geom = new osg::Geometry;
				mpr_p_east_southhalf_geom->setDataVariance(osg::Object::DYNAMIC);
			}
			else
				mpr_p_east_southhalf_geom->removePrimitiveSet(0,mpr_p_east_southhalf_geom->getNumPrimitiveSets());
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> p_VertexTexCoords = new osg::Vec2Array;
		
			mpr_p_east_southhalf_geom->setColorArray(colorArray.get());

			p_VertexArray->reserve(mpr_p_east_southhalf_vertexarray->size()*2);
			p_VertexTexCoords->reserve(mpr_p_east_southhalf_texcoords->size()*2);

			osg::Vec3d point;
			osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
			pIndexArray->reserve(mpr_p_east_southhalf_vertexarray->size()*2);
			for (long k = 0; k < (long)mpr_p_east_southhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_east_southhalf_vertexarray->at(k).x(),mpr_p_east_southhalf_vertexarray->at(k).y(),mpr_p_east_southhalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_east_southhalf_vertexarray->at(k);
					point.set(mpr_p_east_southhalf_vertexarray->at(k).y(),mpr_p_east_southhalf_vertexarray->at(k).x(),mpr_p_east_southhalf_vertexarray->at(k).z());
				}

				p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));

				//ellips->convertLatLongHeightToXYZ(
				//	p_neighbour_edge_vertex_array->at(k).x(),p_neighbour_edge_vertex_array->at(k).y(),p_neighbour_edge_vertex_array->at(k).z() + exaggeration,
				//	point.x(), point.y(), point.z() );
				//p_VertexArray->push_back(point - mpr_local_origin);
				//p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin  + _exaggeration);
				p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));

				// 纹理coord
				p_VertexTexCoords->push_back(mpr_p_east_southhalf_texcoords->at(k));
				p_VertexTexCoords->push_back(mpr_p_east_southhalf_texcoords->at(k));
				pIndexArray->push_back(2*k);
				pIndexArray->push_back(2*k+1);
			}
			mpr_p_east_southhalf_geom->setVertexArray(p_VertexArray.get());
			mpr_p_east_southhalf_geom->setTexCoordArray(0,p_VertexTexCoords.get());
			mpr_p_east_southhalf_geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,p_texture.get());	
			osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,pIndexArray->size(),&pIndexArray->front());
			mpr_p_east_southhalf_geom->addPrimitiveSet(drawElements.get());		
			//mpr_p_east_southhalf_geom->addPrimitiveSet( new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_STRIP,pIndexArray->size(),&pIndexArray->front()));
			mpr_p_east_southhalf_geom->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		}
		break;
	}

}

void CGlbGlobeTerrainTile::ModifyEdgeGeometry(osg::Vec3dArray* p_neighbour_edge_vertex_array,GlbTileEdgeEnum edge)
{	
	if (p_neighbour_edge_vertex_array==NULL)
		return;

	glbFloat exaggeration = 0;
	osg::Vec3d point;		
	//osg::ref_ptr<osg::EllipsoidModel> ellips = new osg::EllipsoidModel(osg::WGS_84_RADIUS_EQUATOR,osg::WGS_84_RADIUS_POLAR);
	switch(edge)
	{
	case GLB_NORTHEDGE_WESTSECTION:
		if (mpr_p_north_westhalf_geom)
		{			
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;			
			p_VertexArray->reserve(mpr_p_north_westhalf_vertexarray->size()*2);			

			for (long k = 0; k < (long)mpr_p_north_westhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_north_westhalf_vertexarray->at(k).x(),mpr_p_north_westhalf_vertexarray->at(k).y(),mpr_p_north_westhalf_vertexarray->at(k).z(),					
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_north_westhalf_vertexarray->at(k);
					point.set( mpr_p_north_westhalf_vertexarray->at(k).y(), mpr_p_north_westhalf_vertexarray->at(k).x(), mpr_p_north_westhalf_vertexarray->at(k).z()+exaggeration);
				}

				p_VertexArray->push_back(point - mpr_local_origin);
				p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin);
			}
			//mpr_p_north_westhalf_geom->setVertexArray(p_VertexArray.get());				
			// 添加修改vecarray策略 2013.8.19
			CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_north_westhalf_geom.get(),p_VertexArray.get());
			mpr_terrain->AddOperation(_opera);
		}
		break;
	case GLB_NORTHEDGE_EASTSECTION:
		if (mpr_p_north_easthalf_geom)
		{
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;		
			p_VertexArray->reserve(mpr_p_north_easthalf_vertexarray->size()*2);
			for (long k = 0; k < (long)mpr_p_north_easthalf_vertexarray->size(); k++)
			{				
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_north_easthalf_vertexarray->at(k).x(),mpr_p_north_easthalf_vertexarray->at(k).y(),mpr_p_north_easthalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_north_easthalf_vertexarray->at(k);
					point.set( mpr_p_north_easthalf_vertexarray->at(k).y(), mpr_p_north_easthalf_vertexarray->at(k).x(), mpr_p_north_easthalf_vertexarray->at(k).z()+exaggeration);
				}
				p_VertexArray->push_back(point - mpr_local_origin);		
				p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin);
			}
			//mpr_p_north_easthalf_geom->setVertexArray(p_VertexArray.get());	
			// 添加修改vecarray策略 2013.8.19
			CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_north_easthalf_geom.get(),p_VertexArray.get());
			mpr_terrain->AddOperation(_opera);
		}
		break;
	case GLB_SOUTHEDGE_WESTSECTION:
		if (mpr_p_south_westhalf_geom)
		{
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;			
			p_VertexArray->reserve(mpr_p_south_westhalf_vertexarray->size()*2);		
			for (long k = 0; k < (long)mpr_p_south_westhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_south_westhalf_vertexarray->at(k).x(),mpr_p_south_westhalf_vertexarray->at(k).y(),mpr_p_south_westhalf_vertexarray->at(k).z() ,
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_south_westhalf_vertexarray->at(k);
					point.set( mpr_p_south_westhalf_vertexarray->at(k).y(), mpr_p_south_westhalf_vertexarray->at(k).x(), mpr_p_south_westhalf_vertexarray->at(k).z()+exaggeration);
				}
				p_VertexArray->push_back(point - mpr_local_origin);	
				p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin);	
			}
			//mpr_p_south_westhalf_geom->setVertexArray(p_VertexArray.get());
			// 添加修改vecarray策略 2013.8.19
			CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_south_westhalf_geom.get(),p_VertexArray.get());
			mpr_terrain->AddOperation(_opera);

		}
		break;
	case GLB_SOUTHEDGE_EASTSECTION:
		if (mpr_p_south_easthalf_geom)
		{			
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;			
			p_VertexArray->reserve(mpr_p_south_easthalf_vertexarray->size()*2);		
			for (long k = 0; k < (long)mpr_p_south_easthalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_south_easthalf_vertexarray->at(k).x(),mpr_p_south_easthalf_vertexarray->at(k).y(),mpr_p_south_easthalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_south_easthalf_vertexarray->at(k);
					point.set( mpr_p_south_easthalf_vertexarray->at(k).y(), mpr_p_south_easthalf_vertexarray->at(k).x(), mpr_p_south_easthalf_vertexarray->at(k).z()+exaggeration);
				}
				p_VertexArray->push_back(point - mpr_local_origin);
				p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin);	

			}
			//mpr_p_south_easthalf_geom->setVertexArray(p_VertexArray.get());	
			// 添加修改vecarray策略 2013.8.19
			CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_south_easthalf_geom.get(),p_VertexArray.get());
			mpr_terrain->AddOperation(_opera);
		}
		break;
	case GLB_WESTEDGE_NORTHSECTION:
		if (mpr_p_west_northhalf_geom)
		{
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;			
			p_VertexArray->reserve(mpr_p_west_northhalf_vertexarray->size()*2);

			for (long k = 0; k < (long)mpr_p_west_northhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_west_northhalf_vertexarray->at(k).x(),mpr_p_west_northhalf_vertexarray->at(k).y(),mpr_p_west_northhalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_west_northhalf_vertexarray->at(k);
					point.set( mpr_p_west_northhalf_vertexarray->at(k).y(), mpr_p_west_northhalf_vertexarray->at(k).x(), mpr_p_west_northhalf_vertexarray->at(k).z()+exaggeration);
				}
				p_VertexArray->push_back(point - mpr_local_origin);
				p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin);
			}
			//mpr_p_west_northhalf_geom->setVertexArray(p_VertexArray.get());
			// 添加修改vecarray策略 2013.8.19
			CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_west_northhalf_geom.get(),p_VertexArray.get());
			mpr_terrain->AddOperation(_opera);
		}
		break;
	case GLB_WESTEDGE_SOUTHSECTION:
		if (mpr_p_west_southhalf_geom)
		{			
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;			
			p_VertexArray->reserve(mpr_p_west_southhalf_vertexarray->size()*2);			
			for (long k = 0; k < (long)mpr_p_west_southhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_west_southhalf_vertexarray->at(k).x(),mpr_p_west_southhalf_vertexarray->at(k).y(),mpr_p_west_southhalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_west_southhalf_vertexarray->at(k);
					point.set( mpr_p_west_southhalf_vertexarray->at(k).y(), mpr_p_west_southhalf_vertexarray->at(k).x(), mpr_p_west_southhalf_vertexarray->at(k).z()+exaggeration);
				}
				p_VertexArray->push_back(point - mpr_local_origin);
				p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin);			
			}
			//mpr_p_west_southhalf_geom->setVertexArray(p_VertexArray.get());		
			// 添加修改vecarray策略 2013.8.19
			CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_west_southhalf_geom.get(),p_VertexArray.get());
			mpr_terrain->AddOperation(_opera);
		}
		break;
	case GLB_EASTEDGE_NORTHSECTION:
		if (mpr_p_east_northhalf_geom)
		{			
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;			
			p_VertexArray->reserve(mpr_p_east_northhalf_vertexarray->size()*2);		
			for (long k = 0; k < (long)mpr_p_east_northhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_east_northhalf_vertexarray->at(k).x(),mpr_p_east_northhalf_vertexarray->at(k).y(),mpr_p_east_northhalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_east_northhalf_vertexarray->at(k);
					point.set( mpr_p_east_northhalf_vertexarray->at(k).y(), mpr_p_east_northhalf_vertexarray->at(k).x(), mpr_p_east_northhalf_vertexarray->at(k).z()+exaggeration);
				}
				p_VertexArray->push_back(point - mpr_local_origin);	
				p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin);			
			}
			//mpr_p_east_northhalf_geom->setVertexArray(p_VertexArray.get());		
			// 添加修改vecarray策略 2013.8.19
			CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_east_northhalf_geom.get(),p_VertexArray.get());
			mpr_terrain->AddOperation(_opera);
		}
		break;
	case GLB_EASTEDGE_SOUTHSECTION:
		if (mpr_p_east_southhalf_geom)
		{
			osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array;			
			p_VertexArray->reserve(mpr_p_east_southhalf_vertexarray->size()*2);
			for (long k = 0; k < (long)mpr_p_east_southhalf_vertexarray->size(); k++)
			{
				if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
					g_ellipsmodel->convertLatLongHeightToXYZ(
						mpr_p_east_southhalf_vertexarray->at(k).x(),mpr_p_east_southhalf_vertexarray->at(k).y(),mpr_p_east_southhalf_vertexarray->at(k).z(),
						point.x(), point.y(), point.z() );
				}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
					//point = mpr_p_east_southhalf_vertexarray->at(k);
					point.set( mpr_p_east_southhalf_vertexarray->at(k).y(), mpr_p_east_southhalf_vertexarray->at(k).x(), mpr_p_east_southhalf_vertexarray->at(k).z()+exaggeration);
				}

				p_VertexArray->push_back(point - mpr_local_origin);
				p_VertexArray->push_back(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin);
			}
			//mpr_p_east_southhalf_geom->setVertexArray(p_VertexArray.get());			
			// 添加修改vecarray策略 2013.8.19
			CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_east_southhalf_geom.get(),p_VertexArray.get());
			mpr_terrain->AddOperation(_opera);
		}
		break;
	}

}

void CGlbGlobeTerrainTile::RemoveEdgeGeometry(GlbTileEdgeEnum edge)
{
	if (!mpr_p_geode)
		return;

	switch(edge)
	{
	case GLB_WESTEDGE_NORTHSECTION:		
		{
			if (mpr_p_west_northhalf_geom && mpr_p_geode->containsDrawable(mpr_p_west_northhalf_geom.get()))
			{// 如果存在补边面片则移除之 remove drawable
				CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_northhalf_geom.get());
				mpr_terrain->AddOperation(_opera);
			}	
			//if (mpr_nwchild && mpr_nwchild->IsInitialized())
			//{
			//	mpr_nwchild->RemoveEdgeGeometry(GLB_WESTEDGE_NORTHSECTION);
			//	mpr_nwchild->RemoveEdgeGeometry(GLB_WESTEDGE_SOUTHSECTION);
			//}
			//mpr_p_west_northhalf_geom = NULL;
		}
		break;
	case GLB_WESTEDGE_SOUTHSECTION:
		{
			if (mpr_p_west_southhalf_geom && mpr_p_geode->containsDrawable(mpr_p_west_southhalf_geom.get()))
			{// 如果存在补边面片则移除之 remove drawable
				CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_west_southhalf_geom.get());
				mpr_terrain->AddOperation(_opera);
			}
			//mpr_p_west_southhalf_geom = NULL;
		}
		break;
	case GLB_EASTEDGE_NORTHSECTION:
		{
			if (mpr_p_east_northhalf_geom && mpr_p_geode->containsDrawable(mpr_p_east_northhalf_geom.get()))
			{// 如果存在补边面片则移除之 remove drawable
				CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_east_northhalf_geom.get());
				mpr_terrain->AddOperation(_opera);
			}
			//mpr_p_east_northhalf_geom = NULL;
		}
		break;
	case GLB_EASTEDGE_SOUTHSECTION:
		{
			if (mpr_p_east_southhalf_geom && mpr_p_geode->containsDrawable(mpr_p_east_southhalf_geom.get()))
			{// 如果存在补边面片则移除之 remove drawable
				CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_east_southhalf_geom.get());
				mpr_terrain->AddOperation(_opera);
			}	
			//mpr_p_east_southhalf_geom = NULL;
		}
		break;
	case GLB_NORTHEDGE_EASTSECTION:
		{
			if (mpr_p_north_easthalf_geom && mpr_p_geode->containsDrawable(mpr_p_north_easthalf_geom.get()))
			{// 如果存在补边面片则移除之 remove drawable
				CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_easthalf_geom.get());
				mpr_terrain->AddOperation(_opera);
			}
			//mpr_p_north_easthalf_geom = NULL;
		}	
		break;
	case GLB_NORTHEDGE_WESTSECTION:		
		{
			if (mpr_p_north_westhalf_geom && mpr_p_geode->containsDrawable(mpr_p_north_westhalf_geom.get()))
			{// 如果存在补边面片则移除之 remove drawable
				CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_north_westhalf_geom.get());
				mpr_terrain->AddOperation(_opera);
			}
			//mpr_p_north_westhalf_geom = NULL;
		}		
		break;
	case GLB_SOUTHEDGE_EASTSECTION:
		{
			if (mpr_p_south_easthalf_geom && mpr_p_geode->containsDrawable(mpr_p_south_easthalf_geom.get()))
			{// 如果存在补边面片则移除之 remove drawable
				CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_south_easthalf_geom.get());
				mpr_terrain->AddOperation(_opera);
			}
			//mpr_p_south_easthalf_geom = NULL;
		}		
		break;
	case GLB_SOUTHEDGE_WESTSECTION:
		{
			if (mpr_p_south_westhalf_geom && mpr_p_geode->containsDrawable(mpr_p_south_westhalf_geom.get()))
			{// 如果存在补边面片则移除之 remove drawable
				CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_REMOVE,CGlbGlobeTerrain::GLB_DRAWABLE,mpr_p_geode.get(),mpr_p_south_westhalf_geom.get());
				mpr_terrain->AddOperation(_opera);
			}
			//mpr_p_south_westhalf_geom = NULL;
		}
		break;
	}
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetNorthDirectionTile()
{
	glbInt32 levelzero_tilesize_degrees = mpr_terrain->GetLevelZeroTileSizeOfDegrees();
	glbInt32 latitude_cnt = 180.0 / levelzero_tilesize_degrees + 0.5;
	glbInt32 longitude_cnt = 360.0 / levelzero_tilesize_degrees + 0.5;
	glbInt32 topmost_tile_row = GetRowFromLatitude(/*mpr_south*/mpr_north, levelzero_tilesize_degrees);
	glbInt32 topmost_tile_column = GetColFromLongitude(mpr_west, levelzero_tilesize_degrees);
	glbInt32 key = topmost_tile_row * longitude_cnt + topmost_tile_column;

	// 1.判断north是否在0级的边界位置-90.0,-54.0,-18.0,18.0,54.0,90.0
	// 1.1首先计算tile所属的level_0的块范围
	CGlbGlobeTerrainTile* p_topmost_parent = mpr_terrain->GetTerrainLevelZeroTile(key);
	glbDouble topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt;
	topmost_parent_east = topmost_parent_west = topmost_parent_south = topmost_parent_north = -360;
	if (p_topmost_parent)
		p_topmost_parent->GetRange(topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt);
	CGlbGlobeTerrainTile* p_north_tile = NULL;	
	if ( fabs(mpr_north - topmost_parent_north) < FAZHI)
		//if (long(mpr_north+90) % levelzero_tilesize_degrees == 0)
	{// 在块的north在其所属0级tile的north边界位置,已经是0级块的最北边了		
		//glbInt32 north_tile_key = (topmost_tile_row + 1) * longitude_cnt + topmost_tile_column;
		glbInt32 north_tile_key = (topmost_tile_row - 1) * longitude_cnt + topmost_tile_column;
		p_north_tile = mpr_terrain->GetTerrainLevelZeroTile(north_tile_key);			
	}
	else{
		switch(mpr_location_enum)
		{
		case GLB_NORTHEAST:		
		case GLB_NORTHWEST:
			{// tile的north和south完全包含mpr_north的tile是我们要找的tile 
				CGlbGlobeTerrainTile* p_tile = GetParentTile();				
				glbDouble _east,_west,_south,_north,_minAlt,_maxAlt;				
				while(p_tile)
				{
					p_tile->GetRange(_east,_west,_south,_north,_minAlt,_maxAlt);
					if ((mpr_north-_north)*(mpr_north-_south) < 0)
					{// 找到，跳出循环
						p_north_tile = p_tile;
						break;	//return p_north_tile;
					}
					p_tile = p_tile->GetParentTile();
				}	

				if(p_north_tile==NULL){
					bool imposible = true;
				}
				else
				{
					glbDouble east,west,south,north;
					p_north_tile->GetRange(east,west,south,north,_minAlt,_maxAlt);
					glbDouble center_latitude = ( north + south ) * 0.5;
					glbDouble center_longitude = ( west + east ) * 0.5;
					glbDouble center_longitude2 = ( mpr_west + mpr_east ) * 0.5;
					if (center_longitude2 < center_longitude )
					{
						p_north_tile = p_north_tile->GetNorthWestChild();
					}
					else
					{
						p_north_tile = p_north_tile->GetNorthEastChild();
					}
				}
			}			
			break;
		case GLB_SOUTHEAST:
			return GetParentTile()->GetNorthEastChild();
		case GlB_SOUTHWEST:
			return GetParentTile()->GetNorthWestChild();
			//p_north_tile = GetParentTile();
			//break;
		}
	}
	CGlbGlobeTerrainTile* pNorthnearest = NULL;
	if (p_north_tile)
	{
		pNorthnearest = p_north_tile->GetNorthNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);	
	}
	
	return pNorthnearest;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetNorthNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south)
{
	if ( fabs(mpr_west-west)<FAZHI && fabs(mpr_east-east)<FAZHI && fabs(mpr_south-north)<FAZHI)
		return this;
	if (mpr_swchild==NULL && mpr_sechild==NULL)
		return this;
	//if ( (mpr_p_southwest_childtile && mpr_p_southwest_childtile->IsInitialized()==FALSE) && 
	//			(mpr_p_southeast_childtile && mpr_p_southeast_childtile->IsInitialized()==FALSE) )
	//	return this;

	//double center_latitude = ( mpr_north + mpr_south ) * 0.5;
	glbDouble center_longitude = ( mpr_west + mpr_east ) * 0.5;
	CGlbGlobeTerrainTile* p_result = NULL;	
	if ( ((mpr_west-FAZHI)<=west) && ((center_longitude+FAZHI)>=east) && fabs(mpr_south-north)<FAZHI)
	{// northwest位置		
		if (mpr_swchild)// && mpr_p_southwest_childtile->IsInitialized() && !p_result)
		{
			p_result = mpr_swchild->GetNorthNeighbourInTileRange(west,east,north,south);
		}		
		//else if (mpr_p_southwest_childtile)
		//	return mpr_p_southwest_childtile;
		else
			return this;
	}
	if ( ((center_longitude-FAZHI)<=west) && ((mpr_east+FAZHI)>=east) && fabs(mpr_south-north)<FAZHI)
	{
		if (mpr_sechild)// && mpr_p_southeast_childtile->IsInitialized() && !p_result)
		{
			p_result = mpr_sechild->GetNorthNeighbourInTileRange(west,east,north,south);
		}		
		//else if (mpr_p_southeast_childtile)
		//	return mpr_p_southeast_childtile;
		else
			return this;
	}
	return p_result;	
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetWestDirectionTile()
{
	glbInt32 levelzero_tilesize_degrees = mpr_terrain->GetLevelZeroTileSizeOfDegrees();
	glbInt32 latitude_cnt = 180.0 / levelzero_tilesize_degrees + 0.5;
	glbInt32 longitude_cnt = 360.0 / levelzero_tilesize_degrees + 0.5;
	glbInt32 topmost_tile_row = GetRowFromLatitude(/*mpr_south*/mpr_north, levelzero_tilesize_degrees);
	glbInt32 topmost_tile_column = GetColFromLongitude(mpr_west, levelzero_tilesize_degrees);
	glbInt32 key = topmost_tile_row * longitude_cnt + topmost_tile_column;

	// 1.判断north是否在0级的边界位置-90.0,-54.0,-18.0,18.0,54.0,90.0
	// 1.1首先计算tile所属的level_0的块范围
	CGlbGlobeTerrainTile* p_topmost_parent = mpr_terrain->GetTerrainLevelZeroTile(key);
	glbDouble topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt;
	topmost_parent_east = topmost_parent_west = topmost_parent_south = topmost_parent_north = -360;
	if (p_topmost_parent)
		p_topmost_parent->GetRange(topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt);
	CGlbGlobeTerrainTile* p_west_tile = NULL;
	if ( fabs(mpr_west - topmost_parent_west) < FAZHI)
		//if ( long(mpr_west+180)%levelzero_tilesize_degrees == 0)
	{// 在块的north在其所属0级tile的north边界位置,已经是0级块的最北边了	
		glbInt32 west_tile_key = topmost_tile_row * longitude_cnt + topmost_tile_column - 1;
		//glbInt32 west_tile_key = (latitude_cnt-1-topmost_tile_row) * longitude_cnt + topmost_tile_column - 1;
		if (topmost_tile_column==0)// -180(180)度位置做循环处理
			west_tile_key = topmost_tile_row * longitude_cnt + longitude_cnt - 1;
		p_west_tile = mpr_terrain->GetTerrainLevelZeroTile(west_tile_key);			
	}else{
		switch(mpr_location_enum)
		{

		case GLB_NORTHEAST:		
			return GetParentTile()->GetNorthWestChild();
		case GLB_SOUTHEAST:
			return GetParentTile()->GetSouthWestChild();
		case GlB_SOUTHWEST:
		case GLB_NORTHWEST:
			{// tile的east和west完全包含mpr_west的tile是我们要找的tile 
				//p_west_tile = GetParentTile();
				CGlbGlobeTerrainTile* p_tile = GetParentTile();
				glbDouble _east,_west,_south,_north,_minAlt,_maxAlt;				
				while(p_tile)
				{
					p_tile->GetRange(_east,_west,_south,_north,_minAlt,_maxAlt);
					if ((mpr_west-_east)*(mpr_west-_west) < 0)
					{// 找到，返回之
						p_west_tile = p_tile;
						break;
					}
					p_tile = p_tile->GetParentTile();
				}	
				if (p_west_tile==NULL)
				{
					bool imposible = true;
				}
				else
				{
					glbDouble east,west,south,north;
					p_west_tile->GetRange(east,west,south,north,_minAlt,_maxAlt);
					glbDouble center_latitude = ( north + south ) * 0.5;				
					glbDouble center_latitude2 = ( mpr_north + mpr_south ) * 0.5;
					if (center_latitude2 < center_latitude )
					{
						p_west_tile = p_west_tile->GetSouthWestChild();
					}
					else
					{
						p_west_tile = p_west_tile->GetNorthWestChild();
					}			
				}
			}
			break;
		}
	}

	CGlbGlobeTerrainTile* pWestnearest = NULL;
	if (p_west_tile)
	{
		pWestnearest = p_west_tile->GetWestNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);
	}
	//// 验证找到的tile是否正确
	//if (pWestnearest)
	//{
	//	if (fabs(mpr_west-pWestnearest->mpr_east) > FAZHI && (mpr_west!=-180.0&&pWestnearest->mpr_east!=180.0))
	//	{
	//		WCHAR wBuff[128];
	//		wsprintf(wBuff,L"ERROR : GetWestDirectionTile error. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"WestnearestTile Level:%d, south:%.8f, north: %.8f, west: %.8f, east: %.8f\n",pWestnearest->mpr_level,pWestnearest->mpr_south,pWestnearest->mpr_north,pWestnearest->mpr_west,pWestnearest->mpr_east);
	//	}
	//}
	//else{
	//	pWestnearest = p_west_tile->GetWestNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);
	//	WCHAR wBuff[128];
	//	wsprintf(wBuff,L"ERROR : GetWestNeighbourInTileRange failed! [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//	CGlbSysLog::WriteLog(wBuff);
	//	_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//	CGlbSysLog::WriteLog(wBuff);
	//}	 
	return pWestnearest;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetWestNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south)
{
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		if ( fabs(mpr_south-south)<FAZHI && fabs(mpr_north-north)<FAZHI && (fabs(mpr_east-west)<FAZHI || (mpr_east==180.0&&west==-180.0)))
			return this;
	}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		if ( fabs(mpr_south-south)<FAZHI && fabs(mpr_north-north)<FAZHI && (fabs(mpr_east-west)<FAZHI))
			return this;
	}
	if (mpr_nechild==NULL && mpr_sechild==NULL)
		return this;
	//if ( (mpr_p_northwest_childtile && mpr_p_northwest_childtile->IsInitialized()==FALSE) && 
	//	(mpr_p_southwest_childtile && mpr_p_southwest_childtile->IsInitialized()==FALSE) )
	//	return this;

	glbDouble center_latitude = ( mpr_north + mpr_south ) * 0.5;
	CGlbGlobeTerrainTile* p_result = NULL;
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		if ( ((mpr_south-FAZHI)<=south) && ((center_latitude+FAZHI)>=north) && (fabs(mpr_east-west)<FAZHI || (mpr_east==180.0&&west==-180.0)))
		{// northwest位置		
			if (mpr_sechild)// && mpr_p_southwest_childtile->IsInitialized() && !p_result)
			{
				p_result = mpr_sechild->GetWestNeighbourInTileRange(west,east,north,south);
			}
			else
				return this;
		}
	}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		if ( ((mpr_south-FAZHI)<=south) && ((center_latitude+FAZHI)>=north) && (fabs(mpr_east-west)<FAZHI))
		{// northwest位置		
			if (mpr_sechild)// && mpr_p_southwest_childtile->IsInitialized() && !p_result)
			{
				p_result = mpr_sechild->GetWestNeighbourInTileRange(west,east,north,south);
			}
			else
				return this;
		}
	}
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
		if ( ((center_latitude-FAZHI)<=south) && ((mpr_north+FAZHI)>=north) && (fabs(mpr_east-west)<FAZHI || (mpr_east==180.0&&west==-180.0)))
		{
			if (mpr_nechild)// && mpr_p_northwest_childtile->IsInitialized() && !p_result)
			{
				p_result = mpr_nechild->GetWestNeighbourInTileRange(west,east,north,south);
			}	
			else
				return this;
		}
	}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		if ( ((center_latitude-FAZHI)<=south) && ((mpr_north+FAZHI)>=north) && (fabs(mpr_east-west)<FAZHI))
		{
			if (mpr_nechild)// && mpr_p_northwest_childtile->IsInitialized() && !p_result)
			{
				p_result = mpr_nechild->GetWestNeighbourInTileRange(west,east,north,south);
			}	
			else
				return this;
		}
	}
	return p_result;	
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetSouthDirectionTile()
{
	glbInt32 levelzero_tilesize_degrees = mpr_terrain->GetLevelZeroTileSizeOfDegrees();
	glbInt32 latitude_cnt = 180.0 / levelzero_tilesize_degrees + 0.5;
	glbInt32 longitude_cnt = 360.0 / levelzero_tilesize_degrees + 0.5;
	glbInt32 topmost_tile_row = GetRowFromLatitude(/*mpr_south*/mpr_north, levelzero_tilesize_degrees);
	glbInt32 topmost_tile_column = GetColFromLongitude(mpr_west, levelzero_tilesize_degrees);
	glbInt32 key = topmost_tile_row * longitude_cnt + topmost_tile_column;

	// 1.判断north是否在0级的边界位置-90.0,-54.0,-18.0,18.0,54.0,90.0
	// 1.1首先计算tile所属的level_0的块范围
	CGlbGlobeTerrainTile* p_topmost_parent = mpr_terrain->GetTerrainLevelZeroTile(key);
	glbDouble topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt;
	topmost_parent_east = topmost_parent_west = topmost_parent_south = topmost_parent_north = -360;
	if (p_topmost_parent)
		p_topmost_parent->GetRange(topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt);
	CGlbGlobeTerrainTile* p_south_tile = NULL;

	if ( fabs(mpr_south - topmost_parent_south) < FAZHI)
		//if (long(mpr_south+90)%levelzero_tilesize_degrees==0)
	{// 在块的north在其所属0级tile的north边界位置,已经是0级块的最北边了	
		//glbInt32 south_tile_key = (topmost_tile_row - 1) * longitude_cnt + topmost_tile_column;
		glbInt32 south_tile_key = (topmost_tile_row + 1) * longitude_cnt + topmost_tile_column;
		p_south_tile = mpr_terrain->GetTerrainLevelZeroTile(south_tile_key);			
	}else{
		switch(mpr_location_enum)
		{
		case GLB_NORTHEAST:		
			return GetParentTile()->GetSouthEastChild();
		case GLB_NORTHWEST:
			return GetParentTile()->GetSouthWestChild();		
		case GLB_SOUTHEAST:
		case GlB_SOUTHWEST:
			{// tile的north和south完全包含mpr_south的tile是我们要找的tile 
				CGlbGlobeTerrainTile* p_tile = GetParentTile();
				glbDouble _east,_west,_south,_north,_minAlt,_maxAlt;				
				while(p_tile)
				{
					p_tile->GetRange(_east,_west,_south,_north,_minAlt,_maxAlt);
					if ((mpr_south-_north)*(mpr_south-_south) < 0)
					{// 找到，返回之
						p_south_tile = p_tile;
						break;
					}
					p_tile = p_tile->GetParentTile();
				}	

				if (p_south_tile==NULL)
				{
					bool imposible = true;
				}
				else
				{
					glbDouble east,west,south,north;
					p_south_tile->GetRange(east,west,south,north,_minAlt,_maxAlt);
					glbDouble center_latitude = ( north + south ) * 0.5;
					glbDouble center_longitude = ( west + east ) * 0.5;
					glbDouble center_longitude2 = ( mpr_west + mpr_east ) * 0.5;
					if (center_longitude2 < center_longitude )
					{
						p_south_tile = p_south_tile->GetSouthWestChild();
					}
					else
					{
						p_south_tile = p_south_tile->GetSouthEastChild();
					}					
				}
			}
			break;
		}
	}

	CGlbGlobeTerrainTile* pSouthnearest = NULL;
	if (p_south_tile)
	{
		pSouthnearest = p_south_tile->GetSouthNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);
	}
	//// 验证正确性
	//if (pSouthnearest==NULL)
	//{
	//	if (long(mpr_south+90)%36!=0)
	//	{
	//		int efsdsfsdafs = 1;			
	//	}
	//}
	//// 验证找到的tile是否正确
	//if (pSouthnearest)
	//{
	//	if (fabs(mpr_south-pSouthnearest->mpr_north) > FAZHI)
	//	{
	//		WCHAR wBuff[128];
	//		wsprintf(wBuff,L"ERROR : GetSouthDirectionTile error. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"SouthnearestTile Level:%d, south:%.8f, north: %.8f, west: %.8f, east: %.8f\n",pSouthnearest->mpr_level,pSouthnearest->mpr_south,pSouthnearest->mpr_north,pSouthnearest->mpr_west,pSouthnearest->mpr_east);
	//	}
	//}
	//else{
	//	if (fabs(mpr_south+90.0) > FAZHI)
	//	{
	//		WCHAR wBuff[128];
	//		wsprintf(wBuff,L"ERROR : GetNorthNeighbourInTileRange failed! [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//		CGlbSysLog::WriteLog(wBuff);
	//	}
	//}	 
	return pSouthnearest;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetSouthNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south)
{
	if ( fabs(mpr_west-west)<FAZHI && fabs(mpr_east-east)<FAZHI && fabs(mpr_north-south)<FAZHI)
		return this;
	if (mpr_nwchild==NULL && mpr_nechild==NULL)
		return this;
	//if ( (mpr_p_northwest_childtile && mpr_p_northwest_childtile->IsInitialized()==FALSE) && 
	//	(mpr_p_northeast_childtile && mpr_p_northeast_childtile->IsInitialized()==FALSE) )
	//	return this;

	double center_longitude = ( mpr_west + mpr_east ) * 0.5;
	CGlbGlobeTerrainTile* p_result = NULL;
	if ( ((mpr_west-FAZHI)<=west) && ((center_longitude+FAZHI)>=east) && fabs(mpr_north-south)<FAZHI)
	{// northwest位置		
		if (mpr_nwchild)// && mpr_p_northwest_childtile->IsInitialized() && !p_result)
		{
			p_result = mpr_nwchild->GetSouthNeighbourInTileRange(west,east,north,south);
		}
		else
			return this;
	}
	if ( ((center_longitude-FAZHI)<=west) && ((mpr_east+FAZHI)>=east) && fabs(mpr_north-south)<FAZHI)
	{
		if (mpr_nechild)// && mpr_p_northeast_childtile->IsInitialized() && !p_result)
		{
			p_result = mpr_nechild->GetSouthNeighbourInTileRange(west,east,north,south);
		}	
		else
			return this;
	}
	return p_result;	
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetEastDirectionTile()
{
	glbInt32 levelzero_tilesize_degrees = mpr_terrain->GetLevelZeroTileSizeOfDegrees();
	glbInt32 latitude_cnt = 180.0 / levelzero_tilesize_degrees + 0.5;
	glbInt32 longitude_cnt = 360.0 / levelzero_tilesize_degrees + 0.5;
	glbInt32 topmost_tile_row = GetRowFromLatitude(/*mpr_south*/mpr_north, levelzero_tilesize_degrees);
	glbInt32 topmost_tile_column = GetColFromLongitude(mpr_west, levelzero_tilesize_degrees);
	glbInt32 key = topmost_tile_row * longitude_cnt + topmost_tile_column;

	// 1.判断north是否在0级的边界位置-90.0,-54.0,-18.0,18.0,54.0,90.0
	// 1.1首先计算tile所属的level_0的块范围
	CGlbGlobeTerrainTile* p_topmost_parent = mpr_terrain->GetTerrainLevelZeroTile(key);
	glbDouble topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt;
	topmost_parent_east = topmost_parent_west = topmost_parent_south = topmost_parent_north = -360;
	if (p_topmost_parent)
		p_topmost_parent->GetRange(topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt);
	CGlbGlobeTerrainTile* p_east_tile = NULL;
	if ( fabs(mpr_east - topmost_parent_east) < FAZHI)
		//if ( long(mpr_east+180)%levelzero_tilesize_degrees == 0)
	{// 在块的north在其所属0级tile的north边界位置,已经是0级块的最北边了	
		glbInt32 east_tile_key = topmost_tile_row * longitude_cnt + topmost_tile_column + 1;
		if (topmost_tile_column==longitude_cnt-1)// -180(180)度位置做循环处理
			east_tile_key = topmost_tile_row * longitude_cnt ;
		p_east_tile = mpr_terrain->GetTerrainLevelZeroTile(east_tile_key);			
	}else{
		switch(mpr_location_enum)
		{
		case GlB_SOUTHWEST:
			return GetParentTile()->GetSouthEastChild();
		case GLB_NORTHWEST:
			return GetParentTile()->GetNorthEastChild();
		case GLB_NORTHEAST:		
		case GLB_SOUTHEAST:
			{// tile的east和west完全包含mpr_east的tile是我们要找的tile 				
				CGlbGlobeTerrainTile* p_tile = GetParentTile();
				glbDouble _east,_west,_south,_north,_minAlt,_maxAlt;				
				while(p_tile)
				{
					p_tile->GetRange(_east,_west,_south,_north,_minAlt,_maxAlt);
					if ((mpr_east-_east)*(mpr_east-_west) < 0)
					{// 找到，返回之
						p_east_tile = p_tile;
						break;
					}
					p_tile = p_tile->GetParentTile();
				}	
				if (p_east_tile==NULL)
				{
					bool imposible = true;
				}
				else
				{
					glbDouble east,west,south,north;
					p_east_tile->GetRange(east,west,south,north,_minAlt,_maxAlt);
					glbDouble center_latitude = ( north + south ) * 0.5;				
					glbDouble center_latitude2 = ( mpr_north + mpr_south ) * 0.5;
					if (center_latitude2 < center_latitude )
					{
						p_east_tile = p_east_tile->GetSouthEastChild();
					}
					else
					{
						p_east_tile = p_east_tile->GetNorthEastChild();
					}					
				}
			}
			break;
		}
	}

	CGlbGlobeTerrainTile* pEastnearest = NULL;
	if (p_east_tile)
	{
		pEastnearest = p_east_tile->GetEastNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);
	}

	//// 验证找到的tile是否正确
	//if (pEastnearest)
	//{
	//	if (fabs(mpr_east-pEastnearest->mpr_west) > FAZHI && (mpr_east!=180.0&&pEastnearest->mpr_west!=-180.0))
	//	{
	//		WCHAR wBuff[128];
	//		wsprintf(wBuff,L"ERROR : GetEastDirectionTile error. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"EastnearestTile Level:%d, south:%.8f, north: %.8f, west: %.8f, east: %.8f\n",pEastnearest->mpr_level,pEastnearest->mpr_south,pEastnearest->mpr_north,pEastnearest->mpr_west,pEastnearest->mpr_east);
	//	}
	//}
	//else{
	//	pEastnearest = p_east_tile->GetEastNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);
	//	WCHAR wBuff[128];
	//	wsprintf(wBuff,L"ERROR : GetEastNeighbourInTileRange failed! [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//	CGlbSysLog::WriteLog(wBuff);
	//	_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//	CGlbSysLog::WriteLog(wBuff);
	//}	 
	return pEastnearest;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetEastNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south)
{
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		if ( fabs(mpr_south-south)<FAZHI && fabs(mpr_north-north)<FAZHI && (fabs(mpr_west-east)<FAZHI || (east==180.0&&mpr_west==-180.0)))
			return this;
	}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		if ( fabs(mpr_south-south)<FAZHI && fabs(mpr_north-north)<FAZHI && fabs(mpr_west-east)<FAZHI )
			return this;
	}
	if (mpr_nwchild==NULL && mpr_swchild==NULL)
		return this;
	//if ( (mpr_p_northwest_childtile && mpr_p_northwest_childtile->IsInitialized()==FALSE) && 
	//	(mpr_p_southwest_childtile && mpr_p_southwest_childtile->IsInitialized()==FALSE) )
	//	return this;

	double center_latitude = ( mpr_north + mpr_south ) * 0.5;
	CGlbGlobeTerrainTile* p_result = NULL;
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		if ( ((mpr_south-FAZHI)<=south) && ((center_latitude+FAZHI)>=north) && (fabs(mpr_west-east)<FAZHI || (east==180.0&&mpr_west==-180.0)))
		{// northwest位置		
			if (mpr_swchild)// && mpr_p_southwest_childtile->IsInitialized() && !p_result)
			{
				p_result = mpr_swchild->GetEastNeighbourInTileRange(west,east,north,south);
			}
			else
				return this;
		}
	}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		if ( ((mpr_south-FAZHI)<=south) && ((center_latitude+FAZHI)>=north) && fabs(mpr_west-east)<FAZHI )
		{// northwest位置		
			if (mpr_swchild)// && mpr_p_southwest_childtile->IsInitialized() && !p_result)
			{
				p_result = mpr_swchild->GetEastNeighbourInTileRange(west,east,north,south);
			}
			else
				return this;
		}
	}
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		if ( ((center_latitude-FAZHI)<=south) && ((mpr_north+FAZHI)>=north) && (fabs(mpr_west-east)<FAZHI || (east==180.0&&mpr_west==-180.0)))
		{
			if (mpr_nwchild)// && mpr_p_northwest_childtile->IsInitialized() && !p_result)
			{
				p_result = mpr_nwchild->GetEastNeighbourInTileRange(west,east,north,south);
			}	
			else
				return this;
		}
	}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		if ( ((center_latitude-FAZHI)<=south) && ((mpr_north+FAZHI)>=north) && (fabs(mpr_west-east)<FAZHI))
		{
			if (mpr_nwchild)// && mpr_p_northwest_childtile->IsInitialized() && !p_result)
			{
				p_result = mpr_nwchild->GetEastNeighbourInTileRange(west,east,north,south);
			}	
			else
				return this;
		}
	}

	return p_result;	
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetNorthDirectionTileOnFlatTerrain()
{
	CGlbGlobeTerrainTile* p_topmost_tile = GetTopMostTile(this);
	glbDouble topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt;	
	if (p_topmost_tile)
		p_topmost_tile->GetRange(topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt);

	CGlbGlobeTerrainTile* p_north_tile = NULL;	
	if ( fabs(mpr_north - topmost_parent_north) < FAZHI)
		//if (long(mpr_north+90) % levelzero_tilesize_degrees == 0)
	{// 在块的north在其所属0级tile的north边界位置,已经是0级块的最北边了		

		//long north_tile_key = (p_topmost_tile->mpr_row + 1) * longitude_cnt + topmost_tile_column;
		//p_north_tile = mpr_p_globe_terrain->GetTerrainLevelZeroTile(north_tile_key);	
		p_north_tile = mpr_terrain->GetTopmostNorthNeibourTile(p_topmost_tile);
	}
	else{
		switch(mpr_location_enum)
		{
		case GLB_NORTHEAST:		
		case GLB_NORTHWEST:
			{// tile的north和south完全包含mpr_north的tile是我们要找的tile 
				CGlbGlobeTerrainTile* p_tile = GetParentTile();				
				glbDouble _east,_west,_south,_north;				
				while(p_tile)
				{
					p_tile->GetRange(_east,_west,_south,_north,minAlt,maxAlt);
					if ((mpr_north-_north)*(mpr_north-_south) < 0)
					{// 找到，跳出循环
						p_north_tile = p_tile;
						break;	//return p_north_tile;
					}
					p_tile = p_tile->GetParentTile();
				}	

				if(p_north_tile==NULL){
					bool imposible = true;
				}
				else
				{
					glbDouble east,west,south,north;
					p_north_tile->GetRange(east,west,south,north,minAlt,maxAlt);
					glbDouble center_latitude = ( north + south ) * 0.5;
					glbDouble center_longitude = ( west + east ) * 0.5;
					glbDouble center_longitude2 = ( mpr_west + mpr_east ) * 0.5;
					if (center_longitude2 < center_longitude )
					{
						p_north_tile = p_north_tile->GetNorthWestChild();
					}
					else
					{
						p_north_tile = p_north_tile->GetNorthEastChild();
					}
				}
			}			
			break;
		case GLB_SOUTHEAST:
			return GetParentTile()->GetNorthEastChild();
		case GlB_SOUTHWEST:
			return GetParentTile()->GetNorthWestChild();
			//p_north_tile = GetParentTile();
			//break;
		}
	}
	CGlbGlobeTerrainTile* pNorthnearest = NULL;
	if (p_north_tile)
	{
		pNorthnearest = p_north_tile->GetNorthNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);	
	}

	//// 验证找到的tile是否正确
	//if (pNorthnearest)
	//{
	//	if (fabs(mpr_north-pNorthnearest->mpr_south) > FAZHI)
	//	{
	//		WCHAR wBuff[128];
	//		wsprintf(wBuff,L"ERROR : GetNorthDirectionTile error. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"NorthnearestTile Level:%d, south:%.8f, north: %.8f, west: %.8f, east: %.8f\n",pNorthnearest->mpr_level,pNorthnearest->mpr_south,pNorthnearest->mpr_north,pNorthnearest->mpr_west,pNorthnearest->mpr_east);
	//	}
	//}
	//else{
	//	//if (fabs(mpr_north-90.0) > FAZHI)
	//	//{
	//	//	WCHAR wBuff[128];
	//	//	wsprintf(wBuff,L"ERROR : GetNorthNeighbourInTileRange failed! [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//	//	CGlbSysLog::WriteLog(wBuff);
	//	//	_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//	//	CGlbSysLog::WriteLog(wBuff);
	//	//}
	//}	 
	return pNorthnearest;

}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetSouthDirectionTileOnFlatTerrain()
{
	CGlbGlobeTerrainTile* p_topmost_tile = GetTopMostTile(this);
	glbDouble topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt;	
	if (p_topmost_tile)
		p_topmost_tile->GetRange(topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt);

	CGlbGlobeTerrainTile* p_south_tile = NULL;
	if ( fabs(mpr_south - topmost_parent_south) < FAZHI)
		//if (long(mpr_south+90)%levelzero_tilesize_degrees==0)
	{// 在块的north在其所属0级tile的north边界位置,已经是0级块的最北边了	
		//long south_tile_key = (topmost_tile_row - 1) * longitude_cnt + topmost_tile_column;
		//p_south_tile = mpr_p_globe_terrain->GetTerrainLevelZeroTile(south_tile_key);			
		p_south_tile = mpr_terrain->GetTopmostSouthNeibourTile(p_topmost_tile);
	}else{
		switch(mpr_location_enum)
		{
		case GLB_NORTHEAST:		
			return GetParentTile()->GetSouthEastChild();
		case GLB_NORTHWEST:
			return GetParentTile()->GetSouthWestChild();		
		case GLB_SOUTHEAST:
		case GlB_SOUTHWEST:
			{// tile的north和south完全包含mpr_south的tile是我们要找的tile 
				CGlbGlobeTerrainTile* p_tile = GetParentTile();
				glbDouble _east,_west,_south,_north;				
				while(p_tile)
				{
					p_tile->GetRange(_east,_west,_south,_north,minAlt,maxAlt);
					if ((mpr_south-_north)*(mpr_south-_south) < 0)
					{// 找到，返回之
						p_south_tile = p_tile;
						break;
					}
					p_tile = p_tile->GetParentTile();
				}	

				if (p_south_tile==NULL)
				{
					bool imposible = true;
				}
				else
				{
					glbDouble east,west,south,north;
					p_south_tile->GetRange(east,west,south,north,minAlt,maxAlt);
					glbDouble center_latitude = ( north + south ) * 0.5;
					glbDouble center_longitude = ( west + east ) * 0.5;
					glbDouble center_longitude2 = ( mpr_west + mpr_east ) * 0.5;
					if (center_longitude2 < center_longitude )
					{
						p_south_tile = p_south_tile->GetSouthWestChild();
					}
					else
					{
						p_south_tile = p_south_tile->GetSouthEastChild();
					}					
				}
			}
			break;
		}
	}

	CGlbGlobeTerrainTile* pSouthnearest = NULL;
	if (p_south_tile)
	{
		pSouthnearest = p_south_tile->GetSouthNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);
	}
	//// 验证正确性
	//if (pSouthnearest==NULL)
	//{
	//	if (long(mpr_south+90)%36!=0)
	//	{
	//		int efsdsfsdafs = 1;			
	//	}
	//}
	//// 验证找到的tile是否正确
	//if (pSouthnearest)
	//{
	//	if (fabs(mpr_south-pSouthnearest->mpr_north) > FAZHI)
	//	{			
	//		WCHAR wBuff[128];
	//		wsprintf(wBuff,L"ERROR : GetSouthDirectionTile error. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"SouthnearestTile Level:%d, south:%.8f, north: %.8f, west: %.8f, east: %.8f\n",pSouthnearest->mpr_level,pSouthnearest->mpr_south,pSouthnearest->mpr_north,pSouthnearest->mpr_west,pSouthnearest->mpr_east);
	//	}
	//}
	//else{
	//	//if (fabs(mpr_south+90.0) > FAZHI)
	//	//{
	//	//	WCHAR wBuff[128];
	//	//	wsprintf(wBuff,L"ERROR : GetNorthNeighbourInTileRange failed! [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//	//	CGlbSysLog::WriteLog(wBuff);
	//	//	_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//	//	CGlbSysLog::WriteLog(wBuff);
	//	//}
	//}	 
	return pSouthnearest;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetEastDirectionTileOnFlatTerrain()
{
	CGlbGlobeTerrainTile* p_topmost_tile = GetTopMostTile(this);
	glbDouble topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt;	
	if (p_topmost_tile)
		p_topmost_tile->GetRange(topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt);
	CGlbGlobeTerrainTile* p_east_tile = NULL;
	if ( fabs(mpr_east - topmost_parent_east) < FAZHI)
		//if ( long(mpr_east+180)%levelzero_tilesize_degrees == 0)
	{// 在块的north在其所属0级tile的north边界位置,已经是0级块的最北边了	
		p_east_tile = mpr_terrain->GetTopmostEastNeibourTile(p_topmost_tile);
	}else{
		switch(mpr_location_enum)
		{
		case GlB_SOUTHWEST:
			return GetParentTile()->GetSouthEastChild();
		case GLB_NORTHWEST:
			return GetParentTile()->GetNorthEastChild();
		case GLB_NORTHEAST:		
		case GLB_SOUTHEAST:
			{// tile的east和west完全包含mpr_east的tile是我们要找的tile 				
				CGlbGlobeTerrainTile* p_tile = GetParentTile();
				glbDouble _east,_west,_south,_north;				
				while(p_tile)
				{
					p_tile->GetRange(_east,_west,_south,_north,minAlt,maxAlt);
					if ((mpr_east-_east)*(mpr_east-_west) < 0)
					{// 找到，返回之
						p_east_tile = p_tile;
						break;
					}
					p_tile = p_tile->GetParentTile();
				}	
				if (p_east_tile==NULL)
				{
					bool imposible = true;
				}
				else
				{
					glbDouble east,west,south,north;
					p_east_tile->GetRange(east,west,south,north,minAlt,maxAlt);
					glbDouble center_latitude = ( north + south ) * 0.5;				
					glbDouble center_latitude2 = ( mpr_north + mpr_south ) * 0.5;
					if (center_latitude2 < center_latitude )
					{
						p_east_tile = p_east_tile->GetSouthEastChild();
					}
					else
					{
						p_east_tile = p_east_tile->GetNorthEastChild();
					}					
				}
			}
			break;
		}
	}

	CGlbGlobeTerrainTile* pEastnearest = NULL;
	if (p_east_tile)
	{
		pEastnearest = p_east_tile->GetEastNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);
	}
	else
		return NULL;
	//// 验证找到的tile是否正确
	//if (pEastnearest)
	//{
	//	if (fabs(mpr_east-pEastnearest->mpr_west) > FAZHI && (mpr_east!=180.0&&pEastnearest->mpr_west!=-180.0))
	//	{
	//		WCHAR wBuff[128];
	//		wsprintf(wBuff,L"ERROR : GetEastDirectionTile error. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"EastnearestTile Level:%d, south:%.8f, north: %.8f, west: %.8f, east: %.8f\n",pEastnearest->mpr_level,pEastnearest->mpr_south,pEastnearest->mpr_north,pEastnearest->mpr_west,pEastnearest->mpr_east);
	//	}
	//}
	//else{
	//	pEastnearest = p_east_tile->GetEastNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);
	//	WCHAR wBuff[128];
	//	wsprintf(wBuff,L"ERROR : GetEastNeighbourInTileRange failed! [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//	CGlbSysLog::WriteLog(wBuff);
	//	_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//	CGlbSysLog::WriteLog(wBuff);
	//}	 
	return pEastnearest;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetWestDirectionTileOnFlatTerrain()
{
	CGlbGlobeTerrainTile* p_topmost_tile = GetTopMostTile(this);
	glbDouble topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt;	
	if (p_topmost_tile)
		p_topmost_tile->GetRange(topmost_parent_east,topmost_parent_west,topmost_parent_south,topmost_parent_north,minAlt,maxAlt);

	CGlbGlobeTerrainTile* p_west_tile = NULL;
	if ( fabs(mpr_west - topmost_parent_west) < FAZHI)
		//if ( long(mpr_west+180)%levelzero_tilesize_degrees == 0)
	{// 在块的north在其所属0级tile的north边界位置,已经是0级块的最北边了	
		p_west_tile = mpr_terrain->GetTopmostWestNeibourTile(p_topmost_tile);
	}else{
		switch(mpr_location_enum)
		{
		case GLB_NORTHEAST:		
			return GetParentTile()->GetNorthWestChild();
		case GLB_SOUTHEAST:
			return GetParentTile()->GetSouthWestChild();
		case GlB_SOUTHWEST:
		case GLB_NORTHWEST:
			{// tile的east和west完全包含mpr_west的tile是我们要找的tile 
				//p_west_tile = GetParentTile();
				CGlbGlobeTerrainTile* p_tile = GetParentTile();
				glbDouble _east,_west,_south,_north,_minAlt,_maxAlt;				
				while(p_tile)
				{
					p_tile->GetRange(_east,_west,_south,_north,_minAlt,_maxAlt);
					if ((mpr_west-_east)*(mpr_west-_west) < 0)
					{// 找到，返回之
						p_west_tile = p_tile;
						break;
					}
					p_tile = p_tile->GetParentTile();
				}	
				if (p_west_tile==NULL)
				{
					bool imposible = true;
				}
				else
				{
					glbDouble east,west,south,north;
					p_west_tile->GetRange(east,west,south,north,_minAlt,_maxAlt);
					glbDouble center_latitude = ( north + south ) * 0.5;				
					glbDouble center_latitude2 = ( mpr_north + mpr_south ) * 0.5;
					if (center_latitude2 < center_latitude )
					{
						p_west_tile = p_west_tile->GetSouthWestChild();
					}
					else
					{
						p_west_tile = p_west_tile->GetNorthWestChild();
					}			
				}
			}
			break;
		}
	}

	CGlbGlobeTerrainTile* pWestnearest = NULL;
	if (p_west_tile)
	{
		pWestnearest = p_west_tile->GetWestNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);
	}
	else 
		return NULL;
	//// 验证找到的tile是否正确
	//if (pWestnearest)
	//{
	//	if (fabs(mpr_west-pWestnearest->mpr_east) > FAZHI && (mpr_west!=-180.0&&pWestnearest->mpr_east!=180.0))
	//	{
	//		WCHAR wBuff[128];
	//		wsprintf(wBuff,L"ERROR : GetWestDirectionTile error. [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//		CGlbSysLog::WriteLog(wBuff);
	//		_swprintf(wBuff,L"WestnearestTile Level:%d, south:%.8f, north: %.8f, west: %.8f, east: %.8f\n",pWestnearest->mpr_level,pWestnearest->mpr_south,pWestnearest->mpr_north,pWestnearest->mpr_west,pWestnearest->mpr_east);
	//	}
	//}
	//else{		
	//	pWestnearest = p_west_tile->GetWestNeighbourInTileRange(mpr_west,mpr_east,mpr_north,mpr_south);
	//	WCHAR wBuff[128];
	//	wsprintf(wBuff,L"ERROR : GetWestNeighbourInTileRange failed! [Level: %d , row: %d, col: %d]\n",mpr_level,(long)mpr_row,(long)mpr_column);
	//	CGlbSysLog::WriteLog(wBuff);
	//	_swprintf(wBuff,L"south:%.3f, north: %.8f, west: %.8f, east: %.8f\n",mpr_south,mpr_north,mpr_west,mpr_east);
	//	CGlbSysLog::WriteLog(wBuff);
	//}	 
	return pWestnearest;
}

osg::ref_ptr<osg::Vec3dArray> CGlbGlobeTerrainTile::GetNeighbourTileEdgeVecArray_Ext(CGlbGlobeTerrainTile* p_neighbour_tile,GlbTileEdgeEnum base_edge)
{
	glbDouble east,west,south,north,minAlt,maxAlt;
	p_neighbour_tile->GetRange(east,west,south,north,minAlt,maxAlt);
	glbDouble neighbour_center_latitude = (south + north)*0.5;
	glbDouble neighbour_center_longitude = (west + east)*0.5;

	glbDouble latitude_span = osg::DegreesToRadians((mpr_north-mpr_south) / mpr_vertex_count * 0.5);
	glbDouble longitude_span = osg::DegreesToRadians((mpr_east-mpr_west) / mpr_vertex_count * 0.5);
	if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
		 latitude_span = (mpr_north-mpr_south) / mpr_vertex_count * 0.5;
		 longitude_span = (mpr_east-mpr_west) / mpr_vertex_count * 0.5;
	}

	osg::Vec3dArray* p_edgeVertexArray = NULL;
	osg::ref_ptr<osg::Vec3dArray> p_half_edgeVertexArray = NULL;
	osg::ref_ptr<osg::Vec3dArray> p_new_half_edgeVertexArray = NULL;
	glbInt32 k = 0;
	glbDouble t = 0.0;
	glbDouble z = 0.0;
	osg::Vec3d point;
	//osg::ref_ptr<osg::EllipsoidModel> ellips = new osg::EllipsoidModel(osg::WGS_84_RADIUS_EQUATOR,osg::WGS_84_RADIUS_POLAR);

	glbDouble vLen=0.0;
	osg::Vec3d line_dir;
	glbDouble longitude_range = 0.0;
	glbDouble latitude_range = 0.0;
	switch (base_edge)
	{
	case GLB_NORTHEDGE_WESTSECTION:	
		{	
			glbDouble north_westhalf_center_longitude = mpr_west + (mpr_east - mpr_west) * 0.25;
			if (north_westhalf_center_longitude < neighbour_center_longitude){
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_WESTSECTION);
			}
			else{
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_EASTSECTION);
			}		
			osg::Vec3d vtStart = mpr_p_north_westhalf_vertexarray->at(0);
			osg::Vec3d vtEnd = mpr_p_north_westhalf_vertexarray->at(mpr_p_north_westhalf_vertexarray->size()-1);
			//////////////////////////////////////////////////////////////////////////
			glbInt32 index_0 = -1;
			glbInt32 index_1 = -1;
			glbDouble longitude0,longitude1;
			glbDouble latitude = p_edgeVertexArray->at(0).x();

			for (k = 0; k < (glbInt32)p_edgeVertexArray->size()-1; k++)
			{
				longitude0 = p_edgeVertexArray->at(k).y();
				longitude1 = p_edgeVertexArray->at(k+1).y();
				if (longitude0-longitude_span < vtStart.y()	&& (fabs(p_edgeVertexArray->at(k).x()-latitude) < FAZHI))
					index_0 = k;
				if  (longitude1+longitude_span > vtEnd.y() 	&& (fabs(p_edgeVertexArray->at(k).x()-latitude) < FAZHI) && (index_1==-1))
					index_1 = k+1;
			}
			if (index_0>-1 && index_1>-1 && index_1>index_0)
			{
				glbBool bInitialized = p_neighbour_tile->IsInitialized();
				glbInt32 cc = index_1-index_0;

				p_half_edgeVertexArray = new osg::Vec3dArray;
				for (k = index_0; k <= index_1; k++){				
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
						g_ellipsmodel->convertLatLongHeightToXYZ(
							p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).z(),						
							point.x(), point.y(), point.z() );
					}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
						//point = p_edgeVertexArray->at(k);
						point.set(p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).z());
					}
					p_half_edgeVertexArray->push_back(point);// - mpr_local_origin);
				}
				// 非initialized，奇数位置的坐标位置需要纠正
				if (bInitialized==false && (p_half_edgeVertexArray->size()%2==1)){
					for (k = 1; k < (glbInt32)p_half_edgeVertexArray->size(); k+=2){
						osg::Vec3d v = (p_half_edgeVertexArray->at(k-1)+p_half_edgeVertexArray->at(k+1))*0.5;
						p_half_edgeVertexArray->at(k).set(v);
					}
				}
				else if (bInitialized==false && p_half_edgeVertexArray->size()==2){
					osg::Vec3d v1,v2;
					if (index_0%2==1){		
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).z());
							v2.set(p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).z());
						}
						p_half_edgeVertexArray->at(0).set((v1+v2)*0.5);// - mpr_local_origin);
					}
					if (index_1%2==1){
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}
						else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).z());
							v2.set(p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).z());
						}
						p_half_edgeVertexArray->at(1).set((v1+v2)*0.5);// - mpr_local_origin);
					}
				}
				else if (bInitialized==false){
					int error = 1;
				}
				//----------------------------------
				if (p_half_edgeVertexArray->size()==9){				
					p_new_half_edgeVertexArray = new osg::Vec3dArray;					
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size(); k++){												
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));							
					}					
				}
				else if(p_half_edgeVertexArray->size()==5)
				{//
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{						
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));
						p_new_half_edgeVertexArray->push_back((p_half_edgeVertexArray->at(k)+p_half_edgeVertexArray->at(k+1))*0.5);							
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==3)
				{// 有问题
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{
						vtStart = p_half_edgeVertexArray->at(k);
						vtEnd = p_half_edgeVertexArray->at(k+1);
						osg::Vec3d vtCenter = (vtStart+vtEnd)*0.5;
						p_new_half_edgeVertexArray->push_back(vtStart);
						p_new_half_edgeVertexArray->push_back((vtStart+vtCenter)*0.5);
						p_new_half_edgeVertexArray->push_back(vtCenter);
						p_new_half_edgeVertexArray->push_back((vtEnd+vtCenter)*0.5);

/*						for (long m = 0; m < 4 ; m++)
						{
							p_new_half_edgeVertexArray->push_back(vtStart+(vtEnd-vtStart)*0.25*m);
						}	*/		
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==2)
				{// 有问题
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					longitude0 = p_edgeVertexArray->at(index_0).y();
					longitude1 = p_edgeVertexArray->at(index_1).y();
					longitude_range = longitude1 - longitude0;

					vtStart = p_half_edgeVertexArray->at(0);

					line_dir = p_half_edgeVertexArray->at(1)-p_half_edgeVertexArray->at(0);					
					for (k = 0; k < (long)mpr_p_north_westhalf_vertexarray->size(); k++)
					{
						vLen = (mpr_p_north_westhalf_vertexarray->at(k).y()-longitude0) / longitude_range;
						p_new_half_edgeVertexArray->push_back(vtStart+line_dir*vLen);
					}
				}
				else{//p_half_edgeVertexArray->size() == 9 5 3 2 以外的数
					int error = 1;
				}
			}
			else{//if (index_0>-1 && index_1>-1 && index_1>index_0)
				int error = 1;
			}		
		}
		break;
	case GLB_NORTHEDGE_EASTSECTION:
		{
			glbDouble north_easthalf_center_longitude = mpr_east - (mpr_east - mpr_west) * 0.25;
			if (north_easthalf_center_longitude < neighbour_center_longitude){
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_WESTSECTION);
			}
			else{
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_EASTSECTION);
			}					
			osg::Vec3d vtStart = mpr_p_north_easthalf_vertexarray->at(0);
			osg::Vec3d vtEnd = mpr_p_north_easthalf_vertexarray->at(mpr_p_north_easthalf_vertexarray->size()-1);
			//////////////////////////////////////////////////////////////////////////
			glbInt32 index_0 = -1;
			glbInt32 index_1 = -1;
			glbDouble longitude0,longitude1;
			glbDouble latitude = p_edgeVertexArray->at(0).x();

			for (k = 0; k < (glbInt32)p_edgeVertexArray->size()-1; k++)
			{
				longitude0 = p_edgeVertexArray->at(k).y();
				longitude1 = p_edgeVertexArray->at(k+1).y();
				if (longitude0-longitude_span < vtStart.y()	&& (fabs(p_edgeVertexArray->at(k).x()-latitude) < FAZHI))
					index_0 = k;
				if  (longitude1+longitude_span > vtEnd.y() 	&& (fabs(p_edgeVertexArray->at(k).x()-latitude) < FAZHI) && (index_1==-1))
					index_1 = k+1;
			}
			if (index_0>-1 && index_1>-1 && index_1>index_0)
			{
				glbBool bInitialized = p_neighbour_tile->IsInitialized();
				glbInt32 cc = index_1-index_0;

				p_half_edgeVertexArray = new osg::Vec3dArray;
				for (k = index_0; k <= index_1; k++){
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
						g_ellipsmodel->convertLatLongHeightToXYZ(
							p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).z(),						
							point.x(), point.y(), point.z() );
					}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
						point.set(p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).z());
					}
					p_half_edgeVertexArray->push_back(point);// - mpr_local_origin);
				}
				// 非initialized，奇数位置的坐标位置需要纠正
				if (bInitialized==false && (p_half_edgeVertexArray->size()%2==1)){
					for (k = 1; k < (glbInt32)p_half_edgeVertexArray->size(); k+=2){
						osg::Vec3d v = (p_half_edgeVertexArray->at(k-1)+p_half_edgeVertexArray->at(k+1))*0.5;
						p_half_edgeVertexArray->at(k).set(v);
					}
				}
				else if (bInitialized==false && p_half_edgeVertexArray->size()==2){
					osg::Vec3d v1,v2;
					if (index_0%2==1){	
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}
						else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).z());
							v2.set(p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).z());
						}						
						p_half_edgeVertexArray->at(0).set((v1+v2)*0.5);// - mpr_local_origin);
					}
					if (index_1%2==1){
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).z());
							v2.set(p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).z());
						}

						p_half_edgeVertexArray->at(1).set((v1+v2)*0.5);// - mpr_local_origin);
					}
				}
				else if (bInitialized==false){
					int error = 1;
				}
				//----------------------------------
				if (p_half_edgeVertexArray->size()==9){				
					p_new_half_edgeVertexArray = new osg::Vec3dArray;					
					for (k = 0; k < (long)p_half_edgeVertexArray->size(); k++){												
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));							
					}					
				}
				else if(p_half_edgeVertexArray->size()==5)
				{//
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (long)p_half_edgeVertexArray->size()-1; k++)
					{						
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));
						p_new_half_edgeVertexArray->push_back((p_half_edgeVertexArray->at(k)+p_half_edgeVertexArray->at(k+1))*0.5);							
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==3)
				{// 有问题
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (long)p_half_edgeVertexArray->size()-1; k++)
					{
						vtStart = p_half_edgeVertexArray->at(k);
						vtEnd = p_half_edgeVertexArray->at(k+1);
						osg::Vec3d vtCenter = (vtStart+vtEnd)*0.5;
						p_new_half_edgeVertexArray->push_back(vtStart);
						p_new_half_edgeVertexArray->push_back((vtStart+vtCenter)*0.5);
						p_new_half_edgeVertexArray->push_back(vtCenter);
						p_new_half_edgeVertexArray->push_back((vtEnd+vtCenter)*0.5);
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==2)
				{// 有问题
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					longitude0 = p_edgeVertexArray->at(index_0).y();
					longitude1 = p_edgeVertexArray->at(index_1).y();
					longitude_range = longitude1 - longitude0;

					vtStart = p_half_edgeVertexArray->at(0);

					line_dir = p_half_edgeVertexArray->at(1)-p_half_edgeVertexArray->at(0);
					for (k = 0; k < (long)mpr_p_north_easthalf_vertexarray->size(); k++)
					{
						vLen = (mpr_p_north_easthalf_vertexarray->at(k).y()-longitude0) / longitude_range;
						p_new_half_edgeVertexArray->push_back(vtStart+line_dir*vLen);
					}
				}
				else{//p_half_edgeVertexArray->size() == 9 5 3 2 以外的数
					int error = 1;
				}
			}
			else{//if (index_0>-1 && index_1>-1 && index_1>index_0)
				int error = 1;
			}
		}		
		break;
	case GLB_SOUTHEDGE_WESTSECTION:
		{	
			glbDouble south_westhalf_center_longitude = mpr_west + (mpr_east - mpr_west) * 0.25;
			if (south_westhalf_center_longitude < neighbour_center_longitude){
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_NORTHEDGE_WESTSECTION);
			}
			else{
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_NORTHEDGE_EASTSECTION);
			}	
			osg::Vec3d vtStart = mpr_p_south_westhalf_vertexarray->at(0);
			osg::Vec3d vtEnd = mpr_p_south_westhalf_vertexarray->at(mpr_p_south_westhalf_vertexarray->size()-1);
			//////////////////////////////////////////////////////////////////////////
			glbInt32 index_0 = -1;
			glbInt32 index_1 = -1;
			glbDouble longitude0,longitude1;
			glbDouble latitude = p_edgeVertexArray->at(0).x();
			
			for (k = 0; k < (glbInt32)p_edgeVertexArray->size()-1; k++)
			{
				longitude0 = p_edgeVertexArray->at(k).y();
				longitude1 = p_edgeVertexArray->at(k+1).y();
				if (longitude0-longitude_span < vtStart.y()	&& (fabs(p_edgeVertexArray->at(k).x()-latitude) < FAZHI))
					index_0 = k;
				if  (longitude1+longitude_span > vtEnd.y() 	&& (fabs(p_edgeVertexArray->at(k).x()-latitude) < FAZHI) && (index_1==-1))
					index_1 = k+1;
			}
			if (index_0>-1 && index_1>-1 && index_1>index_0)
			{
				glbBool bInitialized = p_neighbour_tile->IsInitialized();
				glbInt32 cc = index_1-index_0;

				p_half_edgeVertexArray = new osg::Vec3dArray;
				for (k = index_0; k <= index_1; k++){
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
						g_ellipsmodel->convertLatLongHeightToXYZ(
							p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).z(),						
							point.x(), point.y(), point.z() );
					}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
						//point = p_edgeVertexArray->at(k);
						point.set(p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).z());
					}

					p_half_edgeVertexArray->push_back(point);// - mpr_local_origin);
				}
				// 非initialized，奇数位置的坐标位置需要纠正
				if (bInitialized==false && (p_half_edgeVertexArray->size()%2==1)){
					for (k = 1; k < (glbInt32)p_half_edgeVertexArray->size(); k+=2){
						osg::Vec3d v = (p_half_edgeVertexArray->at(k-1)+p_half_edgeVertexArray->at(k+1))*0.5;
						p_half_edgeVertexArray->at(k).set(v);
					}
				}
				else if (bInitialized==FALSE && p_half_edgeVertexArray->size()==2){
					osg::Vec3d v1,v2;
					if (index_0%2==1){		
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).z());
							v2.set(p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).z());
						}
						p_half_edgeVertexArray->at(0).set((v1+v2)*0.5);// - mpr_local_origin);
					}
					if (index_1%2==1){
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).z());
							v2.set(p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).z());
						}
						p_half_edgeVertexArray->at(1).set((v1+v2)*0.5);// - mpr_local_origin);
					}
				}
				else if (bInitialized==false){
					int error = 1;
				}
				//----------------------------------
				if (p_half_edgeVertexArray->size()==9){				
					p_new_half_edgeVertexArray = new osg::Vec3dArray;					
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size(); k++){												
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));							
					}					
				}
				else if(p_half_edgeVertexArray->size()==5)
				{//
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{						
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));
						p_new_half_edgeVertexArray->push_back((p_half_edgeVertexArray->at(k)+p_half_edgeVertexArray->at(k+1))*0.5);							
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==3)
				{// 有问题
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{
						vtStart = p_half_edgeVertexArray->at(k);
						vtEnd = p_half_edgeVertexArray->at(k+1);
						osg::Vec3d vtCenter = (vtStart+vtEnd)*0.5;
						p_new_half_edgeVertexArray->push_back(vtStart);
						p_new_half_edgeVertexArray->push_back((vtStart+vtCenter)*0.5);
						p_new_half_edgeVertexArray->push_back(vtCenter);
						p_new_half_edgeVertexArray->push_back((vtEnd+vtCenter)*0.5);
/*						for (long m = 0; m < 4 ; m++)
						{
							p_new_half_edgeVertexArray->push_back(vtStart+(vtEnd-vtStart)*0.25*m);
						}	*/		
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==2)
				{// 有问题
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					longitude0 = p_edgeVertexArray->at(index_0).y();
					longitude1 = p_edgeVertexArray->at(index_1).y();
					longitude_range = longitude1 - longitude0;

					vtStart = p_half_edgeVertexArray->at(0);
					line_dir = p_half_edgeVertexArray->at(1)-p_half_edgeVertexArray->at(0);
					for (k = 0; k < (glbInt32)mpr_p_south_westhalf_vertexarray->size(); k++)
					{
						vLen = (mpr_p_south_westhalf_vertexarray->at(k).y()-longitude0) / longitude_range;
						p_new_half_edgeVertexArray->push_back(vtStart+line_dir*vLen);
					}
				}
				else{//p_half_edgeVertexArray->size() == 9 5 3 2 以外的数
					int error = 1;
				}
			}
			else{//if (index_0>-1 && index_1>-1 && index_1>index_0)
				int error = 1;
			}
		}
		break;
	case GLB_SOUTHEDGE_EASTSECTION:
		{
			glbDouble south_easthalf_center_longitude = mpr_east - (mpr_east - mpr_west) * 0.25;
			if (south_easthalf_center_longitude < neighbour_center_longitude){
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_NORTHEDGE_WESTSECTION);
			}
			else{
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_NORTHEDGE_EASTSECTION);
			}	
			osg::Vec3d vtStart = mpr_p_south_easthalf_vertexarray->at(0);
			osg::Vec3d vtEnd = mpr_p_south_easthalf_vertexarray->at(mpr_p_south_easthalf_vertexarray->size()-1);
			//////////////////////////////////////////////////////////////////////////
			glbInt32 index_0 = -1;
			glbInt32 index_1 = -1;
			glbDouble longitude0,longitude1;
			glbDouble latitude = p_edgeVertexArray->at(0).x();

			for (k = 0; k < (glbInt32)p_edgeVertexArray->size()-1; k++)
			{
				longitude0 = p_edgeVertexArray->at(k).y();
				longitude1 = p_edgeVertexArray->at(k+1).y();
				if (longitude0-longitude_span < vtStart.y()	&& (fabs(p_edgeVertexArray->at(k).x()-latitude) < FAZHI))
					index_0 = k;
				if  (longitude1+longitude_span > vtEnd.y() 	&& (fabs(p_edgeVertexArray->at(k).x()-latitude) < FAZHI) && (index_1==-1))
					index_1 = k+1;
			}
			if (index_0>-1 && index_1>-1 && index_1>index_0)
			{
				glbBool bInitialized = p_neighbour_tile->IsInitialized();
				glbInt32 cc = index_1-index_0;

				p_half_edgeVertexArray = new osg::Vec3dArray;
				for (k = index_0; k <= index_1; k++){
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
						g_ellipsmodel->convertLatLongHeightToXYZ(
							p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).z(),						
							point.x(), point.y(), point.z() );
					}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
						point.set(p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).z());
					}
					p_half_edgeVertexArray->push_back(point);// - mpr_local_origin);
				}
				// 非initialized，奇数位置的坐标位置需要纠正
				if (bInitialized==false && (p_half_edgeVertexArray->size()%2==1)){
					for (k = 1; k < (glbInt32)p_half_edgeVertexArray->size(); k+=2){
						osg::Vec3d v = (p_half_edgeVertexArray->at(k-1)+p_half_edgeVertexArray->at(k+1))*0.5;
						p_half_edgeVertexArray->at(k).set(v);
					}
				}
				else if (bInitialized==false && p_half_edgeVertexArray->size()==2){
					osg::Vec3d v1,v2;
					if (index_0%2==1){			
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).z());
							v2.set(p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).z());
						}

						p_half_edgeVertexArray->at(0).set((v1+v2)*0.5);// - mpr_local_origin);
					}
					if (index_1%2==1){
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).z());
							v2.set(p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).z());
						}
						p_half_edgeVertexArray->at(1).set((v1+v2)*0.5);// - mpr_local_origin);
					}
				}
				else if (bInitialized==false){
					int error = 1;
				}

				if (p_half_edgeVertexArray->size()==9){				
					p_new_half_edgeVertexArray = new osg::Vec3dArray;					
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size(); k++){												
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));							
					}					
				}
				else if(p_half_edgeVertexArray->size()==5)
				{
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{						
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));
						p_new_half_edgeVertexArray->push_back((p_half_edgeVertexArray->at(k)+p_half_edgeVertexArray->at(k+1))*0.5);							
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==3)
				{
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{
						vtStart = p_half_edgeVertexArray->at(k);
						vtEnd = p_half_edgeVertexArray->at(k+1);
						osg::Vec3d vtCenter = (vtStart+vtEnd)*0.5;
						p_new_half_edgeVertexArray->push_back(vtStart);
						p_new_half_edgeVertexArray->push_back((vtStart+vtCenter)*0.5);
						p_new_half_edgeVertexArray->push_back(vtCenter);
						p_new_half_edgeVertexArray->push_back((vtEnd+vtCenter)*0.5);
/*						for (long m = 0; m < 4 ; m++)
						{
							p_new_half_edgeVertexArray->push_back(vtStart+(vtEnd-vtStart)*0.25*m);
						}	*/		
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==2)
				{
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					longitude0 = p_edgeVertexArray->at(index_0).y();
					longitude1 = p_edgeVertexArray->at(index_1).y();
					longitude_range = longitude1 - longitude0;

					vtStart = p_half_edgeVertexArray->at(0);

					line_dir = p_half_edgeVertexArray->at(1)-p_half_edgeVertexArray->at(0);
					for (k = 0; k < (glbInt32)mpr_p_south_easthalf_vertexarray->size(); k++)
					{
						vLen = (mpr_p_south_easthalf_vertexarray->at(k).y()-longitude0) / longitude_range;
						p_new_half_edgeVertexArray->push_back(vtStart+line_dir*vLen);
					}
				}
				else{//p_half_edgeVertexArray->size() == 9 5 3 2 以外的数
					int error = 1;
				}
			}
			else{//if (index_0>-1 && index_1>-1 && index_1>index_0)
				int error = 1;
			}
			//////////////////////////////////////////////////////////////////////////
		}
		break;
	case GLB_WESTEDGE_NORTHSECTION:
		{
			glbDouble west_northhalf_center_latitude = mpr_north - (mpr_north - mpr_south) * 0.25;
			if (west_northhalf_center_latitude > neighbour_center_latitude){
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_EASTEDGE_NORTHSECTION);
			}
			else{
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_EASTEDGE_SOUTHSECTION);
			}	
			osg::Vec3d vtStart = mpr_p_west_northhalf_vertexarray->at(0);
			osg::Vec3d vtEnd = mpr_p_west_northhalf_vertexarray->at(mpr_p_west_northhalf_vertexarray->size()-1);
			//////////////////////////////////////////////////////////////////////////
			glbInt32 index_0 = -1;
			glbInt32 index_1 = -1;
			glbDouble latitude0,latitude1;
			glbDouble longitude = p_edgeVertexArray->at(0).y();

			for (k = 0; k < (glbInt32)p_edgeVertexArray->size()-1; k++)
			{
				latitude0 = p_edgeVertexArray->at(k).x();
				latitude1 = p_edgeVertexArray->at(k+1).x();
				if (latitude0+latitude_span  > vtStart.x()	&& (fabs(p_edgeVertexArray->at(k).y()-longitude) < FAZHI))
					index_0 = k;
				if  (latitude1-latitude_span < vtEnd.x()	&& (fabs(p_edgeVertexArray->at(k).y()-longitude) < FAZHI) && (index_1==-1))
					index_1 = k+1;
			}
			if (index_0>-1 && index_1>-1 && index_1>index_0)
			{
				glbBool bInitialized = p_neighbour_tile->IsInitialized();
				glbInt32 cc = index_1-index_0;

				p_half_edgeVertexArray = new osg::Vec3dArray;
				for (k = index_0; k <= index_1; k++){
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
						g_ellipsmodel->convertLatLongHeightToXYZ(
							p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).z(),						
							point.x(), point.y(), point.z() );
					}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
						point.set(p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).z());
					}
					p_half_edgeVertexArray->push_back(point);// - mpr_local_origin);
				}
				// 非initialized，奇数位置的坐标位置需要纠正
				if (bInitialized==false && (p_half_edgeVertexArray->size()%2==1)){
					for (k = 1; k < (long)p_half_edgeVertexArray->size(); k+=2){
						osg::Vec3d v = (p_half_edgeVertexArray->at(k-1)+p_half_edgeVertexArray->at(k+1))*0.5;
						p_half_edgeVertexArray->at(k).set(v);
					}
				}
				else if (bInitialized==false && p_half_edgeVertexArray->size()==2){
					osg::Vec3d v1,v2;
					if (index_0%2==1){	
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).z());
							v2.set(p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).z());
						}
						p_half_edgeVertexArray->at(0).set((v1+v2)*0.5);// - mpr_local_origin);
					}
					if (index_1%2==1){
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).z());
							v2.set(p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).z());
						}
						p_half_edgeVertexArray->at(1).set((v1+v2)*0.5);// - mpr_local_origin);
					}
				}
				else if (bInitialized==false){
					int error = 1;
				}
				//----------------------------------
				if (p_half_edgeVertexArray->size()==9){				
					p_new_half_edgeVertexArray = new osg::Vec3dArray;					
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size(); k++){												
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));							
					}					
				}
				else if(p_half_edgeVertexArray->size()==5)
				{//
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{						
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));
						p_new_half_edgeVertexArray->push_back((p_half_edgeVertexArray->at(k)+p_half_edgeVertexArray->at(k+1))*0.5);							
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==3)
				{// 有问题
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{
						vtStart = p_half_edgeVertexArray->at(k);
						vtEnd = p_half_edgeVertexArray->at(k+1);
						osg::Vec3d vtCenter = (vtStart+vtEnd)*0.5;
						p_new_half_edgeVertexArray->push_back(vtStart);
						p_new_half_edgeVertexArray->push_back((vtStart+vtCenter)*0.5);
						p_new_half_edgeVertexArray->push_back(vtCenter);
						p_new_half_edgeVertexArray->push_back((vtEnd+vtCenter)*0.5);
/*						for (long m = 0; m < 4 ; m++)
						{
						p_new_half_edgeVertexArray->push_back(vtStart+(vtEnd-vtStart)*0.25*m);
						}	*/		
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==2)
				{// 有问题
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					latitude0 = p_edgeVertexArray->at(index_0).x();
					latitude1 = p_edgeVertexArray->at(index_1).x();
					latitude_range = latitude1 - latitude0;

					vtStart = p_half_edgeVertexArray->at(0);

					line_dir = p_half_edgeVertexArray->at(1)-p_half_edgeVertexArray->at(0);
					for (k = 0; k < (glbInt32)mpr_p_west_northhalf_vertexarray->size(); k++)
					{
						vLen = (mpr_p_west_northhalf_vertexarray->at(k).x()-latitude0) / latitude_range;
						p_new_half_edgeVertexArray->push_back(vtStart+line_dir*vLen);
					}
				}
				else{//p_half_edgeVertexArray->size() == 9 5 3 2 以外的数
					int error = 1;
				}
			}
			else{//if (index_0>-1 && index_1>-1 && index_1>index_0)
				int error = 1;
			}				
		}
		break;
	case GLB_WESTEDGE_SOUTHSECTION:
		{
			glbDouble west_southhalf_center_latitude = mpr_south + (mpr_north - mpr_south) * 0.25;
			if (west_southhalf_center_latitude > neighbour_center_latitude){
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_EASTEDGE_NORTHSECTION);
			}
			else{
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_EASTEDGE_SOUTHSECTION);
			}	
			osg::Vec3d vtStart = mpr_p_west_southhalf_vertexarray->at(0);
			osg::Vec3d vtEnd = mpr_p_west_southhalf_vertexarray->at(mpr_p_west_southhalf_vertexarray->size()-1);
			//////////////////////////////////////////////////////////////////////////
			glbInt32 index_0 = -1;
			glbInt32 index_1 = -1;
			glbDouble latitude0,latitude1;
			glbDouble longitude = p_edgeVertexArray->at(0).y();

			for (k = 0; k < (glbInt32)p_edgeVertexArray->size()-1; k++)
			{
				latitude0 = p_edgeVertexArray->at(k).x();
				latitude1 = p_edgeVertexArray->at(k+1).x();
				if (latitude0+latitude_span  > vtStart.x()	&& (fabs(p_edgeVertexArray->at(k).y()-longitude) < FAZHI))
					index_0 = k;
				if  (latitude1-latitude_span < vtEnd.x()	&& (fabs(p_edgeVertexArray->at(k).y()-longitude) < FAZHI) && (index_1==-1))
					index_1 = k+1;
			}
			if (index_0>-1 && index_1>-1 && index_1>index_0)
			{
				glbBool bInitialized = p_neighbour_tile->IsInitialized();
				long cc = index_1-index_0;

				p_half_edgeVertexArray = new osg::Vec3dArray;
				for (k = index_0; k <= index_1; k++){
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
						g_ellipsmodel->convertLatLongHeightToXYZ(
							p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).z(),						
							point.x(), point.y(), point.z() );
					}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
						point = p_edgeVertexArray->at(k);
						point.set(p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).z());
					}
					p_half_edgeVertexArray->push_back(point);// - mpr_local_origin);
				}
				// 非initialized，奇数位置的坐标位置需要纠正
				if (bInitialized==false && (p_half_edgeVertexArray->size()%2==1)){
					for (k = 1; k < (glbInt32)p_half_edgeVertexArray->size(); k+=2){
						osg::Vec3d v = (p_half_edgeVertexArray->at(k-1)+p_half_edgeVertexArray->at(k+1))*0.5;
						p_half_edgeVertexArray->at(k).set(v);
					}
				}
				else if (bInitialized==false && p_half_edgeVertexArray->size()==2){
					osg::Vec3d v1,v2;
					if (index_0%2==1){		
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).z());
							v2.set(p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).z());
						}
						p_half_edgeVertexArray->at(0).set((v1+v2)*0.5);// - mpr_local_origin);
					}
					if (index_1%2==1){
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).z());
							v2.set(p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).z());
						}
						p_half_edgeVertexArray->at(1).set((v1+v2)*0.5);// - mpr_local_origin);
					}
				}
				else if (bInitialized==false){
					int error = 1;
				}
				//----------------------------------
				if (p_half_edgeVertexArray->size()==9){				
					p_new_half_edgeVertexArray = new osg::Vec3dArray;					
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size(); k++){												
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));							
					}					
				}
				else if(p_half_edgeVertexArray->size()==5)
				{//
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{						
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));
						p_new_half_edgeVertexArray->push_back((p_half_edgeVertexArray->at(k)+p_half_edgeVertexArray->at(k+1))*0.5);							
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==3)
				{// 有问题
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{
						vtStart = p_half_edgeVertexArray->at(k);
						vtEnd = p_half_edgeVertexArray->at(k+1);
						osg::Vec3d vtCenter = (vtStart+vtEnd)*0.5;
						p_new_half_edgeVertexArray->push_back(vtStart);
						p_new_half_edgeVertexArray->push_back((vtStart+vtCenter)*0.5);
						p_new_half_edgeVertexArray->push_back(vtCenter);
						p_new_half_edgeVertexArray->push_back((vtEnd+vtCenter)*0.5);
/*						for (long m = 0; m < 4 ; m++)
						{
							p_new_half_edgeVertexArray->push_back(vtStart+(vtEnd-vtStart)*0.25*m);
						}		*/	
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==2)
				{// 有问题
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					latitude0 = p_edgeVertexArray->at(index_0).x();
					latitude1 = p_edgeVertexArray->at(index_1).x();
					latitude_range = latitude1 - latitude0;

					vtStart = p_half_edgeVertexArray->at(0);

					line_dir = p_half_edgeVertexArray->at(1)-p_half_edgeVertexArray->at(0);
					for (k = 0; k < (glbInt32)mpr_p_west_southhalf_vertexarray->size(); k++)
					{
						vLen = (mpr_p_west_southhalf_vertexarray->at(k).x()-latitude0) / latitude_range;
						p_new_half_edgeVertexArray->push_back(vtStart+line_dir*vLen);
					}
				}
				else{//p_half_edgeVertexArray->size() == 9 5 3 2 以外的数
					int error = 1;
				}
			}
			else{//if (index_0>-1 && index_1>-1 && index_1>index_0)
				int error = 1;
			}			
		}
		break;
	case GLB_EASTEDGE_NORTHSECTION:
		{		
			glbDouble east_northhalf_center_latitude = mpr_north - (mpr_north - mpr_south) * 0.25;
			if (east_northhalf_center_latitude > neighbour_center_latitude){
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_WESTEDGE_NORTHSECTION);
			}
			else{
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_WESTEDGE_SOUTHSECTION);
			}	
			osg::Vec3d vtStart = mpr_p_east_northhalf_vertexarray->at(0);
			osg::Vec3d vtEnd = mpr_p_east_northhalf_vertexarray->at(mpr_p_east_northhalf_vertexarray->size()-1);
			//////////////////////////////////////////////////////////////////////////
			glbInt32 index_0 = -1;
			glbInt32 index_1 = -1;
			glbDouble latitude0,latitude1;
			glbDouble longitude = p_edgeVertexArray->at(0).y();

			for (k = 0; k < (glbInt32)p_edgeVertexArray->size()-1; k++)
			{
				latitude0 = p_edgeVertexArray->at(k).x();
				latitude1 = p_edgeVertexArray->at(k+1).x();
				if (latitude0+latitude_span  > vtStart.x()	&& (fabs(p_edgeVertexArray->at(k).y()-longitude) < FAZHI))
					index_0 = k;
				if  (latitude1-latitude_span < vtEnd.x()	&& (fabs(p_edgeVertexArray->at(k).y()-longitude) < FAZHI) && (index_1==-1))
					index_1 = k+1;
			}
			if (index_0>-1 && index_1>-1 && index_1>index_0)
			{
				glbBool bInitialized = p_neighbour_tile->IsInitialized();
				glbInt32 cc = index_1-index_0;

				p_half_edgeVertexArray = new osg::Vec3dArray;
				for (k = index_0; k <= index_1; k++){
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
						g_ellipsmodel->convertLatLongHeightToXYZ(
							p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).z(),						
							point.x(), point.y(), point.z() );
					}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){						
						point.set(p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).z());
					}
					p_half_edgeVertexArray->push_back(point);// - mpr_local_origin);
				}
				// 非initialized，奇数位置的坐标位置需要纠正
				if (bInitialized==false && (p_half_edgeVertexArray->size()%2==1)){
					for (k = 1; k < (glbInt32)p_half_edgeVertexArray->size(); k+=2){
						osg::Vec3d v = (p_half_edgeVertexArray->at(k-1)+p_half_edgeVertexArray->at(k+1))*0.5;
						p_half_edgeVertexArray->at(k).set(v);
					}
				}
				else if (bInitialized==false && p_half_edgeVertexArray->size()==2){
					osg::Vec3d v1,v2;
					if (index_0%2==1){		
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).z());
							v2.set(p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).z());
						}
						p_half_edgeVertexArray->at(0).set((v1+v2)*0.5);// - mpr_local_origin);
					}
					if (index_1%2==1){
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).z());
							v2.set(p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).z());
						}
						p_half_edgeVertexArray->at(1).set((v1+v2)*0.5);// - mpr_local_origin);
					}
				}
				else if (bInitialized==false){
					int error = 1;
				}
				//----------------------------------
				if (p_half_edgeVertexArray->size()==9){				
					p_new_half_edgeVertexArray = new osg::Vec3dArray;					
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size(); k++){												
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));							
					}		
					if (mpr_level!=p_neighbour_tile->GetLevel()){
						int levelerror = 1;
					}
				}
				else if(p_half_edgeVertexArray->size()==5)
				{//
					if (mpr_level!=p_neighbour_tile->GetLevel()+1){
						int levelerror = 1;
					}
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{						
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));
						p_new_half_edgeVertexArray->push_back((p_half_edgeVertexArray->at(k)+p_half_edgeVertexArray->at(k+1))*0.5);							
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==3)
				{// 有问题
					if (mpr_level!=p_neighbour_tile->GetLevel()+2){
						int levelerror = 1;
					}
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{
						vtStart = p_half_edgeVertexArray->at(k);
						vtEnd = p_half_edgeVertexArray->at(k+1);
						osg::Vec3d vtCenter = (vtStart+vtEnd)*0.5;
						p_new_half_edgeVertexArray->push_back(vtStart);
						p_new_half_edgeVertexArray->push_back((vtStart+vtCenter)*0.5);
						p_new_half_edgeVertexArray->push_back(vtCenter);
						p_new_half_edgeVertexArray->push_back((vtEnd+vtCenter)*0.5);
/*						for (long m = 0; m < 4 ; m++)
						{
							p_new_half_edgeVertexArray->push_back(vtStart+(vtEnd-vtStart)*0.25*m);
						}	*/		
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==2)
				{// 有问题
					if (mpr_level<p_neighbour_tile->GetLevel()+3){
						int levelerror = 1;
					}
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					latitude0 = p_edgeVertexArray->at(index_0).x();
					latitude1 = p_edgeVertexArray->at(index_1).x();
					latitude_range = latitude1 - latitude0;

					vtStart = p_half_edgeVertexArray->at(0);

					line_dir = p_half_edgeVertexArray->at(1)-p_half_edgeVertexArray->at(0);
					for (k = 0; k < (glbInt32)mpr_p_east_northhalf_vertexarray->size(); k++)
					{
						vLen = (mpr_p_east_northhalf_vertexarray->at(k).x()-latitude0) / latitude_range;
						p_new_half_edgeVertexArray->push_back(vtStart+line_dir*vLen);
					}
				}
				else{//p_half_edgeVertexArray->size() == 9 5 3 2 以外的数
					int error = 1;
				}
			}
			else{//if (index_0>-1 && index_1>-1 && index_1>index_0)
				int error = 1;
			}		
		}
		break;
	case GLB_EASTEDGE_SOUTHSECTION:
		{
			glbDouble east_southhalf_center_latitude = mpr_south + (mpr_north - mpr_south) * 0.25;
			if (east_southhalf_center_latitude > neighbour_center_latitude){
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_WESTEDGE_NORTHSECTION);
			}
			else{
				p_edgeVertexArray = p_neighbour_tile->GetEdgeVertexArray(GLB_WESTEDGE_SOUTHSECTION);
			}	
			osg::Vec3 vtStart = mpr_p_east_southhalf_vertexarray->at(0);
			osg::Vec3 vtEnd = mpr_p_east_southhalf_vertexarray->at(mpr_p_east_southhalf_vertexarray->size()-1);
			//////////////////////////////////////////////////////////////////////////
			glbInt32 index_0 = -1;
			glbInt32 index_1 = -1;
			glbDouble latitude0,latitude1;
			glbDouble longitude = p_edgeVertexArray->at(0).y();

			for (k = 0; k < (glbInt32)p_edgeVertexArray->size()-1; k++)
			{
				latitude0 = p_edgeVertexArray->at(k).x();
				latitude1 = p_edgeVertexArray->at(k+1).x();
				if (latitude0+latitude_span  > vtStart.x()	&& (fabs(p_edgeVertexArray->at(k).y()-longitude) < FAZHI))
					index_0 = k;
				if  (latitude1-latitude_span < vtEnd.x() && (fabs(p_edgeVertexArray->at(k).y()-longitude) < FAZHI) && (index_1==-1))
					index_1 = k+1;
			}
			if (index_0>-1 && index_1>-1 && index_1>index_0)
			{
				glbBool bInitialized = p_neighbour_tile->IsInitialized();
				glbInt32 cc = index_1-index_0;

				p_half_edgeVertexArray = new osg::Vec3dArray;
				for (k = index_0; k <= index_1; k++){
					if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
						g_ellipsmodel->convertLatLongHeightToXYZ(
							p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).z(),						
							point.x(), point.y(), point.z() );
					}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
						point.set(p_edgeVertexArray->at(k).y(),p_edgeVertexArray->at(k).x(),p_edgeVertexArray->at(k).z());
					}
					p_half_edgeVertexArray->push_back(point);// - mpr_local_origin);
				}
				// 非initialized，奇数位置的坐标位置需要纠正
				if (bInitialized==false && (p_half_edgeVertexArray->size()%2==1)){
					for (k = 1; k < (long)p_half_edgeVertexArray->size(); k+=2){
						osg::Vec3d v = (p_half_edgeVertexArray->at(k-1)+p_half_edgeVertexArray->at(k+1))*0.5;
						p_half_edgeVertexArray->at(k).set(v);
					}
				}
				else if (bInitialized==false && p_half_edgeVertexArray->size()==2){
					osg::Vec3d v1,v2;
					if (index_0%2==1){			
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							v1.set(p_edgeVertexArray->at(index_0-1).y(),p_edgeVertexArray->at(index_0-1).x(),p_edgeVertexArray->at(index_0-1).z());
							v2.set(p_edgeVertexArray->at(index_0+1).y(),p_edgeVertexArray->at(index_0+1).x(),p_edgeVertexArray->at(index_0+1).z());
						}
						p_half_edgeVertexArray->at(0).set((v1+v2)*0.5);// - mpr_local_origin);
					}
					if (index_1%2==1){
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).z(),						
								v1.x(), v1.y(), v1.z() );
							g_ellipsmodel->convertLatLongHeightToXYZ(
								p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).z(),						
								v2.x(), v2.y(), v2.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							v1.set(p_edgeVertexArray->at(index_1-1).y(),p_edgeVertexArray->at(index_1-1).x(),p_edgeVertexArray->at(index_1-1).z());
							v2.set(p_edgeVertexArray->at(index_1+1).y(),p_edgeVertexArray->at(index_1+1).x(),p_edgeVertexArray->at(index_1+1).z());
						}
						p_half_edgeVertexArray->at(1).set((v1+v2)*0.5);// - mpr_local_origin);
					}
				}
				else if (bInitialized==false){
					int error = 1;
				}
				//----------------------------------
				if (p_half_edgeVertexArray->size()==9){		
					if (mpr_level!=p_neighbour_tile->GetLevel()){
						int levelerror = 1;
					}
					p_new_half_edgeVertexArray = new osg::Vec3dArray;					
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size(); k++){												
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));							
					}					
				}
				else if(p_half_edgeVertexArray->size()==5)
				{//
					if (mpr_level!=p_neighbour_tile->GetLevel()+1){
						int levelerror = 1;
					}
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{						
						p_new_half_edgeVertexArray->push_back(p_half_edgeVertexArray->at(k));
						p_new_half_edgeVertexArray->push_back((p_half_edgeVertexArray->at(k)+p_half_edgeVertexArray->at(k+1))*0.5);							
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==3)
				{// 有问题
					if (mpr_level!=p_neighbour_tile->GetLevel()+2){
						int levelerror = 1;
					}
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					for (k = 0; k < (glbInt32)p_half_edgeVertexArray->size()-1; k++)
					{
						vtStart = p_half_edgeVertexArray->at(k);
						vtEnd = p_half_edgeVertexArray->at(k+1);
						osg::Vec3d vtCenter = (vtStart+vtEnd)*0.5;
						p_new_half_edgeVertexArray->push_back(vtStart);
						p_new_half_edgeVertexArray->push_back((vtStart+vtCenter)*0.5);
						p_new_half_edgeVertexArray->push_back(vtCenter);
						p_new_half_edgeVertexArray->push_back((vtEnd+vtCenter)*0.5);
/*						for (long m = 0; m < 4 ; m++)
						{
							p_new_half_edgeVertexArray->push_back(vtStart+(vtEnd-vtStart)*0.25*m);
						}	*/		
					}
					p_new_half_edgeVertexArray->push_back( p_half_edgeVertexArray->at(p_half_edgeVertexArray->size()-1));
				}
				else if (p_half_edgeVertexArray->size()==2)
				{// 有问题
					if (mpr_level<p_neighbour_tile->GetLevel()+3){
						int levelerror = 1;
					}
					p_new_half_edgeVertexArray = new osg::Vec3dArray;
					latitude0 = p_edgeVertexArray->at(index_0).x();
					latitude1 = p_edgeVertexArray->at(index_1).x();
					latitude_range = latitude1 - latitude0;

					vtStart = p_half_edgeVertexArray->at(0);

					line_dir = p_half_edgeVertexArray->at(1)-p_half_edgeVertexArray->at(0);
					for (k = 0; k < (glbInt32)mpr_p_east_southhalf_vertexarray->size(); k++)
					{
						vLen = (mpr_p_east_southhalf_vertexarray->at(k).x()-latitude0) / latitude_range;
						p_new_half_edgeVertexArray->push_back(vtStart+line_dir*vLen);
					}
				}
				else{//p_half_edgeVertexArray->size() == 9 5 3 2 以外的数
					int error = 1;
				}
			}
			else{//if (index_0>-1 && index_1>-1 && index_1>index_0)
				int error = 1;
			}	
		}
		break;
	}
	return p_new_half_edgeVertexArray;
}

BOOL CGlbGlobeTerrainTile::IsEdgeNeedMerge(CGlbGlobeTerrainTile* p_base_tile, 
	CGlbGlobeTerrainTile* p_neighbout_tile, GlbTileEdgeEnum base_edge, osg::Vec3dArray* p_VertexArray)
{
	// 测试先考虑同级之间数据不一致行的问题。先不接边
	return FALSE;

	BOOL bNeedMerge = FALSE;
	long k=0;
	switch(base_edge)
	{
	case GLB_NORTHEDGE_WESTSECTION:
		{
			osg::Vec3dArray* p_base_west_vertex_array = p_base_tile->GetEdgeVertexArray(GLB_NORTHEDGE_WESTSECTION);
			osg::Vec3dArray* p_neighbour_west_vertex_array = p_neighbout_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_WESTSECTION);
			for (k = 0; k < (long)p_base_west_vertex_array->size(); k++){				
				if ( fabs(p_base_west_vertex_array->at(k).z() - p_neighbour_west_vertex_array->at(k).z()) > FAZHI)
				{
					bNeedMerge=TRUE;		
					p_VertexArray = p_neighbour_west_vertex_array;
					return bNeedMerge;				
				}
			}				
		}
		break;
	case GLB_NORTHEDGE_EASTSECTION:
		{
			osg::Vec3dArray* p_base_east_vertex_array = p_base_tile->GetEdgeVertexArray(GLB_NORTHEDGE_EASTSECTION);
			osg::Vec3dArray* p_neighbour_east_vertex_array = p_neighbout_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_EASTSECTION);
			for (k = 0; k < (long)p_base_east_vertex_array->size(); k++){				
				if ( fabs(p_base_east_vertex_array->at(k).z() - p_neighbour_east_vertex_array->at(k).z()) > FAZHI)
				{
					bNeedMerge=TRUE;	
					p_VertexArray = p_neighbour_east_vertex_array;
					return bNeedMerge;	
				}
			}		
		}
		break;
	case GLB_SOUTHEDGE_WESTSECTION:
		{
			osg::Vec3dArray* p_base_west_vertex_array = p_base_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_WESTSECTION);
			osg::Vec3dArray* p_neighbour_west_vertex_array = p_neighbout_tile->GetEdgeVertexArray(GLB_NORTHEDGE_WESTSECTION);
			for (k = 0; k < (long)p_base_west_vertex_array->size(); k++){	
				if ( fabs(p_base_west_vertex_array->at(k).z() - p_neighbour_west_vertex_array->at(k).z()) > FAZHI)
				{
					bNeedMerge=TRUE;
					p_VertexArray = p_neighbour_west_vertex_array;
					return bNeedMerge;
				}			
			}		
		}
		break;
	case GLB_SOUTHEDGE_EASTSECTION:
		{
			osg::Vec3dArray* p_base_east_vertex_array = p_base_tile->GetEdgeVertexArray(GLB_SOUTHEDGE_EASTSECTION);
			osg::Vec3dArray* p_neighbour_east_vertex_array = p_neighbout_tile->GetEdgeVertexArray(GLB_NORTHEDGE_EASTSECTION);
			for (k = 0; k < (long)p_base_east_vertex_array->size(); k++){				
				if ( fabs(p_base_east_vertex_array->at(k).z() - p_neighbour_east_vertex_array->at(k).z()) > FAZHI)
				{
					bNeedMerge=TRUE;
					p_VertexArray = p_neighbour_east_vertex_array;
					return bNeedMerge;
				}								
			}		
		}
		break;
	case GLB_WESTEDGE_NORTHSECTION:
		{
			osg::Vec3dArray* p_base_north_vertex_array = p_base_tile->GetEdgeVertexArray(GLB_WESTEDGE_NORTHSECTION);
			osg::Vec3dArray* p_neighbour_north_vertex_array = p_neighbout_tile->GetEdgeVertexArray(GLB_EASTEDGE_NORTHSECTION);			
			for (k = 0; k < (long)p_base_north_vertex_array->size(); k++){
				if (!bNeedMerge)
				{
					if ( fabs(p_base_north_vertex_array->at(k).z() - p_neighbour_north_vertex_array->at(k).z()) > FAZHI)
					{
						bNeedMerge=TRUE;
						p_VertexArray = p_neighbour_north_vertex_array;
						return bNeedMerge;
					}
				}				
			}		
		}
		break;
	case GLB_WESTEDGE_SOUTHSECTION:
		{			
			osg::Vec3dArray* p_base_south_vertex_array = p_base_tile->GetEdgeVertexArray(GLB_WESTEDGE_SOUTHSECTION);
			osg::Vec3dArray* p_neighbour_south_vertex_array = p_neighbout_tile->GetEdgeVertexArray(GLB_EASTEDGE_SOUTHSECTION);
			for (k = 0; k < (long)p_base_south_vertex_array->size(); k++){
				if ( fabs(p_base_south_vertex_array->at(k).z() - p_neighbour_south_vertex_array->at(k).z()) > FAZHI)
				{
					bNeedMerge=TRUE;
					p_VertexArray = p_neighbour_south_vertex_array;
					return bNeedMerge;
				}					
			}		
		}
		break;
	case GLB_EASTEDGE_NORTHSECTION:
		{
			osg::Vec3dArray* p_base_north_vertex_array = p_base_tile->GetEdgeVertexArray(GLB_EASTEDGE_NORTHSECTION);
			osg::Vec3dArray* p_neighbour_north_vertex_array = p_neighbout_tile->GetEdgeVertexArray(GLB_WESTEDGE_NORTHSECTION);			
			for (k = 0; k < (long)p_base_north_vertex_array->size(); k++){				
				if ( fabs(p_base_north_vertex_array->at(k).z() - p_neighbour_north_vertex_array->at(k).z()) > FAZHI)
				{
					bNeedMerge=TRUE;		
					p_VertexArray = p_neighbour_north_vertex_array;
					return bNeedMerge;
				}
			}		
		}
		break;
	case GLB_EASTEDGE_SOUTHSECTION:
		{			
			osg::Vec3dArray* p_base_south_vertex_array = p_base_tile->GetEdgeVertexArray(GLB_EASTEDGE_SOUTHSECTION);
			osg::Vec3dArray* p_neighbour_south_vertex_array = p_neighbout_tile->GetEdgeVertexArray(GLB_WESTEDGE_SOUTHSECTION);
			for (k = 0; k < (long)p_base_south_vertex_array->size(); k++){				
				if ( fabs(p_base_south_vertex_array->at(k).z() - p_neighbour_south_vertex_array->at(k).z()) > FAZHI)
				{
					bNeedMerge=TRUE;
					p_VertexArray = p_neighbour_south_vertex_array;
					return bNeedMerge;
				}			
			}		
		}
		break;
	}
	return bNeedMerge;
}

CGlbGlobeTerrainTile* CGlbGlobeTerrainTile::GetTopMostTile(CGlbGlobeTerrainTile* p_tile)
{
	if (p_tile==NULL)
		return NULL;
	if (p_tile->GetParentTile()){
		return GetTopMostTile(p_tile->GetParentTile());
	}		
	return p_tile;
}

GlbGlobeChildLocationEnum CGlbGlobeTerrainTile::GetLocationFromLongitudeLatitude(glbDouble longOrX,glbDouble latOrY)
{
	GlbGlobeChildLocationEnum location = GLB_UNKONWN;
	glbDouble center_latitude = (mpr_south + mpr_north) * 0.5;
	glbDouble center_longitude = (mpr_west + mpr_east) * 0.5;
	if ( latOrY < center_latitude && longOrX <= center_longitude )
		location = GlB_SOUTHWEST;
	if ( latOrY < center_latitude && longOrX > center_longitude)
		location = GLB_SOUTHEAST;
	if (latOrY >= center_latitude && longOrX <= center_longitude)
		location = GLB_NORTHWEST;
	if (latOrY >= center_latitude && longOrX > center_longitude)
		location = GLB_NORTHEAST;
	return location;
}

glbBool CGlbGlobeTerrainTile::IsDomlayerInterTile(CGlbGlobeDomLayer* pDomlayer)
{
	glbBool bInter = false;
	glbBool bHasPyramid = pDomlayer->HasPyramid();	
	if (bHasPyramid)
	{// 有金子塔,判断是否跟tile有交集
		glbInt32 minLevel,maxLevel;
		CGlbExtent ext;
		IGlbRasterDataset* rd = dynamic_cast<IGlbRasterDataset*>(pDomlayer->GetDataset());
		if (rd)
		{
			rd->GetPyramidLevels(&minLevel,&maxLevel);
			glbDouble minX,maxX,minY,maxY;
			rd->GetLevelExtent(min(mpr_level,maxLevel),&ext);
			ext.Get(&minX,&maxX,&minY,&maxY);

			minX = max(mpr_west,minX);
			minY = max(mpr_south,minY);
			maxX = min(mpr_east,maxX);
			maxY = min(mpr_north,maxY);			
			if (maxX>minX && maxY>minY)
				bInter = true;			
		}
	}
	else
	{
		//判断domlayer和tile是否有交集
		glbref_ptr<CGlbExtent> pDomlayerBound = NULL;
		if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
			pDomlayerBound = pDomlayer->GetBound(false);
		else
			pDomlayerBound = pDomlayer->GetBound(true);
		glbDouble minX,maxX,minY,maxY;
		if (pDomlayerBound)
		{
			pDomlayerBound->Get(&minX,&maxX,&minY,&maxY);

			minX = max(mpr_west,minX);
			minY = max(mpr_south,minY);
			maxX = min(mpr_east,maxX);
			maxY = min(mpr_north,maxY);

			if (maxX>minX && maxY>minY)
				bInter = true;			
		}
	}		

	return bInter;
}

glbBool CGlbGlobeTerrainTile::IsDemlayerInterTile(CGlbGlobeDemLayer* pDemlayer)
{
	glbBool bInter = false;
	glbBool bHasPyramid = pDemlayer->HasPyramid();	
	if (bHasPyramid)
	{// 有金子塔,判断是否跟tile有交集
		glbInt32 minLevel,maxLevel;
		CGlbExtent ext;
		IGlbRasterDataset* rd = dynamic_cast<IGlbRasterDataset*>(pDemlayer->GetDataset());
		if (rd)
		{
			rd->GetPyramidLevels(&minLevel,&maxLevel);
			glbDouble minX,maxX,minY,maxY;
			rd->GetLevelExtent(min(mpr_level,maxLevel),&ext);
			ext.Get(&minX,&maxX,&minY,&maxY);

			minX = max(mpr_west,minX);
			minY = max(mpr_south,minY);
			maxX = min(mpr_east,maxX);
			maxY = min(mpr_north,maxY);			
			if (maxX>minX && maxY>minY)
				bInter = true;			
		}
	}
	else
	{
		//判断demlayer和tile是否有交集
		glbref_ptr<CGlbExtent> pDemlayerBound = NULL;
		if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
			pDemlayerBound = pDemlayer->GetBound(false);
		else
			pDemlayerBound = pDemlayer->GetBound(true);
		glbDouble minX,maxX,minY,maxY;
		if (pDemlayerBound)
		{
			pDemlayerBound->Get(&minX,&maxX,&minY,&maxY);

			minX = max(mpr_west,minX);
			minY = max(mpr_south,minY);
			maxX = min(mpr_east,maxX);
			maxY = min(mpr_north,maxY);

			if (maxX>minX && maxY>minY)
				bInter = true;			
		}
	}		

	return bInter;
}

glbBool CGlbGlobeTerrainTile::InterpolateDomLayerData(CGlbGlobeDomLayer* pLayer,glbInt32 maxLevel,CGlbPixelBlock* p_pixelblock)
{
	CGlbGlobeTerrainTile* parent_tile = this;
	while(parent_tile->GetLevel() != maxLevel)
	{
		parent_tile = parent_tile->GetParentTile();		
	}

	if (parent_tile->GetLevel() != maxLevel)
		return false;
	
	glbDouble _west,_east,_south,_north,_minAlt,_maxAlt;
	parent_tile->GetRange(_east,_west,_south,_north,_minAlt,_maxAlt);

	glbInt32 _row,_column;
	parent_tile->GetPosition(_row,_column);
	CGlbPixelBlock* pixelblock=NULL;
	pLayer->CreatePixelBlock(&pixelblock);
	if (!pixelblock)
		return false;

	glbBool bHasPyramid = pLayer->HasPyramid();
	glbBool bsucc=false;
	if (bHasPyramid)
	{// 有金子塔
		// 加载跟当前地形块tile等级相同的，行列号则通过范围来确定
		bsucc = pLayer->ReadData(maxLevel,_row,_column,pixelblock);				
	}	

	glbByte* pixeldata=NULL;
	pixelblock->GetPixelData(&pixeldata);
	glbByte* ppData=NULL;
	p_pixelblock->GetPixelData(&ppData);

	if (!pixeldata|| !ppData)
	{//pixelblock 必须已经创建
		delete pixelblock;
		return false;
	}	
	glbInt32 blockColumns,blockRows;
	pixelblock->GetSize(blockColumns,blockRows);

	// 计算当前tile区域应该对应的max_level级的tile的纹理数据[256*256]的起始终止行列数
	glbInt32 startRow,endRow,startColumn,endColumn;
	startColumn = (mpr_west-_west)*blockColumns/(_east-_west)+0.5;
	endColumn = (mpr_east-_west)*blockColumns/(_east-_west)+0.1;
	startRow = (_north-mpr_north)*blockRows/(_north-_south)+0.5;
	endRow = (_north-mpr_south)*blockRows/(_north-_south)+0.1;
	if (endColumn<=startColumn)
		endColumn = startColumn+1;
	if (endRow<=startRow)
		endRow = startRow+1;

	// end 计算
	glbInt32 cc = blockColumns/abs(endColumn-startColumn);
	glbInt32 rr = blockRows/abs(endRow-startRow);
 
	glbInt32 bandCount = p_pixelblock->GetBandCount();
	//glbByte* pVal = new glbByte[bandCount];
	//void* _val=NULL;
	glbByte pVal[4];
	//循环计算每个像素点的RGBA值
	for(glbInt32 i = startRow; i < endRow; i++)
		for(glbInt32 j = startColumn; j < endColumn; j++)
	{
		for(glbInt32 k = 0; k < bandCount; k++)
		{
			//pixelblock->GetPixelVal(k,i,j,&_val);		
			//pVal[k] = *((BYTE*)_val);

			pVal[k] = pixeldata[(i*blockRows+j)*bandCount+k];
		}

		glbInt32 localRow = (i-startRow)*rr;
		glbInt32 localColumn = (j-startColumn)*cc;		
		for(glbInt32 m = 0; m < rr; m++)
			for(glbInt32 n = 0; n < cc; n++)
			{
				for(glbInt32 l = 0; l < bandCount; l++)
				{
					//void* val = (void*)(pVal+l);
					//p_pixelblock->SetPixelVal(l,localRow+m,localColumn+n,val);
					////glbByte refer = pVal[l];				
					////p_pixelblock->GetPixelVal(l,localRow+m,localColumn+n,&_val);
					////refer = *((BYTE*)_val);
					///*int kk = 0;*/

					ppData[((localRow+m)*blockRows+localColumn+n)*bandCount+l] = pVal[l];
				}
			}		
	}
	//delete pVal;

	if (pixelblock)
		delete pixelblock;

	return true;
}

glbBool CGlbGlobeTerrainTile::InterpolateDemDataFromDemLayer(glbFloat* demData, glbByte* dataMask,glbInt32 row, glbInt32 column, glbDouble &srcWest,glbDouble &srcEast, glbDouble &srcSouth,glbDouble &srcNorth)
{
	// 1. 根据tile的range获取在(sr_west,src_east,src_south,src_north)中的哪个方位 - 西北 ，西南， 东北， 东南
	GlbGlobeChildLocationEnum location = GLB_UNKONWN;
	glbDouble srcCenterX = (srcWest+srcEast)*0.5;
	glbDouble srcCenterY = (srcNorth+srcSouth)*0.5;
	glbDouble center_longitude = (mpr_west+mpr_east)*0.5;
	glbDouble center_latitude = (mpr_south+mpr_north)*0.5;
	if ( center_longitude < srcCenterX && center_latitude < srcCenterY )
	{
		location = GlB_SOUTHWEST;
		srcEast = srcCenterX;
		srcNorth = srcCenterY;		
	}
	else if ( center_longitude < srcCenterX && center_latitude > srcCenterY )
	{
		location = GLB_NORTHWEST;
		srcEast = srcCenterX;
		srcSouth = srcCenterY;
	}
	else if ( center_longitude > srcCenterX && center_latitude > srcCenterY )
	{
		location = GLB_NORTHEAST;
		srcWest = srcCenterX;
		srcSouth = srcCenterY;
	}
	else if ( center_longitude > srcCenterX && center_latitude < srcCenterY )
	{
		location = GLB_SOUTHEAST;
		srcWest = srcCenterX;
		srcNorth = srcCenterY;
	}

	if (location==GLB_UNKONWN)
	{
		return false;
	}
	// 2. 根据方位，取出demData和dataMask中的部分数据 ，加密一倍放入新的newDemData和newDataMask中
	glbFloat* p_data = demData;
	glbInt32 dem_columns = column;
	glbInt32 dem_rows = row;
	glbFloat* p_new_data = NULL;
	glbByte* newDataMask = NULL;
	
	p_new_data = new glbFloat[row*column];
	newDataMask = new glbByte[row*column];	

	memset(p_new_data,0,sizeof(glbFloat)*row*column);
	memset(newDataMask,0,sizeof(glbByte)*row*column);

	glbInt32 dem_half_rows = dem_rows / 2 + (dem_rows % 2);		//9
	glbInt32 dem_half_columns = dem_columns / 2 + (dem_columns % 2); // 9

	// step 1 : 写真实数据
	for (glbInt32 i = 0 ; i < dem_half_rows; i++)
		for(glbInt32 j = 0; j < dem_half_columns; j++)
		{			
			switch(location)
			{
			case GLB_NORTHWEST:	//西北
				{
					p_new_data[2*i*dem_columns+j*2] = p_data[i*dem_columns+j];		
					newDataMask[2*i*dem_columns+j*2] = dataMask[i*dem_columns+j];	
					if (j < dem_half_columns-1){
						p_new_data[2*i*dem_columns+j*2+1] = (p_data[i*dem_columns+j] + p_data[i*dem_columns+j+1]) *0.5;	
						newDataMask[2*i*dem_columns+j*2+1] = max(dataMask[i*dem_columns+j] ,dataMask[i*dem_columns+j+1]);
					}				
				}
				break;
			case GLB_NORTHEAST:	//东北			
				{
					p_new_data[2*i*dem_columns+j*2] = p_data[i*dem_columns+j+dem_half_columns-1];	
					newDataMask[2*i*dem_columns+j*2] = dataMask[i*dem_columns+j+dem_half_columns-1];	
					if (j < dem_half_columns-1){
						p_new_data[2*i*dem_columns+j*2+1] = (p_data[i*dem_columns+j+dem_half_columns-1] + p_data[i*dem_columns+j+dem_half_columns]) *0.5;	
						newDataMask[2*i*dem_columns+j*2+1] = max(dataMask[i*dem_columns+j+dem_half_columns-1],dataMask[i*dem_columns+j+dem_half_columns]);
					}
				}							
				break;
			case GlB_SOUTHWEST: //西南
				{
					p_new_data[2*i*dem_columns+j*2] = p_data[(i+dem_half_rows-1)*dem_columns+j];
					newDataMask[2*i*dem_columns+j*2] = dataMask[(i+dem_half_rows-1)*dem_columns+j];
					if (j < dem_half_columns-1){
						p_new_data[2*i*dem_columns+j*2+1] = (p_data[(i+dem_half_rows-1)*dem_columns+j] + p_data[(i+dem_half_rows-1)*dem_columns+j+1]) *0.5;	
						newDataMask[2*i*dem_columns+j*2+1] = max(dataMask[(i+dem_half_rows-1)*dem_columns+j],dataMask[(i+dem_half_rows-1)*dem_columns+j+1]);
					}
				}								
				break;
			case GLB_SOUTHEAST: //东南
				{
					p_new_data[2*i*dem_columns+j*2] = p_data[(i+dem_half_rows-1)*dem_columns+j+dem_half_columns-1];
					newDataMask[2*i*dem_columns+j*2] = dataMask[(i+dem_half_rows-1)*dem_columns+j+dem_half_columns-1];
					if (j < dem_half_columns-1){
						p_new_data[2*i*dem_columns+j*2+1] = (p_data[(i+dem_half_rows-1)*dem_columns+j+dem_half_columns-1] + p_data[(i+dem_half_rows-1)*dem_columns+j+dem_half_columns]) *0.5;
						newDataMask[2*i*dem_columns+j*2+1] = max(dataMask[(i+dem_half_rows-1)*dem_columns+j+dem_half_columns-1],dataMask[(i+dem_half_rows-1)*dem_columns+j+dem_half_columns]);
					}
				}				
				break;
			}
		}
		// step 2 ： 插值中间数据	
		for (glbInt32 i = 1 ; i < dem_rows; i+=2)
			for(glbInt32 j = 0; j < dem_columns; j++)
			{			
				p_new_data[i*dem_columns+j] = (p_new_data[(i-1)*dem_columns+j] + p_new_data[(i+1)*dem_columns+j]) * 0.5;
				newDataMask[i*dem_columns+j] = max(newDataMask[(i-1)*dem_columns+j],newDataMask[(i+1)*dem_columns+j]);
			}	

	// 3. 拷贝newDemData和newDataMask的内容到demData和dataMask中，删除newDemData和newDataMask
	 if (p_new_data)
	 {
		 memcpy(demData,p_new_data,row*column*sizeof(glbFloat));
		 delete []p_new_data;
	 }
	 if (newDataMask)
	 {		
		 memcpy(dataMask,newDataMask,row*column*sizeof(glbByte));
		 delete []newDataMask;
	 }
	return true;
}

glbBool CGlbGlobeTerrainTile::InterpolateDemLayerData(CGlbGlobeDemLayer* pLayer,glbInt32 maxLevel,CGlbPixelBlock* p_pixelblock)
{
	CGlbGlobeTerrainTile* parent_tile = this;
	while(parent_tile->GetLevel() != maxLevel)
	{
		parent_tile = parent_tile->GetParentTile();		
	}

	if (parent_tile->GetLevel() != maxLevel)
		return false;

	glbDouble _west,_east,_south,_north,_minAlt,_maxAlt;
	parent_tile->GetRange(_east,_west,_south,_north,_minAlt,_maxAlt);	

	glbInt32 _row,_column;
	parent_tile->GetPosition(_row,_column);
	CGlbPixelBlock* pixelblock=NULL;
	pLayer->CreatePixelBlock(&pixelblock);

	glbBool bHasPyramid = pLayer->HasPyramid();
	glbBool bsucc=false;
	if (bHasPyramid)
	{// 有金子塔
		// 加载跟当前地形块tile等级相同的，行列号则通过范围来确定
		bsucc = pLayer->ReadData(maxLevel,_row,_column,pixelblock);				
	}	

	GlbPixelTypeEnum pixeltype = p_pixelblock->GetPixelType();
	if (pixeltype!=GLB_PIXELTYPE_FLOAT32)
	{
		return false;
	}

	glbInt32 blockColumns,blockRows;
	pixelblock->GetSize(blockColumns,blockRows);
	glbByte* maskData= NULL;
	pixelblock->GetMaskData(&maskData);

	// 新算法，根据本tile在parent_tile中的方位，不断迭代插值图层的pixelblock直到本块数据位置
	glbFloat* pSrcDemdata=NULL;
	pixelblock->GetPixelData((glbByte**)(&pSrcDemdata)); 
	glbFloat* newDemdata=new glbFloat[mpr_demsize*mpr_demsize];
	memcpy(newDemdata,pSrcDemdata,mpr_demsize*mpr_demsize*sizeof(glbFloat));
	glbByte* newDataMask = new glbByte[mpr_demsize*mpr_demsize];
	memcpy(newDataMask,maskData,mpr_demsize*mpr_demsize*sizeof(glbByte));
	glbInt32 level = maxLevel;
	while(level!=mpr_level)
	{
		glbBool res = InterpolateDemDataFromDemLayer(newDemdata,newDataMask,blockRows,blockColumns,_west,_east,_south,_north);
		level++;
	}

	// 将结果填入p_pixelblock中
	glbFloat* pDesDemdata=NULL;
	p_pixelblock->GetPixelData((glbByte**)(&pDesDemdata)); 
	memcpy(pDesDemdata,newDemdata,mpr_demsize*mpr_demsize*sizeof(glbFloat));
	glbByte* pDesDataMask=NULL;
	p_pixelblock->GetMaskData(&pDesDataMask);
	memcpy(pDesDataMask,newDataMask,mpr_demsize*mpr_demsize*sizeof(glbByte));

	// 释放内存
	if (newDemdata)
		delete [] newDemdata;
	if (newDataMask)
		delete [] newDataMask;
	// 结束新算法	

	//// 计算当前tile区域应该对应的max_level级的tile的纹理数据[256*256]的起始终止行列数
	//glbInt32 startRow,endRow,startColumn,endColumn;
	//startColumn = (mpr_west-_west)*(blockColumns-blockColumns%2)/(_east-_west);
	//endColumn = (mpr_east-_west)*(blockColumns-blockColumns%2)/(_east-_west);
	//startRow = (_north-mpr_north)*(blockRows-blockRows%2)/(_north-_south);
	//endRow = (_north-mpr_south)*(blockRows-blockRows%2)/(_north-_south);
	//if (endColumn<=startColumn)
	//	endColumn = startColumn+1;
	//if (endRow<=startRow)
	//	endRow = startRow+1;

	//// end 计算
	//glbInt32 cc = (blockColumns-blockColumns%2)/abs(endColumn-startColumn);
	//glbInt32 rr = (blockRows-blockRows%2)/abs(endRow-startRow);	

	//glbByte* pmask = NULL;
	//p_pixelblock->GetMaskData(&pmask);
	//memset(pmask,255,blockRows*blockColumns);

	//glbInt32 bandCount = p_pixelblock->GetBandCounts();
	//void* _val=NULL;
	//glbFloat pVal;
	////循环计算每个顶点的高程值 - 有问题
	//for(glbInt32 i = startRow; i <= endRow; i++)
	//	for(glbInt32 j = startColumn; j <= endColumn; j++)
	//	{			
	//		pixelblock->GetPixelVal(0,j,i,&_val);
	//		glbByte* maskval=NULL;
	//		pixelblock->GetPixelValMask(j,i,&maskval);
	//		pVal = *((glbFloat*)_val);	
	//		glbInt32 localRow = (i-startRow)*rr;
	//		glbInt32 localColumn = (j-startColumn)*cc;		
	//		for(glbInt32 m = 0; m < rr; m++)
	//			for(glbInt32 n = 0; n < cc; n++)
	//			{
	//				glbInt32 x = localColumn+n;
	//				glbInt32 y = localRow+m;
	//				if (x>blockColumns-1)
	//					x =blockColumns-1;
	//				if (y> blockRows-1)
	//					y = blockRows-1;
	//				void* val = (void*)(&pVal);
	//				p_pixelblock->SetPixelVal(0,x,y,val);	
	//				pmask[y*blockColumns+x] = *maskval;
	//			}		
	//	}
	//delete pVal;
	if (pixelblock)
		delete pixelblock;

	return true;
}

void CGlbGlobeTerrainTile::ComputeBoundBox()
{
	mpr_boundbox.init();

	// tile的外围网格点，用于判断是否在视域范围内。可见性判断	
	std::vector<osg::Vec3d> pointvec;
	glbDouble averageAlt = (mpr_minalt+mpr_maxalt)*0.5;
	pointvec.push_back(osg::Vec3d(mpr_west,mpr_north,mpr_minalt));
	pointvec.push_back(osg::Vec3d(mpr_west,mpr_south,mpr_minalt));
	pointvec.push_back(osg::Vec3d(mpr_east,mpr_north,mpr_minalt));
	pointvec.push_back(osg::Vec3d(mpr_east,mpr_south,mpr_minalt));
	pointvec.push_back(osg::Vec3d(mpr_west,mpr_north,mpr_maxalt));
	pointvec.push_back(osg::Vec3d(mpr_west,mpr_south,mpr_maxalt));
	pointvec.push_back(osg::Vec3d(mpr_east,mpr_north,mpr_maxalt));
	pointvec.push_back(osg::Vec3d(mpr_east,mpr_south,mpr_maxalt));
	size_t i = 0;
	osg::Vec3d point;
	for(i =0;i<pointvec.size();i++)
	{		
		if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
		{
			g_ellipsmodel->convertLatLongHeightToXYZ(
				osg::DegreesToRadians( pointvec[i].y() ),
				osg::DegreesToRadians( pointvec[i].x() ),
				pointvec[i].z(),
				point.x(), point.y(), point.z() );
			mpr_boundbox.expandBy(point);

			//mpr_surrond_pointvec.push_back(osg::Vec3(point.x(),point.y(),point.z()));
		}
		else if (mpr_globe_type==GLB_GLOBETYPE_FLAT)
		{
			mpr_boundbox.expandBy(pointvec[i]);
		}
	}  
}

glbBool CGlbGlobeTerrainTile::UpdateEdgeGeometryVertexArray()
{
	if (mpr_nwchild)
		mpr_nwchild->UpdateEdgeGeometryVertexArray();
	if (mpr_nechild)
		mpr_nechild->UpdateEdgeGeometryVertexArray();
	if (mpr_swchild)
		mpr_swchild->UpdateEdgeGeometryVertexArray();
	if (mpr_sechild)
		mpr_sechild->UpdateEdgeGeometryVertexArray();

	UpdateEdgeGeomVerttexArray(GLB_WESTEDGE_NORTHSECTION);///<西部边缘的北半部
	UpdateEdgeGeomVerttexArray(GLB_WESTEDGE_SOUTHSECTION);///<西部边缘的南半部
	UpdateEdgeGeomVerttexArray(GLB_EASTEDGE_NORTHSECTION);///<东部边缘的北半部
	UpdateEdgeGeomVerttexArray(GLB_EASTEDGE_SOUTHSECTION);///<东部边缘的南半部

	UpdateEdgeGeomVerttexArray(GLB_NORTHEDGE_WESTSECTION);///<北部边缘的西半部
	UpdateEdgeGeomVerttexArray(GLB_NORTHEDGE_EASTSECTION);///<北部边缘的东半部
	UpdateEdgeGeomVerttexArray(GLB_SOUTHEDGE_WESTSECTION);///<南部边缘的西半部
	UpdateEdgeGeomVerttexArray(GLB_SOUTHEDGE_EASTSECTION);///<南部边缘的东半部
	return true;
}

glbBool CGlbGlobeTerrainTile::UpdateEdgeGeomVerttexArray(GlbTileEdgeEnum edge)
{
	switch(edge)
	{
	case GLB_NORTHEDGE_WESTSECTION:
		{
			if (mpr_p_north_westhalf_geom)
			{				
				CGlbGlobeTerrainTile* p_north_tile = (mpr_globe_type==GLB_GLOBETYPE_GLOBE) ? GetNorthDirectionTile() : GetNorthDirectionTileOnFlatTerrain();
				if (p_north_tile)
				{
					osg::ref_ptr<osg::Vec3dArray> p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_WESTSECTION);
					if (p_neighbour_edge_vertex_array==NULL)
					{// 临边顶点数组 -- 测试代码 2014.11.23 李总测试中发现进入此种情况
						p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_WESTSECTION);
						return false;
					}
					osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array();		
					p_VertexArray->reserve(mpr_p_north_westhalf_vertexarray->size()*2);					
					osg::Vec3d point;
					for (long k = 0; k < (long)mpr_p_north_westhalf_vertexarray->size(); k++)
					{
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								mpr_p_north_westhalf_vertexarray->at(k).x(),mpr_p_north_westhalf_vertexarray->at(k).y(),mpr_p_north_westhalf_vertexarray->at(k).z(),					
								point.x(), point.y(), point.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){							
							point.set(mpr_p_north_westhalf_vertexarray->at(k).y(),mpr_p_north_westhalf_vertexarray->at(k).x(),mpr_p_north_westhalf_vertexarray->at(k).z());
						}
						p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));
						p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));						
					}
					//mpr_p_north_westhalf_geom->setVertexArray(p_VertexArray.get());
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_north_westhalf_geom.get(),p_VertexArray.get());
					mpr_terrain->AddPackageOperation(_opera);
				}
			}
		}
		break;
	case GLB_NORTHEDGE_EASTSECTION:
		{
			if (mpr_p_north_easthalf_geom)
			{
				CGlbGlobeTerrainTile* p_north_tile = (mpr_globe_type==GLB_GLOBETYPE_GLOBE) ? GetNorthDirectionTile() : GetNorthDirectionTileOnFlatTerrain();
				if (p_north_tile)
				{
					osg::ref_ptr<osg::Vec3dArray> p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_EASTSECTION);
					if (p_neighbour_edge_vertex_array==NULL)
					{// 临边顶点数组 -- 测试代码 2014.11.23 李总测试中发现进入此种情况
						p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_north_tile,GLB_NORTHEDGE_WESTSECTION);
						return false;
					}

					osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array();		
					p_VertexArray->reserve(mpr_p_north_easthalf_vertexarray->size()*2);					
					osg::Vec3d point;
					for (long k = 0; k < (long)mpr_p_north_easthalf_vertexarray->size(); k++)
					{				
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								mpr_p_north_easthalf_vertexarray->at(k).x(),mpr_p_north_easthalf_vertexarray->at(k).y(),mpr_p_north_easthalf_vertexarray->at(k).z(),
								point.x(), point.y(), point.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){							
							point.set(mpr_p_north_easthalf_vertexarray->at(k).y(),mpr_p_north_easthalf_vertexarray->at(k).x(),mpr_p_north_easthalf_vertexarray->at(k).z());
						}
						p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));		
						p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));						
					}					
					//mpr_p_north_easthalf_geom->setVertexArray(p_VertexArray.get());
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_north_easthalf_geom.get(),p_VertexArray.get());
					mpr_terrain->AddPackageOperation(_opera);
				}
			}
		}
		break;
	case GLB_SOUTHEDGE_WESTSECTION:
		{
			if (mpr_p_south_westhalf_geom)
			{			
				CGlbGlobeTerrainTile* p_south_tile = (mpr_globe_type==GLB_GLOBETYPE_GLOBE) ?  GetSouthDirectionTile() : GetSouthDirectionTileOnFlatTerrain();
				if (p_south_tile)
				{
					osg::ref_ptr<osg::Vec3dArray> p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_SOUTHEDGE_WESTSECTION);
					if (p_neighbour_edge_vertex_array==NULL)
					{// 临边顶点数组 -- 测试代码 2014.11.23 李总测试中发现进入此种情况
						p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_NORTHEDGE_WESTSECTION);
						return false;
					}

					osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array();
					p_VertexArray->reserve(mpr_p_south_westhalf_vertexarray->size()*2);
					osg::Vec3d point;
					for (long k = 0; k < (long)mpr_p_south_westhalf_vertexarray->size(); k++)
					{
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								mpr_p_south_westhalf_vertexarray->at(k).x(),mpr_p_south_westhalf_vertexarray->at(k).y(),mpr_p_south_westhalf_vertexarray->at(k).z() ,
								point.x(), point.y(), point.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							//point = mpr_p_south_westhalf_vertexarray->at(k);
							point.set(mpr_p_south_westhalf_vertexarray->at(k).y(),mpr_p_south_westhalf_vertexarray->at(k).x(),mpr_p_south_westhalf_vertexarray->at(k).z());
						}

						p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));									
						p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));					
					}
					//mpr_p_south_westhalf_geom->setVertexArray(p_VertexArray.get());
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_south_westhalf_geom.get(),p_VertexArray.get());
					mpr_terrain->AddPackageOperation(_opera);
				}
			}		
		}
		break;
	case GLB_SOUTHEDGE_EASTSECTION:
		{
			if (mpr_p_south_easthalf_geom)
			{
				CGlbGlobeTerrainTile* p_south_tile = (mpr_globe_type==GLB_GLOBETYPE_GLOBE) ?  GetSouthDirectionTile() : GetSouthDirectionTileOnFlatTerrain();
				if (p_south_tile)
				{
					osg::ref_ptr<osg::Vec3dArray> p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_SOUTHEDGE_EASTSECTION);
					if (p_neighbour_edge_vertex_array==NULL)
					{// 临边顶点数组 -- 测试代码 2014.11.23 李总测试中发现进入此种情况
						p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_south_tile,GLB_NORTHEDGE_WESTSECTION);
						return false;
					}

					osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array();
					p_VertexArray->reserve(mpr_p_south_easthalf_vertexarray->size()*2);
					osg::Vec3d point;
					for (long k = 0; k < (long)mpr_p_south_easthalf_vertexarray->size(); k++)
					{
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								mpr_p_south_easthalf_vertexarray->at(k).x(),mpr_p_south_easthalf_vertexarray->at(k).y(),mpr_p_south_easthalf_vertexarray->at(k).z() ,
								point.x(), point.y(), point.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){
							//point = mpr_p_south_westhalf_vertexarray->at(k);
							point.set(mpr_p_south_easthalf_vertexarray->at(k).y(),mpr_p_south_easthalf_vertexarray->at(k).x(),mpr_p_south_easthalf_vertexarray->at(k).z());
						}

						p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));									
						p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));					
					}
					//mpr_p_south_easthalf_geom->setVertexArray(p_VertexArray.get());
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_south_easthalf_geom.get(),p_VertexArray.get());
					mpr_terrain->AddPackageOperation(_opera);
				}
			}		
		}
		break;
	case GLB_WESTEDGE_NORTHSECTION:
		{
			if (mpr_p_west_northhalf_geom)
			{
				CGlbGlobeTerrainTile* p_west_tile = (mpr_globe_type==GLB_GLOBETYPE_GLOBE) ? GetWestDirectionTile() : GetWestDirectionTileOnFlatTerrain();
				if (p_west_tile)
				{
					osg::ref_ptr<osg::Vec3dArray> p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_WESTEDGE_NORTHSECTION);
					if (p_neighbour_edge_vertex_array==NULL)
					{// 临边顶点数组 -- 测试代码 2014.11.23 李总测试中发现进入此种情况
						p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_NORTHEDGE_WESTSECTION);
						return false;
					}

					osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array();
					p_VertexArray->reserve(mpr_p_west_northhalf_vertexarray->size()*2);
					osg::Vec3d point;
					for (long k = 0; k < (long)mpr_p_west_northhalf_vertexarray->size(); k++)
					{
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								mpr_p_west_northhalf_vertexarray->at(k).x(),mpr_p_west_northhalf_vertexarray->at(k).y(),mpr_p_west_northhalf_vertexarray->at(k).z() ,
								point.x(), point.y(), point.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){						
							point.set(mpr_p_west_northhalf_vertexarray->at(k).y(),mpr_p_west_northhalf_vertexarray->at(k).x(),mpr_p_west_northhalf_vertexarray->at(k).z());
						}

						p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));									
						p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));					
					}
					//mpr_p_west_northhalf_geom->setVertexArray(p_VertexArray.get());
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_west_northhalf_geom.get(),p_VertexArray.get());
					mpr_terrain->AddPackageOperation(_opera);
				}
			}
		}
		break;
	case GLB_WESTEDGE_SOUTHSECTION:
		{
			if (mpr_p_west_southhalf_geom)
			{
				CGlbGlobeTerrainTile* p_west_tile = (mpr_globe_type==GLB_GLOBETYPE_GLOBE) ? GetWestDirectionTile() : GetWestDirectionTileOnFlatTerrain();
				if (p_west_tile)
				{
					osg::ref_ptr<osg::Vec3dArray> p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_WESTEDGE_SOUTHSECTION);
					if (p_neighbour_edge_vertex_array==NULL)
					{// 临边顶点数组 -- 测试代码 2014.11.23 李总测试中发现进入此种情况
						p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_west_tile,GLB_NORTHEDGE_WESTSECTION);
						return false;
					}

					osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array();
					p_VertexArray->reserve(mpr_p_west_southhalf_vertexarray->size()*2);
					osg::Vec3d point;
					for (long k = 0; k < (long)mpr_p_west_southhalf_vertexarray->size(); k++)
					{
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								mpr_p_west_southhalf_vertexarray->at(k).x(),mpr_p_west_southhalf_vertexarray->at(k).y(),mpr_p_west_southhalf_vertexarray->at(k).z() ,
								point.x(), point.y(), point.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){						
							point.set(mpr_p_west_southhalf_vertexarray->at(k).y(),mpr_p_west_southhalf_vertexarray->at(k).x(),mpr_p_west_southhalf_vertexarray->at(k).z());
						}

						p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));									
						p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));					
					}
					//mpr_p_west_southhalf_geom->setVertexArray(p_VertexArray.get());
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_west_southhalf_geom.get(),p_VertexArray.get());
					mpr_terrain->AddPackageOperation(_opera);
				}
			}
		}
		break;
	case GLB_EASTEDGE_NORTHSECTION:
		{
			if (mpr_p_east_northhalf_geom)
			{
				CGlbGlobeTerrainTile* p_east_tile = (mpr_globe_type==GLB_GLOBETYPE_GLOBE) ? GetEastDirectionTile() : GetEastDirectionTileOnFlatTerrain();
				if (p_east_tile)
				{
					osg::ref_ptr<osg::Vec3dArray> p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_EASTEDGE_NORTHSECTION);
					if (p_neighbour_edge_vertex_array==NULL)
					{// 临边顶点数组 -- 测试代码 2014.11.23 李总测试中发现进入此种情况
						p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_NORTHEDGE_WESTSECTION);
						return false;
					}

					osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array();
					p_VertexArray->reserve(mpr_p_east_northhalf_vertexarray->size()*2);
					osg::Vec3d point;
					for (long k = 0; k < (long)mpr_p_east_northhalf_vertexarray->size(); k++)
					{
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								mpr_p_east_northhalf_vertexarray->at(k).x(),mpr_p_east_northhalf_vertexarray->at(k).y(),mpr_p_east_northhalf_vertexarray->at(k).z() ,
								point.x(), point.y(), point.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){						
							point.set(mpr_p_east_northhalf_vertexarray->at(k).y(),mpr_p_east_northhalf_vertexarray->at(k).x(),mpr_p_east_northhalf_vertexarray->at(k).z());
						}

						p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));									
						p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));					
					}
					//mpr_p_east_northhalf_geom->setVertexArray(p_VertexArray.get());
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_east_northhalf_geom.get(),p_VertexArray.get());
					mpr_terrain->AddPackageOperation(_opera);
				}
			}
		}
		break;
	case GLB_EASTEDGE_SOUTHSECTION:
		{
			if (mpr_p_east_southhalf_geom)
			{
				CGlbGlobeTerrainTile* p_east_tile = (mpr_globe_type==GLB_GLOBETYPE_GLOBE) ? GetEastDirectionTile() : GetEastDirectionTileOnFlatTerrain();
				if (p_east_tile)
				{
					osg::ref_ptr<osg::Vec3dArray> p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_EASTEDGE_SOUTHSECTION);
					if (p_neighbour_edge_vertex_array==NULL)
					{// 临边顶点数组 -- 测试代码 2014.11.23 李总测试中发现进入此种情况
						p_neighbour_edge_vertex_array = GetNeighbourTileEdgeVecArray_Ext(p_east_tile,GLB_NORTHEDGE_WESTSECTION);
						return false;
					}

					osg::ref_ptr<osg::Vec3Array> p_VertexArray = new osg::Vec3Array();
					p_VertexArray->reserve(mpr_p_east_southhalf_vertexarray->size()*2);
					osg::Vec3d point;
					for (long k = 0; k < (long)mpr_p_east_southhalf_vertexarray->size(); k++)
					{
						if (mpr_globe_type==GLB_GLOBETYPE_GLOBE){
							g_ellipsmodel->convertLatLongHeightToXYZ(
								mpr_p_east_southhalf_vertexarray->at(k).x(),mpr_p_east_southhalf_vertexarray->at(k).y(),mpr_p_east_southhalf_vertexarray->at(k).z() ,
								point.x(), point.y(), point.z() );
						}else if (mpr_globe_type==GLB_GLOBETYPE_FLAT){						
							point.set(mpr_p_east_southhalf_vertexarray->at(k).y(),mpr_p_east_southhalf_vertexarray->at(k).x(),mpr_p_east_southhalf_vertexarray->at(k).z());
						}

						p_VertexArray->push_back(osg::Vec3f(point - mpr_local_origin));									
						p_VertexArray->push_back(osg::Vec3f(p_neighbour_edge_vertex_array->at(k) - mpr_local_origin));					
					}
					//mpr_p_east_southhalf_geom->setVertexArray(p_VertexArray.get());
					CGlbGlobeTerrain::GlbTileOperation _opera(CGlbGlobeTerrain::GLB_OPENERAION_MODIFY,CGlbGlobeTerrain::GLB_VECARRAY,mpr_p_geode.get(),mpr_p_east_southhalf_geom.get(),p_VertexArray.get());
					mpr_terrain->AddPackageOperation(_opera);
				}
			}
		}
		break;
	}
	return true;
}

glbBool CGlbGlobeTerrainTile::GetSortedChildTiles(std::map<glbDouble,glbref_ptr<CGlbGlobeTerrainTile>>& tiles)
{
	if (!mpr_terrain)
		return false;
#if 1
	// 思路1 ： 根据相机到块中心点距离排序
	osg::Vec3d camerapos;
	mpr_terrain->mpr_globe->GetView()->GetCameraPos(camerapos);
	glbDouble east,west,south,north,minAlt,maxAlt;
	if (mpr_nwchild)
	{
		mpr_nwchild->GetRange(east,west,south,north,minAlt,maxAlt);	

		glbDouble centerLon = (east + west)*0.5;
		glbDouble centerLat = (south + north)*0.5;
		glbDouble dist = sqrt((camerapos.x()-centerLon)*(camerapos.x()-centerLon) + (camerapos.y()-centerLat)*(camerapos.y()-centerLat));

		tiles[dist] = mpr_nwchild;
	}	

	if (mpr_nechild)
	{
		mpr_nechild->GetRange(east,west,south,north,minAlt,maxAlt);	

		glbDouble centerLon = (east + west)*0.5;
		glbDouble centerLat = (south + north)*0.5;
		glbDouble dist = sqrt((camerapos.x()-centerLon)*(camerapos.x()-centerLon) + (camerapos.y()-centerLat)*(camerapos.y()-centerLat));

		tiles[dist] = mpr_nechild;
	}

	if (mpr_swchild)
	{
		mpr_swchild->GetRange(east,west,south,north,minAlt,maxAlt);	

		glbDouble centerLon = (east + west)*0.5;
		glbDouble centerLat = (south + north)*0.5;
		glbDouble dist = sqrt((camerapos.x()-centerLon)*(camerapos.x()-centerLon) + (camerapos.y()-centerLat)*(camerapos.y()-centerLat));

		tiles[dist] = mpr_swchild;
	}

	if (mpr_sechild)
	{
		mpr_sechild->GetRange(east,west,south,north,minAlt,maxAlt);	

		glbDouble centerLon = (east + west)*0.5;
		glbDouble centerLat = (south + north)*0.5;
		glbDouble dist = sqrt((camerapos.x()-centerLon)*(camerapos.x()-centerLon) + (camerapos.y()-centerLat)*(camerapos.y()-centerLat));

		tiles[dist] = mpr_sechild;
	}
#else
	// 思路2 ： 根据所占屏幕有效区域的面积大小来决定
	if (mpr_nwchild)
	{
		glbDouble area = mpr_nwchild->computeSrcProjArea();
		tiles[1/area] = mpr_nwchild;
	}
	if (mpr_nechild)
	{
		glbDouble area = mpr_nechild->computeSrcProjArea();
		tiles[1/area] = mpr_nechild;
	}
	if (mpr_swchild)
	{
		glbDouble area = mpr_swchild->computeSrcProjArea();
		tiles[1/area] = mpr_swchild;
	}
	if (mpr_sechild)
	{
		glbDouble area = mpr_sechild->computeSrcProjArea();
		tiles[1/area] = mpr_sechild;
	}
#endif
	return true;
}

glbBool CGlbGlobeTerrainTile::FilterOnTerrainObjects(std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs, std::vector<glbref_ptr<CGlbGlobeRObject>>& outObjs)
{
	if (terrainobjs.size()<=0) return false;

	std::vector<glbref_ptr<CGlbGlobeRObject>>::iterator _itr = terrainobjs.begin();
	while(_itr != terrainobjs.end())
	{
		glbref_ptr<CGlbExtent> ext = (*_itr)->GetBound(false);
		if(ext == NULL){++_itr;continue;}
		glbref_ptr<CGlbExtent> interExt = ext->Intersect(*(mpr_extent.get()),false);
		if (interExt!=NULL)
		{// 相交
			outObjs.push_back(*_itr);
		}
		++_itr;
	}

	if (outObjs.size()>0)
		return true;

	return false;
}

glbBool CGlbGlobeTerrainTile::FilterObjDirtyExt(std::vector<CGlbExtent>&objDirtyExts, std::vector<CGlbExtent>& outObjDirtyExts)
{
	if (objDirtyExts.size()<=0) return false;
	std::vector<CGlbExtent>::iterator _itr = objDirtyExts.begin();
	while (_itr != objDirtyExts.end())
	{
		glbref_ptr<CGlbExtent> interExt = _itr->Intersect(*(mpr_extent.get()),false);
		if (interExt!=NULL)
		{// 相交
			outObjDirtyExts.push_back(*_itr);
		}
		++_itr;
	}
	return false;
}

typedef Coordinate PT;
glbDouble CGlbGlobeTerrainTile::computeSrcProjArea()
{
	CGlbGlobeView* globeview = mpr_terrain->mpr_globe->GetView();
	if (globeview==NULL)
		return -1.0;

	glbDouble area = 0;
	osg::Camera* p_camera = globeview->GetOsgCamera();
	double vx = p_camera->getViewport()->x();
	double vy = p_camera->getViewport()->y();
	double vwidth = p_camera->getViewport()->width();
	double vheight = p_camera->getViewport()->height();
	double minX = vx;
	double minY = vy;
	double maxX = vx+vwidth;
	double maxY = vy+vheight;

	geos::geom::GeometryFactory factory;
	geos::geom::CoordinateArraySequenceFactory csf;

	//// 1. 由ext生成rectPoly
	//geos::geom::CoordinateSequence* cs1 = csf.create(5,2);
	//cs1->setAt(PT(minX,minY,0),0);
	//cs1->setAt(PT(maxX,minY,0),1);
	//cs1->setAt(PT(maxX,maxY,0),2);
	//cs1->setAt(PT(minX,maxY,0),3);
	//cs1->setAt(PT(minX,minY,0),4);
	//geos::geom::LinearRing* ring1 = factory.createLinearRing(cs1);
	//geos::geom::Geometry* rectPoly = factory.createPolygon(ring1,NULL);

	geos::geom::CoordinateSequence* cs2 = csf.create(5,2);
	if (mpr_globe_type==GLB_GLOBETYPE_GLOBE)
	{
		glbDouble averageAlt = (mpr_minalt+mpr_maxalt)*0.5;
		glbDouble x0,y0,x1,y1;
		osg::Vec3d vec1,vec2;

		// 测试西北角点
		x0 = x1 = mpr_west;
		y0 = mpr_north;		
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());		
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		cs2->setAt(PT(vec1.x(),vec1.y(),0),0);

		cs2->setAt(PT(vec1.x(),vec1.y(),0),4);

		// 测试西南角点
		x0 = mpr_west;
		y0 = y1 = mpr_south;			
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());	
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		cs2->setAt(PT(vec1.x(),vec1.y(),0),1);

		// 测试东南角点
		x0 = x1 = mpr_east;
		y0 = mpr_south;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		cs2->setAt(PT(vec1.x(),vec1.y(),0),2);

		// 测试东北角点
		x0 = mpr_east;
		y0 = y1 = mpr_north;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		cs2->setAt(PT(vec1.x(),vec1.y(),0),3);

		geos::geom::LinearRing* shell = factory.createLinearRing(cs2);
		geos::geom::Geometry* tilePoly = factory.createPolygon(shell,NULL);

		if (tilePoly)
			area = tilePoly->getArea();
		//3. 求交集
		//geos::geom::Geometry* outGeom=NULL;
		//if (tilePoly && rectPoly)
		//{
		//	outGeom = rectPoly->intersection(tilePoly);
		//	if (outGeom)
		//	{
		//		GeometryTypeId typeId = outGeom->getGeometryTypeId();
		//		std::string type = outGeom->getGeometryType();		

		//		area = outGeom->getArea();
		//	}
		//}	

		if (tilePoly) delete tilePoly;
		//if (outGeom) delete outGeom;

		//// 测试西边中点
		//x0 = x1 = mpr_west;
		//y0 = (mpr_north+mpr_south)*0.5;
		//y1 = y0 - (mpr_north - mpr_south)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		//WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		//WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		////if( ( (_vx-vec1.x())*(_vx2-vec1.x())<=0 && (_vy-vec1.y())*(_vy2-vec1.y())<=0 ) ||
		////	( (_vx-vec2.x())*(_vx2-vec2.x())<=0 && (_vy-vec2.y())*(_vy2-vec2.y())<=0 ) )
		////{
		//	double screenLen5 = (vec1-vec2).length();
		//	if (screenLen5 > TEST_RESOLUTION_RADIO*2)
		//	{// 像素放大显示，需要细分
		//		return true;
		//	}
		////}

		//// 测试南边中点
		//x0 = (mpr_west+mpr_east)*0.5;
		//y0 = y1 = mpr_south;	
		//x1 = x0 + (mpr_east-mpr_west)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		//WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		//WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		////if( ( (_vx-vec1.x())*(_vx2-vec1.x())<=0 && (_vy-vec1.y())*(_vy2-vec1.y())<=0 ) ||
		////	( (_vx-vec2.x())*(_vx2-vec2.x())<=0 && (_vy-vec2.y())*(_vy2-vec2.y())<=0 ) )
		////{
		//	double screenLen6 = (vec1-vec2).length();
		//	if (screenLen6 > TEST_RESOLUTION_RADIO*2)
		//	{// 像素放大显示，需要细分
		//		return true;
		//	}
		////}
		//// 测试东边中点
		//x0 = x1 = mpr_east;
		//y0 = (mpr_south+mpr_north)*0.5;
		//y1 = y0 + (mpr_north-mpr_south)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		//WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		//WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		////if( ( (_vx-vec1.x())*(_vx2-vec1.x())<=0 && (_vy-vec1.y())*(_vy2-vec1.y())<=0 ) ||
		////	( (_vx-vec2.x())*(_vx2-vec2.x())<=0 && (_vy-vec2.y())*(_vy2-vec2.y())<=0 ) )
		////{
		//	double screenLen7 = (vec1-vec2).length();
		//	if (screenLen7 > TEST_RESOLUTION_RADIO*2)
		//	{// 像素放大显示，需要细分
		//		return true;
		//	}
		////}

		// 测试北边中点
		//x0 = (mpr_east+mpr_west)*0.5;
		//y0 = y1 = mpr_north;
		//x1 = x0 - (mpr_east-mpr_west)*TEST_RESOLUTION_RADIO/TILE_TEXTURE_SIZE;
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y0),osg::DegreesToRadians(x0),averageAlt,vec1.x(),vec1.y(),vec1.z());
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y1),osg::DegreesToRadians(x1),averageAlt,vec2.x(),vec2.y(),vec2.z());
		//WorldToScreen(vec1.x(),vec1.y(),vec1.z());
		//WorldToScreen(vec2.x(),vec2.y(),vec2.z());
		////if( ( (_vx-vec1.x())*(_vx2-vec1.x())<=0 && (_vy-vec1.y())*(_vy2-vec1.y())<=0 ) ||
		////	( (_vx-vec2.x())*(_vx2-vec2.x())<=0 && (_vy-vec2.y())*(_vy2-vec2.y())<=0 ) )
		////{
		//	double screenLen8 = (vec1-vec2).length();
		//	if (screenLen8 > TEST_RESOLUTION_RADIO*2)
		//	{// 像素放大显示，需要细分
		//		return true;
		//	}
		////}

	}
	else if(mpr_globe_type==GLB_GLOBETYPE_FLAT)
	{
		glbDouble xmin,xmax,ymin,ymax,zmin,zmax;
		mpr_extent->Get(&xmin,&xmax,&ymin,&ymax,&zmin,&zmax);
		glbDouble averagez = (zmin+zmax)*0.5f;

		glbDouble x0,y0,z0,x1,y1,z1;

		// 测试左上角点
		x0 = x1 = xmin;
		y0 = ymax;
		z0 = z1 = averagez;			
		WorldToScreen(x0,y0,z0);		
		cs2->setAt(PT(x0,y0,0),0);
		cs2->setAt(PT(x0,y0,0),4);

		// 测试左下角点
		x0 = xmin;
		y0 = y1 = ymin;
		z0 = z1 = averagez;			
		WorldToScreen(x0,y0,z0);		
		cs2->setAt(PT(x0,y0,0),1);
		// 测试右下角点
		x0 = x1 = xmax;
		y0 = ymin;
		z0 = z1 = averagez;			
		WorldToScreen(x0,y0,z0);		
		cs2->setAt(PT(x0,y0,0),2);

		// 测试右上角点
		x0 = xmax;
		y0 = y1 = ymax;
		z0 = z1 = averagez;		
		WorldToScreen(x0,y0,z0);
		cs2->setAt(PT(x0,y0,0),3);

		geos::geom::LinearRing* shell = factory.createLinearRing(cs2);
		geos::geom::Geometry* tilePoly = factory.createPolygon(shell,NULL);

		area = tilePoly->getArea();
		//3. 求交集
		//geos::geom::Geometry* outGeom=NULL;
		//if (tilePoly && rectPoly)
		//{
		//	outGeom = rectPoly->intersection(tilePoly);
		//	if (outGeom)
		//	{
		//		GeometryTypeId typeId = outGeom->getGeometryTypeId();
		//		std::string type = outGeom->getGeometryType();		

		//		area = outGeom->getArea();
		//	}
		//}	
	
		if (tilePoly) delete tilePoly;
		//if (outGeom) delete outGeom;
	}

	//if (rectPoly) delete rectPoly;

	return area;
}


void CGlbGlobeTerrainTile::RefreshTexture_Step1(	std::vector<CGlbExtent>						&objDirtyExts,
							std::vector<CGlbExtent>						&domDirtyExts,
							std::vector<glbref_ptr<CGlbGlobeDomLayer>>  &domlayers,                   
							std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainObjs)
{
	if (mpr_isDataLoaded==false)
		return;
	//1. 分别计算objDirtyExts和domDirtyExts区域与当前tile范围是否有交集
	glbBool _tobjDirty=false;
	glbBool _domlayerDirty = false;
	_tobjDirty = objDirtyExts.size()>0 ? true : false;
	_domlayerDirty = domDirtyExts.size()>0 ? true : false;

	if (_tobjDirty || _domlayerDirty)
	{// 刷新区域与地形块区域有交集

		// 1. 递归刷新子块
		glbref_ptr<CGlbGlobeTerrainTile> p_northwestchild = GetNorthWestChild();
		if (p_northwestchild )
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_northwestchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_northwestchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_northwestchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);
		}

		glbref_ptr<CGlbGlobeTerrainTile> p_northeastchild = GetNorthEastChild();
		if (p_northeastchild )
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_northeastchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_northeastchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_northeastchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);
		}

		glbref_ptr<CGlbGlobeTerrainTile> p_southwestchild = GetSouthWestChild();	
		if (p_southwestchild)
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_southwestchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_southwestchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_southwestchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);
		}

		glbref_ptr<CGlbGlobeTerrainTile> p_southeastchild = GetSouthEastChild();
		if (p_southeastchild )
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_southeastchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_southeastchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_southeastchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);	
		}

		// 2. 创建刷新任务，重新生成mpr_texutre
		//CGlbGlobeRefreshTileTextureTask* task = new CGlbGlobeRefreshTileTextureTask(this,domlayers,terrainObjs,_domlayerDirty);
		//mpr_terrain->mpr_globe->mpr_taskmanager->AddTerrainTask(task);

		if (mpr_node->getNumParents() > 0)
		{
			LoadDomData(domlayers,terrainObjs,_domlayerDirty);
			mpr_textureRefreshed = true;
		}
	}
}


void CGlbGlobeTerrainTile::RefreshTexture_Step2(	std::vector<CGlbExtent>						&objDirtyExts,
	std::vector<CGlbExtent>						&domDirtyExts,
	std::vector<glbref_ptr<CGlbGlobeDomLayer>>  &domlayers,                   
	std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainObjs)
{
	if (mpr_isDataLoaded==false)
		return;
	//1. 分别计算objDirtyExts和domDirtyExts区域与当前tile范围是否有交集
	glbBool _tobjDirty=false;
	glbBool _domlayerDirty = false;
	_tobjDirty = objDirtyExts.size()>0 ? true : false;
	_domlayerDirty = domDirtyExts.size()>0 ? true : false;	

	if (_tobjDirty || _domlayerDirty)
	{// 刷新区域与地形块区域有交集

		// 1. 递归刷新子块
		glbref_ptr<CGlbGlobeTerrainTile> p_northwestchild = GetNorthWestChild();
		if (p_northwestchild )
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_northwestchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_northwestchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_northwestchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);
		}

		glbref_ptr<CGlbGlobeTerrainTile> p_northeastchild = GetNorthEastChild();
		if (p_northeastchild )
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_northeastchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_northeastchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_northeastchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);
		}

		glbref_ptr<CGlbGlobeTerrainTile> p_southwestchild = GetSouthWestChild();	
		if (p_southwestchild)
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_southwestchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_southwestchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_southwestchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);
		}

		glbref_ptr<CGlbGlobeTerrainTile> p_southeastchild = GetSouthEastChild();
		if (p_southeastchild )
		{
			std::vector<glbref_ptr<CGlbGlobeRObject>> childtobjs;
			std::vector<CGlbExtent> childDirtyExts;
			p_southeastchild->FilterOnTerrainObjects(terrainObjs,childtobjs);
			p_southeastchild->FilterObjDirtyExt(objDirtyExts,childDirtyExts);
			p_southeastchild->RefreshTexture(childDirtyExts,domDirtyExts,domlayers,childtobjs);	
		}

		// 2. 创建刷新任务，重新生成mpr_texutre
		//CGlbGlobeRefreshTileTextureTask* task = new CGlbGlobeRefreshTileTextureTask(this,domlayers,terrainObjs,_domlayerDirty);
		//mpr_terrain->mpr_globe->mpr_taskmanager->AddTerrainTask(task);

		if (mpr_textureRefreshed == false)
		{
			LoadDomData(domlayers,terrainObjs,_domlayerDirty);			
		}
		else
		{// 还原标识
			mpr_textureRefreshed = false;
		}
	}
}