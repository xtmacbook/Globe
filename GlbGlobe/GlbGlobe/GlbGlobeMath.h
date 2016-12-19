#ifndef _GLBGLOBEMATH_H
#define _GLBGLOBEMATH_H

#include "GlbPoint.h"
#include "GlbLine.h"
#include "GlbPolygon.h"


#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MAX3(a, b, c) MAX( MAX(a ,b) ,c)
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MIN3(a, b, c) MIN( MIN(a, b) ,c)

//点(p_x,p_y)到线段(p1,p2)的最短距离
double pointToLine(double seg_sx,double seg_sy,double seg_ex,double seg_ey, double ptx,double pty);

//判断点（px,py）是否在多边形polygon内 【polygon_pts 多边形点集， ptCnt 多边形点数】 目前只支持单个环的多边形 coordDimension 坐标纬度默认为二维坐标（x,y）
bool PtInPolygon(double px, double py, double* polygon_pts, int ptCnt, int coordDimension=2);

bool PtInPolygonExt(double px,double py, double* polygon_pts, int ptCnt, int coordDimension=2);

//判断两个线段ln1,ln2是否相交(sx,sy)起点的坐标,(ex,ey)终点的坐标
bool Intersect(double segment1_sx,double segment1_sy,double segment1_ex,double segment1_ey, 
				double segment2_sx,double segment2_sy,double segment2_ex,double segment2_ey,
				double inter_x=0.0, double inter_y=0.0);

//判断两条线(多线段线)是否相交
bool Intersect(CGlbLine* ln1, CGlbLine* ln2);

//判断线段与多边形是否相交
bool Intersect(CGlbLine* ln, CGlbPolygon* poly);

//判断多边形与多边形是否相交
bool Intersect(CGlbPolygon* poly1, CGlbPolygon* poly2);

double Cross(osg::Vec2d v0,osg::Vec2d v1);

//chenpeng
#include "osg/LineSegment"
#include "osg/Plane"

//射线与点相交
glbBool GLB_DLLCLASS_EXPORT intersect3D_SegmentPoint(osg::LineSegment *Seg1,osg::Vec3d point,glbDouble threshold);

//射线与线段相交
glbBool GLB_DLLCLASS_EXPORT intersect3D_SegmentSegment(osg::LineSegment *Seg1,osg::LineSegment *Seg2,glbDouble threshold,osg::Vec3d &point);

//射线与面相交
glbBool GLB_DLLCLASS_EXPORT intersect3D_SegmentPlane(osg::LineSegment *S,osg::Plane *Pn,osg::Vec3d pPoint,osg::Vec3d &point);

//brief 线段与球求交
//startPos 线段起始点
//endPos 线段终止点
//centerPos 球中心点
//radius 球半径
//IntersectPos 交点
//return 交点个数
glbInt32 GLB_DLLCLASS_EXPORT IntersectRaySphere(osg::Vec3d startPos,osg::Vec3d endPos,osg::Vec3d centerPos,glbDouble radius,std::vector<osg::Vec3d>& IntersectPos);

//射线与节点的包围球相交
glbBool GLB_DLLCLASS_EXPORT intersectRayBoundingSphere(osg::Vec3d startPos,osg::Vec3d endPos,const osg::BoundingSphere& bs);

glbBool GLB_DLLCLASS_EXPORT intersectRayBoundingBox(osg::Vec3d start,osg::Vec3d end,const osg::BoundingBox &bb);


/************************************************************************/
/* 线段和AABB 是否有交点                                                                     */
/************************************************************************/
glbBool GLB_DLLCLASS_EXPORT intersectSegmentAABBTest(const osg::Vec3d start,const osg::Vec3d end,const osg::Vec3d Min,const osg::Vec3d Max);

/************************************************************************
  射线与包围AABB是否有相交,并返回交点
   origin:射线起点
   dir:   射线方向向量
   Min,Max: AABB
   interPo: 交点
   Graphic Gems :395
************************************************************************/
glbBool GLB_DLLCLASS_EXPORT intersectRayAABB(const osg::Vec3d& origin, const osg::Vec3d& dir,const osg::Vec3d&Min,
	const osg::Vec3d&Max,osg::Vec3d&interPo);

/************************************************************************
  射线与三角形否有相交
  origin:射线起点
  dir:   射线方向向量
  v0 v1 v2 ：三角形三个点
  //t 是射线原点和三角形交点的距离，
  //u v 是三角形的质心坐标系(barycentric coordinate) 
  ///参考://http://www.scratchapixel.com/code.php?id=9&origin=/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle
  交点T(u,v) = (1 - u - v)v0 + uv1 + vv2;
************************************************************************/
bool RayTriangleIntersect(  const osg::Vec3 &orig, const osg::Vec3 &dir, const osg::Vec3 &v0, 
					const osg::Vec3 &v1, const osg::Vec3 &v2, 
					double &t, double &u, double &v);

/************************************************************************
  射线与包围AABB是否有相交
   rayOrigin:射线起点
   dir:   射线方向向量
   Min,Max: AABB
   tNear/tFar : 射线起始点距离 进入/离开box距离
************************************************************************/
bool RayAABB( const osg::Vec3d& min, const osg::Vec3d& max,const osg::Vec3d&rayOrigin, const osg::Vec3d& rayDir,
	double &tNear, double &tFar );
#endif