#include "StdAfx.h"
#include "GlbGlobeUGPlane.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include <osgDB/ReadFile>
#include <osg/TexEnv>
#include <osg/Fog>
#include <osg/BlendFunc>
#include <osg/BlendColor>

using namespace GlbGlobe;

class CGlbUGPlaneCallBack : public osg::NodeCallback
{
public:
	/**
	 * @brief 构造函数
	**/
	CGlbUGPlaneCallBack(CGlbGlobe* pglobe)
	{ 
		mpr_p_globe = pglobe;
		mpr_west = DBL_MAX;
		mpr_east = DBL_MAX;
		mpr_south = DBL_MAX;
		mpr_north = DBL_MAX;
		mpr_size_lat = 0.0;
		mpr_size_lon = 0.0;
		mpr_opacity = 1.0;
		mpr_center = osg::Vec3d(0.0,0.0,0.0);
		mpr_p_ugswitch = NULL;
		mpr_showFilter = false;
		InitializeCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
	}
	/**
	 * @brief 析构函数
	**/
	~CGlbUGPlaneCallBack()
	{ 
		DeleteCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
	}
	/**
	 * @brief 回调函数
	**/
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		if(nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
		{
			if (mpr_p_globe)
			{
				if (mpr_west == DBL_MAX)
				{ 
					if ( mpr_p_globe->GetTerrainDataset() &&  mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
					{
						mpr_west = mpr_p_globe->GetTerrainDataset()->GetExtent()->GetLeft();
						mpr_east = mpr_p_globe->GetTerrainDataset()->GetExtent()->GetRight(); 
						mpr_south = mpr_p_globe->GetTerrainDataset()->GetExtent()->GetBottom();
						mpr_north = mpr_p_globe->GetTerrainDataset()->GetExtent()->GetTop();
					}
				}
			}
			if(mpr_p_globe->IsUnderGroundMode())	//开启地下模式
			{
				osg::Group* gp = dynamic_cast<osg::Group*>(node);
				if (!mpr_p_ugswitch.valid())
					mpr_p_ugswitch = new osg::Switch;
				if (gp->getNumChildren() == 0)
					gp->addChild(mpr_p_ugswitch);
				if(mpr_p_ugswitch->getNumChildren() == 0 && mpr_showFilter)
				{
					osg::ref_ptr<osg::Group> group = createUGPlane();
					mpr_p_ugswitch->addChild(group.get());
				}
				//mpr_p_ugswitch = dynamic_cast<osg::Switch*>(node);
				osg::Vec3d cameraPos;
				mpr_p_globe->GetView()->GetCameraPos(cameraPos);
				if (cameraPos.z() - mpr_p_globe->GetView()->GetTerrainElevationOfCameraPos() < 0.0)	//视点高度小于地面
				{
					if(!mpr_p_ugswitch->getValue(0))   //如果地下参考面被隐藏，则显示
					{  
						//Lock();
						EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
						mpr_p_ugswitch->setValue(0,true);
						//UnLock();
						LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
					}
					if (mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)	//球面模式
					{
						//交点移动超过2个单位，需要重新生成地下参考面
						if (fabs(cameraPos.x() - mpr_center.x()) >= 2.0 * mpr_size_lon || fabs(cameraPos.y() - mpr_center.y()) >= 2.0 * mpr_size_lat)
						{ 
							//Lock();
 							EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
							if(mpr_p_ugswitch->getNumChildren())
								mpr_p_ugswitch->removeChild(mpr_p_ugswitch->getChild(0));
							osg::ref_ptr<osg::Group> group = createUGPlane();
							mpr_p_ugswitch->addChild(group.get());
							//UnLock();
							LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
						}
					}
					else if(mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
					{
						if(mpr_p_ugswitch->getNumChildren() == 0)  //没有地下参考面节点
						{
							//Lock();
							EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
							osg::ref_ptr<osg::Group> group = createUGPlane();
							mpr_p_ugswitch->addChild(group.get());
							//UnLock();
							LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
						}
					}
				}
				else if(cameraPos.z() - mpr_p_globe->GetView()->GetTerrainElevationOfCameraPos() < 100) //视点高度小于100米
				{
					if (mpr_p_ugswitch.valid())
						if(mpr_p_ugswitch->getValue(0) && mpr_showFilter == false)  //如果地下参考面显示，则隐藏
						{
							//Lock();
							EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
							mpr_p_ugswitch->setValue(0,false);
							//UnLock();
							LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
						}
					if(mpr_p_ugswitch->getNumChildren() == 0)  //没有地下参考面节点
					{
						//Lock();
						EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
						osg::ref_ptr<osg::Group> group = createUGPlane();
						mpr_p_ugswitch->addChild(group.get());
						//UnLock();
						LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
					}
				}else
				{	
					if (mpr_p_ugswitch.valid())
						if(mpr_p_ugswitch->getValue(0) && mpr_showFilter == false)  //如果地下参考面显示，则隐藏
							mpr_p_ugswitch->setValue(0,false);
				}
			}else
			{
				//mpr_p_ugswitch = dynamic_cast<osg::Switch*>(node);
				//if (mpr_p_ugswitch->getNumChildren())
				//	mpr_p_ugswitch->removeChild(mpr_p_ugswitch->getChild(0));
				osg::Group* gp = dynamic_cast<osg::Group*>(node);   //去除地下参考面节点
				if (gp->getNumChildren())
					gp->removeChild(gp->getChild(0));
			}
		}
		traverse(node,nv);
	}
	/**
	 * @brief 创建地下参考面
	**/
	osg::ref_ptr<osg::Group> createUGPlane()
	{
		if (mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			osg::Vec3d center;
			mpr_p_globe->GetView()->GetCameraPos(center);
			ComputeBlockSize(center);

			//求当前center点离球心距离
			osg::Vec3d center_w;
			glbDouble UGdepth = mpr_p_globe->GetUnderGroundAltitude();
			mpr_center = osg::Vec3d(center.x(),center.y(),UGdepth);

			osg::Matrixd mat_centerToworld;
			g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),UGdepth,mat_centerToworld);
			//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),UGdepth,center_w.x(),center_w.y(),center_w.z());
			center_w = mpr_center * mat_centerToworld;
			osg::Matrixd mat_worldTocenter = osg::Matrixd::inverse(mat_centerToworld);
			glbDouble radius = center_w.length();
			glbDouble terrainElevationOfCameraPos = mpr_p_globe->GetView()->GetTerrainElevationOfCameraPos();
			//if (UGdepth > terrainElevationOfCameraPos)
			//	angle = 0.0;
			//else
			glbDouble angle = acos(radius/(radius-UGdepth+terrainElevationOfCameraPos));	//地下参考面在相机下最多能显示的范围，球面角度描述
			angle = osg::RadiansToDegrees(angle);

			//不考虑经纬度差异
			glbInt32 lat_num = 2.0*angle/mpr_size_lat;
			if (lat_num % 2 == 0)		//保证方格为偶数个，线为奇数个
				lat_num += 5;
			else
				lat_num += 4;	//两边各多画1个
			glbInt32 lon_num = 2.0*angle/mpr_size_lon;
			if (lon_num % 2 == 0)		//保证方格为偶数个，线为奇数个
				lon_num += 5;
			else
				lon_num += 4;	//两边各多画1个

			osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> coord = new osg::Vec2Array;
			osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
			osg::ref_ptr<osg::DrawElementsUInt> index = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS,0);
			//以(center.x(),center.y(),UGdepth)为中心
			glbInt32 startlat_num = -(lat_num-1)/2;
			glbInt32 startlon_num = -(lon_num-1)/2;
			for ( glbInt32 i = startlat_num; i < lat_num+startlat_num; i++)
			{
				for ( glbInt32 j = startlon_num; j < lon_num+startlon_num; j++)
				{
					osg::Vec3d vec(center.x() + j*mpr_size_lon,center.y() + i*mpr_size_lat,UGdepth);
					g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(vec.y()),osg::DegreesToRadians(vec.x()),vec.z(),vec.x(),vec.y(),vec.z());
					osg::Vec3f vecf = vec * mat_worldTocenter;
					vertex->push_back(vecf);
					coord->push_back(osg::Vec2(10.0*(j - startlon_num),10.0*(i - startlat_num)));
				}
			}

			geom->setVertexArray(vertex.get());
			geom->setTexCoordArray(0,coord.get());
			for(glbInt32 i = 0; i < lat_num -1; i++)
			{
				for(glbInt32 j = 0; j < lon_num -1; j++)
				{
					glbInt32 n = i*(lon_num-1)+j;
					index->push_back(n);
					index->push_back(n+1);
					index->push_back(n+1+lon_num);
					index->push_back(n+lon_num);
				}
			}
			osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
			color->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
			geom->setColorArray(color.get());
			geom->setColorBinding(osg::Geometry::BIND_OVERALL);
			geom->addPrimitiveSet(index.get());
			osg::ref_ptr<osg::Geode> geode = new osg::Geode();
			geode->addDrawable(geom.get());

			//贴纹理			
			HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
			CGlbWString dpath = CGlbPath::GetModuleDir(hmd);
			dpath += L"\\res\\ugpic.bmp";
			osg::ref_ptr<osg::Image> image = osgDB::readImageFile(dpath.ToString().c_str());
			osg::ref_ptr<osg::Texture2D> tex2d = new osg::Texture2D;
			if (image.get())
				tex2d->setImage(image.get());
			tex2d->setWrap(osg::Texture::WRAP_S,osg::Texture2D::REPEAT);
			tex2d->setWrap(osg::Texture::WRAP_T,osg::Texture2D::REPEAT);
			//设置纹理环境
			osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
			texenv->setMode(osg::TexEnv::REPLACE);//贴花
			//启用纹理单元0
			geode->getOrCreateStateSet()->setTextureAttributeAndModes(0,tex2d.get(),osg::StateAttribute::ON);
			geode->getOrCreateStateSet()->setMode( GL_CULL_FACE, osg::StateAttribute::ON );
			//设置纹理环境
			geode->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());

			//雾效果
			osg::ref_ptr<osg::Fog> fog = new osg::Fog();
			fog->setColor(osg::Vec4(0.0, 0.0, 1.0, 1.0));
			if((UGdepth - terrainElevationOfCameraPos) >= - 5000.0)
				fog->setDensity(0.0001f);
			else if((UGdepth - terrainElevationOfCameraPos) < - 1000.0 && (UGdepth - terrainElevationOfCameraPos) >= - 5000.0)
				fog->setDensity(0.00003f);
			else
				fog->setDensity(0.000008f);
			geode->getOrCreateStateSet()->setAttributeAndModes(fog.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);	
			osg::BlendColor *pBlenColor = new osg::BlendColor(osg::Vec4(1.0,1.0,1.0,mpr_opacity));  
			//osg::BlendFunc *pBlendFun = new osg::BlendFunc(osg::BlendFunc::CONSTANT_ALPHA,osg::BlendFunc::DST_ALPHA);
			osg::BlendFunc *pBlendFun = new osg::BlendFunc(osg::BlendFunc::ONE,osg::BlendFunc::ZERO);
			geode->getOrCreateStateSet()->setAttributeAndModes(pBlenColor,osg::StateAttribute::ON);
			geode->getOrCreateStateSet()->setAttributeAndModes(pBlendFun,osg::StateAttribute::ON);
			geode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
			geode->setName("UnderGroundPlane");
			geode->setNodeMask(0x02);

			osg::ref_ptr<osg::Group> group = new osg::Group;
			osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
			group->addChild(mt.get());
			mt->setMatrix(mat_centerToworld);
			mt->addChild(geode.get());
			return group;
		}else if (mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
		{
			glbDouble UGdepth = mpr_p_globe->GetUnderGroundAltitude();
			osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec2Array> coord = new osg::Vec2Array;
			osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
			//顶点
			vertex->push_back(osg::Vec3(mpr_west,mpr_south,UGdepth));
			vertex->push_back(osg::Vec3(mpr_east,mpr_south,UGdepth));
			vertex->push_back(osg::Vec3(mpr_east,mpr_north,UGdepth));
			vertex->push_back(osg::Vec3(mpr_west,mpr_north,UGdepth));
			glbInt32 coefficient_x = (mpr_east - mpr_west)/500.0;
			coefficient_x = coefficient_x > 1 ? coefficient_x : 1;
			glbInt32 coefficient_y = (mpr_north - mpr_south)/500.0;
			coefficient_y = coefficient_y > 1 ? coefficient_y : 1;
			coord->push_back(osg::Vec2(0.0,0.0));
			coord->push_back(osg::Vec2(coefficient_x,0.0));
			coord->push_back(osg::Vec2(coefficient_x,coefficient_y));
			coord->push_back(osg::Vec2(0.0,coefficient_y));
			geom->setVertexArray(vertex.get());
			geom->setTexCoordArray(0,coord.get());
			osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
			color->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
			geom->setColorArray(color.get());
			geom->setColorBinding(osg::Geometry::BIND_OVERALL);
			osg::ref_ptr<osg::DrawElementsUInt> quadBase = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
			quadBase->push_back(0);
			quadBase->push_back(1);
			quadBase->push_back(2);
			quadBase->push_back(3);
			geom->addPrimitiveSet(quadBase.get());
			osg::ref_ptr<osg::Geode> geode = new osg::Geode();
			geode->addDrawable(geom.get());
			//贴纹理
			CGlbWString dpath = CGlbPath::GetExecDir();
			dpath += L"\\res\\ugpic.bmp";
			osg::ref_ptr<osg::Image> image = osgDB::readImageFile(dpath.ToString().c_str());
			osg::ref_ptr<osg::Texture2D> tex2d = new osg::Texture2D;
			if (image.get())
				tex2d->setImage(image.get());
			tex2d->setWrap(osg::Texture::WRAP_S,osg::Texture2D::REPEAT);
			tex2d->setWrap(osg::Texture::WRAP_T,osg::Texture2D::REPEAT);
			//设置纹理环境
			osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
			texenv->setMode(osg::TexEnv::REPLACE);//贴花
			//启用纹理单元0
			geode->getOrCreateStateSet()->setTextureAttributeAndModes(0,tex2d.get(),osg::StateAttribute::ON);
			geode->getOrCreateStateSet()->setMode( GL_CULL_FACE, osg::StateAttribute::ON );
			//设置纹理环境
			geode->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());

			//雾效果
			osg::ref_ptr<osg::Fog> fog = new osg::Fog();
			fog->setColor(osg::Vec4(0.0, 0.0, 1.0, 1.0));
			glbDouble terrainElevationOfCameraPos = mpr_p_globe->GetView()->GetTerrainElevationOfCameraPos();
			if((UGdepth - terrainElevationOfCameraPos) >= - 5000.0)
				fog->setDensity(0.0001f);
			else if((UGdepth - terrainElevationOfCameraPos) < - 1000.0 && (UGdepth - terrainElevationOfCameraPos) >= - 5000.0)
				fog->setDensity(0.00003f);
			else
				fog->setDensity(0.000008f);
			geode->getOrCreateStateSet()->setAttributeAndModes(fog.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);	
			osg::BlendColor *pBlenColor = new osg::BlendColor(osg::Vec4(1.0,1.0,1.0,mpr_opacity));  
			//osg::BlendFunc *pBlendFun = new osg::BlendFunc(osg::BlendFunc::CONSTANT_ALPHA,osg::BlendFunc::DST_ALPHA);
			osg::BlendFunc *pBlendFun = new osg::BlendFunc(osg::BlendFunc::ONE,osg::BlendFunc::ZERO);
			geode->getOrCreateStateSet()->setAttributeAndModes(pBlenColor,osg::StateAttribute::ON);
			geode->getOrCreateStateSet()->setAttributeAndModes(pBlendFun,osg::StateAttribute::ON);
			geode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
			geode->setName("UnderGroundPlane");
			geode->setNodeMask(0x02);

			osg::ref_ptr<osg::Group> group = new osg::Group;
			group->addChild(geode.get());
			return group;
		}
		return NULL;
	}
	/**
	 * @brief 计算地下参考面块大小
	 * @param [in] center : 相机位置
	**/
	void ComputeBlockSize(osg::Vec3d& center)
	{
		if (mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)	  //单位°
		{		
			if (abs(center.y()) >89.0)
			{
				mpr_size_lat = 1.0;
				mpr_size_lon = 50;
			}
			mpr_size_lat = 0.1;
			mpr_size_lon = 0.1 / cos(osg::DegreesToRadians(center.y()));
		}else if (mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)	   //单位m
		{
			mpr_size_lat = 10000;
			mpr_size_lon = 10000;
		}
	}

	/**
	 * @brief 设置地下参考面透明度
	 * @param [in] opacity : 0.0为全透明，1.0为不透明
	**/
	glbBool SetUnderGroundOpacity(glbDouble opacity)
	{
		if (!mpr_p_ugswitch.valid())
			return false;
		if(mpr_opacity == opacity)
			return false;
		mpr_opacity = opacity;
		osg::BlendColor *pBlenColor=new osg::BlendColor(osg::Vec4d(1.0,1.0,1.0,mpr_opacity));  
		mpr_p_ugswitch->getOrCreateStateSet()->setAttributeAndModes(pBlenColor,osg::StateAttribute::ON);	
		return true;
	}

	/**
	 * @brief 获取地下参考面透明度
	 * @return : 0.0为全透明，1.0为不透明
	**/
	glbDouble GetUnderGroundOpacity()
	{
		return mpr_opacity;
	}

	/**
	 * @brief 获取地下参考面节点
	 * @return : 地下参考面节点
	**/
	osg::Switch* getUGPlaneNode()
	{
		return mpr_p_ugswitch.get();
	}

	/**
	 * @brief 计算屏幕上某点(x,y)处的地下参考面坐标      
	 *	@param [in] x 屏幕点的x坐标
	 * @param [in] y 屏幕点的y坐标
	 * @param [out] lonOrX 经度或X
	 * @param [out] latOrY 纬度或Y
	 * @param [out] altOrZ 高度或Z
	 * @note 输入点(x,y)为屏幕坐标,即窗口左下角点处为原点（0,0）
  		 - GLB_GLOBETYPE_GLOBE球模式下 lonOrX,latOrY单位为度
  		 - GLB_GLOBETYPE_FLAT平面模式下 lonOrX,latOrY单位为米	
	 * @return 成功返回true
	 *		-	失败返回false
	**/
	glbBool ScreenToUGPlaneCoordinate(glbInt32 x,glbInt32 y,glbDouble &lonOrx,glbDouble &latOry,glbDouble &altOrz)
	{
		CGlbGlobeView* p_globeview = mpr_p_globe->GetView();
		if (p_globeview==NULL)
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

		osgUtil::LineSegmentIntersector* lsi = new osgUtil::LineSegmentIntersector(geostart, geoend);
		osgUtil::IntersectionVisitor iv( lsi );
		if (mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE){
			lsi->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
		}else if (mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT){
			lsi->setIntersectionLimit(osgUtil::Intersector::NO_LIMIT);
		}

		//Lock();
		EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
		if (mpr_p_ugswitch)
			mpr_p_ugswitch->accept( iv );		
		//UnLock();
		LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	

		osgUtil::LineSegmentIntersector::Intersections& results = lsi->getIntersections();
		if (!results.empty() )
		{
			if (mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE){
				osg::Vec3d wgscoord = results.begin()->getWorldIntersectPoint();
				osg::Vec3d geodetic;
				g_ellipsmodel->convertXYZToLatLongHeight(
					wgscoord.x(),     wgscoord.y(),     wgscoord.z(),
					geodetic.y(), geodetic.x(), geodetic.z() );		
				lonOrx = osg::RadiansToDegrees(geodetic.x());// longitude
				latOry = osg::RadiansToDegrees(geodetic.y());// latitude
				altOrz = geodetic.z();						// height
			}		
			else if (mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
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

	/**
	 * @brief 计算射线是否与地下参考面相交      
	 *	@param [in] ln_pt1射线上一点(端点)
	 * @param [in] ln_dir射线方向
	 * @param [out] InterPos 如果相交，交点坐标	
	 * @return 相交返回true
	 *		-	不相交返回false
	**/
	glbBool IsRayInterUGPlane(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos)
	{
		if(!mpr_p_ugswitch.valid())
			return false;
		ln_dir.normalize();
		//double r = osg::WGS_84_RADIUS_EQUATOR;
		osg::Vec3d geostart = ln_pt1;
		osg::Vec3d geoend = ln_pt1 + ln_dir*ln_pt1.length();

		osgUtil::LineSegmentIntersector* lsi = new osgUtil::LineSegmentIntersector(geostart, geoend);
		osgUtil::IntersectionVisitor iv( lsi );
		if (mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE){
			lsi->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);
		}else if (mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT){
			lsi->setIntersectionLimit(osgUtil::Intersector::NO_LIMIT);
		}

		//Lock();
		EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
		mpr_p_ugswitch->accept( iv );		
		//UnLock();
		LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);	

		osgUtil::LineSegmentIntersector::Intersections& results = lsi->getIntersections();
		if (!results.empty() )
		{
			InterPos = results.begin()->getWorldIntersectPoint();
			if (mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
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
	/**
	 * @brief 设置地下参考面高度
	**/
	void SetAltitude(glbDouble altitude)
	{
		if(mpr_p_ugswitch == NULL)return;
		//Lock();
		EnterCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
		if(mpr_p_ugswitch->getNumChildren())
			mpr_p_ugswitch->removeChild(mpr_p_ugswitch->getChild(0));
		osg::ref_ptr<osg::Group> group = createUGPlane();
		mpr_p_ugswitch->addChild(group.get());
		//UnLock();
		LeaveCriticalSection(&mpr_tilebuffer_exchange_criticalsection);
	}

	void SetShowFilter(glbBool isFilter)
	{
		mpr_showFilter = isFilter;
	}
private:
	CGlbGlobe* mpr_p_globe;		//场景指针
	glbDouble mpr_size_lat;		//地下参考面块纬度间隔
	glbDouble mpr_size_lon;		//地下参考面块经度间隔
	osg::Vec3d mpr_center;		//地下参考面中心位置
	glbDouble mpr_opacity;		//地下参考面透明度
	osg::ref_ptr<osg::Switch> mpr_p_ugswitch;  //地下参考面节点

	glbDouble mpr_south;												//南边界
	glbDouble mpr_north;												//北边界
	glbDouble mpr_west;													//西边界
	glbDouble mpr_east;													//东边界
	CRITICAL_SECTION mpr_tilebuffer_exchange_criticalsection;			//数据交换临界区
	glbBool      mpr_showFilter;
};

CGlbGlobeUGPlane::CGlbGlobeUGPlane(CGlbGlobe* pglobe)
{
	mpr_UGPlane = new osg::Group;
	mpr_p_ugPlaneCallback = new CGlbUGPlaneCallBack(pglobe);
	mpr_UGPlane->setUpdateCallback(mpr_p_ugPlaneCallback);
	osg::Switch* sw = ((CGlbUGPlaneCallBack*)mpr_p_ugPlaneCallback)->getUGPlaneNode();
	mpr_UGPlane->addChild(sw);
} 

CGlbGlobeUGPlane::~CGlbGlobeUGPlane(void)
{
	mpr_UGPlane->removeUpdateCallback(mpr_p_ugPlaneCallback);
	mpr_p_ugPlaneCallback = NULL;
}

glbBool CGlbGlobeUGPlane::IsRayInterUGPlane(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos)
{
	if (!mpr_p_ugPlaneCallback)
		return false;
	return ((CGlbUGPlaneCallBack*)mpr_p_ugPlaneCallback)->IsRayInterUGPlane(ln_pt1,ln_dir,InterPos);
}

glbBool CGlbGlobeUGPlane::ScreenToUGPlaneCoordinate(glbInt32 x,glbInt32 y,glbDouble &lonOrx,glbDouble &latOry,glbDouble &altOrz)
{
	if (!mpr_p_ugPlaneCallback)
		return false;
	return ((CGlbUGPlaneCallBack*)mpr_p_ugPlaneCallback)->ScreenToUGPlaneCoordinate(x,y,lonOrx,latOry,altOrz);
}

osg::Node* CGlbGlobeUGPlane::getNode()
{
	return mpr_UGPlane;
}

void CGlbGlobeUGPlane::SetAltitude(glbDouble altitude)
{
	((CGlbUGPlaneCallBack*)mpr_p_ugPlaneCallback)->SetAltitude(altitude);
}

void GlbGlobe::CGlbGlobeUGPlane::SetShowFilter( glbBool isFilter )
{
	((CGlbUGPlaneCallBack*)mpr_p_ugPlaneCallback)->SetShowFilter(isFilter);
}
