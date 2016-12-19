#include "Stdafx.h"
#include "GlbGlobeMath.h"
#include <osg/Vec2d>

#define DOTEPSILON   0.000000001
#define MOLLER_TRUMBORE //是否使用 Moller-Trumbore 算法
#define CULLING //是否测试三角形的正反面


//点(p_x,p_y)到线段(p1,p2)的最短距离
double pointToLine(double seg_sx,double seg_sy,double seg_ex,double seg_ey, double ptx,double pty)
{
	double ans = 0;
	double a, b, c;

	a = sqrt((seg_sx-seg_ex)*(seg_sx-seg_ex)+(seg_sy-seg_ey)*(seg_sy-seg_ey));//distance(p1, p2);

	b = sqrt((seg_sx-ptx)*(seg_sx-ptx)+(seg_sy-pty)*(seg_sy-pty));//distance(p1, p);

	c = sqrt((seg_ex-ptx)*(seg_ex-ptx)+(seg_ey-pty)*(seg_ey-pty));//distance(p2, p);

	if (c+b==a) {//点在线段上
		ans = 0;
		return ans;
	}

	if (a<=0.00001) {//不是线段，是一个点
		ans = b;
		return ans;
	}

	if (c*c >= a*a + b*b) { //组成直角三角形或钝角三角形，p1为直角或钝角
		ans = b;
		return ans;
	}

	if (b * b >= a * a + c * c) {// 组成直角三角形或钝角三角形，p2为直角或钝角
		ans = c;
		return ans;
	}

	// 组成锐角三角形，则求三角形的高
	double p0 = (a + b + c) / 2;// 半周长
	double s = sqrt(p0 * (p0 - a) * (p0 - b) * (p0 - c));// 海伦公式求面积
	ans = 2*s / a;// 返回点到线的距离（利用三角形面积公式求高）
	return ans;
}

bool PtInPolygon(double px,double py, double* polygon_pts, int ptCnt, int coordDimension)
{
	int nCross = 0;
	for(int i=0;i<ptCnt-1;i++)
	{
		double p1_x = polygon_pts[i*coordDimension];
		double p1_y = polygon_pts[i*coordDimension+1];
		double p2_x = polygon_pts[(i+1)*coordDimension];
		double p2_y = polygon_pts[(i+1)*coordDimension+1];
		// 求解 y=p.y 与 p1p2 的交点
		if(p1_y == p2_y)// p1p2 与 y=p0.y平行
			continue;
		if ( py < min(p1_y, p2_y) ) // 交点在p1p2延长线上
			continue;
		if ( py >= max(p1_y, p2_y) ) // 交点在p1p2延长线上
			continue;
		// 求交点的 X 坐标 --------------------------------------------------------------
		double x = (double)(py - p1_y) * (double)(p2_x - p1_x) / (double)(p2_y - p1_y) + p1_x;
		if(x > px)
			nCross++; // 只统计单边交点
	}
	// 单边交点为偶数，点在多边形之外 ---
	return (nCross%2==1);
}

bool PtInPolygonExt(double px,double py, double* polygon_pts, int ptCnt, int coordDimension)
{
	bool oddNodes=false;
	int nCross = 0;
	for(int i=0;i<ptCnt-1;i++)
	{
		double p1_x = polygon_pts[i*coordDimension];
		double p1_y = polygon_pts[i*coordDimension+1];
		double p2_x = polygon_pts[(i+1)*coordDimension];
		double p2_y = polygon_pts[(i+1)*coordDimension+1];

		if((p1_y< py && p2_y>=py||  p2_y<py && p1_y>=py)
			&&  (p1_x<=px || p2_x<=px))
		{
			if(p1_x+(py-p1_y)/(p2_y-p1_y)*(p2_x-p1_x)<px) 
			{
				oddNodes=!oddNodes;
			}
		}

	}
	return oddNodes;

}

double Cross(osg::Vec2d v0,osg::Vec2d v1)
{
	//P × Q = x1*y2 - x2*y1
	return v0.x()*v1.y() - v1.x()*v0.y();
}

bool Intersect(double segment1_sx,double segment1_sy,double segment1_ex,double segment1_ey, 
				double segment2_sx,double segment2_sy,double segment2_ex,double segment2_ey,
				double inter_x, double inter_y)
{// 计算几何方法[矢量算法]

	// 1.  设以线段 P1P2 为对角线的矩形为R1， 设以线段 Q1Q2 为对角线的矩形为R2，如果R1和R2不相交，则两线段不会有交点
	double seg1_minx = min(segment1_sx,segment1_ex);
	double seg1_maxx = max(segment1_sx,segment1_ex);
	double seg1_miny = min(segment1_sy,segment1_ey);
	double seg1_maxy = max(segment1_sy,segment1_ey);

	double seg2_minx = min(segment2_sx,segment2_ex);
	double seg2_maxx = max(segment2_sx,segment2_ex);
	double seg2_miny = min(segment2_sy,segment2_ey);
	double seg2_maxy = max(segment2_sy,segment2_ey);

	double minx = max(seg1_minx,seg2_minx);
	double miny = max(seg1_miny,seg2_miny);
	double maxx = min(seg1_maxx,seg2_maxx);
	double maxy = min(seg1_maxy,seg2_maxy);
	if (minx>maxx || miny>maxy)
		return false;

	// 2.  跨立试验
	//如果两线段相交，则两线段必然相互跨立对方，所谓跨立，指的是一条线段的两端点分别位于另一线段所在直线的两边。
	//判断是否跨立，还是要用到矢量叉积的几何意义。若P1P2跨立Q1Q2 ，则矢量 ( P1 - Q1 ) 和( P2 - Q1 )位于矢量( Q2 - Q1 ) 的两侧，即：
	//( P1 - Q1 ) × ( Q2 - Q1 ) * ( P2 - Q1 ) × ( Q2 - Q1 ) < 0
	//	上式可改写成：
	//	( P1 - Q1 ) × ( Q2 - Q1 ) * ( Q2 - Q1 ) × ( P2 - Q1 ) > 0
	//	当 ( P1 - Q1 ) × ( Q2 - Q1 ) = 0 时，说明线段P1P2和Q1Q2共线（但是不一定有交点）。同理判断Q1Q2跨立P1P2的依据是：
	//	( Q1 - P1 ) × ( P2 - P1 ) * ( Q2 - P1 ) × ( P2 - P1 ) < 0
	osg::Vec2d P1(segment1_sx,segment1_sy);
	osg::Vec2d P2(segment1_ex,segment1_ey);
	osg::Vec2d Q1(segment2_sx,segment2_sy);
	osg::Vec2d Q2(segment2_ex,segment2_ey);	
	// P × Q = x1*y2 - x2*y1
	// 叉积的一个非常重要性质是可以通过它的符号判断两矢量相互之间的顺逆时针关系：
	//	　　若 P × Q > 0 , 则P在Q的顺时针方向。
	//  　　若 P × Q < 0 , 则P在Q的逆时针方向。
	//	　　若 P × Q = 0 , 则P与Q共线，但可能同向也可能反向。
	double a = Cross(P1 - Q1,Q2 - Q1);//(P1 - Q1) * (Q2 - Q1);
	double b = Cross(P2 - Q1,Q2 - Q1);//(P2 - Q1) * (Q2 - Q1);

	double c = Cross(Q1 - P1,P2 - P1);//(Q1 - P1) * (P2 - P1);
	double d = Cross(Q2 - P1,P2 - P1);// (Q2 - P1) * (P2 - P1);

	if (a*b<0 && c*d<0)
	{
		inter_x = segment1_sx;
		inter_y = segment1_sy;
		double t=((segment1_sx-segment2_sx)*(segment2_sy-segment2_ey)-(segment1_sy-segment2_sy)*(segment2_sx-segment2_ex))
			/((segment1_sx-segment1_ex)*(segment2_sy-segment2_ey)-(segment1_sy-segment1_ey)*(segment2_sy-segment2_ex));
		inter_x+=(segment1_ex-segment1_sx)*t;
		inter_y+=(segment1_ey-segment1_sy)*t;
		return true;
	}
	return false;
}

bool Intersect(CGlbLine* ln1, CGlbLine* ln2)
{
	if (!ln1 || !ln2)	return false;
	// 1. 两个线的外包矩形相交
	CGlbExtent* ext1 = const_cast<CGlbExtent*>(ln1->GetExtent());
	CGlbExtent* ext2 = const_cast<CGlbExtent*>(ln2->GetExtent());
	glbDouble minX1,minX2,minY1,minY2,maxX1,maxX2,maxY1,maxY2;
	ext1->Get(&minX1,&maxX1,&minY1,&maxY1);
	ext2->Get(&minX2,&maxX2,&minY2,&maxY2);
	glbDouble minX = max(minX1,minX2);
	glbDouble minY = max(minY1,minY2);
	glbDouble maxX = min(maxX1,maxX2);
	glbDouble maxY = min(maxY1,maxY2);
	if (minX<=maxX && minY<=maxY)
	{
		// 判断线是否闭合
		glbInt32 ptCnt = ln1->GetCount();
		glbDouble x0,x1,y0,y1;
		ln1->GetPoint(0,&x0,&y0);
		ln1->GetPoint(ptCnt-1,&x1,&y1);
		glbDouble close_dis = sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
		glbBool bLn1Closed = false;
		if (close_dis<0.0000001)
			bLn1Closed = true;
		ptCnt = ln2->GetCount();
		ln2->GetPoint(0,&x0,&y0);
		ln2->GetPoint(ptCnt-1,&x1,&y1);
		close_dis = sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
		glbBool bLn2Closed = false;
		if (close_dis<0.0000001)
			bLn2Closed = true;

		if (bLn1Closed && bLn2Closed)
		{// 两条线都闭合，使用多边形求交算法
			glbref_ptr<CGlbPolygon> poly1 = new CGlbPolygon();
			glbref_ptr<CGlbPolygon> poly2 = new CGlbPolygon();
			poly1->SetExtRing(ln1);
			poly2->SetExtRing(ln2);

			bool bInter = Intersect(poly1.get(),poly2.get());
			return bInter;
		}
		else if (bLn1Closed)
		{// 线1闭合，线2不闭合
			glbref_ptr<CGlbPolygon> poly = new CGlbPolygon();			
			poly->SetExtRing(ln1);

			bool bInter = Intersect(ln2,poly.get());
			return bInter;
		}
		else if (bLn2Closed)
		{// 线2闭合， 线1不闭合
			glbref_ptr<CGlbPolygon> poly = new CGlbPolygon();			
			poly->SetExtRing(ln2);

			bool bInter = Intersect(ln1,poly.get());
			return bInter;
		}
		else
		{// 两条线都不闭合
			// 2. 线线求交
			glbInt32 ptCnt1 = ln1->GetCount();
			glbInt32 ptCnt2 = ln2->GetCount();
			glbDouble ptx[4];
			glbDouble pty[4];

			for(glbInt32 i = 0; i < (ptCnt1-1); i++)
			{
				ln1->GetPoint(i,&ptx[0],&pty[0]);
				ln1->GetPoint(i,&ptx[1],&pty[1]);
				for(glbInt32 j = 0; j < (ptCnt2-1); j++)
				{
					ln2->GetPoint(j,&ptx[2],&pty[2]);
					ln2->GetPoint(j,&ptx[3],&pty[3]);

					bool binter = Intersect(ptx[0],pty[0],ptx[1],pty[1],ptx[2],pty[2],ptx[3],pty[3]);
					// 有交集返回true
					if(binter)
						return true;
				}		
			}
		}
	}	
	return false;
}

bool Intersect(CGlbLine* ln, CGlbPolygon* poly)
{// 目前算法只支持单环多边形
	if (!ln || !poly)	return false;

	glbInt32 coordDimension = poly->GetCoordDimension();
	CGlbLine* ringLn = const_cast<CGlbLine*>(poly->GetExtRing());
	if (!ringLn)		return false;

	// 1. 线和多边形的外包矩形相交
	CGlbExtent* ext1 = const_cast<CGlbExtent*>(ln->GetExtent());
	CGlbExtent* ext2 = const_cast<CGlbExtent*>(poly->GetExtent());
	glbDouble minX1,minX2,minY1,minY2,maxX1,maxX2,maxY1,maxY2;
	ext1->Get(&minX1,&maxX1,&minY1,&maxY1);
	ext2->Get(&minX2,&maxX2,&minY2,&maxY2);
	glbDouble minX = max(minX1,minX2);
	glbDouble minY = max(minY1,minY2);
	glbDouble maxX = min(maxX1,maxX2);
	glbDouble maxY = min(maxY1,maxY2);
	if (minX<=maxX && minY<=maxY)
	{

		// 提示多环情况处理不了
		glbInt32 inringCnt = poly->GetInRingCount();
		if (inringCnt>0)
		{// 多环
			printf("error ! too much rings， cannot do this!");
			return false;
		}

		glbInt32 ptCnt;
		// 判断线是否闭合
		ptCnt = ln->GetCount();
		glbDouble x0,x1,y0,y1;
		ln->GetPoint(0,&x0,&y0);
		ln->GetPoint(ptCnt-1,&x1,&y1);
		glbDouble close_dis = sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
		glbBool bLnClosed = false;
		if (close_dis<0.0000001)
			bLnClosed = true;

		if (bLnClosed && ptCnt>3)
		{// 线闭合，转为多边形与多边形求交
			glbref_ptr<CGlbPolygon> newpoly = new CGlbPolygon();			
			newpoly->SetExtRing(ln);
			bool bInter = Intersect(newpoly.get(),poly);
			return bInter;
		}

		// 2. 包含测试 ： 线上某点在多边形内，则线与多边形相交
		ptCnt = ln->GetCount();
		glbDouble ptx,pty;
		glbInt32 polyptCnt = ringLn->GetCount();
		glbDouble* polyPts = (glbDouble*)ringLn->GetPoints();

		for(glbInt32 i = 0; i <ptCnt; i++)
		{
			ln->GetPoint(i,&ptx,&pty);
			bool bInPoly = PtInPolygon(ptx,pty,polyPts,polyptCnt,coordDimension);
			if (bInPoly)
				return true;
		}
		// 3. 相交测试 ： 线上的某段线段与多边形相交，则线与多边形相交	
		glbDouble ptx2,pty2;

		glbDouble _ptx,_pty,_ptx2,_pty2;
		for(glbInt32 i = 0; i <(ptCnt-1); i++)
		{
			ln->GetPoint(i,&ptx,&pty);
			ln->GetPoint(i+1,&ptx2,&pty2);

			for(glbInt32 j = 0; j < (polyptCnt-1); j++)
			{
				ringLn->GetPoint(j,&_ptx,&_pty);
				ringLn->GetPoint(j,&_ptx2,&_pty2);

				bool binter = Intersect(ptx,pty,ptx2,pty2,_ptx,_pty,_ptx2,_pty2);
				// 有交集返回true
				if(binter)
					return true;
			}				
		}

	}
	return false;
}

bool Intersect(CGlbPolygon* poly1, CGlbPolygon* poly2)
{
	if (!poly1 || !poly2)	return false;
	// 1. 多边形和多边形的外包矩形相交
	CGlbExtent* ext1 = const_cast<CGlbExtent*>(poly1->GetExtent());
	CGlbExtent* ext2 = const_cast<CGlbExtent*>(poly2->GetExtent());
	glbDouble minX1,minX2,minY1,minY2,maxX1,maxX2,maxY1,maxY2;
	ext1->Get(&minX1,&maxX1,&minY1,&maxY1);
	ext2->Get(&minX2,&maxX2,&minY2,&maxY2);
	glbDouble minX = max(minX1,minX2);
	glbDouble minY = max(minY1,minY2);
	glbDouble maxX = min(maxX1,maxX2);
	glbDouble maxY = min(maxY1,maxY2);
	if (minX<=maxX && minY<=maxY)
	{
		// 提示多环情况处理不了
		glbInt32 inringCnt1 = poly1->GetInRingCount();
		glbInt32 inringCnt2 = poly2->GetInRingCount();
		if (inringCnt1>0 || inringCnt2>0)
		{// 多环
			printf("error ! too much rings， cannot do this!");
			return false;
		}

		CGlbLine* ringLn1 = (CGlbLine*)poly1->GetExtRing();
		CGlbLine* ringLn2 = (CGlbLine*)poly2->GetExtRing();
		if (!ringLn1 || !ringLn2)
			return false;

		// 2. 包含测试 ： 任意一个多边形的点在另一个多边形内
		glbDouble* pts_1 = (glbDouble*)ringLn1->GetPoints();
		glbDouble* pts_2 = (glbDouble*)ringLn2->GetPoints();
		glbInt32 ptCnt1 = ringLn1->GetCount();
		glbInt32 ptCnt2 = ringLn2->GetCount();
		if (ptCnt1<3 || ptCnt2<3)
			return false;

		glbInt32 coordDimension1 = poly1->GetCoordDimension();
		glbInt32 coordDimension2 = poly2->GetCoordDimension();

		glbDouble ptx,pty;
		glbInt32 i;
		for(i = 0; i <ptCnt1; i++)
		{
			ringLn1->GetPoint(i,&ptx,&pty);
			bool bInPoly = PtInPolygon(ptx,pty,pts_2,ptCnt2,coordDimension2);
			if (bInPoly)
				return true;
		}
		for(i = 0; i <ptCnt1; i++)
		{
			ringLn2->GetPoint(i,&ptx,&pty);
			bool bInPoly = PtInPolygon(ptx,pty,pts_1,ptCnt1,coordDimension1);
			if (bInPoly)
				return true;
		}

		// 3. 相交测试 ： 分别判断一个多边形的外环是否与另一个多边形相交
		glbDouble ptx2,pty2;
		glbDouble _ptx,_pty,_ptx2,_pty2;
		for(i = 0; i <(ptCnt1-1); i++)
		{
			ringLn1->GetPoint(i,&ptx,&pty);
			ringLn1->GetPoint(i+1,&ptx2,&pty2);

			for(glbInt32 j = 0; j < (ptCnt2-1); j++)
			{
				ringLn2->GetPoint(j,&_ptx,&_pty);
				ringLn2->GetPoint(j,&_ptx2,&_pty2);

				bool binter = Intersect(ptx,pty,ptx2,pty2,_ptx,_pty,_ptx2,_pty2);
				// 有交集返回true
				if(binter)
					return true;
			}				
		}
	}
	return false;
}

glbBool intersect3D_SegmentPoint( osg::LineSegment *Seg1,osg::Vec3d point,glbDouble threshold )
{
	osg::Vec3d linePoint = Seg1->start();
	osg::Vec3d lineDir = Seg1->start() - Seg1->end();
	//先求出点到直线的垂足位置（根据两向量垂直）
	double t = ( lineDir.x() * (point.x() - linePoint.x()) + 
		lineDir.y() * (point.y() - linePoint.y()) + 
		lineDir.z() * (point.z() - linePoint.z()) ) / 
		(lineDir.x() * lineDir.x() + lineDir.y() * lineDir.y() + lineDir.z() * lineDir.z());

	osg::Vec3d pedalPoint = lineDir * t + Seg1->start();
	//垂足与点的距离确定点在直线上
	double temp1 = (point.x() - pedalPoint.x()) * (point.x() - pedalPoint.x());
	double temp2 = (point.y() - pedalPoint.y()) * (point.y() - pedalPoint.y());
	double temp3 = (point.z() - pedalPoint.z()) * (point.z() - pedalPoint.z());
	double dirDis = sqrt(fabs(temp1) + fabs(temp2) + fabs(temp3));

	if(dirDis < threshold)
		return true;
	return false;
}

glbBool intersect3D_SegmentSegment( osg::LineSegment *Seg1,osg::LineSegment *Seg2,glbDouble threshold,osg::Vec3d &point )
{
	osg::Vec3d segDir1 = Seg1->end() - Seg1->start();
	osg::Vec3d segDir2 = Seg2->end() - Seg2->start();
	//求公垂线
	osg::Vec3d tempDir = segDir1 ^ segDir2;

	osg::Vec3d tempAB = Seg1->start() - Seg2->start();
	//tempAB.normalize();//这里不能normalize，因为要计算投影长度
	double dirDis = fabs(tempAB * tempDir) / sqrt(tempDir.x() * tempDir.x() + 
		tempDir.y() * tempDir.y() + tempDir.z() * tempDir.z());

	tempDir.normalize();
	osg::Vec3d transDir(-tempDir.x() * dirDis,-tempDir.y() * dirDis,-tempDir.z() * dirDis);

	if(dirDis < threshold)//表示两直线相交??这个值现在有问题??按像素长度确定阈值
	{//平移公垂线之后两线段相交，方程联立求交点位置
		double t = ( (Seg2->start().x() - Seg1->start().x() - transDir.x()) * (Seg2->start().y() - Seg2->end().y()) - 
			(Seg2->start().y() - Seg1->start().y() - transDir.y()) * (Seg2->start().x() - Seg2->end().x()) ) / 
			( (Seg2->start().y() - Seg2->end().y()) * (Seg1->start().x() - Seg1->end().x()) - 
			(Seg2->start().x() - Seg2->end().x())*(Seg1->start().y() - Seg1->end().y()) );

		//double t2 = ( (Seg1->start().x() - Seg1->end().x()) * (Seg2->start().y() - Seg1->start().y() - transDir.y()) - 
		//	(Seg1->start().y() - Seg1->end().y()) * (Seg2->start().x() - Seg1->start().x() - transDir.x()) ) / 
		//	( (Seg1->start().y() - Seg1->end().y()) * (Seg2->start().x() - Seg2->end().x()) - 
		//	(Seg1->start().x() - Seg1->end().x())*(Seg2->start().y() - Seg2->end().y()) );

		if(t <= 0 && t >= -1)
		{
			point.x() = Seg1->start().x() - segDir1.x() * t;
			point.y() = Seg1->start().y() - segDir1.y() * t;
			point.z() = Seg1->start().z() - segDir1.z() * t;
			return true;
		}
	}
	return false;
}

glbBool intersect3D_SegmentPlane( osg::LineSegment *S,osg::Plane *Pn,osg::Vec3d pPoint,osg::Vec3d &point )
{
	osg::Vec3d lineDir = S->end() - S->start();
	lineDir.normalize();
	osg::Vec3d planeDir = Pn->getNormal();
	osg::Vec3d tempDir = S->start() - pPoint;
	tempDir.normalize();
	float temp = planeDir * tempDir;//连线到面法线的投影长度
	if(fabs(lineDir * planeDir) < 0.00000001)//两向量垂直
	{
		if(fabs(temp) < 0.00000001)
			return true;//相交，直线全在平面内
		else
			return false;//没有交集，直线与平面平行
	}

	double t = 0.0;
	osg::Vec4d factor = Pn->asVec4();
	//根据点到面的距离为0求解交点位置
	t = -(factor.x() * S->start().x() + factor.y() * S->start().y() + factor.z() * S->start().z() + factor.w()) / 
		(lineDir.x() * factor.x() + lineDir.y() * factor.y() + lineDir.z() * factor.z());

	point.x() = S->start().x() + lineDir.x() * t;
	point.y() = S->start().y() + lineDir.y() * t;
	point.z() = S->start().z() + lineDir.z() * t;
	return true;
}

glbInt32 IntersectRaySphere( osg::Vec3d startPos,osg::Vec3d endPos,osg::Vec3d centerPos,glbDouble radius,std::vector<osg::Vec3d>& IntersectPos )
{
	std::vector<osg::Vec3d>().swap(IntersectPos);
	osg::Vec3d vecPos = endPos - startPos;
	/************************************************************************/
	/* 求方程((end-start)*t + start).leng() = radius^2 的解    
	/* 即a*X^2 + b*X + c = 0的解
	/************************************************************************/
	glbDouble a = vecPos.length2();
	glbDouble b = vecPos*startPos;
	glbDouble c = startPos.length2() - radius*radius;
	glbDouble discr = b*b - a*c;
	glbInt32 result = 0;
	if (a == 0)
		return 0;
	if (discr == 0)	//单解
	{
		glbDouble t = -b/a;
		if (t <= 1.0 && t>= 0.0)
		{
			osg::Vec3d Pos = startPos + vecPos*t;
			IntersectPos.push_back(Pos);
			result++;
		}
	}else if(discr > 0)	//双解
	{	
		if (a > 0)
		{		
			glbDouble t = -(b+sqrt(discr))/a;
			if (t <= 1.0 && t >= 0.0)
			{
				osg::Vec3d Pos = startPos + vecPos*t;
				IntersectPos.push_back(Pos);
				result++;
			}
			t = -(b-sqrt(discr))/a;
			if (t <= 1.0 && t >= 0.0)
			{
				osg::Vec3d Pos = startPos + vecPos*t;
				IntersectPos.push_back(Pos);
				result++;
			}
		}else{
			glbDouble t = -(b-sqrt(discr))/a;
			if (t <= 1.0 && t >= 0.0)
			{
				osg::Vec3d Pos = startPos + vecPos*t;
				IntersectPos.push_back(Pos);
				result++;
			}
			t = -(b+sqrt(discr))/a;
			if (t <= 1.0 && t >= 0.0)
			{
				osg::Vec3d Pos = startPos + vecPos*t;
				IntersectPos.push_back(Pos);
				result++;
			}
		}
	}
	return result;
}

glbBool intersectRayBoundingSphere( osg::Vec3d startPos,osg::Vec3d endPos,const osg::BoundingSphere& bs )
{
	if (!bs.valid()) return false;

	osg::Vec3d _start = startPos;
	osg::Vec3d _end = endPos;

	osg::Vec3d sm = _start - bs._center;
	double c = sm.length2()-bs._radius*bs._radius;
	if (c<0.0) return true;

	osg::Vec3d se = _end-_start;
	double a = se.length2();
	double b = (sm*se)*2.0;
	double d = b*b-4.0*a*c;

	if (d<0.0) return false;

	d = sqrt(d);

	double div = 1.0/(2.0*a);

	double r1 = (-b-d)*div;
	double r2 = (-b+d)*div;

	if (r1<=0.0 && r2<=0.0) return false;

	if (r1>=1.0 && r2>=1.0) return false;

	return true;
}

glbBool intersectRayBoundingBox( osg::Vec3d start,osg::Vec3d end,const osg::BoundingBox &bb )
{
	osg::Vec3d bb_min(bb._min);
	osg::Vec3d bb_max(bb._max);

	double epsilon = 1e-6;

	// compate s and e against the xMin to xMax range of bb.
	if (start.x()<=end.x())
	{
		// trivial reject of segment wholely outside.
		if (end.x()<bb_min.x()) return false;
		if (start.x()>bb_max.x()) return false;

		if (start.x()<bb_min.x())
		{
			// clip s to xMin.
			double r = (bb_min.x()-start.x())/(end.x()-start.x()) - epsilon;
			if (r>0.0) start = start + (end-start)*r;
		}

		if (end.x()>bb_max.x())
		{
			// clip e to xMax.
			double r = (bb_max.x()-start.x())/(end.x()-start.x()) + epsilon;
			if (r<1.0) end = start+(end-start)*r;
		}
	}
	else
	{
		if (start.x()<bb_min.x()) return false;
		if (end.x()>bb_max.x()) return false;

		if (end.x()<bb_min.x())
		{
			// clip e to xMin.
			double r = (bb_min.x()-end.x())/(start.x()-end.x()) - epsilon;
			if (r>0.0) end = end + (start-end)*r;
		}

		if (start.x()>bb_max.x())
		{
			// clip s to xMax.
			double r = (bb_max.x()-end.x())/(start.x()-end.x()) + epsilon;
			if (r<1.0) start = end + (start-end)*r;
		}
	}

	// compate s and e against the yMin to yMax range of bb.
	if (start.y()<=end.y())
	{
		// trivial reject of segment wholely outside.
		if (end.y()<bb_min.y()) return false;
		if (start.y()>bb_max.y()) return false;

		if (start.y()<bb_min.y())
		{
			// clip s to yMin.
			double r = (bb_min.y()-start.y())/(end.y()-start.y()) - epsilon;
			if (r>0.0) start = start + (end-start)*r;
		}

		if (end.y()>bb_max.y())
		{
			// clip e to yMax.
			double r = (bb_max.y()-start.y())/(end.y()-start.y()) + epsilon;
			if (r<1.0) end = start+(end-start)*r;
		}
	}
	else
	{
		if (start.y()<bb_min.y()) return false;
		if (end.y()>bb_max.y()) return false;

		if (end.y()<bb_min.y())
		{
			// clip e to yMin.
			double r = (bb_min.y()-end.y())/(start.y()-end.y()) - epsilon;
			if (r>0.0) end = end + (start-end)*r;
		}

		if (start.y()>bb_max.y())
		{
			// clip s to yMax.
			double r = (bb_max.y()-end.y())/(start.y()-end.y()) + epsilon;
			if (r<1.0) start = end + (start-end)*r;
		}
	}

	// compate s and e against the zMin to zMax range of bb.
	if (start.z()<=end.z())
	{
		// trivial reject of segment wholely outside.
		if (end.z()<bb_min.z()) return false;
		if (start.z()>bb_max.z()) return false;

		if (start.z()<bb_min.z())
		{
			// clip s to zMin.
			double r = (bb_min.z()-start.z())/(end.z()-start.z()) - epsilon;
			if (r>0.0) start = start + (end-start)*r;
		}

		if (end.z()>bb_max.z())
		{
			// clip e to zMax.
			double r = (bb_max.z()-start.z())/(end.z()-start.z()) + epsilon;
			if (r<1.0) end = start+(end-start)*r;
		}
	}
	else
	{
		if (start.z()<bb_min.z()) return false;
		if (end.z()>bb_max.z()) return false;

		if (end.z()<bb_min.z())
		{
			// clip e to zMin.
			double r = (bb_min.z()-end.z())/(start.z()-end.z()) - epsilon;
			if (r>0.0) end = end + (start-end)*r;
		}

		if (start.z()>bb_max.z())
		{
			// clip s to zMax.
			double r = (bb_max.z()-end.z())/(start.z()-end.z()) + epsilon;
			if (r<1.0) start = end + (start-end)*r;
		}
	}

	// OSG_NOTICE<<"clampped segment "<<s<<" "<<e<<std::endl;

	// if (s==e) return false;

	return true;
}

glbBool  intersectSegmentAABBTest(const osg::Vec3d start,const osg::Vec3d end,const osg::Vec3d Min,
	const osg::Vec3d Max)
{
	//采用separating axis 映射到坐标轴上进行比较

	osg::Vec3d c = (Min + Max) * 0.5f; //Box 中心点
	osg::Vec3d e = Max - c;            // Box外扩
	
	osg::Vec3d m = (start + end) * 0.5f;   // 线段中点
	osg::Vec3d d = end - m;				  //  线段外扩
	m = m - c;                           // 平移线段和AABB到原点

	
	glbDouble adx = fabs(d.x());
	if (fabs(m.x()) > e.x() + adx) return false;
	glbDouble ady = fabs(d.y());
	if (fabs(m.y()) > e.y() + ady) return false;
	glbDouble adz = fabs(d.z());
	if (fabs(m.z()) > e.z() + adz) return false;

	//当线段接近平行于坐标轴时候加上一个epsilon 
	adx += DBL_MIN; ady += DBL_MIN; adz += DBL_MIN;

	if (fabs(m.y() * d.z() - m.z() * d.y()) > e.y() * adz + e.z() * ady) return false;
	if (fabs(m.z() * d.x() - m.x() * d.z()) > e.x() * adz + e.z() * adx) return false;
	if (fabs(m.x() * d.y() - m.y() * d.x()) > e.x() * ady + e.y() * adx) return false;

	 
	return true;
}


#define NUMDIM	3
#define RIGHT	0
#define LEFT	1
#define MIDDLE	2

glbBool GLB_DLLCLASS_EXPORT intersectRayAABB(const osg::Vec3d& origin, const osg::Vec3d& dir,
	const osg::Vec3d&minB,const osg::Vec3d&maxB,osg::Vec3d coord)
{
	char inside = TRUE;
	char quadrant[NUMDIM];
	register int i;
	int whichPlane;
	double maxT[NUMDIM];
	double candidatePlane[NUMDIM];

	/* Find candidate planes; this loop can be avoided if
   	rays cast all from the eye(assume perpsective view) */
	for (i=0; i<NUMDIM; i++)
		if(origin[i] < minB[i]) {
			quadrant[i] = LEFT;
			candidatePlane[i] = minB[i];
			inside = FALSE;
		}else if (origin[i] > maxB[i]) {
			quadrant[i] = RIGHT;
			candidatePlane[i] = maxB[i];
			inside = FALSE;
		}else	{
			quadrant[i] = MIDDLE;
		}

	/* Ray origin inside bounding box */
	if(inside)	{
		coord = origin;
		return (TRUE);
	}


	/* Calculate T distances to candidate planes */
	for (i = 0; i < NUMDIM; i++)
		if (quadrant[i] != MIDDLE && dir[i] !=0.)
			maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
		else
			maxT[i] = -1.;

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	for (i = 1; i < NUMDIM; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.) return (FALSE);
	for (i = 0; i < NUMDIM; i++)
		if (whichPlane != i) {
			coord[i] = origin[i] + maxT[whichPlane] *dir[i];
			if (coord[i] < minB[i] || coord[i] > maxB[i])
				return (FALSE);
		} else {
			coord[i] = candidatePlane[i];
		}
	return (TRUE);				/* ray hits box */
}


bool RayTriangleIntersect(  const osg::Vec3 &orig, const osg::Vec3 &dir, 
	const osg::Vec3 &v0, const osg::Vec3 &v1, const osg::Vec3 &v2, 
	double &t, double &u, double &v)
{
#ifdef MOLLER_TRUMBORE 
	//三角形两个边的向量
	osg::Vec3 v0v1 = v1 - v0; 
	osg::Vec3 v0v2 = v2 - v0;

	osg::Vec3 pvec = dir ^ v0v2; 
	float det = v0v1 * pvec;

#ifdef CULLING 
	// 如果det为负，则为三角形背面
	// 如果det接近为0，则射线与三角形接近平行
	if (det < DOTEPSILON) return false; 
#else 
	// 射线三角形平行
	if (fabs(det) < DOTEPSILON) return false; 
#endif 

	float invDet = 1 / det; 

	osg::Vec3 tvec = orig - v0; 
	u = (tvec * (pvec)) * invDet; 

	if (u < 0 || u > 1) return false; 

	osg::Vec3 qvec = tvec ^ v0v1; 
	v = (dir * (qvec)) * invDet; 
	if (v < 0 || u + v > 1) return false; 

	t = (v0v2 * (qvec)) * invDet; 

	return true; 
#else 
	// 计算三角形所在的平面
	osg::Vec3 v0v1 = v1 - v0; 
	osg::Vec3 v0v2 = v2 - v0; 

	osg::Vec3 N = v0v1 ^ v0v2; 
	float denom = N * N; 

	// Step 1:找出平面

	// 判断平面和射线是否平行
	float NdotRayDirection = N * dir; 
	if (fabs(NdotRayDirection) < kdTreeEpsilon)
		return false; // 平行则不相交

	// 计算距离
	float d = N * v0; 


	t = (N * orig + d) / NdotRayDirection; 
	//判断三角形是否在射线背面
	if (t < 0) return false;  

	// 计算交点
	osg::Vec3 P = orig + dir * t ; 

	// 交点是否在平面内部
	osg::Vec3 C; 

	// edge 0
	osg::Vec3 edge0 = v1 - v0; 
	osg::Vec3 vp0 = P - v0; 
	C = edge0 ^ vp0; 
	if (N * C < 0) return false; // P 在三角形内部

	// edge 1
	osg::Vec3 edge1 = v2 - v1; 
	osg::Vec3 vp1 = P - v1; 
	C = edge1 ^ vp1; 
	if ((u = N * C) < 0)  return false; 

	// edge 2
	osg::Vec3 edge2 = v0 - v2; 
	osg::Vec3 vp2 = P - v2; 
	C = edge2 ^ vp2; 
	if ((v = N * C) < 0) return false; 

	u /= denom; 
	v /= denom; 

	return true; 
#endif
}

bool RayAABB(const osg::Vec3d& min, const osg::Vec3d& max,const osg::Vec3d& ray_o,
	const osg::Vec3d& ray_dir, double &t_near, double &t_far)
{
	osg::Vec3d dirfrac( 1.0f / ray_dir.x(), 1.0f / ray_dir.y(), 1.0f / ray_dir.z() );

	double t1 = ( min.x() - ray_o.x() ) * dirfrac.x();
	double t2 = ( max.x() - ray_o.x() ) * dirfrac.x();
	double t3 = ( min.y() - ray_o.y() ) * dirfrac.y();
	double t4 = ( max.y() - ray_o.y() ) * dirfrac.y();
	double t5 = ( min.z() - ray_o.z() ) * dirfrac.z();
	double t6 = ( max.z() - ray_o.z() ) * dirfrac.z();

	double tmin = MAX( MAX( MIN( t1, t2 ), MIN( t3, t4 ) ), MIN( t5, t6 ) );
	double tmax = MIN( MIN( MAX( t1, t2 ), MAX( t3, t4 ) ), MAX( t5, t6 ) );

	// If tmax < 0, 则box与射线相交，但是box在整个射线的后面
	if ( tmax < 0.0f )
	{
		return false;
	}

	//不相交
	if ( tmin > tmax ) 
	{
		return false;
	}

	t_near = tmin;
	t_far = tmax;

	return true;
}

