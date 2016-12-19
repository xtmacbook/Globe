#include "StdAfx.h"
#include "GlbFPSManipulator.h"
#include "GlbGlobeView.h"
using namespace GlbGlobe;


/************************************************************************/
/*                                                                      */
/************************************************************************/
extern const float 	ELEVATE_THRESHOLD_FPS;	//相机在地上时，位于地形之上的阈值，单位m
extern const osg::Vec3d invalidFlyFocus;
enum GLB_FPS_CAMERAKEY
{
	A = 0,
	D,
	W,
	S
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//event
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

glbBool CGlbFPSManipulator::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
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
		return handleMouseRelease( ea, aa );
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
	case (osgGA::GUIEventAdapter::KEYUP):
		{
			//首先将flyFocus变为无效的点
			setFlyFocus(invalidFlyFocus);
			return processKeyEvent(ea,aa);
		}
	default:
		break;
	}

	return false;
}


void CGlbFPSManipulator::flushMouseEventStack()
{
	_ga_t1 = NULL;
	_ga_t0 = NULL;
}

void CGlbFPSManipulator::addMouseEvent( const osgGA::GUIEventAdapter& ea )
{
	_ga_t1 = _ga_t0;
	_ga_t0 = &ea;
}

glbBool CGlbFPSManipulator::processKeyEvent(const osgGA::GUIEventAdapter& ea,
														osgGA::GUIActionAdapter& aa)
{
	if(IsFlying())
	{
		StopFlying();
	}

	switch(ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::KEYUP):
		return processKeyUp(ea,aa);
	case (osgGA::GUIEventAdapter::KEYDOWN):
		return processKeyDown(ea,aa);
	default:
		break;
	}
	return false;
}

glbBool CGlbFPSManipulator::processKeyUp(const osgGA::GUIEventAdapter& ea,
													osgGA::GUIActionAdapter& aa)
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

glbBool CGlbFPSManipulator::processKeyDown(const osgGA::GUIEventAdapter& ea,
													osgGA::GUIActionAdapter& aa)
{
	switch(ea.getKey())
	{
	case (osgGA::GUIEventAdapter::KEY_W):
		{
			mpr_wasd[W] = true;
			//moveCameraByKey();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_S):
		{
			mpr_wasd[S] = true;
			//moveCameraByKey();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_A):
		{
			mpr_wasd[A] = true;
			//moveCameraByKey();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_D):
		{
			mpr_wasd[D] = true;
			//moveCameraByKey();
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

 
void CGlbFPSManipulator:: home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	setByMatrix(mpr_homeMatrix);
}

glbBool CGlbFPSManipulator::handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	if(IsFlying())
	{
		StopFlying();
	}

	flushMouseEventStack();
	addMouseEvent( ea );

	performMovementLeftAndMiddlePush();
	
	us.requestRedraw();
	us.requestContinuousUpdate( false );

	return true;
}

glbBool CGlbFPSManipulator::handleMouseDrag( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	addMouseEvent(ea);

	if( performMovement(ea) )
		us.requestRedraw();

	us.requestContinuousUpdate( false );
	return true;
}

glbBool CGlbFPSManipulator::handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	flushMouseEventStack();
	return true;
}

glbBool CGlbFPSManipulator::handleFrame(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{

	if(mpr_mouseCallBack.valid() && mpr_mouseCallBack->checkAnimationStart())
	{
		CFPSManipulatorCallback::MoveType mt = mpr_mouseCallBack->getMoveType();

		if(CFPSManipulatorCallback::GLOBEMANIPULOAOR == mt)
		{
			setFlyFocus(Globe_focus_temp);
			setByMatrix(Globe_matrix);

			//
			osg::Vec3d tmp = Globe_matrix.getTrans();
			convertXYZToLatLongHeight(tmp.x(),tmp.y(),tmp.z(),tmp);
			rectifyCameraPos(tmp);

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

	//时时检测镜头是否到达地下
	osg::Vec3d cameraLLH = getCameraLLHDegree();
	GLdouble elv = getTerrianElv(cameraLLH.x(),cameraLLH.y());

	GLdouble delta =  cameraLLH.z() - elv;

	glbBool camerUnderGround = (delta <  0.0001) ? true:false;

	mpr_globe->NotifyCameraIsUnderground(camerUnderGround);

	mpr_camera_is_underGround = camerUnderGround;


	//时时检测开启和关闭地下模式
	GLboolean enableUnderGround = mpr_globe->IsUnderGroundMode();
	if(!enableUnderGround)
	{
		//关闭,如果相机在地下，则上移
		if(mpr_camera_is_underGround)
		{

			GLdouble underGroudDepth = -delta;
			GLdouble camerH = elv + mpr_personHigh + underGroudDepth + ELEVATE_THRESHOLD_FPS;

			osg::Vec3d newCameraLLH = cameraLLH;
			newCameraLLH.z() = camerH;

			mpr_camera_is_underGround = false;

			rectifyCameraPos(newCameraLLH,true);
		}

	}

	us.requestRedraw();

	return false;
}

glbBool CGlbFPSManipulator::handleMouseDoubleClick(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
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
		osg::Vec3d targetPos = pushPoint;
		osg::Vec3d lookvec = targetPos - cameraPos;
		lookvec.normalize();
		osg::Vec3d llt;
		convertXYZToLatLongHeight(targetPos.x(),targetPos.y(),targetPos.z(),llt);
		//targetPos处向上向量
		osg::Vec3d up(cos(osg::DegreesToRadians(llt.y()))*cos(osg::DegreesToRadians(llt.x())),
			sin(osg::DegreesToRadians(llt.y()))*cos(osg::DegreesToRadians(llt.x())), 
			sin(osg::DegreesToRadians(llt.x())));

		up.normalize();

		glbDouble tempnomal = lookvec*up;
		tempnomal = max(min(tempnomal,1.0),-1.0);
		glbDouble pitch = -osg::RadiansToDegrees(acos(tempnomal)) + 90; 

		flyTo2(llt.y(),llt.x(),llt.z(),0.33*distance,-GetCameraYaw(),pitch,1.0);
#if 0
		CameraLLHPPY currentCameraPPY;
		currentCameraPPY.setCameraPos(getCameraXYZ());
		currentCameraPPY.setCameraPY(getCameraPitChYaw());

		CameraLLHPPY newCamera = computeTargetPointCameraPPY(pushPoint);
		newCamera.setPY(currentCameraPPY.getPY());

		animationCamer(currentCameraPPY,newCamera,mpr_animationTimes,0);
#endif

	}
	else if(ea.getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		osg::Vec3d pushPoint;
		GLdouble distance;
		if(!getPushPoint(pushPoint,distance))
		{
			return true;
		}
		osg::Vec3d llh;
		convertXYZToLatLongHeight(pushPoint.x(),pushPoint.y(),pushPoint.z(),llh);
		FlyAround(llh.y(),llh.x(),llh.z(),10.0,true);
	}

	us.requestContinuousUpdate( false );
	return true;
}

glbBool CGlbFPSManipulator::handleScroll(const osgGA::GUIEventAdapter& ea,
	osgGA::GUIActionAdapter& aa)
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

	aa.requestContinuousUpdate( false );
#else
	//先求zoomdelt
	glbDouble zoomdelt = DBL_MAX;
	osg::Vec3d cameraXYZ = getCameraXYZ();
	osg::Vec3d focuse;
	GetFocusPos(focuse);
	convertLatLongHeightToXYZ(focuse.y(),focuse.x(),focuse.z(),focuse);
	zoomdelt = (cameraXYZ-focuse).length();

	glbDouble ZoomSpeed = 0.1;
	ZoomSpeed *= abs(zoomDelta);

	if (mpr_globe->IsUnderGroundMode())
	{	
		glbDouble zoom_break = ELEVATE_THRESHOLD_FPS;
		GLdouble mpr_pitch = GetCameraPitch() - 90.0;
		if (abs(mpr_pitch) > 1.0)
			zoom_break /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
		else if (abs(mpr_pitch) > 0.1)
			zoom_break *= 60;
		else 
			zoom_break *= 600;
		if (abs(zoomdelt) > zoom_break)
		{
			if (zoomDelta > 0)	//前滚
			{
				zoomdelt = -abs(zoomdelt) * ZoomSpeed;
			}
			else
			{
				zoomdelt = abs(zoomdelt) * ZoomSpeed;
			}
		}
		else{	//突变
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
			zoomdelt = -abs(zoomdelt) * ZoomSpeed;
		}
		else
		{
			zoomdelt = abs(zoomdelt) * ZoomSpeed;
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

	//相机沿lookvec方向移动zoomdelt距离
	setByMatrix(osg::Matrixd::translate(0.0,0.0,zoomdelt) * getMatrix());


	rectifyCameraPos(getCameraLLHDegree());
#endif

	aa.requestRedraw();
	return true;
}

glbBool CGlbFPSManipulator::performMovement(const osgGA::GUIEventAdapter& ea)
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
	   if(osgGA::GUIEventAdapter::DRAG ==  ea.getEventType())
		{
			return performMovementLeftMouseButton( eventTimeDelta, dx, dy );
		}
	}
	else if(buttonMask == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{
		if(osgGA::GUIEventAdapter::DRAG ==  ea.getEventType())
		{
			glbDouble dx = _ga_t0->getX() - _ga_t1->getX();
			glbDouble dy = _ga_t0->getY() - _ga_t1->getY();

			glbDouble yaw   = -dx*0.3;	//系数暂定0.3
			glbDouble pitch = dy*0.3;

			rotateAboutPoint(mpr_mousePushPoint,pitch,yaw);

			return true;
		}
	}
	return false;
}

glbBool CGlbFPSManipulator::performMovementLeftAndMiddlePush()
{
	glbDouble dis;
	getPushPoint(mpr_mousePushPoint,dis);
	return true;
}
glbBool CGlbFPSManipulator::performMovementLeftMouseButton( const glbDouble eventTimeDelta, const glbDouble dx, const glbDouble dy )
{
	rotatePitchYaw(dx,-dy,true);

	return false;
}

void GlbGlobe::CGlbFPSManipulator::moveCameraByKey()
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
