#include "StdAfx.h"
#include "GLbCollisionIntersector.h"
#include "GlbTriangleFunctor.h"
#include <osg/PrimitiveSet>
#include <osg/Notify>
#include <algorithm>    
using namespace Glb_Collision;

/*
	Glb_GetTraingFuctor:该类用来将获取的三角形点信息保存到列表中
	方便后面的碰撞遍历
*/
struct Glb_GetTraingFuctor
{
public:

    typedef Glb_Collision::TraingVec TV;

	Glb_GetTraingFuctor()
	{
        pointsV = Glb_Collision::SweptSphereCollision::Instance().getCurrentPtr();  
    }

	Glb_GetTraingFuctor(Glb_Collision::CollisionPacket *cp)
	{
		cP = cp;
	}

	inline void operator () (const osg::Vec3* v1,const osg::Vec3* v2,const osg::Vec3* v3,
							const osg::Matrixd&local2W)
	{
		//三角形上的三个点地址依次保存到列表中
        TV * newPoints1 = new TV;
        TV * newPoints2 = new TV;
        TV * newPoints3 = new TV;

		newPoints1->point = osg::Vec3d((*v1) * local2W);
		newPoints1->next = newPoints2;
		newPoints2->point = osg::Vec3d((*v2) * local2W);
		newPoints2->next = newPoints3;
		newPoints3->point = osg::Vec3d((*v3) * local2W);

        pointsV->next = newPoints1;
        pointsV = newPoints3;
		//重新设置当前列表的指针，方便下次存储数据
		Glb_Collision::SweptSphereCollision::Instance().setCurrentPtr(pointsV);
    }

	std::vector<osg::Vec3d>& getPoints();
private:
	Glb_Collision::CollisionPacket * cP; //封装的碰撞信息
    Glb_Collision::TraingVec * pointsV; //所有三角形点列表
};


/*
	函数模板 :解决 std::max与Visual C++中的全局的宏max冲突。 
*/

template <class T> const T& MIN (const T& a, const T& b) {
	return !(b<a)?a:b;     // or: return !comp(b,a)?a:b; for version (2)
}
template <class T> const T& MAX (const T& a, const T& b) {
	return (a<b)?b:a;     // or: return comp(a,b)?b:a; for version (2)
}

Glb_CollisionIntersector::Glb_CollisionIntersector(const osg::Vec3d er,const osg::Vec3d ve,const osg::Vec3d pos):
	eRadius(er),velocity(ve),position(pos)
{
	
}

osgUtil::Intersector* Glb_CollisionIntersector::clone( osgUtil::IntersectionVisitor& iv )
{
	if (_coordinateFrame==MODEL && iv.getModelMatrix()==0)
	{
		osg::ref_ptr<Glb_CollisionIntersector> lsi = new Glb_CollisionIntersector(eRadius,velocity,position);
		lsi->_parent = this;
		lsi->_intersectionLimit = this->_intersectionLimit;
		return lsi.release();
	}

	osg::Matrix matrix;
	switch (_coordinateFrame)
	{
	case(WINDOW):
		if (iv.getWindowMatrix()) matrix.preMult( *iv.getWindowMatrix() );
		if (iv.getProjectionMatrix()) matrix.preMult( *iv.getProjectionMatrix() );
		if (iv.getViewMatrix()) matrix.preMult( *iv.getViewMatrix() );
		if (iv.getModelMatrix()) matrix.preMult( *iv.getModelMatrix() );
		break;
	case(PROJECTION):
		if (iv.getProjectionMatrix()) matrix.preMult( *iv.getProjectionMatrix() );
		if (iv.getViewMatrix()) matrix.preMult( *iv.getViewMatrix() );
		if (iv.getModelMatrix()) matrix.preMult( *iv.getModelMatrix() );
		break;
	case(VIEW):
		if (iv.getViewMatrix()) matrix.preMult( *iv.getViewMatrix() );
		if (iv.getModelMatrix()) matrix.preMult( *iv.getModelMatrix() );
		break;
	case(MODEL):
		if (iv.getModelMatrix()) matrix = *iv.getModelMatrix();
		break;
	}

	osg::Matrixd inverse;
	inverse.invert(matrix);

	osg::Vec3d v = osg::Matrixd::transform3x3(velocity,inverse);
	osg::ref_ptr<Glb_CollisionIntersector> lsi = new Glb_CollisionIntersector(eRadius,v,position * inverse);
	lsi->_parent = this;
	lsi->ABaseX = osg::Matrixd::transform3x3(ABaseX,inverse);
	lsi->ABaseY = osg::Matrixd::transform3x3(ABaseY,inverse);
	lsi->ABaseZ = osg::Matrixd::transform3x3(ABaseZ,inverse);
	lsi->localToWorld = matrix;
	lsi->_intersectionLimit = this->_intersectionLimit;
	return lsi.release();
}

bool Glb_CollisionIntersector::enter( const osg::Node& node )
{
	if (reachedLimit()) return false;
	
	return !node.isCullingActive() || intersects( node.getBound() );
}

void Glb_CollisionIntersector::leave()
{

}

void Glb_CollisionIntersector::intersect( osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable )
{
	 if (reachedLimit()) return;
	 /*
		1.暂时使用OBB与drawable AABB相交
		2.如果1步没有相交，则使用moving sphere与AABB相交,此步为了防止人物
		  当前没有碰撞，但是移动过程中发生碰撞,后续可以考虑使用OBB与AABB的动态碰撞
	 */

    if(!OBBIntersectAABB(drawable->getBound())) return;

	//如果有交点则，进行drawable遍历
	 GLb_TriangleFunctor<Glb_GetTraingFuctor> tf;
	 tf.setLocalToWorld(localToWorld);
	 drawable->accept(tf);

}  

void Glb_CollisionIntersector::reset()
{

}

bool Glb_CollisionIntersector::containsIntersections()
{
	return false;
}

bool Glb_CollisionIntersector::intersects( const osg::BoundingSphere& bs )
{
    osg::Vec3d center = bs.center();
	return SweptSphere(center,osg::Vec3d(0,0,0),bs.radius());
}


bool Glb_CollisionIntersector::SphereIntersectSphere(osg::Vec3d&cB,osg::Vec3d&vB,double rB)const
{
    //使用椭球的最大半径作为碰撞检测的半径
    double rA = 0.0;
    if(eRadius.x() > eRadius.y())
    {
        if(eRadius.x() > eRadius.z())
        {
            rA = eRadius.x();
        }
        else
        {
            rA = eRadius.z();
        }
    }
    else if (eRadius.y() > eRadius.z())
    {
        rA = eRadius.y();
    }
    else
    {
        rA = eRadius.z();
    }

    osg::Vec3d VAB (velocity.x() - vB.x(),
                    velocity.y() - vB.z(),
                    velocity.z() - vB.z());

    osg::Vec3d I   (cB.x() - position.x(),
                    cB.y() - position.y() ,
                    cB.z() - position.z());

    double r2 = std::pow(rA + rB, 2);
    double I2 = I * I;

	return I2 <= r2;
}

bool Glb_CollisionIntersector::OBBIntersectAABB(const osg::BoundingBox&box)
{

	//osg::Vec3d A[] = {osg::Vec3d(1,0,0),osg::Vec3d(0,1,0),osg::Vec3d(0,0,1)};
	osg::Vec3d A[] = {ABaseX,ABaseY,ABaseZ};
	osg::Vec3d B[] = {osg::Vec3d(1,0,0),osg::Vec3d(0,1,0),osg::Vec3d(0,0,1)}; //根据镜头来调节

	//检测现在椭圆的OBB与AABB是否已经相交(此时是静态碰撞检测)
	bool ok =  OBBOverlap(osg::Vec3d(eRadius.x(),eRadius.y(),eRadius.z()),
		osg::Vec3d(position.x(),position.y(),position.z()),
		A, 
		(box._max - box._min) / 2,
		box.center(),
		B);	
	if(ok) return true; 

	return MovingSphereAABB(box); //进行swept aabb相交 判断是否相交
}

const bool Glb_CollisionIntersector::OBBOverlap( /*A */ const osg::Vec3d& a, /* */ const osg::Vec3d& Pa, 
	const osg::Vec3d*A,  /*B */ const osg::Vec3d& b, /* */ const osg::Vec3d& Pb, 
	const osg::Vec3d* B )
{
	//参考gamedev
	const osg::Vec3d v (Pb - Pa);
	//计算矩阵
	const osg::Vec3d T( v * A[0], v * A[1], v * A[2] );

	double R[3][3];
	double ra, rb, t;
	long i, k;

	//计算旋转矩阵
	for( i=0 ; i<3 ; i++ )
		for( k=0 ; k<3 ; k++ )
			R[i][k] = A[i] * B[k];
	//计算平移矩阵
	for( i=0 ; i<3 ; i++ )
	{
		ra = a[i];
		rb =  b[0]*fabs(R[i][0]) + b[1]*fabs(R[i][1]) + b[2]*fabs(R[i][2]);
		t = fabs( T[i] );
		if( t > ra + rb ) 
			return false;

	}

	//B为基坐标
	for( k=0 ; k<3 ; k++ )
	{
		ra =  a[0]*fabs(R[0][k]) + a[1]*fabs(R[1][k]) + a[2]*fabs(R[2][k]);
		rb = b[k];
		t =  fabs( T[0]*R[0][k] + T[1]*R[1][k] +  T[2]*R[2][k] );
		if( t > ra + rb )
			return false;
	}

	//L = A0 x B0
	ra =  a[1]*fabs(R[2][0]) + a[2]*fabs(R[1][0]);

	rb =  b[1]*fabs(R[0][2]) + b[2]*fabs(R[0][1]);

	t =  fabs( T[2]*R[1][0] -  T[1]*R[2][0] );

	if( t > ra + rb )
		return false;

	//L = A0 x B1
	ra =  a[1]*fabs(R[2][1]) + a[2]*fabs(R[1][1]);

	rb =  b[0]*fabs(R[0][2]) + b[2]*fabs(R[0][0]);

	t =  fabs( T[2]*R[1][1] -  T[1]*R[2][1] );

	if( t > ra + rb )
		return false;

	//L = A0 x B2
	ra =  a[1]*fabs(R[2][2]) + a[2]*fabs(R[1][2]);

	rb =  b[0]*fabs(R[0][1]) + b[1]*fabs(R[0][0]);

	t =  fabs( T[2]*R[1][2] -  T[1]*R[2][2] );

	if( t > ra + rb )
		return false;

	//L = A1 x B0
	ra =  a[0]*fabs(R[2][0]) + a[2]*fabs(R[0][0]);

	rb =  b[1]*fabs(R[1][2]) + b[2]*fabs(R[1][1]);

	t =  fabs( T[0]*R[2][0] -  T[2]*R[0][0] );

	if( t > ra + rb )
		return false;

	//L = A1 x B1
	ra =  a[0]*fabs(R[2][1]) + a[2]*fabs(R[0][1]);

	rb =  b[0]*fabs(R[1][2]) + b[2]*fabs(R[1][0]);

	t =  fabs( T[0]*R[2][1] -  T[2]*R[0][1] );

	if( t > ra + rb )
		return false;

	//L = A1 x B2
	ra =  a[0]*fabs(R[2][2]) + a[2]*fabs(R[0][2]);

	rb =  b[0]*fabs(R[1][1]) + b[1]*fabs(R[1][0]);

	t =  fabs( T[0]*R[2][2] -  T[2]*R[0][2] );

	if( t > ra + rb )
		return false;

	//L = A2 x B0
	ra =  a[0]*fabs(R[1][0]) + a[1]*fabs(R[0][0]);

	rb =  b[1]*fabs(R[2][2]) + b[2]*fabs(R[2][1]);

	t =  fabs( T[1]*R[0][0] -  T[0]*R[1][0] );

	if( t > ra + rb )
		return false;

	//L = A2 x B1
	ra =  a[0]*fabs(R[1][1]) + a[1]*fabs(R[0][1]);

	rb =  b[0] *fabs(R[2][2]) + b[2]*fabs(R[2][0]);

	t =  fabs( T[1]*R[0][1] -  T[0]*R[1][1] );

	if( t > ra + rb )
		return false;

	ra =  a[0]*fabs(R[1][2]) + a[1]*fabs(R[0][2]);

	rb =  b[0]*fabs(R[2][1]) + b[1]*fabs(R[2][0]);

	t =  fabs( T[1]*R[0][2] -  T[0]*R[1][2] );

	if( t > ra + rb )
		return false;

	return true;
}

bool Glb_CollisionIntersector::SweptSphere( osg::Vec3d&cB,osg::Vec3d&vB,double rB ) const
{
	//使用椭球的最大半径作为碰撞检测的半径
	double rA = 0.0;
	if(eRadius.x() > eRadius.y())
	{
		if(eRadius.x() > eRadius.z())
		{
			rA = eRadius.x();
		}
		else
		{
			rA = eRadius.z();
		}
	}
	else if (eRadius.y() > eRadius.z())
	{
		rA = eRadius.y();
	}
	else
	{
		rA = eRadius.z();
	}

	osg::Vec3d VAB (velocity.x() - vB.x(),
		velocity.y() - vB.z(),
		velocity.z() - vB.z());

	osg::Vec3d I(cB.x() - position.x(),
		cB.y() - position.y() ,
		cB.z() - position.z());

	double r2 = std::pow(rA + rB, 2);
	double I2 = I * I;

	//d - >VAB d is normal
	osg::Vec3d d(VAB);
	d.normalize();
	double s = I * d;

	//劲量早的返回不相交 (real time)
	if(I2 >= r2) //开始两球不相交
	{
		if(s < 0) //运行方向和球心连线相反
		{
			return false;
		}
		//映射点大于球心连线
		double m2 = I2 - s * s;
		if(m2 > r2)
		{
			return false; 
		}

		double a = VAB * VAB;
		double b = 2 * ((-I) * VAB);
		double c = I2 - r2;

		double b2 = b * b;
		double sqr = b2 - 4 * a * c;
		if(sqr < 0) 
		{
			return false;
		}

		double q = -(b + _copysign(1.0,b) * std::sqrt(sqr)) / 2;
		double t0 = q / a;
		double t1 = c / q;

		t0 = (std::min)(t0,t1);
		if (t0 >=0 && t0 <= 1) //swept sphere
		{
			return true;
		}
		return false;
	}
	else if(I2 < r2)
	{
		//目前已经相交或者小球在打球内部
		//只要判断下一步小球是否和大球脱离关系即可
		//return (I - VAB).length2() < r2;
		return true;
	}
	return false;
}

bool Glb_CollisionIntersector::SweptAABB( const osg::BoundingBox&box )
{
	/*double u0;
	double u1;
	return AABBSweep
		(
		(box._max - box._min) / 2,box.center(),
		box.center(),cP->eRadius,
		cP->R3Position,	cP->R3Position + cP->R3Velocity,	u0,	u1
		);*/

	double xInvEntry, yInvEntry,zInvEntry;
	double xInvExit, yInvExit,zInvExit;

	if (velocity.x() > 0.0f) //b1 ellipisiod b2 box
	{
		xInvEntry = box._min.x() - (position.x() + eRadius.x());
		xInvExit =  box._max.x() - (position.x() - eRadius.x());
	}
	else 
	{
		xInvEntry = box._max.x() - (position.x() - eRadius.x());
		xInvExit = box._min.x() -  (position.x() + eRadius.x());
	}

	if (velocity.y() > 0.0f)
	{
		yInvEntry = box._min.y() - (position.y() + eRadius.y());
		yInvExit =  box._max.y() - (position.y() - eRadius.y());
	}
	else
	{
		yInvEntry = box._max.y() - (position.y() - eRadius.y());
		yInvExit  =  box._min.y() - (position.y() + eRadius.y());
	}
	if (velocity.z() > 0.0f)
	{
		zInvEntry = box._min.z() - (position.z() + eRadius.z());
		zInvExit =  box._max.z() - (position.z() - eRadius.z());
	}
	else
	{
		zInvEntry = box._max.z() - (position.z() - eRadius.z());
		zInvExit  =  box._min.z() - (position.z() + eRadius.z());
	}

	double xEntry, yEntry,zEntry;
	double xExit, yExit,zExit;

	if (velocity.x() == 0.0f)
	{
		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		xEntry = xInvEntry / velocity.x();
		xExit = xInvExit /velocity.x();
	}

	if (velocity.y() == 0.0f)
	{
		yEntry = -std::numeric_limits<double>::infinity();
		yExit = std::numeric_limits<double>::infinity();
	}
	else
	{
		yEntry = yInvEntry / velocity.y();
		yExit = yInvExit /velocity.y();
	}
	if (velocity.z() == 0.0f)
	{
		zEntry = -std::numeric_limits<double>::infinity();
		zExit = std::numeric_limits<double>::infinity();
	}
	else
	{
		zEntry = zInvEntry / velocity.z();
		zExit = zInvExit / velocity.z();
	}
	
	double entryTime = MAX(MAX(xEntry, yEntry),zEntry);
	double exitTime  = MIN(MIN(xExit, yExit),zExit);
	
	/*std::cout << "entryTime: " << entryTime << std::endl;
	std::cout << "exitTime: " << exitTime << std::endl;*/

	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f && zEntry < 0.0 || xEntry > 1.0 || yEntry > 1.0 || zEntry > 1.0)
	{
		return false;
	}

	else // if there was a collision
	{        		
		return true; //AB首次相交
	}
}

Glb_Collision::VECTOR Corner(const osg::BoundingBox&box, int n)
{
	Glb_Collision::VECTOR p;
	p.x() = ((n & 1) ? box._max.x() : box._min.x());
	p.y() = ((n & 1) ? box._max.y() : box._min.y());
	p.z() = ((n & 1) ? box._max.z() : box._min.z());
	return p;
}

bool Glb_CollisionIntersector::MovingSphereAABB( const osg::BoundingBox&box )
{
	double rA = 0.0;
	if(eRadius.x() > eRadius.y())
	{
		if(eRadius.x() > eRadius.z())
		{
			rA = eRadius.x();
		}
		else
		{
			rA = eRadius.z();
		}
	}
	else if (eRadius.y() > eRadius.z())
	{
		rA = eRadius.y();
	}
	else
	{
		rA = eRadius.z();
	}

	/*
		将AABB的所有变外扩球体的半径，然后转换为射线与外包相交
		此时会在AABB的8个定点，多余部分caplsue，后续排除
		相交位置:面区域，定点，边上(线)
	*/
	Glb_Collision::AABB e(box._min,box._max);
	e._min.x() -= rA; e._min.y() -= rA; e._min.z() -= rA;
	e._max.x() += rA; e._max.y() += rA; e._max.z() += rA;
	
	Glb_Collision::VECTOR p;
	Glb_Collision::VECTOR d;
	double t;
	//首先与外扩的AABB相交,没有交点返回
	if (!IntersectRayAABB(position, d, e, t, p) || t > 1.0f)
		return false;

	//下面位运算计算此时射线和外扩体交点的位置，为面上，边上，还是定点上
	int u = 0, v = 0;
	if (p.x() < box._min.x()) u |= 1;
	if (p.x() > box._max.x()) v |= 1;
	if (p.y() < box._min.y()) u |= 2;
	if (p.y() > box._max.y()) v |= 2;
	if (p.z() < box._min.z()) u |= 4;
	if (p.z() > box._max.z()) v |= 4;
	
	int m = u + v;
	////定义球体移动的线段
	Glb_Collision::Segment seg(position, position + velocity);
	Glb_Collision::Capsule cap;
	//如果为定点部位(只是比外扩前AABB大的角部，可能并不是定点)
	if (m == 7) {
		double tmin = DBL_MAX;

		cap.seg.a = Corner(box, v);
		cap.seg.b = Corner(box, v ^ 1);
		cap.r = rA;
		if (IntersectSegmentCapsule(seg, cap, t))
			tmin = MIN(t, tmin);

		cap.seg.a = Corner(box, v);
		cap.seg.b = Corner(box, v^2);
		cap.r = rA;
		if (IntersectSegmentCapsule(seg, cap, t))
			tmin = MIN(t, tmin);

		cap.seg.a = Corner(box, v);
		cap.seg.b = Corner(box, v ^ 4);
		cap.r = rA;
		if (IntersectSegmentCapsule(seg, cap, t))
			tmin = MIN(t, tmin);

		if (tmin == DBL_MAX) 
			return false;  
		t = tmin; //首次相交的时间
		return true; 
	}
	//此时在面上
	if ((m & (m - 1)) == 0) {
		return true;
	}
	//边上
	cap.seg.a = Corner(box, u^7);
	cap.seg.b = Corner(box, v);
	cap.r = rA;
	return IntersectSegmentCapsule(seg, cap, t);
}

bool Glb_CollisionIntersector::IntersectRayAABB( const Glb_Collision::VECTOR& p, const Glb_Collision::VECTOR&d, const Glb_Collision::AABB a, 
	double &tmin,
	Glb_Collision::VECTOR& q )
{
	tmin = 0.0f;  
	float tmax = FLT_MAX;
	for (int i = 0; i < 3; i++) {
		if (abs(d[i]) < DBL_EPSILON) {
			// 平行
			if (p[i] < a._min[i] || p[i] > a._max[i]) return false;
		} else {
			
			double ood = 1.0f / d[i];
			double t1 = (a._min[i] - p[i]) * ood;
			double t2 = (a._max[i] - p[i]) * ood;
	
			if (t1 > t2) std::swap(t1, t2);
			
			if (t1 > tmin) tmin = t1;
			if (t2 > tmax) tmax = t2;
			
			if (tmin > tmax) return false;
		}
	}
	
	q = p + d * tmin;

	return true;
}

bool Glb_CollisionIntersector::IntersectSegmentCapsule( const Glb_Collision::Segment seg, const Glb_Collision::Capsule cap, double& t )
{
	Glb_Collision::VECTOR d = cap.seg.b - cap.seg.a;
	Glb_Collision::VECTOR m = seg.a - cap.seg.a;
	Glb_Collision::VECTOR n = seg.b - seg.a;
	double md = m * d;
	double nd = n * d;
	double dd = d * d;

	if (md < 0.0f && md + nd < 0.0f) { 
		Glb_Collision::Sphere sp(cap.seg.a, cap.r);
		if(IntersectSegmentSphere(seg, sp, t))
			return true;
		return false; 
	}
	if (md > dd && md + nd > dd) { 
		Glb_Collision::Sphere sp(cap.seg.b, cap.r);
		if(IntersectSegmentSphere(seg, sp, t))
			return true;	
		return false;	  
	}

	double nn = n * n;
	double mn = m * n;
	double a = dd * nn - nd * nd;
	double k = m * m - cap.r * cap.r;
	double c = dd * k - md * md;
	if (fabs(a) < DBL_EPSILON) {	
		if (c > 0.0f) 
			return false; 

		if (md < 0.0f){ 
			Glb_Collision::Sphere sp(cap.seg.a, cap.r);
			IntersectSegmentSphere(seg, sp, t);
		}
		else if (md > dd){
			Glb_Collision::Sphere sp(cap.seg.b, cap.r);
			IntersectSegmentSphere(seg, sp, t);
		}
		else {
			t = 0.0f; 
		}
		return true;
	}

	double b = dd * mn - nd * md;
	double discr = b * b - a * c;
	if (discr < 0.0f) 
		return false;  

	t = (-b - sqrt(discr)) / a;
	if (t < 0.0f || t > 1.0f) 
		return false;  

	if (md + (t) * nd < 0.0f) {
		Glb_Collision::Sphere sp(cap.seg.a, cap.r);
		return IntersectSegmentSphere(seg, sp, t);
	} 
	else if (md + (t) * nd > dd) {
		Glb_Collision::Sphere sp(cap.seg.b, cap.r);
		return IntersectSegmentSphere(seg, sp, t);
	}
	return true;	
}

bool Glb_CollisionIntersector::IntersectSegmentSphere( const Glb_Collision::Segment seg, const Glb_Collision::Sphere sp, double& t )
{
	Glb_Collision::VECTOR d = seg.b - seg.a;
	Glb_Collision::VECTOR dir = d;
	dir.normalize();

	Glb_Collision::VECTOR colP;
	if (IntersectRaySphere(seg.a, dir, sp, t, colP)) {
		t = (t) / d.length();
		if (t <= 1)
			return true;   
		else
			return false;
	}
	else
		return false;
}

bool  Glb_CollisionIntersector::IntersectRaySphere(const Glb_Collision::VECTOR& p, Glb_Collision::VECTOR dir, Glb_Collision::Sphere sp,
	double&t, Glb_Collision::VECTOR &colP)
{
	//射线表示:
	// R(t) = o + td  o:射线起始点,d ：射线方向向量，t为首次碰撞时间
	Glb_Collision::VECTOR m = p - sp.c;
	double b = m * dir;
	double c = m * m - sp.r * sp.r;

	if (c > 0.0f && b > 0.0f) 
		return false;

	double discr = b * b - c ;
	if (discr < 0.0f) 
		return false;

	t = -b - sqrt(discr);

	if (t < 0.0) 
		t = 0.0;

	colP = p + dir * (t);

	return true;
}

void Glb_Collision::Glb_CollisionIntersector::setABase(const osg::Vec3d*b)
{
	ABaseX = (b[0]);
	ABaseY = (b[1]);
	ABaseZ = (b[2]);
}

