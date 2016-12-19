#include "StdAfx.h"
#include "GlbGlobeClipHandler.h"
#include <osgViewer/View>
#include <osg/Math>
#include <fstream>
using namespace GlbGlobe;

CGlbGlobeClipHandler::CGlbGlobeClipHandler(CGlbGlobe* pglobe)
{
	mpr_p_globe = pglobe;
	mpr_isUserFileExist = loadUserFile("nearfarRatioUserSetting.txt");
}


CGlbGlobeClipHandler::~CGlbGlobeClipHandler(void)
{
}

glbBool CGlbGlobeClipHandler::loadUserFile(std::string filepath)
{
	std::fstream cin(filepath,std::ios::in);
	if (cin)
	{
		char a; //','
		double altitude,nearfarRatio;
		while (!cin.eof())
		{
			cin >> altitude >> a >> nearfarRatio;
			mpr_cameraAltitudeNearfarRatio[altitude] = nearfarRatio;
		}
		cin.close();
		// 如果mpr_cameraAltitudeNearfarRadio数组>0
		if (mpr_cameraAltitudeNearfarRatio.size()>0)
			return true;
	}
	return false;
}

void CGlbGlobeClipHandler::operator()( osg::Node* node, osg::NodeVisitor* nv )
{ 
	if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
	{  		
		if (mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			CGlbGlobeView* pview = mpr_p_globe->GetView();
			osg::Camera* pCamera = pview->GetOsgCamera();
			osg::ref_ptr<osg::CullSettings::ClampProjectionMatrixCallback> clamper = mpr_p_clampers[pCamera];
			if (!clamper.valid())
			{  
				clamper = new CustomProjClamper;
				pCamera->setClampProjectionMatrixCallback(clamper.get());
				mpr_p_clampers[pCamera] = clamper;
			}								 
			CustomProjClamper* pcustom = static_cast<CustomProjClamper*>(clamper.get());
			pcustom->_isFlat = false;
			osg::Vec3d eye,eye_w,eye_t;//, center, up;
			pview->GetCameraPos(eye);
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(eye.y()),osg::DegreesToRadians(eye.x()),eye.z(),eye_w.x(),eye_w.y(),eye_w.z());
			glbDouble terrainElevationOfCameraPos = pview->GetTerrainElevationOfCameraPos();
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(eye.y()),osg::DegreesToRadians(eye.x()),terrainElevationOfCameraPos,eye_t.x(),eye_t.y(),eye_t.z());
			if (abs(eye.z() - terrainElevationOfCameraPos) > 2.0)
				pcustom->_minDistance = 0.3;
			else
				pcustom->_minDistance = 0.03;
			switch(pview->GetManipulatorType())
			{
			case GLB_MANIPULATOR_DRIVE:
			case GLB_MANIPULATOR_ANIMATIONPATH:
			case GLB_MANIPULATOR_FREE:

				{ 
					pcustom->_minDistance = 0.3;
					double pitch = 	osg::DegreesToRadians(pview->GetPitch()) + osg::PI_2;
					glbDouble eye_terrain = eye.z() - terrainElevationOfCameraPos;	//相机地形高度
				
#ifdef XinJiangTaLiMuPenDiXiangMu
					pcustom->_bUnderGround = false;
#endif
					if (!mpr_p_globe->IsUnderGroundMode())	//未开启地下模式
					{
						glbDouble dis_eyeTocenter = eye_w.length();	//相机球心距离的平方
						//设置远裁剪面,设远一点，留些富余
						glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - osg::WGS_84_RADIUS_POLAR * osg::WGS_84_RADIUS_POLAR); //相机远裁剪面
						pcustom->_maxFar = eye_tangent;
						if (eye_terrain > 10000.0)
							pcustom->_nearFarRatio = 0.01;	//0.01
						else if (eye_terrain > 3000.0)
							pcustom->_nearFarRatio = 0.005;	
						else if (eye_terrain > 1000.0)						
							pcustom->_nearFarRatio = 0.0005;							
						else if (eye_terrain > 100.0)						
							pcustom->_nearFarRatio = 0.0001;							
						else if (eye_terrain > 5.0)					
							pcustom->_nearFarRatio = 0.00001;
						else// 地面2米以内
						{
							pcustom->_nearFarRatio = 0.000001;
						}
					}
					else
					{
						glbDouble dis_eyeTocenter = eye_w.length();	//相机球心距离的平方
						//设置远裁剪面,设远一点，留些富余
						glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - osg::WGS_84_RADIUS_POLAR * osg::WGS_84_RADIUS_POLAR); //相机远裁剪面
						pcustom->_maxFar = eye_tangent;

						if(mpr_cameraAltitudeNearfarRatio.size()>0)
						{// 用户从外部文件定义了相机高度和nearFarRatio的对应数组 2015.12.3 马林
							double upperDistance = DBL_MAX;
							double lowerDistance = DBL_MIN;
							double nearFarRatio = 0.01;
							std::map<double,double>::reverse_iterator itr = mpr_cameraAltitudeNearfarRatio.rbegin();
							while(itr != mpr_cameraAltitudeNearfarRatio.rend())
							{
								double eye2terrain = itr->first;								
								if (eye_terrain > eye2terrain)
								{// 自上而下 查找第一个比当前相机距地高度小的itr
									lowerDistance = eye2terrain;
									nearFarRatio = itr->second;
									break;
								}
								else{
									upperDistance = eye2terrain;
								}
								++itr;
							}

							pcustom->_nearFarRatio = nearFarRatio;
						}
						else// 无用户定义文件方式
						{
							if (eye_terrain > 10000.0)
								pcustom->_nearFarRatio = 0.01;	
							else if (eye_terrain > 3000.0)
								pcustom->_nearFarRatio = 0.005;	
							else if (eye_terrain > 1000.0)						
								pcustom->_nearFarRatio = 0.0005;							
							else if (eye_terrain > 100.0)						
								pcustom->_nearFarRatio = 0.0001;							
							else if (eye_terrain > 5.0)					
								pcustom->_nearFarRatio = 0.00001;
							else if (eye_terrain > 0)
								pcustom->_nearFarRatio = 0.0000015;
							else if (eye_terrain > -2.0)
							{							
#ifdef XinJiangTaLiMuPenDiXiangMu
								pcustom->_nearFarRatio = 0.000005;	//0.000001;
#else
								pcustom->_nearFarRatio = 0.0000055;
#endif
							}
							else
							{
#ifdef XinJiangTaLiMuPenDiXiangMu
								pcustom->_bUnderGround = true;
								pcustom->_refFar = osg::WGS_84_RADIUS_POLAR*0.15;//0.2
								pcustom->_nearFarRatio = 0.00005;	//0.00001
#else
								pcustom->_nearFarRatio = 0.000001;	//0.00001
#endif
							}
						}
					}
					//if (!mpr_p_globe->IsUnderGroundMode())	//未开启地下模式
					//{
					//	pcustom->_minDistance = 0.3;
					//	glbDouble mpr_minNearFarRatio = 0.0000016;	//最小远近裁剪面比率
					//	glbDouble mpr_maxNearFarRatio = 0.000008;	//最大远近裁剪面比率

					//	glbDouble dis_eyeTocenter = eye_w.length();	//相机球心距离的平方
					//	//-90°看时，地球在眼睛里张角的一半
					//	if (dis_eyeTocenter < WGS_84_RADIUS_POLAR)
					//		return;
					//	glbDouble angle = asin(WGS_84_RADIUS_POLAR/dis_eyeTocenter);
					//	//相机视角
					//	glbDouble angle_cam,asp,znear,zfar;
					//	pCamera->getProjectionMatrixAsPerspective(angle_cam,asp,znear,zfar);


					//	angle_cam = osg::DegreesToRadians(angle_cam);
					//	glbDouble eye_terrain = eye.z() - terrainElevationOfCameraPos;	//相机地形高度
					//	//设置远裁剪面,设远一点，留些富余
					//	glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - osg::WGS_84_RADIUS_POLAR * osg::WGS_84_RADIUS_POLAR); //相机远裁剪面
					//	double pitch = 	osg::DegreesToRadians(pview->GetPitch()) + osg::PI_2;

					//	//if (pitch+angle_cam < angle)
					//	//{

					//	//}else{
					//		if (pitch < angle)
					//			pcustom->_maxFar = eye_tangent*cos(angle - pitch);
					//		else
					//			pcustom->_maxFar = eye_tangent;
					//		if (eye_terrain < 100.0)
					//		{
					//			pcustom->_maxFar *= osg::clampBetween(eye_terrain/100.0,0.1,1.0);
					//		}
					//	//}
					//	pitch = osg::RadiansToDegrees(pitch);
					//	////考虑不同情况
					//	//if (eye.z() <= 200000.0 && eye.z() >= 15000.0 && fabs(eye.y()) > 35.0 && fabs(pitch) >= 70)
					//	//	pcustom->_maxFar *= 1.3;
					//	//else if (eye.z() < 15000.0 && fabs(eye.y()) <= 55.0)
					//	//	pcustom->_maxFar *= 1.5;
					//	//else if (eye.z() < 15000.0 && fabs(eye.y()) > 55.0 && fabs(eye.y()) <= 69.0)
					//	//	pcustom->_maxFar *= 3.0;
					//	//else if (eye.z() < 15000.0 && fabs(eye.y()) > 69.0 && fabs(eye.y()) <= 78.0)
					//	//	pcustom->_maxFar *= 5.0;
					//	//else if (eye.z() < 15000.0 && fabs(eye.y()) > 78.0 && fabs(eye.y()) <= 84.0)
					//	//	pcustom->_maxFar *= 8.0;
					//	//else if (eye.z() < 15000.0 && fabs(eye.y()) > 84.0)
					//	//	pcustom->_maxFar *= 15.0; 
					//	float vr = (osg::clampBetween(eye_terrain, 0.0, eye.z())-0.0)/(eye.z()-0.0);
					//	pcustom->_nearFarRatio = mpr_minNearFarRatio + vr * (mpr_maxNearFarRatio-mpr_minNearFarRatio);
					//}else{		//开启地下模式
					//	if (eye.z() - terrainElevationOfCameraPos < 0.0)	//地下时
					//	{
					//		glbDouble mpr_minNearFarRatio = 0.00000002;
					//		glbDouble mpr_maxNearFarRatio = 0.0000001;
					//		double dis_eyeTocenter = eye_w.length();	//相机球心距离的平方
					//		//-90°看时，地球的地下参考面在眼睛里张角的一半
					//		glbDouble underGroundAltitude = mpr_p_globe->GetUnderGroundAltitude();
					//		glbDouble radius = WGS_84_RADIUS_POLAR + underGroundAltitude;
					//		if (radius > dis_eyeTocenter)
					//			radius = dis_eyeTocenter;
					//		glbDouble angle = asin(radius/dis_eyeTocenter);
					//		glbDouble eye_terrain = eye.z() - underGroundAltitude;	//相机地下参考面高度
					//		//设置远裁剪面,设远一点，留些富余
					//		glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - radius*radius); //相机远裁剪面
					//		double pitch = 	osg::DegreesToRadians(pview->GetPitch()) + osg::PI_2;
					//		if (pitch < angle)
					//			pcustom->_maxFar = eye_tangent*cos(angle - pitch);
					//		else
					//			pcustom->_maxFar = eye_tangent;
					//		pcustom->_maxFar *= 0.1; 
					//		float vr = (osg::clampBetween(eye_terrain, 0.0, eye.z())-0.0)/(eye.z()-0.0);
					//		pcustom->_nearFarRatio = mpr_minNearFarRatio + vr * (mpr_maxNearFarRatio-mpr_minNearFarRatio);
					//		//glbDouble _near = pcustom->_maxFar*pcustom->_nearFarRatio;
					//	}else if (eye.z() - terrainElevationOfCameraPos < 10.0)
					//	{
					//		glbDouble mpr_minNearFarRatio = 0.000000005;
					//		glbDouble mpr_maxNearFarRatio = 0.000000025;

					//		double dis_eyeTocenter = eye_w.length();	//相机球心距离的平方
					//		//-90°看时，地球在眼睛里张角的一半
					//		if (dis_eyeTocenter < WGS_84_RADIUS_POLAR)
					//			dis_eyeTocenter = WGS_84_RADIUS_POLAR;
					//		glbDouble angle = asin(WGS_84_RADIUS_POLAR/dis_eyeTocenter);
					//		angle = angle > osg::PI/6 ? osg::PI/6 : angle;
					//		glbDouble eye_terrain = eye.z() - terrainElevationOfCameraPos;	//相机地形高度
					//		//设置远裁剪面,设远一点，留些富余
					//		glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - osg::WGS_84_RADIUS_POLAR * osg::WGS_84_RADIUS_POLAR); //相机远裁剪面
					//		double pitch = 	osg::DegreesToRadians(pview->GetPitch()) + osg::PI_2;
					//		if (pitch < angle)
					//			pcustom->_maxFar = eye_tangent*cos(angle - pitch);
					//		else
					//			pcustom->_maxFar = eye_tangent;
					//		//pcustom->_maxFar *= 0.1;
					//		float vr = (osg::clampBetween(eye_terrain, 0.0, eye.z())-0.0)/(eye.z()-0.0);
					//		pcustom->_nearFarRatio = mpr_minNearFarRatio + vr * (mpr_maxNearFarRatio-mpr_minNearFarRatio);
					//		//glbDouble _near = pcustom->_maxFar*pcustom->_nearFarRatio;
					//	}else{	//地上时
					//		glbDouble mpr_minNearFarRatio = 0.0000016;
					//		glbDouble mpr_maxNearFarRatio = 0.000008;

					//		double dis_eyeTocenter = eye_w.length();	//相机球心距离的平方
					//		//-90°看时，地球在眼睛里张角的一半
					//		if (dis_eyeTocenter < WGS_84_RADIUS_POLAR)
					//			dis_eyeTocenter = WGS_84_RADIUS_POLAR;
					//		glbDouble angle = asin(WGS_84_RADIUS_POLAR/dis_eyeTocenter);
					//		glbDouble eye_terrain = eye.z() - terrainElevationOfCameraPos;	//相机地形高度
					//		//设置远裁剪面,设远一点，留些富余
					//		glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - osg::WGS_84_RADIUS_POLAR * osg::WGS_84_RADIUS_POLAR); //相机远裁剪面
					//		double pitch = 	osg::DegreesToRadians(pview->GetPitch()) + osg::PI_2;
					//		if (pitch < angle)
					//			pcustom->_maxFar = eye_tangent*cos(angle - pitch);
					//		else
					//			pcustom->_maxFar = eye_tangent;
					//		if (eye.z() - terrainElevationOfCameraPos < 100.0)
					//		{
					//			pcustom->_maxFar *= osg::clampBetween(eye_terrain/100.0,0.1,1.0);
					//		}
					//		float vr = (osg::clampBetween(eye_terrain, 0.0, eye.z())-0.0)/(eye.z()-0.0);
					//		pcustom->_nearFarRatio = mpr_minNearFarRatio + vr * (mpr_maxNearFarRatio-mpr_minNearFarRatio);
					//	}
					//}
				}
				break;	 
			default:
				break;								    
			}
		}else if (mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
		{
			CGlbGlobeView* pview = mpr_p_globe->GetView();
			osg::Camera* pCamera = pview->GetOsgCamera();
			osg::ref_ptr<osg::CullSettings::ClampProjectionMatrixCallback> clamper = mpr_p_clampers[pCamera];
			if (!clamper.valid())
			{  
				clamper = new CustomProjClamper;
				pCamera->setClampProjectionMatrixCallback(clamper.get());
				mpr_p_clampers[pCamera] = clamper;
			}								 
			CustomProjClamper* pcustom = static_cast<CustomProjClamper*>(clamper.get());
			pcustom->_isFlat = true;
			osg::Vec3d eye; 
			pview->GetCameraPos(eye);	

			glbDouble terrainElevationOfCameraPos = pview->GetTerrainElevationOfCameraPos();
			glbDouble eye_terrain = eye.z() - terrainElevationOfCameraPos;
			if (abs(eye_terrain) > 2.0)
				pcustom->_minDistance = 0.3;
			else
				pcustom->_minDistance = 0.03;
			glbDouble terrainminx,terrainminy,terrainminz,terrainmaxx,terrainmaxy,terrainmaxz;	//地形外包盒
			mpr_p_globe->GetTerrainDataset()->GetExtent()->GetMin(&terrainminx,&terrainminy,&terrainminz);
			mpr_p_globe->GetTerrainDataset()->GetExtent()->GetMax(&terrainmaxx,&terrainmaxy,&terrainmaxz);
			switch(pview->GetManipulatorType())
			{
			case GLB_MANIPULATOR_DRIVE:
			case GLB_MANIPULATOR_ANIMATIONPATH:
			case GLB_MANIPULATOR_FREE:
				{ 
					//glbDouble pitch = osg::DegreesToRadians(pview->GetPitch());	
					//glbDouble angle = (osg::PI_2 + pitch) > osg::PI/6 ? (osg::PI_2 + pitch) : osg::PI/6;
					//glbDouble d = abs(eye.z() - terrainElevationOfCameraPos);
					//pcustom->_minNear = d/cos(osg::PI_2+pitch-osg::PI/6)*cos(angle);
					//glbDouble mpr_minNearFarRatio = 0.0000016;	//最小远近裁剪面比率
					//glbDouble mpr_maxNearFarRatio = 0.000008;		//最大远近裁剪面比率
					// 在高空100米以上和低空100米以下最小做大裁剪面比率应该是两套值才能适应需求
					glbDouble mpr_minNearFarRatio = (eye_terrain>100)? 0.0004 : 0.00016 /*0.00016*/;		//最小远近裁剪面比率					
					glbDouble mpr_maxNearFarRatio = (eye_terrain>100)? 0.004 : 0.0002/*0.0002*/;		//最大远近裁剪面比率
					if (mpr_p_globe->IsUnderGroundMode())	//开启地下模式
					{
						if (eye_terrain < 0.0)	//到地下了
						{						
							//mpr_minNearFarRatio = 0.000016;	//最小远近裁剪面比率
							//mpr_maxNearFarRatio = 0.00002;		//最大远近裁剪面比率
							
							mpr_minNearFarRatio = 0.00016;		//最小远近裁剪面比率
							mpr_maxNearFarRatio = 0.0002;		//最大远近裁剪面比率
							
							glbDouble underGroundAltitude = mpr_p_globe->GetUnderGroundAltitude();
							if (underGroundAltitude < terrainminz)
								terrainminz = underGroundAltitude;
							glbDouble eye_underGround = eye.z() - underGroundAltitude;
							eye_terrain = abs(eye_terrain);
							if (eye_terrain > eye_underGround)
								eye_terrain = eye_underGround;
						}
					}
					//计算近似远裁剪面
					//考虑大地地形最低点terrainMinz与最边缘
					glbDouble eye_dx = max(eye.x() - terrainminx,terrainmaxx - eye.x());
					glbDouble eye_dy = max(eye.y() - terrainminy,terrainmaxy - eye.y());
					glbDouble eye_dz = max(eye.z() - terrainminz,terrainmaxz - eye.z());

					if (GLB_MANIPULATOR_DRIVE == pview->GetManipulatorType())
					{// 第一人称漫游状态时，最远距离需要缩近，最近裁剪面需要近 , 避免近处被裁的问题。2016.9.8 malin
						eye_terrain = eye.z() - terrainElevationOfCameraPos;
						if (eye_terrain < 100.0)
						{
							pcustom->_maxFar = sqrt(eye_dx*eye_dx + eye_dy*eye_dy + eye_dz*eye_dz)*0.5;
							pcustom->_nearFarRatio =  0.000016; //0.000016							
						}
						else
						{
							pcustom->_maxFar = sqrt(eye_dx*eye_dx + eye_dy*eye_dy + eye_dz*eye_dz);
							pcustom->_nearFarRatio =  0.004;
						}					
					}
					else
					{
						//glbDouble pitch = pview->GetPitch() + 90.0;		
						//if (fabs(pitch)<= 30)
						//	pcustom->_maxFar = (eye.z()-terrainminz)/cos(osg::DegreesToRadians(pitch))*3.0;
						//else
						pcustom->_maxFar = sqrt(eye_dx*eye_dx + eye_dy*eye_dy + eye_dz*eye_dz);
						float vr = (osg::clampBetween(eye_terrain, 0.0, eye.z())-0.0)/(eye.z()-0.0);
						pcustom->_nearFarRatio = mpr_minNearFarRatio + vr * (mpr_maxNearFarRatio-mpr_minNearFarRatio); 

						//eye_terrain = eye.z() - terrainElevationOfCameraPos;
						//if (eye_terrain < 100.0)
						//{
						//	pcustom->_isNearGround = true;
						//	pcustom->_nearFarRatio =  0.000032; //0.000016
						//}
						//else
						//	pcustom->_isNearGround = false;
					}
				}
				break;		
			default:
				break;	
			}
		}
	}
	traverse( node, nv );
}