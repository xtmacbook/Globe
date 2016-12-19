#include "StdAfx.h"
#include "GlbGlobeManipulator.h"
#include "GlbGlobeView.h"
#include "GlbGlobeMath.h"

using namespace GlbGlobe;

glbDouble testx,testy;

CGlbGlobeManipulator::CGlbGlobeManipulator(CGlbGlobe *globe)
{
	mpr_globe = globe;	
	mpr_focusPos = osg::Vec3d(116.3,39.9,0.0);	//北京经纬度
	mpr_distance = 30000000;					//3万公里
	mpr_yaw = 0.0;
	mpr_pitch = -90.0;
	mpr_roll = 0.0;
	_ga_t0 = NULL;
	_ga_t1 = NULL;
	_ga_touch = NULL;
	_ga_release = NULL;
	_pushPos = osg::Vec3d(DBL_MAX,DBL_MAX,DBL_MAX);
	_pushPos2 = osg::Vec3d(DBL_MAX,DBL_MAX,DBL_MAX);
	_FlyCallback = NULL;
	isVirtualFocus = false;
	mpr_lockmode = GLB_LOCKMODE_FREE;
	mpr_isctrldown = false;
	mpr_isshiftdown = false;
	mpr_isDetectCollision = false;
	mpr_isShake = false;
	mpr_shakeTime = -1;
	mpr_shakeStartTime = -1;
	mpr_shakeHorDegree = 5;
	mpr_shakeVerDegree = 2;

	mpr_bUseReferencePlane = false;
	mpr_referencePlaneZ = -10.0;
}


CGlbGlobeManipulator::~CGlbGlobeManipulator(void)
{
}

glbBool CGlbGlobeManipulator::SetLockMode(GlbGlobeLockModeEnum type)
{ 
	if (mpr_lockmode == type)
		return false;
	mpr_lockmode = type;
	flushMouseEventStack();
	switch(type)
	{
	case GLB_LOCKMODE_FREE:
		return true;
	case GLB_LOCKMODE_2D:
		{ 
			SetCameraPitch(-90.0);
			return true;
		}		
	case GLB_LOCKMODEL_2DN:
		{ 
			SetCameraPitch(-90.0);
			SetCameraYaw(0.0);
			return true;
		}
	default:
		break;
	}
	return true;
}

void CGlbGlobeManipulator::home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	mpr_focusPos = osg::Vec3d(116.3,39.9,0.0);	//北京经纬度
	mpr_distance = 30000000;					//3万公里
	mpr_yaw = 0.0;
	mpr_pitch = -90.0;
	mpr_isUnderGround = false;
	mpr_globe->NotifyCameraIsUnderground(mpr_isUnderGround);
	aa.requestRedraw();
}

void CGlbGlobeManipulator::setByMatrix(const osg::Matrixd& matrix)
{	
	osg::Vec3d cameraPos_w(matrix(3,0),matrix(3,1),matrix(3,2));
	glbDouble distan = cameraPos_w.length();
	osg::Vec3d focusPos_w = osg::Vec3d(0,0,-mpr_distance)*matrix;
	osg::Vec3d lookvec = focusPos_w - cameraPos_w;
	osg::Vec3d oldfocusPos;
	
	osg::Vec3d tempFocusPos = mpr_focusPos;
	DegToRad(tempFocusPos);
	g_ellipsmodel->convertLatLongHeightToXYZ(tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z(),oldfocusPos.x(),oldfocusPos.y(),oldfocusPos.z());
	osg::Vec3d up(cos(tempFocusPos.x())*cos(tempFocusPos.y()), sin(tempFocusPos.x())*cos(tempFocusPos.y()), sin(tempFocusPos.y()));
	std::vector<osg::Vec3d> IntersectPoses;
	glbInt32 IntersectResult = IntersectRayPlane(cameraPos_w,focusPos_w,oldfocusPos,up,IntersectPoses);
	//glbDouble radius = oldfocusPos.length();
	//glbInt32 IntersectResult = IntersectRaySphere(cameraPos_w,focusPos_w,osg::Vec3d(0.0,0.0,0.0),radius,IntersectPoses);
	if (IntersectResult>0)
	{
		focusPos_w = IntersectPoses[0];
	}
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
	g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z());
	osg::Matrixd mat_focusToworld;
	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z(),mat_focusToworld);	
	osg::Matrixd mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);		
	RadToDeg(tempFocusPos);
	mpr_focusPos = tempFocusPos;

	mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));

	if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)	//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
	{
		glbDouble dm = mat_camTofocus(2,0)*mat_camTofocus(2,0)+mat_camTofocus(2,1)*mat_camTofocus(2,1)+mat_camTofocus(2,2)*mat_camTofocus(2,2);
		dm = sqrt(dm);
		mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
	}else{	  
		mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
	}

	if (mpr_isShake)
	{// 更新位置
		mpr_orignPitch = mpr_pitch;
		mpr_orignyaw = mpr_yaw;
		GetFocusPos(mpr_shakefocusPos);
		mpr_shakeDistance = mpr_distance;
	}
	
}

//此函数ok,此函数同时进行了焦点纠偏
void CGlbGlobeManipulator::setByMatrix2(const osg::Matrixd& matrix)
{
	osg::Vec3d focusPos_w,ObjfocusPos_w;
	osg::Vec3d focusPos;
	osg::Vec3d cameraPos;
	osg::Vec3d cameraPos_w(matrix(3,0),matrix(3,1),matrix(3,2));

	if (abs(mpr_distance) < 0.01)
	{// 保持相机--焦点距离不能小于阈值
		mpr_distance = 0.01;
	}
	osg::Vec3d lookvec = osg::Vec3d(0.0,0.0,-mpr_distance)*matrix - cameraPos_w;
	//计算射线与地形交点
	glbBool isfocus = mpr_globe->IsRayInterTerrain(cameraPos_w,lookvec,focusPos_w);

	osg::Matrixd mat_focusToworld;
	osg::Matrixd mat_camTofocus;
	osg::Vec3d   oldfocus    = mpr_focusPos;
	glbDouble    olddistance = mpr_distance;
	if (mpr_globe->IsUnderGroundMode())	//地下模式，相机在地下时
	{
		//相机位置地形海拔
		glbDouble cameraAlt = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();		
		g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),
			cameraPos.y(),cameraPos.x(),cameraPos.z());
		if (cameraPos.z() < cameraAlt)
		{//相机在地下时
			//射线与地形交点,不能太远,distance有距离限制
			if (isfocus)
			{//相机焦点射线与地形有交点
				g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),
					focusPos.y(),focusPos.x(),focusPos.z());
				osg::Vec3d tempFocusPos;
				tempFocusPos = focusPos;
				g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z(),mat_focusToworld);	
				mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);
				RadToDeg(tempFocusPos);
				mpr_focusPos = tempFocusPos;
				mpr_distance = (focusPos_w - cameraPos_w).length();

				if (mpr_distance < WGS_84_RADIUS_POLAR*osg::DegreesToRadians(1.0))	
				{ //ditance不太远
					//mpr_isVirtualFocus = false;
					mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));
					if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)
					{//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
						glbDouble dm =   mat_camTofocus(2,0)*mat_camTofocus(2,0)
							+ mat_camTofocus(2,1)*mat_camTofocus(2,1)
							+ mat_camTofocus(2,2)*mat_camTofocus(2,2);
						dm = sqrt(dm);
						mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
					}else{	  
						mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
					}					
					return;
				}
			}
			//计算射线与地下参考面交点
			glbBool isugfocus = mpr_globe->IsRayInterUGPlane(cameraPos_w,lookvec,focusPos_w);
			if (isugfocus)
			{
				g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),
					focusPos.y(),focusPos.x(),focusPos.z());
				//mpr_focusPos = focusPos;
				osg::Vec3d tempFocusPos = focusPos;
				g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z(),mat_focusToworld);	
				mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);
				RadToDeg(tempFocusPos);
				mpr_focusPos = tempFocusPos;
				mpr_distance = (focusPos_w - cameraPos_w).length();

				if (mpr_distance < WGS_84_RADIUS_POLAR*osg::DegreesToRadians(1.0))	
				{//ditance不太远 
					//mpr_isVirtualFocus = false;
					mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));

					if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)
					{//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
						glbDouble dm =   mat_camTofocus(2,0)*mat_camTofocus(2,0)
							+ mat_camTofocus(2,1)*mat_camTofocus(2,1)
							+ mat_camTofocus(2,2)*mat_camTofocus(2,2);
						dm = sqrt(dm);
						mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
					}else{	  
						mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
					}					
					return;
				}
			}
			//地下模式-相机在地下：无焦点 或者 相机到焦点距离太大
			//mpr_isVirtualFocus  = true;
			mpr_virtualFocusPos = oldfocus;
			olddistance         = min(1000.0,olddistance);
			osg::Vec3d tempFocusPos;
			tempFocusPos        = osg::Vec3d(0,0,-olddistance)*matrix;
			mpr_distance        = olddistance;
			g_ellipsmodel->convertXYZToLatLongHeight(tempFocusPos.x(),tempFocusPos.y(),tempFocusPos.z(),
				tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z());
			g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z(),mat_focusToworld);	
			mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);
			RadToDeg(tempFocusPos);
			mpr_focusPos = tempFocusPos;

			mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));
			if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)
			{//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
				glbDouble dm =   mat_camTofocus(2,0)*mat_camTofocus(2,0)
					+ mat_camTofocus(2,1)*mat_camTofocus(2,1)
					+ mat_camTofocus(2,2)*mat_camTofocus(2,2);
				dm = sqrt(dm);
				mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
			}else{	  
				mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
			}			
			return;
		}
	}
	//相机在地上：地下/地上模式
	if (isfocus)	
	{//与地形有焦点	
		//mpr_isVirtualFocus = false;
		g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),
			focusPos.y(),focusPos.x(),focusPos.z());
		//mpr_focusPos = focusPos;
		g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(focusPos.y(),focusPos.x(),focusPos.z(),mat_focusToworld);	
		mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);
		RadToDeg(focusPos);
		mpr_focusPos = focusPos;
		mpr_distance = (focusPos_w - cameraPos_w).length();
	}else{		//与没有焦点
		//mpr_isVirtualFocus  = true;
		mpr_distance        = min(1000.0,olddistance);
		mpr_virtualFocusPos = mpr_focusPos;
		//mpr_focusPos        = osg::Vec3d(0,0,-mpr_distance)*matrix;
		osg::Vec3d tempFocusPos = osg::Vec3d(0,0,-mpr_distance)*matrix;
		g_ellipsmodel->convertXYZToLatLongHeight(tempFocusPos.x(),tempFocusPos.y(),tempFocusPos.z(),
			tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z());
		g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z(),mat_focusToworld);	
		mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);
		RadToDeg(tempFocusPos);
		mpr_focusPos = tempFocusPos;
	}	
	mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));
	if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)
	{//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
		glbDouble dm =   mat_camTofocus(2,0)*mat_camTofocus(2,0)
			+ mat_camTofocus(2,1)*mat_camTofocus(2,1)
			+ mat_camTofocus(2,2)*mat_camTofocus(2,2);
		dm = sqrt(dm); 
		mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
	}else{	  
		mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
	}	

	if (mpr_isShake)
	{// 更新位置
		mpr_orignPitch = mpr_pitch;
		mpr_orignyaw = mpr_yaw;
		GetFocusPos(mpr_shakefocusPos);
		mpr_shakeDistance = mpr_distance;
	}
}

//此函数ok
osg::Matrixd CGlbGlobeManipulator::getMatrix() const
{
	osg::Matrixd focusToWorld;
	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(mpr_focusPos.y()),osg::DegreesToRadians(mpr_focusPos.x()),mpr_focusPos.z(),focusToWorld);
// 	//////////////////////////////////////////////////////////////////////////
// 	//验证
// 	osg::Vec3d testvec(0.0,0.0,0.0);
// 	testvec = testvec * focusToWorld;
// 	osg::Vec3d testvec2;
// 	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_focusPos.y()),osg::DegreesToRadians(mpr_focusPos.x()),mpr_focusPos.z(),testvec2.x(),testvec2.y(),testvec2.z());
// 	//////////////////////////////////////////////////////////////////////////
//  //验证
//  osg::Matrixd ma = osg::Matrixd::translate(0.0,0.0,mpr_distance)*
//  	osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(mpr_pitch),1.0,0.0,0.0)*
//  	osg::Matrixd::rotate(osg::DegreesToRadians(-mpr_yaw),0.0,0.0,1.0);
//  osg::Vec3d cam = osg::Vec3(0.0,0.0,0.0) * ma;
// 	g_ellipsmodel->convertXYZToLatLongHeight(cam.x(),cam.y(),cam.z(),cam.y(),cam.x(),cam.z());
// 	cam.x() = osg::RadiansToDegrees(cam.x());
// 	cam.y() = osg::RadiansToDegrees(cam.y());
// 	//////////////////////////////////////////////////////////////////////////
	return osg::Matrixd::translate(0.0,0.0,mpr_distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(mpr_pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-mpr_yaw),0.0,0.0,1.0)*
		focusToWorld;
}

void CGlbGlobeManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
{
	setByMatrix(osg::Matrix::inverse(matrix));
}

osg::Matrixd CGlbGlobeManipulator::getInverseMatrix() const
{
	return osg::Matrixd::inverse(getMatrix());
}

void CGlbGlobeManipulator::SetCameraYaw(glbDouble yaw)
{
	if (mpr_lockmode == GLB_LOCKMODEL_2DN)
		yaw = 0.0;
	glbDouble old_yaw = GetCameraYaw();
	RotateCameraYaw(yaw-old_yaw);
	//更新焦点
	//UpdateFocusElevate(getMatrix());
}

glbDouble CGlbGlobeManipulator::GetCameraYaw()
{
	//由matrix,获取相机经纬度
	osg::Matrixd mat_camera = getMatrix();
	osg::Vec3d cameraPos_w(mat_camera(3,0),mat_camera(3,1),mat_camera(3,2));	//相机位置(地心直角坐标系)
	osg::Matrixd mat_cameraToworld;
	g_ellipsmodel->computeLocalToWorldTransformFromXYZ(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),mat_cameraToworld);
	osg::Matrixd mat = mat_camera * osg::Matrixd::inverse(mat_cameraToworld);
	return osg::RadiansToDegrees(atan2(-mat(0,1),mat(0,0)));
}

void CGlbGlobeManipulator::RotateCameraYaw(glbDouble yaw)
{
	if (abs(yaw) < 0.0001)
	{
		return;
	}  
	osg::Matrixd mat = getMatrix();

	osg::Vec3d cameraPos_w = osg::Vec3(0.0,0.0,0.0)*mat;
	osg::Vec3d cameraPos;
	GetCameraPos(cameraPos);
	DegToRad(cameraPos);
	//相机处
	osg::Vec3d up(cos(cameraPos.x())*cos(cameraPos.y()), sin(cameraPos.x())*cos(cameraPos.y()), sin(cameraPos.y()));

	osg::Matrixd rote = osg::Matrixd::rotate(-osg::DegreesToRadians(yaw),up);
	mat = mat*osg::Matrixd::translate(-cameraPos_w)*rote*osg::Matrixd::translate(cameraPos_w);
	setByMatrix(mat);
	//更新焦点
	//UpdateFocusElevate(mat);
}

void CGlbGlobeManipulator::SetCameraPitch(glbDouble pitch)
{
	if (mpr_lockmode == GLB_LOCKMODE_2D || mpr_lockmode == GLB_LOCKMODEL_2DN)
		pitch = -90.0;
	glbDouble old_pitch = GetCameraPitch();
	RotateCameraPitch(pitch - old_pitch);
	//更新焦点
	//UpdateFocusElevate(getMatrix());
}

glbDouble CGlbGlobeManipulator::GetCameraPitch()
{
	//由matrix,获取相机经纬度
	osg::Matrixd mat_camera = getMatrix();
	osg::Vec3d cameraPos_w(mat_camera(3,0),mat_camera(3,1),mat_camera(3,2));
	osg::Matrixd mat_cameraToworld;
	g_ellipsmodel->computeLocalToWorldTransformFromXYZ(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),mat_cameraToworld);
	osg::Matrixd mat = mat_camera * osg::Matrixd::inverse(mat_cameraToworld);
	if (mat(2,2) > 1.0 || mat(2,2) < -1.0)	//防止mat(2,2)溢出(-1.0,1.0)范围,asin无法计算
	{
		glbDouble dm = mat(2,0)*mat(2,0)+mat(2,1)*mat(2,1)+mat(2,2)*mat(2,2);
		dm = sqrt(dm);
		return osg::RadiansToDegrees(asin(-mat(2,2)/dm));
	}else{	  
		return osg::RadiansToDegrees(asin(-mat(2,2)));
	}
}

void CGlbGlobeManipulator::RotateCameraPitch(glbDouble pitch)
{
	if (abs(pitch) < 0.0001)		   
	{
		return;
	}
	glbDouble old_pitch = GetCameraPitch();
	if (pitch > 0)
	{
		if (old_pitch > 90.00000000000001 - pitch)
			return;
	}else{
		if (old_pitch < -90.00000000000001 - pitch)
			return;
	}
	osg::Matrixd mat = getMatrix();
	osg::Matrixd rote = osg::Matrixd::rotate(osg::DegreesToRadians(pitch),1.0,0.0,0.0);
	mat = rote*mat;
	setByMatrix(mat);
	//UpdateFocusElevate(mat);
}

//此函数细节待调试
void CGlbGlobeManipulator::Zoom(glbDouble delt)
{
	if (abs(mpr_pitch) < 1e-7)
		return;
	{
		osg::Vec3d cameraPos;
		GetCameraPos(cameraPos);
		if (cameraPos.z() > MaxCameraPosZ && delt<0)
			return;
	}
	//先求zoomdelt
	glbDouble zoomdelt = DBL_MAX;
	osg::Vec3d ZoomCenter,ZoomCenter0;
	osg::Matrixd mat = getMatrix();	   
	osg::Vec3d cameraPos_w(mat(3,0),mat(3,1),mat(3,2));
	osg::Vec3d lookvec = osg::Vec3d(0.0,0.0,-mpr_distance) * mat - cameraPos_w;

	osg::Viewport* viewport = mpr_globe->GetView()->GetOsgCamera()->getViewport();
	glbDouble centerx,centery;
	mpr_globe->GetView()->getScreenFocusPt(centerx,centery);
	glbBool isObjfocus = false;
	//焦点
	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_focusPos.y()),osg::DegreesToRadians(mpr_focusPos.x()),mpr_focusPos.z(),ZoomCenter.x(),ZoomCenter.y(),ZoomCenter.z());
	zoomdelt = (ZoomCenter-cameraPos_w).length();
	if (mpr_isDetectCollision)	//开启碰撞检测
	{
		//isObjfocus = mpr_globe->GetView()->Pick(centerx,centery,results,InterPoints);
		isObjfocus = mpr_globe->GetView()->PickNearestObject(centerx,centery,ZoomCenter0);
	}
	if (isObjfocus)
	{
		//ZoomCenter0 = InterPoints[0];
		glbDouble zoomdelt1 = (ZoomCenter0-cameraPos_w).length();
		if (zoomdelt1 < zoomdelt)
		{
			zoomdelt = zoomdelt1;
			ZoomCenter = ZoomCenter0;
		}				
	}

	//glbBool isIntersect = false;//mpr_globe->GetView()->getSightLineInterTerrain(ZoomCenter);
	//glbBool isIntersect = mpr_globe->IsRayInterTerrain(cameraPos_w,lookvec,ZoomCenter);		//视线与地形求交
	//if(isIntersect)
	//{
	//	glbDouble zoomdelt1 = (ZoomCenter-cameraPos_w).length();
	//	if (zoomdelt1 > zoomdelt)	//先与对象相交
	//		ZoomCenter = ZoomCenter0;
	//	else
	//		zoomdelt = zoomdelt1;
	//}else{
	//	ZoomCenter = ZoomCenter0;
	//}

	glbBool isuIntersect = mpr_globe->IsRayInterTerrain(cameraPos_w,lookvec,ZoomCenter0);	//视线与地形求交
	if (isuIntersect)
	{
		glbDouble uzoomdelt = (ZoomCenter0 - cameraPos_w).length();
		if (uzoomdelt < zoomdelt)
		{
			zoomdelt = uzoomdelt;
			ZoomCenter = ZoomCenter0;
		}
		//zoomdelt = zoomdelt < uzoomdelt ? zoomdelt : -uzoomdelt;
	}
	else
	{// 与地面没有交点时,需要计算是否可以会和物体有交点【或者地面隐藏了，或者在地形范围外】
		//mpr_globe->GetView()->getScreenFocusPt(centerx,centery);
		//osg::Vec3d InterPoint;
		//glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(centerx,centery,InterPoint);
		//if (isObjfocus)
		//{
		//	glbDouble zoomdelt1 = (ZoomCenter0-cameraPos_w).length();
		//	if (zoomdelt1 < zoomdelt)
		//	{
		//		zoomdelt = zoomdelt1;
		//		ZoomCenter = ZoomCenter0;
		//	}				
		//}
		//else
		//{
		//	zoomdelt = 100.0;
		//}
	}

	//if (!(isuIntersect || isIntersect || (isObjfocus && InterPoints.size())))
	//	zoomdelt = mpr_distance;
	if (zoomdelt >= MaxCameraPosZ && delt < 0)
		return;
	
	//根据zoomdelt设置ZoomSpeed
	//glbDouble ZoomSpeed;
	//if (zoomdelt > 100000)		//100km
	//	ZoomSpeed = 0.3;
	//else if (zoomdelt > 10000)	//10km	
	//	ZoomSpeed = 0.2;
	//else						//10km以下

	glbDouble ZoomSpeed = 0.1;
	if (mpr_isctrldown)
		ZoomSpeed = 0.05;
	else
		ZoomSpeed = 0.1;
	ZoomSpeed *= abs(delt);

	if (mpr_globe->IsUnderGroundMode())	//开启了地下模式
	{	
		glbDouble zoom_break = MinElevateThreshold;
		if (abs(mpr_pitch) > 1.0)
			zoom_break /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
		else if (abs(mpr_pitch) > 0.1)
			zoom_break *= 60;
		else 
			zoom_break *= 600;
		if (abs(zoomdelt) > zoom_break)
		{
			if (delt > 0)	//前滚
			{
				zoomdelt = -abs(zoomdelt) * ZoomSpeed;
			}else{
				zoomdelt = abs(zoomdelt) * ZoomSpeed;
			}
			//相机沿lookvec方向移动zoomdelt距离
			setByMatrix(osg::Matrixd::translate(0.0,0.0,zoomdelt) * mat);
		}else{	//突变
			if (delt > 0)	//前滚
				zoomdelt = - (zoomdelt * 2.4);
			else
				zoomdelt = zoomdelt * 2.4;
			//相机沿lookvec方向移动zoomdelt距离
			setByMatrix2(osg::Matrixd::translate(0.0,0.0,zoomdelt) * mat);
		}
	}else	//未开启地下模式
	{
		if (delt > 0)	//前滚
		{
			zoomdelt = -abs(zoomdelt) * ZoomSpeed;
		}else{
			zoomdelt = abs(zoomdelt) * ZoomSpeed;
		}
		//相机沿lookvec方向移动zoomdelt距离
		setByMatrix(osg::Matrixd::translate(0.0,0.0,zoomdelt) * mat);
	}  
	//相机纠偏
	osg::Vec3d camPos;
	GetCameraPos(camPos);
	if (mpr_globe->IsUnderGroundMode())
	{
		if (camPos.z() - mpr_globe->GetUnderGroundAltitude() < ElevateThreshold)
		{
			zoomdelt = camPos.z() - mpr_globe->GetUnderGroundAltitude() - ElevateThreshold;
			zoomdelt /= sin(osg::DegreesToRadians(mpr_pitch));
			setByMatrix(osg::Matrixd::translate(0.0,0.0,zoomdelt) * getMatrix());
		}
	}else{
		if (camPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() < ElevateThreshold)
		{
			zoomdelt = camPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() - ElevateThreshold;
			zoomdelt /= sin(osg::DegreesToRadians(mpr_pitch));
			setByMatrix(osg::Matrixd::translate(0.0,0.0,zoomdelt) * getMatrix());
		}
	}
	//UpdateFocusElevate(getMatrix());
	//相机纠偏
	//UpdateCameraElevate();
// 	//////////////////////////////////////////////////////////////////////////
// 	osg::Vec3d cameraPos;
// 	GetCameraPos(cameraPos);
// 	if (cameraPos.z() > MaxCameraPosZ && delt<0)
// 	{
// 		return;
// 	}
// 	//先求zoomdelt
// 	glbDouble zoomdelt = 0.0;
// 	if (abs(mpr_pitch) < 3.0)
// 	{
// 		zoomdelt =  (cameraPos.z()-mpr_globe->GetView()->GetTerrainElevationOfCameraPos()) * 20;
// 	}
// 	zoomdelt = (cameraPos.z()-mpr_globe->GetView()->GetTerrainElevationOfCameraPos())/sin(osg::DegreesToRadians(-mpr_pitch));
// 
// 	if (mpr_globe->IsUnderGroundMode())	//开启了地下模式
// 	{
// 		if (abs(cameraPos.z()-mpr_globe->GetView()->GetTerrainElevationOfCameraPos()) > 0.2)
// 		{
// 			if (delt > 0)	//前滚
// 			{
// 				zoomdelt = -abs(zoomdelt) * ZoomSpeed;
// 			}else{
// 				zoomdelt = abs(zoomdelt) * ZoomSpeed;
// 			}
// 		}else{	//突变
// 			zoomdelt = - (zoomdelt *2.2);
// 		}
// 	}else	//未开启地下模式
// 	{
// 		if (delt > 0)	//前滚
// 		{
// 			zoomdelt = -abs(zoomdelt) * ZoomSpeed;
// 		}else{
// 			zoomdelt = abs(zoomdelt) * ZoomSpeed;
// 		}
// 	}
// 
// 	mpr_distance += zoomdelt;
// 	//相机纠偏
// 	UpdateCameraElevate();
// 	//如果穿越了地形，需重新计算mpr_focusPos
// 	isCross = 0;
// 	if (isCross)
// 	{
// 		glbDouble focusx,focusy,focusz;
// 		double centerx,centery; 
// 		//获取osgviewer中心的屏幕坐标
// 		mpr_globe->GetView()->getScreenFocusPt(centerx,centery);
// 		glbBool isfocus = mpr_globe->ScreenToTerrainCoordinate(centerx,centery,focusx,focusy,focusz); //~~
// 		if (isfocus)
// 		{
// 			mpr_focusPos = osg::Vec3d(focusx,focusy,focusz);
// 		}else{
// 		//计算相机与地下参考面的焦点focus
// 		//if (存在)
// 		//	mpr_focusPos = focus;
// 		//else
// 			//维护一个虚焦点
// 			//焦点与相机偏移同一个向量,直角坐标系下
// 			osg::Vec3d cameraPos_new;
// 			GetCameraPos(cameraPos_new);
// 			DegToRad(&cameraPos);
// 			DegToRad(&cameraPos_new);
// 			osg::Vec3d cameraPos_w;
// 			osg::Vec3d cameraPos_new_w;
// 			g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),cameraPos.z(),cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z());
// 			g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos_new.y(),cameraPos_new.x(),cameraPos_new.z(),cameraPos_new_w.x(),cameraPos_new_w.y(),cameraPos_new_w.z());
// 			osg::Vec3d trans = cameraPos_new_w - cameraPos_w;
// 			osg::Vec3d focusPos_w;
// 			DegToRad(&mpr_focusPos);
// 			g_ellipsmodel->convertLatLongHeightToXYZ(mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z(),focusPos_w.x(),focusPos_w.y(),focusPos_w.z());
// 			focusPos_w += trans;
// 			g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z());
// 			RadToDeg(&mpr_focusPos);
// 		}
// 	}	
}

//此函数ok
void CGlbGlobeManipulator::Rotate(glbDouble lonOrX,glbDouble latOrY,glbDouble altOrZ,glbDouble pitch,glbDouble yaw)
{ 
	switch (mpr_lockmode)
	{ 
	case GLB_LOCKMODE_2D:
		{
			pitch = 0.0;
			break;
		} 
	case GLB_LOCKMODEL_2DN:	 
			return;
	default:
		break;
	}
	if (mpr_pitch + pitch < -90.0)
	{
		pitch = -90.0 - mpr_pitch;
	}else if (mpr_pitch + pitch > 90.0)
	{
		pitch = 90.0 - mpr_pitch;
	}
	//先转pitch,再转yaw
	osg::Matrixd mat = getMatrix();
	//选中点从本地坐标到世界坐标的变换矩阵
	osg::Matrixd mat_positionToworld;
	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(latOrY),osg::DegreesToRadians(lonOrX),altOrZ,mat_positionToworld);

	osg::Vec3d rotate_yaw = osg::Vec3d(0.0,0.0,1.0);
	osg::Vec3d rotate_ptch = osg::Vec3d(1.0,0.0,0.0) * osg::Matrixd::rotate(osg::DegreesToRadians(-mpr_yaw),0.0,0.0,1.0);

	//选中点地理坐标系下，旋转矩阵
	osg::Matrixd mamat_rotate = osg::Matrixd::rotate(osg::DegreesToRadians(pitch),rotate_ptch) * osg::Matrixd::rotate(osg::DegreesToRadians(yaw),rotate_yaw);
	// 先乘变换矩阵mat_positionToworld的逆矩阵变换到局部坐标系，然后旋转mamat_rotate，再乘变换矩阵mat_positionToworld转到世界坐标
	mat = mat * osg::Matrixd::inverse(mat_positionToworld) * mamat_rotate * mat_positionToworld;
	//判断相机距地面高度，是否需要纠偏
	osg::Vec3d cameraPos_w = mat.getTrans();
	osg::Vec3d cameraPos;
	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	glbDouble elevation = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevation = mpr_globe->GetElevationAt(osg::RadiansToDegrees(cameraPos.x()),osg::RadiansToDegrees(cameraPos.y()));
	else
		elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
	if (!mpr_globe->IsUnderGroundMode()) //未开启地下模式
	{
		glbDouble alt = mpr_globe->GetUnderGroundAltitude();
		if (elevation < alt)
			return;
		if (cameraPos.z() - elevation < ElevateThreshold)
		{			
			return;
			//pitch = 0.0;
			//osg::Matrixd mamat_rotate = osg::Matrixd::rotate(osg::DegreesToRadians(pitch),rotate_ptch) * osg::Matrixd::rotate(osg::DegreesToRadians(yaw),rotate_yaw);
			//mat = mat * osg::Matrixd::inverse(mat_positionToworld) * mamat_rotate * mat_positionToworld;
			//setByMatrix(mat);
		}
	}else{	//开启地下模式
		if (cameraPos.z() - mpr_globe->GetUnderGroundAltitude() < ElevateThreshold)
		{
			return;
			//pitch = 0.0;
			//osg::Matrixd mamat_rotate = osg::Matrixd::rotate(osg::DegreesToRadians(pitch),rotate_ptch) * osg::Matrixd::rotate(osg::DegreesToRadians(yaw),rotate_yaw);
			//mat = mat * osg::Matrixd::inverse(mat_positionToworld) * mamat_rotate * mat_positionToworld;
			//setByMatrix(mat);
		}else if (abs(cameraPos.z() - elevation) < MinElevateThreshold)
		{
			osg::Vec3d newcameraPos = cameraPos;
			if (cameraPos.z() - elevation > 0)
				newcameraPos.z() = MinElevateThreshold+elevation;
			else
				newcameraPos.z() = -MinElevateThreshold+elevation;
			osg::Matrixd mat = getMatrix();
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPos.x(),cameraPos.y(),cameraPos.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(newcameraPos.y()),osg::DegreesToRadians(newcameraPos.x()),newcameraPos.z(),newcameraPos.x(),newcameraPos.y(),newcameraPos.z());
			mat.postMultTranslate(newcameraPos - cameraPos);
		}
	}
	if (mat.isNaN())
	{// 存在这种情况
		return;
	}
	setByMatrix2(mat);
}

void CGlbGlobeManipulator::Drag(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2)
{
	// 如果没有抓住任何对象或地面，不允许拖拽
	if (mpr_isPushPicked==false)
		return;

	if (mpr_isshiftdown)	//Shift键按下
	{
		if (abs(mpr_pitch) < 60)	//竖直面平移
		{
			//逻辑：相机与鼠标的射线与 _push点所在竖直平面求交
			osg::Vec3d cameraPos_w = _pushMatrix.getTrans();
			osg::Vec3d pushPos;
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_pushPos.y()),osg::DegreesToRadians(_pushPos.x()),_pushPos.z(),pushPos.x(),pushPos.y(),pushPos.z());
			osg::Vec3d up(cos(osg::DegreesToRadians(_pushPos.x()))*cos(osg::DegreesToRadians(_pushPos.y())), sin(osg::DegreesToRadians(_pushPos.x()))*cos(osg::DegreesToRadians(_pushPos.y())), sin(osg::DegreesToRadians(_pushPos.y())));
			osg::Vec3d cameraTopushPos = pushPos - cameraPos_w;
			//osg::Vec3d nomalvec =  pushPos - cameraPos_w;
			osg::Vec3d nomalvec = (cameraTopushPos^up)^up;
			nomalvec.normalize();
			osg::Matrixd mat = _pushMatrix;	
			osg::Matrixd inverseVPW;			
			inverseVPW.invert(_pushVPW);
			osg::Vec3d startPos_h(ptx2,pty2,0.0);
			osg::Vec3d endPos_h(ptx2,pty2,1.0);
			startPos_h = startPos_h * inverseVPW;
			endPos_h = endPos_h * inverseVPW;
			std::vector<osg::Vec3d> IntersectPoses;
			glbInt32 IntersectResult = IntersectRayPlane(startPos_h,endPos_h,pushPos,nomalvec,IntersectPoses);
			if (IntersectResult)
			{
				osg::Vec3d IntersectPos = IntersectPoses[0];	//世界坐标
				osg::Vec3d delt = (pushPos-IntersectPos);
				mat.postMultTranslate(delt);
				//GlbLogWOutput(GLB_LOGTYPE_INFO,L"delt:(%.5f,%.5f,%.5f) \r\n",delt.x(),delt.y(),delt.z());
			}
			setByMatrix(mat);
			osg::Vec3d new_cameraPos;
			GetCameraPos(new_cameraPos);
			glbDouble elevation = 0.0;
			if (mpr_globe->GetView()->getUpdateCameraMode())
				elevation = mpr_globe->GetElevationAt(new_cameraPos.x(),new_cameraPos.y());
			else
				elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			UpdateCameraElevate(elevation);
			return;
		}
	}
	if(abs(mpr_pitch) < 30.0)	//大俯仰角情况下不会出现 相机与按下点与地平线夹角很小的情况
	{
		//先求相机与按下点与地平线夹角
		osg::Vec3d pushcamersPos = _pushMatrix.getTrans();
		glbDouble cameralon,cameralat,cameraalt;
		g_ellipsmodel->convertXYZToLatLongHeight(pushcamersPos.x(),pushcamersPos.y(),pushcamersPos.z(),cameralat,cameralon,cameraalt);
		osg::Matrixd pushptToworld;
		g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(_pushPos.y()),osg::DegreesToRadians(_pushPos.x()),_pushPos.z(),pushptToworld);
		osg::Vec3d pushcamersPos_atpushPos = pushcamersPos * osg::Matrixd::inverse(pushptToworld);
		pushcamersPos_atpushPos.normalize();
		if (pushcamersPos_atpushPos.z() > 1.0)
			pushcamersPos_atpushPos.z() = 1.0;
		else if (pushcamersPos_atpushPos.z() < -1.0)
			pushcamersPos_atpushPos.z() = -1.0;
		glbDouble pushpitch = asin(-pushcamersPos_atpushPos.z());
		pushpitch = osg::RadiansToDegrees(pushpitch);
		if (abs(pushpitch) < 15.0)	//小角度特殊处理
		{
			//逻辑：相机与鼠标的射线与 _push点所在水平平面求交
			osg::Vec3d cameraPos_w = _pushMatrix.getTrans();
			osg::Vec3d pushPos;
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_pushPos.y()),osg::DegreesToRadians(_pushPos.x()),_pushPos.z(),pushPos.x(),pushPos.y(),pushPos.z());
			osg::Vec3d up(cos(osg::DegreesToRadians(_pushPos.x()))*cos(osg::DegreesToRadians(_pushPos.y())), sin(osg::DegreesToRadians(_pushPos.x()))*cos(osg::DegreesToRadians(_pushPos.y())), sin(osg::DegreesToRadians(_pushPos.y())));
			osg::Vec3d east(-sin(osg::DegreesToRadians(_pushPos.x())), cos(osg::DegreesToRadians(_pushPos.x())), 0);	//east
			osg::Vec3d eastEx = east*osg::Matrix::rotate(osg::DegreesToRadians(-mpr_yaw),up);
			osg::Vec3d nomalvec = up*osg::Matrix::rotate(osg::DegreesToRadians(30.0-abs(pushpitch)),eastEx);
			if (pushpitch > 0 && (cameraalt - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() < 0))	//在地下时，drag中地面
				nomalvec = -up*osg::Matrix::rotate(osg::DegreesToRadians(-20.0+pushpitch),eastEx);				
			osg::Vec3d rotate_vec = east*osg::Matrix::rotate(osg::DegreesToRadians(mpr_yaw),up);
			nomalvec.normalize();
			osg::Matrixd mat = _pushMatrix;	
			osg::Matrixd inverseVPW;			
			inverseVPW.invert(_pushVPW);

			//osg::Vec3d startPos_v(ptx1,pty2,0.0);	//竖直
			//osg::Vec3d endPos_v(ptx1,pty2,1.0);		
			//startPos_v = startPos_v * inverseVPW;
			//endPos_v = endPos_v * inverseVPW;
			//std::vector<osg::Vec3d> IntersectPoses2;
			//glbInt32 IntersectResult = IntersectRayPlane(startPos_v,endPos_v,pushPos,nomalvec,IntersectPoses2);
			//glbDouble distan;
			//if (IntersectResult)
			//{
			//	osg::Vec3d IntersectPos = IntersectPoses2[0];	//世界坐标
			//	osg::Vec3d delt = (pushPos-IntersectPos);
			//	distan = delt.length();
			//	if (pty2 > pty1)
			//	{
			//		//tranVec = nomalvec*distan*(-20.0);	
			//		//mat.postMultTranslate(tranVec);
			//		mat.preMultTranslate(osg::Vec3d(0.0,0.0,10.0*distan));
			//	}else{
			//		//tranVec = nomalvec*distan*2.0;
			//		//mat.postMultTranslate(tranVec);
			//		mat.preMultTranslate(osg::Vec3d(0.0,0.0,-distan));
			//	}				
			//}
			//新增2014.12.10，解决小角度下拖拽中心点上下不一致的问题
			glbDouble centerx,centery;
			mpr_globe->GetView()->getScreenFocusPt(centerx,centery);
			if (mpr_isUnderGround)
			{
				if (mpr_pitch > 0.0 && pty1 > centery)
				{
					pty2 = 2*pty1 - pty2;
				}else if (mpr_pitch < 0.0 && pty1 < centery)
				{
					pty2 = 2*pty1 - pty2;
				}				
			}

			osg::Vec3d startPos_h(ptx2,pty2,0.0);
			osg::Vec3d endPos_h(ptx2,pty2,1.0);
			startPos_h = startPos_h * inverseVPW;
			endPos_h = endPos_h * inverseVPW;
			std::vector<osg::Vec3d> IntersectPoses;
			//pushPos = pushPos * osg::Matrixd::translate(tranVec);
			glbInt32 IntersectResult = IntersectRayPlane(startPos_h,endPos_h,pushPos,nomalvec,IntersectPoses);
			if (IntersectResult)
			{
				osg::Vec3d IntersectPos = IntersectPoses[0];	//世界坐标
				osg::Vec3d delt = (pushPos-IntersectPos);
				//delt = delt * osg::Matrix::rotate(osg::PI_2,rotate_vec);
				//if (pty2 > pty1)
				//	mat.postMultTranslate(delt * sqrt(distan/10));
				//else if (pty2 < pty1)
				//	mat.postMultTranslate(delt / sqrt(distan/10));
				//else
					mat.postMultTranslate(delt);
			}	
			osg::Vec3d old_cameraPos_w = mat.getTrans();
			osg::Vec3d new_cameraPos,new_cameraPos_w;

			g_ellipsmodel->convertXYZToLatLongHeight(old_cameraPos_w.x(),old_cameraPos_w.y(),old_cameraPos_w.z(),new_cameraPos.y(),new_cameraPos.x(),new_cameraPos.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(new_cameraPos.y(),new_cameraPos.x(),cameraalt,new_cameraPos_w.x(),new_cameraPos_w.y(),new_cameraPos_w.z());
			mat.postMultTranslate(new_cameraPos_w-old_cameraPos_w);
			setByMatrix(mat);
			glbDouble elevation = 0.0;
			if (mpr_globe->GetView()->getUpdateCameraMode())
				elevation = mpr_globe->GetElevationAt(osg::RadiansToDegrees(new_cameraPos.x()),osg::RadiansToDegrees(new_cameraPos.y()));
			else
				elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			UpdateCameraElevate(elevation);
			return;
		}
	}
	osg::Matrixd oldmatrix = getMatrix(); 
	osg::Vec3d oldcameraPos_w = oldmatrix.getTrans();
	
	osg::Vec3d startPos(ptx2,pty2,0.0);
	osg::Vec3d endPos(ptx2,pty2,1.0);
	osg::Matrixd inverseVPW;
	inverseVPW.invert(_pushVPW);
	startPos = startPos * inverseVPW;
	endPos = endPos * inverseVPW;
	osg::Vec3d center(0.0,0.0,0.0);	//地球球心世界坐标	
	osg::Vec3d pushPos;
	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_pushPos.y()),osg::DegreesToRadians(_pushPos.x()),_pushPos.z(),pushPos.x(),pushPos.y(),pushPos.z());
	glbDouble radius = pushPos.length();	//球半径
	std::vector<osg::Vec3d> IntersectPoses;
	//线段与球求交
	glbInt32 IntersectResult = IntersectRaySphere(startPos,endPos,center,radius,IntersectPoses);

	if (IntersectResult)	//求交成功，有交点
	{
		osg::Vec3d IntersectPos = IntersectPoses[0];	//世界坐标
		
		osg::Vec3d oldcameraPos;
		g_ellipsmodel->convertXYZToLatLongHeight(oldcameraPos_w.x(),oldcameraPos_w.y(),oldcameraPos_w.z(),oldcameraPos.x(),oldcameraPos.y(),oldcameraPos.z());
		if (abs(_pushPos.z() - oldcameraPos.z()) < 500)
		{
			//g_ellipsmodel->convertLatLongHeightToXYZ(IntersectPos.y(),IntersectPos.x(),IntersectPos.z(),IntersectPos.x(),IntersectPos.y(),IntersectPos.z());
			osg::Vec3d delt = (pushPos-IntersectPos); //* sin(osg::DegreesToRadians(abs(mpr_pitch)));
			setByMatrix(_pushMatrix * osg::Matrixd::translate(delt));
		}else{
			//DegToRad(IntersectPos);
			g_ellipsmodel->convertXYZToLatLongHeight(IntersectPos.x(),IntersectPos.y(),IntersectPos.z(),IntersectPos.y(),IntersectPos.x(),IntersectPos.z());
			glbDouble angle = computeGlobeAngle(_pushPos.y(),_pushPos.x(),osg::RadiansToDegrees(IntersectPos.y()),osg::RadiansToDegrees(IntersectPos.x()));
			//angle *= sin(osg::DegreesToRadians(abs(mpr_pitch)));
			osg::Vec3d pt0(osg::DegreesToRadians(_pushPos.x()),osg::DegreesToRadians(_pushPos.y()),_pushPos.z());
			osg::Vec3d pt1up(cos(pt0.x())*cos(pt0.y()), sin(pt0.x())*cos(pt0.y()), sin(pt0.y()));
			osg::Vec3d pt2up(cos(IntersectPos.x())*cos(IntersectPos.y()), sin(IntersectPos.x())*cos(IntersectPos.y()), sin(IntersectPos.y())); 
			osg::Vec3d rotate_vec = pt1up^pt2up;
			setByMatrix(_pushMatrix * osg::Matrixd::rotate(-angle,rotate_vec));
		}
	}else{
		setByMatrix(oldmatrix);
	}
	osg::Vec3d new_cameraPos;
	GetCameraPos(new_cameraPos);
	glbDouble elevation = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevation = mpr_globe->GetElevationAt(new_cameraPos.x(),new_cameraPos.y());
	else
		elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
	UpdateCameraElevate(elevation);
	if (mpr_lockmode == GLB_LOCKMODE_2D)
	{
		SetCameraPitch(-90.0);
	}
	if (mpr_lockmode == GLB_LOCKMODEL_2DN)
	{
		SetCameraPitch(-90.0);
		SetCameraYaw(0.0);
	}
}

//void CGlbGlobeManipulator::Drag(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2)
//{
//	osg::Matrixd oldmatrix = getMatrix(); 
//	osg::Vec3d oldcameraPos_w = oldmatrix.getTrans();
//	//求交
//	std::vector<glbref_ptr<CGlbGlobeObject>> results;
//	std::vector<osg::Vec3d> InterPoints;
//	glbDouble ptx1_s = ptx1,pty1_s = pty1;
//	mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
//	glbBool isObjfocus = mpr_globe->GetView()->Pick(ptx1_s,pty1_s,results,InterPoints);
//	osg::Vec3d pt11_w;
//	if (isObjfocus)
//	{
//		CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(results[0].get());
//		if (mpr_globe->IsUnderGroundMode())
//			pt11_w = InterPoints[0];
//		else
//			if (robj && robj->IsGround())
//				pt11_w = InterPoints[0];		
//	}
//	osg::Vec3d pt1;
//	if (mpr_globe->IsUnderGroundMode()) //开启地下模式
//	{
//		glbBool isfocus = mpr_globe->ScreenToTerrainCoordinate(ptx1,pty1,pt1.x(),pt1.y(),pt1.z());
//		if (!isfocus)
//		{
//			glbBool isugfocus = mpr_globe->ScreenToUGPlaneCoordinate(ptx1,pty1,pt1.x(),pt1.y(),pt1.z());
//			if (!isugfocus && !isObjfocus)
//				return;
//		}
//	}else{ 
//		glbBool isfocus = mpr_globe->ScreenToTerrainCoordinate(ptx1,pty1,pt1.x(),pt1.y(),pt1.z());
//	 	if (!isfocus && !isObjfocus)
//	 		return;
//	}
//	if (isObjfocus)
//	{
//		osg::Vec3d pt1_w;
//		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt1.y()),osg::DegreesToRadians(pt1.x()),pt1.z(),pt1_w.x(),pt1_w.y(),pt1_w.z());
//		glbDouble dis1 = (oldcameraPos_w - pt1_w).length();
//		glbDouble dis2 = (oldcameraPos_w - pt11_w).length();
//		if (dis2 < dis1)
//		{
//			g_ellipsmodel->convertXYZToLatLongHeight(pt11_w.x(),pt11_w.y(),pt11_w.z(),pt1.y(),pt1.x(),pt1.z());
//			RadToDeg(pt1);
//		}
//	}
//	osg::Vec3d _Pos = pt1;
//	
//
//	//setByMatrix(_pushMatrix);	//相机 回到鼠标按下的状态
//	osg::Vec3d startPos(ptx2,pty2,0.0);
//	osg::Vec3d endPos(ptx2,pty2,1.0);
//	mpr_globe->GetView()->ScreenToWorld(startPos.x(),startPos.y(),startPos.z());
//	mpr_globe->GetView()->ScreenToWorld(endPos.x(),endPos.y(),endPos.z());
//	osg::Vec3d center(0.0,0.0,0.0);	//地球球心世界坐标
//	//glbDouble radius = (osg::WGS_84_RADIUS_EQUATOR+osg::WGS_84_RADIUS_POLAR)*0.5+_pushPos.z();	//球半径
//	osg::Vec3d pushPos;
//	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_Pos.y()),osg::DegreesToRadians(_Pos.x()),_Pos.z(),pushPos.x(),pushPos.y(),pushPos.z());
//	glbDouble radius = pushPos.length();
//	std::vector<osg::Vec3d> IntersectPoses;
//	//线段与球求交
//	glbInt32 IntersectResult = IntersectRaySphere(startPos,endPos,center,radius,IntersectPoses);
//	//glbBool IntersectResult;
//	//osg::Vec3d IntersectPos;
//	//if (mpr_globe->IsUnderGroundMode()) //开启地下模式
//	//{
//	//	IntersectResult = mpr_globe->ScreenToTerrainCoordinate(ptx2,pty2,IntersectPos.x(),IntersectPos.y(),IntersectPos.z()); 
//	//	if (!IntersectResult)
//	//	{
//	//		IntersectResult = mpr_globe->ScreenToUGPlaneCoordinate(ptx2,pty2,IntersectPos.x(),IntersectPos.y(),IntersectPos.z());
//	//		if (!IntersectResult)
//	//			return;
//	//	}
//	//}else{ 
//	//	IntersectResult = mpr_globe->ScreenToTerrainCoordinate(ptx2,pty2,IntersectPos.x(),IntersectPos.y(),IntersectPos.z());
//	//	if (!IntersectResult)
//	//		return;
//	//}
//
//	if (IntersectResult)	//求交成功，有交点
//	{
//		osg::Vec3d IntersectPos = IntersectPoses[0];
//		//第一个交点
//		g_ellipsmodel->convertXYZToLatLongHeight(IntersectPos.x(),IntersectPos.y(),IntersectPos.z(),IntersectPos.y(),IntersectPos.x(),IntersectPos.z());
//		osg::Vec3d oldcameraPos;
//		g_ellipsmodel->convertXYZToLatLongHeight(oldcameraPos_w.x(),oldcameraPos_w.y(),oldcameraPos_w.z(),oldcameraPos.x(),oldcameraPos.y(),oldcameraPos.z());
//		if (abs(_Pos.z() - oldcameraPos.z()) < 500)
//		{
//			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_Pos.y()),osg::DegreesToRadians(_Pos.x()),_Pos.z(),_Pos.x(),_Pos.y(),_Pos.z());
//			g_ellipsmodel->convertLatLongHeightToXYZ(IntersectPos.y(),IntersectPos.x(),IntersectPos.z(),IntersectPos.x(),IntersectPos.y(),IntersectPos.z());		
//			
//			setByMatrix(oldmatrix * osg::Matrixd::translate(_Pos-IntersectPos));
//			//mpr_focusPos -= IntersectPos-_Pos;		
//			////获取变化后相机高度
//			//osg::Matrixd mat = getMatrix();
//			//osg::Vec3d cameraPos_w = mat.getTrans();
//			//osg::Vec3d cameraPos;
//			//osg::Vec3d newcameraPos_w;
//			//g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
//			//g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),oldcameraPos.z(),newcameraPos_w.x(),newcameraPos_w.y(),newcameraPos_w.z());
//			//mat.postMultTranslate(newcameraPos_w - cameraPos_w);
//			//setByMatrix(mat);
//		}else{
//			//DegToRad(IntersectPos);
//			glbDouble angle = computeGlobeAngle(_Pos.y(),_Pos.x(),osg::RadiansToDegrees(IntersectPos.y()),osg::RadiansToDegrees(IntersectPos.x()));
//			osg::Vec3d pt0(osg::DegreesToRadians(_Pos.x()),osg::DegreesToRadians(_Pos.y()),_pushPos.z());
//			osg::Vec3d pt1up(cos(pt0.x())*cos(pt0.y()), sin(pt0.x())*cos(pt0.y()), sin(pt0.y()));
//			osg::Vec3d pt2up(cos(IntersectPos.x())*cos(IntersectPos.y()), sin(IntersectPos.x())*cos(IntersectPos.y()), sin(IntersectPos.y())); 
//			osg::Vec3d rotate_vec = pt1up^pt2up;
//			setByMatrix(oldmatrix * osg::Matrixd::rotate(-angle,rotate_vec));
//		}
//	}else{
//		setByMatrix(oldmatrix);
//	}
//	if (mpr_lockmode == GLB_LOCKMODE_2D)
//	{
//		SetCameraPitch(-90.0);
//	}
//	if (mpr_lockmode == GLB_LOCKMODEL_2DN)
//	{
//		SetCameraPitch(-90.0);
//		SetCameraYaw(0.0);
//	}
//}
//此函数ok
//void CGlbGlobeManipulator::Drag(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2)
//{
//	//获取原来相机高度
//	osg::Vec3d oldcameraPos,oldcameraPos_w;
//	oldcameraPos_w = osg::Vec3d(0.0,0.0,0.0) * getMatrix();
//	g_ellipsmodel->convertXYZToLatLongHeight(oldcameraPos_w.x(),oldcameraPos_w.y(),oldcameraPos_w.z(),oldcameraPos.y(),oldcameraPos.x(),oldcameraPos.z());
//	RadToDeg(oldcameraPos);
//	//焦点相机同步移动
// 	osg::Vec3d pt1,pt2,pt1_w,pt2_w,pt11_w,pt22_w;
//	glbBool isfocus,isugfocus;
//	std::vector<glbref_ptr<CGlbGlobeObject>> results,results2;
//	std::vector<osg::Vec3d> InterPoints,InterPoints2;
//	glbDouble ptx1_s = ptx1,pty1_s = pty1,ptx2_s = ptx2,pty2_s = pty2;
//	mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
//	mpr_globe->GetView()->ScreenToWindow(ptx2_s,pty2_s);
//	glbBool isObjfocus = mpr_globe->GetView()->Pick(ptx1_s,pty1_s,results,InterPoints);
//	if (isObjfocus)
//	{
//		CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(results[0].get());
//		if (mpr_globe->IsUnderGroundMode())
//			pt11_w = InterPoints[0];
//		else
//			if (robj && robj->IsGround())
//				pt11_w = InterPoints[0];		
//	}
//	glbBool isObjfocus2 = mpr_globe->GetView()->Pick(ptx2_s,pty2_s,results2,InterPoints2);
//	if (isObjfocus2)
//	{
//		CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(results2[0].get());
//		if (mpr_globe->IsUnderGroundMode())
//			pt22_w = InterPoints2[0];
//		else
//			if (robj && robj->IsGround())
//				pt22_w = InterPoints2[0];
//	}
//	if (mpr_globe->IsUnderGroundMode()) //开启地下模式
//	{
//		isfocus = mpr_globe->ScreenToTerrainCoordinate(ptx1,pty1,pt1.x(),pt1.y(),pt1.z());
//		if (!isfocus)
//		{
//			isugfocus = mpr_globe->ScreenToUGPlaneCoordinate(ptx1,pty1,pt1.x(),pt1.y(),pt1.z());
//			if (!isugfocus && !isObjfocus)
//				return;
//			else{
//				isugfocus = mpr_globe->ScreenToUGPlaneCoordinate(ptx2,pty2,pt2.x(),pt2.y(),pt2.z());
//				if (!isugfocus && !isObjfocus2)
//					return;
//			}
//		}else{
//			isfocus = mpr_globe->ScreenToTerrainCoordinate(ptx2,pty2,pt2.x(),pt2.y(),pt2.z()); 
//			if (!isfocus && !isObjfocus2)
//				return;
//		} 
//	}else{ 
//		isfocus = mpr_globe->ScreenToTerrainCoordinate(ptx1,pty1,pt1.x(),pt1.y(),pt1.z());
// 		if (!isfocus && !isObjfocus)
// 			return;
// 		isfocus = mpr_globe->ScreenToTerrainCoordinate(ptx2,pty2,pt2.x(),pt2.y(),pt2.z());
// 		if (!isfocus && !isObjfocus2)
// 			return;
//	}
//	glbInt32 flag = 0;
//	osg::Vec3d pt1temp = pt1;
//	if (isObjfocus)
//	{
//		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt1.y()),osg::DegreesToRadians(pt1.x()),pt1.z(),pt1_w.x(),pt1_w.y(),pt1_w.z());
//		glbDouble dis1 = (oldcameraPos_w - pt1_w).length();
//		glbDouble dis2 = (oldcameraPos_w - pt11_w).length();
//		if (dis2 < dis1)
//		{
//			g_ellipsmodel->convertXYZToLatLongHeight(pt11_w.x(),pt11_w.y(),pt11_w.z(),pt1.y(),pt1.x(),pt1.z());
//			RadToDeg(pt1);
//			flag |= 0x1;
//		}
//	}
//	if (isObjfocus2)
//	{
//		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt2.y()),osg::DegreesToRadians(pt2.x()),pt2.z(),pt2_w.x(),pt2_w.y(),pt2_w.z());
//		glbDouble dis1 = (oldcameraPos_w - pt2_w).length();
//		glbDouble dis2 = (oldcameraPos_w - pt22_w).length();
//		if (dis2 < dis1)
//		{
//			g_ellipsmodel->convertXYZToLatLongHeight(pt22_w.x(),pt22_w.y(),pt22_w.z(),pt2.y(),pt2.x(),pt2.z());
//			RadToDeg(pt2);
//			flag |= 0x2;
//		}
//	}
//
//	GlbLogWOutput(GLB_LOGTYPE_INFO,L"第1点:(%.3f,%.3f,%.3f),第2点:(%.3f,%.3f,%.3f),%d \r\n",pt1.x(),pt1.y(),pt1.z(),pt1temp.x(),pt1temp.y(),pt1temp.z(),flag);
//
// 	//焦点由pt1->pt2,经度增加了pt2.x()-pt1.x(),纬度增加了pt2.y()-pt1.y(),那么，相机转动为：
//	if (abs(oldcameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos()) > 1000.0)	//相机距地面1000米以上
//	{
// 		osg::Matrixd mat = getMatrix();
//		glbDouble angle = computeGlobeAngle(pt1.y(),pt1.x(),pt2.y(),pt2.x());
//		DegToRad(pt1);
//		DegToRad(pt2);
//		osg::Vec3d pt1up(cos(pt1.x())*cos(pt1.y()), sin(pt1.x())*cos(pt1.y()), sin(pt1.y()));
//		osg::Vec3d pt2up(cos(pt2.x())*cos(pt2.y()), sin(pt2.x())*cos(pt2.y()), sin(pt2.y())); 	
//		osg::Vec3d rotate_vec = pt1up^pt2up;
//		mat = mat * osg::Matrixd::rotate(-angle,rotate_vec);
//
//		//获取变化后相机高度
//		osg::Vec3d cameraPos_w = mat.getTrans();
//		osg::Vec3d cameraPos;
//		osg::Vec3d newcameraPos_w;
//		g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
//		g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),oldcameraPos.z(),newcameraPos_w.x(),newcameraPos_w.y(),newcameraPos_w.z());
//		mat.postMultTranslate(newcameraPos_w - cameraPos_w);
//		setByMatrix(mat);
//	}else{	//相机距地面1000米以下
//		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt1.y()),osg::DegreesToRadians(pt1.x()),oldcameraPos.z(),pt1.x(),pt1.y(),pt1.z());
//		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt2.y()),osg::DegreesToRadians(pt2.x()),oldcameraPos.z(),pt2.x(),pt2.y(),pt2.z());		
//		
//		setByMatrix(getMatrix() * osg::Matrixd::translate(pt1-pt2));
//		//mpr_focusPos -= pt2-pt1;		
//		////获取变化后相机高度
//		//osg::Matrixd mat = getMatrix();
//		//osg::Vec3d cameraPos_w = mat.getTrans();
//		//osg::Vec3d cameraPos;
//		//osg::Vec3d newcameraPos_w;
//		//g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
//		//g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),oldcameraPos.z(),newcameraPos_w.x(),newcameraPos_w.y(),newcameraPos_w.z());
//		//mat.postMultTranslate(newcameraPos_w - cameraPos_w);
//		//setByMatrix(mat);
//	}
//	//相机纠偏 
//	//UpdateCameraElevate();
//	if (mpr_lockmode == GLB_LOCKMODE_2D)
//	{
//		SetCameraPitch(-90.0);
//	}
//	if (mpr_lockmode == GLB_LOCKMODEL_2DN)
//	{
//		SetCameraPitch(-90.0);
//		SetCameraYaw(0.0);
//	}
//	////////////////////////////////////////////////////////////////////////////
//	////获取屏幕点pt1，pt2在地形上的坐标
//	//osg::Vec3d pt0,pt1,pt2;
//	//glbBool isfocus = mpr_globe->ScreenToTerrainCoordinate(ptx1,pty1,pt1.x(),pt1.y(),pt1.z());
//	//if (!isfocus)
//	//	return;
//	//isfocus = mpr_globe->ScreenToTerrainCoordinate(ptx2,pty2,pt2.x(),pt2.y(),pt2.z());
//	//if (!isfocus)
//	//	return;	
//	////中间点pt0
//	//isfocus = mpr_globe->ScreenToTerrainCoordinate(ptx1,pty2,pt0.x(),pt0.y(),pt0.z());
//	//if (!isfocus)
//	//	return;	
//	////pt1->pt0
//	//glbDouble dangley = computeGlobeAngle(pt1.y(),pt1.x(),pt0.y(),pt0.x());
//	////pt0->pt2
//	//glbDouble danglex = computeGlobeAngle(pt0.y(),pt0.x(),pt2.y(),pt2.x());
//	////danglex与pitch有关系
//
//	//if (ptx1 > ptx2)
//	//	danglex = -danglex;
//	//if (pty1 < pty2)
//	//	dangley = -dangley;
//	//Move(danglex,dangley);	
//}

GlbGlobeManipulatorTypeEnum CGlbGlobeManipulator::GetType()
{
	return GLB_MANIPULATOR_FREE;
}

glbBool CGlbGlobeManipulator::FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
	//先求当前相机与地面或对象焦点，距离，姿态
	computeObjMatrix();
	//开启路径动画
	KeyPoint pt0;
	pt0.focusPos = mpr_focusPos;
	pt0.distan = mpr_distance;
	pt0.yaw = mpr_yaw;
	pt0.pitch = mpr_pitch;
	osg::Vec3d cameraPos;
	GetCameraPos(cameraPos);
	osg::Vec3d pt0w,pt1w;
	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),pt0w.x(),pt0w.y(),pt0w.z());

	KeyPoint pt1;
	pt1.focusPos = osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	mpr_focus_temp = pt1.focusPos;
	pt1.distan = distance;
	pt1.yaw = yaw;
	pt1.pitch = pitch;
	osg::Matrixd focusToWorld;
	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(mpr_focus_temp.y()),osg::DegreesToRadians(mpr_focus_temp.x()),mpr_focus_temp.z(),focusToWorld);
	osg::Matrixd mat = osg::Matrixd::translate(0.0,0.0,distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
		focusToWorld;
	pt1w = mat.getTrans();
	glbDouble _First_Last_distance = (pt0w-pt1w).length();

	osg::Camera* pcamera = mpr_globe->GetView()->GetOsgCamera();
	if (pcamera)
	{
		if (IsFlying())
			StopFlying();		
		_FlyCallback = new CGlbGlobeFlyCallback(pt0,pt1,_First_Last_distance,seconds,mpr_isUnderGround);
		pcamera->setUpdateCallback(_FlyCallback.get());
	}
	return true;
}

glbBool CGlbGlobeManipulator::FlyTo2(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
	//先求当前相机与地面或对象焦点，距离，姿态
	computeObjMatrix();
	//开启路径动画
	if (mpr_distance > 3*distance)	//拉近焦点
	{
		osg::Vec3d cameraPos,cameraPos_w;
		osg::Vec3d focusPos_w,newfocusPos_w;
		GetCameraPos(cameraPos);
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_focusPos.y()),osg::DegreesToRadians(mpr_focusPos.x()),mpr_focusPos.z(),focusPos_w.x(),focusPos_w.y(),focusPos_w.z());
		osg::Vec3d lookvec = focusPos_w - cameraPos_w;
		lookvec.normalize();
		newfocusPos_w = cameraPos_w + lookvec*3*distance;
		osg::Vec3d tempFocusPos;
		g_ellipsmodel->convertXYZToLatLongHeight(newfocusPos_w.x(),newfocusPos_w.y(),newfocusPos_w.z(),tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z());
		RadToDeg(tempFocusPos);
		mpr_focusPos = tempFocusPos;
		UpdataMatrix(cameraPos,mpr_focusPos);
	}
	KeyPoint pt0;
	pt0.focusPos = mpr_focusPos;
	pt0.distan = mpr_distance;
	pt0.yaw = mpr_yaw;
	pt0.pitch = mpr_pitch;

	KeyPoint pt1;
	pt1.focusPos = osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	mpr_focus_temp = pt1.focusPos;
	pt1.distan = distance;
	pt1.yaw = yaw;
	pt1.pitch = pitch;

	osg::Camera* pcamera = mpr_globe->GetView()->GetOsgCamera();
	if (pcamera)
	{
		if (IsFlying())
			StopFlying();
		glbBool _flag = mpr_globe->IsUnderGroundMode();

		_FlyCallback = new CGlbGlobeFlyCallback(/*this,*/pt0,pt1,seconds,(mpr_isUnderGround & _flag));
		pcamera->setUpdateCallback(_FlyCallback.get());
	}
	return true;
}

glbBool CGlbGlobeManipulator::computeObjMatrix()
{
	//获取相机位置
	osg::Matrixd matrix = getMatrix();
	osg::Vec3d cameraPos_w = matrix.getTrans();
	//获取焦点
	glbDouble fx,fy;
	mpr_globe->GetView()->getScreenFocusPt(fx,fy);
	//glbBool isObjfocus = mpr_globe->GetView()->Pick(fx,fy,results,InterPoints);
	osg::Vec3d InterPoint;
	glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(fx,fy,InterPoint);
	if (isObjfocus)
	{
		//osg::Vec3d InterPoint = InterPoints[0];
		glbDouble distan = (InterPoint - cameraPos_w).length();
		if (distan < mpr_distance)	//先与物体对象相交
		{
			//计算distance
			mpr_distance = distan;
			//计算yaw，pitch
			osg::Vec3d tempFocusPos;
			g_ellipsmodel->convertXYZToLatLongHeight(InterPoint.x(),InterPoint.y(),InterPoint.z(),tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z());
			osg::Matrixd mat_focusToworld;
			g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z(),mat_focusToworld);	
			osg::Matrixd mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);
			RadToDeg(tempFocusPos);
			mpr_focusPos = tempFocusPos;
			mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));

			if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)	//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
			{
				glbDouble dm = mat_camTofocus(2,0)*mat_camTofocus(2,0)+mat_camTofocus(2,1)*mat_camTofocus(2,1)+mat_camTofocus(2,2)*mat_camTofocus(2,2);
				dm = sqrt(dm);
				mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
			}else{	  
				mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
			}
			return true;
		}
	}
	return false;
}

glbBool CGlbGlobeManipulator::FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, glbBool repeat)
{ 
	//相机位置不变,计算yaw,pitch,distance,JumpTo到新焦点位置
	osg::Vec3d cameraPos;
	GetCameraPos(cameraPos);
	osg::Vec3d _focusPos;
	_focusPos = osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	//由相机位置，焦点位置计算相机矩阵
	glbDouble oldyaw = mpr_yaw;
	osg::Vec3d camerPos = cameraPos;
	osg::Vec3d focusPos = _focusPos;
	DegToRad(camerPos);
	DegToRad(focusPos);

	g_ellipsmodel->convertLatLongHeightToXYZ(camerPos.y(),camerPos.x(),camerPos.z(),camerPos.x(),camerPos.y(),camerPos.z());
	g_ellipsmodel->convertLatLongHeightToXYZ(focusPos.y(),focusPos.x(),focusPos.z(),focusPos.x(),focusPos.y(),focusPos.z());
	glbDouble _distance = 0.0;
	_distance = (focusPos-camerPos).length();
	osg::Matrixd mat;
	g_ellipsmodel->computeLocalToWorldTransformFromXYZ(focusPos.x(),focusPos.y(),focusPos.z(),mat);
	osg::Vec3d LocalPos = camerPos*osg::Matrixd::inverse(mat);
	LocalPos.normalize();
	glbDouble _pitch,_yaw;
	if (LocalPos.z() > 1.0)
	{
		_pitch = -90.0;
		_yaw = oldyaw;
	}
	else if (LocalPos.z() < -1.0)
	{
		_pitch = 90.0;
		_yaw = oldyaw;
	}
	else{
		_pitch = osg::RadiansToDegrees(asin(-LocalPos.z()));
		_yaw = osg::RadiansToDegrees(atan2(-LocalPos.x(),-LocalPos.y()));
	}
	//JumpTo(_focusPos.x(),_focusPos.y(),_focusPos.z(),_distance,_yaw,_pitch);
// 	//设置动画路径
// 	osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
// 	if (repeat)	 
// 		path->setLoopMode(osg::AnimationPath::LOOP);
// 	else
// 		path->setLoopMode(osg::AnimationPath::NO_LOOPING); 
// 
// 	//获取环绕中心点，即新焦点
// 	osg::Vec3d center;
// 	yOrlat = osg::DegreesToRadians(yOrlat);
// 	xOrlon = osg::DegreesToRadians(xOrlon);
// 	g_ellipsmodel->convertLatLongHeightToXYZ(yOrlat,xOrlon,zOrAlt,center.x(),center.y(),center.z());
// 	osg::Vec3d centerUp(cos(xOrlon)*cos(yOrlat), sin(xOrlon)*cos(yOrlat), sin(yOrlat));
// 
// 	osg::Matrixd LocalToWorld;
// 	g_ellipsmodel->computeLocalToWorldTransformFromXYZ(center.x(),center.y(),center.z(),LocalToWorld);
// 
// 	//DegToRad(&cameraPos);	//弧度制
// 	//osg::Vec3d cameraPos_w;		//世界坐标系下相机坐标
// 	//g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),cameraPos.z(),cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z());
// 	osg::Matrixd mat = getMatrix();
// 	osg::Vec3d cameraPos_local = mat.getTrans() * osg::Matrixd::inverse(LocalToWorld);	//相机在区域坐标系(center点)下的坐标
// 	osg::Quat cameraQuat = mat.getRotate();
// 
// 	glbInt32 numSamples = 40;
// 	glbDouble yaw_delt =  2.0f*osg::PI/((glbDouble)numSamples-1.0f);
// 	glbDouble yaw = 0.0;
// 	glbDouble tim = 0.0;
// 	glbDouble tim_delt = seconds/(glbDouble)numSamples;
// 	for ( glbInt32 i = 0; i < numSamples; i++)
// 	{
// 		osg::Vec3d pos = cameraPos_local * osg::Matrix::rotate(yaw,0.0,0.0,1.0) * LocalToWorld;
// 		osg::Quat quat = cameraQuat * osg::Quat(yaw,centerUp);
// 		path->insert(tim,osg::AnimationPath::ControlPoint(pos,quat));
// 		tim += tim_delt;
// 		yaw += yaw_delt;
// 	}
	
	//开启路径动画
	osg::Camera* pcamera = mpr_globe->GetView()->GetOsgCamera();
	if (pcamera)
	{	
		_FlyCallback = new CGlbGlobeFlyCallback(this,seconds,_focusPos,_distance);
		pcamera->setUpdateCallback(_FlyCallback.get());
	}
	return true;
}

glbBool CGlbGlobeManipulator::IsFlying()
{
	if (_FlyCallback.valid())
		return !_FlyCallback->getPause();
	return false;
}

glbBool CGlbGlobeManipulator::StopFlying()
{
	if (_FlyCallback.valid() && _FlyCallback->getPause() == false)
		_FlyCallback->setPause(true);
	return true;
}

glbBool CGlbGlobeManipulator::JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch)
{  
	mpr_focusPos = osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	mpr_focus_temp = mpr_focusPos;
	mpr_yaw = yaw;
	mpr_pitch = pitch;
	mpr_distance = distance; 
	if (mpr_lockmode == GLB_LOCKMODE_2D)
	{
		mpr_pitch =-90.0;
	}else if (mpr_lockmode == GLB_LOCKMODEL_2DN)
	{ 
		mpr_pitch = -90.0;
		yaw = 0.0;
	} 
	//UpdateFocusElevate(getMatrix());
	osg::Vec3d cameraPos;
	GetCameraPos(cameraPos);
	glbDouble elevation = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevation = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
	else
		elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
	UpdateCameraElevate2(elevation);
	return true;
}

glbBool CGlbGlobeManipulator::SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance)
{
	if (mpr_lockmode == GLB_LOCKMODEL_2DN)
		return false;
 	if (yaw < 0.0 || yaw > 360.0)
	{
		while(yaw < 0.0)
		{
			yaw += 360.0;
		}
		while(yaw > 360.0)
		{
			yaw -= 360.0;
		}
	}
	mpr_yaw = yaw;
	osg::Vec3d new_cameraPos;
	GetCameraPos(new_cameraPos);
	glbDouble elevation = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevation = mpr_globe->GetElevationAt(new_cameraPos.x(),new_cameraPos.y());
	else
		elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
	UpdateCameraElevate(elevation);
	return true;
}

glbDouble CGlbGlobeManipulator::GetYaw()
{
	return mpr_yaw;
}

glbBool CGlbGlobeManipulator::SetPitch(glbDouble pitch,osg::Vec3d old_focus,glbDouble olddistance)
{
	if (mpr_lockmode == GLB_LOCKMODE_2D || mpr_lockmode == GLB_LOCKMODEL_2DN)
		return false;
	if (pitch > 0 || pitch < -90.0)
	{
		return false;
	}
	mpr_pitch = pitch;
	osg::Vec3d new_cameraPos;
	GetCameraPos(new_cameraPos);
	glbDouble elevation = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevation = mpr_globe->GetElevationAt(new_cameraPos.x(),new_cameraPos.y());
	else
		elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
	UpdateCameraElevate(elevation);
	return true;
}

glbDouble CGlbGlobeManipulator::GetPitch()
{
	return mpr_pitch;
}

glbBool CGlbGlobeManipulator::GetCameraPos(osg::Vec3d& cameraPos)
{
	osg::Vec3d cameraPos_w = getMatrix().getTrans();
	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	RadToDeg(cameraPos);
	return true;
}

glbBool CGlbGlobeManipulator::GetFocusPos(osg::Vec3d& focusPos)
{
	focusPos = mpr_focusPos;
	return true;
}

glbDouble CGlbGlobeManipulator::GetDistance()
{
	return mpr_distance;
}

void CGlbGlobeManipulator::SetCameraParam(glbDouble yaw, glbDouble pitch, osg::Vec3d focusPos, glbDouble distance)
{
	if (pitch > 0) pitch = 0;
	if (pitch < -90.0) pitch = -90.0;
	
	mpr_yaw = yaw;
	mpr_pitch = pitch;
	mpr_focusPos = focusPos;
	mpr_distance = distance;
}

//此函数ok
glbBool CGlbGlobeManipulator::Move(glbDouble dangleX,glbDouble dangleY)
{
	glbDouble old_focus_alt = mpr_focusPos.z();
	osg::Matrixd mat = getMatrix();
	osg::Vec3d oldcameraPos_w(mat(3,0),mat(3,1),mat(3,2));	//相机在地心世界坐标系下坐标
	osg::Vec3d oldcameraPos;
	g_ellipsmodel->convertXYZToLatLongHeight(oldcameraPos_w.x(),oldcameraPos_w.y(),oldcameraPos_w.z(),oldcameraPos.y(),oldcameraPos.x(),oldcameraPos.z());

	osg::Vec3d test = osg::Vec3d(0.0,1.0,0.0) * mat;

	osg::Vec3d rotate_x = osg::Vec3d(0.0,1.0,0.0) * mat * osg::Matrixd::translate(-oldcameraPos_w);
	osg::Vec3d rotate_y = osg::Vec3d(1.0,0.0,0.0) * mat * osg::Matrixd::translate(-oldcameraPos_w);

	mat = mat * osg::Matrixd::rotate(osg::DegreesToRadians(-dangleX),rotate_x) * osg::Matrixd::rotate(osg::DegreesToRadians(-dangleY),rotate_y);
	//获取变化后相机高度
	//获取变化后相机高度
	osg::Vec3d cameraPos_w = mat.getTrans();
	osg::Vec3d cameraPos;
	osg::Vec3d newcameraPos_w;
	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),oldcameraPos.z(),newcameraPos_w.x(),newcameraPos_w.y(),newcameraPos_w.z());
	mat.postMultTranslate(newcameraPos_w - cameraPos_w);
	setByMatrix(mat);
	//osg::Vec3d cameraPos;
	//GetCameraPos(cameraPos);
	//UpdataMatrix(osg::Vec3d(cameraPos.x(),cameraPos.y(),oldcameraPos.z()),mpr_focusPos);
	//mpr_focusPos.z() = old_focus_alt;	
	//焦点纠偏
	//UpdateFocusElevate(mat);
	//相机纠偏,姿态不变，相机上升
	osg::Vec3d camPos;
	GetCameraPos(camPos);	
	glbDouble elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
	if (!mpr_globe->IsUnderGroundMode())	//未开启地下模式
	{			
		if (camPos.z() - elevation < ElevateThreshold)
		{		
			glbDouble zoomdelt = camPos.z() - elevation - ElevateThreshold;
			zoomdelt /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
			setByMatrix(osg::Matrixd::translate(0.0,0.0,-zoomdelt) * mat);
		}
	}else{	//开启地下模式
		if (abs(camPos.z() - elevation) < MinElevateThreshold)
		{
			osg::Vec3d newcameraPos = camPos;
			if (camPos.z() - elevation > 0)
				newcameraPos.z() = MinElevateThreshold+elevation;
			else
				newcameraPos.z() = -MinElevateThreshold+elevation;
			osg::Matrixd mat = getMatrix();
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(camPos.y()),osg::DegreesToRadians(camPos.x()),camPos.z(),camPos.x(),camPos.y(),camPos.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(newcameraPos.y()),osg::DegreesToRadians(newcameraPos.x()),newcameraPos.z(),newcameraPos.x(),newcameraPos.y(),newcameraPos.z());
			mat.postMultTranslate(newcameraPos - camPos);
			setByMatrix(mat);
		}
		if (cameraPos.z() - mpr_globe->GetUnderGroundAltitude() < ElevateThreshold)
		{
			glbDouble zoomdelt = camPos.z() - mpr_globe->GetUnderGroundAltitude() - ElevateThreshold;
			zoomdelt /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
			setByMatrix(osg::Matrixd::translate(0.0,0.0,-zoomdelt) * mat);
		}
	}	
	if (mpr_lockmode == GLB_LOCKMODE_2D)
	{
		SetCameraPitch(-90.0);
	}
	if (mpr_lockmode == GLB_LOCKMODEL_2DN)
	{
		SetCameraPitch(-90.0);
		SetCameraYaw(0.0);
	}
	return true;
}

glbBool CGlbGlobeManipulator::TranslateCamera(glbDouble x,glbDouble y,glbDouble z)
{
	osg::Matrixd mat = getMatrix();
	mat.postMultTranslate(osg::Vec3d(x,y,z));
	setByMatrix(mat);
	osg::Vec3d cameraPos;
	GetCameraPos(cameraPos);
	glbDouble elevation = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevation = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
	else
		elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
	UpdateCameraElevate(elevation);
	return true;
}

glbBool CGlbGlobeManipulator::DetectCollision(glbBool mode)
{
	mpr_isDetectCollision = mode;
	return mode;
} 

glbBool CGlbGlobeManipulator::SetUnderGroundMode(glbBool mode)
{
	return mpr_globe->SetUnderGroundMode(mode);
}

glbBool CGlbGlobeManipulator::SetUndergroundDepth(glbDouble zOrAlt)
{
	return mpr_globe->SetUnderGroundAltitude(zOrAlt);
}

glbBool CGlbGlobeManipulator::UpdateCameraElevate(glbDouble elevation)
{
	//相机纠偏
	osg::Vec3d cameraPos;
	GetCameraPos(cameraPos);	
	if (!mpr_globe->IsUnderGroundMode())	//未开启地下模式
	{	
		if (cameraPos.z() - elevation < ElevateThreshold)
		{
  			//cameraPos.z() = ElevateThreshold + elevation;
			////相机位置有变化,焦点位置不变，yaw不变，需更新pitch，distance
			//UpdataMatrix(cameraPos,mpr_focusPos);
			//相机姿态不变，位置抬高
			osg::Vec3d cameraPos_w,newcameraPos_w;
			DegToRad(cameraPos);
			g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),cameraPos.z(),cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),elevation+ElevateThreshold,newcameraPos_w.x(),newcameraPos_w.y(),newcameraPos_w.z());
			setByMatrix(getMatrix()*osg::Matrixd::translate(newcameraPos_w-cameraPos_w));
			return true;
		}
	}else{	//开启地下模式
		if (abs(cameraPos.z() - elevation) < MinElevateThreshold)
		{
			osg::Vec3d newcameraPos = cameraPos;
			if (cameraPos.z() - elevation > 0)
				newcameraPos.z() = MinElevateThreshold+elevation;
			else
				newcameraPos.z() = -MinElevateThreshold+elevation;
			osg::Matrixd mat = getMatrix();
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPos.x(),cameraPos.y(),cameraPos.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(newcameraPos.y()),osg::DegreesToRadians(newcameraPos.x()),newcameraPos.z(),newcameraPos.x(),newcameraPos.y(),newcameraPos.z());
			mat.postMultTranslate(newcameraPos - cameraPos);
			setByMatrix(mat);
			return true;
		}
		if (cameraPos.z() - mpr_globe->GetUnderGroundAltitude() < ElevateThreshold)
		{
			//cameraPos.z() = ElevateThreshold +mpr_globe->GetUnderGroundAltitude();
			////相机位置有变化,焦点位置不变，yaw不变，需更新pitch，distance
			//UpdataMatrix(cameraPos,mpr_focusPos);
			//相机姿态不变，位置抬高
			osg::Vec3d cameraPos_w,newcameraPos_w;
			DegToRad(cameraPos);
			g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),cameraPos.z(),cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),mpr_globe->GetUnderGroundAltitude()+ElevateThreshold,newcameraPos_w.x(),newcameraPos_w.y(),newcameraPos_w.z());
			setByMatrix(getMatrix()*osg::Matrixd::translate(newcameraPos_w-cameraPos_w));
			return true;
		}
	}
	return false;
}

glbBool CGlbGlobeManipulator::UpdateCameraElevate2(glbDouble cameraElevate)
{
	//相机纠偏
	osg::Vec3d cameraPos;
	GetCameraPos(cameraPos);	
	if (!mpr_globe->IsUnderGroundMode())	//未开启地下模式
	{	
		//glbDouble cameraElevate = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
		//glbDouble cameraElevate =  mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
		if (cameraPos.z() - cameraElevate < ElevateThreshold)
		{
			cameraPos.z() = ElevateThreshold + cameraElevate;
			//相机位置有变化,焦点位置不变，yaw不变，需更新pitch，distance
			UpdataMatrix(cameraPos,mpr_focus_temp);
			return true;
		}
	}else{	//开启地下模式
		glbDouble cameraElevate = 0.0;
		if (mpr_globe->GetView()->getUpdateCameraMode())
			cameraElevate = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
		else
			cameraElevate = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
		//glbDouble cameraElevate =  mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
		if (abs(cameraPos.z() - cameraElevate) < MinElevateThreshold)
		{
			osg::Vec3d newcameraPos = cameraPos;
			if (cameraPos.z() - cameraElevate > 0)
				newcameraPos.z() = MinElevateThreshold+cameraElevate;
			else
				newcameraPos.z() = -MinElevateThreshold+cameraElevate;
			osg::Matrixd mat = getMatrix();
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPos.x(),cameraPos.y(),cameraPos.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(newcameraPos.y()),osg::DegreesToRadians(newcameraPos.x()),newcameraPos.z(),newcameraPos.x(),newcameraPos.y(),newcameraPos.z());
			mat.postMultTranslate(newcameraPos - cameraPos);
			setByMatrix(mat);
			return true;
		}
		if (cameraPos.z() - mpr_globe->GetUnderGroundAltitude() < ElevateThreshold)
		{
			cameraPos.z() = ElevateThreshold +mpr_globe->GetUnderGroundAltitude();
			//相机位置有变化,焦点位置不变，yaw不变，需更新pitch，distance
			UpdataMatrix(cameraPos,mpr_focus_temp);
			return true;
		}
	}
	return false;
}

//此函数ok										 -
glbBool CGlbGlobeManipulator::UpdateFocusElevate(glbDouble elevation)
{
	//return true;
	osg::Matrixd mat = getMatrix();
	osg::Vec3d cameraPos_w = mat.getTrans();
	osg::Vec3d focusPos;
	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_focusPos.y()),osg::DegreesToRadians(mpr_focusPos.x()),mpr_focusPos.z(),focusPos.x(),focusPos.y(),focusPos.z());
	osg::Vec3d lookvec = focusPos - cameraPos_w;
	osg::Vec3d focusPos_w;
	osg::Vec3d cameraPos;
	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	RadToDeg(cameraPos);
	if (mpr_globe->IsUnderGroundMode())
	{
		glbBool isfocus = mpr_globe->IsRayInterTerrain(cameraPos_w,lookvec,focusPos_w);	//计算射线与地形交点
		if (isfocus)
		{
			glbDouble dist = (focusPos_w - cameraPos_w).length();
			if (dist > WGS_84_RADIUS_POLAR*osg::DegreesToRadians(1.0))
			{
				isfocus = false;
			}
		}
		if (!isfocus && cameraPos.z() < mpr_globe->GetView()->GetTerrainElevationOfCameraPos())/*|| (abs(cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos()) < MinElevateThreshold)*/
		{
			isfocus = mpr_globe->IsRayInterUGPlane(cameraPos_w,lookvec,focusPos_w);	//计算射线与地下参考面交点
		}
		if (isfocus)
		{
			if (abs(focusPos.x()-focusPos_w.x()) < 0.1 && abs(focusPos.y()-focusPos_w.y()) < 0.1 && abs(focusPos.z()-focusPos_w.z()) < 0.1)
			{
				return false;
			}else{
				osg::Vec3d tempFocusPos;
				g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z());
				RadToDeg(tempFocusPos);
				mpr_focusPos = tempFocusPos;
				UpdataMatrix(cameraPos,mpr_focusPos);
				return true;
			}
		}

	}else{
		glbBool isfocus = mpr_globe->IsRayInterTerrain(cameraPos_w,lookvec,focusPos_w);	//计算射线与地形交点
		if (isfocus)
		{
			if (abs(focusPos.x()-focusPos_w.x()) < 0.1 && abs(focusPos.y()-focusPos_w.y()) < 0.1 && abs(focusPos.z()-focusPos_w.z()) < 0.1)
			{
				return false;
			}else{
				osg::Vec3d tempFocusPos;
				g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),tempFocusPos.y(),tempFocusPos.x(),tempFocusPos.z());
				RadToDeg(tempFocusPos);
				mpr_focusPos = tempFocusPos;
				UpdataMatrix(cameraPos,mpr_focusPos);
				return true;
			}
		}
	}
	//setByMatrix(mat);
	//osg::Matrixd mat_focusToworld;
	//g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),focusPos.y(),focusPos.x(),focusPos.z());
	//mpr_focusPos = focusPos;
	//g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z(),mat_focusToworld);	
	//osg::Matrixd mat_camTofocus = getMatrix() * osg::Matrixd::inverse(mat_focusToworld);
	//RadToDeg(mpr_focusPos);
	//mpr_distance = (focusPos_w - cameraPos_w).length();

	//if (mpr_distance < WGS_84_RADIUS_POLAR*osg::DegreesToRadians(5.0))	//ditance不太远
	//{ 
	//	isVirtualFocus = false;
	//	mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));

	//	if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)	//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
	//	{
	//		glbDouble dm = mat_camTofocus(2,0)*mat_camTofocus(2,0)+mat_camTofocus(2,1)*mat_camTofocus(2,1)+mat_camTofocus(2,2)*mat_camTofocus(2,2);
	//		dm = sqrt(dm);
	//		mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
	//	}else{	  
	//		mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
	//	}
	//}
	
	////计算新的mpr_distance
	//osg::Vec3d focusPos_w;
	//osg::Vec3d focusPos;
	//osg::Vec3d cameraPos;
	//osg::Vec3d cameraPos_w(matrix(3,0),matrix(3,1),matrix(3,2));
	//if (abs(mpr_distance) < 0.01)
	//	mpr_distance = 0.01;
	//osg::Vec3d lookvec = osg::Vec3d(0.0,0.0,-mpr_distance)*matrix - cameraPos_w;
	//glbBool isfocus = mpr_globe->IsRayInterTerrain(cameraPos_w,lookvec,focusPos_w);	//计算射线与地形交点
	//osg::Matrixd mat_focusToworld;
	//osg::Matrixd mat_camTofocus;
	//osg::Vec3d oldfocus = mpr_focusPos;
	//glbDouble olddistance = mpr_distance;

	//if (mpr_globe->IsUnderGroundMode())	//地下模式，相机在地下时
	//{
	//	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	//	//相机在地下时
	//	if (cameraPos.z() < mpr_globe->GetView()->GetTerrainElevationOfCameraPos())
	//	{
	//		//射线与地形交点,不能太远,distance有距离限制
	//		if (isfocus)
	//		{
	//			g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),focusPos.y(),focusPos.x(),focusPos.z());
	//			mpr_focusPos = focusPos;
	//			g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z(),mat_focusToworld);	
	//			mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);
	//			RadToDeg(mpr_focusPos);
	//			mpr_distance = (focusPos_w - cameraPos_w).length();
	//			if (mpr_distance < WGS_84_RADIUS_POLAR*osg::DegreesToRadians(5.0))	//ditance不太远
	//			{ 	 
	//				isVirtualFocus = false;
	//				if(IsSamePos(focusPos,mpr_focusPos))	/*焦点没变化*/
	//				{
	//					return false;
	//				}					
	//				mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));

	//				if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)	//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
	//				{
	//					glbDouble dm = mat_camTofocus(2,0)*mat_camTofocus(2,0)+mat_camTofocus(2,1)*mat_camTofocus(2,1)+mat_camTofocus(2,2)*mat_camTofocus(2,2);
	//					dm = sqrt(dm);
	//					mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
	//				}else{	  
	//					mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
	//				}
	//				return true;
	//			}
	//		}
	//		//射线与地下参考面交点,不能太远
	//		glbBool isugfocus = mpr_globe->IsRayInterUGPlane(cameraPos_w,lookvec,focusPos_w);	//计算射线与地下参考面交点
	//		if (isugfocus)
	//		{
	//			g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),focusPos.y(),focusPos.x(),focusPos.z());
	//			mpr_focusPos = focusPos;
	//			g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z(),mat_focusToworld);	
	//			mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);
	//			RadToDeg(mpr_focusPos);
	//			mpr_distance = (focusPos_w - cameraPos_w).length();			
	//			if (mpr_distance < WGS_84_RADIUS_POLAR*osg::DegreesToRadians(5.0))	//ditance不太远
	//			{ 
	//				isVirtualFocus = false;
	//				if(IsSamePos(focusPos,mpr_focusPos))	/*焦点没变化*/
	//				{
	//					return false;
	//				}
	//				mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));

	//				if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)	//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
	//				{
	//					glbDouble dm = mat_camTofocus(2,0)*mat_camTofocus(2,0)+mat_camTofocus(2,1)*mat_camTofocus(2,1)+mat_camTofocus(2,2)*mat_camTofocus(2,2);
	//					dm = sqrt(dm);
	//					mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
	//				}else{	  
	//					mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
	//				}
	//				return true;
	//			}
	//		}

	//		//无焦点
	//		isVirtualFocus = true;
	//		mpr_virtualFocusPos = oldfocus;
	//		mpr_distance = olddistance;
	//		mpr_focusPos = osg::Vec3d(0,0,-olddistance)*matrix; 
	//		g_ellipsmodel->convertXYZToLatLongHeight(mpr_focusPos.x(),mpr_focusPos.y(),mpr_focusPos.z(),mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z());
	//		g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z(),mat_focusToworld);	
	//		mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);		
	//		RadToDeg(mpr_focusPos);
	//		mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));

	//		if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)	//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
	//		{
	//			glbDouble dm = mat_camTofocus(2,0)*mat_camTofocus(2,0)+mat_camTofocus(2,1)*mat_camTofocus(2,1)+mat_camTofocus(2,2)*mat_camTofocus(2,2);
	//			dm = sqrt(dm);
	//			mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
	//		}else{	  
	//			mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
	//		}
	//		return true;
	//	}
	//}

	//if (isfocus)
	//{	 
	//	g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),focusPos.y(),focusPos.x(),focusPos.z());
	//	mpr_focusPos = focusPos;
	//	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z(),mat_focusToworld);	
	//	mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);
	//	RadToDeg(mpr_focusPos);
	//	if(IsSamePos(focusPos,mpr_focusPos))	/*焦点没变化*/
	//	{
	//		return false;
	//	}
	//	//mpr_distance = lookvec.length();
	//	mpr_distance = (focusPos_w - cameraPos_w).length();
	//}else{
	//	mpr_focusPos = osg::Vec3d(0,0,-mpr_distance)*matrix; 
	//	g_ellipsmodel->convertXYZToLatLongHeight(mpr_focusPos.x(),mpr_focusPos.y(),mpr_focusPos.z(),mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z());
	//	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(mpr_focusPos.y(),mpr_focusPos.x(),mpr_focusPos.z(),mat_focusToworld);	
	//	mat_camTofocus = matrix * osg::Matrixd::inverse(mat_focusToworld);		
	//	RadToDeg(mpr_focusPos);
	//}
	//mpr_yaw = osg::RadiansToDegrees(atan2(-mat_camTofocus(0,1),mat_camTofocus(0,0)));

	//if (mat_camTofocus(2,2) > 1.0 || mat_camTofocus(2,2) < -1.0)	//防止mat_camTofocus(2,2)溢出(-1.0,1.0)范围,asin无法计算
	//{
	//	glbDouble dm = mat_camTofocus(2,0)*mat_camTofocus(2,0)+mat_camTofocus(2,1)*mat_camTofocus(2,1)+mat_camTofocus(2,2)*mat_camTofocus(2,2);
	//	dm = sqrt(dm);
	//	mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)/dm));
	//}else{	  
	//	mpr_pitch = osg::RadiansToDegrees(asin(-mat_camTofocus(2,2)));
	//}
	//return true;
	////////////////////////////////////////////////////////////////////////////
	//////焦点纠偏
	////osg::Vec3d focusPos_w;
	////osg::Vec3d focusPos;
	////osg::Vec3d cameraPos;
	//////osg::Matrixd mat = getMatrix();
	////osg::Vec3d cameraPos_w(mat(3,0),mat(3,1),mat(3,2));
	////osg::Vec3d pos = osg::Vec3d(0.0,0.0,-mpr_distance) * mat;
	////g_ellipsmodel->convertXYZToLatLongHeight(pos.x(),pos.y(),pos.z(),pos.y(),pos.x(),pos.z());
	////RadToDeg(&pos);
	////osg::Vec3d lookvec = osg::Vec3d(0.0,0.0,-mpr_distance)*mat - cameraPos_w;
	////glbBool isfocus = mpr_globe->IsRayInterTerrain(cameraPos_w,lookvec,focusPos_w);	//计算射线与地形交点
	////if (!isfocus)	/*没有交点*/
	////{
	////	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	////	RadToDeg(&cameraPos);
	////	mpr_focusPos = computeFocusPosition(cameraPos,mpr_distance,mpr_yaw,mpr_pitch);
	////	return true; 		
	////}else{
	////	g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),focusPos.y(),focusPos.x(),focusPos.z());
	////	RadToDeg(&focusPos);
	////	if(IsSamePos(focusPos,mpr_focusPos))	/*焦点没变化*/
	////	{
	////		return false;
	////	}else{	/*焦点有变化*/
	////		g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	////		RadToDeg(&cameraPos);
	////		mpr_focusPos = focusPos;
	////		UpdataMatrix(cameraPos,mpr_focusPos,mpr_distance,mpr_yaw,mpr_pitch);	//更新pitch，distance
	////		return true;
	////	}
	////} 
	return false;
}

//此函数ok
osg::Vec3d CGlbGlobeManipulator::computeCameraPosition(osg::Vec3d& focusPos,glbDouble distance,glbDouble yaw,glbDouble pitch)
{ 
	osg::Matrixd focusToWorld;
	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),focusToWorld);
	osg::Matrixd mat = osg::Matrixd::translate(0.0,0.0,mpr_distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(mpr_pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-mpr_yaw),0.0,0.0,1.0)*
		focusToWorld;

	osg::Vec3d cameraPos_w;
	osg::Vec3d cameraPos;
	cameraPos_w = mat.getTrans();
	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
	RadToDeg(cameraPos);
	return cameraPos;

	/**在焦点当地地理坐标系下
	 *向量(0,distance,0)经过绕(1,0,0)轴转动pitch角，
	 *绕(0,0,1)轴转动yaw角即为相机look向量
	**/
	//////////////////////////////////////////////////////////////////////////
	//ok，计算误差在1e-8到1e-9之间
//  	pitch = osg::DegreesToRadians(pitch);
//  	yaw = osg::DegreesToRadians(yaw);
//  	osg::Vec3d lookvec = osg::Matrixd::rotate(yaw,0.0,0.0,1.0)*osg::Matrixd::rotate(osg::PI_2+pitch,0.0,1.0,0.0)*osg::Vec3d(0.0,0.0,distance);
//  	//将focusPos由地心大地坐标系的变换矩阵 到 地心空间直角坐标系
//  	osg::Matrixd focusToWorld;
//  	focusPos = osg::Vec3d(osg::DegreesToRadians(focusPos.x()),osg::DegreesToRadians(focusPos.y()),focusPos.z());
//  	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(focusPos.y(),focusPos.x(),focusPos.z(),focusToWorld);	//获取焦点处LocalToWorld的变换矩阵
//  	osg::Vec3d cameraPos_w = lookvec * focusToWorld;//相机在地心空间直角坐标系下的世界坐标
//  	osg::Vec3d cameraPos;
//  	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());//将cameraPos由地心空间直角坐标系 到 地心大地坐标系的变换矩阵
// 	RadToDeg(&cameraPos);
//  	return cameraPos;

// 	//ok，计算误差在1e-8到1e-9之间
//   	osg::Vec3d focusPos_c;	//焦点处，地心坐标
//   	osg::Vec4d cameraPos_c(distance,0.0,0.0,1.0);
// 	DegToRad(&focusPos);//角度转弧度
//   	cameraPos_c = osg::Matrixd::rotate(-focusPos.x(),0.0,0.0,1.0) * osg::Matrixd::rotate(focusPos.y(),0.0,1.0,0.0) * cameraPos_c;//转动经纬度
//   
//   	osg::Vec3d rotate_pitch(-sin(focusPos.x()), cos(focusPos.x()), 0);	//east
//   	osg::Vec3d rotate_yaw(cos(focusPos.x())*cos(focusPos.y()), sin(focusPos.x())*cos(focusPos.y()), sin(focusPos.y()));	//up
//   
//   	cameraPos_c = osg::Matrixd::rotate(osg::DegreesToRadians(yaw),rotate_yaw) * osg::Matrixd::rotate(osg::DegreesToRadians(pitch+90),rotate_pitch) * cameraPos_c;
//   
//   	g_ellipsmodel->convertLatLongHeightToXYZ(focusPos.y(),focusPos.x(),focusPos.z(),focusPos_c.x(),focusPos_c.y(),focusPos_c.z());	//球面系转直角系
//   	cameraPos_c = /*cameraPos_c + focusPos_c;*/cameraPos_c * osg::Matrix::translate(focusPos_c);
//   
//   	osg::Vec3d cameraPos;
//   	g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_c.x(),cameraPos_c.y(),cameraPos_c.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
//  	RadToDeg(&cameraPos);//弧度转角度
//   	return cameraPos;
}

////此函数有问题
//osg::Vec3d CGlbGlobeManipulator::computeFocusPosition(osg::Vec3d& cameraPos,glbDouble distance,glbDouble yaw,glbDouble pitch)
//{
//	//////////////////////////////////////////////////////////////////////////
//	/**在相机坐标系下，
//	 *点(0,0,-distance)即为focus位置
//	**/
//	osg::Matrixd mat = getMatrix();
//	osg::Vec3d focusPos_w = osg::Vec3d(0,0,-mpr_distance)*mat;
//	osg::Vec3d focusPos;
//	g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),focusPos.y(),focusPos.x(),focusPos.z());
//	RadToDeg(focusPos);
//	return focusPos;
//
////   	pitch = osg::DegreesToRadians(pitch);
//// 	yaw = osg::DegreesToRadians(yaw);
//// 	osg::Vec3d lookvec = osg::Vec3d(0.0,0.0,-distance)*osg::Matrixd::rotate(osg::PI_2+pitch,0.0,1.0,0.0)*osg::Matrixd::rotate(yaw,0.0,0.0,1.0);
//// 
////  	osg::Matrixd cameraToWorld;
////  	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(cameraPos.y(),cameraPos.x(),cameraPos.z(),cameraToWorld);	//获取相机处LocalToWorld的变换矩阵
////  	osg::Vec3d focusPos_w = lookvec * cameraToWorld;
////  	osg::Vec3d focusPos;
////  	g_ellipsmodel->convertXYZToLatLongHeight(focusPos_w.x(),focusPos_w.y(),focusPos_w.z(),focusPos.y(),focusPos.x(),focusPos.z());
//// 	RadToDeg(&focusPos);
////  	return focusPos;
//}

//此函数ok~~
void CGlbGlobeManipulator::UpdataMatrix(osg::Vec3d& camPos,osg::Vec3d& focPos)
{
	glbDouble oldyaw = mpr_yaw;
	mpr_focusPos = focPos;
	osg::Vec3d camerPos = camPos;
	osg::Vec3d focusPos = focPos;
	DegToRad(camerPos);
	DegToRad(focusPos);

	g_ellipsmodel->convertLatLongHeightToXYZ(camerPos.y(),camerPos.x(),camerPos.z(),camerPos.x(),camerPos.y(),camerPos.z());
	g_ellipsmodel->convertLatLongHeightToXYZ(focusPos.y(),focusPos.x(),focusPos.z(),focusPos.x(),focusPos.y(),focusPos.z());
	mpr_distance = (focusPos-camerPos).length();
	osg::Matrixd mat;
	g_ellipsmodel->computeLocalToWorldTransformFromXYZ(focusPos.x(),focusPos.y(),focusPos.z(),mat);
	osg::Vec3d LocalPos = camerPos*osg::Matrixd::inverse(mat);
	LocalPos.normalize();
	if (LocalPos.z() > 1.0)
	{
		mpr_pitch = -90.0;
		mpr_yaw = oldyaw;
	}
	else if (LocalPos.z() < -1.0)
	{
		mpr_pitch = 90.0;
		mpr_yaw = oldyaw;
	}
	else{
		mpr_pitch = osg::RadiansToDegrees(asin(-LocalPos.z()));
		mpr_yaw = oldyaw;
		//mpr_yaw = osg::RadiansToDegrees(atan2(-LocalPos.x(),-LocalPos.y()));
	}	
	//////////////////////////////////////////////////////////////////////////
	////在世界坐标系下
	//osg::Vec3d cameraPos = camPos;
	//osg::Vec3d focusPos = focPos;
	//DegToRad(cameraPos);
	//DegToRad(focusPos);
	////相机处向上向量
	//osg::Vec3d cameraup(cos(cameraPos.x())*cos(cameraPos.y()), sin(cameraPos.x())*cos(cameraPos.y()), sin(cameraPos.y()));

	//g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),cameraPos.z(),cameraPos.x(),cameraPos.y(),cameraPos.z());
	//g_ellipsmodel->convertLatLongHeightToXYZ(focusPos.y(),focusPos.x(),focusPos.z(),focusPos.x(),focusPos.y(),focusPos.z());
	//osg::Vec3d lookvec = focusPos-cameraPos;	//世界坐标系下
	//glbDouble distan = lookvec.length();
	//osg::Quat quat;
	//quat.makeRotate(osg::Vec3d(0.0,0.0,-distan),lookvec);  

	//osg::Vec3d s = lookvec^cameraup;
	//osg::Vec3d up = s^lookvec;	//相机朝向向量
	//up.normalize();

	////相机向上向量rotate后
	//osg::Vec3d up2 = osg::Vec3d(0.0,1.0,0.0)*osg::Matrixd::rotate(quat);
	//up2.normalize();

	//osg::Quat quat2;
	//quat2.makeRotate(up2,up);	

	//osg::Matrixd mat = osg::Matrixd::rotate(quat) * osg::Matrix::rotate(quat2) * osg::Matrixd::translate(cameraPos);
	//glbDouble oldyaw = mpr_yaw;
	//setByMatrix(mat);
	//mpr_yaw = oldyaw;
}

glbBool CGlbGlobeManipulator::IsSamePos(osg::Vec3d vec1,osg::Vec3d vec2)
{
	if (abs(vec1.x() - vec2.x()) < ZERO_E7 && abs(vec1.y() - vec2.y()) < ZERO_E7 && abs(vec1.z() - vec2.z()) < 0.1)
		return true;
	else
		return false;
}

glbDouble CGlbGlobeManipulator::getMoveCoefficient(glbDouble Alt)
{
	if (Alt > 100000)
		return 1.0;
	else if (Alt < 10)
		Alt = 10.0;
	return Alt*0.000001;
}

glbBool CGlbGlobeManipulator::ScenePtToGePt(glbDouble x,glbDouble y,osg::Vec3d& resultPos)
{
	osg::Vec3d pt11_w;
	glbDouble ptx1_s = x,pty1_s = x;
	mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
	//glbBool isObjfocus = mpr_globe->GetView()->Pick(ptx1_s,pty1_s,results,InterPoints);
	osg::Vec3d InterPoint;
	glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx1_s,pty1_s,InterPoint);
	if (isObjfocus)
	{
		pt11_w = InterPoint;
		//pt11_w = InterPoints[0];	
	}
	glbBool ischoose = mpr_globe->ScreenToTerrainCoordinate(x,y,resultPos.x(),resultPos.y(),resultPos.z());
	if (!ischoose)
	{
		if (mpr_globe->IsUnderGroundMode())	  //开启地下模式 
		{
			glbBool isugchoose = mpr_globe->ScreenToUGPlaneCoordinate(x,y,resultPos.x(),resultPos.y(),resultPos.z());
			if (!isugchoose)
			{
				resultPos = osg::Vec3d(DBL_MAX,DBL_MAX,DBL_MAX);
				return false;
			}
		}else if (!isObjfocus)			//未开地下模式
		{
			resultPos = osg::Vec3d(DBL_MAX,DBL_MAX,DBL_MAX);
			return false;
		}
	}			
	if (isObjfocus)
	{
		osg::Vec3d oldcameraPos_w = getMatrix().getTrans();
		osg::Vec3d pt1_w;
		osg::Vec3d pttemp = resultPos;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(resultPos.y()),osg::DegreesToRadians(resultPos.x()),resultPos.z(),pt1_w.x(),pt1_w.y(),pt1_w.z());
		glbDouble dis1 = (oldcameraPos_w - pt1_w).length();
		glbDouble dis2 = (oldcameraPos_w - pt11_w).length();
		if (dis2 < dis1)
		{
			g_ellipsmodel->convertXYZToLatLongHeight(pt11_w.x(),pt11_w.y(),pt11_w.z(),resultPos.y(),resultPos.x(),resultPos.z());
			RadToDeg(resultPos);
		}		
	}
	return true;
}

glbBool CGlbGlobeManipulator::handleTouch(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
	if (!ea.getTouchData())
		return false;
	glbDouble tim1 = 0.0;
	glbDouble tim2 = 0.0;
	unsigned int touchpointnum = ea.getTouchData()->getNumTouchPoints();
	if (touchpointnum == 1)	//单点触控
	{
		tim1 = ea.getTime();
		switch(ea.getTouchData()->get(0).phase)
		{
		case osgGA::GUIEventAdapter::TOUCH_BEGAN:	//push
			{
				_isTouchDelay = true;
				//判断双击
				if (_ga_release != NULL)
				{
					glbDouble time_delt = abs(ea.getTime() - _ga_release->getTime());						
					glbDouble tx = ea.getTouchData()->get(0).x - _ga_release->getTouchData()->get(0).x;
					glbDouble ty = ea.getTouchData()->get(0).y - _ga_release->getTouchData()->get(0).y;
					glbDouble distan_delt = tx*tx+ty*ty;
					if (time_delt < 0.5 && distan_delt < 400)	//两次单击时间间隔小于0.5s,定义为双击
					{
						//return false;
						_ga_t0 = &ea;
						if (IsFlying())
							StopFlying(); 
						//GlbLogWOutput(GLB_LOGTYPE_INFO,L"flyto \r\n");
						glbDouble dx = _ga_t0->getTouchData()->get(0).x;
						glbDouble dy = _ga_t0->getTouchData()->get(0).y;
						//窗口坐标系(鼠标位置) 转换为 屏幕坐标
						mpr_globe->GetView()->WindowToScene(dx,dy);
						osg::Vec3d pt11_w;
						glbDouble ptx1_s = dx,pty1_s = dy;
						mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
						//glbBool isObjfocus = mpr_globe->GetView()->Pick(ptx1_s,pty1_s,results,InterPoints);
						osg::Vec3d InterPoint;
						glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx1_s,pty1_s,InterPoint);
						if (isObjfocus)
						{
							//GlbLogWOutput(GLB_LOGTYPE_INFO,L"isObjfocus\r\n");
							//CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(results[0].get());
							//if (mpr_globe->IsUnderGroundMode())
							pt11_w = InterPoint;
							//else
							//	if (robj && robj->IsGround())
							//		pt11_w = InterPoints[0];	
						}
						glbDouble lon = 0.0;
						glbDouble lat = 0.0;
						glbDouble alt = 0.0;
						glbBool isclick = mpr_globe->ScreenToTerrainCoordinate(dx,dy,lon,lat,alt);
						glbDouble seconds = 1.0;
						if (!isclick)
						{  
							if (mpr_globe->IsUnderGroundMode())	//开启地下模式
							{
								isclick = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,lon,lat,alt);
								if (!isclick  && !isObjfocus)
									return false;
							}else if (!isObjfocus)
								return false;
						}
						glbDouble groundlon = lon,groundlat = lat,groundalt = alt;
						glbDouble distan = 0.0;	
						osg::Vec3d pt1_w;
						osg::Vec3d oldcameraPos_w = getMatrix().getTrans();
						g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),alt,pt1_w.x(),pt1_w.y(),pt1_w.z());
						glbDouble dis1 = (oldcameraPos_w - pt1_w).length();
						distan = dis1;
						if (isObjfocus)
						{
							//GlbLogWOutput(GLB_LOGTYPE_INFO,L"isObjfocus\r\n");
							glbDouble dis2 = (oldcameraPos_w - pt11_w).length();
							if (dis2 < dis1)
							{			
								g_ellipsmodel->convertXYZToLatLongHeight(pt11_w.x(),pt11_w.y(),pt11_w.z(),lat,lon,alt);
								lon = osg::RadiansToDegrees(lon);
								lat = osg::RadiansToDegrees(lat);
								distan = min(dis1,dis2);
								//if (isclick)
									//GlbLogWOutput(GLB_LOGTYPE_INFO,L"第1点:(%.5f,%.5f,%.5f),第2点:(%.5f,%.5f,%.5f) \r\n",lon,lat,alt,groundlon,groundlat,groundalt);
							}
						}				
						if (mpr_lockmode == GLB_LOCKMODE_2D || mpr_lockmode == GLB_LOCKMODEL_2DN)
						{
							//if (distan > 0)
							FlyTo2(lon,lat,alt,0.33*distan,mpr_yaw,mpr_pitch,seconds);
							//else
							//	FlyTo(lon,lat,alt,0.33*dis1,mpr_yaw,mpr_pitch,seconds);
							us.requestRedraw();
							return true;
						}
						//计算目标pitch角
						osg::Vec3d cameraPos = getMatrix().getTrans();
						osg::Vec3d targetPos;
						g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),alt,targetPos.x(),targetPos.y(),targetPos.z());
						osg::Vec3d lookvec = targetPos - cameraPos;
						lookvec.normalize();
						//targetPos处向上向量
						osg::Vec3d up(cos(osg::DegreesToRadians(lon))*cos(osg::DegreesToRadians(lat)), sin(osg::DegreesToRadians(lon))*cos(osg::DegreesToRadians(lat)), sin(osg::DegreesToRadians(lat)));
						up.normalize();
						glbDouble tempnomal = lookvec*up;
						tempnomal = max(min(tempnomal,1.0),-1.0);
						glbDouble pitch = -osg::RadiansToDegrees(acos(tempnomal)) + 90; 
						//GlbLogWOutput(GLB_LOGTYPE_INFO,L"FlyTo2:%f,%f,%f,%f,%f,%f \r\n",lon,lat,alt,distan,mpr_yaw,pitch);
						//if (distan > 0)
						FlyTo2(lon,lat,alt,0.33*distan,mpr_yaw,pitch,seconds);
						//else
						//	FlyTo(lon,lat,alt,0.33*dis1,mpr_yaw,pitch,seconds);
						us.requestRedraw();
						return true;
					}
				}
				//GlbLogWOutput(GLB_LOGTYPE_INFO,L"push");
				_ga_t0 = &ea;
				_ga_t1 = &ea;
				if (IsFlying())
					StopFlying();
				_pushpitch = mpr_pitch;
				_pushyaw = mpr_yaw;
				_pushMatrix = getMatrix();
				osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
				_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();
				//记录选中点经纬高
				glbDouble dx = ea.getTouchData()->get(0).x;//touchpoint.x;
				glbDouble dy = ea.getTouchData()->get(0).y;
				//窗口坐标系(鼠标位置) 转换为 屏幕坐标,触控版
				mpr_globe->GetView()->WindowToScene(dx,dy);
				ScenePtToGePt(dx,dy,_pushPos);
				//绘制选中点图标
				us.requestRedraw();
				return true;
			}
			break;
		case osgGA::GUIEventAdapter::TOUCH_ENDED:	//release
			{						
				//GlbLogWOutput(GLB_LOGTYPE_INFO,L"release");
				flushMouseEventStack();
				addMouseEvent(ea);
				_pushMatrix.makeIdentity();
				_pushVPW.makeIdentity();
				_pushPos = osg::Vec3(FLT_MAX,FLT_MAX,FLT_MAX);
				_ga_release = &ea;
				return false;
			}
			break;
		case osgGA::GUIEventAdapter::TOUCH_MOVED:	//drag,左键拖拽
			{
				if (abs(tim1 - tim2) < 1.0)
					return false;
				if (IsFlying())
					//StopFlying();
					return false;
				if (_ga_t1 == NULL)
					return false;
				if (ea.getTime() - _ga_t1->getTime() < 1.0 && _isTouchDelay)
				{
					if (abs(ea.getTouchData()->get(0).x-_ga_t1->getTouchData()->get(0).x) > 2.0 && abs(ea.getTouchData()->get(0).y-_ga_t1->getTouchData()->get(0).y) > 2.0)
						_isTouchDelay = false;
					return false;
				}

				if (_isTouchDelay)
				{
					//获取左键按下时刻鼠标位置点在地形上的3维坐标
					glbDouble dx0 = _ga_t0->getTouchData()->get(0).x;
					glbDouble dy0 = _ga_t0->getTouchData()->get(0).y;
					//获取左键按下时，鼠标位置
					glbDouble dx1 = ea.getTouchData()->get(0).x;
					glbDouble dy1 = ea.getTouchData()->get(0).y;
					//窗口坐标系(鼠标位置) 转换为 屏幕坐标
					mpr_globe->GetView()->WindowToScene(dx0,dy0);
					mpr_globe->GetView()->WindowToScene(dx1,dy1);	
					//获取拖拽的向量				
					glbDouble dx = dx1 - dx0;
					glbDouble dy = dy1 - dy0;
					_ga_t0 = &ea;
					glbDouble yaw = -dx*0.3;	//系数暂定0.3
					glbDouble pitch = dy*0.3;

					Rotate(_pushPos.x(),_pushPos.y(),_pushPos.z(),pitch,yaw);	
					us.requestRedraw();
					return true;
				}else{
					//GlbLogWOutput(GLB_LOGTYPE_INFO,L"drag");
					//获取左键按下时刻鼠标位置点在地形上的3维坐标
					glbDouble dx0 = _ga_t1->getTouchData()->get(0).x;
					glbDouble dy0 = _ga_t1->getTouchData()->get(0).y;
					//获取左键按下时，鼠标位置
					glbDouble dx = ea.getTouchData()->get(0).x;
					glbDouble dy = ea.getTouchData()->get(0).y;
					//窗口坐标系(鼠标位置) 转换为 屏幕坐标
					mpr_globe->GetView()->WindowToScene(dx0,dy0);
					mpr_globe->GetView()->WindowToScene(dx,dy);

					//获取左键按下后，当前鼠标位置
					_ga_t0 = &ea;
					//glbDouble dx = _ga_t0->getX();
					//glbDouble dy = _ga_t0->getY();
					//窗口坐标系(鼠标位置) 转换为 屏幕坐标
					//mpr_globe->GetView()->WindowToScreen(dx,dy);
					Drag(dx0,dy0,dx,dy);
					us.requestRedraw();
					return true;
				}
			}
			break;
		default:
			break;
		}
	}else if (touchpointnum > 1)	//多点点触控,zoom
	{
		tim2 = ea.getTime();
		if (_ga_touch == NULL)
			_ga_touch = &ea;
		if (tim2 - _ga_touch->getTime() > 0.5)
		{
			_ga_touch = &ea;
			//GlbLogWOutput(GLB_LOGTYPE_INFO,L"更新_ga_touch \r\n");
			//第0点
			glbDouble touchtx0 = _ga_touch->getTouchData()->get(0).x;
			glbDouble touchty0 = _ga_touch->getTouchData()->get(0).y;
			mpr_globe->GetView()->WindowToScene(touchtx0,touchty0);
			ScenePtToGePt(touchtx0,touchty0,_pushPos);
			//第1点
			glbDouble touchtx1 = _ga_touch->getTouchData()->get(1).x;
			glbDouble touchty1 = _ga_touch->getTouchData()->get(1).y;
			mpr_globe->GetView()->WindowToScene(touchtx1,touchty1);
			ScenePtToGePt(touchtx1,touchty1,_pushPos2);			
		}
		glbDouble oldtx0 = _ga_touch->getTouchData()->get(0).x;
		glbDouble oldty0 = _ga_touch->getTouchData()->get(0).y;
		glbDouble newtx0 = ea.getTouchData()->get(0).x;
		glbDouble newty0 = ea.getTouchData()->get(0).y;
		mpr_globe->GetView()->WindowToScene(oldtx0,oldty0);
		mpr_globe->GetView()->WindowToScene(newtx0,newty0);
		//第1点
		glbDouble oldtx1 = _ga_touch->getTouchData()->get(1).x;
		glbDouble oldty1 = _ga_touch->getTouchData()->get(1).y;
		glbDouble newtx1 = ea.getTouchData()->get(1).x;
		glbDouble newty1 = ea.getTouchData()->get(1).y;
		mpr_globe->GetView()->WindowToScene(oldtx1,oldty1);
		mpr_globe->GetView()->WindowToScene(newtx1,newty1);

		//GlbLogWOutput(GLB_LOGTYPE_INFO,L"多点触控:比对点(%f,%f,%f,%f),当前点(%f,%f,%f,%f)\r\n",oldtx0,oldty0,oldtx1,oldty1,newtx0,newty0,newtx1,newty1);
		if (ea.getTouchData()->get(0).phase == osgGA::GUIEventAdapter::TOUCH_ENDED || ea.getTouchData()->get(1).phase == osgGA::GUIEventAdapter::TOUCH_ENDED)
		{
			if (ea.getTouchData()->get(0).phase == osgGA::GUIEventAdapter::TOUCH_ENDED)
			{
				//GlbLogWOutput(GLB_LOGTYPE_INFO,L"release\r\n");
				_ga_t0 = &ea;
				_ga_t1 = &ea;
				if (IsFlying())
					StopFlying();
				_pushpitch = mpr_pitch;
				_pushyaw = mpr_yaw;
				_pushMatrix = getMatrix();
				osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
				_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();
				//记录选中点经纬高
				//glbDouble dx = ea.getTouchData()->get(1).x;//touchpoint.x;
				//glbDouble dy = ea.getTouchData()->get(1).y;
				////窗口坐标系(鼠标位置) 转换为 屏幕坐标,触控版
				//mpr_globe->GetView()->WindowToScene(dx,dy);
				//ScenePtToGePt(dx,dy,_pushPos);
				_pushPos = _pushPos2;
				_pushPos2 = osg::Vec3d(DBL_MAX,DBL_MAX,DBL_MAX);
				return false;
			}			
			if (ea.getTouchData()->get(1).phase == osgGA::GUIEventAdapter::TOUCH_ENDED)
			{
				//GlbLogWOutput(GLB_LOGTYPE_INFO,L"release\r\n");
				_ga_t0 = &ea;
				_ga_t1 = &ea;
				if (IsFlying())
					StopFlying();
				_pushpitch = mpr_pitch;
				_pushyaw = mpr_yaw;
				_pushMatrix = getMatrix();
				osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
				_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();
				//记录选中点经纬高
				//glbDouble dx = ea.getTouchData()->get(0).x;//touchpoint.x;
				//glbDouble dy = ea.getTouchData()->get(0).y;
				////窗口坐标系(鼠标位置) 转换为 屏幕坐标,触控版
				//mpr_globe->GetView()->WindowToScene(dx,dy);
				//ScenePtToGePt(dx,dy,_pushPos);
				_pushPos2 = osg::Vec3d(DBL_MAX,DBL_MAX,DBL_MAX);
				return false;
			}
		}else if ((ea.getTouchData()->get(0).phase == osgGA::GUIEventAdapter::TOUCH_MOVED) && (ea.getTouchData()->get(1).phase == osgGA::GUIEventAdapter::TOUCH_MOVED))
		{
			//GlbLogWOutput(GLB_LOGTYPE_INFO,L"move\r\n");
			//第0点
			glbDouble oldtx0 = _ga_t0->getTouchData()->get(0).x;
			glbDouble oldty0 = _ga_t0->getTouchData()->get(0).y;
			glbDouble newtx0 = ea.getTouchData()->get(0).x;
			glbDouble newty0 = ea.getTouchData()->get(0).y;
			glbDouble touchtx0 = _ga_touch->getTouchData()->get(0).x;
			glbDouble touchty0 = _ga_touch->getTouchData()->get(0).y;
			mpr_globe->GetView()->WindowToScene(oldtx0,oldty0);
			mpr_globe->GetView()->WindowToScene(newtx0,newty0);
			mpr_globe->GetView()->WindowToScene(touchtx0,touchty0);

			//第1点
			glbDouble oldtx1 = _ga_t0->getTouchData()->get(1).x;
			glbDouble oldty1 = _ga_t0->getTouchData()->get(1).y;
			glbDouble newtx1 = ea.getTouchData()->get(1).x;
			glbDouble newty1 = ea.getTouchData()->get(1).y;
			glbDouble touchtx1 = _ga_touch->getTouchData()->get(1).x;
			glbDouble touchty1 = _ga_touch->getTouchData()->get(1).y;
			mpr_globe->GetView()->WindowToScene(oldtx1,oldty1);
			mpr_globe->GetView()->WindowToScene(newtx1,newty1);
			mpr_globe->GetView()->WindowToScene(touchtx1,touchty1);
			//rotate
//   			if (abs(newtx0 - touchtx0) < 3.0 && abs(newty0 - touchty0) < 3.0)	//第1个点位置不变
//   			{
//   				GlbLogWOutput(GLB_LOGTYPE_INFO,L"rotate\r\n");
//   				//ScenePtToGePt(touchtx0,touchty0,_pushPos);
//   				glbDouble yaw,pitch;
//   				osg::Vec2d old_vec(oldtx1-oldtx0,oldty1-oldty0);
//   				osg::Vec2d new_vec(newtx1-oldtx0,newty1-oldty0);
//   				pitch = (old_vec.length()-new_vec.length())*0.1;
//   				yaw = osg::RadiansToDegrees(atan2(old_vec.y(),old_vec.x())-atan2(new_vec.y(),new_vec.x()));
//   
//   				_ga_t0 = &ea;
//   				if (_pushPos.x() > 1000.0)
//   				{
//   					return false;
//   				}else{
//   					Rotate(_pushPos.x(),_pushPos.y(),_pushPos.z(),pitch,yaw);
//   					us.requestRedraw();							
//   					return true;
//   				}
//   			}else if (abs(newtx1 - touchtx1) < 3.0 && abs(newty1 - touchty1) < 3.0)	//第2个点位置不变
//   			{
//   				GlbLogWOutput(GLB_LOGTYPE_INFO,L"rotate\r\n");
//   				//ScenePtToGePt(touchtx1,touchty1,_pushPos);
//   
//   				glbDouble yaw,pitch;
//   				osg::Vec2d old_vec(oldtx0-oldtx1,oldty0-oldty1);
//   				osg::Vec2d new_vec(newtx0-oldtx1,newty0-oldty1);
//   				pitch = (old_vec.length()-new_vec.length())*0.1;
//   				yaw = osg::RadiansToDegrees(atan2(old_vec.y(),old_vec.x())-atan2(new_vec.y(),new_vec.x()));
//   
//   				_ga_t0 = &ea;
//   				if (_pushPos2.x() > 1000.0)
//   				{
//   					return false;
//   				}else{
//   					Rotate(_pushPos2.x(),_pushPos2.y(),_pushPos2.z(),pitch,yaw);
//   					us.requestRedraw();							
//   					return true;
//   				}
//   			}else{
				//GlbLogWOutput(GLB_LOGTYPE_INFO,L"zoom\r\n");
				glbDouble old_distan = sqrt((oldtx0-oldtx1)*(oldtx0-oldtx1) + (oldty0-oldty1)*(oldty0-oldty1));
				glbDouble new_distan = sqrt((newtx0-newtx1)*(newtx0-newtx1) + (newty0-newty1)*(newty0-newty1));
				if (IsFlying())
					StopFlying();
				glbDouble zoomDelta = new_distan/old_distan - 1.0;
				zoomDelta *= 10.0;
				Zoom(zoomDelta);
				_ga_t0 = &ea;
				us.requestRedraw();
				return true;
//			}
		}
	//	switch(ea.getTouchData()->get(0).phase)
	//	{
	//	case osgGA::GUIEventAdapter::TOUCH_BEGAN:
	//		{
	//			if (ea.getTouchData()->get(1).phase == osgGA::GUIEventAdapter::TOUCH_BEGAN)
	//			{
	//				GlbLogWOutput(GLB_LOGTYPE_INFO,L"push2");
	//				if (IsFlying())
	//					StopFlying();
	//				_pushpitch = mpr_pitch;
	//				_pushyaw = mpr_yaw;
	//				_pushMatrix = getMatrix();
	//				osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
	//				_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();
	//				//记录选中点经纬高
	//				glbDouble dx = ea.getTouchData()->get(0).x;
	//				glbDouble dy = ea.getTouchData()->get(0).y;
	//				//窗口坐标系(鼠标位置) 转换为 屏幕坐标,触控版
	//				mpr_globe->GetView()->WindowToScene(dx,dy);
	//				ScenePtToGePt(dx,dy,_pushPos);

	//				//记录选中点经纬高
	//				dx = ea.getTouchData()->get(1).x;
	//				dy = ea.getTouchData()->get(1).y;
	//				//窗口坐标系(鼠标位置) 转换为 屏幕坐标,触控版
	//				mpr_globe->GetView()->WindowToScene(dx,dy);
	//				ScenePtToGePt(dx,dy,_pushPos2);
	//				_ga_t0 = &ea;
	//				_ga_t1 = &ea;
	//				//绘制选中点图标
	//				us.requestRedraw();					
	//				return false;
	//			}else if (ea.getTouchData()->get(1).phase == osgGA::GUIEventAdapter::TOUCH_MOVED)
	//			{
	//				GlbLogWOutput(GLB_LOGTYPE_INFO,L"move2");
	//				if (_ga_t0->getTouchData()->get(0).phase == osgGA::GUIEventAdapter::TOUCH_BEGAN)
	//				{
	//					glbDouble yaw,pitch;
	//					glbDouble or_tx,or_ty,old_tx,old_ty,new_tx,new_ty;
	//					old_tx = _ga_t0->getTouchData()->get(1).x;
	//					old_ty = _ga_t0->getTouchData()->get(1).y;
	//					or_tx = _ga_t0->getTouchData()->get(0).x;
	//					or_ty = _ga_t0->getTouchData()->get(0).y;
	//					new_tx = ea.getTouchData()->get(1).x;
	//					new_tx = ea.getTouchData()->get(1).y;
	//					osg::Vec2d old_vec(old_tx-or_tx,old_ty-or_ty);
	//					osg::Vec2d new_vec(new_tx-or_tx,new_ty-or_ty);
	//					pitch = (old_vec.length()-new_vec.length())*0.3;
	//					yaw = osg::RadiansToDegrees(atan2(old_ty,old_tx)-atan2(new_ty,new_tx));

	//					_ga_t0 = &ea;
	//					if (_pushPos.x() > 1000.0)
	//					{
	//						return false;
	//					}else{
	//						Rotate(_pushPos.x(),_pushPos.y(),_pushPos.z(),pitch,yaw);
	//						us.requestRedraw();							
	//						return true;
	//					}						
	//				}						
	//			}else if (ea.getTouchData()->get(1).phase == osgGA::GUIEventAdapter::TOUCH_ENDED)
	//			{
	//				GlbLogWOutput(GLB_LOGTYPE_INFO,L"release2");
	//				_pushPos2 = mpr_focusPos;	//此时相当于是单点触控的begin

	//				_ga_t0 = &ea;
	//				_ga_t1 = &ea;
	//				if (IsFlying())
	//					StopFlying();
	//				_pushpitch = mpr_pitch;
	//				_pushyaw = mpr_yaw;
	//				_pushMatrix = getMatrix();
	//				osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
	//				_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();
	//				////记录选中点经纬高
	//				//glbDouble dx = ea.getTouchData()->get(0).x;//touchpoint.x;
	//				//glbDouble dy = ea.getTouchData()->get(0).y;
	//				////窗口坐标系(鼠标位置) 转换为 屏幕坐标,触控版
	//				//mpr_globe->GetView()->WindowToScene(dx,dy);
	//				//ScenePtToGePt(dx,dy,_pushPos);
	//				return false;
	//			}
	//		}
	//		break;
	//	case osgGA::GUIEventAdapter::TOUCH_MOVED:
	//		{
	//			if (ea.getTouchData()->get(1).phase == osgGA::GUIEventAdapter::TOUCH_BEGAN)
	//			{
	//				GlbLogWOutput(GLB_LOGTYPE_INFO,L"move3");
	//				if (_ga_t0->getTouchData()->get(1).phase == osgGA::GUIEventAdapter::TOUCH_BEGAN)
	//				{
	//					glbDouble yaw,pitch;
	//					glbDouble or_tx,or_ty,old_tx,old_ty,new_tx,new_ty;
	//					old_tx = _ga_t0->getTouchData()->get(0).x;
	//					old_ty = _ga_t0->getTouchData()->get(0).y;
	//					or_tx = _ga_t0->getTouchData()->get(1).x;
	//					or_ty = _ga_t0->getTouchData()->get(1).y;
	//					new_tx = ea.getTouchData()->get(0).x;
	//					new_tx = ea.getTouchData()->get(0).y;
	//					osg::Vec2d old_vec(old_tx-or_tx,old_ty-or_ty);
	//					osg::Vec2d new_vec(new_tx-or_tx,new_ty-or_ty);
	//					pitch = (old_vec.length()-new_vec.length())*0.3;
	//					yaw = osg::RadiansToDegrees(atan2(old_vec.y(),old_vec.x())-atan2(new_vec.y(),new_vec.x()));

	//					_ga_t0 = &ea;
	//					if (_pushPos2.x() > 1000.0)
	//					{
	//						return false;
	//					}else{
	//						Rotate(_pushPos2.x(),_pushPos2.y(),_pushPos2.z(),pitch,yaw);
	//						us.requestRedraw();							
	//						return true;
	//					}
	//				}
	//			}else if (ea.getTouchData()->get(1).phase == osgGA::GUIEventAdapter::TOUCH_MOVED)	//2点都移动，拉近，拉远，zoom
	//			{
	//				GlbLogWOutput(GLB_LOGTYPE_INFO,L"move");
	//				//第0点
	//				glbDouble oldtx0 = _ga_t0->getTouchData()->get(0).x;
	//				glbDouble oldty0 = _ga_t0->getTouchData()->get(0).y;
	//				glbDouble newtx0 = ea.getTouchData()->get(0).x;
	//				glbDouble newty0 = ea.getTouchData()->get(0).y;
	//				mpr_globe->GetView()->WindowToScene(oldtx0,oldty0);
	//				mpr_globe->GetView()->WindowToScene(newtx0,newty0);
	//				//第1点
	//				glbDouble oldtx1 = _ga_t0->getTouchData()->get(1).x;
	//				glbDouble oldty1 = _ga_t0->getTouchData()->get(1).y;
	//				glbDouble newtx1 = ea.getTouchData()->get(1).x;
	//				glbDouble newty1 = ea.getTouchData()->get(1).y;
	//				mpr_globe->GetView()->WindowToScene(oldtx1,oldty1);
	//				mpr_globe->GetView()->WindowToScene(newtx1,newty1);
	//				//rotate
	//				if (abs(newtx0 - oldtx0) < 1.0 && abs(newty0 - oldty0) < 1.0)	//第1个点位置不变
	//				{
	//					ScenePtToGePt(oldtx0,oldty0,_pushPos);

	//					glbDouble yaw,pitch;
	//					osg::Vec2d old_vec(oldtx1-oldtx0,oldty1-oldty0);
	//					osg::Vec2d new_vec(newtx1-oldtx0,newty1-oldty0);
	//					pitch = (old_vec.length()-new_vec.length())*0.3;
	//					yaw = osg::RadiansToDegrees(atan2(old_vec.y(),old_vec.x())-atan2(new_vec.y(),new_vec.x()));

	//					_ga_t0 = &ea;
	//					if (_pushPos.x() > 1000.0)
	//					{
	//						return false;
	//					}else{
	//						Rotate(_pushPos.x(),_pushPos.y(),_pushPos.z(),pitch,yaw);
	//						us.requestRedraw();							
	//						return true;
	//					}
	//				}else if (abs(newtx1 - oldtx1) < 1.0 && abs(newty1 - oldty1) < 1.0)	//第2个点位置不变
	//				{
	//					ScenePtToGePt(oldtx1,oldty1,_pushPos);

	//					glbDouble yaw,pitch;
	//					osg::Vec2d old_vec(oldtx0-oldtx1,oldty0-oldty1);
	//					osg::Vec2d new_vec(newtx0-oldtx1,newty0-oldty1);
	//					pitch = (old_vec.length()-new_vec.length())*0.3;
	//					yaw = osg::RadiansToDegrees(atan2(old_vec.y(),old_vec.x())-atan2(new_vec.y(),new_vec.x()));

	//					_ga_t0 = &ea;
	//					if (_pushPos2.x() > 1000.0)
	//					{
	//						return false;
	//					}else{
	//						Rotate(_pushPos2.x(),_pushPos2.y(),_pushPos2.z(),pitch,yaw);
	//						us.requestRedraw();							
	//						return true;
	//					}
	//				}else{
	//					glbDouble old_distan = sqrt((oldtx0-oldtx1)*(oldtx0-oldtx1) + (oldty0-oldty1)*(oldty0-oldty1));
	//					glbDouble new_distan = sqrt((newtx0-newtx1)*(newtx0-newtx1) + (newty0-newty1)*(newty0-newty1));
	//					if (IsFlying())
	//						StopFlying();
	//					glbDouble zoomDelta = new_distan/old_distan - 1.0;
	//					Zoom(zoomDelta);
	//					_ga_t0 = &ea;
	//					us.requestRedraw();
	//					return true;
	//				}
	//			}else if (ea.getTouchData()->get(1).phase == osgGA::GUIEventAdapter::TOUCH_ENDED)
	//			{
	//				GlbLogWOutput(GLB_LOGTYPE_INFO,L"release3");
	//				_pushPos2 = mpr_focusPos;	//此时相当于是单点触控的begin

	//				_ga_t0 = &ea;
	//				_ga_t1 = &ea;
	//				if (IsFlying())
	//					StopFlying();
	//				_pushpitch = mpr_pitch;
	//				_pushyaw = mpr_yaw;
	//				_pushMatrix = getMatrix();
	//				osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
	//				_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();
	//				////记录选中点经纬高
	//				//glbDouble dx = ea.getTouchData()->get(0).x;//touchpoint.x;
	//				//glbDouble dy = ea.getTouchData()->get(0).y;
	//				////窗口坐标系(鼠标位置) 转换为 屏幕坐标,触控版
	//				//mpr_globe->GetView()->WindowToScene(dx,dy);
	//				//ScenePtToGePt(dx,dy,_pushPos);
	//				return false;
	//			}
	//		}
	//		break;
	//	case osgGA::GUIEventAdapter::TOUCH_ENDED:
	//		{
	//			GlbLogWOutput(GLB_LOGTYPE_INFO,L"releaseall");

	//			_ga_t0 = &ea;
	//			_ga_t1 = &ea;
	//			if (IsFlying())
	//				StopFlying();
	//			_pushpitch = mpr_pitch;
	//			_pushyaw = mpr_yaw;
	//			_pushMatrix = getMatrix();
	//			osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
	//			_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();
	//			////记录选中点经纬高
	//			glbDouble dx = ea.getTouchData()->get(1).x;//touchpoint.x;
	//			glbDouble dy = ea.getTouchData()->get(1).y;
	//			//窗口坐标系(鼠标位置) 转换为 屏幕坐标,触控版
	//			mpr_globe->GetView()->WindowToScene(dx,dy);
	//			ScenePtToGePt(dx,dy,_pushPos);
	//			return false;
	//		}
	//		break;
	//	default:
	//		break;
	//	}
	//	_ga_t0 = &ea;
	//	return false;
	}
	_ga_t0 = &ea;
	return false;
}

glbBool CGlbGlobeManipulator::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::FRAME):
		{
			if (_FlyCallback.valid() && (IsFlying() || _FlyCallback->IslastFrame()))
			{ 
				if (_FlyCallback->getAnimationTime() > 0.0)		//防止第一帧跳帧
				{ 
					osg::Matrixd mat = _FlyCallback->getMatrix();
					setByMatrix(mat); 
					//相机纠偏
					if (_FlyCallback->IslastFrame())
						_FlyCallback->resetlastFrame();
					mpr_focus_temp = _FlyCallback->getfocus();
					osg::Vec3d cameraPos;
					GetCameraPos(cameraPos);
					glbDouble elevate = 0.0;
					if (mpr_globe->GetView()->getUpdateCameraMode())
						elevate = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
					else
						elevate = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
					UpdateCameraElevate2(elevate);
					GetCameraPos(cameraPos);
					if (cameraPos.z() > elevate)
						mpr_isUnderGround = false;
					else
						mpr_isUnderGround = true;
					mpr_globe->NotifyCameraIsUnderground(mpr_isUnderGround);	
					us.requestRedraw();					
					return true; 
				}
				if (_FlyCallback->IslastFrame())
				{
					//焦点纠偏
					setByMatrix(getMatrix());
					osg::Vec3d cameraPos;
					GetCameraPos(cameraPos);
					glbDouble elevate = 0.0;
					if (mpr_globe->GetView()->getUpdateCameraMode())
						elevate = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
					else
						elevate = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
					UpdateCameraElevate2(elevate);
					GetCameraPos(cameraPos);
					if (cameraPos.z() > elevate)
						mpr_isUnderGround = false;
					else
						mpr_isUnderGround = true;
					mpr_globe->NotifyCameraIsUnderground(mpr_isUnderGround);
					_FlyCallback->resetlastFrame();
					us.requestRedraw();	
					return true;
				}
			}

			if (mpr_isShake)
			{// 相机震动		
				if (mpr_shakeStartTime < 0)// 记录初始时间 get time in seconds of event
				{
					mpr_shakeStartTime = ea.getTime();
					//mpr_shakeStartTime = us.asView()->getFrameStamp()->getSimulationTime();
				}
				// 判断时间是否超出shaketime
				double tim =  ea.getTime();//us.asView()->getFrameStamp()->getSimulationTime();
				if (tim - mpr_shakeStartTime > mpr_shakeTime && mpr_shakeTime > 0)
				{// 停止
					Shake(false);
				}
				else
				{
					static int frameNum = 0;				
					if (frameNum % 2 == 1)
					{// 定时回位
						mpr_pitch = mpr_orignPitch;
						mpr_yaw = mpr_orignyaw;
						mpr_focusPos = mpr_shakefocusPos; 
						mpr_distance = mpr_shakeDistance;
					}
					else
					{// 晃动
						// rand() / double(RAND_MAX)可以取得0～1之间的浮点数
						// (0 , 2)度之间抖动
						float shakeVerticalOffset =  - (rand() / double(RAND_MAX)) * mpr_shakeVerDegree;				
						mpr_pitch = mpr_orignPitch + shakeVerticalOffset;
						// (-1 , 1)度之间抖动
						float shakeHorizonOffset = ( rand() / double(RAND_MAX) - 0.5) * mpr_shakeHorDegree;
						mpr_yaw = mpr_orignyaw + shakeHorizonOffset;
						// focuse pos 上下抖动 [0 - 1]
						double maxDis = mpr_shakeDistance * tan(osg::DegreesToRadians(2.0)); // mpr_shakeVerDegree
						float z = mpr_shakefocusPos.z() + maxDis * ( rand() / double(RAND_MAX)) * 1.0;
						mpr_focusPos.set(mpr_shakefocusPos.x(),mpr_shakefocusPos.y(),z);
					}
					frameNum++;
					if (frameNum==10000)
						frameNum = 0;			
				}	
			}

			//相机纠偏
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			//glbBool isUpdate = UpdateCameraElevate(mpr_globe->GetView()->GetTerrainElevationOfCameraPos());
			glbDouble elevate = 0.0;
			if (mpr_globe->GetView()->getUpdateCameraMode())
				elevate = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
			else
				elevate = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			glbBool isUpdate = UpdateCameraElevate(elevate);
			GetCameraPos(cameraPos);
			if (cameraPos.z() > elevate)
				mpr_isUnderGround = false;
			else
				mpr_isUnderGround = true;
			mpr_globe->NotifyCameraIsUnderground(mpr_isUnderGround);	

			if (isUpdate)
				us.requestRedraw();
			return true;
		}		
	case(osgGA::GUIEventAdapter::RESIZE):
		{ 
			init(ea,us);
			us.requestRedraw();
			return true;
		}
	default:
		break;
	}

	if (ea.getHandled()) 
		return false;

	if (ea.getTouchData())
	{
		return handleTouch(ea,us);
	}

	switch (ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::PUSH):	//鼠标按下,将当前状态保存到_ga_t1里
		{
			_ga_t0 = &ea;
			_ga_t1 = &ea;
			if (_ga_t0->getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON || _ga_t0->getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)	//滚轮或者左键按下
			{ 	
				if (IsFlying())
					StopFlying();
				// 记录鼠标按下时的相机矩阵和VPW矩阵以及鼠标按下时的pitch和yaw角,以备使用
				_pushpitch = mpr_pitch;
				_pushyaw = mpr_yaw;
				_pushMatrix = getMatrix();
				osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
				_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();

				//记录选中点经纬高
				glbDouble dx = _ga_t1->getX();
				glbDouble dy = _ga_t1->getY();				
				//窗口坐标系(鼠标位置) 转换为 屏幕坐标
				//计算与地面或地下参考面的交点
				glbBool ischoose = mpr_globe->ScreenToTerrainCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());
				if (!ischoose)
				{// 与地面没有交点则跟地下参考面求交
					if (mpr_globe->IsUnderGroundMode())	  //开启地下模式 					
						ischoose = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());					
				}
				
				osg::Vec3d pt11_w;
				glbDouble ptx1_s = dx,pty1_s = dy;
				mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
				//计算在此位置能够拣选到的最近对象和交点
				osg::Vec3d InterPoint;
				glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx1_s,pty1_s,InterPoint);

				mpr_isPushPicked = true;
				if (!ischoose && !isObjfocus)
				{// 跟对象和地面/地下参考面都没有交集
					mpr_isPushPicked = false;
				}
				else if (isObjfocus && ischoose)
				{// 比较对象上的pick点和地面/地下参考面的pick点哪个离相机近，就选用哪个点做pushPos			
					osg::Vec3d pushPos;
					g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_pushPos.y()),osg::DegreesToRadians(_pushPos.x()),_pushPos.z(),pushPos.x(),pushPos.y(),pushPos.z());

					osg::Vec3d cameraPos_w = getMatrix().getTrans();
					glbDouble dis1 = (cameraPos_w - InterPoint).length();
					glbDouble dis2 = (cameraPos_w - pushPos).length();
					if (dis1 < dis2)
					{// 对象上的pick点离相机近
						//_pushPos = InterPoint;
						g_ellipsmodel->convertXYZToLatLongHeight(InterPoint.x(),InterPoint.y(),InterPoint.z(),_pushPos.y(),_pushPos.x(),_pushPos.z());
						RadToDeg(_pushPos);
					}
				}
				else if (isObjfocus && !ischoose)
				{// 只跟对象有交点
					//_pushPos = InterPoint;
					g_ellipsmodel->convertXYZToLatLongHeight(InterPoint.x(),InterPoint.y(),InterPoint.z(),_pushPos.y(),_pushPos.x(),_pushPos.z());
					RadToDeg(_pushPos);
				}			
				//绘制选中点图标
				us.requestRedraw();
				return true;
			}
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::RELEASE):	//鼠标释放,
		{
			flushMouseEventStack();
			addMouseEvent(ea);
			_pushMatrix.makeIdentity();
			_pushVPW.makeIdentity();
			_pushPos = osg::Vec3d(DBL_MAX,DBL_MAX,DBL_MAX);
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::DOUBLECLICK):	//双击
		{ 
			_ga_t0 = &ea;
			if (_ga_t0->getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)	//左键双击
			{ 
				if (IsFlying())
					StopFlying(); 
				glbDouble dx = _ga_t0->getX();
				glbDouble dy = _ga_t0->getY();
				//窗口坐标系(鼠标位置) 转换为 屏幕坐标
				//mpr_globe->GetView()->WindowToScreen(dx,dy);
				osg::Vec3d pt11_w;
				glbDouble ptx1_s = dx,pty1_s = dy;
				mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
				//glbBool isObjfocus = mpr_globe->GetView()->Pick(ptx1_s,pty1_s,results,InterPoints);
				osg::Vec3d InterPoint;
				glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx1_s,pty1_s,InterPoint);
				if (isObjfocus)
				{
					//CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(results[0].get());
					//if (mpr_globe->IsUnderGroundMode())
						pt11_w = InterPoint;
					//else
					//	if (robj && robj->IsGround())
					//		pt11_w = InterPoints[0];	
				}

				glbDouble lon = 0.0;
				glbDouble lat = 0.0;
				glbDouble alt = 0.0;
				glbBool isclick = mpr_globe->ScreenToTerrainCoordinate(dx,dy,lon,lat,alt);
				glbDouble seconds = 1.0;
				if (!isclick)
				{  
					if (mpr_globe->IsUnderGroundMode())	//开启地下模式
					{
						isclick = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,lon,lat,alt);
						if (!isclick  && !isObjfocus)
							return false;
					}else if (!isObjfocus)
						return false;
				}
				glbDouble groundlon = lon,groundlat = lat,groundalt = alt;
				glbDouble distan = 0.0;	
				osg::Vec3d pt1_w;
				osg::Vec3d oldcameraPos_w = getMatrix().getTrans();
				g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),alt,pt1_w.x(),pt1_w.y(),pt1_w.z());
				glbDouble dis1 = (oldcameraPos_w - pt1_w).length();
				distan = dis1;
				if (isObjfocus)
				{					
					glbDouble dis2 = (oldcameraPos_w - pt11_w).length();
					if (dis2 < dis1)
					{			
						g_ellipsmodel->convertXYZToLatLongHeight(pt11_w.x(),pt11_w.y(),pt11_w.z(),lat,lon,alt);
						lon = osg::RadiansToDegrees(lon);
						lat = osg::RadiansToDegrees(lat);
						distan = min(dis1,dis2);
						//if (isclick)
							//GlbLogWOutput(GLB_LOGTYPE_INFO,L"第1点:(%.5f,%.5f,%.5f),第2点:(%.5f,%.5f,%.5f) \r\n",lon,lat,alt,groundlon,groundlat,groundalt);
					}
				}				
				if (mpr_lockmode == GLB_LOCKMODE_2D || mpr_lockmode == GLB_LOCKMODEL_2DN)
				{
					//if (distan > 0)
						FlyTo2(lon,lat,alt,0.33*distan,mpr_yaw,mpr_pitch,seconds);
					//else
					//	FlyTo(lon,lat,alt,0.33*dis1,mpr_yaw,mpr_pitch,seconds);
					us.requestRedraw();
					return true;
				}
				//计算目标pitch角
				osg::Vec3d cameraPos = getMatrix().getTrans();
				osg::Vec3d targetPos;
				g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),alt,targetPos.x(),targetPos.y(),targetPos.z());
				osg::Vec3d lookvec = targetPos - cameraPos;
				lookvec.normalize();
				//targetPos处向上向量
				osg::Vec3d up(cos(osg::DegreesToRadians(lon))*cos(osg::DegreesToRadians(lat)), sin(osg::DegreesToRadians(lon))*cos(osg::DegreesToRadians(lat)), sin(osg::DegreesToRadians(lat)));
				up.normalize();
				glbDouble tempnomal = lookvec*up;
				tempnomal = max(min(tempnomal,1.0),-1.0);
				glbDouble pitch = -osg::RadiansToDegrees(acos(tempnomal)) + 90; 
				//if (distan > 0)
					FlyTo2(lon,lat,alt,0.33*distan,mpr_yaw,pitch,seconds);
				//else
				//	FlyTo(lon,lat,alt,0.33*dis1,mpr_yaw,pitch,seconds);
				us.requestRedraw();
				return true;
			}else if (ea.getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) //右键双击
			{
				if (IsFlying())
					StopFlying();
				if (mpr_lockmode == GLB_LOCKMODE_2D || mpr_lockmode == GLB_LOCKMODEL_2DN)
					return false;
				glbDouble dx = _ga_t0->getX();
				glbDouble dy = _ga_t0->getY();
				//窗口坐标系(鼠标位置) 转换为 屏幕坐标
				//mpr_globe->GetView()->WindowToScreen(dx,dy);
				osg::Vec3d pt11_w;
				glbDouble ptx1_s = dx,pty1_s = dy;
				mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
				//glbBool isObjfocus = mpr_globe->GetView()->Pick(ptx1_s,pty1_s,results,InterPoints);
				osg::Vec3d InterPoint;
				glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx1_s,pty1_s,InterPoint);
				if (isObjfocus)
				{
					//CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(results[0].get());
					//if (mpr_globe->IsUnderGroundMode())
						pt11_w = InterPoint;
					//else
					//	if (robj && robj->IsGround())
					//		pt11_w = InterPoints[0];	
				}

				glbDouble lon = 0.0;
				glbDouble lat = 0.0;
				glbDouble alt = 0.0;
				glbBool isclick = mpr_globe->ScreenToTerrainCoordinate(dx,dy,lon,lat,alt);
				glbDouble seconds = 10.0;
				if (!isclick) 
				{ 
					if (mpr_globe->IsUnderGroundMode())	//开启地下模式
					{
						glbBool isugclick = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,lon,lat,alt);
						if (!isugclick && !isObjfocus)
							return false;
					}else if (!isObjfocus)
						return false;
				}
				if (isObjfocus)
				{
					osg::Vec3d oldcameraPos_w = getMatrix().getTrans();
					osg::Vec3d pt1_w;
					g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),alt,pt1_w.x(),pt1_w.y(),pt1_w.z());
					glbDouble dis1 = (oldcameraPos_w - pt1_w).length();
					glbDouble dis2 = (oldcameraPos_w - pt11_w).length();
					if (dis2 < dis1)
					{
						g_ellipsmodel->convertXYZToLatLongHeight(pt11_w.x(),pt11_w.y(),pt11_w.z(),lat,lon,alt);
						lon = osg::RadiansToDegrees(lon);
						lat = osg::RadiansToDegrees(lat);
					}
				}
				FlyAround(lon,lat,alt,seconds,true);
				us.requestRedraw();
				return true;
			}
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::DRAG):
		{
			if (IsFlying())
				StopFlying();
			if (_ga_t1 == NULL)
				return false;
			//if (_pushPos.x() > 100000.0)
			//	return false;
			if (_ga_t1->getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)		//滚轮按下拖拽
			{
				//获取拖拽的向量				
				glbDouble dx = ea.getX() - _ga_t0->getX();
				glbDouble dy = ea.getY() - _ga_t0->getY();
				_ga_t0 = &ea;
				glbDouble tempd = 1.0;
				if (mpr_isctrldown)
					tempd = 0.1;
				else
					tempd = 0.3;
				glbDouble yaw = -dx*tempd;	//系数暂定0.3
				glbDouble pitch = dy*tempd;
				 
				Rotate(_pushPos.x(),_pushPos.y(),_pushPos.z(),pitch,yaw);	
				us.requestRedraw();
				return true;
			}
			else if (_ga_t1->getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)	//左键按下拖拽
			{
				//获取左键按下时刻鼠标位置点在地形上的3维坐标
				glbDouble dx0 = _ga_t1->getX();
				glbDouble dy0 = _ga_t1->getY();
				//获取左键按下时，鼠标位置
				glbDouble dx = ea.getX();
				glbDouble dy = ea.getY();
				//窗口坐标系(鼠标位置) 转换为 屏幕坐标
				//mpr_globe->GetView()->WindowToScreen(dx0,dy0);

				//获取左键按下后，当前鼠标位置
				_ga_t0 = &ea;
				//glbDouble dx = _ga_t0->getX();
				//glbDouble dy = _ga_t0->getY();
				//窗口坐标系(鼠标位置) 转换为 屏幕坐标
				//mpr_globe->GetView()->WindowToScreen(dx,dy);
				Drag(dx0,dy0,dx,dy);
				us.requestRedraw();
				return true;
			}
		}
		break;
	case (osgGA::GUIEventAdapter::MOVE):
		{
			_ga_t0 = &ea;
			return false;
		}		
		break;
	case (osgGA::GUIEventAdapter::KEYDOWN):
		{
			if (handleKeyDown(ea,us))
			{
				us.requestRedraw();
				return true;
			}
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::KEYUP):
		{
			switch(ea.getKey())
			{
			case (osgGA::GUIEventAdapter::KEY_Control_L):	//松开control键，所有旋转操作速度变慢
			case (osgGA::GUIEventAdapter::KEY_Control_R):
				{
					mpr_isctrldown = false;
					return true;
				}
				break;
			case (osgGA::GUIEventAdapter::KEY_Shift_L):	//松开Shift键，所有旋转操作速度变慢
			case (osgGA::GUIEventAdapter::KEY_Shift_R):
				{
					mpr_isshiftdown = false;
					return true;
				}
				break;
			default:
				break;
			}
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::SCROLL):	//滚轮滚动
		{
			if (IsFlying())
				StopFlying();
			if (ea.getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)	//滚轮按下
				return false;
			_ga_t0 = &ea;
			glbDouble zoomDelta = _ga_t0->getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP ? 1.0 : -1.0;
			Zoom(zoomDelta);
			us.requestRedraw();
			return true;
		}
		break;
	default: 
		break;
	}
	return false;
}

void CGlbGlobeManipulator::flushMouseEventStack()
{
	_ga_t1 = NULL;
	_ga_t0 = NULL;
}

void CGlbGlobeManipulator::addMouseEvent(const osgGA::GUIEventAdapter& ea)
{
	_ga_t1 = _ga_t0;
	_ga_t0 = &ea;
}

bool CGlbGlobeManipulator::isMouseMoving()
{
	if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

	const float velocity = 0.1f;

	float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();
	float len = sqrtf(dx*dx+dy*dy);
	float dt = _ga_t0->getTime()-_ga_t1->getTime();

	return (len>dt*velocity);
}

glbBool CGlbGlobeManipulator::handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
	switch(ea.getKey())
	{
#ifdef _DEBUG
	//case (osgGA::GUIEventAdapter::KEY_1):		
	//	{
	//		if (IsFlying())
	//			StopFlying();
	//		SetLockMode(GLB_LOCKMODE_FREE);
	//		us.requestRedraw();
	//		return true;
	//	} 
	//	break;
	//case (osgGA::GUIEventAdapter::KEY_2):		
	//	{
	//		if (IsFlying())
	//			StopFlying();
	//		SetLockMode(GLB_LOCKMODE_2D);
	//		us.requestRedraw();
	//		return true;
	//	}
	//	break;
	//case (osgGA::GUIEventAdapter::KEY_3):		
	//	{
	//		if (IsFlying())
	//			StopFlying();
	//		SetLockMode(GLB_LOCKMODEL_2DN);
	//		us.requestRedraw();
	//		return true;
	//	}
	//	break;
	//case (osgGA::GUIEventAdapter::KEY_J):		//focus	yaw 减少0.5
	//	{
	//		if (mpr_lockmode == GLB_LOCKMODEL_2DN)
	//			return false;
	//		_ga_t0 = &ea;
	//		Rotate(mpr_focusPos.x(),mpr_focusPos.y(),mpr_focusPos.z(),0,-0.5);
	//		us.requestRedraw();
	//		return true;
	//	}
	//	break;
	//case (osgGA::GUIEventAdapter::KEY_L):		//focus	yaw 增加0.5
	//	{
	//		if (mpr_lockmode == GLB_LOCKMODEL_2DN)
	//			return false;
	//		_ga_t0 = &ea;
	//		Rotate(mpr_focusPos.x(),mpr_focusPos.y(),mpr_focusPos.z(),0,0.5);
	//		us.requestRedraw();
	//		return true;
	//	}
	//	break;
	//case (osgGA::GUIEventAdapter::KEY_I):		//focus	pitch 减少0.2
	//	{
	//		if (mpr_lockmode == GLB_LOCKMODEL_2DN || mpr_lockmode == GLB_LOCKMODE_2D)
	//			return false;
	//		_ga_t0 = &ea;
	//		Rotate(mpr_focusPos.x(),mpr_focusPos.y(),mpr_focusPos.z(),-0.2,-0);
	//		us.requestRedraw();
	//		return true;
	//	}
	//	break;
	//case (osgGA::GUIEventAdapter::KEY_K):		//focus	pitch 增加0.2
	//	{
	//		if (mpr_lockmode == GLB_LOCKMODEL_2DN || mpr_lockmode == GLB_LOCKMODE_2D)
	//			return false;
	//		_ga_t0 = &ea;
	//		Rotate(mpr_focusPos.x(),mpr_focusPos.y(),mpr_focusPos.z(),0.2,0);
	//		us.requestRedraw();
	//		return true;
	//	}
	//	break;
#endif
	case (osgGA::GUIEventAdapter::KEY_F1):
		{
			mpr_globe->GetView()->setUpdateCameraMode(false);
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_F2):
		{
			mpr_globe->GetView()->setUpdateCameraMode(true);
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_F3):
		{
			mpr_isDetectCollision = !mpr_isDetectCollision;
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Control_L):	//按下control键，所有旋转操作速度变慢
	case (osgGA::GUIEventAdapter::KEY_Control_R):
		{
			mpr_isctrldown = true;
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Shift_L):	//按下Shift键，所有旋转操作速度变慢
	case (osgGA::GUIEventAdapter::KEY_Shift_R):
		{
			mpr_isshiftdown = true;
			return false;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Space):	//空格
		{
			if (IsFlying())
				StopFlying(); 
			_ga_t0 = &ea;
			home(ea,us);
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Home):	//yaw = 0
		{
			if (IsFlying())
				StopFlying(); 
			if (mpr_lockmode == GLB_LOCKMODEL_2DN)
				return false;
			_ga_t0 = &ea;
			SetCameraYaw(0);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_End):	//yaw = 180
		{
			if (IsFlying())
				StopFlying(); 
			if (mpr_lockmode == GLB_LOCKMODEL_2DN)
				return false;
			_ga_t0 = &ea;
			SetCameraYaw(180);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Page_Up):	//pitch = 0
		{
			if (IsFlying())
				StopFlying(); 
			if (mpr_lockmode == GLB_LOCKMODEL_2DN || mpr_lockmode == GLB_LOCKMODE_2D)
				return false;
			_ga_t0 = &ea;
			SetCameraPitch(0);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Page_Down):	//pitch = -90
		{
			if (IsFlying())
				StopFlying(); 
			if (mpr_lockmode == GLB_LOCKMODEL_2DN || mpr_lockmode == GLB_LOCKMODE_2D)
				return false;
			_ga_t0 = &ea;
			SetCameraPitch(-90);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Left):		//相机yaw角减小1.0°
		{  			
			if (IsFlying())
				StopFlying();
			if (mpr_lockmode == GLB_LOCKMODEL_2DN)
				return false; 
			_ga_t0 = &ea;
			glbDouble tempd = 1.0;
			if (mpr_isctrldown)
				tempd = 0.3;
			else
				tempd = 1.0;
			RotateCameraYaw(-tempd);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Right):		//相机yaw角增加1.0°
		{			
			if (IsFlying())	
				StopFlying();
			if (mpr_lockmode == GLB_LOCKMODEL_2DN)
				return false; 
			_ga_t0 = &ea;
			glbDouble tempd = 1.0;
			if (mpr_isctrldown)
				tempd = 0.3;
			else
				tempd = 1.0;
			RotateCameraYaw(tempd);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Up):			//相机pitch角增加0.5°
		{				
			if (IsFlying())
				StopFlying();
			if (mpr_lockmode == GLB_LOCKMODEL_2DN || mpr_lockmode == GLB_LOCKMODE_2D)
				return false; 
			_ga_t0 = &ea;
			glbDouble tempd = 1.0;
			if (mpr_isctrldown)
				tempd = 0.3;
			else
				tempd = 1.0;
			RotateCameraPitch(tempd*0.5);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Down):		//相机pitch角减少0.5°
		{
			if (IsFlying())
				StopFlying();
			if (mpr_lockmode == GLB_LOCKMODEL_2DN || mpr_lockmode == GLB_LOCKMODE_2D)
				return false;
			_ga_t0 = &ea;
			glbDouble tempd = 1.0;
			if (mpr_isctrldown)
				tempd = 0.3;
			else
				tempd = 1.0;
			RotateCameraPitch(-tempd*0.5);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_W):	
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			glbDouble angle = getMoveCoefficient(cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos());
			Move(0,angle);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_S):	
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			glbDouble angle = getMoveCoefficient(cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos());
			Move(0,-angle);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_A):	
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			glbDouble angle = getMoveCoefficient(cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos());
			Move(angle,0);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_D):	
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			glbDouble angle = getMoveCoefficient(cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos());
			Move(-angle,0);
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_Z):		//相机、焦点海拔拉低,20%
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			glbDouble tempd = 1.0;
			if (mpr_isctrldown)
				tempd = 0.1;
			else
				tempd = 0.2;
			glbDouble terrainElevationOfCameraPos = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			osg::Vec3d cameraPos;
			osg::Vec3d cameraPos_w;
			osg::Matrixd mat = getMatrix();
			cameraPos_w = mat.getTrans();
			g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
			osg::Vec3d newcameraPos_w;
			if (abs(cameraPos.z() - terrainElevationOfCameraPos) < 0.1)
			{
				g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),cameraPos.z() - abs(cameraPos.z()-terrainElevationOfCameraPos)-0.1,newcameraPos_w.x(),newcameraPos_w.y(),newcameraPos_w.z());
			}			
			g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),cameraPos.z() - abs(cameraPos.z() - terrainElevationOfCameraPos)*tempd,newcameraPos_w.x(),newcameraPos_w.y(),newcameraPos_w.z());
			mat.postMultTranslate(newcameraPos_w - cameraPos_w);
			setByMatrix(mat);
			//相机纠偏
			//UpdateCameraElevate();
			us.requestRedraw();
			return true;
		}
		break;
	case (osgGA::GUIEventAdapter::KEY_C):		//相机海拔拉高20%
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			glbDouble tempd = 1.0;
			if (mpr_isctrldown)
				tempd = 0.1;
			else
				tempd = 0.2;
			glbDouble terrainElevationOfCameraPos = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			osg::Vec3d cameraPos;
			osg::Vec3d cameraPos_w;
			osg::Matrixd mat = getMatrix();
			cameraPos_w = mat.getTrans();
			g_ellipsmodel->convertXYZToLatLongHeight(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),cameraPos.y(),cameraPos.x(),cameraPos.z());
			if (cameraPos.z() > MaxCameraPosZ)
			{
				return false;
			}
			osg::Vec3d newcameraPos_w;
			if (abs(cameraPos.z() - terrainElevationOfCameraPos) < 0.1)
			{
				g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),cameraPos.z() + abs(cameraPos.z()-terrainElevationOfCameraPos)+0.1,newcameraPos_w.x(),newcameraPos_w.y(),newcameraPos_w.z());
			}
			g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),cameraPos.z() + abs(cameraPos.z() - terrainElevationOfCameraPos)*tempd,newcameraPos_w.x(),newcameraPos_w.y(),newcameraPos_w.z());
			mat.postMultTranslate(newcameraPos_w - cameraPos_w);
			setByMatrix(mat);
			//相机纠偏
			//UpdateCameraElevate();
			us.requestRedraw();
			return true;
		}
		break;
	//case (osgGA::GUIEventAdapter::KEY_K):
	//	{
	//		mpr_shakeTime = 10;
	//		Shake(!mpr_isShake);
	//	}
	//	break;
	default: 
		break;
	}
	return false;
}

/**
	* @brief 构造函数
**/
CGlbGlobeManipulator::CGlbGlobeFlyCallback::CGlbGlobeFlyCallback():
	_timeOffset(0.0),
	_timeMultiplier(1.0),
	_firstTime(DBL_MAX),
	_latestTime(0.0),
	_pause(false),
	_pauseTime(0.0),
	_islastFrame(false){
		_matrix.makeIdentity();
		mpr_flag = 0;
	}
/**
	* @brief 构造函数
**/
CGlbGlobeManipulator::CGlbGlobeFlyCallback::CGlbGlobeFlyCallback(/*CGlbGlobeManipulator* manipulator,*/KeyPoint pt0,KeyPoint pt1,glbDouble loopTime,glbBool isUnderMode):
	_loopTime(loopTime),
	//_manipulator(manipulator),
	_timeOffset(0.0),
	_timeMultiplier(1.0),
	_firstTime(DBL_MAX),
	_latestTime(0.0),
	_pause(false),
	_pauseTime(0.0),
	_islastFrame(false),
	_isUnder(isUnderMode){
		_firstPt = pt0;
		_lastPt = pt1;
		_matrix.makeIdentity();
		_isfar = false;
		mpr_flag = 1;
		if (abs(pt1.focusPos.x() -pt0.focusPos.x()) > 1 || abs(pt1.focusPos.y() -pt0.focusPos.y()) > 1)
			_isfar = true;	
		//求cameraPos
		osg::Matrixd focusToWorld;
		g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(pt0.focusPos.y()),osg::DegreesToRadians(pt0.focusPos.x()),pt0.focusPos.z(),focusToWorld);
		osg::Matrixd mat = osg::Matrixd::translate(0.0,0.0,pt0.distan)*osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pt0.pitch),1.0,0.0,0.0)*osg::Matrixd::rotate(osg::DegreesToRadians(-pt0.yaw),0.0,0.0,1.0)*focusToWorld;
		_firstcameraPos = mat.getTrans();
		g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(pt1.focusPos.y()),osg::DegreesToRadians(pt1.focusPos.x()),pt1.focusPos.z(),focusToWorld);
		osg::Matrixd mat2 = osg::Matrixd::translate(0.0,0.0,pt1.distan)*osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pt1.pitch),1.0,0.0,0.0)*osg::Matrixd::rotate(osg::DegreesToRadians(-pt1.yaw),0.0,0.0,1.0)*focusToWorld;
		_lastcameraPos = mat2.getTrans();

 	//	osg::Matrixd focusToWorld;
 	//	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(pt0.focusPos.y()),osg::DegreesToRadians(pt0.focusPos.x()),pt0.focusPos.z(),focusToWorld);
 	//	osg::Matrixd mat = osg::Matrixd::translate(0.0,0.0,pt0.distan)*osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pt0.pitch),1.0,0.0,0.0)*osg::Matrixd::rotate(osg::DegreesToRadians(-pt0.yaw),0.0,0.0,1.0)*focusToWorld;
 	//	_firstcameraPos = mat.getTrans();
 	//	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt0.focusPos.y()),osg::DegreesToRadians(pt0.focusPos.x()),pt0.focusPos.z(),_firstPt.focusPos.x(),_firstPt.focusPos.y(),_firstPt.focusPos.z());
 	//	_firstLookat = _firstPt.focusPos - _firstcameraPos;
 	//	//_firstLookat = -_firstcameraPos * osg::Matrixd::inverse(focusToWorld);
 	//	_firstPt.distan = min(pt1.distan*3.0,_firstPt.distan);
 	//	_firstPt.focusPos = _firstcameraPos+_firstLookat*(_firstPt.distan/_firstLookat.length());
 	//	g_ellipsmodel->convertXYZToLatLongHeight(_firstPt.focusPos.x(),_firstPt.focusPos.y(),_firstPt.focusPos.z(),_firstPt.focusPos.y(),_firstPt.focusPos.x(),_firstPt.focusPos.z());
 	//	_firstPt.focusPos.x() = osg::RadiansToDegrees(_firstPt.focusPos.x());
 	//	_firstPt.focusPos.y() = osg::RadiansToDegrees(_firstPt.focusPos.y());
 	//	_firstPt.distan = min(pt1.distan*3.0,_firstPt.distan);
		//g_ellipsmodel->convertXYZToLatLongHeight(_firstcameraPos.x(),_firstcameraPos.y(),_firstcameraPos.z(),_firstcameraPos.y(),_firstcameraPos.x(),_firstcameraPos.z());
		//_firstcameraPos.x() = osg::RadiansToDegrees(_firstcameraPos.x());
		//_firstcameraPos.y() = osg::RadiansToDegrees(_firstcameraPos.y());
		//_firstQuat = mat.getRotate();

		//g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(pt1.focusPos.y()),osg::DegreesToRadians(pt1.focusPos.x()),pt1.focusPos.z(),focusToWorld);
		//mat = osg::Matrixd::translate(0.0,0.0,pt1.distan)*osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pt1.pitch),1.0,0.0,0.0)*osg::Matrixd::rotate(osg::DegreesToRadians(-pt1.yaw),0.0,0.0,1.0)*focusToWorld;
		//_lastcameraPos = mat.getTrans();
		////_lastLookat = -_lastcameraPos * osg::Matrixd::inverse(focusToWorld);
		//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt1.focusPos.y()),osg::DegreesToRadians(pt1.focusPos.x()),pt1.focusPos.z(),_lastPt.focusPos.x(),_lastPt.focusPos.y(),_lastPt.focusPos.z());
		//_lastLookat = _lastPt.focusPos - _lastcameraPos;
		//g_ellipsmodel->convertXYZToLatLongHeight(_lastcameraPos.x(),_lastcameraPos.y(),_lastcameraPos.z(),_lastcameraPos.y(),_lastcameraPos.x(),_lastcameraPos.z());
		//_lastcameraPos.x() = osg::RadiansToDegrees(_lastcameraPos.x());
		//_lastcameraPos.y() = osg::RadiansToDegrees(_lastcameraPos.y());
		//_lastQuat = mat.getRotate();
	}
/**
	* @brief 构造函数
**/
CGlbGlobeManipulator::CGlbGlobeFlyCallback::CGlbGlobeFlyCallback(KeyPoint pt0,KeyPoint pt1,glbDouble distan,glbDouble loopTime,glbBool isUnderMode):
	_loopTime(loopTime),
	_manipulator(NULL),
	_timeOffset(0.0),
	_timeMultiplier(1.0),
	_firstTime(DBL_MAX),
	_latestTime(0.0),
	_pause(false),
	_pauseTime(0.0),
	_islastFrame(false),
	_isUnder(isUnderMode){
		_firstPt = pt0;
		_lastPt = pt1;
		_matrix.makeIdentity();
		_isfar = false;
		mpr_flag = 2;
		if (abs(pt1.focusPos.x() -pt0.focusPos.x()) > 0.01 || abs(pt1.focusPos.x() -pt0.focusPos.x()) > 0.01)
			_isfar = true;
		_First_Last_distance = distan;
	}
/**
	* @brief 构造函数
**/
CGlbGlobeManipulator::CGlbGlobeFlyCallback::CGlbGlobeFlyCallback(CGlbGlobeManipulator* manipulator,glbDouble loopTime,osg::Vec3d centerPos,glbDouble distan):
	_manipulator(manipulator),
	_loopTime(loopTime),
	_timeOffset(0.0),
	_timeMultiplier(1.0),
	_firstTime(DBL_MAX),
	_latestTime(0.0),
	_pause(false),
	_pauseTime(0.0),
	_focus_temp(centerPos),
	_distance(distan),
	_islastFrame(false){
		mpr_flag = 3;
		_matrix.makeIdentity();
		manipulator->GetFocusPos(_firstPt.focusPos);
		_firstPt.distan = manipulator->GetDistance();
		_firstPt.yaw = manipulator->GetYaw();
		_firstPt.pitch = manipulator->GetPitch();

		osg::Vec3d cameraPos;
		manipulator->GetCameraPos(cameraPos);
		osg::Vec3d focusPos = centerPos;
		_lastPt.focusPos = focusPos;
		//由相机位置，焦点位置计算相机矩阵
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPos.x(),cameraPos.y(),cameraPos.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),focusPos.x(),focusPos.y(),focusPos.z());
		_lastPt.distan= (focusPos-cameraPos).length();
		osg::Matrixd mat;
		g_ellipsmodel->computeLocalToWorldTransformFromXYZ(focusPos.x(),focusPos.y(),focusPos.z(),mat);
		osg::Vec3d LocalPos = cameraPos*osg::Matrixd::inverse(mat);
		LocalPos.normalize();
		if (LocalPos.z() > 1.0)
		{
			_lastPt.pitch = -90.0;
			_lastPt.yaw = _firstPt.yaw;
		}
		else if (LocalPos.z() < -1.0)
		{
			_lastPt.pitch = 90.0;
			_lastPt.yaw = _firstPt.yaw;
		}
		else{
			_lastPt.pitch = osg::RadiansToDegrees(asin(-LocalPos.z()));
			_lastPt.yaw = osg::RadiansToDegrees(atan2(-LocalPos.x(),-LocalPos.y()));
		}
}

void CGlbGlobeManipulator::CGlbGlobeFlyCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{ 
	if (nv->getVisitorType()==osg::NodeVisitor::UPDATE_VISITOR && nv->getFrameStamp())
	{
		//if (_animationPath.valid())
		if (mpr_flag == 1)	//flyto2
		{
			_latestTime = nv->getFrameStamp()->getSimulationTime();
			if (!_pause)
			{ 	 
				// Only update _firstTime the first time, when its value is still DBL_MAX
				if (_firstTime==DBL_MAX)
					_firstTime = _latestTime;
				glbDouble tim = getAnimationTime();
				glbDouble ratio = tim/_loopTime;
				if (ratio > 1.0)
					ratio = 1.0;
				//焦点线性插值
				osg::Vec3d focusPos = interpolate(ratio,_firstPt.focusPos,_lastPt.focusPos);
				_focus_temp = focusPos;
				glbDouble distan = _firstPt.distan + (_lastPt.distan-_firstPt.distan)*ratio;	//距离
				if (_lastPt.distan + _firstPt.distan < 100000 && _isfar && !_isUnder)	//两个关键点高度和比较小时
				{					
					glbDouble mid_distan = _First_Last_distance;//2*(_lastPt.distan + _firstPt.distan);
					if (mid_distan > 500000000)
					{
						int temp = mid_distan/500000000.0;
						mid_distan /= temp;
					}
					if (ratio < 0.5)
						distan = mid_distan - (mid_distan-_firstPt.distan)*sqrt(1-ratio*ratio*4);
					else
						distan = mid_distan + (_lastPt.distan - mid_distan)*sqrt(1-(1.0-ratio)*(1.0-ratio)*4);
				}else if (_lastPt.distan/_firstPt.distan > 10.0)  //两个关键点高度相差很大时
				{
					distan = _lastPt.distan - (_lastPt.distan-_firstPt.distan)*sqrt(1-ratio*ratio);
				}else if (_lastPt.distan/_firstPt.distan < 0.1)	 //两个关键点高度相差很大时
				{
					distan = _firstPt.distan + (_lastPt.distan-_firstPt.distan)*sqrt(1-(1.0-ratio)*(1.0-ratio));
				}
				//保证delt_yaw在(-180,180)之间
				glbDouble delt_yaw = _lastPt.yaw-_firstPt.yaw;				
				if(delt_yaw > 180)
					delt_yaw -= 360;
				if(delt_yaw < -180)
					delt_yaw += 360;
				glbDouble yaw = _firstPt.yaw + delt_yaw*ratio;	//偏航						
				glbDouble pitch = 0.0;
				//if (_lastPt.distan + _firstPt.distan < 100000 && _isfar && !_isUnder)	//两个关键点高度和比较小时
				//{	
				//	glbDouble mid_pitch = (_lastPt.pitch+_firstPt.pitch)*0.5;
				//	glbDouble delt_pitch = 20.0;
				//	//if (mid_pitch < 0.0)
				//	//{
				//		mid_pitch = max(mid_pitch-delt_pitch,-89.99);
				//	//}else{
				//	//	mid_pitch = min(mid_pitch+delt_pitch,89.99);
				//	//}
				//	if (ratio < 0.5)
				//		pitch = mid_pitch - (mid_pitch-_firstPt.pitch)*(1-2*ratio);//俯仰
				//	else
				//		pitch = mid_pitch + (_lastPt.pitch-mid_pitch)*(-1+2*ratio);//俯仰
				//}else{
					pitch = _firstPt.pitch + (_lastPt.pitch-_firstPt.pitch)*ratio;//俯仰
				//}

				osg::Matrixd focusToWorld;
				g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),focusToWorld);
				_matrix = osg::Matrixd::translate(0.0,0.0,distan)*
					osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
					osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
					focusToWorld;
				////相机位置线性插值
				//g_ellipsmodel->convertXYZToLatLongHeight(_firstcameraPos.x(),_firstcameraPos.y(),_firstcameraPos.z(),_firstcameraPos.y(),_firstcameraPos.x(),_firstcameraPos.z());
				//g_ellipsmodel->convertXYZToLatLongHeight(_lastcameraPos.x(),_lastcameraPos.y(),_lastcameraPos.z(),_lastcameraPos.y(),_lastcameraPos.x(),_lastcameraPos.z());
				//_firstcameraPos.x() = osg::RadiansToDegrees(_firstcameraPos.x());
				//_firstcameraPos.y() = osg::RadiansToDegrees(_firstcameraPos.y());
				//_lastcameraPos.x() = osg::RadiansToDegrees(_lastcameraPos.x());
				//_lastcameraPos.y() = osg::RadiansToDegrees(_lastcameraPos.y());
				//osg::Vec3d cameraPos = interpolate(ratio,_firstcameraPos,_lastcameraPos);
				////焦点位置线性插值
				//osg::Vec3d focusPos = interpolate(ratio,_firstPt.focusPos,_lastPt.focusPos);
				//_manipulator->UpdataMatrix(cameraPos,focusPos);
				//_matrix = _manipulator->getMatrix();
				////UpdateMatrix(cameraPos,focusPos);
				if (ratio >= 1.0)
				{
					_islastFrame = true;
					setPause(true);
				}
			}
		}else if (mpr_flag == 2)	//flyto
		{		
			_latestTime = nv->getFrameStamp()->getSimulationTime();
			if (!_pause)
			{ 	 
				// Only update _firstTime the first time, when its value is still DBL_MAX
				if (_firstTime==DBL_MAX)
					_firstTime = _latestTime;
				glbDouble tim = getAnimationTime();
				glbDouble ratio = tim/_loopTime;
				if (ratio > 1.0)
					ratio = 1.0;
				//焦点线性插值
				osg::Vec3d focusPos = interpolate(ratio,_firstPt.focusPos,_lastPt.focusPos);
				_focus_temp = focusPos;
				glbDouble distan = _firstPt.distan + (_lastPt.distan-_firstPt.distan)*ratio;	//距离
				if (_lastPt.distan + _firstPt.distan < 100000 && _isfar && !_isUnder)	//两个关键点高度和比较小时
				{					
					glbDouble mid_distan = _First_Last_distance;//2*(_lastPt.distan + _firstPt.distan);
					while (mid_distan > 500000000)
						mid_distan /= 2;
					if (ratio < 0.5)
						distan = mid_distan - (mid_distan-_firstPt.distan)*sqrt(1-ratio*ratio*4);
						//distan = _firstPt.distan + (mid_distan-_firstPt.distan)*ratio*2;
					else
						distan = mid_distan + (_lastPt.distan - mid_distan)*sqrt(1-(1.0-ratio)*(1.0-ratio)*4);
				}else if (_lastPt.distan/_firstPt.distan > 10.0)  //两个关键点高度相差很大时
				{
					distan = _lastPt.distan - (_lastPt.distan-_firstPt.distan)*sqrt(1-ratio*ratio);
				}else if (_lastPt.distan/_firstPt.distan < 0.1)	 //两个关键点高度相差很大时
				{
					distan = _firstPt.distan + (_lastPt.distan-_firstPt.distan)*sqrt(1-(1.0-ratio)*(1.0-ratio));
				}

				//保证delt_yaw在(-180,180)之间
				glbDouble delt_yaw = _lastPt.yaw-_firstPt.yaw;				
				if(delt_yaw > 180)
					delt_yaw -= 360;
				if(delt_yaw < -180)
					delt_yaw += 360;
				glbDouble yaw = _firstPt.yaw + delt_yaw*ratio;	//偏航	
				glbDouble pitch = 0.0;
				if (_lastPt.distan + _firstPt.distan < 100000 && !_isUnder)	//两个关键点高度和比较小时
				{	
					glbDouble mid_pitch = (_lastPt.pitch+_firstPt.pitch)*0.5;
					glbDouble delt_pitch = 0.0;
					if (_First_Last_distance < 100.0)
						delt_pitch = 0.0;
					else if (_First_Last_distance < 1000.0)
						delt_pitch = (_First_Last_distance-100.0)*0.03;
					else if (_First_Last_distance < 10000.0)
						delt_pitch = 2.7+(_First_Last_distance-1000.0)*0.003;
					else if (_First_Last_distance < 100000.0)
						delt_pitch = 5.4+(_First_Last_distance - 10000.0)*0.0003;
					else
						delt_pitch = 8.1+(_First_Last_distance - 100000.0)*0.00003;
					delt_pitch = min(delt_pitch,10.0);

					//if (mid_pitch < 0.0)
					//{
						mid_pitch = max(mid_pitch-delt_pitch,-89.99);
					//}else{
					//	mid_pitch = min(mid_pitch+delt_pitch,89.99);
					//}
					if (ratio < 0.5)
						pitch = mid_pitch - (mid_pitch-_firstPt.pitch)*(1-2*ratio);//俯仰
					else
						pitch = mid_pitch + (_lastPt.pitch-mid_pitch)*(-1+2*ratio);//俯仰
				}else{
					pitch = _firstPt.pitch + (_lastPt.pitch-_firstPt.pitch)*ratio;//俯仰
				}				
				osg::Matrixd focusToWorld;
				g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),focusToWorld);
				_matrix = osg::Matrixd::translate(0.0,0.0,distan)*
					osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
					osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
					focusToWorld;

				if (ratio >= 1.0)
				{
					_islastFrame = true;
					setPause(true);
				}
				//osg::AnimationPath::ControlPoint cp;
				//if (_animationPath->getInterpolatedControlPoint(tim,cp))				
					//cp.getMatrix(_matrix);
				//{
					//osg::Vec3d cameraPos = cpcamera.getPosition();
					//osg::Vec3d focusPos = cpfocus.getPosition();

					////世界坐标系下
					//osg::Vec3d lookvec = focusPos-cameraPos;
					//glbDouble lon,lat,alt;
					//g_ellipsmodel->convertXYZToLatLongHeight(cameraPos.x(),cameraPos.y(),cameraPos.z(),lat,lon,alt);
					//osg::Vec3d cameraup(cos(lon)*cos(lat), sin(lon)*cos(lat), sin(lat));
					//
					//glbDouble distan = lookvec.length();
					//osg::Quat quat;
					//quat.makeRotate(osg::Vec3d(0.0,0.0,-distan),lookvec);

					//osg::Vec3d s = lookvec^cameraup;
					//osg::Vec3d up = s^lookvec;	//相机朝向向量
					//up.normalize();

					////相机向上向量rotate后
					//osg::Vec3d up2 = osg::Vec3d(0.0,1.0,0.0)*osg::Matrixd::rotate(quat);
					//up2.normalize();

					//osg::Quat quat2;
					//quat2.makeRotate(up2,up);

					//_matrix = osg::Matrixd::rotate(quat)*osg::Matrixd::rotate(quat2)*osg::Matrixd::translate(cameraPos);
				//}
				//if (_animationPath->getLoopMode() == osg::AnimationPath::NO_LOOPING && tim > _animationPath->getPeriod())
					//setPause(true);
			}
		}else if (mpr_flag == 3) //flyaround
		{
			//先需要flyto到目标位置
			glbDouble tim = _latestTime;
			_latestTime = nv->getFrameStamp()->getSimulationTime();
			tim = _latestTime-tim;
			if (!_pause)
			{ 	 
				// Only update _firstTime the first time, when its value is still DBL_MAX
				if (_firstTime==DBL_MAX)
				{
					_firstTime = _latestTime;
					tim = 0.0;
				}
				glbDouble ratio = getAnimationTime();
				if (ratio < 1.0)	//先需要飞行1.0秒
				{
					//焦点线性插值
					osg::Vec3d focusPos = interpolate(ratio,_firstPt.focusPos,_lastPt.focusPos);
					_focus_temp = focusPos;
					glbDouble distan = _firstPt.distan + (_lastPt.distan-_firstPt.distan)*ratio;	//距离
					if (_lastPt.distan + _firstPt.distan < 100000 && _isfar && !_isUnder)	//两个关键点高度和比较小时
					{					
						glbDouble mid_distan = _First_Last_distance;//2*(_lastPt.distan + _firstPt.distan);
						while (mid_distan > 500000000)
							mid_distan /= 2;
						if (ratio < 0.5)
							distan = mid_distan - (mid_distan-_firstPt.distan)*sqrt(1-ratio*ratio*4);
						//distan = _firstPt.distan + (mid_distan-_firstPt.distan)*ratio*2;
						else
							distan = mid_distan + (_lastPt.distan - mid_distan)*sqrt(1-(1.0-ratio)*(1.0-ratio)*4);
					}else if (_lastPt.distan/_firstPt.distan > 10.0)  //两个关键点高度相差很大时
					{
						distan = _lastPt.distan - (_lastPt.distan-_firstPt.distan)*sqrt(1-ratio*ratio);
					}else if (_lastPt.distan/_firstPt.distan < 0.1)	 //两个关键点高度相差很大时
					{
						distan = _firstPt.distan + (_lastPt.distan-_firstPt.distan)*sqrt(1-(1.0-ratio)*(1.0-ratio));
					}

					//保证delt_yaw在(-180,180)之间
					glbDouble delt_yaw = _lastPt.yaw-_firstPt.yaw;				
					if(delt_yaw > 180)
						delt_yaw -= 360;
					if(delt_yaw < -180)
						delt_yaw += 360;
					glbDouble yaw = _firstPt.yaw + delt_yaw*ratio;	//偏航	
					glbDouble pitch = 0.0;
					if (_lastPt.distan + _firstPt.distan < 100000 && !_isUnder)	//两个关键点高度和比较小时
					{	
						glbDouble mid_pitch = (_lastPt.pitch+_firstPt.pitch)*0.5;
						glbDouble delt_pitch = 0.0;
						if (_First_Last_distance < 100.0)
							delt_pitch = 0.0;
						else if (_First_Last_distance < 1000.0)
							delt_pitch = (_First_Last_distance-100.0)*0.03;
						else if (_First_Last_distance < 10000.0)
							delt_pitch = 2.7+(_First_Last_distance-1000.0)*0.003;
						else if (_First_Last_distance < 100000.0)
							delt_pitch = 5.4+(_First_Last_distance - 10000.0)*0.0003;
						else
							delt_pitch = 8.1+(_First_Last_distance - 100000.0)*0.00003;
						delt_pitch = min(delt_pitch,10.0);

						//if (mid_pitch < 0.0)
						//{
						mid_pitch = max(mid_pitch-delt_pitch,-89.99);
						//}else{
						//	mid_pitch = min(mid_pitch+delt_pitch,89.99);
						//}
						if (ratio < 0.5)
							pitch = mid_pitch - (mid_pitch-_firstPt.pitch)*(1-2*ratio);//俯仰
						else
							pitch = mid_pitch + (_lastPt.pitch-mid_pitch)*(-1+2*ratio);//俯仰
					}else{
						pitch = _firstPt.pitch + (_lastPt.pitch-_firstPt.pitch)*ratio;//俯仰
					}				
					osg::Matrixd focusToWorld;
					g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),focusToWorld);
					_matrix = osg::Matrixd::translate(0.0,0.0,distan)*
						osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
						osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
						focusToWorld;			
				}else if (_manipulator)
				{
					//glbDouble yaw = 360.0*tim/_loopTime + _manipulator->GetYaw();
					//osg::Vec3d focusPos;
					//_manipulator->GetFocusPos(focusPos);
					//glbDouble distan = _manipulator->GetDistance();
					//_manipulator->SetYaw(yaw,_focus_temp,_distance);
					glbDouble delt_yaw = 360.0*(tim)/_loopTime;
					_manipulator->Rotate(_focus_temp.x(),_focus_temp.y(),_focus_temp.z(),0.0,delt_yaw);	
					//_manipulator->SetYaw(delt_yaw+_manipulator->GetYaw(),_focus_temp,_manipulator->GetDistance());
					_matrix = _manipulator->getMatrix();
				}
			}
		}
		traverse(node,nv);
	}
}

glbDouble CGlbGlobeManipulator::CGlbGlobeFlyCallback::getAnimationTime() const
{
	return ((_latestTime-_firstTime)-_timeOffset)*_timeMultiplier;
}

void CGlbGlobeManipulator::CGlbGlobeFlyCallback::setPause(glbBool pause)
{ 
	if (_pause == pause)
		return;	
	_pause = pause;
	if (_firstTime==DBL_MAX)
		return;
	if (_pause)
		_pauseTime = _latestTime;
	else
		_firstTime += (_latestTime-_pauseTime);
}

osg::Matrixd CGlbGlobeManipulator::CGlbGlobeFlyCallback::getMatrix() const
{
	return _matrix;
} 

void CGlbGlobeManipulator::CGlbGlobeFlyCallback::UpdateMatrix(osg::Vec3d cameraPos,osg::Vec3d focusPos)
{
	//初始相机lookat(0.0,1.0,0.0)
	cameraPos.x() = osg::DegreesToRadians(cameraPos.x());
	cameraPos.y() = osg::DegreesToRadians(cameraPos.y());
	focusPos.x() = osg::DegreesToRadians(focusPos.x());
	focusPos.y() = osg::DegreesToRadians(focusPos.y());
	g_ellipsmodel->convertLatLongHeightToXYZ(cameraPos.y(),cameraPos.x(),cameraPos.z(),cameraPos.x(),cameraPos.y(),cameraPos.z());
	g_ellipsmodel->convertLatLongHeightToXYZ(focusPos.y(),focusPos.x(),focusPos.z(),focusPos.x(),focusPos.y(),focusPos.z());
	osg::Vec3d lookvec = focusPos-cameraPos;
	osg::Quat quat;
	quat.makeRotate(osg::Vec3d(0.0,1.0,0.0),lookvec);
	_matrix = osg::Matrixd::rotate(quat)*osg::Matrixd::translate(cameraPos);
}

osg::Vec3d CGlbGlobeManipulator::CGlbGlobeFlyCallback::interpolate(glbDouble ratio,osg::Vec3d& pt0,osg::Vec3d& pt1)
{
	glbDouble delt = pt1.x()-pt0.x();
	if(delt > 180)
		delt -= 360;
	if(delt < -180)
		delt += 360;
	osg::Vec3d v;
	v.x() = pt0.x() + ratio*delt;
	v.y() = pt0.y()	+ ratio*(pt1.y()-pt0.y());
	v.z() = pt0.z()	+ ratio*(pt1.z()-pt0.z());
	return v;
}

osg::Quat CGlbGlobeManipulator::CGlbGlobeFlyCallback::interpolate(glbDouble ratio,osg::Quat& q0,osg::Quat& q1)
{
	osg::Quat q;	
	q.x() = q0.x()	+ ratio*(q1.x()-q0.x());
	q.y() = q0.y()	+ ratio*(q1.y()-q0.y());
	q.z() = q0.z()	+ ratio*(q1.z()-q0.z());
	q.w() = q0.w()	+ ratio*(q1.w()-q0.w());
	return q;
}

//glbInt32 CGlbGlobeManipulator::IntersectRaySphere(osg::Vec3d startPos,osg::Vec3d endPos,osg::Vec3d centerPos,glbDouble radius,std::vector<osg::Vec3d>& IntersectPos)
//{
//	std::vector<osg::Vec3d>().swap(IntersectPos);
//	osg::Vec3d vecPos = endPos - startPos;
//	/************************************************************************/
//	/* 求方程((end-start)*t + start).leng() = radius^2 的解    
//	/* 即a*X^2 + b*X + c = 0的解
//	/************************************************************************/
//	glbDouble a = vecPos.length2();
//	glbDouble b = vecPos*startPos;
//	glbDouble c = startPos.length2() - radius*radius;
//	glbDouble discr = b*b - a*c;
//	glbInt32 result = 0;
//	if (a == 0)
//		return 0;
//	if (discr == 0)	//单解
//	{
//		glbDouble t = -b/a;
//		if (t <= 1.0 && t>= 0.0)
//		{
//			osg::Vec3d Pos = startPos + vecPos*t;
//			IntersectPos.push_back(Pos);
//			result++;
//		}
//	}else if(discr > 0)	//双解
//	{	
//		if (a > 0)
//		{		
//			glbDouble t = -(b+sqrt(discr))/a;
//			if (t <= 1.0 && t >= 0.0)
//			{
//				osg::Vec3d Pos = startPos + vecPos*t;
//				IntersectPos.push_back(Pos);
//				result++;
//			}
//			t = -(b-sqrt(discr))/a;
//			if (t <= 1.0 && t >= 0.0)
//			{
//				osg::Vec3d Pos = startPos + vecPos*t;
//				IntersectPos.push_back(Pos);
//				result++;
//			}
//		}else{
//			glbDouble t = -(b-sqrt(discr))/a;
//			if (t <= 1.0 && t >= 0.0)
//			{
//				osg::Vec3d Pos = startPos + vecPos*t;
//				IntersectPos.push_back(Pos);
//				result++;
//			}
//			t = -(b+sqrt(discr))/a;
//			if (t <= 1.0 && t >= 0.0)
//			{
//				osg::Vec3d Pos = startPos + vecPos*t;
//				IntersectPos.push_back(Pos);
//				result++;
//			}
//		}
//	}
//	return result;
//}

glbInt32 CGlbGlobeManipulator::IntersectRayPlane(osg::Vec3d startPos,osg::Vec3d endPos,osg::Vec3d planePos,osg::Vec3d nomalvec,std::vector<osg::Vec3d>& IntersectPos)
{
	std::vector<osg::Vec3d>().swap(IntersectPos);
	osg::Vec3d lineDir = endPos - startPos;
	lineDir.normalize();
	osg::Vec3d tempDir = startPos - planePos;
	tempDir.normalize();
	float temp = nomalvec * tempDir;
	if(fabs(lineDir * nomalvec) < 0.00000001)
	{
		if(fabs(temp) < 0.00000001)
			return -1;//相交，直线全在平面内
		else
			return 0;//没有交集，直线与平面平行
	}
	double t = 0.0;
	osg::Vec4d factor(nomalvec.x(),nomalvec.y(),nomalvec.z(),-nomalvec*planePos);
	t = -(factor.x() * startPos.x() + factor.y() * startPos.y() + factor.z() * startPos.z() + factor.w()) / 
		(lineDir.x() * factor.x() + lineDir.y() * factor.y() + lineDir.z() * factor.z());
	if (t < 0)
	{
		return 0;
	}
	osg::Vec3d point = startPos + lineDir*t;
	IntersectPos.push_back(point);
	return 1;
}

void CGlbGlobeManipulator::Shake(glbBool isShake)
{
	if (mpr_isShake == isShake)
		return;	

	if (isShake)
	{
		srand((int)time(0));
		mpr_orignPitch = mpr_pitch;
		mpr_orignyaw = mpr_yaw;
		GetFocusPos(mpr_shakefocusPos);
		mpr_shakeDistance = mpr_distance;	
	}
	else
	{
		mpr_pitch = mpr_orignPitch;
		mpr_yaw = mpr_orignyaw;
		mpr_focusPos = mpr_shakefocusPos; 
		mpr_distance = mpr_shakeDistance;
	}
	mpr_shakeStartTime = -1;

	mpr_isShake = isShake;
}

void CGlbGlobeManipulator::SetShakeSope(glbDouble horizontalDegree, glbDouble verticalDegree)
{
	mpr_shakeHorDegree = horizontalDegree;
	mpr_shakeVerDegree = verticalDegree;
}

void CGlbGlobeManipulator::SetShakeTime(glbDouble seconds)
{
	mpr_shakeTime = seconds;
}

void CGlbGlobeManipulator::EnalbeVirtualReferencePlane(glbBool bEnable)
{
	mpr_bUseReferencePlane = bEnable;
}		
void CGlbGlobeManipulator::SetVirtualReferencePlane(glbDouble zOrAltitude)
{
	mpr_referencePlaneZ = zOrAltitude;
}		
glbDouble CGlbGlobeManipulator::GetVirtualReferencePlane()
{
	return mpr_referencePlaneZ;
}

void CGlbGlobeManipulator::Push(glbInt32 ptx,glbInt32 pty)
{
	_pushpitch = mpr_pitch;
	_pushyaw = mpr_yaw;
	_pushMatrix = getMatrix();
	osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
	_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();

	//记录选中点经纬高
	glbDouble dx = ptx;
	glbDouble dy = pty;				
	//窗口坐标系(鼠标位置) 转换为 屏幕坐标
	//计算与地面或地下参考面的交点
	glbBool ischoose = mpr_globe->ScreenToTerrainCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());
	if (!ischoose)
	{// 与地面没有交点则跟地下参考面求交
		if (mpr_globe->IsUnderGroundMode())	  //开启地下模式 					
			ischoose = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());					
	}

	osg::Vec3d pt11_w;
	glbDouble ptx1_s = dx,pty1_s = dy;
	mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
	//计算在此位置能够拣选到的最近对象和交点
	osg::Vec3d InterPoint;
	glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx1_s,pty1_s,InterPoint);

	mpr_isPushPicked = true;
	if (!ischoose && !isObjfocus)
	{// 跟对象和地面/地下参考面都没有交集
		mpr_isPushPicked = false;
	}
	else if (isObjfocus && ischoose)
	{// 比较对象上的pick点和地面/地下参考面的pick点哪个离相机近，就选用哪个点做pushPos			
		osg::Vec3d pushPos;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_pushPos.y()),osg::DegreesToRadians(_pushPos.x()),_pushPos.z(),pushPos.x(),pushPos.y(),pushPos.z());

		osg::Vec3d cameraPos_w = getMatrix().getTrans();
		glbDouble dis1 = (cameraPos_w - InterPoint).length();
		glbDouble dis2 = (cameraPos_w - pushPos).length();
		if (dis1 < dis2)
		{// 对象上的pick点离相机近
			//_pushPos = InterPoint;
			g_ellipsmodel->convertXYZToLatLongHeight(InterPoint.x(),InterPoint.y(),InterPoint.z(),_pushPos.y(),_pushPos.x(),_pushPos.z());
			RadToDeg(_pushPos);
		}
	}
	else if (isObjfocus && !ischoose)
	{// 只跟对象有交点
		//_pushPos = InterPoint;
		g_ellipsmodel->convertXYZToLatLongHeight(InterPoint.x(),InterPoint.y(),InterPoint.z(),_pushPos.y(),_pushPos.x(),_pushPos.z());
		RadToDeg(_pushPos);
	}			
}

void CGlbGlobeManipulator::Rotate(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2)
{
	//获取拖拽的向量				
	glbDouble dx = ptx2 - ptx1;
	glbDouble dy = pty2 - pty1;

	glbDouble tempd = 1.0;
	if (mpr_isctrldown)
		tempd = 0.1;
	else
		tempd = 0.3;
	glbDouble yaw = -dx*tempd;	//系数暂定0.3
	glbDouble pitch = dy*tempd;

	Rotate(_pushPos.x(),_pushPos.y(),_pushPos.z(),pitch,yaw);	
}

void CGlbGlobeManipulator::Zoom(glbBool isScrollUp)
{
	glbDouble zoomDelta = isScrollUp ? 1.0 : -1.0;
	Zoom(zoomDelta);
}

