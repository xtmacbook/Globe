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
		// ���mpr_cameraAltitudeNearfarRadio����>0
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
					glbDouble eye_terrain = eye.z() - terrainElevationOfCameraPos;	//������θ߶�
				
#ifdef XinJiangTaLiMuPenDiXiangMu
					pcustom->_bUnderGround = false;
#endif
					if (!mpr_p_globe->IsUnderGroundMode())	//δ��������ģʽ
					{
						glbDouble dis_eyeTocenter = eye_w.length();	//������ľ����ƽ��
						//����Զ�ü���,��Զһ�㣬��Щ����
						glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - osg::WGS_84_RADIUS_POLAR * osg::WGS_84_RADIUS_POLAR); //���Զ�ü���
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
						else// ����2������
						{
							pcustom->_nearFarRatio = 0.000001;
						}
					}
					else
					{
						glbDouble dis_eyeTocenter = eye_w.length();	//������ľ����ƽ��
						//����Զ�ü���,��Զһ�㣬��Щ����
						glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - osg::WGS_84_RADIUS_POLAR * osg::WGS_84_RADIUS_POLAR); //���Զ�ü���
						pcustom->_maxFar = eye_tangent;

						if(mpr_cameraAltitudeNearfarRatio.size()>0)
						{// �û����ⲿ�ļ�����������߶Ⱥ�nearFarRatio�Ķ�Ӧ���� 2015.12.3 ����
							double upperDistance = DBL_MAX;
							double lowerDistance = DBL_MIN;
							double nearFarRatio = 0.01;
							std::map<double,double>::reverse_iterator itr = mpr_cameraAltitudeNearfarRatio.rbegin();
							while(itr != mpr_cameraAltitudeNearfarRatio.rend())
							{
								double eye2terrain = itr->first;								
								if (eye_terrain > eye2terrain)
								{// ���϶��� ���ҵ�һ���ȵ�ǰ�����ظ߶�С��itr
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
						else// ���û������ļ���ʽ
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
					//if (!mpr_p_globe->IsUnderGroundMode())	//δ��������ģʽ
					//{
					//	pcustom->_minDistance = 0.3;
					//	glbDouble mpr_minNearFarRatio = 0.0000016;	//��СԶ���ü������
					//	glbDouble mpr_maxNearFarRatio = 0.000008;	//���Զ���ü������

					//	glbDouble dis_eyeTocenter = eye_w.length();	//������ľ����ƽ��
					//	//-90�㿴ʱ���������۾����Žǵ�һ��
					//	if (dis_eyeTocenter < WGS_84_RADIUS_POLAR)
					//		return;
					//	glbDouble angle = asin(WGS_84_RADIUS_POLAR/dis_eyeTocenter);
					//	//����ӽ�
					//	glbDouble angle_cam,asp,znear,zfar;
					//	pCamera->getProjectionMatrixAsPerspective(angle_cam,asp,znear,zfar);


					//	angle_cam = osg::DegreesToRadians(angle_cam);
					//	glbDouble eye_terrain = eye.z() - terrainElevationOfCameraPos;	//������θ߶�
					//	//����Զ�ü���,��Զһ�㣬��Щ����
					//	glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - osg::WGS_84_RADIUS_POLAR * osg::WGS_84_RADIUS_POLAR); //���Զ�ü���
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
					//	////���ǲ�ͬ���
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
					//}else{		//��������ģʽ
					//	if (eye.z() - terrainElevationOfCameraPos < 0.0)	//����ʱ
					//	{
					//		glbDouble mpr_minNearFarRatio = 0.00000002;
					//		glbDouble mpr_maxNearFarRatio = 0.0000001;
					//		double dis_eyeTocenter = eye_w.length();	//������ľ����ƽ��
					//		//-90�㿴ʱ������ĵ��²ο������۾����Žǵ�һ��
					//		glbDouble underGroundAltitude = mpr_p_globe->GetUnderGroundAltitude();
					//		glbDouble radius = WGS_84_RADIUS_POLAR + underGroundAltitude;
					//		if (radius > dis_eyeTocenter)
					//			radius = dis_eyeTocenter;
					//		glbDouble angle = asin(radius/dis_eyeTocenter);
					//		glbDouble eye_terrain = eye.z() - underGroundAltitude;	//������²ο���߶�
					//		//����Զ�ü���,��Զһ�㣬��Щ����
					//		glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - radius*radius); //���Զ�ü���
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

					//		double dis_eyeTocenter = eye_w.length();	//������ľ����ƽ��
					//		//-90�㿴ʱ���������۾����Žǵ�һ��
					//		if (dis_eyeTocenter < WGS_84_RADIUS_POLAR)
					//			dis_eyeTocenter = WGS_84_RADIUS_POLAR;
					//		glbDouble angle = asin(WGS_84_RADIUS_POLAR/dis_eyeTocenter);
					//		angle = angle > osg::PI/6 ? osg::PI/6 : angle;
					//		glbDouble eye_terrain = eye.z() - terrainElevationOfCameraPos;	//������θ߶�
					//		//����Զ�ü���,��Զһ�㣬��Щ����
					//		glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - osg::WGS_84_RADIUS_POLAR * osg::WGS_84_RADIUS_POLAR); //���Զ�ü���
					//		double pitch = 	osg::DegreesToRadians(pview->GetPitch()) + osg::PI_2;
					//		if (pitch < angle)
					//			pcustom->_maxFar = eye_tangent*cos(angle - pitch);
					//		else
					//			pcustom->_maxFar = eye_tangent;
					//		//pcustom->_maxFar *= 0.1;
					//		float vr = (osg::clampBetween(eye_terrain, 0.0, eye.z())-0.0)/(eye.z()-0.0);
					//		pcustom->_nearFarRatio = mpr_minNearFarRatio + vr * (mpr_maxNearFarRatio-mpr_minNearFarRatio);
					//		//glbDouble _near = pcustom->_maxFar*pcustom->_nearFarRatio;
					//	}else{	//����ʱ
					//		glbDouble mpr_minNearFarRatio = 0.0000016;
					//		glbDouble mpr_maxNearFarRatio = 0.000008;

					//		double dis_eyeTocenter = eye_w.length();	//������ľ����ƽ��
					//		//-90�㿴ʱ���������۾����Žǵ�һ��
					//		if (dis_eyeTocenter < WGS_84_RADIUS_POLAR)
					//			dis_eyeTocenter = WGS_84_RADIUS_POLAR;
					//		glbDouble angle = asin(WGS_84_RADIUS_POLAR/dis_eyeTocenter);
					//		glbDouble eye_terrain = eye.z() - terrainElevationOfCameraPos;	//������θ߶�
					//		//����Զ�ü���,��Զһ�㣬��Щ����
					//		glbDouble eye_tangent = sqrt( dis_eyeTocenter*dis_eyeTocenter - osg::WGS_84_RADIUS_POLAR * osg::WGS_84_RADIUS_POLAR); //���Զ�ü���
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
			glbDouble terrainminx,terrainminy,terrainminz,terrainmaxx,terrainmaxy,terrainmaxz;	//���������
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
					//glbDouble mpr_minNearFarRatio = 0.0000016;	//��СԶ���ü������
					//glbDouble mpr_maxNearFarRatio = 0.000008;		//���Զ���ü������
					// �ڸ߿�100�����Ϻ͵Ϳ�100��������С����ü������Ӧ��������ֵ������Ӧ����
					glbDouble mpr_minNearFarRatio = (eye_terrain>100)? 0.0004 : 0.00016 /*0.00016*/;		//��СԶ���ü������					
					glbDouble mpr_maxNearFarRatio = (eye_terrain>100)? 0.004 : 0.0002/*0.0002*/;		//���Զ���ü������
					if (mpr_p_globe->IsUnderGroundMode())	//��������ģʽ
					{
						if (eye_terrain < 0.0)	//��������
						{						
							//mpr_minNearFarRatio = 0.000016;	//��СԶ���ü������
							//mpr_maxNearFarRatio = 0.00002;		//���Զ���ü������
							
							mpr_minNearFarRatio = 0.00016;		//��СԶ���ü������
							mpr_maxNearFarRatio = 0.0002;		//���Զ���ü������
							
							glbDouble underGroundAltitude = mpr_p_globe->GetUnderGroundAltitude();
							if (underGroundAltitude < terrainminz)
								terrainminz = underGroundAltitude;
							glbDouble eye_underGround = eye.z() - underGroundAltitude;
							eye_terrain = abs(eye_terrain);
							if (eye_terrain > eye_underGround)
								eye_terrain = eye_underGround;
						}
					}
					//�������Զ�ü���
					//���Ǵ�ص�����͵�terrainMinz�����Ե
					glbDouble eye_dx = max(eye.x() - terrainminx,terrainmaxx - eye.x());
					glbDouble eye_dy = max(eye.y() - terrainminy,terrainmaxy - eye.y());
					glbDouble eye_dz = max(eye.z() - terrainminz,terrainmaxz - eye.z());

					if (GLB_MANIPULATOR_DRIVE == pview->GetManipulatorType())
					{// ��һ�˳�����״̬ʱ����Զ������Ҫ����������ü�����Ҫ�� , ����������õ����⡣2016.9.8 malin
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