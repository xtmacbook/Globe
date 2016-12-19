#include "StdAfx.h"
#include "GlbPlanePathManipulator.h"
#include "GlbGlobeView.h"

using namespace GlbGlobe;


CGlbPlanePathManipulator::CGlbPlanePathManipulator(CGlbGlobe* globe,CGlbGlobeDynamic* dynamicObj)
{
	mpr_globe = globe;
	mpr_focusPos = osg::Vec3d(0.0,0.0,0.0);
	mpr_distance = 0.0;
	mpr_pitch = 0.0;
	mpr_yaw = 0.0;
	mpr_roll = 0.0;
	mpr_dynamicObj = dynamicObj;
	if (dynamicObj)
		dynamicObj->GetControlPoint(&mpr_cp);
}


CGlbPlanePathManipulator::~CGlbPlanePathManipulator(void)
{
}

osg::Matrixd CGlbPlanePathManipulator::getMatrix() const
{  
	osg::Matrixd mat;
	mpr_cp.getMatrix(mat);
	//��һ�˳��ӽ�
	if (mpr_dynamicObj->GetTraceMode() == GLB_DYNAMICTRACMODE_FIRST)
		return osg::Matrixd::translate(0.0,0.0,0.01)*mat;
	//��3�˳��ӽǣ���ȡȥ����۲��ʼ��̬��pitch��roll�ǲ��ܶ�̬����Ӱ��
	osg::Vec3d centerPos = mat.getTrans();
	glbDouble yaw = atan2(-mat(1,0),mat(1,1));
	glbDouble pitch;
	if (mat(1,2) > 1.0)
		pitch = osg::PI_2;
	else if (mat(1,2) < -1.0)
		pitch = -osg::PI_2;
	else
	pitch = asin(mat(1,2));
	glbDouble roll = atan2(-mat(0,2),mat(2,2));
	glbDouble ditan = mpr_dynamicObj->GetThirdModeDistance();
	switch (mpr_dynamicObj->GetTraceMode())
	{
	case GLB_DYNAMICTRACMODE_THIRD_TOP:	  
		{
			pitch = -osg::PI_2;
		}
		break;
	case GLB_DYNAMICTRACMODE_THIRD_BOTTOM: 
		{
			pitch = osg::PI_2;
		}
		break;
	case GLB_DYNAMICTRACMODE_THIRD_LEFT:	   
		{
			yaw -= osg::PI_2;
			pitch = -osg::PI_2/9.0;
		}
		break;
	case GLB_DYNAMICTRACMODE_THIRD_RIGHT: 
		{
			yaw += osg::PI_2;
			pitch = -osg::PI_2/9.0;
		}
		break;
	case GLB_DYNAMICTRACMODE_THIRD_BACK:
		{
			pitch = -osg::PI_2/9.0;
		}
		break;		
	case GLB_DYNAMICTRACMODE_THIRD_BACKTOP:
		{
			pitch = osg::DegreesToRadians(mpr_dynamicObj->GetThirdModePitch());
		}
		break;
	}
	return osg::Matrix::translate(0.0,0.0,ditan) * osg::Matrixd::rotate(pitch + osg::PI_2,1.0,0.0,0.0) * osg::Matrix::rotate(yaw,0.0,0.0,1.0) * osg::Matrixd::translate(centerPos); 
}

void CGlbPlanePathManipulator::setByMatrix(const osg::Matrixd& matrix)
{ 
	//��ȡ���λ��
	osg::Vec3d cameraPos = matrix.getTrans();
	//����۲췽��
	osg::Vec3d lookvec = osg::Vec3d(0.0,0.0,-1.0)*matrix - cameraPos;
	osg::Vec3d focusPos;
	//������������ν���
	glbBool isfocus = mpr_globe->IsRayInterTerrain(cameraPos,lookvec,focusPos);
	if (isfocus)	//�޽���
	{
		if (mpr_globe->IsUnderGroundMode())	//��������ģʽ
		{
			if (cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() < 0.0)	//����ڵ���
			{
				isfocus = mpr_globe->IsRayInterUGPlane(cameraPos,lookvec,focusPos);
			}
		}
	}
	if (!isfocus)		//�齹�㣬ditance����ά��һ���齹��
		focusPos = osg::Vec3d(0.0,0.0,-mpr_distance) * matrix;

	mpr_distance = (focusPos - cameraPos).length();
	mpr_focusPos = focusPos;
	if (matrix(2,2) > 1.0)
		mpr_pitch = -90.0;
	else if (matrix(2,2) < -1.0)
		mpr_pitch = 90.0;
	else
		mpr_pitch = osg::RadiansToDegrees(-asin(matrix(2,2)));
	//mpr_yaw = osg::RadiansToDegrees(atan2(matrix(2,0),-matrix(2,1)));
	mpr_yaw = osg::RadiansToDegrees(atan2(-matrix(1,0),matrix(1,1)));
	mpr_roll = osg::RadiansToDegrees(atan2(-matrix(0,2),matrix(1,2)));
	//glbDouble roll = osg::RadiansToDegrees(atan2(-matrix(2,0),matrix(2,1)));
	if (abs(mpr_pitch) > 89.99)
	{
		mpr_yaw -= mpr_roll;
		mpr_roll = 0.0;
	}
}

osg::Matrixd CGlbPlanePathManipulator::getInverseMatrix() const
{
	return osg::Matrixd::inverse(getMatrix());
}

void CGlbPlanePathManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
{
	setByMatrix(osg::Matrixd::inverse(matrix));
}

glbBool CGlbPlanePathManipulator::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::FRAME):
		{
			if (!mpr_dynamicObj->GetControlPoint(&mpr_cp))
				return false;
			setByMatrix(getMatrix()); 
			//�����ƫ
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			//glbBool isUpdate = UpdateCameraElevate(mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y()));
			double elevation = 0.0;
			if (mpr_globe->GetView()->getUpdateCameraMode())
				elevation = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
			else
				elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			glbBool isUpdate = UpdateCameraElevate(elevation);

			GetCameraPos(cameraPos);
			if (cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() > 0.0)
				mpr_globe->NotifyCameraIsUnderground(false);
			else
				mpr_globe->NotifyCameraIsUnderground(true);
			aa.requestRedraw();
			return true; 
		}		
	case(osgGA::GUIEventAdapter::RESIZE):
		{
			//init(ea,aa);
			//aa.requestRedraw();
			return false;
		}
	default:
		break;
	}

	if (ea.getHandled()) 
		return false;
	return false;
}

GlbGlobeManipulatorTypeEnum CGlbPlanePathManipulator::GetType()
{
	return GLB_MANIPULATOR_ANIMATIONPATH;
}

glbDouble CGlbPlanePathManipulator::GetYaw()
{
	return mpr_yaw;
}

glbDouble CGlbPlanePathManipulator::GetPitch()
{
	return mpr_pitch;
}

glbBool CGlbPlanePathManipulator::GetCameraPos(osg::Vec3d& cameraPos)
{
	cameraPos = getMatrix().getTrans();
	return true;
}
void CGlbPlanePathManipulator::SetCameraYaw(glbDouble yaw)
{
	return;
}
glbDouble CGlbPlanePathManipulator::GetCameraYaw()
{
	return 0.0;
}
void CGlbPlanePathManipulator::SetCameraPitch(glbDouble pitch)
{
	return;
}
glbDouble CGlbPlanePathManipulator::GetCameraPitch()
{
	return 0.0;
}

glbBool CGlbPlanePathManipulator::GetFocusPos(osg::Vec3d& focusPos)
{
	focusPos = mpr_focusPos;
	return true;
}

glbDouble CGlbPlanePathManipulator::GetDistance()
{
	return mpr_distance;
}

glbBool CGlbPlanePathManipulator::UpdateCameraElevate(glbDouble elevation)
{
	//������ξ�ƫ������������ߣ���Ȼ����̬����pitch�ı�
	osg::Vec3d oldcameraPos;
	GetCameraPos(oldcameraPos);
	if(oldcameraPos.z() - elevation < 0.0)
	{
		osg::Vec3d cameraPos(oldcameraPos.x(),oldcameraPos.y(),elevation);
		osg::AnimationPath::ControlPoint cp;
		mpr_dynamicObj->GetControlPoint(&cp);
		osg::Vec3d center = cp.getPosition();
		osg::Quat quat;
		quat.makeRotate(center-oldcameraPos,center-cameraPos);
		osg::Matrixd mat = getMatrix()*osg::Matrix::translate(cameraPos-oldcameraPos)*osg::Matrixd::rotate(quat);
		setByMatrix(mat);
		return true;
	}	
	return false;
}

glbBool CGlbPlanePathManipulator::FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
	return false;
}

glbBool CGlbPlanePathManipulator::FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, bool repeat)
{
	return false;
}

glbBool CGlbPlanePathManipulator::IsFlying()
{
	return false;
}

glbBool CGlbPlanePathManipulator::StopFlying()
{
	return false;
}

glbBool CGlbPlanePathManipulator::JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch)
{
	return false;
}

glbBool CGlbPlanePathManipulator::SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance)
{
	return false;
}

glbBool CGlbPlanePathManipulator::SetPitch(glbDouble pitch,osg::Vec3d old_focus,glbDouble olddistance)
{
	return false;
}

glbBool CGlbPlanePathManipulator::Move(glbDouble dangleX,glbDouble dangleY)
{
	return false;
}

glbBool CGlbPlanePathManipulator::TranslateCamera(glbDouble x,glbDouble y,glbDouble z)
{
	return false;
}

glbBool CGlbPlanePathManipulator::DetectCollision(glbBool mode)
{
	return false;
}

glbBool CGlbPlanePathManipulator::SetUnderGroundMode(glbBool mode)
{
	return false;
}

glbBool CGlbPlanePathManipulator::SetUndergroundDepth(glbDouble zOrAlt)
{
	return false;
}

glbBool CGlbPlanePathManipulator::UpdateFocusElevate(glbDouble elevation)
{
	return false;
}

void CGlbPlanePathManipulator::Shake(glbBool isShake)
{

}

void CGlbPlanePathManipulator::SetShakeSope(glbDouble horizontalDegree, glbDouble verticalDegree)
{

}

void CGlbPlanePathManipulator::SetShakeTime(glbDouble seconds)
{

}

void CGlbPlanePathManipulator::EnalbeVirtualReferencePlane(glbBool bEnable)
{
	
}		
void CGlbPlanePathManipulator::SetVirtualReferencePlane(glbDouble zOrAltitude)
{
	
}		
glbDouble CGlbPlanePathManipulator::GetVirtualReferencePlane()
{
	return -10.0;
}