#include "StdAfx.h"
#include "GlbGlobeDriveManipulator.h"
#include "GlbGlobeView.h"

using namespace GlbGlobe;

CGlbGlobeDriveManipulator::CGlbGlobeDriveManipulator(CGlbGlobe* globe)
{
	mpr_globe = globe;
	mpr_cameraPos = osg::Vec3d(116.3,39.9,1.0);
	mpr_pitch = 0.0;
	mpr_yaw = 0.0;
	mpr_roll = 0.0;
	mpr_velocity = 0.0;
	mpr_acceleration = 3.0;	//单位m/s2
	mpr_maxSpeed = 100.0;	//m/s
}


CGlbGlobeDriveManipulator::~CGlbGlobeDriveManipulator(void)
{
}

osg::Matrixd CGlbGlobeDriveManipulator::getMatrix() const
{ 
	osg::Matrixd cameraToWorld;
	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(mpr_cameraPos.y()),osg::DegreesToRadians(mpr_cameraPos.x()),mpr_cameraPos.z(),cameraToWorld);
	osg::Vec3d yaw_vec(0.0,0.0,1.0);
	osg::Vec3d pitch_vec(cos(osg::DegreesToRadians(mpr_yaw)),sin(osg::DegreesToRadians(mpr_yaw)),0.0);
	osg::Vec3d roll_vec(-sin(osg::DegreesToRadians(mpr_yaw))*cos(osg::DegreesToRadians(mpr_pitch)),cos(osg::DegreesToRadians(mpr_pitch))*cos(osg::DegreesToRadians(mpr_yaw)),sin(osg::DegreesToRadians(mpr_pitch))); 
	//Z_X_Y
	return osg::Matrixd::rotate(osg::DegreesToRadians(mpr_yaw),yaw_vec)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(mpr_pitch),pitch_vec)*		
		osg::Matrixd::rotate(osg::DegreesToRadians(mpr_roll),roll_vec)*
		cameraToWorld;
}

osg::Matrixd CGlbGlobeDriveManipulator::getInverseMatrix() const
{
	return osg::Matrixd::inverse(getMatrix());
}

void CGlbGlobeDriveManipulator::setByMatrix(const osg::Matrixd& matrix)
{
	mpr_cameraPos = matrix.getTrans();
	g_ellipsmodel->convertXYZToLatLongHeight(mpr_cameraPos.x(),mpr_cameraPos.y(),mpr_cameraPos.z(),mpr_cameraPos.y(),mpr_cameraPos.x(),mpr_cameraPos.z());
	mpr_cameraPos.x() = osg::RadiansToDegrees(mpr_cameraPos.x());
	mpr_cameraPos.y() = osg::RadiansToDegrees(mpr_cameraPos.y());
	osg::Matrixd cameraToWorld;
	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(mpr_cameraPos.y()),osg::DegreesToRadians(mpr_cameraPos.x()),mpr_cameraPos.z(),cameraToWorld);
	osg::Matrixd mat = matrix * osg::Matrixd::inverse(cameraToWorld);
	if (mat(2,2) > 1.0)
		mpr_pitch = -90.0;
	else if (mat(2,2) < -1.0)
		mpr_pitch = 90.0;
	else
		mpr_pitch = osg::RadiansToDegrees(-asin(mat(2,2)));
	mpr_yaw = osg::RadiansToDegrees(atan2(mat(2,0),-mat(2,1)));
	mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(1,2)));
}

void CGlbGlobeDriveManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
{  
	setByMatrix(osg::Matrixd::inverse(matrix));
}

void CGlbGlobeDriveManipulator::home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	mpr_cameraPos = osg::Vec3d(116.3,39.9,1.0);
	mpr_pitch = 0.0;
	mpr_yaw = 0.0;
	mpr_roll = 0.0;
	mpr_velocity = 0.0;
}

glbBool CGlbGlobeDriveManipulator::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::FRAME):
		{

			return false;
		}
		break;
	case(osgGA::GUIEventAdapter::RESIZE):
		{
			init(ea,aa);
			aa.requestRedraw();
			return true;
		}
	default:
		break;
	}

	if (ea.getHandled()) 
		return false;

	switch (ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::KEYDOWN):	//按键
		{
			if (handleKeyDown(ea,aa))
			{
				aa.requestRedraw();
				return true;
			}
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::DRAG):	//鼠标拖拽
		{
		}
		break;
	case (osgGA::GUIEventAdapter::SCROLL):	//滚轮滚动
		{
		}
		break;
	}
	return false;
}

glbBool CGlbGlobeDriveManipulator::handleKeyDown(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	glbDouble tim = 1.0;
	switch(ea.getKey())
	{
	case (osgGA::GUIEventAdapter::KEY_W):		//加速	
		{
			mpr_velocity += mpr_acceleration * tim;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_S):		//减速	
		{
			mpr_velocity -= mpr_acceleration * tim;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_A):		//左转	
		{
			mpr_yaw += 1.0;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_D):		//右转	
		{
			mpr_yaw -= 1.0;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Space):	//跳跃
		{

		}
		break;
	}
	return false;
}

void CGlbGlobeDriveManipulator::init(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{

}

GlbGlobeManipulatorTypeEnum CGlbGlobeDriveManipulator::GetType()
{
	return GLB_MANIPULATOR_DRIVE;
}				 

glbBool CGlbGlobeDriveManipulator::FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
	return false;
}

glbBool CGlbGlobeDriveManipulator::FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, bool repeat)
{
	return false;
}

glbBool CGlbGlobeDriveManipulator::IsFlying()
{
	return false;
}

glbBool CGlbGlobeDriveManipulator::StopFlying()
{
	return false;
}

glbBool CGlbGlobeDriveManipulator::JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch)
{
	return false;
}

glbBool CGlbGlobeDriveManipulator::SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance)
{
	return false;
}

glbDouble CGlbGlobeDriveManipulator::GetYaw()
{
	return mpr_yaw;
}

glbBool CGlbGlobeDriveManipulator::SetPitch(glbDouble pitch,osg::Vec3d old_focus,glbDouble olddistance)
{
	return false;
}

glbDouble CGlbGlobeDriveManipulator::GetPitch()
{
	return mpr_pitch;
}

glbBool CGlbGlobeDriveManipulator::GetCameraPos(osg::Vec3d& cameraPos)
{
	cameraPos = mpr_cameraPos;
	return true;
}

glbBool CGlbGlobeDriveManipulator::GetFocusPos(osg::Vec3d& focusPos)
{
	return false;
}

glbDouble CGlbGlobeDriveManipulator::GetDistance()
{
	return 0;
}

glbBool CGlbGlobeDriveManipulator::Move(glbDouble dangleX,glbDouble dangleY)
{
	return false;
}

glbBool CGlbGlobeDriveManipulator::TranslateCamera(glbDouble x,glbDouble y,glbDouble z)
{
	return false;
}

glbBool CGlbGlobeDriveManipulator::DetectCollision(glbBool mode)
{
	return mode;
}

glbBool CGlbGlobeDriveManipulator::SetUnderGroundMode(glbBool mode)
{
	return mode;
}

glbBool CGlbGlobeDriveManipulator::SetUndergroundDepth(glbDouble zOrAlt)
{
	return false;
}

glbBool CGlbGlobeDriveManipulator::UpdateFocusElevate(glbDouble elevation)
{
	return false;
}

glbBool CGlbGlobeDriveManipulator::UpdateCameraElevate(glbDouble elevation)
{
	//高度纠正，pitch角纠正
	mpr_cameraPos.z() = elevation + 1.0;
	

	return false;
}

void CGlbGlobeDriveManipulator::Shake(glbBool isShake)
{

}

void CGlbGlobeDriveManipulator::SetShakeSope(glbDouble horizontalDegree, glbDouble verticalDegree)
{

}

void CGlbGlobeDriveManipulator::SetShakeTime(glbDouble seconds)
{

}