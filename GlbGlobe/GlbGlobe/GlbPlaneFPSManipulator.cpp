#include "StdAfx.h"
#include "GlbPlaneFPSManipulator.h"
#include "GlbGlobeView.h"
#include "GlbGlobeMath.h"
#include "GlbGlobeManipulator.h"
using namespace GlbGlobe;

#ifndef	ELEVATE_THRESHOLD_FPS 
#define ELEVATE_THRESHOLD_FPS 0.1	//相机在地上时，位于地形之上的阈值，单位m
#endif

#ifndef GROUND_ALTITUDE_THRESHOLD_FPS 
#define GROUND_ALTITUDE_THRESHOLD_FPS 10.0 //相机与地下参考面的阈值
#endif

#ifndef CAMERA_UNDERGROUND_TOP_HOLDPFS
#define CAMERA_UNDERGROUND_TOP_HOLDPFS 0.5 //相机在地下与头顶地形缓冲阈值
#endif

#ifndef FPSEYEFOCUSVAL
#define FPSEYEFOCUSVAL 1000.0  //眼睛距交点位置的距离
#endif


extern glbDouble GlbGlobe::erf(GLdouble x)
{
	// constants
	GLdouble a1 =  0.254829592;
	GLdouble a2 = -0.284496736;
	GLdouble a3 =  1.421413741;
	GLdouble a4 = -1.453152027;
	GLdouble a5 =  1.061405429;
	GLdouble p  =  0.3275911;

	int sign = 1;
	if (x < 0)
		sign = -1;
	x = fabs(x);

	GLdouble t = 1.0/(1.0 + p*x);
	GLdouble y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

	return sign*y;
}
extern GLdouble GlbGlobe::sign(const osg::Vec2d p1, const osg::Vec2d p2, const osg::Vec2d p3)
{
	return (p1.x() - p3.x()) * (p2.y() - p3.y()) - (p2.x() - p3.x()) * (p1.y() - p3.y());
}
extern bool GlbGlobe::pointInTriangle(const osg::Vec2d pt, const osg::Vec2d v1, const osg::Vec2d v2, const osg::Vec2d v3)
{
	bool b1, b2, b3;

	b1 = sign(pt, v1, v2) < 0.0f;
	b2 = sign(pt, v2, v3) < 0.0f;
	b3 = sign(pt, v3, v1) < 0.0f;

	return ((b1 == b2) && (b2 == b3));
}
extern bool GlbGlobe::pointInSegment(const osg::Vec2d pt,const osg::Vec2d v1, const osg::Vec2d v2)
{
	osg::Vec2d a ((v1.x() - pt.x()),(v1.y() - pt.y()));
	osg::Vec2d b ((v2.x() - pt.x()),(v2.y() - pt.y()));

	a.normalize();
	b.normalize();

	return ( a == -b);
}
extern bool GlbGlobe::pointInScene(const osg::Vec3d p,const osg::Vec3d min,const osg::Vec3d max)
{
	bool b0,b1,b2;

	const osg::Vec2d point(p.x(),p.y());

	b0 = pointInTriangle(point,osg::Vec2d(min.x(),min.y()),osg::Vec2d(max.x(),min.y()),osg::Vec2d(min.x(),max.y()));
	b1 = pointInTriangle(point,osg::Vec2d(max.x(),max.y()),osg::Vec2d(min.x(),max.y()),osg::Vec2d(max.x(),min.y()));
	b2 = pointInSegment(point,osg::Vec2d(max.x(),min.y()),osg::Vec2d(min.x(),max.y()));
	
	return b0 || b1 || b2;
}

extern bool GlbGlobe::rayTriangle(osg::Vec3d origin,osg::Vec3d rayDir,osg::Vec3d p0,osg::Vec3d p1,osg::Vec3d p2,osg::Vec3d&interPoint)
{
	osg::Vec3d    u, v, n;              
	osg::Vec3d    dir, w0, w;           
	double        r, a, b;              

	u = p1 - p0;
	v = p2 - p0;
	n = u ^ v;              

	dir = rayDir;              
	w0 = origin - p0;
	a = -(n * w0);
	b = (n * dir);
	if (fabs(b) < DBL_MIN) {     // 平行于三角形
		if (a == 0)                 // 射线在三角形里
			return false;
		else return false;              
	}

	r = a / b;
	if (r < 0.0)                   
		return false;                   

	interPoint = origin +  dir * r;            // 相交点

	// is I inside T?
	float    uu, uv, vv, wu, wv, D;
	uu = u * u;
	uv = u* v ;
	vv = v * v;
	w = interPoint - p0;
	wu = w * u;
	wv = w * v;
	D = uv * uv - uu * vv;
	float s, t;
	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 || s > 1.0)         // 点I在三角形外部
		return false;
	t = (uv * wu - uu * wv) / D;
	if (t < 0.0 || (s + t) > 1.0)  //  点I在三角形外部
		return false;

	return true;                       
}

struct classcomp 
{
	bool operator() (const glbref_ptr<CGlbGlobeRObject>& lhs, const glbref_ptr<CGlbGlobeRObject>& rhs) const
	{return lhs->GetId() < rhs->GetId();}
};
static std::map<glbInt32,std::set<glbref_ptr<CGlbGlobeRObject>,classcomp > >		mpr_CullisionLays; //图层对象
static std::map<glbInt32,glbref_ptr<CGlbGlobeRObject> >					mpr_CollisionObjs; //需要做碰撞的对象

typedef std::map<glbInt32,glbref_ptr<CGlbGlobeRObject> > ::const_iterator MapIter;
typedef std::map<glbInt32,std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  > > ::iterator LayMapIter;
typedef std::map<glbInt32,std::set<glbref_ptr<CGlbGlobeRObject>,classcomp  > > ::const_iterator ConstLayMapIter;

enum GLB_FPS_PLANE_CAMERAKEY
{
	A = 0,
	D,
	W,
	S
};

#define  invalidFlyFocus  osg::Vec3d(DBL_MAX,DBL_MAX,DBL_MAX)

/*                     算法从CGlbGlobeManipulator flyto                                        */
void GlbGlobe::CGlbPlaneFPSManipulator::planeFlyToFromCGlbGlobeManipulatorinit(FLYTYPE flyType,const KeyPoint&pt0,const KeyPoint&pt1,GLdouble loopTime)
{

	_firstPoint				= pt0;
	_lastPoint				= pt1;
	Globe_isfar				= false;

	Globe_firstTime			= DBL_MAX;
	Globe_latestTime		= 0.0;
	Globe_loopTime			= loopTime;
	Globe_pause				= false;
	Globe_Fly_flag			= flyType;

	Globe_matrix.makeIdentity();

}

void GlbGlobe::CGlbPlaneFPSManipulator::planeFlyToFromCGlbGlobeManipulator(osg::NodeVisitor* nv)
{
	
	if(FLYAROUND != Globe_Fly_flag)
	{
		if (!Globe_pause)
		{
			Globe_latestTime = nv->getFrameStamp()->getSimulationTime();
			if (Globe_firstTime==DBL_MAX)
				Globe_firstTime = Globe_latestTime;
			glbDouble tim = Globe_latestTime - Globe_firstTime;
			glbDouble ratio = tim/Globe_loopTime;
			if (ratio > 1.0)
				ratio = 1.0;

			//焦点插值
			osg::Vec3d focusPos = osg::Vec3d(_firstPoint.focusPos.x()+ratio*(_lastPoint.focusPos.x()-_firstPoint.focusPos.x()),
				_firstPoint.focusPos.y()+ratio*(_lastPoint.focusPos.y()-_firstPoint.focusPos.y()),
				_firstPoint.focusPos.z()+ratio*(_lastPoint.focusPos.z()-_firstPoint.focusPos.z()));

			//距离插值
			glbDouble distan = _firstPoint.distan + (_lastPoint.distan-_firstPoint.distan)*ratio;
			if (_lastPoint.distan + _firstPoint.distan < 20000 && Globe_isfar)	//两个关键点高度和比较小时
			{					
				glbDouble mid_distan = 2*(_lastPoint.distan + _firstPoint.distan);
				while (mid_distan < 20000)
					mid_distan *= 2;
				if (ratio < 0.5)
					distan = _firstPoint.distan + (mid_distan-_firstPoint.distan)*ratio*2;
				else
					distan = _lastPoint.distan + (mid_distan-_lastPoint.distan)*(1-ratio)*2;
			}else if (_lastPoint.distan/_firstPoint.distan > 10.0)  //两个关键点高度相差很大时
			{
				distan = _lastPoint.distan - (_lastPoint.distan-_firstPoint.distan)*sqrt(1-ratio*ratio);
			}else if (_lastPoint.distan/_firstPoint.distan < 0.1)	 //两个关键点高度相差很大时
			{
				distan = _firstPoint.distan + (_lastPoint.distan-_firstPoint.distan)*sqrt(1-(1.0-ratio)*(1.0-ratio));
			}
			//保证delt_yaw在(-180,180)之间
			glbDouble delt_yaw = _lastPoint.yaw-_firstPoint.yaw;				
			if(delt_yaw > 180)
				delt_yaw -= 360;
			if(delt_yaw < -180)
				delt_yaw += 360;
			glbDouble yaw = _firstPoint.yaw + delt_yaw*ratio;	//偏航						
			glbDouble pitch = _firstPoint.pitch + (_lastPoint.pitch-_firstPoint.pitch)*ratio;//俯仰

			if (tim > Globe_loopTime*1.05)
			{
				Globe_matrix = osg::Matrixd::translate(0.0,0.0,_lastPoint.distan)*
					osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(_lastPoint.pitch),1.0,0.0,0.0)*
					osg::Matrixd::rotate(osg::DegreesToRadians(-_lastPoint.yaw),0.0,0.0,1.0)*
					osg::Matrixd::translate(_lastPoint.focusPos);

				Globe_pause = true;
			}
			else
			{
				Globe_matrix = osg::Matrixd::translate(0.0,0.0,distan)*
					osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
					osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
					osg::Matrixd::translate(focusPos);
			}
		}
	}
	else if(FLYAROUND == Globe_Fly_flag)
	{
		if (!Globe_pause)
		{
			glbDouble frametime = nv->getFrameStamp()->getSimulationTime();
			glbDouble delt_time = frametime - Globe_latestTime;
			Globe_latestTime = frametime;
			if (Globe_firstTime==DBL_MAX)
			{
				Globe_firstTime = Globe_latestTime;
				delt_time  = 0.0;
			}

			osg::Vec3d focusPos;
			GetFocusPos(focusPos);
	
			glbDouble delt_yaw = 360.0*(delt_time)/Globe_loopTime;
			
			osg::Matrixd mat = getMatrix();
			const osg::Vec3d euler = quatToEuler(mat.getRotate());

			GLdouble mpr_pitch = osg::RadiansToDegrees(euler.x()) - 90.0;
			GLdouble mpr_yaw   =  osg::RadiansToDegrees(-euler.y());
			GLdouble pitch = 0.0;
			GLdouble yaw   = delt_yaw;

			if (mpr_pitch + pitch < -90.0)
				pitch = -90.0 - mpr_pitch;
			else if (mpr_pitch + pitch > 90.0)
				pitch = 90.0 - mpr_pitch;

			//先转pitch,再转yaw
			
			osg::Vec3d rotate_center(focusPos.x(),focusPos.y(),focusPos.z());

			osg::Vec3d rotate_yaw = osg::Vec3d(0.0,0.0,1.0);
			osg::Vec3d rotate_ptch = osg::Vec3d(1.0,0.0,0.0) * 
				osg::Matrixd::rotate(osg::DegreesToRadians(-mpr_yaw),0.0,0.0,1.0);

			// 以rotate_center为中心旋转，所以需要先平移到rotate_center位置
			mat = mat * osg::Matrixd::translate(-rotate_center)*
				osg::Matrixd::rotate(osg::DegreesToRadians(pitch),rotate_ptch)*
				osg::Matrixd::rotate(osg::DegreesToRadians(yaw),rotate_yaw)*
				osg::Matrixd::translate(rotate_center);

			Globe_matrix = mat;
			//相机纠偏
			//osg::Vec3d cameraPos = mat.getTrans();

		}
	}
}

/*                -----------------------------------                          */


GlbGlobe::CGlbPlaneFPSManipulator::CGlbPlaneFPSManipulator(CGlbGlobe* globe):
	mpr_globe(globe),
	mpr_cameraXYZ(osg::Vec3d(0.0,0.0,0.0))
{
	initParams();
}


GlbGlobe::CGlbPlaneFPSManipulator::~CGlbPlaneFPSManipulator()
{
	if(mpr_wasd != nullptr)
	{
		delete[] mpr_wasd;
		mpr_wasd = nullptr;
	}
}

osg::Matrixd GlbGlobe::CGlbPlaneFPSManipulator::getMatrix() const
{
	osg::Quat rotate(mpr_rotateV[0],osg::X_AXIS,mpr_rotateV[1],
		osg::Y_AXIS,mpr_rotateV[2],osg::Z_AXIS);

	return osg::Matrixd(rotate) * osg::Matrixd::translate(mpr_cameraXYZ);
}

osg::Matrixd GlbGlobe::CGlbPlaneFPSManipulator::getInverseMatrix() const
{
	return osg::Matrixd::inverse(getMatrix());
}

void GlbGlobe::CGlbPlaneFPSManipulator::setByMatrix(const osg::Matrixd& matrix)
{
	////////////////////////////////////////////////////

	const osg::Vec3d cameraPos = matrix.getTrans();
	setCameraXYZ(cameraPos);

	//局部坐标系的旋转

	osg::Quat quat = matrix.getRotate();

	fixVerticalAxis(quat,osg::Vec3d(0,0,1),true);

	const osg::Vec3d euler = quatToEuler(quat);

	setCameraPitchYaw(euler);
 
}

void GlbGlobe::CGlbPlaneFPSManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
{
	setByMatrix(osg::Matrixd::inverse(matrix));
}

GlbGlobe::GlbGlobeManipulatorTypeEnum GlbGlobe::CGlbPlaneFPSManipulator::GetType()
{
	return GLB_MANIPULATOR_DRIVE;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
	osg::Vec3d focus;
	GetFocusPos(focus);
	KeyPoint pt0;
	pt0.focusPos = focus;
	pt0.distan = GetDistance();
	pt0.yaw = -GetCameraYaw();
	pt0.pitch = GetCameraPitch() - 90.0;

	KeyPoint pt1;
	pt1.focusPos = osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	pt1.distan = distance;
	pt1.yaw = yaw;
	pt1.pitch = pitch;

	osg::Matrixd mat = osg::Matrixd::translate(0.0,0.0,distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
		osg::Matrixd::translate(pt1.focusPos);
	//对目标点进行相机纠偏
	osg::Vec3d cameraPos = mat.getTrans();
	
	glbBool inScene =  limitCameraByScene(cameraPos);

	//保持相机看向的焦点不变,纠偏方式2，基于焦点的纠偏
	if (!inScene)
	{
		osg::Vec3d lookvec = pt1.focusPos - cameraPos;
		pt1.distan = lookvec.length();

		if (lookvec.z()/pt1.distan > 1.0)
			pt1.pitch = 90.0;
		else if (lookvec.z()/pt1.distan < -1.0)
			pt1.pitch = -90.0;
		else
			pt1.pitch = osg::RadiansToDegrees(asin(lookvec.z()/pt1.distan));
	}

	planeFlyToFromCGlbGlobeManipulatorinit(FLYTO,pt0,pt1,seconds);

	CameraLLHPPY s,t; //s t 暂时不使用
	animationCamer(s,t,3.0,2,false,false);
	return true;	
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, glbBool repeat)
{
	osg::Vec3d cameraPos = getMatrix().getTrans();
	osg::Vec3d focusPos(xOrlon,yOrlat,zOrAlt);
	UpdataMatrix(cameraPos,focusPos);

	KeyPoint pt0;
	KeyPoint pt1;

	planeFlyToFromCGlbGlobeManipulatorinit(FLYAROUND,pt0,pt1,10.0);

	CameraLLHPPY s,t; //s t 暂时不使用
	animationCamer(s,t,3.0,2,false,false);

	return true;
}

// 根据相机位置与焦点位置计算其他参数
void CGlbPlaneFPSManipulator::UpdataMatrix(osg::Vec3d& cameraPos,osg::Vec3d& focusPos)
{ 
	osg::Matrixd matrix = osg::Matrixd::lookAt(cameraPos,focusPos,getWorldUp());

	setByInverseMatrix(matrix);
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::IsFlying()
{
	return (mpr_mouseCallBack.valid() && mpr_mouseCallBack->checkAnimationStart()) ? true :false;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch)
{
	osg::Vec3d mpr_focusPos (xOrlon,yOrlat,zOrAlt);

	GLdouble nestYaw	= -yaw;
	GLdouble nestPitch	= pitch + 90.0;
	
	const osg::Matrixd mt =  osg::Matrixd::translate(0.0,0.0,distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
		osg::Matrixd::translate(mpr_focusPos);

	//相机纠偏
	osg::Vec3d cameraPos = mt.getTrans();
	setCameraPitchYaw(osg::Vec3d(osg::DegreesToRadians(nestPitch),0.0,osg::DegreesToRadians(nestYaw)));
	
	GLdouble deltaZ =  cameraPos.z() -  getTerrianElv(cameraPos.x(),cameraPos.y(),true);
	mpr_camera_is_underGround = (deltaZ > 0.001)? false:true;

	rectifyCameraPos(cameraPos);

	return true;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::GetCameraPos(osg::Vec3d& cameraPos)
{
	cameraPos = mpr_cameraXYZ;
	return true;
}

void GlbGlobe::CGlbPlaneFPSManipulator::moveCameraByKey()
{
	unsigned int keySize = sizeof(mpr_wasd) / sizeof(mpr_wasd[0]);
	for(unsigned int i = 0;i < keySize;i++)
	{
		if(mpr_wasd[i] && i == A)
		{
			/*osg::Vec3d rightDir = getCameraRightVector();
			moveCameraPos(-rightDir,mpr_moveSpeed );*/
			rotatePitchYaw(osg::DegreesToRadians(3.),0.,false);
		}
		if(mpr_wasd[i] && i == D)
		{
			/*osg::Vec3d rightDir = getCameraRightVector();
			moveCameraPos(rightDir,mpr_moveSpeed);*/
			rotatePitchYaw(osg::DegreesToRadians(-3.),0.,false);
		}
		if(mpr_wasd[i] && i == W)
		{
			osg::Vec3d viewDir = getCameraVector();
			/*osg::Vec3d up = getWorldUp();
			osg::Vec3d right = viewDir ^ up;*/
			moveCameraPos(/*up ^ right*/viewDir,mpr_moveSpeed);
		}
		if(mpr_wasd[i] && i == S)
		{
			osg::Vec3d viewDir = getCameraVector();
			/*osg::Vec3d up = getWorldUp();
			osg::Vec3d right = viewDir ^ up;*/
			moveCameraPos(/*up ^ right*/viewDir,-mpr_moveSpeed);
		}
	}
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{

	switch(ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::FRAME):
		{	
			return handleFrame(ea,aa);
		}
	case(osgGA::GUIEventAdapter::RESIZE):
		{
			init(ea,aa);
			aa.requestRedraw();
			return true;
		}
	default:
		break;
	}
	/** Get whether this event has been handled by an event handler or not.*/
	if (ea.getHandled())
		return false;

	/*for events*/
	switch(ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::PUSH):
		{
			setFlyFocus(invalidFlyFocus);
			return handleMousePush( ea, aa );
		}
	case (osgGA::GUIEventAdapter::RELEASE):
		{
			return handleMouseRelease( ea, aa );
		}
	case (osgGA::GUIEventAdapter::DRAG):
		{
			//首先将flyFocus变为无效的点
			setFlyFocus(invalidFlyFocus);
			return handleMouseDrag( ea, aa );
		}
	case (osgGA::GUIEventAdapter::DOUBLECLICK):
		return handleMouseDoubleClick(ea,aa);
	case (osgGA::GUIEventAdapter::SCROLL):
		{
			//首先将flyFocus变为无效的点
			setFlyFocus(invalidFlyFocus);
			return handleScroll(ea,aa);
		}
	case (osgGA::GUIEventAdapter::KEYDOWN):
		{
			//首先将flyFocus变为无效的点
			setFlyFocus(invalidFlyFocus);
			return processKeyDownEvent(ea,aa);
		}
	case (osgGA::GUIEventAdapter::KEYUP):
		{
			return processKeyUpEvent(ea,aa);
		}
	default:
		break;
	}
	return false;
}

glbDouble GlbGlobe::CGlbPlaneFPSManipulator::GetCameraPitch()
{
	const osg::Vec3d py = getCameraPitChYaw();

	return osg::RadiansToDegrees(py.x());
}

glbDouble GlbGlobe::CGlbPlaneFPSManipulator::GetDistance()
{
	osg::Vec3d focus;
	GetFocusPos(focus);

	return (focus - getCameraXYZ()).length();
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::SetUnderGroundMode(glbBool mode)
{
	return mpr_globe->SetUnderGroundMode(mode);
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::SetUndergroundDepth(glbDouble zOrAlt)
{
	return mpr_globe->SetUnderGroundAltitude(zOrAlt);
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::UpdateCameraElevate(glbDouble elevation)
{
	return true;
}

void GlbGlobe::CGlbPlaneFPSManipulator::SetVirtualReferencePlane(glbDouble zOrAltitude)
{
	mpr_referencePlaneZ = zOrAltitude;
}

glbDouble GlbGlobe::CGlbPlaneFPSManipulator::GetPitch()
{
	return osg::RadiansToDegrees(getCameraPitChYaw()[0] - osg::PI_2);
}

void GlbGlobe::CGlbPlaneFPSManipulator::outPFSManipulator()
{

}

glbDouble GlbGlobe::CGlbPlaneFPSManipulator::GetDriveMoveSpeed()
{
	return mpr_moveSpeed;
}

void GlbGlobe::CGlbPlaneFPSManipulator::setFlyTimes(const glbDouble&)
{

}

glbBool CGlbPlaneFPSManipulator::AddCollisionObject(CGlbGlobeRObject* obj)
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

glbBool CGlbPlaneFPSManipulator::RemoveCollisionObjectOrLayerById(int idx,glbBool lays)
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

glbBool CGlbPlaneFPSManipulator::RemoveAllCollsionObject()
{
	mpr_CollisionObjs.clear();
	mpr_CullisionLays.clear();

	return true;
}

CGlbGlobeRObject* CGlbPlaneFPSManipulator::getCollisionObjectByObjId(int idx)
{

	MapIter iter = mpr_CollisionObjs.find(idx);

	if(iter != mpr_CollisionObjs.end())
	{
		return (iter->second).get();
	}

	return NULL;
}

glbInt32 CGlbPlaneFPSManipulator::GetCollisionObjectCount()
{
	glbInt32 num = 0;

	for_each(mpr_CullisionLays.cbegin(),mpr_CullisionLays.cend(),[&num](const std::pair<glbInt32,std::set<glbref_ptr<CGlbGlobeRObject>,classcomp > >& iter){
		num += iter.second.size();
	});

	return mpr_CollisionObjs.size() + num;
}


void GlbGlobe::CGlbPlaneFPSManipulator::setCameraXYZ(const osg::Vec3d XYZ)
{
	mpr_cameraXYZ = XYZ;
}

void GlbGlobe::CGlbPlaneFPSManipulator::setCameraPitchYaw(const osg::Vec3d py)
{
	mpr_rotateV[0] = py.x();
	mpr_rotateV[2] = py.z();
}


osg::Vec3d GlbGlobe::CGlbPlaneFPSManipulator::getCameraPitChYaw() const
{
	return mpr_rotateV;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::StopFlying()
{
	if(mpr_mouseCallBack.valid())
	{
		mpr_mouseCallBack->stopAnimation();
	}
	return true;;
}

glbDouble GlbGlobe::CGlbPlaneFPSManipulator::GetYaw()
{
	return osg::RadiansToDegrees(-getCameraPitChYaw()[2]);
}

glbDouble GlbGlobe::CGlbPlaneFPSManipulator::GetCameraYaw()
{
	const osg::Vec3d py = getCameraPitChYaw();

	return osg::RadiansToDegrees(py.z());
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::GetFocusPos(osg::Vec3d& focusPos)
{
	if(mpr_focus_temp != invalidFlyFocus)
	{
		focusPos = mpr_focus_temp;
		return true;
	}

	osg::Vec3d focus;

	osg::Vec3d eye				= getCameraXYZ();
	osg::Vec3d lookDir			= getCameraVector();
	glbBool isfocus				= mpr_globe->IsRayInterTerrain(eye,lookDir,focus);

	if(!isfocus)
	{
		osg::Viewport* viewport = mpr_globe->GetView()->GetOsgCamera()->getViewport();
		glbDouble centerx,centery;
		mpr_globe->GetView()->getScreenFocusPt(centerx,centery);
		isfocus = mpr_globe->GetView()->PickNearestObject(centerx,centery,focus);
	}

	if(!isfocus)
	{
		const osg::Matrixd m = getInverseMatrix();
		osg::Vec3d up;
		m.getLookAt(eye,focus,up);
		
		if((focus - eye).length() > FPSEYEFOCUSVAL)
		{
			lookDir.normalize();
			/* 暂时眼睛距交点位置距离不能超过FPSEYEFOCUSVAL */
			focus = eye + lookDir * FPSEYEFOCUSVAL; 
		}
	}

	focusPos = focus;
	return true;
}

 

void GlbGlobe::CGlbPlaneFPSManipulator::EnalbeVirtualReferencePlane(glbBool bEnable)
{
	mpr_bUseReferencePlane = bEnable;
}

glbDouble GlbGlobe::CGlbPlaneFPSManipulator::GetVirtualReferencePlane()
{
	return mpr_referencePlaneZ;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::globeManipulator2FPSManipulator()
{
	osg::Vec3d viewDir = getCameraVector();
	viewDir.normalize();

	const osg::Vec2d py(getCameraPitChYaw().x(),getCameraPitChYaw().z());
	osg::Vec3d descendPositionXYZ;

	if(findDescendCameraPosition(getCameraXYZ(),viewDir,descendPositionXYZ) )
	{
		//get now camera
		CameraLLHPPY currentCamera;
		currentCamera.setCameraPos(getCameraXYZ());
		currentCamera.setPY(py);

		CameraLLHPPY targetCamera;
		targetCamera.setCameraPos(descendPositionXYZ);
		targetCamera.setPY(osg::Vec2d(osg::PI_2,osg::DegreesToRadians(currentCamera.getY())));

		animationCamer(currentCamera,targetCamera,60.0,0,false,false);

		return true;
	}

	return false;
}

osg::Vec3d GlbGlobe::CGlbPlaneFPSManipulator::getCameraVector() const
{
	osg::Quat rotate = getMatrix().getRotate();
	return	rotate * osg::Vec3d(0,0,-1);
}

void GlbGlobe::CGlbPlaneFPSManipulator::animationCamer(const CameraLLHPPY s,const CameraLLHPPY t,glbDouble loopTime, unsigned int type,glbBool collision /*= true*/,glbBool refL /*= true*/, ManipulatorOutCallBack pf /*= nullptr*/,glbBool cpf /*= true*/)
{
	bool newCallback = true;

	if(mpr_mouseCallBack.valid())
	{
		newCallback = false;

		if(mpr_mouseCallBack->checkAnimationStart()) return;
	}
	
	if(!newCallback)
	{
		 mpr_mouseCallBack->init();
		 mpr_mouseCallBack->setStartCamera(s);
		 mpr_mouseCallBack->setTargetCamera(t);
	}

	if(newCallback)
	{
		mpr_mouseCallBack = new CFPSPlaneManipulatorCallback(s,t,mpr_globe);
		mpr_mouseCallBack->setManipulator(this);
	}

	mpr_mouseCallBack->setAnimationTime(loopTime);

	CFPSPlaneManipulatorCallback::MoveType tp;
	switch (type)
	{
	case 0:
		tp = CFPSPlaneManipulatorCallback::XYZ;
		break;
	case 2:
		tp = CFPSPlaneManipulatorCallback::GLOBEMANIPULOAOR ;
		break;
	default:
		tp = CFPSPlaneManipulatorCallback::XYZ;
	}
	
	mpr_mouseCallBack->setMoveType(tp);
	mpr_mouseCallBack->setCollision(collision);
	mpr_mouseCallBack->setRefLocation(refL);
	if(pf) mpr_mouseCallBack->setPFCallBack(pf);
	/*if(cpf) mpr_mouseCallBack->setCameraAscendCallBack(
	std::bind(&GlbGlobe::CGlbFPSManipulator::cameraCallBack,this, std::placeholders::_1)
	);*/

	if(newCallback)
	{
		osg::Camera* pCamera = mpr_globe->GetView()->GetOsgCamera();
		if(pCamera)  pCamera->setUpdateCallback(mpr_mouseCallBack);
	}

}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if(mpr_mouseCallBack.valid() && mpr_mouseCallBack->checkAnimationStart())
	{
		CFPSPlaneManipulatorCallback::MoveType mt = mpr_mouseCallBack->getMoveType();

		if(CFPSPlaneManipulatorCallback::GLOBEMANIPULOAOR == mt)
		{
			//setFlyFocus(getFlyFocuse());
			setByMatrix(Globe_matrix);

			rectifyCameraPos(Globe_matrix.getTrans());
		}
		else
		{
			const CameraLLHPPY& currentPPY = mpr_mouseCallBack->getCurrerntCamera();
			const CameraLLHPPY& afterPPY   = mpr_mouseCallBack->getAfterCameraF();

			osg::Vec3d dir = (afterPPY.getCameraPos() - currentPPY.getCameraPos());
			glbDouble len = dir.length();
			dir.length();
			if(!moveCameraPos(dir,len,mpr_mouseCallBack->getCollision(),mpr_mouseCallBack->getRefLocation()))
			{
				mpr_mouseCallBack->stopAnimation();
			}
			else
			{
				setCameraPitchYawAlex(afterPPY.getPY());
			}
		}

	}
	
	moveCameraByKey();
	//{
	//时时检测镜头是否到达地下
	osg::Vec3d cameraXYZ = getCameraXYZ();
	GLdouble elv = getTerrianElv(cameraXYZ.x(),cameraXYZ.y());

	GLdouble delta =  cameraXYZ.z() - elv;

	glbBool camerUnderGround = (delta <  0.001) ? true:false;

	mpr_globe->NotifyCameraIsUnderground(camerUnderGround);

	mpr_camera_is_underGround = camerUnderGround;


	//}

	//{
	//时时检测开启和关闭地下模式
	GLboolean enableUnderGround = mpr_globe->IsUnderGroundMode();
	if(!enableUnderGround)
	{
		//关闭,如果相机在地下，则上移
		if(mpr_camera_is_underGround)
		{

			GLdouble underGroudDepth = -delta;
			GLdouble camerH = elv + mpr_personHigh + underGroudDepth + ELEVATE_THRESHOLD_FPS;

			osg::Vec3d newCameraXYZ = cameraXYZ;
			newCameraXYZ.z() = camerH;

			mpr_camera_is_underGround = false;

			rectifyCameraPos(newCameraXYZ,true);
		}

	}

	//}

	
	us.requestRedraw();

	return false;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::findDescendCameraPosition(const osg::Vec3d currentXYZ,const osg::Vec3d viewDir,osg::Vec3d&XYZ)
{
	osg::Vec3d targetPos;
	//get the intersect of view and scene

	//相机在地上还是地下
	GLdouble elv = getTerrianElv(currentXYZ.x(),currentXYZ.y());
	GLdouble delta =  currentXYZ.z() - elv;

	glbBool camerUnderGround = (delta <  0.001) ? true:false;
	if(camerUnderGround)
	{
		//与场景
		
	}
	else
	{
		glbBool isfocus = mpr_globe->IsRayInterTerrain(currentXYZ,viewDir,targetPos);
		if(isfocus)
		{
			/*获取此处的高程*/
			GLdouble elv = getTerrianElv(targetPos.x(),targetPos.y(),true);
			XYZ = targetPos;
			XYZ.z() = mpr_personHigh + elv;

			return true;
		}
	}
	return false;
}

glbDouble GlbGlobe::CGlbPlaneFPSManipulator::getTerrianElv(const glbDouble&lat,const glbDouble&lon,glbBool nowCamerPos /*= false*/) const
{
	return (nowCamerPos)? mpr_globe->GetElevationAt(lat,lon):
		mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
}

void GlbGlobe::CGlbPlaneFPSManipulator::initParams(void)
{
	mpr_rotateV					= osg::Vec3d(osg::PI_2,0,0);
	mpr_rotateStep				= 15.0;
	mpr_moveSpeed				= 1.0;
	mpr_animationTimes			= 10.0;
	mpr_personHigh				= 2.5;
	mpr_mouseCallBack			= NULL;
	mpr_ascendHith				= 0.0;
	mpr_focus_temp				= invalidFlyFocus;//表示无效的focus点

	mpr_bUseReferencePlane		= false;
	mpr_referencePlaneZ			= -10.0;
	mpr_camera_is_underGround	= false;

	mpr_wasd   = new glbBool[4];
	mpr_wasd[0] = false;
	mpr_wasd[1] = false;
	mpr_wasd[2] = false;
	mpr_wasd[3] = false;

	if(mpr_globe)
	{
		GLdouble minX,minY,minZ;
		GLdouble maxX,maxY,maxZ;
		mpr_globe->GetTerrainDataset()->GetExtent()->
			Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);

		mpr_SceneMin = osg::Vec3d(minX,minY,minZ);
		mpr_SceneMax = osg::Vec3d(maxX,maxY,maxZ);
	}
}

void GlbGlobe::CGlbPlaneFPSManipulator::setCameraPitchYawAlex(const osg::Vec2 py)
{
	mpr_rotateV[0] = py.x();
	mpr_rotateV[2] = py.y();
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::processKeyDownEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	switch(ea.getKey())
	{
	case (osgGA::GUIEventAdapter::KEY_W):
		{
			mpr_wasd[W] = true;
			moveCameraByKey();
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_S):
		{
			mpr_wasd[S] = true;
			moveCameraByKey();
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_A):
		{
			mpr_wasd[A] = true;
			moveCameraByKey();
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_D):
		{
			mpr_wasd[D] = true;
			moveCameraByKey();
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_Up):
		{
			rotatePitchYaw(0.,osg::DegreesToRadians(8.),false);
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_Down):
		{
			rotatePitchYaw(0.,osg::DegreesToRadians(-8.),false);
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_Left):
		{
			rotatePitchYaw(osg::DegreesToRadians(8.),0.,false);
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_Right):
		{
			rotatePitchYaw(osg::DegreesToRadians(-8.),0.,false);
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_Q): //快速视角水平
		{
			osg::Vec3d py = getCameraPitChYaw();
			py.x() = osg::PI_2;
			setCameraPitchYaw(py);
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_Page_Up):
		{
			ascendCamera(1);
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_Page_Down):
		{
			/*mpr_personHigh -= 1.0;
			rectifyCameraPos(getCameraLLHDegree());*/
			ascendCamera(-1);
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_KP_Add): // '+'
		{
			mpr_moveSpeed *= 1.2;
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_KP_Subtract): //'-'
		{
			mpr_moveSpeed *= 1/1.2;
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_Space):
		{
			//moveCamerUnOrUpGround();
			home(ea,aa);
			return true;			
		}
	case (osgGA::GUIEventAdapter::KEY_0):
		{
			//just for test
			return true;			
		}
	default:
		break;
	}
	return false;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::handleScroll(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if(IsFlying())
	{
		mpr_mouseCallBack->stopAnimation();
	}

	osg::Camera* pCamera = mpr_globe->GetView()->GetOsgCamera();
	if(!pCamera) return false;

	addMouseEvent(ea);	

	glbDouble zoomDelta = _ga_t0->getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP ? 1.0 : -1.0;
#if 0
	CameraLLHPPY currentCameraPPY;
	currentCameraPPY.setCameraPos(getCameraXYZ());
	currentCameraPPY.setCameraPY(getCameraPitChYaw());

	osg::Vec3d viewDir = getCameraVector();
	viewDir.normalize();

	CameraLLHPPY newCamera ;
	newCamera.setCameraPos(currentCameraPPY.getCameraPos() + viewDir * 50 * zoomDelta);
	newCamera.setPY(currentCameraPPY.getPY());

	animationCamer(currentCameraPPY,newCamera,mpr_animationTimes,0);
#else

	//先求zoomdelt
	glbDouble zoomdelt = DBL_MAX;
	osg::Vec3d cameraXYZ = getCameraXYZ();
	osg::Vec3d focuse;
	GetFocusPos(focuse);
	zoomdelt = (cameraXYZ-focuse).length();

	if (mpr_bUseReferencePlane)
	{// 使用虚拟参考面
		 
		osg::Vec3d IntersectPos;
		glbBool IntersectResult = intersectRayPlane(getCameraXYZ(),getCameraVector(),IntersectPos);
		
		if (IntersectResult)
		{
			glbDouble zoomdelt1 = (IntersectPos-getCameraXYZ()).length();

			if (zoomdelt1 < zoomdelt)
			{// 与虚拟地形交点在其它交点前面
				zoomdelt = zoomdelt1;
			}
		}
	}

	glbDouble ZoomSpeed;
	if (zoomdelt > 100000)		//100km
		ZoomSpeed = 0.3;
	else if (zoomdelt > 1000)	//1km	
		ZoomSpeed = 0.2;
	else						//1km以下
		ZoomSpeed = 0.1;

	if (mpr_globe->IsUnderGroundMode())	//开启了地下模式
	{	
		glbDouble zoom_break = MinElevateThreshold*10.0;
		GLdouble mpr_pitch = GetCameraPitch() - 90.0;
		if (abs(mpr_pitch) > 1.0)
			zoom_break /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
		else
			zoom_break *= 60;
		if (abs(zoomdelt) > zoom_break)
		{
			if (zoomDelta > 0)	//前滚
			{
				zoomdelt = -fabs(zoomdelt) * ZoomSpeed;
			}else{
				zoomdelt = fabs(zoomdelt) * ZoomSpeed;
			}
		}else{	//突变
			if (zoomDelta > 0)	//前滚
				zoomdelt = - (zoomdelt * 2.4);
			else
				zoomdelt = zoomdelt * 2.4;
		}
	}
	else	//未开启地下模式
	{
		if (zoomDelta > 0)	//前滚
		{
			zoomdelt = -fabs(zoomdelt) * ZoomSpeed;
		}else{
			zoomdelt = fabs(zoomdelt) * ZoomSpeed;
		}
	}

	if(mpr_globe->CheckCullisionModel())
	{
		osg::Vec3d viewDir = getCameraVector();
		viewDir.normalize();
		osg::Vec3d newCameraXYZ = cameraXYZ -  viewDir * zoomdelt;
		GLdouble distance;
		glbBool collision = intersect(cameraXYZ,newCameraXYZ,distance);
		if(collision)
		{
			//zoomdelt = (zoomdelt < 0) ? -distance : distance;
			return true; //暂时不前进
		}
	}

	setByMatrix(osg::Matrixd::translate(0.0,0.0,zoomdelt) * getMatrix());
	cameraXYZ = getCameraXYZ();
	limitCameraByScene(cameraXYZ);

	rectifyCameraPos(cameraXYZ);
#endif
	us.requestContinuousUpdate( false );

	us.requestRedraw();
	
	return true;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::performMovement()
{
	if( _ga_t0.get() == NULL || _ga_t1.get() == NULL )
		return false;

	glbDouble eventTimeDelta = _ga_t0->getTime() - _ga_t1->getTime();
	if( eventTimeDelta < 0. )
	{
		OSG_WARN << "Manipulator warning: eventTimeDelta = " << eventTimeDelta << std::endl;
		eventTimeDelta = 0.;
	}

	float dx = _ga_t0->getXnormalized() - _ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized() - _ga_t1->getYnormalized();

	if( dx == 0. && dy == 0. )
		return false;

	unsigned int buttonMask = _ga_t1->getButtonMask();

	if( buttonMask == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		return performMovementLeftMouseButton( eventTimeDelta, dx, dy );
	}
	else if(buttonMask == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{
		glbDouble dx = _ga_t0->getX() - _ga_t1->getX();
		glbDouble dy = _ga_t0->getY() - _ga_t1->getY();

		glbDouble yaw  = -dx*0.3;	//系数暂定0.3
		glbDouble pitch = dy*0.3;

		rotateAboutPoint(mpr_mousePushPoint,pitch,yaw);

		return true;
	}
	return false;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if(IsFlying())
	{
		StopFlying();
	}

	flushMouseEventStack();
	addMouseEvent( ea );

	performMovementLeftAndMiddlePush();

	us.requestContinuousUpdate( false );
	
	return true;

}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	addMouseEvent(ea);

	if( performMovement() )
		us.requestRedraw();

	us.requestContinuousUpdate( false );
	return true;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	flushMouseEventStack();
	return true;
}

void GlbGlobe::CGlbPlaneFPSManipulator::beforePFSManipulator()
{
	mpr_homeMatrix = getMatrix();
}

void GlbGlobe::CGlbPlaneFPSManipulator::setFlyFocus(const osg::Vec3d focusPoint)
{

}

osg::Vec3d GlbGlobe::CGlbPlaneFPSManipulator::getCameraXYZ() const
{
	return mpr_cameraXYZ;
}

void GlbGlobe::CGlbPlaneFPSManipulator::rectifyCameraPos(const osg::Vec3d newPos,glbBool needFootCollision /*= true*/)
{
	//static GLdouble delta =
	osg::Vec3d newPosXYZ = newPos;

	limitCameraByScene(newPosXYZ);

	glbBool isUnderGround    = mpr_camera_is_underGround;
	glbBool enableUnderGound = mpr_globe->IsUnderGroundMode();

	if(!isUnderGround) //地上
	{
		if(needFootCollision)
		{
			const osg::Vec3d up		= getWorldUp();
			osg::Vec3d otherPoint = newPosXYZ - up * (mpr_personHigh) * 5.0;

			//与脚下的求交
			glbDouble distanceToMove;
			glbBool haveCollusion = intersect(newPosXYZ,otherPoint,distanceToMove);
			if(haveCollusion)
			{
				

				if((newPosXYZ.z() - otherPoint.z()) < (ELEVATE_THRESHOLD_FPS + mpr_personHigh))
				{
					newPosXYZ.z() = otherPoint.z() + (ELEVATE_THRESHOLD_FPS + mpr_personHigh);
				}

				setCameraXYZ(newPosXYZ);
				return;
			}
		}

		if(!enableUnderGound)
		{
			glbDouble ele = getTerrianElv(newPos.x(),newPos.y());

			if(newPosXYZ.z() < ele + ELEVATE_THRESHOLD_FPS + mpr_personHigh)
			{
				newPosXYZ.z() = ele + ELEVATE_THRESHOLD_FPS + mpr_personHigh;
			}
		}

		setCameraXYZ(newPosXYZ);
	}

	else
	{
		if(needFootCollision)
		{
			//与三维模型交点
			const osg::Vec3d up		= getWorldUp();
			osg::Vec3d otherPoint = newPosXYZ - up * (mpr_personHigh ) * 5.0;

			//与脚下的求交
			glbDouble distanceToMove;
			glbBool haveCollusion = intersect(newPosXYZ,otherPoint,distanceToMove);

			if(haveCollusion)
			{

				if((newPosXYZ.z() - otherPoint.z()) < (ELEVATE_THRESHOLD_FPS + mpr_personHigh))
				{
					newPosXYZ.z() = otherPoint.z() + (ELEVATE_THRESHOLD_FPS + mpr_personHigh);
				}

				setCameraXYZ(newPosXYZ);
				return;
			}
		}

		//与地下参考面交点
		{
			glbDouble ele = mpr_globe->GetUnderGroundAltitude();

			if(newPosXYZ.z() < ele + GROUND_ALTITUDE_THRESHOLD_FPS + mpr_personHigh)
			{
				newPosXYZ.z() = ele + GROUND_ALTITUDE_THRESHOLD_FPS + mpr_personHigh;
			}
		}

		{
			//与镜头上部地形缓冲高度
			GLdouble ele = getTerrianElv(newPos.x(),newPos.y());

			glbDouble deltaZ = ele - newPosXYZ.z();

			if(std::abs(deltaZ) < CAMERA_UNDERGROUND_TOP_HOLDPFS)
			{
				GLdouble camerH;
				//此时相机处于缓冲区内,直接相机进入地下或者进入地上
				if(mpr_camera_is_underGround) // dixia
				{
					camerH =   mpr_personHigh + ele + std::abs(deltaZ);
				}
				newPosXYZ.z() = camerH;
				mpr_camera_is_underGround = false;
			}
		}

		setCameraXYZ(newPosXYZ);
	}
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::moveCameraPos(const osg::Vec3d dir,glbDouble len,glbBool collision /*= true*/,glbBool ref /*= true*/)
{
	if(dir.length2() <= 0) return false;

	osg::Vec3d dirNormal = dir;
	dirNormal.normalize();

	if(collision && mpr_globe->CheckCullisionModel())
	{
		if(collusionDit(dirNormal,len))
		{
			return false;
		}
	}

	const osg::Vec3d newCameraXYZ = getCameraXYZ() + dirNormal * len;

	if(ref)
	{
		rectifyCameraPos(newCameraXYZ);
	}
	else
	{
		setCameraXYZ(newCameraXYZ);
	}

	return true;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::collusionDit(const osg::Vec3d moveDir, glbDouble& distance)
{
	glbBool cullision = false;

	if(0 ==mpr_CollisionObjs.size() && 0 == mpr_CullisionLays.size()) return cullision;

	osg::Vec3d up = getWorldUp();

	osg::Vec3d viewDir = getCameraVector();
	//osg::Vec3d localUp = localToWorldVec(osg::Vec3d(0,1,0));
	osg::Vec3d rightDir = viewDir ^ up;

#if 0
	osg::Vec3d currentCameraXYZ = getCameraXYZ();
	osg::Vec3d newCameraXYZ = currentCameraXYZ + moveDir * distance;

	glbDouble dis;
	return intersect(currentCameraXYZ,newCameraXYZ,dis);

#else
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

	osgUtil::IntersectionVisitor intersectVisitor( intersectorGroup.get());

	MapIter bg = mpr_CollisionObjs.cbegin();
	MapIter eg = mpr_CollisionObjs.cend();
	while (bg != eg)
	{

		osg::Node * node = bg->second->GetOsgNode();
		if(node)
		{
			node->accept(intersectVisitor);
			if (intersectorGroup->containsIntersections())
			{
				cullision = true;
				break;
				//intersections.push_back(lsi->getIntersections().begin()->getWorldIntersectPoint());
			}
		}
		bg++;
	}

	if(!cullision)
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
				osg::Node* node = (*setB)->GetOsgNode();
				if(node)
				{
					node->accept(intersectVisitor);
					if (intersectorGroup->containsIntersections())
					{
						cullision = true;
						break;
					}
				}
				setB++;
			}
			bbg++;
		}
	}

#endif
	return cullision;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::handleMouseDoubleClick(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if(IsFlying())
	{
		StopFlying();
	}

	addMouseEvent(ea);	

	if(ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		osg::Vec3d pushPoint;
		GLdouble distance;
		if(!getPushPoint(pushPoint,distance))
		{
			return true;
		}

		//计算目标pitch角
		osg::Vec3d cameraPos = getMatrix().getTrans();
		osg::Vec3d lookvec = pushPoint - cameraPos;
		glbDouble pitch;
		if (lookvec.z()/lookvec.length() > 1.0)
			pitch = 90.0;
		else if (lookvec.z()/lookvec.length() < -1.0)
			pitch = -90.0;
		else
			pitch= osg::RadiansToDegrees(asin(lookvec.z()/lookvec.length())); 
		GLdouble yaw = -GetCameraYaw();
		FlyTo2(pushPoint.x(),pushPoint.y(),pushPoint.z(),0.33*distance,yaw,pitch,1.0);
	}
	else if(ea.getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		osg::Vec3d pushPoint;
		GLdouble distance;
		if(!getPushPoint(pushPoint,distance))
		{
			return true;
		}

		FlyAround(pushPoint.x(),pushPoint.y(),pushPoint.z(),10.0,true);
	}

	us.requestContinuousUpdate( false );
	return true;
}

void GlbGlobe::CGlbPlaneFPSManipulator::addMouseEvent(const osgGA::GUIEventAdapter& ea)
{
	_ga_t1 = _ga_t0;
	_ga_t0 = &ea;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::performMovementLeftMouseButton(const glbDouble eventTimeDelta, const glbDouble dx, const glbDouble dy)
{
	rotatePitchYaw(dx,-dy,true);
	return false;
}

void GlbGlobe::CGlbPlaneFPSManipulator::flushMouseEventStack()
{
	_ga_t1 = NULL;
	_ga_t0 = NULL;
}

void GlbGlobe::CGlbPlaneFPSManipulator::ascendCamera(GLdouble high)
{
	osg::Vec3d cameraXYZ = getCameraXYZ();
	cameraXYZ.z() += high;
	rectifyCameraPos(cameraXYZ);
}

void GlbGlobe::CGlbPlaneFPSManipulator::SetDriveMoveSpeed(const glbDouble&speed)
{
	mpr_moveSpeed = speed;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::intersect(const osg::Vec3d start, osg::Vec3d end,glbDouble&distance) const
{
	osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(start,end);

	osgUtil::IntersectionVisitor iv(lsi.get());

	std::vector<osg::Vec3d> intersections;

	MapIter bg = mpr_CollisionObjs.cbegin();
	MapIter eg = mpr_CollisionObjs.cend();
	while (bg != eg)
	{
		osg::Node* node = bg->second->GetOsgNode();
		if(node)
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

osg::Vec3d GlbGlobe::CGlbPlaneFPSManipulator::getWorldUp() const
{
	return osg::Vec3d(0,0,1);
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::limitCameraByScene(osg::Vec3d& cameraPos)
{

	if(pointInScene(cameraPos,mpr_SceneMin,mpr_SceneMax))
	{
		return true;
	}
	if (cameraPos.x()>=mpr_SceneMax.x())
		cameraPos.x() = mpr_SceneMax.x();
	if (cameraPos.x()<=mpr_SceneMin.x())
		cameraPos.x() = mpr_SceneMin.x();
	if (cameraPos.y()>=mpr_SceneMax.y())
		cameraPos.y() = mpr_SceneMax.y();
	if (cameraPos.y()<=mpr_SceneMin.y())
		cameraPos.y() = mpr_SceneMin.y();

	return false;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::getPushPoint(osg::Vec3d&pushPoint,GLdouble&dis) const
{
	glbDouble dx = _ga_t0->getX();
	glbDouble dy = _ga_t0->getY();

	glbDouble ptx = dx;
	glbDouble pty = dy;
	mpr_globe->GetView()->ScreenToWindow(ptx,pty);

	//与场景
	osg::Vec3d interPoint;
	glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx,pty,interPoint);
	//与地形
	glbDouble lon = 0.0;
	glbDouble lat = 0.0;
	glbDouble alt = 0.0;
	glbBool isTerrainclick = mpr_globe->ScreenToTerrainCoordinate(dx,dy,lon,lat,alt);
	if (!isTerrainclick)
	{  
		if (mpr_globe->IsUnderGroundMode())	//开启地下模式
		{
			isTerrainclick = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,lon,lat,alt);
		}
	}

	if(!isObjfocus && !isTerrainclick)
	{
		return false;
	}
	else if(isObjfocus && isTerrainclick)
	{
		// 比较对象上的pick点和地面/地下参考面的pick点哪个离相机近，就选用哪个点做pushPos			
		osg::Vec3d pushPos(lon,lat,alt);
		osg::Vec3d cameraPos_w = getMatrix().getTrans();
		glbDouble dis1 = (cameraPos_w - interPoint).length();
		glbDouble dis2 = (cameraPos_w - pushPos).length();
		pushPoint = (dis1 < dis2) ? interPoint : pushPos;
		dis	   = min(dis1,dis2);
	}
	else if (isObjfocus)
	{
		pushPoint = interPoint;
		osg::Vec3d cameraPos_w = getMatrix().getTrans();
		dis = (cameraPos_w - interPoint).length();
	}

	else if(isTerrainclick)
	{
		pushPoint = osg::Vec3d(lon,lat,alt);
		osg::Vec3d cameraPos_w = getMatrix().getTrans();
		dis = (cameraPos_w - pushPoint).length();
	}

	return true;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::FlyTo2(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
	GLdouble mpr_distance = GetDistance();
	osg::Vec3d mpr_focusPos;
	GetFocusPos(mpr_focusPos);

	GLdouble mpr_yaw = -GetCameraYaw();
	GLdouble mpr_pitch = GetCameraPitch() - 90.0;
	//开启路径动画
	if (mpr_distance > 3*distance)	//拉近焦点
	{
		osg::Vec3d cameraPos;
		GetCameraPos(cameraPos);
		osg::Vec3d lookvec = mpr_focusPos - cameraPos;
		lookvec.normalize();
		mpr_focusPos = cameraPos + lookvec*3*distance;
		mpr_distance = 3*distance;
	}

	KeyPoint pt0;
	pt0.focusPos = mpr_focusPos;
	pt0.distan = mpr_distance;
	pt0.yaw = mpr_yaw;
	pt0.pitch = mpr_pitch;

	KeyPoint pt1;
	pt1.focusPos = osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	pt1.distan = distance;
	pt1.yaw = yaw;
	pt1.pitch = pitch;

	osg::Vec3d cameraPos0;
	GetCameraPos(cameraPos0);
	glbDouble distan = (pt1.focusPos - cameraPos0).length();
	osg::Vec3d lookat = mpr_focusPos - cameraPos0;
	lookat.normalize();
	if (distan < mpr_distance)
	{
		pt0.distan = distan;
		pt0.focusPos = cameraPos0 + lookat*distan; 
	}
	osg::Matrixd mat = osg::Matrixd::translate(0.0,0.0,distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
		osg::Matrixd::translate(pt1.focusPos);
	//对目标点进行相机纠偏
	osg::Vec3d cameraPos = mat.getTrans();

	if(!limitCameraByScene(cameraPos))
	{
		osg::Vec3d lookvec = pt1.focusPos - cameraPos;
		pt1.distan = lookvec.length();
		//如果pitch角为正负90时，mpr_yaw则不变
		//if (lookvec.x() != 0.0 || lookvec.y() != 0.0)
		//mpr_yaw = osg::RadiansToDegrees(atan2(lookvec.x(),lookvec.y()));
		if (lookvec.z()/pt1.distan > 1.0)
			pt1.pitch = 90.0;
		else if (lookvec.z()/pt1.distan < -1.0)
			pt1.pitch = -90.0;
		else
			pt1.pitch = osg::RadiansToDegrees(asin(lookvec.z()/pt1.distan));
	}

	planeFlyToFromCGlbGlobeManipulatorinit(FLYTO2,pt0,pt1,seconds);

	CameraLLHPPY s,t; //s t 暂时不使用
	animationCamer(s,t,3.0,2,false,false);

	return true;
}

void GlbGlobe::CGlbPlaneFPSManipulator::GetHaveAddObjsAndLays(std::vector<glbInt32>& objIds,std::vector<glbInt32>& layIds)
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

glbBool GlbGlobe::CGlbPlaneFPSManipulator::AddCollisionLaysObject(glbInt32 layId,CGlbGlobeRObject* obj)
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

glbBool GlbGlobe::CGlbPlaneFPSManipulator::processKeyUpEvent(const osgGA::GUIEventAdapter&ea,osgGA::GUIActionAdapter&us)
{
	switch(ea.getKey())
	{
	case (osgGA::GUIEventAdapter::KEY_W):
		{
			mpr_wasd[W] = false;
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_S):
		{
			mpr_wasd[S] = false;
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_A):
		{
			mpr_wasd[A] = false;
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_D):
		{
			mpr_wasd[D] = false;
			return true;
		}
	}
	return false;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::intersectRayPlane(osg::Vec3d cameraXYZ,osg::Vec3d viewDir,osg::Vec3d&interPoint)
{
	//p 在平面上

	 glbBool haveI = rayTriangle(cameraXYZ,viewDir, osg::Vec3d(mpr_SceneMin.x(),mpr_SceneMin.y(),1),
													osg::Vec3d(mpr_SceneMax.x(),mpr_SceneMin.y(),1),
													osg::Vec3d(mpr_SceneMin.x(),mpr_SceneMax.y(),1),
													interPoint);
	 if(!haveI)
	 {
		 haveI = rayTriangle(cameraXYZ,viewDir, osg::Vec3d(mpr_SceneMax.x(),mpr_SceneMin.y(),1),
			 osg::Vec3d(mpr_SceneMax.x(),mpr_SceneMax.y(),1),
			 osg::Vec3d(mpr_SceneMin.x(),mpr_SceneMax.y(),1),
			 interPoint);
	 }

	return false;
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::performMovementLeftAndMiddlePush()
{
	glbDouble dis;
	getPushPoint(mpr_mousePushPoint,dis);
	return true;
}

void GlbGlobe::CGlbPlaneFPSManipulator::rotateAboutPoint(const osg::Vec3d aroundPoint,glbDouble pitch,glbDouble yaw)
{

	osg::Matrixd mt(getMatrix());

	osg::Quat quat(osg::DegreesToRadians(GetCameraYaw()),osg::Z_AXIS);
	osg::Vec3d rotate_pitch = quat *  osg::Vec3d(1.0,0.0,0.0); //经过旋转后X轴的向量

	osg::Quat rotate = osg::Quat(osg::DegreesToRadians(pitch),rotate_pitch) * 
				osg::Quat(osg::DegreesToRadians(yaw),osg::Z_AXIS);

	osg::Matrixd newMat = mt * osg::Matrixd::translate(-aroundPoint)*
							osg::Matrixd(rotate)*
							osg::Matrixd::translate(aroundPoint);

	//相机纠偏
	osg::Vec3d newCameraPos = newMat.getTrans();

	limitCameraByScene(newCameraPos);
	setCameraXYZ(newCameraPos);

	//局部坐标系的旋转
	quat = newMat.getRotate();

	fixVerticalAxis(quat,osg::Vec3d(0,0,1),true);

	setCameraPitchYaw(quatToEuler(quat));
}

void GlbGlobe::CGlbPlaneFPSManipulator::home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	setByMatrix(mpr_homeMatrix);
}

void CGlbPlaneFPSManipulator::fixVerticalAxis(osg::Quat& rotation, const osg::Vec3d& localUp, glbBool disallowFlipOver)
{
	osg::Vec3d cameraUp         = rotation * osg::Vec3d( 0.,1.,0. );
	osg::Vec3d cameraRight      = rotation * osg::Vec3d( 1.,0.,0. );
	osg::Vec3d cameraForward    = rotation * osg::Vec3d( 0.,0.,-1. );

	osg::Vec3d newCameraRight1  = cameraForward ^ localUp;
	osg::Vec3d newCameraRight2  = cameraUp ^ localUp;
	osg::Vec3d newCameraRight   = (newCameraRight1.length2() > newCameraRight2.length2()) ?
newCameraRight1 : newCameraRight2;

	if( newCameraRight * cameraRight < 0. )
		newCameraRight = -newCameraRight;

	osg::Quat rotationVerticalAxisCorrection;
	rotationVerticalAxisCorrection.makeRotate( cameraRight, newCameraRight );

	rotation *= rotationVerticalAxisCorrection;

	//防止pitch超过90 和 -90度
	if( disallowFlipOver )
	{
		osg::Vec3d newCameraUp = newCameraRight ^ cameraForward;
		if( newCameraUp * localUp < 0. )
			rotation = osg::Quat( osg::PI, osg::Vec3d( 0.,0.,1. ) ) * rotation;

	}
}

void CGlbPlaneFPSManipulator::rotatePitchYaw(glbDouble yaw,glbDouble pitch,glbBool disallowFlipOver)
{
	osg::Vec3d curPY = getCameraPitChYaw();
	curPY[2] += osg::DegreesToRadians(mpr_rotateStep * yaw);
	curPY[0] += osg::DegreesToRadians(mpr_rotateStep * pitch);

	if(curPY[0] <= 0)
	{
		curPY[0] = 0;
	}
	if(curPY[0] >= osg::PI)
	{
		curPY[0] = osg::PI;
	}

	if(curPY[2] > osg::PI)
	{
		curPY[2] = curPY[2] - 2 * osg::PI;
	}
	if(curPY[2] < -osg::PI)
	{
		curPY[2] = curPY[2] + 2 * osg::PI;
	}

	setCameraPitchYaw(curPY);
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::CFPSPlaneManipulatorCallback::checkAnimationStart() const
{
	return haveStart;
}

GlbGlobe::CGlbPlaneFPSManipulator::CFPSPlaneManipulatorCallback::CFPSPlaneManipulatorCallback(const CameraLLHPPY&b,
	const CameraLLHPPY&a,CGlbGlobe*glb,glbBool col /*= true*/):
currentCamera(b),
	targetCamera(a),
	collision(col),
	globe(glb)
{
	init();
	afterCamera = currentCamera;
}


void GlbGlobe::CGlbPlaneFPSManipulator::CFPSPlaneManipulatorCallback::init()
{
	willStop		= false;
	haveStart		= false;
	refLocation		= true;
	moveType		= XYZ,
	firstLoop		= true;
	looptime		= 0.0;
	frameStamp		= 0;
	pfunCallBack	= NULL;
}

void GlbGlobe::CGlbPlaneFPSManipulator::CFPSPlaneManipulatorCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	if (nv->getVisitorType()==osg::NodeVisitor::UPDATE_VISITOR && !willStop)
	{
		updateCamera(node,nv);
	}
	traverse(node,nv);
}

void GlbGlobe::CGlbPlaneFPSManipulator::CFPSPlaneManipulatorCallback::updateCamera(osg::Node*node,osg::NodeVisitor*nv)
{

	frameStamp ++;

	if(firstLoop)
	{
		if(beforeLoop(node))
		{
			firstLoop = false;
		}
	}

	//使用globe 的flyto不用此判断终结回调
	if(frameStamp > looptime && GLOBEMANIPULOAOR !=  moveType)
	{
		stopAnimation(false,false); 
		
		if(pfunCallBack != NULL)
		{
			pfunCallBack();
		}

		return;
	}
	//使用globe 的flyto判断终结回调
	if(manipulator->Globe_pause && GLOBEMANIPULOAOR == moveType)
	{
		stopAnimation(false,false);
		if(pfunCallBack != NULL)
		{
			pfunCallBack();
		}
		return;
	}

	if(!willStop)
	{
		currentCamera = afterCamera;
		haveStart = true;

		if(XYZ == moveType)
		{
			//move after
			osg::Vec3d dir = (targetCamera.getCameraPos() - currentCamera.getCameraPos());
			dir.normalize();
			afterCamera.setCameraPos(currentCamera.getCameraPos() + dir * stepLen);

			glbDouble perc = frameStamp / looptime;

			GLdouble K = 3;
			GLdouble t = 2 * K * perc - K;
			GLdouble s = ((erf (sqrt(osg::PI) / 2 * t)) + 1) / 2;

			GLdouble pitch      = originCameraPY.x() + pitchYawDelta.x() * s;
			GLdouble yaw		= originCameraPY.y() + pitchYawDelta.y() * s;

			afterCamera.setPY(osg::Vec2d(pitch,yaw));
		}
		
		else if(GLOBEMANIPULOAOR == moveType)
		{
			//const osg::Matrixd m = Globe_matrix;
			manipulator->planeFlyToFromCGlbGlobeManipulator(nv);
		}
		
	}
}

glbBool GlbGlobe::CGlbPlaneFPSManipulator::CFPSPlaneManipulatorCallback::beforeLoop(osg::Node * n)
{
	nodeForCallback = n;

	if(XYZ == moveType)
	{
		stepLen = (targetCamera.getCameraPos() - currentCamera.getCameraPos()).length();
		if(looptime == 0.0) looptime = 1.0;
		stepLen /= looptime;
	}
	else if(GLOBEMANIPULOAOR == moveType)
	{
		return true;
	}
	//pitch yaw
	pitchYawDelta.x() = (targetCamera.getP() - currentCamera.getP()) ;
	pitchYawDelta.y() = (targetCamera.getY() - currentCamera.getY()) ;

	originCameraPY.x() = currentCamera.getP();
	originCameraPY.y() = currentCamera.getY();

	return true;
}

void GlbGlobe::CGlbPlaneFPSManipulator::CFPSPlaneManipulatorCallback::stopAnimation(glbBool removeUpdate /*= false*/,glbBool resetFunCallback /*= true*/)
{
	if(nodeForCallback)
	{
		willStop  = true;
		haveStart = false;
		if(resetFunCallback)  pfunCallBack = NULL;

		if(removeUpdate)
		{
			glbref_ptr<CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask> task = new
				CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask(nodeForCallback,this);
			globe->mpr_p_callback->AddFadeTask(task.get());
		}
	}
}

const CGlbPlaneFPSManipulator::CameraLLHPPY& GlbGlobe::CGlbPlaneFPSManipulator::CFPSPlaneManipulatorCallback::getCurrerntCamera() const
{
	return currentCamera;
}

const CGlbPlaneFPSManipulator::CameraLLHPPY& CGlbPlaneFPSManipulator::CFPSPlaneManipulatorCallback::getAfterCameraF() const
{
	return afterCamera;
}
