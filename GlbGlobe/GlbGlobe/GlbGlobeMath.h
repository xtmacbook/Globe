#ifndef _GLBGLOBEMATH_H
#define _GLBGLOBEMATH_H

#include "GlbPoint.h"
#include "GlbLine.h"
#include "GlbPolygon.h"


#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MAX3(a, b, c) MAX( MAX(a ,b) ,c)
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MIN3(a, b, c) MIN( MIN(a, b) ,c)

//��(p_x,p_y)���߶�(p1,p2)����̾���
double pointToLine(double seg_sx,double seg_sy,double seg_ex,double seg_ey, double ptx,double pty);

//�жϵ㣨px,py���Ƿ��ڶ����polygon�� ��polygon_pts ����ε㼯�� ptCnt ����ε����� Ŀǰֻ֧�ֵ������Ķ���� coordDimension ����γ��Ĭ��Ϊ��ά���꣨x,y��
bool PtInPolygon(double px, double py, double* polygon_pts, int ptCnt, int coordDimension=2);

bool PtInPolygonExt(double px,double py, double* polygon_pts, int ptCnt, int coordDimension=2);

//�ж������߶�ln1,ln2�Ƿ��ཻ(sx,sy)��������,(ex,ey)�յ������
bool Intersect(double segment1_sx,double segment1_sy,double segment1_ex,double segment1_ey, 
				double segment2_sx,double segment2_sy,double segment2_ex,double segment2_ey,
				double inter_x=0.0, double inter_y=0.0);

//�ж�������(���߶���)�Ƿ��ཻ
bool Intersect(CGlbLine* ln1, CGlbLine* ln2);

//�ж��߶��������Ƿ��ཻ
bool Intersect(CGlbLine* ln, CGlbPolygon* poly);

//�ж϶�����������Ƿ��ཻ
bool Intersect(CGlbPolygon* poly1, CGlbPolygon* poly2);

double Cross(osg::Vec2d v0,osg::Vec2d v1);

//chenpeng
#include "osg/LineSegment"
#include "osg/Plane"

//��������ཻ
glbBool GLB_DLLCLASS_EXPORT intersect3D_SegmentPoint(osg::LineSegment *Seg1,osg::Vec3d point,glbDouble threshold);

//�������߶��ཻ
glbBool GLB_DLLCLASS_EXPORT intersect3D_SegmentSegment(osg::LineSegment *Seg1,osg::LineSegment *Seg2,glbDouble threshold,osg::Vec3d &point);

//���������ཻ
glbBool GLB_DLLCLASS_EXPORT intersect3D_SegmentPlane(osg::LineSegment *S,osg::Plane *Pn,osg::Vec3d pPoint,osg::Vec3d &point);

//brief �߶�������
//startPos �߶���ʼ��
//endPos �߶���ֹ��
//centerPos �����ĵ�
//radius ��뾶
//IntersectPos ����
//return �������
glbInt32 GLB_DLLCLASS_EXPORT IntersectRaySphere(osg::Vec3d startPos,osg::Vec3d endPos,osg::Vec3d centerPos,glbDouble radius,std::vector<osg::Vec3d>& IntersectPos);

//������ڵ�İ�Χ���ཻ
glbBool GLB_DLLCLASS_EXPORT intersectRayBoundingSphere(osg::Vec3d startPos,osg::Vec3d endPos,const osg::BoundingSphere& bs);

glbBool GLB_DLLCLASS_EXPORT intersectRayBoundingBox(osg::Vec3d start,osg::Vec3d end,const osg::BoundingBox &bb);


/************************************************************************/
/* �߶κ�AABB �Ƿ��н���                                                                     */
/************************************************************************/
glbBool GLB_DLLCLASS_EXPORT intersectSegmentAABBTest(const osg::Vec3d start,const osg::Vec3d end,const osg::Vec3d Min,const osg::Vec3d Max);

/************************************************************************
  �������ΧAABB�Ƿ����ཻ,�����ؽ���
   origin:�������
   dir:   ���߷�������
   Min,Max: AABB
   interPo: ����
   Graphic Gems :395
************************************************************************/
glbBool GLB_DLLCLASS_EXPORT intersectRayAABB(const osg::Vec3d& origin, const osg::Vec3d& dir,const osg::Vec3d&Min,
	const osg::Vec3d&Max,osg::Vec3d&interPo);

/************************************************************************
  �����������η����ཻ
  origin:�������
  dir:   ���߷�������
  v0 v1 v2 ��������������
  //t ������ԭ��������ν���ľ��룬
  //u v �������ε���������ϵ(barycentric coordinate) 
  ///�ο�://http://www.scratchapixel.com/code.php?id=9&origin=/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle
  ����T(u,v) = (1 - u - v)v0 + uv1 + vv2;
************************************************************************/
bool RayTriangleIntersect(  const osg::Vec3 &orig, const osg::Vec3 &dir, const osg::Vec3 &v0, 
					const osg::Vec3 &v1, const osg::Vec3 &v2, 
					double &t, double &u, double &v);

/************************************************************************
  �������ΧAABB�Ƿ����ཻ
   rayOrigin:�������
   dir:   ���߷�������
   Min,Max: AABB
   tNear/tFar : ������ʼ����� ����/�뿪box����
************************************************************************/
bool RayAABB( const osg::Vec3d& min, const osg::Vec3d& max,const osg::Vec3d&rayOrigin, const osg::Vec3d& rayDir,
	double &tNear, double &tFar );
#endif