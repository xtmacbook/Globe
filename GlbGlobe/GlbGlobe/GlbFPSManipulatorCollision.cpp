#include "StdAfx.h"
#include "GlbFPSManipulator.h"
#include "GlbGlobeView.h"
#include "GlbGlobeMath.h"
#include "GLbCollisionIntersector.h"
#include "GlbSweptSphereCollision.h"

using namespace GlbGlobe;

//#define SWEPTSPHERE  //是否使用swept sweep碰撞

extern struct classcomp 
{
	bool operator() (const glbref_ptr<CGlbGlobeRObject>& lhs, const glbref_ptr<CGlbGlobeRObject>& rhs) const
	{return lhs->GetId() < rhs->GetId();}
};
typedef std::map<glbInt32,glbref_ptr<CGlbGlobeRObject> > ::const_iterator MapIter;
typedef std::map<glbInt32,std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  > > ::iterator LayMapIter;
typedef std::map<glbInt32,std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  > > ::const_iterator ConstLayMapIter;


static std::map<glbInt32,std::set<glbref_ptr<CGlbGlobeRObject>,classcomp > >		mpr_CullisionLays; //图层对象
static std::map<glbInt32,glbref_ptr<CGlbGlobeRObject> >					mpr_CollisionObjs; //需要做碰撞的对象

//前进时只要三维场景求交
glbBool CGlbFPSManipulator::cullisionDir(const osg::Vec3d moveDir, glbDouble& distance)
{

	glbBool cullison = false;

	if(0 ==mpr_CollisionObjs.size() && 0 == mpr_CullisionLays.size()) return cullison;

	osg::Vec3d up = getWorldUp();

	osg::Vec3d viewDir = getCameraVector();
	//osg::Vec3d localUp = localToWorldVec(osg::Vec3d(0,1,0));
	osg::Vec3d rightDir = viewDir ^ up;

#if 0 //单根射线
	osg::Vec3d currentCameraXYZ = getCameraXYZ();
	osg::Vec3d newCameraXYZ = currentCameraXYZ + moveDir * distance;

	glbDouble dis;
	return intersect(currentCameraXYZ,newCameraXYZ,dis);


#else //从视锥体进裁剪面4根射线
	osg::Vec3d planeUp = up;
	osg::Vec3d planeFrontDir = up ^ rightDir;

	osg::Vec3d cameraXYZ = getCameraXYZ();
	osg::Camera * main_Camera = mpr_globe->GetView()->GetOsgCamera();
	const osg::Matrixd& pm  = main_Camera->getProjectionMatrix();

	glbDouble frontDis = pm(3,2)/(pm(2,2)-1.0); 
	glbDouble left     = frontDis * (pm(2,0)-1.0) / pm(0,0);
	glbDouble right    = frontDis * (1.0+pm(2,0)) / pm(0,0);

	glbDouble deltaX = std::abs(left - right) / 2;
	glbDouble deltaZ = std::abs(frontDis);
	glbDouble bufferY = 0.2;

	osg::Vec3d rightN = planeFrontDir ^ planeUp;


	std::vector<osg::Vec3d> nowPoints;
	std::vector<osg::Vec3d> afterPoints;

	osg::Vec3d nearLeftTop  = cameraXYZ - rightN * deltaX;
	osg::Vec3d nearRightTop = cameraXYZ + rightN * deltaX;
	osg::Vec3d nearRightBtn = nearRightTop - planeUp * bufferY;  //near
	osg::Vec3d nearLeftBtn  = nearLeftTop - planeUp * bufferY;

	osg::Vec3d farLeftTop  = nearLeftTop + planeFrontDir * deltaZ;  //far
	osg::Vec3d farRightTop = farLeftTop + rightN * deltaX * 2;
	osg::Vec3d farRightBtn = farRightTop  - planeUp * bufferY;
	osg::Vec3d farLeftBtn  = farLeftTop - planeUp * bufferY;

	nowPoints.push_back(nearLeftTop);
	nowPoints.push_back(nearRightTop);
	nowPoints.push_back(nearRightBtn);
	nowPoints.push_back(nearLeftBtn);

	nowPoints.push_back(farLeftTop);
	nowPoints.push_back(farRightTop);
	nowPoints.push_back(farRightBtn);
	nowPoints.push_back(farLeftBtn);

	for (unsigned int i = 0;i < nowPoints.size();i++)
	{
		afterPoints.push_back(nowPoints[i] + moveDir * distance);
	}

	osg::ref_ptr<osgUtil::IntersectorGroup> intersectorGroup = new osgUtil::IntersectorGroup();

	for(unsigned int i=0; i<nowPoints.size(); i++)
	{
		//向前四根ray
		osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = 
			new osgUtil::LineSegmentIntersector(nowPoints[i], afterPoints[i]);
		intersectorGroup->addIntersector( intersector.get() );
	}

	return intersectorWithNode(intersectorGroup);
#endif
}

//进行面片计算返回true,不进行则返回false
glbBool GlbGlobe::lineSegmentSphereIntersect(const osg::Vec3d&o,const osg::Vec3d&e,const osg::Vec3d&c,GLdouble r)
{
	const osg::Vec3d l(c - o);
	GLdouble l2 = l * l;
	GLdouble r2 = r * r;
	if(l2 < r2) return true;  //线段起始点在圆内

	//线段起始点在圆外
	osg::Vec3d d = (e - o); //线段的方向向量
	osg::Vec3d dd = d;
	d.normalize();
	GLdouble   s = l * d; //连线在线段向量的投影
	if(s < 0.0)  return false;

	GLdouble s2 = s * s;
	GLdouble m2 = l2 - s2;
	if(m2 > r2) return false; 

	GLdouble ll = dd * dd;
	if((l2 - m2) > ll) return false;
	return true;
}

glbBool CGlbFPSManipulator::intersect(const osg::Vec3d start, osg::Vec3d& end,glbDouble&distance) const

{
	osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(start,end);

	osgUtil::IntersectionVisitor iv(lsi.get());

	std::vector<osg::Vec3d> intersections;

	MapIter bg = mpr_CollisionObjs.cbegin();
	MapIter eg = mpr_CollisionObjs.cend();
	while (bg != eg)
	{
		osg::Node* node = bg->second->GetOsgNode();
		if (node!=NULL)
		{
			const osg::BoundingSphere sphere = node->getBound();

			//线段和球求教 只是判断是否相交不求交点
			if(lineSegmentSphereIntersect(start,end,sphere.center(),sphere.radius()))
			{
				node->accept(iv);			
				if (lsi->containsIntersections())
				{
					intersections.push_back(lsi->getIntersections().begin()->getWorldIntersectPoint());
				}
			}
		}
		bg++;
	}

	ConstLayMapIter bbg = mpr_CullisionLays.cbegin();
	ConstLayMapIter eeg = mpr_CullisionLays.cend();
	while (bbg != eeg)
	{
		const std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  > &objSet = bbg->second;
		std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  >::const_iterator setB = objSet.cbegin();
		std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  >::const_iterator setE = objSet.cend();

		while(setB != setE)
		{
			osg::Node* node = (*setB)->GetOsgNode();
			if(node)
			{
				const osg::BoundingSphere sphere = node->getBound();
				if(lineSegmentSphereIntersect(start,end,sphere.center(),sphere.radius()))
				{
					node->accept(iv);
					if (lsi->containsIntersections())
					{
						intersections.push_back(lsi->getIntersections().begin()->getWorldIntersectPoint());
					}
				}
			}
			setB++;
		}
		bbg++;
	}

	unsigned int index = 0;
	glbDouble minLen = DBL_MAX  ;
	glbDouble tmp = 0.0;
	//获取最近的碰撞点
	for(unsigned int i = 0;i < intersections.size();i++)
	{
		if( (tmp = (intersections[i] - start).length()) < minLen )
		{
			minLen = tmp;
			index = i;
		}
	}

	if(!intersections.empty())
	{
		end = intersections[index];
		distance = minLen;
	}

	return !intersections.empty();
}

osg::Vec3d CGlbFPSManipulator::moveCameraBySweptSphere(osg::Vec3d cameraXYZ,osg::Vec3d cameraVDir)
{
	Glb_Collision::CollisionPacket cp;
	cp.eRadius = Glb_Collision::VECTOR(0.3,0.2,0.5);

	cp.R3Position.x() = cameraXYZ.x();
	cp.R3Position.y() = cameraXYZ.y();
	cp.R3Position.z() = cameraXYZ.z();

	cp.R3Velocity.x() = cameraVDir.x();
	cp.R3Velocity.y() = cameraVDir.y();
	cp.R3Velocity.z() = cameraVDir.z();

	osg::Vec3d viewDir = getCameraVector();
	osg::Vec3d up = getWorldUp();
	viewDir.normalize();
	osg::Vec3d zBase = -viewDir;
	osg::Vec3d xBase = up ^ zBase;
	osg::Vec3d yBase = zBase ^ xBase;
	osg::Vec3d CameraOBBBaseAix[] = {xBase,yBase,zBase};

	osg::ref_ptr<Glb_Collision::Glb_CollisionIntersector> lsi = 
		new Glb_Collision::Glb_CollisionIntersector(cp.eRadius,cp.R3Velocity,cp.R3Position);
	lsi->setABase(CameraOBBBaseAix);
	osgUtil::IntersectionVisitor iv(lsi.get());

	MapIter bg = mpr_CollisionObjs.cbegin();
	MapIter eg = mpr_CollisionObjs.cend();
	while (bg != eg)
	{
		osg::Node * node = bg->second->GetOsgNode();
		if(node)
		{
			node->accept(iv);
		}
		bg++;
	}

	ConstLayMapIter bbg = mpr_CullisionLays.cbegin();
	ConstLayMapIter eeg = mpr_CullisionLays.cend();
	while (bbg != eeg)
	{
		const std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  > &objSet = bbg->second;
		std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  >::const_iterator setB = objSet.cbegin();
		std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  >::const_iterator setE = objSet.cend();

		while(setB != setE)
		{
			osg::Node * node = (*setB)->GetOsgNode();
			if(node)
			{
				node->accept(iv);
			}
			setB++;
		}
		bbg++;
	}

	Glb_Collision::VECTOR newPos = Glb_Collision::SweptSphereCollision::Instance().CollisionSlide(cp);

	return osg::Vec3d(newPos.x(),newPos.y(),newPos.z());
}

glbBool GlbGlobe::CGlbFPSManipulator::intersectorWithNode(osgUtil::Intersector * itersector)
{
	glbBool collision = false;

	osgUtil::IntersectionVisitor intersectVisitor( itersector);

	MapIter bg = mpr_CollisionObjs.cbegin();
	MapIter eg = mpr_CollisionObjs.cend();
	while (bg != eg)
	{

		osg::Node * node =  (bg->second->GetOsgNode());
		if(node)
		{
			node->accept(intersectVisitor);
			if (itersector->containsIntersections())
			{
				collision = true;
				break;
			}
		}
		bg++;
	}
	if(!collision)
	{
		ConstLayMapIter bbg = mpr_CullisionLays.cbegin();
		ConstLayMapIter eeg = mpr_CullisionLays.cend();
		while (bbg != eeg)
		{
			const std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  > &objSet = bbg->second;
			std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  >::const_iterator setB = objSet.cbegin();
			std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  >::const_iterator setE = objSet.cend();

			while(setB != setE)
			{
				osg::Node * node = (*setB)->GetOsgNode();
				if(node)
				{
					node->accept(intersectVisitor);
					if (itersector->containsIntersections())
					{
						collision = true;
						break;
					}
				}
				setB++;
			}
			bbg++;
		}
	}

	return collision;
}

void GlbGlobe::CGlbFPSManipulator::GetHaveAddObjsAndLays(std::vector<glbInt32>& objIds,std::vector<glbInt32>& layIds)
{
	////对象ids
	for_each(mpr_CollisionObjs.cbegin(),mpr_CollisionObjs.cend(),[&objIds](const std::pair<glbInt32,glbref_ptr<CGlbGlobeRObject> >& iter){

		objIds.push_back(iter.first);
	});

	////图层ids
	for_each(mpr_CullisionLays.cbegin(),mpr_CullisionLays.cend(),[&layIds](const std::pair<glbInt32,std::set<glbref_ptr<CGlbGlobeRObject>,classcomp > >& iter){

		layIds.push_back(iter.first);
	});
}

glbBool GlbGlobe::CGlbFPSManipulator::AddCollisionLaysObject(glbInt32 layId,CGlbGlobeRObject* obj)
{
	LayMapIter lay =  mpr_CullisionLays.find(layId);
	if(lay != mpr_CullisionLays.cend())
	{
		lay->second.insert(obj);
		return true;
	}
	std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  > layObjSet;
	layObjSet.insert(obj);
	mpr_CullisionLays[layId] = layObjSet;

	return true;
}

glbInt32 CGlbFPSManipulator::GetCollisionObjectCount()
{
	glbInt32 num = 0;

	for_each(mpr_CullisionLays.cbegin(),mpr_CullisionLays.cend(),[&num](const std::pair<glbInt32,std::set<glbref_ptr<CGlbGlobeRObject>,classcomp > >& iter){
		num += iter.second.size();
	});

	return mpr_CollisionObjs.size() + num;
}

glbBool CGlbFPSManipulator::AddCollisionObject(CGlbGlobeRObject* obj)
{
	glbInt32 objId = obj->GetId();

	MapIter iter = mpr_CollisionObjs.find(objId);
	if(iter != mpr_CollisionObjs.end())
	{
		return true;
	}

	mpr_CollisionObjs[objId] = obj;

	return true;
}

glbBool CGlbFPSManipulator::RemoveCollisionObjectOrLayerById(int idx,glbBool lays)
{
	glbBool find = false;

	if(!lays)
	{
		MapIter iter = mpr_CollisionObjs.find(idx);

		if(iter != mpr_CollisionObjs.end())
		{
			mpr_CollisionObjs.erase(iter);

			find = true;
		}
	}
	else
	{
		ConstLayMapIter iter = mpr_CullisionLays.find(idx);

		if(iter != mpr_CullisionLays.end())
		{
			mpr_CullisionLays.erase(iter);

			find = true;
		}
	}

	return find;
}

glbBool CGlbFPSManipulator::RemoveAllCollsionObject()
{
	mpr_CollisionObjs.clear();
	mpr_CullisionLays.clear();
	return true;
}

CGlbGlobeRObject* CGlbFPSManipulator::getCollisionObjectByObjId(int idx)
{

	MapIter iter = mpr_CollisionObjs.find(idx);

	if(iter != mpr_CollisionObjs.end())
	{
		return (iter->second).get();
	}

	return NULL;
}

