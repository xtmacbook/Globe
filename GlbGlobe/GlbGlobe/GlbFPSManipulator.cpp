#include "StdAfx.h"
#include "GlbFPSManipulator.h"
#include "GlbGlobeView.h"
#include "GlbGlobeMath.h"
#include "GlbGlobeManipulator.h"

using namespace GlbGlobe;

#ifndef GROUND_ALTITUDE_THRESHOLD_FPS 
#define GROUND_ALTITUDE_THRESHOLD_FPS 10.0 //相机与地下参考面的阈值
#endif

#ifndef CAMERA_UNDERGROUND_TOP_HOLDPFS
#define CAMERA_UNDERGROUND_TOP_HOLDPFS 0.5 //相机在地下与头顶地形缓冲阈值
#endif

#ifndef FPSEYEFOCUSVAL
#define FPSEYEFOCUSVAL 1000.0  //眼睛距交点位置的距离
#endif

extern const osg::Vec3d invalidFlyFocus  = osg::Vec3d(DBL_MAX,DBL_MAX,DBL_MAX);
extern const float ELEVATE_THRESHOLD_FPS = 0.1;//相机在地上时，位于地形之上的阈值，单位m

//////////////////////////////////////////////////////////////////////////

//#define SWEPTSPHERE
//			获取交点世界坐标矩阵

//以下各个函数传递全部为角度，返回也为角度

void GlbGlobe::getLocalToWorldFromLatLong(const osg::Vec3d&focus,osg::Matrixd&focusToWorld)
{
	g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(focus.x()),
													osg::DegreesToRadians(focus.y()),focus.z(),focusToWorld);
}

void GlbGlobe::getLocalToWorldFromXYZMatrix(const osg::Vec3d&focus,osg::Matrixd&focusToWorld)
{
	g_ellipsmodel->computeLocalToWorldTransformFromXYZ(focus.x(),focus.y(),focus.z(),focusToWorld);
}

void GlbGlobe::convertXYZToLatLongHeight(const GLdouble&X,const GLdouble&Y,const GLdouble&Z,osg::Vec3d&llh)
{
	glbDouble x,y,z;

	g_ellipsmodel->convertXYZToLatLongHeight(X,Y,Z,x,y,z);
	
	llh.x() = osg::RadiansToDegrees(x);
	llh.y() = osg::RadiansToDegrees(y);
	llh.z() = z;
}

void GlbGlobe::convertLatLongHeightToXYZ(const GLdouble&lat,const GLdouble&lon,const GLdouble&heigh,osg::Vec3d&xyz)
{
	glbDouble x,y,z;

	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),heigh,x,y,z);

	xyz.x() = x;
	xyz.y() = y;
	xyz.z() = z;
}

osg::Vec2d GlbGlobe::computeIntermediatePointBetweenToPoint(const osg::Vec2d&startLLH, 
	const osg::Vec2d&targetLLH,glbDouble fraction)
{
	glbDouble lat1 = startLLH.x();
	glbDouble lng1 = startLLH.y();
	glbDouble lat2 = targetLLH.x();
	glbDouble lng2 = targetLLH.y();

	glbDouble d= 2 * std::asin(
		std::sqrt(std::pow((std::sin((lat1 - lat2) / 2)),2) +
		std::cos(lat1) * std::cos(lat2) *
		std::pow(std::sin((lng1-lng2) / 2), 2))
		);

	glbDouble A = std::sin((1 - fraction) * d) / std::sin(d);
	glbDouble B = std::sin(fraction * d) / std::sin(d);

	glbDouble x = A * std::cos(lat1) * std::cos(lng1) + B *
		std::cos(lat2) * std::cos(lng2);
	glbDouble y = A * std::cos(lat1) * std::sin(lng1) + B *
		std::cos(lat2) * std::sin(lng2);
	glbDouble z = A * std::sin(lat1) + B * std::sin(lat2);

	glbDouble lat = std::atan2(z, std::sqrt(std::pow(x, 2) + std::pow(y, 2)));
	glbDouble lng = std::atan2(y, x);

	osg::Vec2d llh;
	llh.x() =  lat;
	llh.y() =  lng;

	return llh;
}

GLdouble GlbGlobe::computeTowPointGreatCircleDistance(const osg::Vec2d&startLLH,const osg::Vec2d&targetLLH)
{
	const GLdouble lat1 = startLLH.x();
	const GLdouble lat2  = targetLLH.x();
	//@
	const GLdouble deltaLat = osg::DegreesToRadians((osg::RadiansToDegrees(lat2) - osg::RadiansToDegrees(lat1)));
	const GLdouble deltaLon = osg::DegreesToRadians((osg::RadiansToDegrees(targetLLH.y()) - osg::RadiansToDegrees(startLLH.y())));

	const GLdouble a  = std::sin(deltaLat/2) * std::sin(deltaLat/2) +
		std::cos(lat1) * std::cos(lat2) *
		std::sin(deltaLon/2) * std::sin(deltaLon/2);
	
	const GLdouble c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));

	GLdouble R = (g_ellipsmodel->getRadiusPolar() + g_ellipsmodel->getRadiusPolar()) / 2;

	return R * c;
}

osg::Vec3d GlbGlobe::quatToEuler(osg::Quat q)
{
	glbDouble pitch, roll, yaw;

	roll  = asin( 2* (q.x()*q.z() - q.w()*q.y()) );
	yaw   = atan2( 2* (q.x()*q.y() + q.z()*q.w()) , 1 - (2* (q.y()*q.y() + q.z()*q.z())) );
	pitch = atan2( 2* (q.x()*q.w() + q.y()*q.z()) , 1 - (2* (q.z()*q.z() + q.w()*q.w())) );

	if (_isnan(roll))  roll = 0.0;
	if (_isnan(yaw))   yaw = 0.0;
	if (_isnan(pitch)) pitch = 0.0;

	// 纠正osg坐标系
	pitch = osg::PI-pitch;
	roll  = -roll;

	//确保在[-PI,PI]
	if (roll>osg::PI)
	{
		roll -= 2 * osg::PI;
	}
	else if (roll<-osg::PI)
	{
		roll += 2 * osg::PI;
	}
	if (yaw>osg::PI) 
	{
		yaw -= 2 * osg::PI;
	}
	else if (yaw<-osg::PI) 
	{
		yaw += 2 * osg::PI;
	}
	if (pitch>osg::PI) 
	{
		pitch -= 2 * osg::PI;
	}
	else if (pitch<-osg::PI)
	{
		pitch += 2 * osg::PI;
	}

	return osg::Vec3(pitch,roll,yaw);
}

osg::Vec3d GlbGlobe::interpolate(glbDouble ratio,osg::Vec3d& pt0,osg::Vec3d& pt1)
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

void GlbGlobe::CGlbFPSManipulator::flyToFromCGlbGlobeManipulatorinit(FLYTYPE flyType,const KeyPoint&pt0,const KeyPoint&pt1,GLdouble distan,GLdouble loopTime)
{

	_firstPt = pt0;
	_lastPt = pt1;
	Globe_isfar = false;

	if(FLYTO ==flyType)
	{
		if (abs(_lastPt.focusPos.x() -_firstPt.focusPos.x()) > 0.01 ||
			abs(_lastPt.focusPos.x() -_firstPt.focusPos.x()) > 0.01)
		{
			Globe_isfar			= true;
		}
		Globe_First_Last_distance = distan;
	}
	if(FLYTO2 == flyType)
	{
		if (abs(_lastPt.focusPos.x() -_firstPt.focusPos.x()) > 0.01 ||
			abs(_lastPt.focusPos.x() -_firstPt.focusPos.x()) > 0.01)
		{
			Globe_isfar			= true;
		}
	}

	Globe_firstTime			= DBL_MAX;
	Globe_latestTime		= 0.0;
	Globe_timeMultiplier	= 1.0;
	Globe_loopTime			= loopTime;
	Globe_pause				= false;
	Globe_Fly_flag			= flyType;

	Globe_matrix.makeIdentity();

}

void GlbGlobe::CGlbFPSManipulator::flyToFromCGlbGlobeManipulator(osg::NodeVisitor* nv)
{
	if(FLYTO2 ==  Globe_Fly_flag)
	{
		Globe_latestTime = nv->getFrameStamp()->getSimulationTime();
		if (!Globe_pause)
		{ 	 
			if (Globe_firstTime==DBL_MAX)
				Globe_firstTime = Globe_latestTime;

			glbDouble tim = ((Globe_latestTime-Globe_firstTime))*Globe_timeMultiplier;
			glbDouble ratio = tim/Globe_loopTime;
			if (ratio > 1.0) ratio = 1.0;
			//焦点线性插值
			osg::Vec3d focusPos = interpolate(ratio,_firstPt.focusPos,_lastPt.focusPos);
			Globe_focus_temp = focusPos;

			glbDouble distan = _firstPt.distan + (_lastPt.distan-_firstPt.distan)*ratio;	//距离
			if (_lastPt.distan + _firstPt.distan < 100000 && Globe_isfar && !Globe_isUnder)	//两个关键点高度和比较小时
			{					
				glbDouble mid_distan = Globe_First_Last_distance;//2*(_lastPt.distan + _firstPt.distan);
				if (mid_distan > 500000000)
				{
					int temp = mid_distan/500000000.0;
					mid_distan /= temp;
				}
				if (ratio < 0.5)
					distan = mid_distan - (mid_distan-_firstPt.distan)*sqrt(1-ratio*ratio*4);
				else
					distan = mid_distan + (_lastPt.distan - mid_distan)*sqrt(1-(1.0-ratio)*(1.0-ratio)*4);
			}
			else if (_lastPt.distan/_firstPt.distan > 10.0)  //两个关键点高度相差很大时
			{
				distan = _lastPt.distan - (_lastPt.distan-_firstPt.distan)*sqrt(1-ratio*ratio);
			}
			else if (_lastPt.distan/_firstPt.distan < 0.1)	 //两个关键点高度相差很大时
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
			pitch = _firstPt.pitch + (_lastPt.pitch-_firstPt.pitch)*ratio;//俯仰

			osg::Matrixd focusToWorld;
			getLocalToWorldFromLatLong(osg::Vec3d(focusPos.y(),focusPos.x(),focusPos.z()),focusToWorld);
			Globe_matrix = osg::Matrixd::translate(0.0,0.0,distan)*
				osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
				osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
				focusToWorld;

			if (ratio >= 1.0)
			{
				Globe_pause = true;
			}
		}

	}
	else if(FLYTO == Globe_Fly_flag)
	{
		Globe_latestTime = nv->getFrameStamp()->getSimulationTime();
		if (!Globe_pause)
		{ 	 
			// Only update _firstTime the first time, when its value is still DBL_MAX
			if (Globe_firstTime==DBL_MAX)
			{
				Globe_firstTime = Globe_latestTime;
			}

			glbDouble tim = ((Globe_latestTime-Globe_firstTime))*Globe_timeMultiplier;

			glbDouble ratio = tim/Globe_loopTime;
			if (ratio > 1.0) ratio = 1.0;

			//焦点线性插值
			osg::Vec3d focusPos = interpolate(ratio,_firstPt.focusPos,_lastPt.focusPos);

			glbDouble distan = _firstPt.distan + (_lastPt.distan-_firstPt.distan)*ratio;	//距离
			if (_lastPt.distan + _firstPt.distan < 100000 && Globe_isfar && !Globe_isUnder)	//两个关键点高度和比较小时
			{					
				glbDouble mid_distan = Globe_First_Last_distance;//2*(_lastPt.distan + _firstPt.distan);
				while (mid_distan > 500000000)
					mid_distan /= 2;
				if (ratio < 0.5)
					distan = mid_distan - (mid_distan-_firstPt.distan)*sqrt(1-ratio*ratio*4);
				//distan = _firstPt.distan + (mid_distan-_firstPt.distan)*ratio*2;
				else
					distan = mid_distan + (_lastPt.distan - mid_distan)*sqrt(1-(1.0-ratio)*(1.0-ratio)*4);
			}
			else if (_lastPt.distan/_firstPt.distan > 10.0)  //两个关键点高度相差很大时
			{
				distan = _lastPt.distan - (_lastPt.distan-_firstPt.distan)*sqrt(1-ratio*ratio);
			}
			else if (_lastPt.distan/_firstPt.distan < 0.1)	 //两个关键点高度相差很大时
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
			if (_lastPt.distan + _firstPt.distan < 100000 && !Globe_isUnder)	//两个关键点高度和比较小时
			{	
				glbDouble mid_pitch = (_lastPt.pitch+_firstPt.pitch)*0.5;
				glbDouble delt_pitch = 0.0;
				if (Globe_First_Last_distance < 100.0)
					delt_pitch = 0.0;
				else if (Globe_First_Last_distance < 1000.0)
					delt_pitch = (Globe_First_Last_distance-100.0)*0.03;
				else if (Globe_First_Last_distance < 10000.0)
					delt_pitch = 2.7+(Globe_First_Last_distance-1000.0)*0.003;
				else if (Globe_First_Last_distance < 100000.0)
					delt_pitch = 5.4+(Globe_First_Last_distance - 10000.0)*0.0003;
				else
					delt_pitch = 8.1+(Globe_First_Last_distance - 100000.0)*0.00003;
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
			g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(focusPos.y()),
				osg::DegreesToRadians(focusPos.x()),focusPos.z(),focusToWorld);

			Globe_matrix = osg::Matrixd::translate(0.0,0.0,distan)*
				osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
				osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
				focusToWorld;

			if (ratio >= 1.0)
			{
				Globe_pause = true;
			}
		}
	}
	else if(FLYAROUND == Globe_Fly_flag)
	{
		//fly around
		//先需要flyto到目标位置
		glbDouble tim = Globe_latestTime;
		Globe_latestTime = nv->getFrameStamp()->getSimulationTime();
		tim = Globe_latestTime-tim;
		if (!Globe_pause)
		{ 	 
			// Only update _firstTime the first time, when its value is still DBL_MAX
			if (Globe_firstTime==DBL_MAX)
			{
				Globe_firstTime = Globe_latestTime;
				tim = 0.0;
			}
			glbDouble ratio = ((Globe_latestTime-Globe_firstTime))*Globe_timeMultiplier;
			

			if (ratio < 1.0)	//先需要飞行1.0秒
			{
				//焦点线性插值
				osg::Vec3d focusPos = interpolate(ratio,_firstPt.focusPos,_lastPt.focusPos);
				Globe_focus_temp = focusPos;
				glbDouble distan = _firstPt.distan + (_lastPt.distan-_firstPt.distan)*ratio;	//距离
				if (_lastPt.distan + _firstPt.distan < 100000 && Globe_isfar && !Globe_isUnder)	//两个关键点高度和比较小时
				{					
					glbDouble mid_distan = Globe_First_Last_distance;//2*(_lastPt.distan + _firstPt.distan);
					while (mid_distan > 500000000)
						mid_distan /= 2;
					if (ratio < 0.5)
						distan = mid_distan - (mid_distan-_firstPt.distan)*sqrt(1-ratio*ratio*4);
					//distan = _firstPt.distan + (mid_distan-_firstPt.distan)*ratio*2;
					else
						distan = mid_distan + (_lastPt.distan - mid_distan)*sqrt(1-(1.0-ratio)*(1.0-ratio)*4);
				}
				else if (_lastPt.distan/_firstPt.distan > 10.0)  //两个关键点高度相差很大时
				{
					distan = _lastPt.distan - (_lastPt.distan-_firstPt.distan)*sqrt(1-ratio*ratio);
				}
				else if (_lastPt.distan/_firstPt.distan < 0.1)	 //两个关键点高度相差很大时
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
				if (_lastPt.distan + _firstPt.distan < 100000 && !Globe_isUnder)	//两个关键点高度和比较小时
				{	
					glbDouble mid_pitch = (_lastPt.pitch+_firstPt.pitch)*0.5;
					glbDouble delt_pitch = 0.0;
					if (Globe_First_Last_distance < 100.0)
						delt_pitch = 0.0;
					else if (Globe_First_Last_distance < 1000.0)
						delt_pitch = (Globe_First_Last_distance-100.0)*0.03;
					else if (Globe_First_Last_distance < 10000.0)
						delt_pitch = 2.7+(Globe_First_Last_distance-1000.0)*0.003;
					else if (Globe_First_Last_distance < 100000.0)
						delt_pitch = 5.4+(Globe_First_Last_distance - 10000.0)*0.0003;
					else
						delt_pitch = 8.1+(Globe_First_Last_distance - 100000.0)*0.00003;
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
				}
				else
				{
					pitch = _firstPt.pitch + (_lastPt.pitch-_firstPt.pitch)*ratio;//俯仰
				}				
				osg::Matrixd focusToWorld;
				g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),focusToWorld);
				Globe_matrix = osg::Matrixd::translate(0.0,0.0,distan)*
					osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
					osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
					focusToWorld;

			}
			else
			{
				glbDouble delt_yaw = 360.0*(tim)/Globe_loopTime;
				//_manipulator->Rotate(_focus_temp.x(),_focus_temp.y(),_focus_temp.z(),0.0,delt_yaw);

				const osg::Matrixd matrix = Globe_matrix;

				const osg::Vec3d cameraPos = matrix.getTrans();
				osg::Vec3d cameraLLH;
				convertXYZToLatLongHeight(cameraPos.x(),cameraPos.y(),cameraPos.z(),cameraLLH);
				osg::Matrixd localTWM;
				getLocalToWorldFromLatLong(cameraLLH,localTWM);

				//局部坐标系的旋转
				const osg::Matrixd localRot = matrix * osg::Matrixd::inverse(localTWM);
				const osg::Quat quat   = localRot.getRotate();
				const osg::Vec3d euler = quatToEuler(quat);

				GLdouble mpr_pitch = osg::RadiansToDegrees(euler.x()) - 90.0;
				GLdouble mpr_yaw   =  osg::RadiansToDegrees(-euler.y());
				GLdouble pitch = 0.0;
				GLdouble yaw   = delt_yaw;

				if (mpr_pitch + pitch < -90.0)
				{
					pitch = -90.0 - mpr_pitch;
				}else if (mpr_pitch + pitch > 90.0)
				{
					pitch = 90.0 - mpr_pitch;
				}

				//先转pitch,再转yaw
				osg::Matrixd mat = matrix;
				//选中点从本地坐标到世界坐标的变换矩阵
				osg::Matrixd mat_positionToworld;
				g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(Globe_focus_temp.y()),osg::DegreesToRadians(Globe_focus_temp.x()),Globe_focus_temp.z(),mat_positionToworld);

				osg::Vec3d rotate_yaw = osg::Vec3d(0.0,0.0,1.0);
				osg::Vec3d rotate_ptch = osg::Vec3d(1.0,0.0,0.0) * osg::Matrixd::rotate(osg::DegreesToRadians(-mpr_yaw),0.0,0.0,1.0);

				//选中点地理坐标系下，旋转矩阵
				osg::Matrixd mamat_rotate = osg::Matrixd::rotate(osg::DegreesToRadians(pitch),rotate_ptch) * osg::Matrixd::rotate(osg::DegreesToRadians(yaw),rotate_yaw);
				// 先乘变换矩阵mat_positionToworld的逆矩阵变换到局部坐标系，然后旋转mamat_rotate，再乘变换矩阵mat_positionToworld转到世界坐标
				mat = mat * osg::Matrixd::inverse(mat_positionToworld) * mamat_rotate * mat_positionToworld;
				
				Globe_matrix = mat;
				//后续考虑地上地下计算
			}
		}
	}
}

/*                -----------------------------------                          */

/************************************************************************/
/* camera callback                                                                     */
/************************************************************************/


GlbGlobe::CGlbFPSManipulator::CFPSManipulatorCallback::CFPSManipulatorCallback(const CameraLLHPPY&b,
	const CameraLLHPPY&a,CGlbGlobe*glb,glbBool col /*= true*/):
	currentCamera(b),
	targetCamera(a),
	collision(col),
	globe(glb)
{
	init();
	afterCamera = currentCamera;
}

void GlbGlobe::CGlbFPSManipulator::CFPSManipulatorCallback::init()
{
	willStop		= false;
	haveStart		= false;
	refLocation		= true;
	moveType		= XYZ,
	firstLoop		= true;
	looptime		= 0.0;
	frameStamp		= 0;
	pfunCallBack	= NULL;
	ascendCallBack	= NULL;
}

GLdouble GlbGlobe::CGlbFPSManipulator::CFPSManipulatorCallback::erf(GLdouble x)
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

void CGlbFPSManipulator::CFPSManipulatorCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	if (nv->getVisitorType()==osg::NodeVisitor::UPDATE_VISITOR && !willStop)
	{
		updateCamera(node,nv);
	}
	traverse(node,nv);
}

void CGlbFPSManipulator::CFPSManipulatorCallback::updateCamera(osg::Node*node,osg::NodeVisitor*nv)
{
	static GLdouble startLatitude  = 0.0;
	static GLdouble startLongitude = 0.0;
	static GLdouble startHigh	   = 0.0;

	static GLdouble targetLatitude  = 0.0;
	static GLdouble targetLongitude = 0.0;
	static GLdouble targetHigh      = 0.0;

	frameStamp ++;

	if(firstLoop)
	{
		if(LATITUDELONGITUDE ==  moveType)
		{
			osg::Vec3d llh;
			convertXYZToLatLongHeight(currentCamera.getCameraPos().x(),currentCamera.getCameraPos().y(),
				currentCamera.getCameraPos().z(),llh);
			startLatitude  = osg::DegreesToRadians(llh.x());
			startLongitude = osg::DegreesToRadians(llh.y());
			startHigh      = llh.z();

			convertXYZToLatLongHeight(targetCamera.getCameraPos().x(),targetCamera.getCameraPos().y(),
				targetCamera.getCameraPos().z(),llh);
			targetLatitude  = osg::DegreesToRadians(llh.x());
			targetLongitude = osg::DegreesToRadians(llh.y());
			targetHigh      = llh.z();
		}

		if(beforeLoop(node,startHigh,targetHigh))
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

			GLdouble pitch      = currentCamera.getP() + pitchYawDelta.x();
			GLdouble yaw		= currentCamera.getY() + pitchYawDelta.y();
			afterCamera.setPY(osg::Vec2d(pitch,yaw));
		}
		else if(LATITUDELONGITUDE ==  moveType)
		{
			glbDouble perc = frameStamp / looptime;

			GLdouble K = 3;
			GLdouble t = 2 * K * perc - K;
			GLdouble s = ((erf (sqrt(osg::PI) / 2 * t)) + 1) / 2;

			/*GLdouble K = 50.0;
			GLdouble t = 2 * K * perc - K;
			GLdouble s = 1 / (1 + exp (-t ));
			if(perc == 1.0) s = 1.0;*/

			osg::Vec2d newLL = computeIntermediatePointBetweenToPoint(
				osg::Vec2d(startLatitude,startLongitude),osg::Vec2d(targetLatitude,targetLongitude),perc);

			osg::Vec3d XYZ;
			GLdouble latitude   = osg::RadiansToDegrees(newLL.x());
			GLdouble longtitude = osg::RadiansToDegrees(newLL.y());
			GLdouble high       = startHigh + frameStamp * stepLen;
			convertLatLongHeightToXYZ(latitude,longtitude,high,XYZ);
			afterCamera.setCameraPos(XYZ);

			GLdouble pitch      = currentCamera.getP() + pitchYawDelta.x();
			GLdouble yaw		= currentCamera.getY() + pitchYawDelta.y();
			afterCamera.setPY(osg::Vec2d(pitch,yaw));
		}
		
		else if(GLOBEMANIPULOAOR == moveType)
		{
			//const osg::Matrixd m = Globe_matrix;
			manipulator->flyToFromCGlbGlobeManipulator(nv);
		}
		
	}
}

glbBool CGlbFPSManipulator::CFPSManipulatorCallback::beforeLoop(osg::Node * n,GLdouble&sh,GLdouble&th)
{
	nodeForCallback = n;

	if(XYZ == moveType)
	{
		stepLen = (targetCamera.getCameraPos() - currentCamera.getCameraPos()).length();
		if(looptime == 0.0) looptime = 1.0;
		stepLen /= looptime;
	}

	else if(LATITUDELONGITUDE ==  moveType)
	{
		GLdouble deltaH = th - sh;
		if(looptime == 0.0 ) looptime = 1.0;
		stepLen = deltaH / looptime; //高度变化值
	}

	else if(GLOBEMANIPULOAOR == moveType)
	{
		return true;
	}
	//pitch yaw
	pitchYawDelta.x() = (targetCamera.getP() - currentCamera.getP()) / looptime;
	pitchYawDelta.y() = (targetCamera.getY() - currentCamera.getY()) / looptime;

	return true;
}

void CGlbFPSManipulator::CFPSManipulatorCallback::stopAnimation(glbBool removeUpdate/* = false*/,glbBool resetFunCallback/* = true*/)
{
	if(nodeForCallback)
	{
		willStop  = true;
		haveStart = false;
		if(resetFunCallback)  pfunCallBack = NULL;

		//调整mpr_ascendHigh
		if(ascendCallBack)
		{
			osg::Vec3d LLh;
			osg::Vec3d cameraXYZ;
			//get current camera Z(high)
			if(GLOBEMANIPULOAOR != moveType)
			{
				cameraXYZ = afterCamera.getCameraPos();
			}
			else
			{
				const osg::Matrix mt = manipulator->Globe_matrix;
				cameraXYZ = mt.getTrans();
			}
			convertXYZToLatLongHeight(cameraXYZ.x(),cameraXYZ.y(),cameraXYZ.z(),LLh);
			ascendCallBack(LLh.z());
		}

		if(ascendCallBack)		ascendCallBack = NULL;
		if(removeUpdate)
		{
			glbref_ptr<CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask> task = new
				CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask(nodeForCallback,this);
			globe->mpr_p_callback->AddFadeTask(task.get());
		}
	}
}

const CGlbFPSManipulator::CameraLLHPPY& CGlbFPSManipulator::CFPSManipulatorCallback::getCurrerntCamera()const
{
	return currentCamera;
}

const CGlbFPSManipulator::CameraLLHPPY& CGlbFPSManipulator::CFPSManipulatorCallback::getAfterCameraF()const
{
	return afterCamera;
}

glbBool CGlbFPSManipulator::CFPSManipulatorCallback::checkAnimationStart() const
{
	return haveStart;
}

//////////////////////////////////////////////////////////////////////////


CGlbFPSManipulator::CGlbFPSManipulator(CGlbGlobe* globe):
	mpr_globe(globe),
	mpr_cameraPos(osg::Vec3d(0.0,0.0,0.0))
{
	initParams();
}

CGlbFPSManipulator::~CGlbFPSManipulator()
{
	if(mpr_wasd != nullptr)
	{
		delete[] mpr_wasd;
		mpr_wasd = nullptr;
	}
}

void CGlbFPSManipulator::beforePFSManipulator()
{
	mpr_homeMatrix = getMatrix();
}

void CGlbFPSManipulator::outPFSManipulator()
{
	osg::Camera * main_Camera = mpr_globe->GetView()->GetOsgCamera();
	if(main_Camera)
	{
		//main_Camera->setComputeNearFarMode(mpr_preComputeNearFarMode);
	}
	
	//删除镜头回调
	if(mpr_mouseCallBack.valid())
	{
		mpr_mouseCallBack->stopAnimation(true);
		mpr_mouseCallBack = NULL;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//set and get
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

osg::Matrixd CGlbFPSManipulator::getMatrix() const
{
	osg::Matrixd result;

	osg::Matrixd ltw;
	osg::Vec3d cameraLLH = getCameraLLHDegree();
	getLocalToWorldFromLatLong(cameraLLH,ltw);

#ifdef _PITCHYAW_
	osg::Quat rotate(mpr_rotateV[0], osg::X_AXIS,
					 mpr_rotateV[1], osg::Y_AXIS,
					 mpr_rotateV[2], osg::Z_AXIS);

	result = osg::Matrixd(rotate) * ltw;
#else
	result =  osg::Matrixd(mpr_rotation)  * ltw;
#endif
	return result;
}

osg::Matrixd CGlbFPSManipulator::getInverseMatrix() const
{
	return osg::Matrixd::inverse(getMatrix());
}

void CGlbFPSManipulator::setByMatrix(const osg::Matrixd& mt)
{
	////////////////////////////////////////////////////
	osg::Matrixd matrix = mt;

	const osg::Vec3d cameraPos = matrix.getTrans();
	setCameraXYZ(cameraPos);
	
	osg::Matrixd localTWM;
	getLocalToWorldFromLatLong(getCameraLLHDegree(),localTWM);

	//局部坐标系的旋转
	osg::Matrixd localRot = matrix * osg::Matrixd::inverse(localTWM);

	osg::Quat quat = localRot.getRotate();

	fixVerticalAxis(quat,osg::Vec3d(0,0,1),true);

#ifdef _PITCHYAW_
	const osg::Vec3d euler = quatToEuler(quat);
	
	setCameraPitchYaw(euler);
#else
	mpr_rotation = quat;
#endif
}

void CGlbFPSManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
{
	setByMatrix(osg::Matrixd::inverse(matrix));
}

void CGlbFPSManipulator::SetDriveMoveSpeed(const glbDouble&speed)
{
	if(speed < 0) return;
	mpr_moveSpeed  = speed;
}

void CGlbFPSManipulator::setFlyTimes(const glbDouble&times)
{
	if(times < 0) //目前测试用的是帧数(10 该动画要10完成)
	{
		mpr_animationTimes = 10;
		return ;
	}
	mpr_animationTimes = times;
}

osg::Vec3d CGlbFPSManipulator::getCameraVector() const
{
	osg::Quat rotate = getMatrix().getRotate();
	return	rotate * osg::Vec3d(0,0,-1);
}

osg::Vec3d CGlbFPSManipulator::getCameraRightVector()const
{
	osg::Quat rotate = getMatrix().getRotate();
	return	rotate * osg::Vec3d(1,0,0);
}

osg::Vec3d CGlbFPSManipulator::getCameraUp()const
{
	osg::Quat rotate = getMatrix().getRotate();
	return	rotate * osg::Vec3d(0,1,0);
}

osg::Vec3d CGlbFPSManipulator::getCameraXYZ() const
{
	osg::Vec3d XYZ;

	osg::Vec3d cameraLLH = getCameraLLHDegree();
	convertLatLongHeightToXYZ(cameraLLH.x(),cameraLLH.y(),cameraLLH.z(),XYZ);

	return XYZ;
}

void CGlbFPSManipulator::setCameraXYZ(const osg::Vec3d XYZ)
{
	osg::Vec3d llh;
	convertXYZToLatLongHeight(XYZ.x(),XYZ.y(),XYZ.z(),llh);
	setCameraLLH(llh);
}

void CGlbFPSManipulator::setCameraPitchYaw(const osg::Vec3d py)
{
	#ifdef	_PITCHYAW_
		mpr_rotateV[0] = py.x();
		mpr_rotateV[2] = py.z();
	#else
	#endif
	
}

osg::Vec3d CGlbFPSManipulator::getCameraPitChYaw()const
{
	#ifdef	_PITCHYAW_
		return mpr_rotateV;
	#else
		return osg::Vec2d();
	#endif
}

void CGlbFPSManipulator::setCameraLLH(const osg::Vec3d LLH)
{
	mpr_cameraPos.x() = LLH.x();
	mpr_cameraPos.y() = LLH.y();
	mpr_cameraPos.z() = LLH.z();
}

void CGlbFPSManipulator::setCameraHigh(const glbDouble H,glbBool add /*= true*/)
{
	osg::Vec3d newCameraLLH = getCameraLLHDegree();

	newCameraLLH.z() = (add)? newCameraLLH.z() + H : H;

	setCameraLLH(newCameraLLH);
}

//degree
glbDouble CGlbFPSManipulator::getTerrianElv(const glbDouble&lat,const glbDouble&lon,glbBool nowCamerPos/* = false*/) const
{
	return (nowCamerPos)? mpr_globe->GetElevationAt(lon,lat):
		mpr_globe->GetView()->GetTerrainElevationOfCameraPos();

	//return mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
}

osg::Vec3d CGlbFPSManipulator::getCameraLLHRadin() const
{
	osg::Vec3d latlon;

	osg::Vec3d cameraPos = getCameraLLHDegree();
	latlon.x() = osg::DegreesToRadians(cameraPos.x());
	latlon.y() = osg::DegreesToRadians(cameraPos.y());
	latlon.z() = cameraPos.z();

	return latlon;
}

osg::Vec3d CGlbFPSManipulator::getCameraLLHDegree() const
{
	return mpr_cameraPos;
}

osg::Vec3d CGlbFPSManipulator::getWorldUp()const
{
	osg::Vec3d cameraPos = getCameraLLHRadin();
	
	osg::Vec3d up = osg::Vec3d( cos(cameraPos.y())*cos(cameraPos.x()), 
		sin(cameraPos.y())*cos(cameraPos.x()), sin(cameraPos.x()));

	up.normalize();
	
	return up;
}

glbBool CGlbFPSManipulator::GetCameraPos(osg::Vec3d& cameraPos)
{
	osg::Vec3d pos = getCameraLLHDegree();

	//此处经纬度切换了 
	cameraPos.x() = pos.y();
	cameraPos.y() = pos.x();
	cameraPos.z() = pos.z();

	return true;
}


GlbGlobe::GlbGlobeManipulatorTypeEnum CGlbFPSManipulator::GetType()
{
	return GLB_MANIPULATOR_DRIVE;
}

glbBool CGlbFPSManipulator::IsFlying()
{
	return (mpr_mouseCallBack.valid() && mpr_mouseCallBack->checkAnimationStart()) ? true :false;
}

glbBool CGlbFPSManipulator::StopFlying()
{
	if(mpr_mouseCallBack.valid())
	{
		mpr_mouseCallBack->stopAnimation();
	}
	return true;
}

glbBool CGlbFPSManipulator::SetUnderGroundMode(glbBool mode)
{
	return mpr_globe->SetUnderGroundMode(mode);
}

glbBool CGlbFPSManipulator::SetUndergroundDepth(glbDouble zOrAlt)
{
	return mpr_globe->SetUnderGroundAltitude(zOrAlt);
}

void CGlbFPSManipulator::EnalbeVirtualReferencePlane(glbBool bEnable)
{
	mpr_bUseReferencePlane = bEnable;
}

void CGlbFPSManipulator::SetVirtualReferencePlane(glbDouble zOrAltitude)
{
	mpr_referencePlaneZ = zOrAltitude;
}

glbDouble CGlbFPSManipulator::GetVirtualReferencePlane()
{
	return mpr_referencePlaneZ;
}

glbBool CGlbFPSManipulator::JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble heigh, glbDouble distance,
												glbDouble yaw, glbDouble pitch)
{
	osg::Vec3d focus(xOrlon,yOrlat,heigh);
	setFlyFocus(focus);

	GLdouble nestYaw	= -yaw;
	GLdouble nestPitch	= pitch + 90.0;

	osg::Matrixd focusToWorld;
	getLocalToWorldFromLatLong(osg::Vec3d(focus.y(),focus.x(),focus.z()),focusToWorld);
	const osg::Matrixd mt =  osg::Matrixd::translate(0.0,0.0,distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
		focusToWorld;

	const osg::Vec3d cameraXYZ = mt.getTrans();
	osg::Vec3d llh;
	convertXYZToLatLongHeight(cameraXYZ.x(),cameraXYZ.y(),cameraXYZ.z(),llh);
	mpr_personHigh = 1.5;
	mpr_ascendHith = 0.0;

	setCameraPitchYaw(osg::Vec3d(osg::DegreesToRadians(nestPitch),0.0,osg::DegreesToRadians(nestYaw)));
	rectifyCameraPos(llh,false);

	return true;
}
glbDouble GlbGlobe::CGlbFPSManipulator::GetCameraYaw()
{
	const osg::Vec3d py = getCameraPitChYaw();

	return osg::RadiansToDegrees(py.z());
}

glbDouble GlbGlobe::CGlbFPSManipulator::GetCameraPitch()
{
	const osg::Vec3d py = getCameraPitChYaw();

	return osg::RadiansToDegrees(py.x());
}

//degree
glbBool GlbGlobe::CGlbFPSManipulator::FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt,
	glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
#if 0
	/*
	* 分三个动画:
	*a:先上升镜头,伴随经纬度的移动
	*b:保持镜头高度不变，在经纬度上移动镜头
	*c:降低镜头,,伴随经纬度的移动
	*/
	/*  -----a ----- */
	//镜头后退 抬高

	const GLdouble deltaV = 4000;
	const GLdouble deltaH = 1000000;

	osg::Vec3d lookV		= getCameraVector();
	const osg::Vec3d up		= getWorldUp();
	const osg::Vec3d right	= lookV ^ up;
	lookV					= up ^ right;
	lookV.normalize();

	const osg::Vec3d cameraPos		= getCameraXYZ();
	osg::Vec3d ascendCameraXYZ	= cameraPos - lookV * deltaV;

	osg::Vec3d ascendPoint;
	convertXYZToLatLongHeight(ascendCameraXYZ.x(),ascendCameraXYZ.y(),ascendCameraXYZ.z(),ascendPoint);
	ascendPoint.z() = deltaH;
	convertLatLongHeightToXYZ(ascendPoint.x(),ascendPoint.y(),ascendPoint.z(),ascendCameraXYZ);

	CameraLLHPPY currentCamera;
	currentCamera.setCameraPos(cameraPos);
	currentCamera.setCameraPY(getCameraPitChYaw());

	/*------a ----------*/
	CameraLLHPPY ascendPointCamera;
	ascendPointCamera.setCameraPos(ascendCameraXYZ);
	ascendPointCamera.setPY(osg::Vec2d(osg::DegreesToRadians(8.0),currentCamera.getY()));
	/*------b ----------*/

	//获取镜头位置
	osg::Matrixd focusToWorld;
	getLocalToWorldFromLatLong(osg::Vec3d(yOrlat,xOrlon,zOrAlt),focusToWorld);
	const osg::Matrixd Mt = osg::Matrixd::translate(0.0,0.0,distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
		focusToWorld;
	const osg::Vec3d forDescenCameraXYZ		= Mt.getTrans();
	const GLdouble forDescendCameraYaw		= -yaw;
	const GLdouble forDescendCameraPitch	= pitch + 90.0; 
	osg::Vec3d viewDir						= Mt.getRotate() * osg::Vec3d(0,0,-1);

	CameraLLHPPY descendPointCamera;
	descendPointCamera.setCameraPos(forDescenCameraXYZ);
	descendPointCamera.setPY(osg::Vec2d(osg::DegreesToRadians(forDescendCameraPitch),
							osg::DegreesToRadians(forDescendCameraYaw)));
	/*------c ----------*/
	//descendCamera
	osg::Vec3d descendDestination;
	findDescendCameraPosition(descendPointCamera.getCameraPos(),descendPointCamera.getPY(),viewDir,descendDestination);

	//a
	animationCamer(currentCamera,ascendPointCamera,100.0,1,false,false,
		[=]()
	{
		CameraLLHPPY s = ascendPointCamera;
		CameraLLHPPY t = descendPointCamera;
		osg::Vec3d vd  = viewDir;
		osg::Vec3d ds = descendDestination;
		osg::Vec2d py = t.getPY();

		GlbGlobe::CGlbFPSManipulator * fp = this;
		//b
		this->animationCamer(s,t,100.0,1,false,false,
		[=]()
		{
			//c
			//get now camera
			osg::Vec3d vdd = vd;
			osg::Vec3d dds = ds;
			osg::Vec2d ppy = py;
			GlbGlobe::CGlbFPSManipulator * ffp = fp;

			ffp->descendCamerAnimation(ds,ppy,vdd);
		}	
		);
	}
	);
	
#endif
 
	KeyPoint p1;
	p1.distan = GetDistance();
	p1.pitch  = GetCameraPitch() - 90.0;
	p1.yaw    = -GetCameraYaw();
	osg::Vec3d focuse;
	GetFocusPos(focuse);
	p1.focusPos = focuse;
	const osg::Vec3d p1CameraXYZ = getCameraXYZ();


	KeyPoint p2;
	p2.focusPos		= osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	mpr_focus_temp	= p2.focusPos;
	p2.distan		= distance;
	p2.yaw			= yaw;
	p2.pitch		= pitch;
	
	osg::Matrixd focusToWorld;
	getLocalToWorldFromLatLong(osg::Vec3d(yOrlat,xOrlon,zOrAlt),focusToWorld);
	osg::Matrixd mat = osg::Matrixd::translate(0.0,0.0,distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
		focusToWorld;

	const osg::Vec3d p2CameraXYZ = mat.getTrans();
	const osg::Vec3d viewDir     = mat.getRotate() * osg::Vec3d(0,0,-1);
	//camera distance
	glbDouble first_Last_distance = (p1CameraXYZ - p2CameraXYZ).length();

	//fly to init
	GLdouble loopTime = 3.0;
	flyToFromCGlbGlobeManipulatorinit(FLYTO,p1,p2,first_Last_distance,loopTime);

	CameraLLHPPY s,t; //s t 暂时不使用
	animationCamer(s,t,3.0,2,false,false);
#if 0
	//find descend position
	osg::Vec3d descendXYZ;
	findDescendCameraPosition(p2CameraXYZ,viewDir,descendXYZ);

	//get now camera
	CameraLLHPPY currentCamera;
	currentCamera.setCameraPos(p2CameraXYZ);
	currentCamera.setPY(osg::Vec2d(osg::DegreesToRadians(pitch + 90.0),osg::DegreesToRadians(-yaw)));

	CameraLLHPPY targetCamera;
	targetCamera.setCameraPos(descendXYZ);
	targetCamera.setPY(osg::Vec2d(osg::PI_2,osg::DegreesToRadians(GetYaw())));

	CameraLLHPPY s,t; //s t 暂时不使用
	animationCamer(s,t,3.0,2,false,false,
		[=]()
	{
		CameraLLHPPY ss = currentCamera;
		CameraLLHPPY tt = targetCamera;

		animationCamer(ss,tt,100.0,1,false,false);
	}
		);
#endif

	return true;
}

glbBool GlbGlobe::CGlbFPSManipulator::FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, glbBool repeat)
{
	/*   参考GlbGlobeManipulator操控器 FlyAround  */

	//相机位置不变,计算yaw,pitch,distance
	osg::Vec3d focusPosXYZ;
	convertLatLongHeightToXYZ(yOrlat,xOrlon,zOrAlt,focusPosXYZ);
	
	const osg::Vec3d cameraXYZ = getCameraXYZ();
	glbDouble distance		   =  (focusPosXYZ-cameraXYZ).length();

	KeyPoint firstKeyPoint;
	KeyPoint lastKeyPoint;

	GetFocusPos(firstKeyPoint.focusPos);
	firstKeyPoint.distan	= GetDistance();
	firstKeyPoint.yaw		= GetYaw();
	firstKeyPoint.pitch		= GetPitch();

	lastKeyPoint.focusPos	=  osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	lastKeyPoint.distan     = distance;

	osg::Matrixd mat;
	getLocalToWorldFromXYZMatrix(focusPosXYZ,mat);
	osg::Vec3d LocalPos = cameraXYZ*osg::Matrixd::inverse(mat);
	LocalPos.normalize();

	if (LocalPos.z() > 1.0)
	{
		lastKeyPoint.pitch = -90.0;
		lastKeyPoint.yaw = firstKeyPoint.yaw;
	}
	else if (LocalPos.z() < -1.0)
	{
		lastKeyPoint.pitch = 90.0;
		lastKeyPoint.yaw = firstKeyPoint.yaw;
	}
	else{
		lastKeyPoint.pitch = osg::RadiansToDegrees(asin(-LocalPos.z()));
		lastKeyPoint.yaw = osg::RadiansToDegrees(atan2(-LocalPos.x(),-LocalPos.y()));
	}

	flyToFromCGlbGlobeManipulatorinit(FLYAROUND,firstKeyPoint,lastKeyPoint,distance,3.0);

	CameraLLHPPY s,t; //s t 暂时不使用
	animationCamer(s,t,3.0,2,false,false);

	return true;
}

glbDouble GlbGlobe::CGlbFPSManipulator::GetYaw()
{
	return osg::RadiansToDegrees(-getCameraPitChYaw()[2]);
}

glbDouble GlbGlobe::CGlbFPSManipulator::GetPitch()
{
	//return osg::RadiansToDegrees(osg::PI_2 - getCameraPitChYaw()[0]);
	return osg::RadiansToDegrees(getCameraPitChYaw()[0] - osg::PI_2);
}

glbBool GlbGlobe::CGlbFPSManipulator::GetFocusPos(osg::Vec3d& focusPos)
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

	osg::Vec3d llh;
	convertXYZToLatLongHeight(focus.x(),focus.y(),focus.z(),llh);

	//该操控器和自由操控器经纬度方式不同
	focusPos.x() = llh.y();
	focusPos.y() = llh.x();
	focusPos.z() = llh.z();

	return true;
}

glbDouble GlbGlobe::CGlbFPSManipulator::GetDistance()
{
	osg::Vec3d focus;
	GetFocusPos(focus);

	osg::Vec3d focusXYZ;
	convertLatLongHeightToXYZ(focus.y(),focus.x(),focus.z(),focusXYZ);

	return (focusXYZ - getCameraXYZ()).length();
}

void CGlbFPSManipulator::initParams(void)
{
#ifdef _PITCHYAW_
	mpr_rotateV     = osg::Vec3d(osg::PI_2,0,0); //默认镜头是朝向Z轴负方向，首先绕X正向旋转90，镜头指向正北
	mpr_rotateStep  = 15.0;
#else
	mpr_rotation		= osg::Quat(osg::PI_2,osg::Vec3(1,0,0));
#endif
	mpr_moveSpeed				= 1.0;
	mpr_animationTimes			= 10.0;
	mpr_personHigh				= 1.5;
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
}

//degree
void CGlbFPSManipulator::rectifyCameraPos(osg::Vec3d&newPos,glbBool needFootCollision/* = true*/)
{

	//static GLdouble delta =

	glbBool isUnderGround    = mpr_camera_is_underGround;
	glbBool enableUnderGound = mpr_globe->IsUnderGroundMode();

	osg::Vec3d newPosXYZ;
	convertLatLongHeightToXYZ(newPos.x(),newPos.y(),newPos.z(),newPosXYZ);

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
				osg::Vec3d LLH;
				convertXYZToLatLongHeight(otherPoint.x(),otherPoint.y(),otherPoint.z(),LLH);

				if((newPos.z() - LLH.z()) < (ELEVATE_THRESHOLD_FPS + mpr_personHigh))
				{
					newPos.z() = LLH.z() + (ELEVATE_THRESHOLD_FPS + mpr_personHigh);
				}

				setCameraLLH(newPos);
				return;
			}
		}

		if(!enableUnderGound)
		{
			glbDouble ele = getTerrianElv(newPos.x(),newPos.y());
			
			if(newPos.z() < ele + ELEVATE_THRESHOLD_FPS + mpr_personHigh)
			{
				newPos.z() = ele + ELEVATE_THRESHOLD_FPS + mpr_personHigh;
			}
		}

		setCameraLLH(newPos);
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
				osg::Vec3d LLH;
				convertXYZToLatLongHeight(otherPoint.x(),otherPoint.y(),otherPoint.z(),LLH);

				if((newPos.z() - LLH.z()) < (ELEVATE_THRESHOLD_FPS + mpr_personHigh))
				{
					newPos.z() = LLH.z() + (ELEVATE_THRESHOLD_FPS + mpr_personHigh);
				}

				setCameraLLH(newPos);
				return;
			}
		}

		//与地下参考面交点
		{
			glbDouble ele = mpr_globe->GetUnderGroundAltitude();
			
			if(newPos.z() < ele + GROUND_ALTITUDE_THRESHOLD_FPS + mpr_personHigh)
			{
				newPos.z() = ele + GROUND_ALTITUDE_THRESHOLD_FPS + mpr_personHigh;
			}
		}

		{
			//与镜头上部地形缓冲高度
			GLdouble ele = getTerrianElv(newPos.x(),newPos.y());

			glbDouble deltaZ = ele - newPos.z();

			if(std::abs(deltaZ) < CAMERA_UNDERGROUND_TOP_HOLDPFS)
			{
				GLdouble camerH;
				//此时相机处于缓冲区内,直接相机进入地下或者进入地上
				if(mpr_camera_is_underGround) // dixia
				{
					camerH =   mpr_personHigh + ele + std::abs(deltaZ);
				}
				
				newPos.z() = camerH;
				mpr_camera_is_underGround = false;
			}
		}

		setCameraLLH(newPos);
	}
	
}

glbBool CGlbFPSManipulator::moveCameraPos(const osg::Vec3d dir,glbDouble len,glbBool collision,glbBool ref /*= true*/)
{
	if(dir.length2() <= 0) return false;

	osg::Vec3d dirNormal = dir;
	dirNormal.normalize();

	osg::Vec3d newCameraXYZ;
#ifndef SWEPTSPHERE
	if(collision && mpr_globe->CheckCullisionModel())
	{
		if(cullisionDir(dirNormal,len))
		{
			return false;
		}
	}

	newCameraXYZ  = getCameraXYZ() + dirNormal * len;
#else
	newCameraXYZ = moveCameraBySweptSphere(getCameraXYZ(),dir * len);
#endif

	osg::Vec3d newLLH;
	convertXYZToLatLongHeight(newCameraXYZ.x(),newCameraXYZ.y(),newCameraXYZ.z(),newLLH);

	if(ref)
	{
		rectifyCameraPos(newLLH);
	}
	else
	{
		setCameraLLH(newLLH);
	}

	return true;
}

void CGlbFPSManipulator::fixVerticalAxis(osg::Quat& rotation, const osg::Vec3d& localUp, glbBool disallowFlipOver)
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

void GlbGlobe::CGlbFPSManipulator::rotateAboutPoint(const osg::Vec3d aroundPoint,glbDouble pitch,glbDouble yaw)
{
	//在局部坐标系先绕Y旋转yaw,然后在旋转后的X轴旋转pitch
	osg::Matrixd positionToworld;
	getLocalToWorldFromXYZMatrix(aroundPoint,positionToworld);

	osg::Quat quat(osg::DegreesToRadians(GetCameraYaw()),osg::Vec3(0.0,0.0,1.0));
	osg::Vec3d rotate_pitch = quat *  osg::Vec3d(1.0,0.0,0.0); //经过旋转后X轴的向量

	//鼠标拖动为了连贯性 可以考虑在局部坐标系下 先绕Z旋转，然后在绕旋转后的X进行旋转
	osg::Quat rotate = osg::Quat(osg::DegreesToRadians(pitch),rotate_pitch) * 
						osg::Quat(osg::DegreesToRadians(yaw),osg::Vec3d(0.0,0.0,1.0));

	// 先乘变换矩阵positionToworld的逆矩阵变换到局部坐标系，然后旋转mamat_rotate，再乘变换矩阵positionToworld转到世界坐标
	osg::Matrixd newMat = getMatrix() * osg::Matrixd::inverse(positionToworld) * osg::Matrixd(rotate) * positionToworld;

	//获取新镜头的位置
	osg::Vec3d newCameraXYZ = newMat.getTrans();
	osg::Vec3d newCameraLLH;
	convertXYZToLatLongHeight(newCameraXYZ.x(),newCameraXYZ.y(),newCameraXYZ.z(),newCameraLLH);
	
	//调整镜头的位置
	rectifyCameraPos(newCameraLLH,false);

	//局部坐标系的旋转
	osg::Matrixd localTWM;
	getLocalToWorldFromLatLong(getCameraLLHDegree(),localTWM);
	quat = (newMat * osg::Matrixd::inverse(localTWM)).getRotate();
	
	//保证镜头不会超过着Z轴头顶和脚下
	fixVerticalAxis(quat,osg::Vec3d(0,0,1),true);
	
	//设置Pitch yaw
	const osg::Vec3d euler = quatToEuler(quat);
	setCameraPitchYaw(euler);
}

void CGlbFPSManipulator::rotatePitchYaw(glbDouble yaw,glbDouble pitch,glbBool disallowFlipOver)
{
#ifdef  _PITCHYAW_

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
	
#else
	osg::Matrixd rotation_matrix;
	rotation_matrix.makeRotate(mpr_rotation);

	osg::Vec3d up = osg::Vec3d(0.0,1.0,0.0) * rotation_matrix;
	osg::Vec3d lv = osg::Vec3d(0.0,0.0,-1.0) * rotation_matrix;
	osg::Vec3d sv = osg::Vec3d(1.0,0.0,0.0) * rotation_matrix;

	osg::Quat yaw_rotation;
	yaw_rotation.makeRotate(yaw,up);
	mpr_rotation *= yaw_rotation;

	osg::Quat pitch_rotation;
	pitch_rotation.makeRotate(pitch,sv);
	mpr_rotation *= pitch_rotation;

	//此处 lockup轴是将local坐标系来算的,将地表的up轴在局部坐标系下计算,
	//防止视角绕着Z轴旋转
	fixVerticalAxis(mpr_rotation,osg::Vec3d(0,0,1),true);
#endif
	
}

glbBool CGlbFPSManipulator::getPushPoint(osg::Vec3d&pushPoint,GLdouble&dis) const
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
		osg::Vec3d pushPos;
		convertLatLongHeightToXYZ(lat,lon,alt,pushPos);
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
		convertLatLongHeightToXYZ(lat,lon,alt,pushPoint);
		osg::Vec3d cameraPos_w = getMatrix().getTrans();
		dis = (cameraPos_w - pushPoint).length();
	}

	return true;
}

void GlbGlobe::CGlbFPSManipulator::animationCamer(const CameraLLHPPY s,const CameraLLHPPY t,glbDouble loopTime,
	unsigned int type,glbBool collision/* = true*/,glbBool refL /*= true*/,ManipulatorOutCallBack pf/* = nullptr*/,glbBool cpf /*= true*/)
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
		mpr_mouseCallBack = new CFPSManipulatorCallback(s,t,mpr_globe);
		mpr_mouseCallBack->setManipulator(this);
	}

	mpr_mouseCallBack->setAnimationTime(loopTime);

	CFPSManipulatorCallback::MoveType tp;
	switch (type)
	{
	case 0:
		tp = CFPSManipulatorCallback::XYZ;
		break;
	case 1:
		tp = CFPSManipulatorCallback::LATITUDELONGITUDE ;
		break;
	case 2:
		tp = CFPSManipulatorCallback::GLOBEMANIPULOAOR ;
		break;
	default:
		tp = CFPSManipulatorCallback::XYZ;
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

glbBool GlbGlobe::CGlbFPSManipulator::globeManipulator2FPSManipulator()
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

		animationCamer(currentCamera,targetCamera,60.0,1,false,false);

		return true;
	}

	return false;
}

glbBool GlbGlobe::CGlbFPSManipulator::fPSManipulator2GlobeManipulator(ManipulatorOutCallBack pf)
{
	//镜头后退500,抬高200
	
	const GLdouble deltaV = 400;
	const GLdouble deltaH = 200;
	
	osg::Vec3d lookV		= getCameraVector();
	const osg::Vec3d up		= getWorldUp();
	const osg::Vec3d right	= lookV ^ up;
	lookV					= up ^ right;
	lookV.normalize();

	const osg::Vec3d cameraPos		= getCameraXYZ();
	osg::Vec3d newCameraPos	= cameraPos - lookV * deltaV;
	osg::Vec3d llh;
	convertXYZToLatLongHeight(newCameraPos.x(),newCameraPos.y(),newCameraPos.z(),llh);
	llh.z() += 200;
	convertLatLongHeightToXYZ(llh.x(),llh.y(),llh.z(),newCameraPos);

	CameraLLHPPY currentCamera;
	currentCamera.setCameraPos(cameraPos);
	currentCamera.setCameraPY(getCameraPitChYaw());

	CameraLLHPPY targetCamera;
	targetCamera.setCameraPos(newCameraPos);
	targetCamera.setPY(osg::Vec2d(osg::DegreesToRadians(60.0),currentCamera.getY()));
	
	animationCamer(currentCamera,targetCamera,80.0,1,false,false,pf,false);

	return true;
}

void GlbGlobe::CGlbFPSManipulator::setCameraPitchYawAlex(const osg::Vec2 py)
{
#ifdef	_PITCHYAW_
	mpr_rotateV[0] = py.x();
	mpr_rotateV[2] = py.y();
#else
#endif
}

glbBool GlbGlobe::CGlbFPSManipulator::findDescendCameraPosition(const osg::Vec3d currentXYZ,const osg::Vec3d viewDir,osg::Vec3d&XYZ)
{
	osg::Vec3d targetPos;
	//get the intersect of view and scene

	//相机在地上还是地下
	osg::Vec3d LLH;
	convertXYZToLatLongHeight(currentXYZ.x(),currentXYZ.y(),currentXYZ.z(),LLH);
	GLdouble elv = getTerrianElv(LLH.x(),LLH.y());

	GLdouble delta =  LLH.z() - elv;

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
			osg::Vec3d llh;
			convertXYZToLatLongHeight(targetPos.x(),targetPos.y(),targetPos.z(),llh);

			const GLdouble elv = getTerrianElv(llh.x(),llh.y());

			llh.z() = elv + ELEVATE_THRESHOLD_FPS + mpr_personHigh;

			convertLatLongHeightToXYZ(llh.x(),llh.y(),llh.z(),XYZ);

			return true;
		}
	}
	return false;
}

void GlbGlobe::CGlbFPSManipulator::descendCamerAnimation(const osg::Vec3d currentXYZ,const osg::Vec2d py,const osg::Vec3d viewDir)
{
	osg::Vec3d descendPositionXYZ;
	findDescendCameraPosition(currentXYZ,viewDir,descendPositionXYZ);

	//get now camera
	CameraLLHPPY currentCamera;
	currentCamera.setCameraPos(currentXYZ);
	currentCamera.setPY(py);

	CameraLLHPPY targetCamera;
	targetCamera.setCameraPos(descendPositionXYZ);
	targetCamera.setPY(osg::Vec2d(osg::PI_2,currentCamera.getY()));

	animationCamer(currentCamera,targetCamera,120.0,1,false,false);
}

void GlbGlobe::CGlbFPSManipulator::computeYawPitchDistanceFocusePararmFromMatrix(const osg::Matrixd mt,glbDouble& yaw, glbDouble& pitch, glbDouble& distance, osg::Vec3d& focuse)
{
	osg::Matrixd localTWM;
	getLocalToWorldFromXYZMatrix(mt.getTrans(),localTWM);

	//局部坐标系的旋转
	osg::Matrixd localRot = mt * osg::Matrixd::inverse(localTWM);

	osg::Quat quat = localRot.getRotate();

	fixVerticalAxis(quat,osg::Vec3d(0,0,1),true);

	const osg::Vec3d euler = quatToEuler(quat);
	
	yaw = -euler.z();
	pitch = euler.x() - osg::PI_2;
}

void GlbGlobe::CGlbFPSManipulator::ascendCamera(GLdouble addHigh)
{
	osg::Vec3d cameraLLh = getCameraLLHDegree();
	cameraLLh.z() += addHigh;
	rectifyCameraPos(cameraLLh);
}

GLdouble GlbGlobe::CGlbFPSManipulator::getCameraAscendHigh() const
{
	return mpr_ascendHith;
}

void GlbGlobe::CGlbFPSManipulator::cameraCallBack(glbDouble cameraZ)
{
	//依据此时镜头的Z值，调整mpr_ascendHigh
	const osg::Vec3d cameraLLH  = getCameraLLHDegree();
	const GLdouble elv			= getTerrianElv(cameraLLH.x(),cameraLLH.y());
	
	mpr_ascendHith = cameraZ - mpr_personHigh - elv;

}

glbBool GlbGlobe::CGlbFPSManipulator::flyTo2(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance,
	glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
	GLdouble mpr_distance = GetDistance();

	osg::Vec3d focusPoint;
	GetFocusPos(focusPoint);

	GLdouble mpr_pitch = GetCameraPitch() - 90.0;
	GLdouble mpr_yaw = -GetCameraYaw();

	if (mpr_distance > 3*distance)	//拉近焦点
	{
		const osg::Vec3d cameraXYZ = getCameraXYZ();

		osg::Vec3d focusXYZ;
		convertLatLongHeightToXYZ(focusPoint.y(),focusPoint.x(),focusPoint.z(),focusXYZ);
		osg::Vec3d lookAt = focusXYZ - cameraXYZ;
		lookAt.normalize();
		focusXYZ = cameraXYZ + lookAt * 3 * distance;
		
		mpr_distance = 3 * distance;

		osg::Matrixd mat;
		getLocalToWorldFromXYZMatrix(focusXYZ,mat);
		osg::Vec3d LocalPos = cameraXYZ*osg::Matrixd::inverse(mat);
		LocalPos.normalize();
		mpr_pitch = osg::RadiansToDegrees(asin(-LocalPos.z()));

		convertXYZToLatLongHeight(focusXYZ.x(),focusXYZ.y(),focusXYZ.z(),focusPoint);
		{
			GLdouble t = focusPoint.y();
			focusPoint.y() = focusPoint.x();
			focusPoint.x() = t;
		}
	}


	KeyPoint pt0;
	pt0.focusPos = focusPoint;
	pt0.distan   = mpr_distance;
	pt0.yaw      = mpr_yaw;
	pt0.pitch	 = mpr_pitch;

	KeyPoint pt1;
	pt1.focusPos = osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	mpr_focus_temp = pt1.focusPos;
	pt1.distan = distance;
	pt1.yaw = yaw;
	pt1.pitch = pitch;

	flyToFromCGlbGlobeManipulatorinit(FLYTO2,pt0,pt1,0.0,1.0);

	CameraLLHPPY s,t; //s t 暂时不使用
	animationCamer(s,t,3.0,2,false,false);

	return true;
}

void GlbGlobe::CGlbFPSManipulator::setFlyFocus(const osg::Vec3d focusPoint)
{
	mpr_focus_temp = focusPoint;
}

glbBool GlbGlobe::CGlbFPSManipulator::UpdateCameraElevate(glbDouble elevation)
{

	return true;
}

void GlbGlobe::CGlbFPSManipulator::moveCamerUnOrUpGround()
{
	if(IsFlying())
	{
		StopFlying();
	}

	glbBool currentCameraUnderGround	= mpr_camera_is_underGround;

	if(mpr_globe->IsUnderGroundMode() ) //开启
	{
		const osg::Vec3d current = getCameraLLHDegree();
		mpr_ascendHith = 0.0; //上下切换镜头，镜头依据参考面，没有ascendHigh

		if(!currentCameraUnderGround)
		{
			//move underGround
			GLdouble cameraZ = mpr_globe->GetUnderGroundAltitude() + GROUND_ALTITUDE_THRESHOLD_FPS;

			mpr_camera_is_underGround = true;

			rectifyCameraPos(osg::Vec3d(current.x(),current.y(),cameraZ),true);
		}
		else
		{
			//move upGround
			GLdouble elv = getTerrianElv(current.x(),current.z());
			elv += 6.0; //稍稍抬高，防止与三维场景有交点

			mpr_camera_is_underGround = false;
			
			rectifyCameraPos(osg::Vec3d(current.x(),current.y(),elv),true);
		}

		mpr_globe->NotifyCameraIsUnderground(mpr_camera_is_underGround);	

	}
}

glbDouble GlbGlobe::CGlbFPSManipulator::GetDriveMoveSpeed()
{
	return mpr_moveSpeed;
}

CGlbFPSManipulator::CameraLLHPPY CGlbFPSManipulator::computeTargetPointCameraPPY(const osg::Vec3d&targe)
{
	CameraLLHPPY result;
	
	result.setCameraPos(targe);

	return result;
}



