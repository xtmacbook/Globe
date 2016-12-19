#include "StdAfx.h"
#include "GlbGlobeSceneObjIdxManager.h"
#include "GlbGlobeOctree.h"
#include "GlbGlobeQtree.h"
#include "GlbGlobeMath.h"
#include <osg/BoundingBox>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/PolytopeIntersector>


#define TILE_DEGREE	36 // 定义与地形一样的分块原则 36度*36度

using namespace GlbGlobe;

// 核心计算代码
void _findNodes( osg::Vec3d ln_pt1,osg::Vec3d ln_pt2, std::vector<glbref_ptr<CGlbGlobeObject>>& result, CGlbGlobeOctree *octant );

void _findNodes( osg::Polytope& polytope, std::vector<glbref_ptr<CGlbGlobeObject>>& result, CGlbGlobeOctree *octant);

void _findNodes( osg::Vec3d point_pos, std::vector<glbref_ptr<CGlbGlobeObject>>& result, CGlbGlobeQtree *octant , double nearest_distance, GlbGlobeTypeEnum globeTye);

void _findNodes( double west, double east,double south, double north, std::vector<glbref_ptr<CGlbGlobeObject>>& result, CGlbGlobeQtree *octant);

void _findNodes( CGlbAxisAlignedBox& bbox, std::vector<glbref_ptr<CGlbGlobeObject>>& result, CGlbGlobeOctree *octant);

bool intersectAndClip(osg::Vec3d& s, osg::Vec3d& e,const osg::BoundingBox& bbInput);


enum Intersection
{
	OUTSIDE=0,
	INSIDE=1,
	INTERSECT=2
};

Intersection intersect( osg::Vec3d ln_pt1, osg::Vec3d ln_pt2, CGlbAxisAlignedBox &two );


CGlbGlobeSceneObjIdxManager::CGlbGlobeSceneObjIdxManager(const WCHAR* name, int max_depth, CGlbExtent* bound)
{
	mpr_max_depth = max_depth;
	mpr_name = name;
	mpr_b_globe_type = true;
	if (bound==NULL)// || bound->IsValid()==false)
		mpr_p_extent = NULL;
	else
	{	
		double minX,maxX,minY,maxY,minZ,maxZ;
		bound->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);
		minZ = min(minZ,-20000);
		maxZ = max(maxZ,20000);
		mpr_p_extent = new CGlbExtent(minX,maxX,minY,maxY,minZ,maxZ);
		mpr_b_globe_type = false;
	}
	//wcscpy_s(mpr_name,_countof(mpr_name),name);
	InitializeCriticalSection(&mpr_objects_criticalsection);//对象操作邻接区

	mpr_onterrain_objects_count = 0;
	mpr_globe_top_octree = NULL;
	mpr_globe_top_qtree = NULL;
}


CGlbGlobeSceneObjIdxManager::~CGlbGlobeSceneObjIdxManager(void)
{
	// 析构
	std::map<long,CGlbGlobeOctree*>::iterator itr = mpr_map_octrees.begin();
	while(itr != mpr_map_octrees.end())
	{
		delete itr->second;
		itr->second = NULL;
		itr++;
	}
	mpr_map_octrees.clear();
	std::map<long,CGlbGlobeQtree*>::iterator qitr = mpr_map_qtrees.begin();
	while(qitr != mpr_map_qtrees.end())
	{
		delete qitr->second;
		qitr->second = NULL;
		qitr++;
	}
	mpr_map_qtrees.clear();
	if(mpr_globe_top_octree) delete mpr_globe_top_octree;
	if(mpr_globe_top_qtree)  delete mpr_globe_top_qtree;
	if(mpr_p_extent)         delete mpr_p_extent;
	mpr_globe_top_octree = NULL;
	mpr_globe_top_qtree = NULL;
	DeleteCriticalSection(&mpr_objects_criticalsection);
}

void CGlbGlobeSceneObjIdxManager::AddObject(CGlbGlobeRObject* obj)
{
	if ( !obj ) return;
	if(obj->IsDestroy())return;

	GlbAltitudeModeEnum altmode = obj->GetAltitudeMode();
	BOOL is_on_terrain = (altmode==GLB_ALTITUDEMODE_ONTERRAIN) ? TRUE : FALSE;

	bool isWorld = (is_on_terrain==TRUE) ? false : true;
	glbref_ptr<CGlbExtent> pExtent = obj->GetBound(isWorld);
	if (pExtent==NULL)
		return;

	EnterCriticalSection(&mpr_objects_criticalsection);

	double mx,my,mz,Mx,My,Mz;
	pExtent->GetMin(&mx,&my,&mz);
	pExtent->GetMax(&Mx,&My,&Mz);
	CGlbAxisAlignedBox objBox(mx,my,mz,Mx,My,Mz);

	if (is_on_terrain==TRUE)
	{// obj贴地形绘制
		if (mpr_p_extent==NULL)
		{// GLB_GLOBE模式
			osg::Vec3 centerpos = objBox.GetCenter();
			long longitude_cnt = 360 / TILE_DEGREE;

			// 只有x,y有意义，x代表经度，y代表维度.单位:度.  x范围[-180.0，180.0] y范围[-90.0，90.0]
			long row = (centerpos.y()-(-90)) / TILE_DEGREE;
			long col = (centerpos.x()-(-180)) / TILE_DEGREE;
			long key = (row * longitude_cnt) + col;
			std::map<long,CGlbGlobeQtree*>::iterator itr_find = mpr_map_qtrees.find(key);
			CGlbGlobeQtree* pQtree = NULL;
			if (itr_find==mpr_map_qtrees.end())
			{// 没有创建，新建之
				pQtree = new CGlbGlobeQtree(0);
				pQtree->mpr_box = GetRootOctreeBoundBoxFromPosition(col,row,TRUE);
				mpr_map_qtrees.insert(pair<long,CGlbGlobeQtree*>(key,pQtree));
			}
			else{
				pQtree = itr_find->second;
			}			
			// 获取pQtree的延展后的外包[延展了1/2]
			CGlbAxisAlignedBox obox;
			pQtree->GetBoundBox(&obox);

			// 判断objBox是否在obox内
			bool isContains = obox.Contains2D(objBox);

			if (isContains)
				_addQtreeObject(obj,pQtree);			
			else
			{// 扩展后的octree外包还是不能将_tempbox包含的话，将obj放到mpr_globe_top_octree中
				if (mpr_globe_top_qtree==NULL)
				{
					mpr_globe_top_qtree = new CGlbGlobeQtree(0);
					mpr_globe_top_qtree->mpr_box.SetExtents(-180.0,-90.0,-5000.0,180.0,90.0,5000);
				}
				mpr_globe_top_qtree->AddObject(obj);
			}
			// 计数加一
			mpr_onterrain_objects_count++;
		}
		else
		{// GLB_FLAT模式
			CGlbGlobeQtree* pQtree = NULL;
			long sz = mpr_map_qtrees.size();
			if (sz<=0){				
				long key = 0;
				pQtree = new CGlbGlobeQtree(0);
				double minX,maxX,minY,maxY,minZ,maxZ;
				mpr_p_extent->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);
				pQtree->mpr_box.SetExtents(minX,minY,minZ,maxX,maxY,maxZ); 
				mpr_map_qtrees.insert(pair<long,CGlbGlobeQtree*>(key,pQtree));
			}
			else{
				if (sz == 1){
					std::map<long,CGlbGlobeQtree*>::iterator itr = mpr_map_qtrees.begin();
					pQtree = itr->second;					
				}
				else{
					int _error = 1;
				}
			}

			if (pQtree)
			{				
				if (pQtree->mpr_box.Intersects(objBox))//只处理与区域相交的情况
					_addQtreeObject(obj,pQtree);	
			}
		}
	}
	else
	{// 非贴地形绘制和默认方式时
		if (mpr_p_extent==NULL)
		{// GLB_GLOBE模式
			glbref_ptr<CGlbExtent> pGeoExtent = obj->GetBound(false);
			// 先根据obj的中心点判断它应该落在【5*10】的mpr_map_octrees中的哪一个子八叉树中
			osg::Vec3d centerpos;
			pGeoExtent->GetCenter(&centerpos.x(),&centerpos.y(),&centerpos.z());
			long longitude_cnt = 360 / TILE_DEGREE;
			// 只有x,y有意义，x代表经度，y代表维度.单位:度.  x范围[-180.0，180.0] y范围[-90.0，90.0]
			long row = (centerpos.y()-(-90)) / TILE_DEGREE;
			long col = (centerpos.x()-(-180)) / TILE_DEGREE;
			long key = (row * longitude_cnt) + col;
			std::map<long,CGlbGlobeOctree*>::iterator itr_find = mpr_map_octrees.find(key);
			CGlbGlobeOctree* pOctree = NULL;
			if (itr_find==mpr_map_octrees.end())
			{// 没有创建，新建之
				pOctree = new CGlbGlobeOctree(0);
				pOctree->mpr_box = GetRootOctreeBoundBoxFromPosition(col,row);			
				mpr_map_octrees.insert(pair<long,CGlbGlobeOctree*>(key,pOctree));
			}else{
				pOctree = itr_find->second;
			}
			// 获取pOctree的延展后的外包[延展了1/2]
			CGlbAxisAlignedBox obox;
			pOctree->GetBoundBox(&obox);

			// 判断objBox是否在obox内
			bool isContains = obox.Contains(objBox);

			if (isContains)
				_addOctreeObject(obj,pOctree);		
			else
			{// 扩展后的octree外包还是不能将_tempbox包含的话，将obj放到mpr_globe_top_octree中
				if (mpr_globe_top_octree==NULL)
				{
					mpr_globe_top_octree = new CGlbGlobeOctree(0);
					mpr_globe_top_octree->mpr_box.SetExtents(-osg::WGS_84_RADIUS_EQUATOR,-osg::WGS_84_RADIUS_EQUATOR,-osg::WGS_84_RADIUS_EQUATOR,
								osg::WGS_84_RADIUS_EQUATOR,osg::WGS_84_RADIUS_EQUATOR,osg::WGS_84_RADIUS_EQUATOR);
				}
				mpr_globe_top_octree->AddObject(obj);
			}					
		}
		else
		{// GLB_FLAT模式
			CGlbGlobeOctree* pOctree = NULL;
			long sz = mpr_map_octrees.size();
			if(sz<=0){
				long key = 0;
				pOctree = new CGlbGlobeOctree(0);
				double minX,maxX,minY,maxY,minZ,maxZ;
				mpr_p_extent->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);
				pOctree->mpr_box.SetExtents(minX,minY,minZ,maxX,maxY,maxZ); 
				mpr_map_octrees.insert(pair<long,CGlbGlobeOctree*>(key,pOctree));
			}
			else{
				if (sz==1){
					std::map<long,CGlbGlobeOctree*>::iterator itr = mpr_map_octrees.begin();
					pOctree = itr->second;
				}				
				else
				{// 错误
					int _error = 1;
				}
			}

			if (pOctree!=NULL){			
				if (pOctree->mpr_box.Intersects(objBox))//只处理与区域相交的情况
					_addOctreeObject(obj,pOctree);		
			}
		}
	}
	LeaveCriticalSection(&mpr_objects_criticalsection);

}

void CGlbGlobeSceneObjIdxManager::RemoveObject(CGlbGlobeRObject* obj)
{
	if (obj==NULL) return;

	GlbAltitudeModeEnum altmode = obj->GetAltitudeMode();
	BOOL is_on_terrain = (altmode==GLB_ALTITUDEMODE_ONTERRAIN) ? TRUE : FALSE;

	EnterCriticalSection(&mpr_objects_criticalsection);

	_removeObject(obj , (is_on_terrain==TRUE) ? TRUE : FALSE);

	// 计数减一
	if (is_on_terrain==TRUE){
		mpr_onterrain_objects_count--;
	}
	LeaveCriticalSection(&mpr_objects_criticalsection);
}

void CGlbGlobeSceneObjIdxManager::UpdateObject(CGlbGlobeRObject* obj)
{
	if (obj==NULL) return;

	GlbAltitudeModeEnum altmode = obj->GetAltitudeMode();
	BOOL is_on_terrain = (altmode==GLB_ALTITUDEMODE_ONTERRAIN) ? TRUE : FALSE;

	// 从管理树中删除object
	RemoveObject( obj );
	// 添加object	
	AddObject( obj );

}

bool CGlbGlobeSceneObjIdxManager::Query(osg::Vec3d ln_pt1, osg::Vec3d ln_pt2, std::vector<glbref_ptr<CGlbGlobeObject>>& result,std::vector<osg::Vec3d>& inter_pts,bool query_crudeAndquickly)
{
	if (ln_pt1.valid()==false || ln_pt2.valid()==false)
	{
		return false;
	}

	if (query_crudeAndquickly)
	{// 粗选 - 只用包围盒求交
		EnterCriticalSection(&mpr_objects_criticalsection);
		// 粗交
		if (mpr_globe_top_octree)
			_findNodes(ln_pt1,ln_pt2,result,mpr_globe_top_octree);

		std::map<long,CGlbGlobeOctree*>::iterator itr = mpr_map_octrees.begin();
		while(itr != mpr_map_octrees.end())
		{
			_findNodes(ln_pt1,ln_pt2,result,itr->second);
			itr++;
		}
		LeaveCriticalSection(&mpr_objects_criticalsection);
	}
	else
	{// 精细求交 - 两次求交
		EnterCriticalSection(&mpr_objects_criticalsection);
		// 粗交
		std::vector<glbref_ptr<CGlbGlobeObject>> _rr;
		if (mpr_globe_top_octree)
			_findNodes(ln_pt1,ln_pt2,_rr,mpr_globe_top_octree);
		
		std::map<long,CGlbGlobeOctree*>::iterator itr = mpr_map_octrees.begin();
		while(itr != mpr_map_octrees.end())
		{
			_findNodes(ln_pt1,ln_pt2,_rr,itr->second);
			itr++;
		}
		LeaveCriticalSection(&mpr_objects_criticalsection);

		//osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);
		// 求交方法三：(用世界坐标值，既可在某个节点node下求交，也可在相机下求交。但需注意，在某个节点node下求交时，需要把该node的有矩阵变换的父节点
		// 都用上，比如根节点->MT节点->cow模型节点，则可用MT节点->accept( iv)或根节点->accept( iv)。求交时是根据执行accept( iv)的节点向下遍历，
		//求出节点的真实世界坐标。而如果用cow->accept( iv) ，则会忽略父节点的MT节点，导致求不出真正的世界坐标值，这样求交会产生错误)
		/*osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(ln_pt1, ln_pt2);
		osgUtil::IntersectionVisitor iv(picker.get());*/

		// 精细求交osgUtil::LineSegmentIntersector
		std::vector<double> dis_from_eye; // 临时数组，存储交点距离ln_pt1的距离
		for (long k = 0; k < (long)_rr.size(); k++)
		{
			CGlbGlobeRObject* pRenderObj = dynamic_cast<CGlbGlobeRObject*>(_rr.at(k).get());
			bool bSel = _isObjectHaveSelected(pRenderObj,result);
			if (bSel)// 已经选中了，不必重复 
				continue;
			
			 
			double gridTime = 0.0;
			double osgTime = 0.0;
			if (pRenderObj && pRenderObj->IsShow() && pRenderObj->IsSelectEnable())
			{// 只对可见对象求交且至于只与可选中对象求交
				
				osg::ref_ptr<osg::Node> pOsgNode = pRenderObj->GetOsgNode();	
				if(pOsgNode.valid())
				{
					osg::Node::NodeMask nmask = pOsgNode->getNodeMask();
					if (nmask == 0x01)// 不允许选择
						continue;
					//osg::Vec3d gridI;
					//osg::Vec3d osgI;
					if ((pRenderObj->GetType()==GLB_OBJECTTYPE_TIN))
					{
						//osg::Timer_t startTime = osg::Timer::instance()->tick();
						osg::Vec3d hitPt;
						glbBool isInter = pRenderObj->interset(ln_pt1,ln_pt2,hitPt);
						if (isInter)
						{// 有相交情况
							double dis = (ln_pt1-hitPt).length();
							// 根据离ln_pt1的距离由近到远排序
							if (result.size()==0)
							{
								result.push_back(_rr.at(k));
								dis_from_eye.push_back(dis);
								inter_pts.push_back(hitPt);
							}
							else
							{
								bool done = false;
								for(long m = 0; m < (long)dis_from_eye.size(); m++)
								{
									if (dis < dis_from_eye.at(m))
									{
										dis_from_eye.insert(dis_from_eye.begin()+m,dis);
										result.insert(result.begin()+m,_rr.at(k));
										inter_pts.insert(inter_pts.begin()+m,hitPt);							
										done = true;
										break;
									}
								}
								if (!done)
								{
									result.push_back(_rr.at(k));
									dis_from_eye.push_back(dis);
									inter_pts.push_back(hitPt);
								}
							}
						}
						//gridI = hitPt;
						//osg::Timer_t afterTime = osg::Timer::instance()->tick();

						//gridTime += osg::Timer::instance()->delta_m(startTime,afterTime);
					}
					else
					{
						//osg::Timer_t startTime = osg::Timer::instance()->tick();
						osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(ln_pt1, ln_pt2);
						osgUtil::IntersectionVisitor iv(picker.get());
						pOsgNode->accept(iv);
						if (picker->containsIntersections())
						{
							osgUtil::LineSegmentIntersector::Intersections& intersections = picker->getIntersections();
							const osgUtil::LineSegmentIntersector::Intersection& hit = *(intersections.begin());
							osg::Vec3d hitPt = hit.getWorldIntersectPoint();
							double dis = (ln_pt1-hitPt).length();
							// 根据离ln_pt1的距离由近到远排序
							if (result.size()==0)
							{
								result.push_back(_rr.at(k));
								dis_from_eye.push_back(dis);
								inter_pts.push_back(hitPt);
							}
							else
							{
								bool done = false;
								for(long m = 0; m < (long)dis_from_eye.size(); m++)
								{
									if (dis < dis_from_eye.at(m))
									{
										dis_from_eye.insert(dis_from_eye.begin()+m,dis);
										result.insert(result.begin()+m,_rr.at(k));
										inter_pts.insert(inter_pts.begin()+m,hitPt);							
										done = true;
										break;
									}
								}
								if (!done)
								{
									result.push_back(_rr.at(k));
									dis_from_eye.push_back(dis);
									inter_pts.push_back(hitPt);
								}
							}					
							//osgI = hitPt;
						}

						//osg::Timer_t afterTime = osg::Timer::instance()->tick();
						//osgTime += osg::Timer::instance()->delta_m(startTime,afterTime);
					}
				}
			}	
		}
	}	

	/*WCHAR wBuff[256];
	WCHAR wBuff2[256];
	swprintf(wBuff,L"Grid Times : %.4f \n",gridTime);  
	swprintf(wBuff2,L"OSG Times : %.4f \n",osgTime);  
	OutputDebugString(wBuff);
	OutputDebugString(wBuff2);*/

	if (result.size() > 0)
		return true;
	return false;
}

bool CGlbGlobeSceneObjIdxManager::Query(osg::Polytope& polytope,std::vector<glbref_ptr<CGlbGlobeObject>>& result,bool query_crudeAndquickly)
{
	EnterCriticalSection(&mpr_objects_criticalsection);
	//if (mpr_p_extent==NULL){// GLB_GLOBE模式
	// 适合GLB_GLOBE和GLB_FLAT两种模式
	// 粗交
	if (!query_crudeAndquickly)
	{// 精细求交 - 
		std::vector<glbref_ptr<CGlbGlobeObject>> _rr;
		if (mpr_globe_top_octree)
			_findNodes(polytope,_rr,mpr_globe_top_octree);

		std::map<long,CGlbGlobeOctree*>::iterator itr = mpr_map_octrees.begin();
		while(itr != mpr_map_octrees.end())
		{
			//_findNodes(polytope,result,itr->second);
			_findNodes(polytope,_rr,itr->second);
			itr++;
		}

		osg::ref_ptr< osgUtil::PolytopeIntersector > picker = new osgUtil::PolytopeIntersector(polytope);
		osgUtil::IntersectionVisitor iv(picker.get());

		// 精细求交osgUtil::PolytopeIntersector	
		for (long k = 0; k < (long)_rr.size(); k++)
		{
			CGlbGlobeRObject* pRenderObj = dynamic_cast<CGlbGlobeRObject*>(_rr.at(k).get());
			if (mpr_p_extent==NULL)
			{// GLB_GLOBE模式需要去重复????GLB_FLAT模式暂时不确定
				bool bSel = _isObjectHaveSelected(pRenderObj,result);
				if (bSel)// 已经选中了，不必重复
					continue;
			}
			if (pRenderObj && pRenderObj->IsShow() && pRenderObj->IsSelectEnable())
			{// 只对可见对象求交,并且只与可选择对象求交
				osg::ref_ptr<osg::Node> pOsgNode = pRenderObj->GetOsgNode();
				
				if (pOsgNode){
					pOsgNode->accept(iv);
					if (picker->containsIntersections())
					{// 不需要考虑距离
						result.push_back(_rr.at(k));							
					}
				}
			}	
		}	
	}
	else
	{// 包围盒求交 - 粗交
		if (mpr_globe_top_octree)
			_findNodes(polytope,result,mpr_globe_top_octree);

		std::map<long,CGlbGlobeOctree*>::iterator itr = mpr_map_octrees.begin();
		while(itr != mpr_map_octrees.end())
		{
			_findNodes(polytope,result,itr->second);			
			itr++;
		}		
	}
	LeaveCriticalSection(&mpr_objects_criticalsection);

	if (result.size() > 0)
		return true;
	return false;
}

long CGlbGlobeSceneObjIdxManager::GetKeyFromPosition(long latitude, long longitude)
{
	long longitude_cnt = 360 / TILE_DEGREE;
	long curRow = (latitude-(-90)) / TILE_DEGREE;
	long curCol = (longitude-(-180)) / TILE_DEGREE;
	long key = (curRow * longitude_cnt) + curCol;

	return key;
}

//CGlbAxisAlignedBox CGlbGlobeOctreeSceneManager::GetRootOctreeBoundBoxFromPosition(long latitude, long longitude)
CGlbAxisAlignedBox CGlbGlobeSceneObjIdxManager::GetRootOctreeBoundBoxFromPosition(long col, long row, BOOL is_on_terrain)
{
	//long longitude_cnt = 360 / TILE_DEGREE;
	//long curRow = (latitude-(-90)) / TILE_DEGREE;
	//long curCol = (longitude-(-180)) / TILE_DEGREE;

	double west = -180.0f + col * TILE_DEGREE;	
	double east = west + TILE_DEGREE;	
	double south = -90.0f + row * TILE_DEGREE;
	double north = south + TILE_DEGREE;

	// 逼近法求包围盒[不能简单的拿四个边角点来判断]
	osg::BoundingBox bb; 
	std::vector<osg::Vec3d> pointvec;
	double _center_longitude = (east + west) * 0.5 ;
	double _center_latitude = (north + south) * 0.5 ;

	pointvec.push_back(osg::Vec3d(_center_longitude,_center_latitude,-5000));
	pointvec.push_back(osg::Vec3d(_center_longitude,_center_latitude,5000));
	long _details = 43;//43//32 //5 //3
	double latitudespan = (north-south) / (_details-1);
	double longitudespan = (east-west) / (_details-1);
	for (long i = 0; i < _details; i++)
	{
		pointvec.push_back(osg::Vec3d(west,south+i*latitudespan,-5000.0));
		pointvec.push_back(osg::Vec3d(west,south+i*latitudespan,5000.0));
		pointvec.push_back(osg::Vec3d(east,south+i*latitudespan,-5000.0));
		pointvec.push_back(osg::Vec3d(east,south+i*latitudespan,5000.0));
		pointvec.push_back(osg::Vec3d(west+i*longitudespan,south,-5000.0));
		pointvec.push_back(osg::Vec3d(west+i*longitudespan,south,5000.0));
		pointvec.push_back(osg::Vec3d(west+i*longitudespan,north,-5000.0));
		pointvec.push_back(osg::Vec3d(west+i*longitudespan,north,5000.0));
	}
	osg::Vec3d point;
	for(long i =0;i<(long)pointvec.size();i++)
	{		
		if (is_on_terrain)
		{			
			bb.expandBy(pointvec.at(i).x(),pointvec.at(i).y(),pointvec.at(i).z());
		}
		else
		{
			g_ellipsmodel->convertLatLongHeightToXYZ(
				osg::DegreesToRadians( pointvec[i].y() ),
				osg::DegreesToRadians( pointvec[i].x() ),
				pointvec[i].z(),
				point.x(), point.y(), point.z() );		
			bb.expandBy(point.x(),point.y(),point.z());
		}		
	}  

	return CGlbAxisAlignedBox(bb.xMin(),bb.yMin(),bb.zMin(),bb.xMax(),bb.yMax(),bb.zMax());
}

void CGlbGlobeSceneObjIdxManager::_addOctreeObject(CGlbGlobeObject* n,CGlbGlobeOctree* octant, long depth)
{
	CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(n);
	if (!robj)return;

	glbref_ptr<CGlbExtent> _extent = robj->GetBound();
	if (!_extent)
		return;
	double mx,my,mz,Mx,My,Mz;
	_extent->GetMin(&mx,&my,&mz);
	_extent->GetMax(&Mx,&My,&Mz);
	CGlbAxisAlignedBox bx(mx,my,mz,Mx,My,Mz);

	//if the octree is twice as big as the scene node,
	//we will add it to a child.
	if ( ( depth < mpr_max_depth ) && octant -> IsTwiceSize( bx ) )
	{
		long x, y, z;
		octant -> GetChildIndexes( bx, &x, &y, &z );

		if ( octant -> mpr_p_children[ x ][ y ][ z ] == 0 )
		{
			octant -> mpr_p_children[ x ][ y ][ z ] = new CGlbGlobeOctree ( octant );
			osg::Vec3& octantMin = octant -> mpr_box.GetMinimum();
			osg::Vec3& octantMax = octant -> mpr_box.GetMaximum();			

			float minx,miny,minz,maxx,maxy,maxz;
			if ( x == 0 )
			{
				minx = octantMin.x();
				maxx = ( octantMin.x() + octantMax.x() ) / 2;
			}

			else
			{
				minx = ( octantMin.x() + octantMax.x() ) / 2;
				maxx = octantMax.x();
			}

			if ( y == 0 )
			{
				miny = octantMin.y();
				maxy = ( octantMin.y() + octantMax.y() ) / 2;
			}

			else
			{
				miny = ( octantMin.y() + octantMax.y() ) / 2;
				maxy = octantMax.y();
			}

			if ( z == 0 )
			{
				minz = octantMin.z();
				maxz = ( octantMin.z() + octantMax.z() ) / 2;
			}

			else
			{
				minz = ( octantMin.z() + octantMax.z() ) / 2;
				maxz = octantMax.z();
			}

			osg::Vec3 _min(minx,miny,minz);
			osg::Vec3 _max(maxx,maxy,maxz);

			octant -> mpr_p_children[ x ][ y ][ z ] -> mpr_box.SetExtents( _min, _max );
			octant -> mpr_p_children[ x ][ y ][ z ] -> mpr_halfsize = ( _max - _min ) / 2;
		}

		_addOctreeObject( n, octant -> mpr_p_children[ x ][ y ][ z ], ++depth );

	}

	else
	{
		octant -> AddObject( n );
	}

}

void CGlbGlobeSceneObjIdxManager::_removeObject(CGlbGlobeObject * n , BOOL is_on_terrain )
{
	//// Skip if octree has been destroyed (shutdown conditions)
	CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(n);
	if (!robj)
		return;

	if (is_on_terrain)
	{
		CGlbGlobeQtree * qt = (CGlbGlobeQtree *)(robj->GetQtree());

		if ( qt )
		{
			qt -> RemoveObject( n );
		}

		robj->SetQtree(0);
	}
	else
	{
		CGlbGlobeOctree * oct = (CGlbGlobeOctree *) (robj -> GetOctree());

		if ( oct )
		{
			oct -> RemoveObject( n );
		}

		robj->SetOctree(0);
	}
}

bool CGlbGlobeSceneObjIdxManager::_isObjectHaveSelected(CGlbGlobeObject* obj, std::vector<glbref_ptr<CGlbGlobeObject>>& result)
{
	if (obj==NULL)	return true;

	if (result.size()<=0) return false;

	std::vector<glbref_ptr<CGlbGlobeObject>>::iterator itr = result.begin();
	while(itr != result.end())
	{
		CGlbGlobeObject* ro = itr->get();
		if (!ro) continue;
		if (obj == ro)
			return true;
		itr++;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
Intersection intersect( osg::Vec3d ln_pt1, osg::Vec3d ln_pt2, CGlbAxisAlignedBox &two )
{
	// Null box?
	if (two.IsNull()) return OUTSIDE;
	// Infinite box?
	if (two.IsInfinite()) return INTERSECT;

	bool inside = true;
	osg::Vec3& twoMin = two.GetMinimum();
	osg::Vec3& twoMax = two.GetMaximum();
	osg::Vec3 origin = ln_pt1;

	osg::Vec3 dir = ln_pt2 - ln_pt1;
	dir.normalize();

	osg::Vec3 maxT(-1, -1, -1);

	int i = 0;
	for(i=0; i<3; i++ )
	{
		if( origin[i] < twoMin[i] )
		{
			inside = false;
			if( dir[i] > 0 )
			{
				maxT[i] = (twoMin[i] - origin[i])/ dir[i];
			}
		}
		else if( origin[i] > twoMax[i] )
		{
			inside = false;
			if( dir[i] < 0 )
			{
				maxT[i] = (twoMax[i] - origin[i]) / dir[i];
			}
		}
	}

	if( inside )
	{
		return INTERSECT;
	}
	int whichPlane = 0;
	if( maxT[1] > maxT[whichPlane])
		whichPlane = 1;
	if( maxT[2] > maxT[whichPlane])
		whichPlane = 2;

	if( ((int)maxT[whichPlane]) & 0x80000000 )
	{
		return OUTSIDE;
	}
	for(i=0; i<3; i++ )
	{
		if( i!= whichPlane )
		{
			float f = origin[i] + maxT[whichPlane] * dir[i];
			if ( f < (twoMin[i] - 0.00001f) ||
				f > (twoMax[i] +0.00001f ) )
			{
				return OUTSIDE;
			}
		}
	}

	return INTERSECT;

}

void _findNodes( double west, double east,double south, double north, std::vector<glbref_ptr<CGlbGlobeObject>>& result, CGlbGlobeQtree *octant)
{
	CGlbAxisAlignedBox obox;
	octant -> GetBoundBox( &obox );
	osg::Vec3 _center = obox.GetCenter();

	CGlbAxisAlignedBox bb(west,south,_center.z(),east,north,_center.z());
	if (obox.Intersects2D(bb)==false)
		return ;

	CGlbGlobeOctree::ObjectList::iterator it = octant->mpr_objects.begin();
	while ( it != octant->mpr_objects.end() )
	{
		CGlbGlobeObject * n = ( *it );
		glbref_ptr<CGlbGlobeRObject> robj = dynamic_cast<CGlbGlobeRObject*>(n);
		if (!robj)// 空指针
		{++it;continue;}
		// 不可见对象
		if (robj->IsShow()==false)
		{++it;continue;}
		// 不可选中对象
		if (robj->IsSelectEnable()==false)
		{++it;continue;}

		bool isWorld = false;		
		//CGlbExtent* _extent = robj->GetBound(isWorld);
		glbref_ptr<IGlbGeometry> outlineGeom = robj->GetOutLine();		
		glbref_ptr<CGlbExtent> _extent = NULL;
		if (outlineGeom)		
			_extent = const_cast<CGlbExtent*>(outlineGeom->GetExtent());		
		else		
			_extent = robj->GetBound(isWorld);
		
		if (_extent)
		{
			double mx,my,mz,Mx,My,Mz;
			_extent->GetMin(&mx,&my,&mz);
			_extent->GetMax(&Mx,&My,&Mz);
			CGlbAxisAlignedBox _tempbox(mx,my,mz,Mx,My,Mz);

			if (bb.Intersects2D(_tempbox) )
			{
				result.push_back( robj );	
			}		
		}	
		
		++it;
	}

	CGlbGlobeQtree* child;

	if ( (child=octant -> mpr_p_children[ 0 ][ 0 ]) != 0 )
		_findNodes( west,east,south,north, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 0 ]) != 0 )
		_findNodes( west,east,south,north, result, child );

	if ( (child=octant -> mpr_p_children[ 0 ][ 1 ]) != 0 )
		_findNodes( west,east,south,north, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 1 ]) != 0 )
		_findNodes( west,east,south,north, result, child );
}

#include "GlbPoint.h"
#include "GlbLine.h"
#include "GlbPolygon.h"

void _findNodes( osg::Vec3d point_pos, std::vector<glbref_ptr<CGlbGlobeObject>>& result, CGlbGlobeQtree *octant, double nearest_distance,GlbGlobeTypeEnum globeTye)
{
	CGlbAxisAlignedBox obox;
	octant -> GetBoundBox( &obox );

	osg::Vec3 pos = obox.GetCenter();
	pos.set(point_pos.x(),point_pos.y(),pos.z());

	if (obox.Contains2D(pos)==false)
		return;

	// 考虑经纬形变
	double meterlonByDegree = 1.0;
	double meterlatByDegree = 1.0;
	if(globeTye == GLB_GLOBETYPE_GLOBE)
	{// 球模式时，需要纠正变形，否则会出现画圆变成椭圆的现象
		// 纬度1度相当于距离多少米
		meterlatByDegree = osg::PI * g_ellipsmodel->getRadiusPolar() / 180;
		double loncircle = cos(osg::DegreesToRadians(point_pos.y())) * g_ellipsmodel->getRadiusEquator();
		// 经度1度相当于距离多少米
		meterlonByDegree =  osg::PI * loncircle / 180; 
	}

	CGlbGlobeOctree::ObjectList::iterator it = octant->mpr_objects.begin();
	while ( it != octant->mpr_objects.end() )
	{
		CGlbGlobeObject * n = ( *it );
		CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(n);
		if (!robj)// 空指针
		{++it;continue;}
		// 不可见
		if (robj->IsShow()==false)
		{++it;continue;}
		// 没有outline，不做精细求交
		if ( robj->GetOutLine() == NULL)
		{++it;continue;}
		// 不可选中对象
		if (robj->IsSelectEnable()==false)
		{++it;continue;}

		CGlbExtent* _extent = const_cast<CGlbExtent*>(robj->GetOutLine()->GetExtent());
		glbDouble mx,my,mz,Mx,My,Mz;
		_extent->GetMin(&mx,&my,&mz);
		_extent->GetMax(&Mx,&My,&Mz);
		if (mx <= pos.x() && pos.x() <= Mx &&
			my <= pos.y() && pos.y() <= My )
		{// 在rect范围内，进一步用objecet的outline来精细判断是否选中
			glbref_ptr<IGlbGeometry> p_glbGeom = robj->GetOutLine();
			if (!p_glbGeom)
			{ ++it;continue; }

			glbDouble ptx,pty;
			CGlbMultiPoint* p_pts = dynamic_cast<CGlbMultiPoint*> (p_glbGeom.get());			
			CGlbMultiLine* p_lns =  dynamic_cast<CGlbMultiLine*> (p_glbGeom.get());
			CGlbMultiPolygon* p_polys = dynamic_cast<CGlbMultiPolygon*> (p_glbGeom.get());			
			glbDouble FAZHI = nearest_distance;//距离阀值
			if (p_pts)
			{// 点状地物				
				glbInt32 ptCnt = p_pts->GetCount();
				for (glbInt32 i = 0 ; i <ptCnt; i++)
				{
					p_pts->GetPoint(i,&ptx,&pty);
					glbDouble dis = sqrt((point_pos.x()-ptx)*(point_pos.x()-ptx)*meterlonByDegree*meterlonByDegree 
						+ (point_pos.y()-pty)*(point_pos.y()-pty)*meterlatByDegree*meterlatByDegree);
					if (dis<FAZHI)//两点间距离小于阀值[这里没有区分球面和平面，有待验证和商榷]
					{
						result.push_back( robj );
						break;
					}
				}				
			}			
			if (p_lns)
			{// 线状地物
				glbInt32 lnCnt = p_lns->GetCount();
				for (glbInt32 i = 0 ; i <lnCnt; i++)
				{
					CGlbLine* ln = const_cast<CGlbLine*>(p_lns->GetLine(i));
					glbInt32 ptCnt = ln->GetCount();
					glbDouble x0,x1,y0,y1;
					ln->GetPoint(0,&x0,&y0);
					ln->GetPoint(ptCnt-1,&x1,&y1);
					glbDouble closeDis = sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1));					
					if (closeDis<0.001)
					{//闭合线
						glbDouble* _pts = (glbDouble*)ln->GetPoints();	
						glbInt32 coordDimension = ln->GetCoordDimension();
						bool bInPoly = PtInPolygon(point_pos.x(),point_pos.y(),_pts,ptCnt,coordDimension);
						if (bInPoly)// 点在多边形内
						{
							result.push_back( robj );	
							break;
						}
					}
					else
					{						
						glbDouble pt1_x,pt1_y,pt2_x,pt2_y;
						double minDis = DBL_MAX;
						for (glbInt32 k = 0; k < ptCnt-1; k++)
						{
							ln->GetPoint(k,&pt1_x,&pt1_y);
							ln->GetPoint(k+1,&pt2_x,&pt2_y);

							// 此处计算有误差
							//double dis = pointToLine(pt1_x,pt1_y,pt2_x,pt2_y,point_pos.x(),point_pos.y());
							double dis = pointToLine(pt1_x,pt1_y,pt2_x,pt2_y,point_pos.x(),point_pos.y())*meterlonByDegree;
							if (minDis > dis)
								minDis = dis;
						}

						if (minDis<nearest_distance)//点到线段间的距离小于阀值
						{
							result.push_back( robj );
							break;
						}
					}
				}
				
			}		
			if (p_polys)
			{// 多边形地物
				glbInt32 polyCnt = p_polys->GetCount();
				for (glbInt32 i = 0 ; i < polyCnt; i++)
				{
					CGlbPolygon* p_poly = const_cast<CGlbPolygon*>(p_polys->GetPolygon(i));
					glbInt32 inRingCnt = p_poly->GetInRingCount();
					if(inRingCnt<=0)
					{//只有外环
						glbInt32 coordDimension = p_poly->GetCoordDimension();
						CGlbLine* ringLn = const_cast<CGlbLine*>(p_poly->GetExtRing());
						glbInt32 ptCnt = ringLn->GetCount();
						glbDouble* pts = (glbDouble*) ringLn->GetPoints();
						bool bInPoly = PtInPolygon(point_pos.x(),point_pos.y(),pts,ptCnt,coordDimension);
						if (bInPoly)// 点在多边形内
							result.push_back( robj );	
					}
					else
					{//有内环嵌套的情况 - 点落在奇数个简单多边形内则在表示点在这些简单多边形组成的复杂多边形内;反之如果是偶数，则表示在复杂多边形外。
						bool isIn = false;
						glbInt32 coordDimension = p_poly->GetCoordDimension();
						CGlbLine* ringLn = const_cast<CGlbLine*>(p_poly->GetExtRing());
						glbInt32 ptCnt = ringLn->GetCount();
						glbDouble* pts = (glbDouble*) ringLn->GetPoints();
						bool bInPoly = PtInPolygon(point_pos.x(),point_pos.y(),pts,ptCnt,coordDimension);
						if (bInPoly)// 点在多边形内
							isIn = true;	
					
						for (glbInt32 k = 0; k < inRingCnt; k++)
						{
							ringLn = const_cast<CGlbLine*>(p_poly->GetInRing(k));
							ptCnt = ringLn->GetCount();
							pts = (glbDouble*) ringLn->GetPoints();
							bInPoly = PtInPolygon(point_pos.x(),point_pos.y(),pts,ptCnt,coordDimension);
							if (bInPoly)
								isIn = !isIn;
						}
						if (isIn)// 点在多边形内
							result.push_back( robj );	
					}
				}			
			}
		}		

		++it;
	}

	CGlbGlobeQtree* child;

	if ( (child=octant -> mpr_p_children[ 0 ][ 0 ]) != 0 )
		_findNodes( point_pos, result, child ,nearest_distance ,globeTye );

	if ( (child=octant -> mpr_p_children[ 1 ][ 0 ]) != 0 )
		_findNodes( point_pos, result, child ,nearest_distance , globeTye);

	if ( (child=octant -> mpr_p_children[ 0 ][ 1 ]) != 0 )
		_findNodes( point_pos, result, child ,nearest_distance , globeTye);

	if ( (child=octant -> mpr_p_children[ 1 ][ 1 ]) != 0 )
		_findNodes( point_pos, result, child ,nearest_distance , globeTye);
}

void _findNodes( osg::Vec3d ln_pt1,osg::Vec3d ln_pt2, std::vector<glbref_ptr<CGlbGlobeObject>>& result, CGlbGlobeOctree *octant )
{	
	CGlbAxisAlignedBox obox;
	octant -> GetBoundBox( &obox );

	Intersection isect = intersect( ln_pt1,ln_pt2, obox );
#if 1
	// ogre 算法	
	if ( isect == OUTSIDE )
		return ;
#else
	// osg 算法
	osg::Vec3& _min = obox.GetMinimum();
	osg::Vec3& _max = obox.GetMaximum();
	osg::BoundingBox bb;
	bb.expandBy(_min);
	bb.expandBy(_max);

	bool bOut = false;
	if ( isect == OUTSIDE )
	{
		bOut = true;
	}

	bool res = intersectAndClip(ln_pt1,ln_pt2,bb);
	if (res == false)
		return;
#endif


	CGlbGlobeOctree::ObjectList::iterator it = octant->mpr_objects.begin();

	while ( it != octant->mpr_objects.end() )
	{
		CGlbGlobeObject * n = ( *it );
		CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(n);
		if (!robj)// 空指针
		{++it;continue;}

		glbref_ptr<CGlbExtent> _extent = robj->GetBound();
		if (!_extent)
		{++it;continue;}
		double mx,my,mz,Mx,My,Mz;
		_extent->GetMin(&mx,&my,&mz);
		_extent->GetMax(&Mx,&My,&Mz);
		CGlbAxisAlignedBox bx(mx,my,mz,Mx,My,Mz);

		Intersection nsect = intersect( ln_pt1,ln_pt2,bx );

		if ( nsect != OUTSIDE )
		{
			result.push_back( robj );			
		}

		++it;
	}

	CGlbGlobeOctree* child;

	if ( (child=octant -> mpr_p_children[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( ln_pt1,ln_pt2, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( ln_pt1,ln_pt2, result, child );

	if ( (child=octant -> mpr_p_children[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( ln_pt1,ln_pt2, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( ln_pt1,ln_pt2, result, child );

	if ( (child=octant -> mpr_p_children[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( ln_pt1,ln_pt2, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( ln_pt1,ln_pt2, result, child );

	if ( (child=octant -> mpr_p_children[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( ln_pt1,ln_pt2, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( ln_pt1,ln_pt2, result, child );

}

void _findNodes( osg::Polytope& polytope, std::vector<glbref_ptr<CGlbGlobeObject>>& result, CGlbGlobeOctree *octant)
{
	CGlbAxisAlignedBox obox;
	octant -> GetBoundBox( &obox );
	osg::Vec3& minVec = obox.GetMinimum();
	osg::Vec3& maxVec = obox.GetMaximum();

	osg::BoundingBox bb(minVec,maxVec);

	if (polytope.contains(bb)==false)
		return;

	CGlbGlobeOctree::ObjectList::iterator it = octant->mpr_objects.begin();

	while ( it != octant->mpr_objects.end() )
	{
		glbref_ptr<CGlbGlobeRObject> robj = dynamic_cast<CGlbGlobeRObject*>(*it);
		if (!robj)// 空指针
		{++it;continue;}
		// 如果视锥与此八叉树节点相交，则认为节点内所有对象都与视锥相交以节省时间 2015.5.4 马林 
		//glbref_ptr<CGlbExtent> _extent = robj->GetBound();
		//if (!_extent)
		//{++it;continue;}
		//double mx,my,mz,Mx,My,Mz;
		//_extent->GetMin(&mx,&my,&mz);
		//_extent->GetMax(&Mx,&My,&Mz);

		//bb.set(mx,my,mz,Mx,My,Mz);
		//CGlbAxisAlignedBox bx(mx,my,mz,Mx,My,Mz);
		if (polytope.contains(bb))
		{
			result.push_back( robj );		
		}		

		++it;
	}

	CGlbGlobeOctree* child;

	if ( (child=octant -> mpr_p_children[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( polytope, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( polytope, result, child );

	if ( (child=octant -> mpr_p_children[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( polytope, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( polytope, result, child );

	if ( (child=octant -> mpr_p_children[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( polytope, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( polytope, result, child );

	if ( (child=octant -> mpr_p_children[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( polytope, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( polytope, result, child );
}

void _findNodes( CGlbAxisAlignedBox& bbox, std::vector<glbref_ptr<CGlbGlobeObject>>& result, CGlbGlobeOctree *octant)
{
	CGlbAxisAlignedBox obox;
	octant -> GetBoundBox( &obox );

	if (bbox.Intersects(obox)==false)
		return;

	CGlbGlobeOctree::ObjectList::iterator it = octant->mpr_objects.begin();

	while ( it != octant->mpr_objects.end() )
	{
		CGlbGlobeObject * n = ( *it );
		CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(n);
		if (!robj)// 空指针
		{++it;continue;}
		// 不可见
		if (robj->IsShow()==false)
		{++it;continue;}

		glbref_ptr<CGlbExtent> _extent = robj->GetBound();
		if (!_extent)
		{++it;continue;}
		double mx,my,mz,Mx,My,Mz;
		_extent->GetMin(&mx,&my,&mz);
		_extent->GetMax(&Mx,&My,&Mz);

		CGlbAxisAlignedBox bx(mx,my,mz,Mx,My,Mz);
		if (bbox.Intersects(bx))
		{// 粗略求交成功
			result.push_back( robj );		
		}		
		++it;
	}

	CGlbGlobeOctree* child;

	if ( (child=octant -> mpr_p_children[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( bbox, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( bbox, result, child );

	if ( (child=octant -> mpr_p_children[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( bbox, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( bbox, result, child );

	if ( (child=octant -> mpr_p_children[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( bbox, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( bbox, result, child );

	if ( (child=octant -> mpr_p_children[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( bbox, result, child );

	if ( (child=octant -> mpr_p_children[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( bbox, result, child );
}

bool intersectAndClip(osg::Vec3d& s, osg::Vec3d& e,const osg::BoundingBox& bbInput)
{
	osg::Vec3d bb_min(bbInput._min);
	osg::Vec3d bb_max(bbInput._max);

#if 1
	double epsilon = 1e-4;
	bb_min.x() -= epsilon;
	bb_min.y() -= epsilon;
	bb_min.z() -= epsilon;
	bb_max.x() += epsilon;
	bb_max.y() += epsilon;
	bb_max.z() += epsilon;
#endif

	// compate s and e against the xMin to xMax range of bb.
	if (s.x()<=e.x())
	{

		// trivial reject of segment wholely outside.
		if (e.x()<bb_min.x()) return false;
		if (s.x()>bb_max.x()) return false;

		if (s.x()<bb_min.x())
		{
			// clip s to xMin.
			s = s+(e-s)*(bb_min.x()-s.x())/(e.x()-s.x());
		}

		if (e.x()>bb_max.x())
		{
			// clip e to xMax.
			e = s+(e-s)*(bb_max.x()-s.x())/(e.x()-s.x());
		}
	}
	else
	{
		if (s.x()<bb_min.x()) return false;
		if (e.x()>bb_max.x()) return false;

		if (e.x()<bb_min.x())
		{
			// clip s to xMin.
			e = s+(e-s)*(bb_min.x()-s.x())/(e.x()-s.x());
		}

		if (s.x()>bb_max.x())
		{
			// clip e to xMax.
			s = s+(e-s)*(bb_max.x()-s.x())/(e.x()-s.x());
		}
	}

	// compate s and e against the yMin to yMax range of bb.
	if (s.y()<=e.y())
	{

		// trivial reject of segment wholely outside.
		if (e.y()<bb_min.y()) return false;
		if (s.y()>bb_max.y()) return false;

		if (s.y()<bb_min.y())
		{
			// clip s to yMin.
			s = s+(e-s)*(bb_min.y()-s.y())/(e.y()-s.y());
		}

		if (e.y()>bb_max.y())
		{
			// clip e to yMax.
			e = s+(e-s)*(bb_max.y()-s.y())/(e.y()-s.y());
		}
	}
	else
	{
		if (s.y()<bb_min.y()) return false;
		if (e.y()>bb_max.y()) return false;

		if (e.y()<bb_min.y())
		{
			// clip s to yMin.
			e = s+(e-s)*(bb_min.y()-s.y())/(e.y()-s.y());
		}

		if (s.y()>bb_max.y())
		{
			// clip e to yMax.
			s = s+(e-s)*(bb_max.y()-s.y())/(e.y()-s.y());
		}
	}

	// compate s and e against the zMin to zMax range of bb.
	if (s.z()<=e.z())
	{

		// trivial reject of segment wholely outside.
		if (e.z()<bb_min.z()) return false;
		if (s.z()>bb_max.z()) return false;

		if (s.z()<bb_min.z())
		{
			// clip s to zMin.
			s = s+(e-s)*(bb_min.z()-s.z())/(e.z()-s.z());
		}

		if (e.z()>bb_max.z())
		{
			// clip e to zMax.
			e = s+(e-s)*(bb_max.z()-s.z())/(e.z()-s.z());
		}
	}
	else
	{
		if (s.z()<bb_min.z()) return false;
		if (e.z()>bb_max.z()) return false;

		if (e.z()<bb_min.z())
		{
			// clip s to zMin.
			e = s+(e-s)*(bb_min.z()-s.z())/(e.z()-s.z());
		}

		if (s.z()>bb_max.z())
		{
			// clip e to zMax.
			s = s+(e-s)*(bb_max.z()-s.z())/(e.z()-s.z());
		}
	}

	// OSG_NOTICE<<"clampped segment "<<s<<" "<<e<<std::endl;

	// if (s==e) return false;

	return true;
}

void CGlbGlobeSceneObjIdxManager::_addQtreeObject(CGlbGlobeObject* n,CGlbGlobeQtree* qtant,long depth)
{
	CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(n);
	if (!robj)return;

	bool is_world = mpr_b_globe_type ? false : true;

	glbref_ptr<CGlbExtent> _extent = NULL;
	glbref_ptr<IGlbGeometry> _outline = robj->GetOutLine();
	if (_outline)
	{
		_extent = const_cast<CGlbExtent*>(_outline->GetExtent());
	}
	else
		_extent = robj->GetBound(is_world);

	if (!_extent)
		return;
	double mx,my,mz,Mx,My,Mz;
	_extent->GetMin(&mx,&my,&mz);
	_extent->GetMax(&Mx,&My,&Mz);
	CGlbAxisAlignedBox bx(mx,my,mz,Mx,My,Mz);

	//if the qtree is twice as big as the scene node,
	//we will add it to a child.
	if ( ( depth < mpr_max_depth ) && qtant -> IsTwiceSize( bx ) )
	{
		long x, y;
		qtant -> GetChildIndexes( bx, &x, &y );

		if ( qtant -> mpr_p_children[ x ][ y ] == 0 )
		{
			qtant -> mpr_p_children[ x ][ y ] = new CGlbGlobeQtree ( qtant );
			osg::Vec3& qtantMin = qtant -> mpr_box.GetMinimum();
			osg::Vec3& qtantMax = qtant -> mpr_box.GetMaximum();			

			float minx,miny,minz,maxx,maxy,maxz;
			if ( x == 0 )
			{
				minx = qtantMin.x();
				maxx = ( qtantMin.x() + qtantMax.x() ) / 2;
			}

			else
			{
				minx = ( qtantMin.x() + qtantMax.x() ) / 2;
				maxx = qtantMax.x();
			}

			if ( y == 0 )
			{
				miny = qtantMin.y();
				maxy = ( qtantMin.y() + qtantMax.y() ) / 2;
			}

			else
			{
				miny = ( qtantMin.y() + qtantMax.y() ) / 2;
				maxy = qtantMax.y();
			}

			minz = maxz = 0;

			osg::Vec3 _min(minx,miny,minz);
			osg::Vec3 _max(maxx,maxy,maxz);

			qtant -> mpr_p_children[ x ][ y ] -> mpr_box.SetExtents( _min, _max );
			qtant -> mpr_p_children[ x ][ y ] -> mpr_halfsize = ( _max - _min ) / 2;
		}

		_addQtreeObject( n, qtant -> mpr_p_children[ x ][ y ], ++depth );

	}
	else
	{
		qtant -> AddObject( n );
	}
}

bool CGlbGlobeSceneObjIdxManager::QueryOnTerrainObjects(osg::Vec3d on_terrain_point, std::vector<glbref_ptr<CGlbGlobeObject>>& result,double nearest_distance, GlbGlobeTypeEnum globeTye)
{
	EnterCriticalSection(&mpr_objects_criticalsection);
	if (mpr_globe_top_qtree)
		_findNodes(on_terrain_point,result,mpr_globe_top_qtree,nearest_distance,globeTye);
	// GLB_GLOBE模式 // GLB_FLAT模式
	std::map<long,CGlbGlobeQtree*>::iterator itr = mpr_map_qtrees.begin();
	while(itr != mpr_map_qtrees.end())
	{			
		osg::Vec3 pos = itr->second->mpr_box.GetCenter();
		pos.set(on_terrain_point.x(),on_terrain_point.y(),pos.z());
		if (itr->second->mpr_box.Contains(pos))//只处理与区域相交的情况
			_findNodes(on_terrain_point,result,itr->second,nearest_distance,globeTye);
		itr++;
	}
	LeaveCriticalSection(&mpr_objects_criticalsection);

	if (result.size()>0)
		return true;	
	return false;
}

bool CGlbGlobeSceneObjIdxManager::QueryObjects(CGlbExtent* t_extent,std::vector<glbref_ptr<CGlbGlobeObject>>& result)
{
	double mx,my,mz,Mx,My,Mz;
	t_extent->GetMin(&mx,&my,&mz);
	t_extent->GetMax(&Mx,&My,&Mz);
	CGlbAxisAlignedBox bx(mx,my,mz,Mx,My,Mz);

	EnterCriticalSection(&mpr_objects_criticalsection);
	if (mpr_globe_top_octree)
		_findNodes(bx,result,mpr_globe_top_octree);

	std::map<long,CGlbGlobeOctree*>::iterator itr = mpr_map_octrees.begin();
	while(itr != mpr_map_octrees.end())
	{
		_findNodes(bx,result,itr->second);
		itr++;
	}	

	LeaveCriticalSection(&mpr_objects_criticalsection);

	if (result.size() > 0)
		return true;

	return false;
}

bool CGlbGlobeSceneObjIdxManager::QueryOnTerrainObjects(CGlbExtent* t_extent,std::vector<glbref_ptr<CGlbGlobeObject>>& result)
{
	if (!t_extent) return false;
	EnterCriticalSection(&mpr_objects_criticalsection);
	// GLB_GLOBE模式  // GLB_FLAT模式
	double minX,maxX,minY,maxY;
	t_extent->Get(&minX,&maxX,&minY,&maxY);
	CGlbAxisAlignedBox bx(minX,minY,0,maxX,maxY,1);

	if (mpr_globe_top_qtree)
		_findNodes(minX,maxX,minY,maxY,result,mpr_globe_top_qtree);

	std::map<long,CGlbGlobeQtree*>::iterator itr = mpr_map_qtrees.begin();
	while(itr != mpr_map_qtrees.end())
	{		
		if (itr->second->mpr_box.Intersects2D(bx))//只处理与区域相交的情况
			_findNodes(minX,maxX,minY,maxY,result,itr->second);
		itr++;
	}
	LeaveCriticalSection(&mpr_objects_criticalsection);

	if (result.size()>0)
		return true;
	return false;
}

long CGlbGlobeSceneObjIdxManager::GetOnTerrainObjectCount()
{
	return mpr_onterrain_objects_count;
}

