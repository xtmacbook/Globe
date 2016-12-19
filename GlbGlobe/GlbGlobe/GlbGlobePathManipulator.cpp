#include "StdAfx.h"
#include "GlbGlobePathManipulator.h"
#include "GlbGlobeView.h"
#include "GlbGlobeMath.h"

using namespace GlbGlobe;

CGlbGlobePathManipulator::CGlbGlobePathManipulator(CGlbGlobe* globe, CGlbGlobeDynamic *dynamicObj) :
	mpr_globe(globe),
	mpr_dynamicObj(dynamicObj),
	mpr_focusPos(osg::Vec3d(0.0,0.0,0.0)),
	mpr_distance(1.0),
	mpr_pitch(0.0),
	mpr_yaw(0.0),
	mpr_roll(0.0)
{
	if (dynamicObj)
		dynamicObj->GetControlPoint(&mpr_cp);
}


CGlbGlobePathManipulator::~CGlbGlobePathManipulator(void)
{
}

osg::Matrixd CGlbGlobePathManipulator::getMatrix() const
{
	return mpr_mat;
}

osg::Matrixd CGlbGlobePathManipulator::computeMatrix()
{  
	osg::Matrixd mat;
	mpr_cp.getMatrix(mat);
	//第一人称视角
	//if (mpr_dynamicObj->GetTraceMode() == GLB_DYNAMICTRACMODE_FIRST)
		//return /*osg::Matrix::rotate(osg::PI_2,1.0,0.0,0.0)**/osg::Matrixd::translate(0.0,0.0,0.01)*mat;
	//第3人称视角，获取去相机观察初始姿态，pitch，roll角不受动态对象影响
	osg::Matrixd localToworld;
	osg::Vec3d centerPos = mat.getTrans();
	g_ellipsmodel->computeLocalToWorldTransformFromXYZ(centerPos.x(),centerPos.y(),centerPos.z(),localToworld);
	//mat.postMult(osg::Matrixd::inverse(localToworld));
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
	case GLB_DYNAMICTRACMODE_FIRST:
		{
			//mpr_dynamicObj->SetShow(false);
			return osg::Matrixd::translate(0.0,0.0,0.01)/**osg::Matrix::rotate(osg::PI_2,1.0,0.0,0.0)*/ * mat/* * localToworld*/;
		}
		break;
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
			//pitch = -osg::PI_2*0.7;
		}
		break;
	}
	//先yaw，后pitch，再roll（z_x_y）
	//return osg::Matrixd::rotate(osg::PI_2,1.0,0.0,0.0) * osg::Matrixd::translate(0.0,-ditan,0.0) * osg::Matrixd::rotate(pitch,1.0,0.0,0.0) * osg::Matrixd::rotate(yaw,0.0,0.0,1.0) * localToworld; 
	return osg::Matrixd::translate(0.0,0.0,ditan) * osg::Matrixd::rotate(osg::PI_2+pitch,1.0,0.0,0.0) * osg::Matrixd::rotate(yaw,0.0,0.0,1.0) * localToworld; 
}

//osg::Matrixd CGlbGlobePathManipulator::getMatrix() const
//{
//	osg::Matrixd focusToWorld;
//	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(mpr_focusPos.y()),osg::DegreesToRadians(mpr_focusPos.x()),mpr_focusPos.z(),focusToWorld);
//	osg::Vec3d yaw_vec(0.0,0.0,1.0);
//	osg::Vec3d pitch_vec(cos(osg::DegreesToRadians(mpr_yaw)),sin(osg::DegreesToRadians(mpr_yaw)),0.0);
//	osg::Vec3d roll_vec(-sin(osg::DegreesToRadians(mpr_yaw))*cos(osg::DegreesToRadians(mpr_pitch)),cos(osg::DegreesToRadians(mpr_pitch))*cos(osg::DegreesToRadians(mpr_yaw)),sin(osg::DegreesToRadians(mpr_pitch))); 
//	//Z_X_Y
//	return osg::Matrixd::translate(0.0,0.0,mpr_distance)*
//		osg::Matrixd::rotate(osg::DegreesToRadians(mpr_yaw),yaw_vec)*
//		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(mpr_pitch),pitch_vec)*		
//		osg::Matrixd::rotate(osg::DegreesToRadians(mpr_roll),roll_vec)*
//		focusToWorld;
//}

void CGlbGlobePathManipulator::setByMatrix(const osg::Matrixd& matrix)
{
	osg::Vec3d cameraPos_w(matrix(3,0),matrix(3,1),matrix(3,2));
	osg::Vec3d cameraPos;
	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	glbDouble distan = cameraPos_w.length();
	osg::Vec3d focusPos_w = osg::Vec3d(0,0,-mpr_distance)*matrix;
	osg::Vec3d lookvec = focusPos_w - cameraPos_w;
	osg::Vec3d oldfocusPos;
	DegToRad(mpr_focusPos);
	g_ellipsmodel->convertLatLongHeightToXYZ(mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z(),oldfocusPos.x(),oldfocusPos.y(),oldfocusPos.z());
	osg::Vec3d up(cos(mpr_focusPos.x())*cos(mpr_focusPos.y()), sin(mpr_focusPos.x())*cos(mpr_focusPos.y()), sin(mpr_focusPos.y()));
	std::vector<osg::Vec3d> IntersectPoses;
	//glbInt32 IntersectResult = IntersectRayPlane(cameraPos_w,focusPos_w,oldfocusPos,up,IntersectPoses);
	glbDouble radius = oldfocusPos.length();
	glbInt32 IntersectResult = IntersectRaySphere(cameraPos_w,focusPos_w,osg::Vec3d(0.0,0.0,0.0),radius,IntersectPoses);
	if (IntersectResult)
	{
		if (abs(cameraPos.z()) < 10000.0)
		{
			osg::Vec3d IntersectPos = IntersectPoses[0];
			glbDouble dist = (IntersectPos-cameraPos_w).length();
			if (dist < osg::WGS_84_RADIUS_EQUATOR*osg::PI/180.0)
			{
				focusPos_w = IntersectPos;
				mpr_distance = dist;
			}else{
				mpr_distance = min(2000.0,mpr_distance);
				osg::Vec3d focusPos_w = osg::Vec3d(0,0,-mpr_distance)*matrix;
				mpr_distance = (focusPos_w-cameraPos_w).length();
			}
		}else{
			focusPos_w = IntersectPoses[0];
			mpr_distance = (focusPos_w-cameraPos_w).length();
		}
	}else
		mpr_distance = (focusPos_w-cameraPos_w).length();
	////相机在地下
	//osg::Vec3d cameraPos;
	//g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	//if (mpr_globe->IsUnderGroundMode() && !IntersectResult)
	//{
	//	if (cameraPos.z() < mpr_globe->GetView()->GetTerrainElevationOfCameraPos())	//相机在地下
	//	{
	//		osg::Vec3d UGfocusPos;
	//		glbBool isUGfocus = mpr_globe->IsRayInterUGPlane(cameraPos_w,focusPos_w,UGfocusPos);
	//		if (isUGfocus)
	//		{
	//			glbDouble dist = (UGfocusPos-cameraPos_w).length();
	//			//if (dist < mpr_distance)
	//			{
	//				mpr_distance = dist;
	//				focusPos_w = UGfocusPos;
	//			}
	//		}
	//	}
	//}
	g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z());
	osg::Matrixd mat_focusToworld;
	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z(),mat_focusToworld);	
	osg::Matrixd mat = matrix * osg::Matrixd::inverse(mat_focusToworld);		
	RadToDeg(mpr_focusPos);

	//转化为欧拉角时需考虑奇点问题，即pitch为正负90°的情况：
	if (mat(2,2) > 1.0)			//-90°
	{
		mpr_pitch = -90.0;
		mpr_yaw = osg::RadiansToDegrees(atan2(mat(2,0),-mat(2,1)));
		mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(1,2)));
		mpr_yaw -= mpr_roll;
		mpr_roll = 0.0;
	}
	else if (mat(2,2) < -1.0)	//90°
	{
		mpr_pitch = 90.0;
		mpr_yaw = osg::RadiansToDegrees(atan2(mat(2,0),-mat(2,1)));
		mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(1,2)));
		mpr_yaw -= mpr_roll;
		mpr_roll = 0.0;
	}
	else
	{
		mpr_pitch = osg::RadiansToDegrees(-asin(mat(2,2)));
		mpr_yaw = osg::RadiansToDegrees(atan2(mat(2,0),-mat(2,1)));
		mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(1,2)));
		if (abs(mpr_pitch) > 89.99)
		{
			mpr_yaw -= mpr_roll;
			mpr_roll = 0.0;
		}
	}
}

// void CGlbGlobePathManipulator::setByMatrix(const osg::Matrixd& matrix)
// {
// 	//获取相机位置
// 	osg::Vec3d cameraPos = matrix.getTrans();
// 	//相机观察方向
// 	osg::Vec3d lookvec = osg::Vec3d(0.0,0.0,-1.0)*matrix - cameraPos;
// 	osg::Vec3d focusPos;
// 	//计算射线与地形交点
// 	glbBool isfocus = mpr_globe->IsRayInterTerrain(cameraPos,lookvec,focusPos);
// 	if (!isfocus)	//无交点
// 	{
// 		if (mpr_globe->IsUnderGroundMode())	//开启地下模式
// 		{
// 			//相机在地下
// 			glbDouble cameralon,cameralat,cameraalt;
// 			g_ellipsmodel->convertXYZToLatLongHeight(cameraPos.x(),cameraPos.y(),cameraPos.z(),cameralat,cameralon,cameraalt);
// 			if (cameraalt - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() < 0.0)	//相机在地下
// 			{
// 				isfocus = mpr_globe->IsRayInterUGPlane(cameraPos,lookvec,focusPos);
// 			}
// 		}
// 	}
// 	if (!isfocus)		//虚焦点，ditance不变维护一个虚焦点
// 		focusPos = osg::Vec3d(0.0,0.0,-mpr_distance) * matrix;
// 
// 	mpr_distance = (focusPos - cameraPos).length();
// 	g_ellipsmodel->convertXYZToLatLongHeight(focusPos.x(),focusPos.y(),focusPos.z(),mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z());
// 	mpr_focusPos.x() = osg::RadiansToDegrees(mpr_focusPos.x());
// 	mpr_focusPos.y() = osg::RadiansToDegrees(mpr_focusPos.y());
// 	osg::Matrixd focusToworld;
// 	g_ellipsmodel->computeLocalToWorldTransformFromXYZ(focusPos.x(),focusPos.y(),focusPos.z(),focusToworld);
// 
// 	osg::Matrixd mat = matrix /** osg::Matrixd::translate(focusPos - cameraPos)*/ * osg::Matrixd::inverse(focusToworld);
// 
// 	//转化为欧拉角时需考虑奇点问题，即pitch为正负90°的情况：
// 	if (mat(2,2) > 1.0)			//-90°
// 	{
// 		mpr_pitch = -90.0;
// 		mpr_yaw = osg::RadiansToDegrees(atan2(mat(2,0),-mat(2,1)));
// 		mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(1,2)));
// 		mpr_yaw -= mpr_roll;
// 		mpr_roll = 0.0;
// 	}
// 	else if (mat(2,2) < -1.0)	//90°
// 	{
// 		mpr_pitch = 90.0;
// 		mpr_yaw = osg::RadiansToDegrees(atan2(mat(2,0),-mat(2,1)));
// 		mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(1,2)));
// 		mpr_yaw -= mpr_roll;
// 		mpr_roll = 0.0;
// 	}
// 	else
// 	{
// 		mpr_pitch = osg::RadiansToDegrees(-asin(mat(2,2)));
// 		mpr_yaw = osg::RadiansToDegrees(atan2(mat(2,0),-mat(2,1)));
// 		mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(1,2)));
// 		if (abs(mpr_pitch) > 89.99)
// 		{
// 			mpr_yaw -= mpr_roll;
// 			mpr_roll = 0.0;
// 		}
// 	}
// }

osg::Matrixd CGlbGlobePathManipulator::getInverseMatrix() const
{
	return osg::Matrixd::inverse(getMatrix());
}

void CGlbGlobePathManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
{
	setByMatrix(osg::Matrixd::inverse(matrix));
}

glbBool CGlbGlobePathManipulator::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::FRAME):
		{
			//if (!mpr_dynamicObj->IsRunning())
			//{
			//	mpr_globe->GetView()->SetDragMode();
			//	return false;
			//}
			////////////////////////////////////////////////////////////////////////////
			//osg::AnimationPath::ControlPoint cp;
			//mpr_dynamicObj->GetControlPoint(&cp);
			//osg::Vec3d position = cp.getPosition();
			//g_ellipsmodel->convertXYZToLatLongHeight(position.x(),position.y(),position.z(),mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z());
			//mpr_focusPos.x() = osg::RadiansToDegrees(mpr_focusPos.x());
			//mpr_focusPos.y() = osg::RadiansToDegrees(mpr_focusPos.y());

			//osg::Matrixd localToworld;
			//g_ellipsmodel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),localToworld);
			//osg::Matrixd mat;
			//cp.getMatrix(mat);
			//mat.postMult(osg::Matrixd::inverse(localToworld));

			//mpr_pitch = osg::RadiansToDegrees(asin(mat(1,2)));
			//mpr_yaw = osg::RadiansToDegrees(atan2(-mat(1,0),mat(1,1)));
			//mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(2,2)));
			//switch (mpr_dynamicObj->GetTraceMode())
			//{
			//case GLB_DYNAMICTRACMODE_FIRST:		//第一人称
			//	{
			//		mpr_distance = 0.01;
			//	}
			//	break;
			//case GLB_DYNAMICTRACMODE_THIRD_TOP:	  
			//	{
			//		mpr_distance = mpr_dynamicObj->GetThirdModeDistance();
			//		mpr_pitch = -90.0;
			//		mpr_roll = 0.0;
			//	}
			//	break;
			//case GLB_DYNAMICTRACMODE_THIRD_BOTTOM: 
			//	{
			//		mpr_distance = mpr_dynamicObj->GetThirdModeDistance();
			//		mpr_pitch = 90.0;
			//		mpr_roll = 0.0;
			//	}
			//	break;
			//case GLB_DYNAMICTRACMODE_THIRD_LEFT:	   
			//	{
			//		mpr_distance = mpr_dynamicObj->GetThirdModeDistance();
			//		mpr_pitch = -10.0;
			//		mpr_yaw -= 90.0;
			//		mpr_roll = 0.0;
			//	}
			//	break;
			//case GLB_DYNAMICTRACMODE_THIRD_RIGHT: 
			//	{
			//		mpr_distance = mpr_dynamicObj->GetThirdModeDistance();
			//		mpr_pitch = -10.0;
			//		mpr_yaw += 90.0;
			//		mpr_roll = 0.0;
			//	}
			//	break;
			//case GLB_DYNAMICTRACMODE_THIRD_BACK:
			//	{
			//		mpr_distance = mpr_dynamicObj->GetThirdModeDistance();
			//		mpr_pitch = -10.0;
			//		mpr_roll = 0.0;
			//	}
			//	break;		
			//case GLB_DYNAMICTRACMODE_THIRD_BACKTOP:
			//	{
			//		mpr_distance = mpr_dynamicObj->GetThirdModeDistance();
			//		mpr_pitch = mpr_dynamicObj->GetThirdModePitch();
			//		mpr_roll = 0.0;
			//	}
			//	break;
			//}
			//////////////////////////////////////////////////////////////////////////
			//相机纠偏
			//UpdateFocusElevate(getMatrix());
			//UpdateCameraElevate();

			//writepath();
			//相机纠偏
			if (!mpr_dynamicObj->GetControlPoint(&mpr_cp))
				return false;
			mpr_mat = computeMatrix();
			setByMatrix(mpr_mat);
			osg::Vec3d cameraPos;
			//GetCameraPos(cameraPos);
			glbDouble cameraElevate = 0.0;
			//if (mpr_globe->GetView()->getUpdateCameraMode())
			//	cameraElevate = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
			//else
				cameraElevate = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			//UpdateCameraElevate(cameraElevate);
			GetCameraPos(cameraPos);
			if (cameraPos.z() - cameraElevate > 0.0)
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

GlbGlobeManipulatorTypeEnum CGlbGlobePathManipulator::GetType()
{
	return GLB_MANIPULATOR_ANIMATIONPATH;
}

glbDouble CGlbGlobePathManipulator::GetYaw()
{
	return mpr_yaw;
}

glbDouble CGlbGlobePathManipulator::GetPitch()
{
	return mpr_pitch;
}

glbBool CGlbGlobePathManipulator::GetCameraPos(osg::Vec3d& cameraPos)
{
	osg::Vec3d cameraPos_w = osg::Vec3d(0.0,0.0,0.0) * getMatrix();
	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	RadToDeg(cameraPos);
	return true;
}
void CGlbGlobePathManipulator::SetCameraYaw(glbDouble yaw)
{
	return;
}
glbDouble CGlbGlobePathManipulator::GetCameraYaw()
{
	return 0.0;
}
void CGlbGlobePathManipulator::SetCameraPitch(glbDouble pitch)
{
	return;
}
glbDouble CGlbGlobePathManipulator::GetCameraPitch()
{
	return 0.0;
}

glbBool CGlbGlobePathManipulator::GetFocusPos(osg::Vec3d& focusPos)
{
	focusPos = mpr_focusPos;
	return true;
}

glbDouble CGlbGlobePathManipulator::GetDistance()
{
	return mpr_distance;
}

glbBool CGlbGlobePathManipulator::UpdateCameraElevate(glbDouble elevation)
{
	//相机地形纠偏，相机海拔拉高，依然看向动态对象，pitch改变
	osg::Vec3d oldcameraPos;
	GetCameraPos(oldcameraPos);
	if(oldcameraPos.z() - elevation < 0.0)
	{
		osg::Vec3d cameraPos(oldcameraPos.x(),oldcameraPos.y(),elevation);
		osg::AnimationPath::ControlPoint cp;
		mpr_dynamicObj->GetControlPoint(&cp);
		osg::Vec3d center = cp.getPosition();
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(oldcameraPos.y()),osg::DegreesToRadians(oldcameraPos.x()),oldcameraPos.z(),oldcameraPos.x(),oldcameraPos.y(),oldcameraPos.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPos.x(),cameraPos.y(),cameraPos.z());
		osg::Quat quat;
		quat.makeRotate(center-oldcameraPos,center-cameraPos);
		osg::Matrixd mat = getMatrix()*osg::Matrix::translate(cameraPos-oldcameraPos)*osg::Matrixd::rotate(quat);
		setByMatrix(mat);
		return true;
	}	
	return false;
}

glbBool CGlbGlobePathManipulator::FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
	return false;
}

glbBool CGlbGlobePathManipulator::FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, bool repeat)
{
	return false;
}

glbBool CGlbGlobePathManipulator::IsFlying()
{
	return false;
}

glbBool CGlbGlobePathManipulator::StopFlying()
{
	return false;
}

glbBool CGlbGlobePathManipulator::JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch)
{
	mpr_focusPos = osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	mpr_yaw = yaw;
	mpr_pitch = pitch;
	mpr_distance = distance;
	return false;
}

glbBool CGlbGlobePathManipulator::SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance)
{
	return false;
}

glbBool CGlbGlobePathManipulator::SetPitch(glbDouble pitch,osg::Vec3d old_focus,glbDouble olddistance)
{
	return false;
}

glbBool CGlbGlobePathManipulator::Move(glbDouble dangleX,glbDouble dangleY)
{
	return false;
}

glbBool CGlbGlobePathManipulator::TranslateCamera(glbDouble x,glbDouble y,glbDouble z)
{
	return false;
}

glbBool CGlbGlobePathManipulator::DetectCollision(glbBool mode)
{
	return false;
}

glbBool CGlbGlobePathManipulator::SetUnderGroundMode(glbBool mode)
{
	return false;
}

glbBool CGlbGlobePathManipulator::SetUndergroundDepth(glbDouble zOrAlt)
{
	return false;
}

glbBool CGlbGlobePathManipulator::UpdateFocusElevate(glbDouble elevation)
{
	//获取相机位置
	osg::Vec3d cameraPos = mpr_mat.getTrans();
	//相机观察方向
	osg::Vec3d lookvec = osg::Vec3d(0.0,0.0,-1.0)*mpr_mat - cameraPos;
	osg::Vec3d focusPos;
	//计算射线与地形交点
	glbBool isfocus = mpr_globe->IsRayInterTerrain(cameraPos,lookvec,focusPos);
	if (!isfocus)	//无交点
	{
	 	if (mpr_globe->IsUnderGroundMode())	//开启地下模式
	 	{
	 		//相机在地下
	 		glbDouble cameralon,cameralat,cameraalt;
	 		g_ellipsmodel->convertXYZToLatLongHeight(cameraPos.x(),cameraPos.y(),cameraPos.z(),cameralat,cameralon,cameraalt);
	 		if (cameraalt - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() < 0.0)	//相机在地下
	 		{
	 			isfocus = mpr_globe->IsRayInterUGPlane(cameraPos,lookvec,focusPos);
	 		}
	 	}
	}
	if (!isfocus)		//虚焦点，ditance不变维护一个虚焦点
	 	focusPos = osg::Vec3d(0.0,0.0,-mpr_distance) * mpr_mat;
	 
	mpr_distance = (focusPos - cameraPos).length();
	g_ellipsmodel->convertXYZToLatLongHeight(focusPos.x(),focusPos.y(),focusPos.z(),mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z());
	mpr_focusPos.x() = osg::RadiansToDegrees(mpr_focusPos.x());
	mpr_focusPos.y() = osg::RadiansToDegrees(mpr_focusPos.y());
	osg::Matrixd focusToworld;
	g_ellipsmodel->computeLocalToWorldTransformFromXYZ(focusPos.x(),focusPos.y(),focusPos.z(),focusToworld);
	 
	osg::Matrixd mat = mpr_mat /** osg::Matrixd::translate(focusPos - cameraPos)*/ * osg::Matrixd::inverse(focusToworld);
	 
	//转化为欧拉角时需考虑奇点问题，即pitch为正负90°的情况：
	if (mat(2,2) > 1.0)			//-90°
	{
	 	mpr_pitch = -90.0;
	 	mpr_yaw = osg::RadiansToDegrees(atan2(mat(2,0),-mat(2,1)));
	 	mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(1,2)));
	 	mpr_yaw -= mpr_roll;
	 	mpr_roll = 0.0;
	}
	else if (mat(2,2) < -1.0)	//90°
	{
	 	mpr_pitch = 90.0;
	 	mpr_yaw = osg::RadiansToDegrees(atan2(mat(2,0),-mat(2,1)));
	 	mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(1,2)));
	 	mpr_yaw -= mpr_roll;
	 	mpr_roll = 0.0;
	}
	else
	{
	 	mpr_pitch = osg::RadiansToDegrees(-asin(mat(2,2)));
	 	mpr_yaw = osg::RadiansToDegrees(atan2(mat(2,0),-mat(2,1)));
	 	mpr_roll = osg::RadiansToDegrees(atan2(-mat(0,2),mat(1,2)));
	 	if (abs(mpr_pitch) > 89.99)
	 	{
	 		mpr_yaw -= mpr_roll;
	 		mpr_roll = 0.0;
	 	}
	}
	return true;
}

void CGlbGlobePathManipulator::Shake(glbBool isShake)
{

}

void CGlbGlobePathManipulator::SetShakeSope(glbDouble horizontalDegree, glbDouble verticalDegree)
{

}

void CGlbGlobePathManipulator::SetShakeTime(glbDouble seconds)
{

}

void CGlbGlobePathManipulator::EnalbeVirtualReferencePlane(glbBool bEnable)
{

}		
void CGlbGlobePathManipulator::SetVirtualReferencePlane(glbDouble zOrAltitude)
{

}		
glbDouble CGlbGlobePathManipulator::GetVirtualReferencePlane()
{
	return -10.0;
}

#include "GlbString.h"
#include <fstream>

void CGlbGlobePathManipulator::writepath()
{
	//CGlbWString fliepath = L"d:\\outpath.txt";
	//std::ofstream file(fliepath.ToString(),ios::out|ios::app); 
	//if (!file)
	//	return;
	////file << mpr_yaw << ' ';
	//file << mpr_pitch << std::endl;
	//file.close();

}
