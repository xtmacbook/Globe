#include "StdAfx.h"

#include "GlbRobjectMeshBox.h"
#include "GlbGlobeMath.h"
#include "GlbFeature.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "CGlbGlobe.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>


#include <osgUtil/LineSegmentIntersector>
#include <osg/ComputeBoundsVisitor>
#include <osg/CoordinateSystemNode>
#include <limits>


using namespace GlbGlobe;

#ifndef GRID_ONE_DIMENSIONAL_NUM
#define  GRID_ONE_DIMENSIONAL_NUM 10 
#endif

#ifndef GRID_TIN_TRAINGLE_NUM
#define GRID_TIN_TRAINGLE_NUM 5000
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define MINNUM -1.7976931348623157E+308



GlbGlobe::GridCell::GridCell()
{
	node = NULL;
	verticesArrays = NULL;

	extend.Set(
		std::numeric_limits<double>::max(),
		MINNUM,
		std::numeric_limits<double>::max(),
		MINNUM,
		std::numeric_limits<double>::max(),
		MINNUM);
}

glbBool ClbRobjectMeshBox::intersect(const osg::Vec3d start,const osg::Vec3d end,
	osg::Vec3d&intersectPoint)
{

	glbBool haveIntersect = false;
	osg::Vec3d tmpPoint;
	GLdouble distance = DBL_MAX;

	std::vector<GridCell>::iterator iter_Grid;
	std::vector<osg::Vec3d > intersectionPointVector;

	osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(start,end);
	osgUtil::IntersectionVisitor iv(lsi.get());

	//将世界坐标系的线转到AABB局部坐标系
	osg::Vec3d AS = start * _worldToAABBSpace;
	osg::Vec3d AE = end * _worldToAABBSpace;

	for(iter_Grid = _boxs.begin();iter_Grid != _boxs.end();iter_Grid++)
	{
		CGlbExtent& extent = iter_Grid->extend;

		glbDouble x,y,z;
		extent.GetMin(&x,&y,&z);
		osg::Vec3d Min(x,y,z);
		extent.GetMax(&x,&y,&z);
		osg::Vec3d Max(x,y,z);

		//与外扩AABB有交点
		if(intersectSegmentAABBTest(AS,AE,Min,Max))
		{
			//osg Node求交点
			iter_Grid->node->accept(iv);
			if (lsi->containsIntersections())
			{
				
				haveIntersect = true;
				osg::Vec3d  tmpInterPoint = lsi->getIntersections().begin()->getWorldIntersectPoint();
				GLdouble currD = (tmpInterPoint - start) * (tmpInterPoint - start); 
				if(distance > currD)
				{
					//最近的点
					distance = currD;
					tmpPoint = tmpInterPoint;
				}
			}
		}
	}

	if(haveIntersect)
	{
		intersectPoint = tmpPoint;
	}

	return haveIntersect;
}

void ClbRobjectMeshBox::updateGrids(const osg::Matrix parentMatrix,const GlbTinSymbolInfo * tinSmblInfo,
	glbref_ptr<CGlbFeature> feature,GLboolean globeMode)
{
	for(unsigned int i = 0;i < _boxs.size();i++)
	{
		osg::Node * node = _boxs[i].node;

		if(node->asGroup())
		{
			if(node->asGroup()->asTransform())
			{
				if(node->asGroup()->asTransform()->asMatrixTransform())
				{
					/*更新矩阵*/
					osg::MatrixTransform * mtNode = node->asGroup()->asTransform()->asMatrixTransform();

					osg::Matrix inverseOrigin = osg::Matrix::inverse(mtNode->getMatrix());
					osg::Vec3 origin = mtNode->getMatrix().getTrans();
					
					//节点父节点(平移缩放)
					mtNode->setMatrix(parentMatrix);
					
					//格子矩阵
					_worldToAABBSpace = _localMatrix * parentMatrix;

					_worldToAABBSpace = osg::Matrixd::inverse(_worldToAABBSpace);
				}
			}
		}
	}
}

void ClbRobjectMeshBox::clearGrids()
{
	_boxs.clear();
}

glbBool ClbRobjectMeshBox::buildMeshBox(CGlbGlobeRObject * obj,IGlbGeometry *geo)
{
	if (obj==NULL||geo==NULL||obj->GetGlobe()==NULL) return false;

	GlbRenderInfo * renderInfo = obj->GetRenderInfo();
	GlbTinSymbolInfo * symbolInfo = dynamic_cast<GlbTinSymbolInfo *>(renderInfo);
	if(NULL == symbolInfo) return false;

	switch(geo->GetType())
	{

	case GLB_GEO_LINE:
	case GLB_GEO_POINT:
		{
			return false;
		}
	case GLB_GEO_MULTITIN:
		{
			CGlbMultiTin *geoTmp = dynamic_cast<CGlbMultiTin*>(geo);
			if(geoTmp == NULL) return false;
			if(GLB_GLOBETYPE_GLOBE == obj->GetGlobe()->GetType())
			{
				return buildGlobeMultTinMeshBoxs(obj,geoTmp);
			}
			else if(GLB_GLOBETYPE_FLAT == obj->GetGlobe()->GetType())
			{
				return buildPlaneMultTinMeshBoxs(obj,geoTmp);
			}
		}
	case GLB_GEO_TIN:
		{
			CGlbTin *geoTmp=dynamic_cast<CGlbTin *>(geo);
			if (geoTmp==NULL) return false;

			if(GLB_GLOBETYPE_GLOBE == obj->GetGlobe()->GetType())
			{

				/*return*/ buildGlobeTinMeshBoxs(obj,geoTmp);
				return true;
			}
			else if(GLB_GLOBETYPE_FLAT == obj->GetGlobe()->GetType())
			{
				return buildPlaneTinMeshBoxs(obj,geoTmp);
			}		
		}
	default:
		{
			return false;				
		}			
	}

	return true;
}

CellPointer ClbRobjectMeshBox::initGrids(GLdouble& cellSize,unsigned int * axisNum,
	std::vector<osg::Vec3d>&OBB8Points)
{
	const osg::Vec3d _minEx = OBB8Points[0];
	const osg::Vec3d _maxEx = OBB8Points[1];


	GLdouble xWeight = _maxEx.x() - _minEx.x();
	GLdouble yWeight = _maxEx.y() - _minEx.y();

	/* 依据Tin对象外包盒的最长轴和最长轴划分格子数量固定来 计算划分格子 */
	//X轴是否最长
	GLboolean xAaix = (xWeight >= yWeight)?true:false;

	if(xAaix)
	{
		cellSize = xWeight / GRID_ONE_DIMENSIONAL_NUM;
		axisNum[0] = GRID_ONE_DIMENSIONAL_NUM;

		axisNum[1] = (int)yWeight / cellSize;

		//最后一格的长度
		GLdouble moreSize = yWeight - axisNum[1] * cellSize;
		//最后一格大于定长格子的一半长度，则最后一格单算一格
		if(moreSize >= 0.5 * cellSize)
		{
			axisNum[1] += 1;
		}
	}
	else
	{
		cellSize = yWeight / GRID_ONE_DIMENSIONAL_NUM;
		axisNum[1] = GRID_ONE_DIMENSIONAL_NUM;

		axisNum[0] = (int)xWeight / cellSize;

		//最后一格的长度
		GLdouble moreSize = xWeight - axisNum[0] * cellSize;

		if(moreSize >= 0.5 * cellSize)
		{
			axisNum[0] += 1;
		}
	}

	CellPointer boxArrays;

#ifdef USE_Z_AXIS
	const unsigned int zNum = 8;
	GLdouble zWeight = _maxEx.z() - _minEx.z();
	zWeight = zWeight / zNum;
	axisNum[2] = zNum;

	boxArrays = new GridCell**[axisNum[0]];
	for(unsigned int i = 0;i <  axisNum[0];i++)
	{
		boxArrays[i] = new GridCell*[axisNum[1]];

		for(unsigned int j = 0;j < axisNum[1];j++)
		{
			boxArrays[i][j] = new GridCell[axisNum[2]];
		}
	}


#else
	boxArrays = new GridCell*[axisNum[0]];
	for(unsigned int i = 0;i <  axisNum[0];i++)
	{
		boxArrays[i] = new GridCell[axisNum[1]];
	}
#endif

	return boxArrays;
}

ClbRobjectMeshBox::ClbRobjectMeshBox()
{
}

void ClbRobjectMeshBox::initLocalAABB(CGlbGlobeRObject * obj, IGlbGeometry * 
	geo,std::vector<osg::Vec3d>&OBB8Points, glbBool local)
{

	//在平移缩放坐标系下的中心点
	osg::Vec3d center = getLocalCenter(obj,geo);

	//parent matrix(平移缩放)
	osg::Matrix pm = getParentNodeMatrix(obj,geo);

	//转到世界坐标系下局部参考系的中心点  mt0当前项目为标准矩阵,
	osg::Vec3d wCenter = center * pm /** mt0*/; 

	osg::Matrixd localToWorld;
	if(!local)
	{
		localToWorld.preMultTranslate(wCenter);
	}
	else
	{	
		g_ellipsmodel->computeLocalToWorldTransformFromXYZ(wCenter.x(),wCenter.y(),wCenter.z(),localToWorld);
	}

	_localMatrix = localToWorld * osg::Matrixd::inverse(pm);

	_worldToAABBSpace =	osg::Matrixd::inverse(localToWorld);

	//获取对象外扩来构建局部AABB
	glbref_ptr<CGlbExtent> extend = obj->GetBound(false);
	glbDouble x,y,z;
	extend->GetMin(&x,&y,&z);
	osg::Vec3d Min(x,y,z);
	extend->GetMax(&x,&y,&z);
	osg::Vec3d Max(x,y,z);

	//世界坐标系下的8个点(经纬度)
	std::vector<osg::Vec3d> wllh8;
	wllh8.push_back(Min);											//0
	wllh8.push_back(osg::Vec3d(Max.x(),Min.y(),Min.z()));			//1
	wllh8.push_back(osg::Vec3d(Max.x(),Min.y(),Max.z()));			//2
	wllh8.push_back(osg::Vec3d(Min.x(),Min.y(),Max.z()));			//3
	wllh8.push_back(osg::Vec3d(Min.x(),Max.y(),Min.z()));			//4
	wllh8.push_back(osg::Vec3d(Max.x(),Max.y(),Min.z()));			//5
	wllh8.push_back(Max);											//6
	wllh8.push_back(osg::Vec3d(Min.x(),Max.y(),Max.z()));			//7

	std::vector<osg::Vec3d>::iterator iter;
	
	//8个AABB的点先转到世界坐标系下
	if(local)
	{
		//球面在Z轴缩放 进行处理
		GLdouble zScale = 1.0;
		GlbRenderInfo * renderInfo = obj->GetRenderInfo();
		GlbTinSymbolInfo * symbolInfo = dynamic_cast<GlbTinSymbolInfo *>(renderInfo);
		CGlbFeature * feature = obj->GetFeature();
		if(symbolInfo->xScale) zScale = symbolInfo->zScale->GetValue(feature);

		//经纬度转到XYZ
		for(iter = wllh8.begin();iter != wllh8.end();iter++)
		{
			osg::Vec3d& v = *iter;

			//z轴缩放
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v.y()),
				osg::DegreesToRadians(v.x()),v.z() * zScale,
				v.x(),v.y(),v.z());

			v = v * pm;
		}
	}
	else
	{
		//平面
		for(iter = wllh8.begin();iter != wllh8.end();iter++)
		{
			osg::Vec3d& v = *iter;
			v = v * pm;
		}
	}
	
	//转到AABB局部坐标系下，获取最大最小外扩点
	osg::BoundingBoxd  box;
	box.init();
	
	for(iter = wllh8.begin();iter != wllh8.end();iter++)
	{
		//world->aabb space
		box.expandBy(*iter * _worldToAABBSpace);
	}

	OBB8Points.push_back(box._min);
	OBB8Points.push_back(box._max);

	/* 
			//     7+------+6
			//     /|     /|
			//    / |    / |
			//   / 4+---/--+5
			// 3+------+2 /    y    
			//  | /    | /     |  /-z
			//  |/     |/      | /
			// 0+------+1      *---x
			*/              
}

void ClbRobjectMeshBox::buildOver(const osg::Matrix&parentM,const osg::Matrixd&localCenter,unsigned int*axisNum, CellPointer grids)
{
	/* 遍历boxArrays 填充MeshBox 返回 */
	for(unsigned int i = 0;i < axisNum[0];i++)
	{
		for(unsigned int j = 0;j < axisNum[1];j++)
		{
#ifdef USE_Z_AXIS
			for (unsigned int r = 0; r < axisNum[2];r++)
			{
				GridCell& box = grids[i][j][r];
				addTriangleToBox(parentM,localCenter,box,grids);
			}
#else
			GridCell& box = grids[i][j];
			addTriangleToBox(parentM,localCenter,box,grids);	
#endif

			
		}
	}

	/*清空格子数组*/
	for(unsigned int i = 0;i < axisNum[0];i++)
	{
		delete [] grids[i];
	}
	delete[] grids;

}

glbBool GlbGlobe::ClbRobjectMeshBox::buildPlaneTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geoo)
{
	CGlbTin *geo=dynamic_cast<CGlbTin *>(geoo);

	//在三角面片多余10000时候才分格
	if(geo->GetTriangleCount() < GRID_TIN_TRAINGLE_NUM) 
	{
		return  false;
	}
	
	GLdouble cellSize;		//划分后格子的定长

	unsigned int XYZAxisNum[3] = {0,0,0};

	std::vector<osg::Vec3d> OBB8Points;
	initLocalAABB(obj,geo,OBB8Points,false);

	//划分格子
	CellPointer grids = initGrids(cellSize,XYZAxisNum,OBB8Points);

	//平移缩放矩阵
	osg::Matrix mt0 = osg::Matrixd::identity();      //mt0
	osg::Matrix pm =  getParentNodeMatrix(obj,geo); //mt1
	mt0.preMult(pm);

	//局部平移Tin对象中心
	osg::Vec3d localToCenter = getLocalCenter(obj,geo);
	osg::Matrixd localToCenterM;
	localToCenterM.makeTranslate(localToCenter);

	//AABB到世界坐标系矩阵
	osg::Matrixd toAABBSpace;
	toAABBSpace.postMult(mt0);
	toAABBSpace.postMult(_worldToAABBSpace);

	//遍历将三角面片放进格子
	getAddTinTriangle(obj,geo,localToCenter,cellSize,XYZAxisNum,toAABBSpace,
		grids,OBB8Points);
	//将格子内节点组成一个OSG节点
	buildOver(getParentNodeMatrix(obj,geo),localToCenterM,XYZAxisNum,grids);

	return true;
}

glbBool GlbGlobe::ClbRobjectMeshBox::buildPlaneMultTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geoo)
{
	/* 在三角面片多余10000时候才分格 */
	CGlbMultiTin *geo = dynamic_cast<CGlbMultiTin*>(geoo);
	unsigned int traingleNum = 0;
	for(glbInt32 i = 0; i < geo->GetCount();i++) 
	{
		CGlbTin *tin = const_cast<CGlbTin *>(geo->GetTin(i));
		traingleNum += tin->GetTriangleCount();
	}
	//到达一定数量三角面片进行格子化
	if(traingleNum < GRID_TIN_TRAINGLE_NUM) return false;

	GLdouble cellSize;		//划分后格子的定长
	unsigned int XYZAxisNum[3] = {0,0,0};

	std::vector<osg::Vec3d> OBB8Points;
	initLocalAABB(obj,geo,OBB8Points,false);

	CellPointer grids = initGrids(cellSize,XYZAxisNum,OBB8Points);
	
	osg::Vec3d localToCenter = getLocalCenter(obj,geo);
	osg::Matrixd localToCenterM;
	localToCenterM.makeTranslate(localToCenter);

	osg::Matrix mt0 = osg::Matrixd::identity();      //mt0
	osg::Matrix pm =  getParentNodeMatrix(obj,geo); //mt1
	mt0.preMult(pm);

	osg::Matrixd toAABBSpace;
	toAABBSpace.postMult(mt0);
	toAABBSpace.postMult(_worldToAABBSpace);

	glbInt32 countNum = geo->GetCount();
	for(glbInt32 i = 0; i < countNum; i++)
	{
		CGlbTin *tin = const_cast<CGlbTin *>(geo->GetTin(i));
		glbInt32 vertexCount = tin->GetVertexCount();
		glbInt32 triangleCount = tin->GetTriangleCount();
		if(vertexCount < 1 || triangleCount < 1) continue;
	
		getAddTinTriangle(obj,tin,localToCenter,cellSize,XYZAxisNum,toAABBSpace,grids,OBB8Points);
	}

	buildOver(getParentNodeMatrix(obj,geo),localToCenterM,XYZAxisNum,grids);

	return true;
}

glbBool GlbGlobe::ClbRobjectMeshBox::buildGlobeTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geoo)
{
	CGlbTin *geo=dynamic_cast<CGlbTin *>(geoo);

	//在三角面片多余10000时候才分格
	if(geo->GetTriangleCount() < GRID_TIN_TRAINGLE_NUM) 
	{
		return  false;
	}

	GLdouble cellSize;		//划分后格子的定长
	unsigned int XCellNum ; //X轴所划分的格子数量
	unsigned int YCellNum ; //Y轴所划分的格子数量

	std::vector<osg::Vec3d> OBB8Points;
	initLocalAABB(obj,geo,OBB8Points,true);

	unsigned int XYZAxisNum[3] = {0,0,0};

	CellPointer grids = initGrids(cellSize,XYZAxisNum,OBB8Points);

	osg::Matrix mt0 = osg::Matrixd::identity();      //mt0
	osg::Matrix pm =  getParentNodeMatrix(obj,geo); //mt1
	mt0.preMult(pm);

	osg::Vec3d localToCenter = getLocalCenter(obj,geo);
	osg::Matrixd localToCenterM;
	localToCenterM.makeTranslate(localToCenter);

	osg::Matrixd toAABBSpace;
	toAABBSpace.postMult(mt0);
	toAABBSpace.postMult(_worldToAABBSpace);

	getAddTinTriangle(obj,geo,localToCenter,cellSize,XYZAxisNum,toAABBSpace,
		grids,OBB8Points);

	buildOver(mt0,localToCenterM,XYZAxisNum,grids);

	return true;
}

glbBool GlbGlobe::ClbRobjectMeshBox::buildGlobeMultTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geoo)
{
	/* 在三角面片多余10000时候才分格 */
	CGlbMultiTin *geo = dynamic_cast<CGlbMultiTin*>(geoo);
	unsigned int traingleNum = 0;
	for(glbInt32 i = 0; i < geo->GetCount();i++) 
	{
		CGlbTin *tin = const_cast<CGlbTin *>(geo->GetTin(i));
		traingleNum += tin->GetTriangleCount();
	}

	if(traingleNum < GRID_TIN_TRAINGLE_NUM) return false;

	GLdouble cellSize;		//划分后格子的定长
	unsigned int XYZAxisNum[3] = {0,0,0};

	std::vector<osg::Vec3d> OBB8Points;
	initLocalAABB(obj,geo,OBB8Points,true);

	CellPointer grids = initGrids(cellSize,XYZAxisNum,OBB8Points);

	osg::Matrix mt0 = osg::Matrixd::identity();      //mt0
	osg::Matrix pm =  getParentNodeMatrix(obj,geo); //mt1
	mt0.preMult(pm);

	osg::Vec3d localToCenter = getLocalCenter(obj,geo);
	osg::Matrixd localToCenterM;
	localToCenterM.makeTranslate(localToCenter);

	osg::Matrixd toAABBSpace;
	toAABBSpace.postMult(mt0);
	toAABBSpace.postMult(_worldToAABBSpace);

	
	glbInt32 countNum = geo->GetCount();
	for(glbInt32 i = 0; i < countNum; i++)
	{
		CGlbTin *tin = const_cast<CGlbTin *>(geo->GetTin(i));
		glbInt32 vertexCount = tin->GetVertexCount();
		glbInt32 triangleCount = tin->GetTriangleCount();
		if(vertexCount < 1 || triangleCount < 1) continue;

		getAddTinTriangle(obj,tin,localToCenter,cellSize,XYZAxisNum,toAABBSpace,
			grids,OBB8Points);

	}
	
	buildOver(pm,localToCenterM,XYZAxisNum,grids);

	return true;
}


void GlbGlobe::ClbRobjectMeshBox:: getAddTinTriangle(CGlbGlobeRObject * obj,CGlbTin * geo,osg::Vec3d& localCenter,
	GLdouble CellSize,
	unsigned int*axisNum, const osg::Matrixd&toAABBSpace,
	CellPointer grids,std::vector<osg::Vec3d>&OBB8Points)
{
	unsigned int triangleCount = geo->GetTriangleCount();
	
	glbBool globeModel = (GLB_GLOBETYPE_GLOBE == obj->GetGlobe()->GetType())?true:false;

	GLdouble zScale = 1.0;

	if(globeModel)
	{
		//球面模式，Z轴进行缩放
		GlbRenderInfo * renderInfo = obj->GetRenderInfo();
		GlbTinSymbolInfo * symbolInfo = dynamic_cast<GlbTinSymbolInfo *>(renderInfo);

		CGlbFeature * feature = obj->GetFeature();
		if(symbolInfo->xScale) zScale = symbolInfo->zScale->GetValue(feature);
	}

	/* 遍历三角形的点,分别判断三角形所在格子 */
	for (unsigned int i = 0;i < triangleCount; i++)
	{
		glbInt32 first_ptidx,second_ptidx,third_ptidx;
		geo->GetTriangle(i,&first_ptidx,&second_ptidx,&third_ptidx);	

		//三角形的三个点
		osg::Vec3d v0;
		osg::Vec3d v1;
		osg::Vec3d v2;

		//获取三个点
		geo->GetVertex(first_ptidx,&v0.x(),&v0.y(),&v0.z());	
		geo->GetVertex(second_ptidx,&v1.x(),&v1.y(),&v1.z());	
		geo->GetVertex(third_ptidx,&v2.x(),&v2.y(),&v2.z());	

		if(globeModel)
		{
			//又源代码而来 ，此处没有考虑mt0 和parentMt
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v0.y()),
				osg::DegreesToRadians(v0.x()),v0.z()*zScale,
				v0.x(),v0.y(),v0.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v1.y()),
				osg::DegreesToRadians(v1.x()),v1.z()*zScale,
				v1.x(),v1.y(),v1.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(v2.y()),
				osg::DegreesToRadians(v2.x()),v2.z()*zScale,
				v2.x(),v2.y(),v2.z());
			
		}

		//局部点坐标
		osg::Vec3 lv0(v0 - localCenter);
		osg::Vec3 lv1(v1 - localCenter);
		osg::Vec3 lv2(v2 - localCenter);

		////先转到AABB所在的坐标系
		osg::Vec3d av0 = v0  * toAABBSpace ; 
		osg::Vec3d av1 = v1  * toAABBSpace ;
		osg::Vec3d av2 = v2  * toAABBSpace ;

		XYIndex gridIndex;
		getGridIndex(&av0,&av1,&av2,gridIndex,CellSize,axisNum,OBB8Points);

		//根据格子索引获取对应box
#ifdef USE_Z_AXIS
		GridCell & nowBox = grids[gridIndex.xindex][gridIndex.yindex][gridIndex.zindex];
#else
		GridCell & nowBox = grids[gridIndex.xindex][gridIndex.yindex];
#endif
		if(nowBox.verticesArrays == nullptr)
		{
			nowBox.verticesArrays = new osg::Vec3Array;
		}

		/* 局部坐标系下三角形 更新外扩Extent */
		if (nowBox.extend.Contains(av0.x(),av0.y(),av0.z())==false)
			nowBox.extend.Merge(av0.x(),av0.y(),av0.z());
		if (nowBox.extend.Contains(av1.x(),av1.y(),av1.z())==false)
			nowBox.extend.Merge(av1.x(),av1.y(),av1.z());
		if (nowBox.extend.Contains(av2.x(),av2.y(),av2.z())==false)
			nowBox.extend.Merge(av2.x(),av2.y(),av2.z());	
		//保存局部点坐标
		nowBox.verticesArrays->push_back(lv0);
		nowBox.verticesArrays->push_back(lv1);
		nowBox.verticesArrays->push_back(lv2);
		
	}
}

void  GlbGlobe::ClbRobjectMeshBox::getGridIndex(const osg::Vec3d* v0,const osg::Vec3d* v1,const osg::Vec3d*v2,
	XYIndex & idx,GLdouble cellSize,unsigned int *axisNum ,std::vector<osg::Vec3d>&OBB8Points)
{

#ifdef USE_Z_AXIS
	/* 获取中心点,判断三角形所在格子 */
	osg::Vec3d center; //三角形的中心点

	center.x() = (v0->x() + v1->x() + v2->x()) / 3;
	center.y() = (v0->y() + v1->y() + v2->y() ) / 3;
	center.z() = (v0->z() + v1->z() + v2->z() ) / 3;

	//x.y轴格子索引
	unsigned int xindex = static_cast<unsigned int> ((center.x() - OBB8Points[0].x()) / cellSize);
	unsigned int yindex = static_cast<unsigned int> ((center.y() - OBB8Points[0].y()) / cellSize);
	unsigned int zindex = static_cast<unsigned int> ((center.z() - OBB8Points[0].z()) / zCellWidth);

	if(xindex > axisNum[0] - 1)
	{
		xindex = axisNum[0] - 1;
	}
	if(yindex > axisNum[1] - 1)
	{
		yindex = axisNum[1] - 1;
	}
	
	if(zindex > axisNum[2] - 1)
	{
		zindex = axisNum[2] - 1;
	}

	idx.xindex = xindex;
	idx.yindex = yindex;
	idx.zindex = zindex;
#else
	/* 获取中心点,判断三角形所在格子 */
	osg::Vec2d center; //三角形的中心点

	center.x() = (v0->x() + v1->x() + v2->x()) / 3;
	center.y() = (v0->y() + v1->y() + v2->y() ) / 3;
	//center.z() = (v0->z() + v1->z() + v2->z() ) / 3;

	//x.y轴格子索引
	unsigned int xindex = static_cast<unsigned int> ((center.x() - OBB8Points[0].x()) / cellSize);
	unsigned int yindex = static_cast<unsigned int> ((center.y() - OBB8Points[0].y()) / cellSize);

	if(xindex > axisNum[0] - 1)
	{
		xindex = axisNum[0] - 1;
	}
	if(yindex > axisNum[1] - 1)
	{
		yindex = axisNum[1] - 1;
	}

	idx.xindex = xindex;
	idx.yindex = yindex;
#endif
	
}

osg::Matrix GlbGlobe::ClbRobjectMeshBox::getParentNodeMatrix(CGlbGlobeRObject * obj, IGlbGeometry *  geo/*,GlbTinSymbolInfo *symbolInfo */)
{

	GlbGlobeTypeEnum type = obj->GetGlobe()->GetType();

	glbBool globeMode = (GLB_GLOBETYPE_GLOBE == type )?true:false;

	GlbRenderInfo * renderInfo = obj->GetRenderInfo();
	GlbTinSymbolInfo * symbolInfo = dynamic_cast<GlbTinSymbolInfo *>(renderInfo);

	CGlbFeature * feature = obj->GetFeature();
	
	//父节点的偏移 offset
	GLdouble xOffset, yOffset ,zOffset;
	if (symbolInfo->xOffset) xOffset = symbolInfo->xOffset->GetValue((feature));
	if (symbolInfo->yOffset) yOffset = symbolInfo->yOffset->GetValue((feature));
	if (symbolInfo->zOffset) zOffset = symbolInfo->zOffset->GetValue((feature));	

	osg::Matrixd mOffset;
	if(globeMode)
	{
		osg::Vec3d center;
		geo->GetExtent()->GetCenter(&center.x(),&center.y(),&center.z());

		double longitude = osg::DegreesToRadians(center.x());
		double latitude = osg::DegreesToRadians(center.y());
		// Compute up vector
		osg::Vec3d    up      ( cos(longitude)*cos(latitude),
			sin(longitude)*cos(latitude), sin(latitude));
		// Compute east vector
		osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
		// Compute north  vector = outer product up x east
		osg::Vec3d    north   = up ^ east;

		north.normalize();


		//以east up north为基坐标的坐标系内进行平移
		osg::Vec3 _Offset = east*xOffset + north*yOffset + up*zOffset;

		mOffset.makeTranslate(_Offset);
	}
	else
	{
		mOffset.makeTranslate(xOffset,yOffset,zOffset);	
	}


	// 父节点的缩放 scale
	glbDouble xScale = 1 , yScale = 1 , zScale = 1;
	if(!globeMode)
	{
		if(symbolInfo->xScale) xScale = symbolInfo->xScale->GetValue(feature);
		if(symbolInfo->yScale) yScale = symbolInfo->yScale->GetValue(feature);
		if(symbolInfo->zScale) zScale = symbolInfo->zScale->GetValue(feature);	
	}

	osg::Matrix mScale;
	mScale.makeScale(xScale,yScale,zScale);

	//local -> world 该矩阵用到平移缩放
	osg::Matrix localToWorld = mScale * mOffset;

	return localToWorld;
}

osg::Vec3d GlbGlobe::ClbRobjectMeshBox::getLocalCenter(CGlbGlobeRObject * obj, IGlbGeometry *  geo)
{
	//获取Tin对象的局部中心点
	osg::Vec3d center;
	geo->GetExtent()->GetCenter(&center.x(),&center.y(),&center.z());

	if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		glbDouble zScale = 1.0;

		CGlbFeature * feature = obj->GetFeature();

		GlbRenderInfo * renderInfo = obj->GetRenderInfo();
		GlbTinSymbolInfo * symbolInfo = dynamic_cast<GlbTinSymbolInfo *>(renderInfo);

		zScale = symbolInfo->zScale->GetValue(feature);
		center.z() *= zScale;

		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(center.y()),
			osg::DegreesToRadians(center.x()),center.z(),
			center.x(),center.y(),center.z());
	}

	return center;
}

osg::Node * GlbGlobe::ClbRobjectMeshBox::testShowGridNode()
{
	osg::ref_ptr<osg::Group> boxs = new osg::Group;

	std::vector<GridCell>::iterator iter;
	for (iter = _boxs.begin();iter != _boxs.end();iter++)
	{
		CGlbExtent & extend = iter->extend;
		glbDouble x,y,z;
		extend.GetMin(&x,&y,&z);
		osg::Vec3d Min(x,y,z);
		extend.GetMax(&x,&y,&z);
		osg::Vec3d Max(x,y,z);

		osg::ref_ptr <osg::Vec3dArray> wllh8 = new osg::Vec3dArray;

		wllh8->push_back (Min);								//0
		wllh8->push_back(osg::Vec3d(Max.x(),Min.y(),Min.z()));			//1
		wllh8->push_back(osg::Vec3d(Max.x(),Min.y(),Max.z()));			//2
		wllh8->push_back(osg::Vec3d(Min.x(),Min.y(),Max.z()));			//3
		wllh8->push_back(osg::Vec3d(Min.x(),Max.y(),Min.z()));			//4
		wllh8->push_back (osg::Vec3d(Max.x(),Max.y(),Min.z()));			//5
		wllh8->push_back (Max);								//6
		wllh8->push_back (osg::Vec3d(Min.x(),Max.y(),Max.z()));			//7

		
		std::vector<osg::Vec3d> iter_Osg;
		osg::Matrixd aabbtw = osg::Matrixd::inverse(_worldToAABBSpace);

		for (unsigned int i = 0;i < wllh8->size();i++)
		{
			osg::Vec3d& v = wllh8->at(i);

			v = v * aabbtw;
		}
		

		GLushort idxLines0[4] = { 0, 1, 2, 3};
		GLushort idxLines1[4] = { 1, 5, 6, 2};
		GLushort idxLines2[4] = { 6, 7, 4, 5};
		GLushort idxLines3[4] = { 7, 3, 0, 4};
		GLushort idxLines4[4] = { 2, 6, 7, 3};
		GLushort idxLines5[4] = { 1, 5, 4, 0};

		osg::ref_ptr<osg::Geometry> cubeGeometry = new osg::Geometry;
		cubeGeometry->setVertexArray(wllh8);
		cubeGeometry->addPrimitiveSet(new osg::DrawElementsUShort(osg::PrimitiveSet::LINE_LOOP,4,idxLines0));
		cubeGeometry->addPrimitiveSet(new osg::DrawElementsUShort(osg::PrimitiveSet::LINE_LOOP,4,idxLines1));
		cubeGeometry->addPrimitiveSet(new osg::DrawElementsUShort(osg::PrimitiveSet::LINE_LOOP,4,idxLines2));
		cubeGeometry->addPrimitiveSet(new osg::DrawElementsUShort(osg::PrimitiveSet::LINE_LOOP,4,idxLines3));
		cubeGeometry->addPrimitiveSet(new osg::DrawElementsUShort(osg::PrimitiveSet::LINE_LOOP,4,idxLines4));
		cubeGeometry->addPrimitiveSet(new osg::DrawElementsUShort(osg::PrimitiveSet::LINE_LOOP,4,idxLines5));

		osg::Vec4Array* colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
		cubeGeometry->setColorArray(colors, osg::Array::BIND_OVERALL);

		osg::ref_ptr<osg::Geode> traingleGeode = new osg::Geode;
		traingleGeode->addDrawable(cubeGeometry);

		boxs->addChild(traingleGeode);
		
	}

	return boxs.release();
}

void ClbRobjectMeshBox::addTriangleToBox(const osg::Matrix&parentM,const osg::Matrixd&localCenter,
	const GridCell&box, CellPointer grids)
{
	if(box.verticesArrays != nullptr && box.verticesArrays->size() >= 3)
	{
		GridCell newBox;
		newBox.extend = box.extend;
		newBox.verticesArrays = box.verticesArrays;

		osg::ref_ptr<osg::Geometry> traingleGeometry = new osg::Geometry;
		traingleGeometry->setVertexArray(newBox.verticesArrays);
		traingleGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,
			box.verticesArrays->size()));
		osg::ref_ptr<osg::Geode> traingleGeode = new osg::Geode;
		traingleGeode->addDrawable(traingleGeometry);

		//此处暂时没有加mt0节点，因为项目目前没有矩阵转换
		osg::ref_ptr<osg::MatrixTransform> localToWorldTranSform = new osg::MatrixTransform;
		osg::ref_ptr<osg::MatrixTransform> localToCenterTranSform = new osg::MatrixTransform;

		//平移缩放矩阵
		localToWorldTranSform->setMatrix(parentM);
		//局部平移Tin对象中心点
		localToCenterTranSform->setMatrix(localCenter);

		localToCenterTranSform->addChild(traingleGeode);

		localToWorldTranSform->addChild(localToCenterTranSform);

		newBox.node = localToWorldTranSform;
		//放入返回容器
		_boxs.push_back(newBox);
	}
}



 

