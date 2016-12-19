#include "StdAfx.h"
#include "GlbSweptSphereCollision.h"

using namespace Glb_Collision;

VECTOR SweptSphereCollision::CollisionSlide( CollisionPacket& cP)
{
	//将世界坐标系下的椭圆信心转到espace(椭圆坐标系)下
	cP.velocity.x()  = cP.R3Velocity.x()/cP.eRadius.x();
    cP.velocity.y()  = cP.R3Velocity.y()/cP.eRadius.y();
    cP.velocity.z()  = cP.R3Velocity.z()/cP.eRadius.z();
	cP.basePoint.x() = cP.R3Position.x()/cP.eRadius.x();
    cP.basePoint.y() = cP.R3Position.y()/cP.eRadius.y();
    cP.basePoint.z() = cP.R3Position.z()/cP.eRadius.z();

	//连续进行5次碰撞 ，直到之间距离小于非常小的值
	cP.collisionRecursionDepth = 0;
	VECTOR finalPosition = CollideWithWorld(cP);

#if 0
	//重力影响，暂时不使用
	cP.velocity = gravity / cP.eRadius;	
	cP.basePoint = finalPosition;
	cP.collisionRecursionDepth = 0;
	finalPosition = CollideWithWorld(cP);
#endif

	finalPosition.x() = finalPosition.x() * cP.eRadius.x();
    finalPosition.y() = finalPosition.y() * cP.eRadius.y();
    finalPosition.z() = finalPosition.z() * cP.eRadius.z();
	return finalPosition;
}

void SweptSphereCollision::init()
{
	 gravity= VECTOR(0.0f, -0.2f, 0.0f);
	 unitsPerMeter = 100.0f;
	 /*
	 * 首先初始化存放点的列表指针
	 * 为了快速保存和访问三角面片的点信心
	 * 暂时使用指针列表，没有使用容器
	 * 指针直接指向drawable内部存放三角面片点的地址
	 */
     headPtr = new TraingVec;
	 currentPtr = headPtr;
}

VECTOR SweptSphereCollision::CollideWithWorld( CollisionPacket& cP)
{
	double unitScale = unitsPerMeter / 100.0;
	double veryCloseDistance = 0.005 * unitScale;

	if (cP.collisionRecursionDepth > 5)
		return cP.basePoint;
	
	cP.normalizedVelocity = cP.velocity;
    cP.normalizedVelocity.normalize();
	cP.nearestDistance = 0.0;
	cP.foundCollision = false;

	//遍历所有三角面片的信息，进行检测碰撞
	//队列首指针只是个标志，没有存放点信息,
	//从第二个开始
    TraingVec * currentTraing = headPtr->next;
    while (currentTraing != NULL) {

        VECTOR p0 (currentTraing->point.x()/cP.eRadius.x(),
                   currentTraing->point.y()/cP.eRadius.y(),
                   currentTraing->point.z()/cP.eRadius.z());
        currentTraing = currentTraing->next;
        VECTOR p1 (currentTraing->point.x()/cP.eRadius.x(),
                   currentTraing->point.y()/cP.eRadius.y(),
                   currentTraing->point.z()/cP.eRadius.z());
        currentTraing = currentTraing->next;
        VECTOR p2 (currentTraing->point.x()/cP.eRadius.x(),
                   currentTraing->point.y()/cP.eRadius.y(),
                   currentTraing->point.z()/cP.eRadius.z());
        currentTraing = currentTraing->next;
        
		//与三角形求交
        SphereCollidingWithTriangle(cP, p0, p1, p2);
    }

	if (cP.foundCollision == false) 
	{
		return cP.basePoint + cP.velocity;
	}
	// *** 碰撞发生***//
	// 最初的碰撞destination point
	
	VECTOR destinationPoint = cP.basePoint + cP.velocity;
	VECTOR newBasePoint = cP.basePoint;
	//防止与已经检测到的碰撞发生再次碰撞
	if (cP.nearestDistance>=veryCloseDistance)
	{
		VECTOR V = cP.velocity;
		V.normalize();
		V = V * (cP.nearestDistance - veryCloseDistance);
		newBasePoint = cP.basePoint + V;

		V.normalize();
		cP.intersectionPoint -=   V * veryCloseDistance;
	}
	// 确定 slip plane
	VECTOR slidePlaneOrigin =cP.intersectionPoint;
	VECTOR slidePlaneNormal = newBasePoint-cP.intersectionPoint;
	slidePlaneNormal.normalize();
	PLANE slidingPlane(slidePlaneOrigin,slidePlaneNormal);

	VECTOR newDestinationPoint = destinationPoint - slidePlaneNormal * slidingPlane.signedDistanceTo(destinationPoint);
	//确定新的velocity vector
	VECTOR newVelocityVector = newDestinationPoint - cP.intersectionPoint;
	
	if (newVelocityVector.length() < veryCloseDistance) {
		return newBasePoint;
	}

	cP.collisionRecursionDepth++;
	cP.basePoint = newBasePoint;
	cP.velocity = newVelocityVector;
	return CollideWithWorld(cP);
}

bool SweptSphereCollision::SphereCollidingWithTriangle( CollisionPacket& cP,  VECTOR &p1,  VECTOR &p2,  VECTOR &p3 )
{
	PLANE trianglePlane(p1,p2,p3);
	// 目前只检测正面朝向平面
	if (trianglePlane.isFrontFacingTo(cP.normalizedVelocity))
	{
		double t0, t1;
		bool embeddedInPlane = false;
		double signedDistToTrianglePlane = trianglePlane.signedDistanceTo(cP.basePoint);
		double normalDotVelocity = trianglePlane.normal * cP.velocity;

		if (normalDotVelocity == 0.0f) //平行
		{
			if (fabs(signedDistToTrianglePlane) >= 1.0f)
			{
				// 没有碰撞
				return false;
			}
			else
			{
				//球体嵌入三角形所在平面
				embeddedInPlane = true;
				t0 = 0.0;
				t1 = 1.0;
			}
		}
		else 
		{
			//不平行则必然与平面相交,计算相交的t0,t1
			t0=(-1.0-signedDistToTrianglePlane)/normalDotVelocity;
			t1=( 1.0-signedDistToTrianglePlane)/normalDotVelocity;
			if (t0 > t1) 
			{
				double temp = t1;
				t1 = t0;
				t0 = temp;
			}
			if (t0 > 1.0f || t1 < 0.0f)
			{
				return false;
			}
			// Clamp
			if (t0 < 0.0) t0 = 0.0;
			if (t1 < 0.0) t1 = 0.0;
			if (t0 > 1.0) t0 = 1.0;
			if (t1 > 1.0) t1 = 1.0;
		}

		VECTOR collisionPoint;
		bool foundCollison = false;
		double t = 1.0;

		//球体没有嵌入到了平面
		if (!embeddedInPlane) 
		{
			VECTOR planeIntersectionPoint = (cP.basePoint-trianglePlane.normal)
											+ (cP.velocity * t0);
			//相交点是否在三角形内部
			if (checkPointInTriangle(planeIntersectionPoint, p1,p2,p3))
			{
				foundCollison = true;
				t = t0;
				collisionPoint = planeIntersectionPoint;
			}
		}
		//如果没有找到collision，则进行与三角形的定点和边的碰撞检测
		//首先检测点p1,p2,p3
		if (foundCollison == false) 
		{
			VECTOR velocity = cP.velocity;
			VECTOR base		= cP.basePoint;
			double velocitySquaredLength = velocity.length2();
			double a,b,c;  
			double newT;

			a = velocitySquaredLength;
			// P1
			b = 2.0*(velocity * (base-p1));
			c = (p1-base).length2() - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) 
			{
				t = newT;
				foundCollison = true;
				collisionPoint = p1;
			}
			// P2
			b = 2.0*(velocity * (base-p2));
			c = (p2-base).length2() - 1.0;
			if (getLowestRoot(a,b,c, t, &newT))
			{
				t = newT;
				foundCollison = true;
				collisionPoint = p2;
			}
			// P3
			b = 2.0*(velocity * (base-p3));
			c = (p3-base).length2() - 1.0;
			if (getLowestRoot(a,b,c, t, &newT))
			{
				t = newT;
				foundCollison = true;
				collisionPoint = p3;
			}
			//边
			// p1 -> p2:
			VECTOR edge = p2-p1;
			VECTOR baseToVertex = p1 - base;
			double edgeSquaredLength = edge.length2();
			double edgeDotVelocity = edge * (velocity);
			double edgeDotBaseToVertex = edge * (baseToVertex);

			a = edgeSquaredLength*-velocitySquaredLength +
				edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(velocity * baseToVertex * 2) -2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-baseToVertex.length2())+
				edgeDotBaseToVertex*edgeDotBaseToVertex;

			if (getLowestRoot(a,b,c, t, &newT))
			{
				// Check if intersection is within line segment:
				double f=(edgeDotVelocity*newT-edgeDotBaseToVertex)/
					edgeSquaredLength;
				if (f >= 0.0 && f <= 1.0) 
				{
					// intersection took place within segment.
					t = newT;
					foundCollison = true;
					collisionPoint = p1 + edge * f;
				}
			}
			// p2 -> p3:
			edge = p3-p2;
			baseToVertex = p2 - base;
			edgeSquaredLength = edge.length2();
			edgeDotVelocity = edge * (velocity);
			edgeDotBaseToVertex = edge * (baseToVertex);
			a = edgeSquaredLength*-velocitySquaredLength +
				edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(velocity * 2 * (baseToVertex))-
				2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-baseToVertex.length2())+
				edgeDotBaseToVertex*edgeDotBaseToVertex;
			if (getLowestRoot(a,b,c, t, &newT)) 
			{
				double f=(edgeDotVelocity*newT-edgeDotBaseToVertex)/
					edgeSquaredLength;
				if (f >= 0.0 && f <= 1.0)
				{
					t = newT;
					foundCollison = true;
					collisionPoint = p2 + edge * f;
				}
			}
			// p3 -> p1:
			edge = p1-p3;
			baseToVertex = p3 - base;
			edgeSquaredLength = edge.length2();
			edgeDotVelocity = edge * (velocity);
			edgeDotBaseToVertex = edge * (baseToVertex);
			a = edgeSquaredLength*-velocitySquaredLength +
				edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(velocity * 2 * (baseToVertex))-
				2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-baseToVertex.length2())+
				edgeDotBaseToVertex*edgeDotBaseToVertex;
			if (getLowestRoot(a,b,c, t, &newT))
			{
				double f=(edgeDotVelocity*newT-edgeDotBaseToVertex)/
					edgeSquaredLength;
				if (f >= 0.0 && f <= 1.0) 
				{
					t = newT;
					foundCollison = true;
					collisionPoint = p3 + edge * f;
				}
			}
		}
		
		if (foundCollison == true) 
		{
			double distToCollision = t*cP.velocity.length();
			if (cP.foundCollision == false ||
				distToCollision < cP.nearestDistance) 
			{
				cP.nearestDistance = distToCollision;
				cP.intersectionPoint=collisionPoint;
				cP.foundCollision = true;
			}
		}
	} 
	
	return false;
}

void SweptSphereCollision::clearTraingVec()
{
    TraingVec * current = headPtr->next;
    while (current != NULL) {
        TraingVec * tmp = current->next;
        delete current;
        current = tmp;
    }

	headPtr->next = NULL;
	currentPtr = headPtr;
}

Glb_Collision::SweptSphereCollision::SweptSphereCollision(void)
{
	init();
}

Glb_Collision::SweptSphereCollision::SweptSphereCollision(const SweptSphereCollision&)
{

}

 
/*

	使用 Barycentric Technique,参考:http://www.blackpawn.com/texts/pointinpoly/
	点是否在三角形内部,前提是该点在三角形所在的平面
	也可以使用Same Side Technique，或者三角形的
	重心坐标方式
*/
bool Glb_Collision::SweptSphereCollision::checkPointInTriangle(const VECTOR& point, const VECTOR& pa,const VECTOR& pb, const VECTOR& pc)
{
#if 0
	VECTOR e10=pb-pa;     //v0
	VECTOR e20=pc-pa;     //v1
	VECTOR vp =point - pa; //v2
	//dot 
	double a = e10.dot(e10); //v0 v0
	double b = e10.dot(e20); //v0 v1
	double c = e20.dot(e20);  //v1 v1
	double d = vp.dot(e10); //v2 v0
	double e = vp.dot(e20); //v2 v1

	double ac_bb=(a*c)-(b*b);
	double x = (d*c)-(e*b);
	double y = (e*a)-(d*b);
	double z = x+y-ac_bb;
	return (( in(z)& ~(in(x)|in(y)) ) & 0x80000000);
#else
	VECTOR v0 = pc - pa;
	VECTOR v1 = pb - pa;
	VECTOR v2 = point - pa;

	//dot 
	double dot00 = v0 * v0;
	double dot01 = v0 * v1;
	double dot02 = v0 * v2;
	double dot11 = v1 * v1;
	double dot12 = v1 * v2;

	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return (u >= 0) && (v >= 0) && (u + v < 1);
#endif
}

bool Glb_Collision::SweptSphereCollision::getLowestRoot(double a, double b, double c, double maxR, double* root)
{

		double determinant = b*b - 4.0f*a*c;
		if (determinant < 0.0f) return false;

		double sqrtD = sqrt(determinant);
		double r1 = (-b - sqrtD) / (2*a);
		double r2 = (-b + sqrtD) / (2*a);
		
		if (r1 > r2) 
		{
			double temp = r2;
			r2 = r1;
			r1 = temp;
		}
		if (r1 > 0 && r1 < maxR) 
		{
			*root = r1;
			return true;
		}

		if (r2 > 0 && r2 < maxR) {
			*root = r2;
			return true;
		}
		return false;
}

Glb_Collision::SweptSphereCollision::~SweptSphereCollision(void)
{

}

