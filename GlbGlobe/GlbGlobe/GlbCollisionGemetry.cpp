#include "StdAfx.h"
#include "GlbCollisionGemetry.h"

using namespace Glb_Collision;

Glb_Collision::PLANE::PLANE(const VECTOR& origin, const VECTOR& normal)
{
	this->normal = normal;
	this->origin = origin;
	equation[0] = normal.x();
	equation[1] = normal.y();
	equation[2] = normal.z();
	equation[3] = -(normal.x()*origin.x()+normal.y()*origin.y()
		+normal.z()*origin.z());
}

Glb_Collision::PLANE::PLANE(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3)
{
	normal = (p2-p1) ^ (p3-p1);
	normal.normalize();
	origin = p1;
	equation[0] = normal.x();
	equation[1] = normal.y();
	equation[2] = normal.z();
	equation[3] = -(normal.x()*origin.x()+normal.y()*origin.y()
		+normal.z()*origin.z());
}

bool Glb_Collision::PLANE::isFrontFacingTo(const VECTOR& direction) const
{
	double dot = normal * (direction);
	return (dot <= 0);
}

double Glb_Collision::PLANE::signedDistanceTo(const VECTOR& point) const
{
	return (point*(normal)) + equation[3];
}

Glb_Collision::AABB::AABB(const VECTOR& i, const VECTOR& a):_min(i), _max(a)
{
}

Glb_Collision::Segment::Segment()
{}

Glb_Collision::Segment::Segment(const VECTOR&_a,const VECTOR&_b):a(_a),b(_b)
{}

Glb_Collision::Capsule::Capsule()
{

}

Glb_Collision::Sphere::Sphere(const VECTOR&cc,double rr):c(cc),r(rr)
{ }


