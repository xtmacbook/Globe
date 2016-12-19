#include "StdAfx.h"
#include "GlbPlaneManipulator.h"
#include "GlbGlobeView.h"
#include "GlbLog.h"
using namespace GlbGlobe;

CGlbPlaneManipulator::CGlbPlaneManipulator(CGlbGlobe *globe)
{
	mpr_globe = globe;
	if (globe)
	{ 
		globe->GetTerrainDataset()->GetExtent()->Get(&mpr_minx,&mpr_maxx,&mpr_miny,&mpr_maxy,&mpr_minz,&mpr_maxz);
		//mpr_minx = globe->GetTerrainDataset()->GetExtent()->GetLeft();		
		//mpr_miny = globe->GetTerrainDataset()->GetExtent()->GetBottom();
		//mpr_maxx = globe->GetTerrainDataset()->GetExtent()->GetRight();
		//mpr_maxy = globe->GetTerrainDataset()->GetExtent()->GetTop();		
		double len = max(mpr_maxx-mpr_minx,mpr_maxy-mpr_miny);
		mpr_camera_minx = mpr_minx - len*1.5;//0.2
		mpr_camera_miny = mpr_miny - len*1.5;
		mpr_camera_maxx = mpr_maxx + len*1.5;		
		mpr_camera_maxy = mpr_maxy + len*1.5;
		globe->GetTerrainDataset()->GetExtent()->GetCenter(&mpr_focusPos.x(),&mpr_focusPos.y(),&mpr_focusPos.z()); 
		mpr_distance = max(mpr_maxx-mpr_minx,mpr_maxy-mpr_miny)*1.732;
		mpr_maxCameraAlt = max(mpr_maxx-mpr_minx,mpr_maxy-mpr_miny)*4.0;
		mpr_angel = osg::RadiansToDegrees(atan2(mpr_maxy - mpr_miny,mpr_maxx-mpr_minx));
	}	
	mpr_speed = osg::Vec2d(0.0,0.0);
	mpr_yaw = 0.0;
	mpr_pitch = -90.0;//��ֱ���¿�,ʵ��ʹ��PI/2+mpr_pitch��Ϊ��x����ת�ǣ��൱�������ԭ��λ�ÿ���z�Ḻ����
	mpr_lockmode = GLB_LOCKMODE_FREE;

	mpr_isShake = false;
	mpr_shakeTime = -1;
	mpr_shakeStartTime = -1;
	mpr_shakeHorDegree = 2;
	mpr_shakeVerDegree = 2;

	mpr_bUseReferencePlane = false; //Ĭ�Ϲر�����ο���
	mpr_referencePlaneZ = -560.0; // ����ƽ�� 1050 //600.0; �żҿ� //���� -560
}

CGlbPlaneManipulator::~CGlbPlaneManipulator(void)
{
}								 

void CGlbPlaneManipulator::CGlbPlaneFlyCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR && nv->getFrameStamp())
	{
		if (_manipulator)	//flyaround
		{
			if (!_pause)
			{
				glbDouble frametime = nv->getFrameStamp()->getSimulationTime();
				glbDouble delt_time = frametime - _latestTime;
				_latestTime = frametime;
				if (_firstTime==DBL_MAX)
				{
					_firstTime = _latestTime;
					delt_time  = 0.0;
				}
				//glbDouble yaw = 360.0*delt_time/_loopTime + _manipulator->GetYaw();
				osg::Vec3d focusPos;
				_manipulator->GetFocusPos(focusPos);
				//glbDouble distan = _manipulator->GetDistance();
				//_manipulator->SetYaw(yaw,focusPos,distan);
				//_matrix = _manipulator->getMatrix();
				glbDouble delt_yaw = 360.0*(delt_time)/_loopTime;
				_manipulator->Rotate(focusPos.x(),focusPos.y(),focusPos.z(),0.0,delt_yaw);					
				_matrix = _manipulator->getMatrix();
			}
		}else{				//flyto
			if (!_pause)
			{
				_latestTime = nv->getFrameStamp()->getSimulationTime();
				if (_firstTime==DBL_MAX)
					_firstTime = _latestTime;
				glbDouble tim = _latestTime - _firstTime;
				glbDouble ratio = tim/_loopTime;
				if (ratio > 1.0)
					ratio = 1.0;

				//�����ֵ
				osg::Vec3d focusPos = osg::Vec3d(_firstPoint.focusPos.x()+ratio*(_lastPoint.focusPos.x()-_firstPoint.focusPos.x()),
												_firstPoint.focusPos.y()+ratio*(_lastPoint.focusPos.y()-_firstPoint.focusPos.y()),
												_firstPoint.focusPos.z()+ratio*(_lastPoint.focusPos.z()-_firstPoint.focusPos.z()));

				//�����ֵ
				glbDouble distan = _firstPoint.distan + (_lastPoint.distan-_firstPoint.distan)*ratio;
				if (_lastPoint.distan + _firstPoint.distan < 20000 && _isfar)	//�����ؼ���߶ȺͱȽ�Сʱ
				{					
					glbDouble mid_distan = 2*(_lastPoint.distan + _firstPoint.distan);
					while (mid_distan < 20000)
						mid_distan *= 2;
					if (ratio < 0.5)
						distan = _firstPoint.distan + (mid_distan-_firstPoint.distan)*ratio*2;
					else
						distan = _lastPoint.distan + (mid_distan-_lastPoint.distan)*(1-ratio)*2;
				}else if (_lastPoint.distan/_firstPoint.distan > 10.0)  //�����ؼ���߶����ܴ�ʱ
				{
					distan = _lastPoint.distan - (_lastPoint.distan-_firstPoint.distan)*sqrt(1-ratio*ratio);
				}else if (_lastPoint.distan/_firstPoint.distan < 0.1)	 //�����ؼ���߶����ܴ�ʱ
				{
					distan = _firstPoint.distan + (_lastPoint.distan-_firstPoint.distan)*sqrt(1-(1.0-ratio)*(1.0-ratio));
				}
				//��֤delt_yaw��(-180,180)֮��
				glbDouble delt_yaw = _lastPoint.yaw-_firstPoint.yaw;				
				if(delt_yaw > 180)
					delt_yaw -= 360;
				if(delt_yaw < -180)
					delt_yaw += 360;
				glbDouble yaw = _firstPoint.yaw + delt_yaw*ratio;	//ƫ��						
				glbDouble pitch = _firstPoint.pitch + (_lastPoint.pitch-_firstPoint.pitch)*ratio;//����
								
				if (tim > _loopTime*1.05)
				{
					_matrix = osg::Matrixd::translate(0.0,0.0,_lastPoint.distan)*
						osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(_lastPoint.pitch),1.0,0.0,0.0)*
						osg::Matrixd::rotate(osg::DegreesToRadians(-_lastPoint.yaw),0.0,0.0,1.0)*
						osg::Matrixd::translate(_lastPoint.focusPos);

					setPause(true);
				}
				else
				{
					_matrix = osg::Matrixd::translate(0.0,0.0,distan)*
						osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
						osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
						osg::Matrixd::translate(focusPos);
				}
			}
		}
		traverse(node,nv);
	}
}

void CGlbPlaneManipulator::CGlbPlaneFlyCallback::setPause(glbBool pause)
{
	if (_pause == pause)
		return;	
	_pause = pause;
	_manipulator = NULL;
	if (_firstTime==DBL_MAX)
		return;
	if (_pause)
		_pauseTime = _latestTime;
	else
		_firstTime += (_latestTime-_pauseTime);
}

osg::Matrixd CGlbPlaneManipulator::getMatrix() const
{
	return osg::Matrixd::translate(0.0,0.0,mpr_distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(mpr_pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-mpr_yaw),0.0,0.0,1.0)*
		osg::Matrixd::translate(mpr_focusPos);
}

void CGlbPlaneManipulator::setByMatrix(const osg::Matrixd& matrix)
{ 
	//���λ��
	osg::Vec3d cameraPos = matrix.getTrans();
	if (cameraPos.isNaN()==true)
		return;

	//����λ��
	if (abs(mpr_distance) < 0.01)
		mpr_distance = 0.01;

	if (_isnan(mpr_distance))
		mpr_distance = 10000.0;

	osg::Vec3d oldfocusPos = osg::Vec3d(0.0,0.0,-mpr_distance)*matrix;
	osg::Vec3d lookvec = oldfocusPos - cameraPos;
	osg::Vec3d focusPos;
	glbBool isIntersect = mpr_globe->IsRayInterTerrain(cameraPos,lookvec,focusPos);
	if (mpr_globe->IsUnderGroundMode())	//��������ģʽ
	{
		//����ڵ���ʱ
		if (cameraPos.z() < mpr_globe->GetView()->GetTerrainElevationOfCameraPos())
		{
			//���������û�н���
			if (!isIntersect)
			{ 
				//����������޽���,������²ο�����
				isIntersect = mpr_globe->IsRayInterUGPlane(cameraPos,lookvec,focusPos);
			}
		}
	}
	if (isIntersect)	//����λ���²ο����н���
	{
		mpr_focusPos = focusPos;
		mpr_oldfocusPos = mpr_focusPos;
	}
	else				//����λ���²ο��涼�޽��㣬ά��һ���齹��
		mpr_focusPos = oldfocusPos;
	lookvec = mpr_focusPos - cameraPos;
	mpr_distance = lookvec.length();
	//if (lookvec.x() != 0.0 || lookvec.y() != 0.0)
		//mpr_yaw = osg::RadiansToDegrees(atan2(lookvec.x(),lookvec.y())); 
	//else{
	mpr_yaw = osg::RadiansToDegrees(atan2(-matrix(0,1),matrix(0,0)));		  
	//}
	if (lookvec.z()/mpr_distance > 1.0)
		mpr_pitch = 90.0;
	else if (lookvec.z()/mpr_distance < -1.0)
		mpr_pitch = -90.0;
	else
	{
		mpr_pitch = osg::RadiansToDegrees(asin(lookvec.z()/mpr_distance));
		//mpr_pitch = osg::RadiansToDegrees(atan(matrix(2,1)/matrix(2,2)));
		//double pitch2 = osg::RadiansToDegrees(asin(lookvec.z()/mpr_distance));
		//if (fabs(mpr_pitch - pitch2) > 0.0001)
		//{
		//	int differ = 1;
		//}
	}
}

osg::Matrixd CGlbPlaneManipulator::getInverseMatrix() const
{
	return osg::Matrixd::inverse(getMatrix());
}

void CGlbPlaneManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
{
	setByMatrix(osg::Matrix::inverse(matrix));
}

glbBool CGlbPlaneManipulator::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{  
	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::FRAME:
		{
			if (/*IsFlying() && */_FlyCallback.valid())
			{ 
				if (_FlyCallback->getAnimationTime() > 0.0)		//��ֹ��һ֡��֡
				{ 
					osg::Matrixd mat = _FlyCallback->getMatrix();
					setByMatrix(mat);
					//osg::Matrixd invertMat = osg::Matrixd::inverse(mat);
					//setByInverseMatrix(invertMat);

					//�����ƫ
					osg::Vec3d cameraPos;
					GetCameraPos(cameraPos);
					glbDouble elevate = 0.0;
					if (mpr_globe->GetView()->getUpdateCameraMode())
						elevate = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
					else
						elevate = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
					glbBool isUpdate = UpdateCameraElevate(elevate);
					UpdateCameraElevateExt2();
					GetCameraPos(cameraPos);
					if (cameraPos.z() > mpr_globe->GetView()->GetTerrainElevationOfCameraPos())
						mpr_globe->NotifyCameraIsUnderground(false);
					else
						mpr_globe->NotifyCameraIsUnderground(true);						

					if (_FlyCallback->getPause())
					{// ��ֹͣʱ����_FlyCallback�ָ�ΪNULL
						osg::Camera* pcamera = mpr_globe->GetView()->GetOsgCamera();
						if (pcamera)
						{
							pcamera->setUpdateCallback(NULL);
							_FlyCallback = NULL;
						}
					}

					//aa.requestRedraw();
					return true; 
				}				
			}

			if (mpr_isShake)
			{// �����		
				if (mpr_shakeStartTime < 0)// ��¼��ʼʱ�� get time in seconds of event
				{
					mpr_shakeStartTime = ea.getTime();
					//mpr_shakeStartTime = us.asView()->getFrameStamp()->getSimulationTime();
				}
				// �ж�ʱ���Ƿ񳬳�shaketime
				double tim =  ea.getTime();//us.asView()->getFrameStamp()->getSimulationTime();
				if (tim - mpr_shakeStartTime > mpr_shakeTime && mpr_shakeTime > 0)
				{// ֹͣ
					Shake(false);
				}
				else
				{
					static int frameNum = 0;				
					if (frameNum % 2 == 1)
					{// ��ʱ��λ
						mpr_pitch = mpr_orignPitch;
						mpr_yaw = mpr_orignyaw;
						mpr_focusPos = mpr_shakefocusPos; 
						mpr_distance = mpr_shakeDistance;
					}
					else
					{// �ζ�
						// rand() / double(RAND_MAX)����ȡ��0��1֮��ĸ�����
						// (0 , 2)��֮�䶶��
						float shakeVerticalOffset =  (rand() / double(RAND_MAX)) * mpr_shakeVerDegree;				
						mpr_pitch = mpr_orignPitch + shakeVerticalOffset;
						// (-1 , 1)��֮�䶶��
						float shakeHorizonOffset = ( rand() / double(RAND_MAX) - 0.5) * mpr_shakeHorDegree;
						mpr_yaw = mpr_orignyaw + shakeHorizonOffset;
						// focuse pos ���¶��� [0 - 1]
						double maxDis = mpr_shakeDistance * tan(osg::DegreesToRadians(1.0));
						float z = mpr_shakefocusPos.z() + maxDis * ( rand() / double(RAND_MAX)) * 1.0;
						mpr_focusPos.set(mpr_shakefocusPos.x(),mpr_shakefocusPos.y(),z);
					}
					frameNum++;
					if (frameNum==10000)
						frameNum = 0;			
				}	
			}			

			//�����ƫ
			glbDouble cameraElevate = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			glbBool isUpdate = UpdateCameraElevate(cameraElevate);
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			if (cameraPos.z() > cameraElevate)
				mpr_globe->NotifyCameraIsUnderground(false);
			else
				mpr_globe->NotifyCameraIsUnderground(true);	
			//����fly
			//if (mpr_speed.length() > ZERO_E7)
			//{
			//	Move(mpr_speed.x(),mpr_speed.y());
			//	aa.requestRedraw();
			//	return true;
			//}

			//if (isUpdate)
			//	aa.requestRedraw();
			return false;
		}
	case osgGA::GUIEventAdapter::RESIZE:
		{
			return false;
		}
	default:
		break;
	}

	if (ea.getHandled())
		return false;

	switch(ea.getEventType())
	{
	case osgGA::GUIEventAdapter::PUSH:
		{
			_ga_t0 = &ea;
			_ga_t1 = &ea;
			if (_ga_t0->getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON || _ga_t0->getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)	//���ֻ����������
			{ 	
				if (IsFlying())
					StopFlying();

				// ��¼��갴��ʱ����������VPW����,�Ա�ʹ��
				_pushMatrix = getMatrix();
				osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
				_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();

				//��¼ѡ�е㾭γ��
				glbDouble dx = _ga_t1->getX();
				glbDouble dy = _ga_t1->getY();
				//��������ϵ(���λ��) ת��Ϊ ��Ļ����
				glbBool ischoose = mpr_globe->ScreenToTerrainCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());
				if (!ischoose)
				{// �����û�н���������²ο�����
					if (mpr_globe->IsUnderGroundMode())	  //��������ģʽ 	
					{
						ischoose = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());		
						if (!ischoose)
						{// �����²ο����޽��㣿 ֱ�Ӹ����²ο���������չ����
							osg::Vec3d startPos(dx,dy,0.0);
							osg::Vec3d endPos(dx,dy,1.0);
							osg::Matrixd inverseVPW;
							inverseVPW.invert(_pushVPW);
							startPos = startPos * inverseVPW;
							endPos = endPos * inverseVPW;
							osg::Vec3d nomalvec(0.0,0.0,1.0);

							osg::Vec3d centerPos((mpr_minx+mpr_maxx)*0.5,(mpr_miny+mpr_maxy)*0.5,mpr_minz);
							glbDouble alt = mpr_globe->GetUnderGroundAltitude();
							centerPos.set(centerPos.x(),centerPos.y(),alt);
							std::vector<osg::Vec3d> IntersectPoses;
							glbInt32 IntersectResult2 = IntersectRayPlane(startPos,endPos,centerPos,nomalvec,IntersectPoses);
							if (IntersectResult2 > 0)
							{
								ischoose = true;
								_pushPos = IntersectPoses[0];
							}
						}
					}
				}
				glbDouble ptx1_s = dx,pty1_s = dy;
				mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);			
				osg::Vec3d objInterPoint;
				glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx1_s,pty1_s,objInterPoint);			

				if (!ischoose && !isObjfocus)
				{// ��û�н��㣬��ֱ��� ������͵����ϵ�����ο�ƽ�� �� ���²ο��棨������չ���󽻣�ȡ���������췽�����Ҿ����������ĵ���Ϊ�ο�����
					//osg::Vec3d cameraPos;
					//GetCameraPos(cameraPos);
					//bool isCameraUnderGround = (cameraPos.z() >= mpr_globe->GetView()->GetTerrainElevationOfCameraPos()) ? false : true;					
					GlbLogWOutput(GLB_LOGTYPE_INFO,L"Warning: NOT intersect ground and objects!");

					osg::Vec3d startPos(dx,dy,0.0);
					osg::Vec3d endPos(dx,dy,1.0);
					osg::Matrixd inverseVPW;
					inverseVPW.invert(_pushVPW);
					startPos = startPos * inverseVPW;
					endPos = endPos * inverseVPW;
					osg::Vec3d nomalvec(0.0,0.0,1.0);

					osg::Vec3d dir = endPos - startPos;
					dir.normalize();
					// ���û������ο������������͵�����ο�ƽ����
					osg::Vec3d centerPos((mpr_minx+mpr_maxx)*0.5,(mpr_miny+mpr_maxy)*0.5,mpr_minz);
					if (mpr_bUseReferencePlane)
					{// ʹ������ο���.
						centerPos.set((mpr_minx+mpr_maxx)*0.5,(mpr_miny+mpr_maxy)*0.5,mpr_referencePlaneZ);
					}
					
					std::vector<osg::Vec3d> IntersectPoses;
					glbInt32 IntersectResult = IntersectRayPlane(startPos,endPos,centerPos,nomalvec,IntersectPoses);
					osg::Vec3d interTerrain;
					osg::Vec3d interUnPlane;

					bool isInterTerrainAtFront = true;
					bool isInterUnPlaneAtFront = true;
					if (IntersectResult > 0)
					{
						interTerrain = IntersectPoses[0];
						osg::Vec3d tt = interTerrain - startPos;
						if (tt.x()*dir.x() < 0 || tt.y()*dir.y() < 0 || tt.z()*dir.z() < 0)
							isInterTerrainAtFront = false;
					}
					
					if (mpr_globe->IsUnderGroundMode())
					{// ���������²ο���ʱ������²ο�������ƽ����
						glbDouble alt = mpr_globe->GetUnderGroundAltitude();
						centerPos.set(centerPos.x(),centerPos.y(),alt);
						glbInt32 IntersectResult2 = IntersectRayPlane(startPos,endPos,centerPos,nomalvec,IntersectPoses);
						if (IntersectResult2 > 0)
						{
							interUnPlane = IntersectPoses[0];
							osg::Vec3d tt = interUnPlane - startPos;
							if (tt.x()*dir.x() < 0 || tt.y()*dir.y() < 0 || tt.z()*dir.z() < 0)
								isInterUnPlaneAtFront = false;

							if (isInterTerrainAtFront && isInterUnPlaneAtFront)
							{
								double dis1 = (interTerrain - startPos).length();
								double dis2 = (interUnPlane - startPos).length();								
								if (dis1 < dis2)
									_pushPos = interTerrain;
								else
									_pushPos = interUnPlane;
							}
							else
							{
								if (isInterTerrainAtFront)
									_pushPos = interTerrain;
								if (isInterUnPlaneAtFront)
									_pushPos = interUnPlane;
							}
						}
					}
					else
					{// û�п�������ģʽֱ��
						osg::Vec3d tt = interTerrain - startPos;
						if (isInterTerrainAtFront = false)
						{// ���������߷����෴����ʾ�����ڱ�������Ҫ�������Ƶ�ǰ��
							_pushPos = startPos + dir * tt.length();
						}
						else
							_pushPos = interTerrain;
					}
					
				}
				else if (isObjfocus && ischoose)
				{// �Ƚ϶����ϵ�pick��͵���/���²ο���/���òο����ϵ�pick���ĸ������������ѡ���ĸ�����pushPos
					osg::Vec3d cameraPos_w = getMatrix().getTrans();
					glbDouble dis1 = (cameraPos_w - objInterPoint).length();
					glbDouble dis2 = (cameraPos_w - _pushPos).length();
					if (dis1 < dis2)
					{// �����ϵ�pick���������
						_pushPos = objInterPoint;
					}
				}
				else if (isObjfocus && !ischoose)
				{
					_pushPos = objInterPoint;
				}
				mpr_bFirstDrag = true;				
				
				//����ѡ�е�ͼ��			
				//aa.requestRedraw();
				return true;
			}
			return false;
		}
	case osgGA::GUIEventAdapter::RELEASE:
		{
			_ga_t0 = &ea;
			_ga_t1 = NULL;
			return false;
		}
	case osgGA::GUIEventAdapter::DOUBLECLICK:
		{
			_ga_t0 = &ea;
			if (_ga_t0->getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)	//���˫��
			{ 
				if (IsFlying())
					StopFlying();
				glbDouble dx = _ga_t0->getX();
				glbDouble dy = _ga_t0->getY();
				//��������ϵ(���λ��) ת��Ϊ ��Ļ����
				//mpr_globe->GetView()->WindowToScreen(dx,dy);
				glbDouble tx,ty,tz;
				osg::Vec3 tInter;

				glbBool isclick = mpr_globe->ScreenToTerrainCoordinate(dx,dy,tx,ty,tz);
				if (!isclick)
				{
					if (mpr_globe->IsUnderGroundMode())	//��������ģʽ					
						isclick = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,tx,ty,tz);

				}
				if (isclick)
					tInter.set(tx,ty,tz);
				
				glbDouble seconds = 1.0;
				osg::Vec3d objInter;
				glbDouble ptx1_s = dx,pty1_s = dy;
				mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);				
				glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx1_s,pty1_s,objInter);
				
				// ��û�н���ֱ�ӷ���
				if (!isclick && !isObjfocus) 
					return false;						

				osg::Vec3d InterPoint;
				osg::Vec3d oldcameraPos_w = getMatrix().getTrans();
				if (isclick && isObjfocus)
				{// ���н��㣬ȡ����һ��
					glbDouble dis1 = (oldcameraPos_w - tInter).length();
					glbDouble dis2 = (oldcameraPos_w - objInter).length();
					if (dis1 < dis2)						
						InterPoint = tInter;					
					else
						InterPoint = objInter;		
				}
				else if (isclick)				
					InterPoint = tInter;				
				else if (isObjfocus)
					InterPoint = objInter;
				
				glbDouble distan = (oldcameraPos_w - InterPoint).length();

				if (mpr_lockmode == GLB_LOCKMODE_2D || mpr_lockmode == GLB_LOCKMODEL_2DN)
				{
					FlyTo2(InterPoint.x(),InterPoint.y(),InterPoint.z(),0.33*distan,mpr_yaw,mpr_pitch,seconds);
					//aa.requestRedraw();
					return true;
				}
				//����Ŀ��pitch��
				osg::Vec3d cameraPos = getMatrix().getTrans();
				osg::Vec3d lookvec = InterPoint - cameraPos;
				glbDouble pitch;
				if (lookvec.z()/lookvec.length() > 1.0)
					pitch = 90.0;
				else if (lookvec.z()/lookvec.length() < -1.0)
					pitch = -90.0;
				else
					pitch= osg::RadiansToDegrees(asin(lookvec.z()/lookvec.length())); 
				FlyTo2(InterPoint.x(),InterPoint.y(),InterPoint.z(),0.33*distan,mpr_yaw,pitch,seconds);
				//aa.requestRedraw();
				return true;

			}else if (_ga_t0->getButtonMask() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) //�Ҽ�˫��
			{
				if (IsFlying())
					StopFlying();
				if (mpr_lockmode == GLB_LOCKMODE_2D || mpr_lockmode == GLB_LOCKMODEL_2DN)
					return false;
				glbDouble dx = _ga_t0->getX();
				glbDouble dy = _ga_t0->getY();
				//��������ϵ(���λ��) ת��Ϊ ��Ļ����
				//mpr_globe->GetView()->WindowToScreen(dx,dy);
				glbDouble x,y,z;
				glbBool isclick = mpr_globe->ScreenToTerrainCoordinate(dx,dy,x,y,z);
				glbDouble seconds = 10.0;
				osg::Vec3d pt11_w;
				glbDouble ptx1_s = dx,pty1_s = dy;
				mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
				osg::Vec3d InterPoint;
				glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx1_s,pty1_s,InterPoint);
				if (isObjfocus)
					pt11_w = InterPoint;
				if (!isclick)
				{  
					if (mpr_globe->IsUnderGroundMode())	//��������ģʽ
					{
						glbBool isugclick = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,x,y,z);
						if (!isugclick)
							return false;
					}else if (!isObjfocus)
						return false;
				}
				if (isObjfocus)
				{
					osg::Vec3d oldcameraPos_w = getMatrix().getTrans();
					osg::Vec3d pt1_w = _pushPos;
					glbDouble dis1 = (oldcameraPos_w - pt1_w).length();
					glbDouble dis2 = (oldcameraPos_w - pt11_w).length();
					if (dis2 < dis1)
					{
						_pushPos = pt11_w;
					}					
				}
				FlyAround(x,y,z,seconds,true);
				//aa.requestRedraw();
				return true;
			}
			return false;
		}
	case osgGA::GUIEventAdapter::DRAG:
		{
			if (IsFlying())
				StopFlying();
			if (_ga_t1 == NULL)
				return false;
			if (_ga_t1->getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)		//���ְ�����ק
			{
				//��ȡ��ק������
				glbDouble dx = ea.getX() - _ga_t0->getX();
				glbDouble dy = ea.getY() - _ga_t0->getY();

				glbDouble yaw = -dx*0.3;	//ϵ���ݶ�0.3
				glbDouble pitch = dy*0.3;

				_ga_t0 = &ea; 
				Rotate(_pushPos.x(),_pushPos.y(),_pushPos.z(),pitch,yaw);	
				//aa.requestRedraw();
				return true;
			}else if (_ga_t1->getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)	//���������ק
			{
				//��ȡ������º���һ�����λ�õ�
				glbDouble dx0 = _ga_t1->getX();
				glbDouble dy0 = _ga_t1->getY();
				//��������ϵ(���λ��) ת��Ϊ ��Ļ����
				//mpr_globe->GetView()->WindowToScreen(dx0,dy0);

				//��ȡ������º󣬵�ǰ���λ�õ�
				_ga_t0 = &ea;
				glbDouble dx = _ga_t0->getX();
				glbDouble dy = _ga_t0->getY();
				//��������ϵ(���λ��) ת��Ϊ ��Ļ����
				//mpr_globe->GetView()->WindowToScreen(dx,dy);

				Drag(dx0,dy0,dx,dy);
				//aa.requestRedraw();
				return true;
			}
			return false;			  
		}
	case osgGA::GUIEventAdapter::MOVE:
		{
			_ga_t0 = &ea;
			return false;
		}
	case osgGA::GUIEventAdapter::KEYDOWN:
		{	 
			if (handleKeyDown(ea,aa))
			{
				aa.requestRedraw();
				return true;
			}
			return false;
		}
	case osgGA::GUIEventAdapter::SCROLL:
		{
			if (IsFlying())
				StopFlying();
			if (ea.getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)	//���ְ���
				return false;
			_ga_t0 = &ea;
			glbFloat zoomDelta = _ga_t0->getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP ? 1.0 : -1.0;
			Zoom(zoomDelta);
			//ZoomCursor(zoomDelta,_ga_t0->getX(),_ga_t0->getY());
			//aa.requestRedraw();
			return true;
		}
	default:
		break;
	}  
	return false;
}

glbBool CGlbPlaneManipulator::handleKeyDown(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	switch(ea.getKey())
	{
	case osgGA::GUIEventAdapter::KEY_Space:
		{
			if(IsFlying())
				StopFlying();
			_ga_t0 = &ea;
  			home(ea,aa);
			aa.requestRedraw();
			return true;
		}
	case osgGA::GUIEventAdapter::KEY_Home:
		{
			if (IsFlying())
				StopFlying(); 
			if (mpr_lockmode == GLB_LOCKMODEL_2DN)
				return false;
			_ga_t0 = &ea;
			SetCameraYaw(0.0);
			aa.requestRedraw();
			return true;
		}
	case osgGA::GUIEventAdapter::KEY_End:
		{
			if (IsFlying())
				StopFlying(); 
			if (mpr_lockmode == GLB_LOCKMODEL_2DN)
				return false;
			_ga_t0 = &ea;
			SetCameraYaw(180.0);
			aa.requestRedraw();
			return true;
		}
	case osgGA::GUIEventAdapter::KEY_Page_Up:
		{
			if (IsFlying())
				StopFlying(); 
			if (mpr_lockmode == GLB_LOCKMODEL_2DN || mpr_lockmode == GLB_LOCKMODE_2D) 
				return false;
			_ga_t0 = &ea;
			SetCameraPitch(0.0);
			aa.requestRedraw();
			return true;
		}
	case osgGA::GUIEventAdapter::KEY_Page_Down:
		{
			if (IsFlying())
				StopFlying(); 
			if (mpr_lockmode == GLB_LOCKMODEL_2DN || mpr_lockmode == GLB_LOCKMODE_2D)
				return false;
			_ga_t0 = &ea;
			SetCameraPitch(-90.0);
			aa.requestRedraw();
			return true;
		}
	case osgGA::GUIEventAdapter::KEY_Left:
		{
			if (IsFlying())
				StopFlying();
			if (mpr_lockmode == GLB_LOCKMODEL_2DN)
				return false;
			_ga_t0 = &ea;
			RotateCameraYaw(-1.0);
			aa.requestRedraw();
			return true;
		}
	case osgGA::GUIEventAdapter::KEY_Right:
		{
			if (IsFlying())
				StopFlying();
			if (mpr_lockmode == GLB_LOCKMODEL_2DN)
				return false;
			_ga_t0 = &ea;
			RotateCameraYaw(1.0);
			aa.requestRedraw();
			return true;
		}
	case osgGA::GUIEventAdapter::KEY_Up:
		{
			if (IsFlying())
				StopFlying();
			if (mpr_lockmode == GLB_LOCKMODEL_2DN || mpr_lockmode == GLB_LOCKMODE_2D)
				return false;
			_ga_t0 = &ea;
			RotateCameraPitch(0.5);
			aa.requestRedraw();
			return true;
		}
	case osgGA::GUIEventAdapter::KEY_Down:
		{
			if (IsFlying())
				StopFlying();
			if (mpr_lockmode == GLB_LOCKMODEL_2DN || mpr_lockmode == GLB_LOCKMODE_2D)
				return false;
			_ga_t0 = &ea;
			RotateCameraPitch(-0.5);
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_W):	
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			glbDouble y = getMoveCoefficient(cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos());
			Move(0,y);
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_S):	
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			glbDouble y = getMoveCoefficient(cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos());
			Move(0,-y);
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_A):	
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			glbDouble x = getMoveCoefficient(cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos());
			Move(x,0);
			aa.requestRedraw();
			return true;
		}
	case (osgGA::GUIEventAdapter::KEY_D):	
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			glbDouble x = getMoveCoefficient(cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos());
			Move(-x,0);
			aa.requestRedraw();
			return true;
		}
	case osgGA::GUIEventAdapter::KEY_Z:
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			glbDouble terrainElevationOfCameraPos = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			glbDouble delt = 0.0;
			if (abs(cameraPos.z() - terrainElevationOfCameraPos) < 0.1)
			{
				delt = 0.1 + abs(cameraPos.z() - terrainElevationOfCameraPos);
			}
			delt = abs(cameraPos.z() - terrainElevationOfCameraPos)*0.2;
			osg::Matrixd mat = getMatrix();
			mat.postMultTranslate(osg::Vec3d(0.0,0.0,-delt));
			setByMatrix(mat);
			//�����ƫ
			GetCameraPos(cameraPos);
			glbDouble elevate = 0.0;
			if (mpr_globe->GetView()->getUpdateCameraMode())
				elevate = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
			else
				elevate = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			UpdateCameraElevate(elevate);
			aa.requestRedraw();
			return true;
		}
	case osgGA::GUIEventAdapter::KEY_C:
		{
			if (IsFlying())
				StopFlying();
			_ga_t0 = &ea;
			glbDouble terrainElevationOfCameraPos = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			osg::Vec3d cameraPos;
			GetCameraPos(cameraPos);
			glbDouble delt = 0.0;
			if (cameraPos.z() > MaxCameraPosZ)
			{
				return false;
			}
			if (abs(cameraPos.z() - terrainElevationOfCameraPos) < 0.1)
			{
				delt = 0.1 + abs(cameraPos.z() - terrainElevationOfCameraPos);
			}
			delt = abs(cameraPos.z() - terrainElevationOfCameraPos);
			osg::Matrixd mat = getMatrix();
			mat.postMultTranslate(osg::Vec3d(0.0,0.0,delt));
			setByMatrix(mat);
			//�����ƫ
			GetCameraPos(cameraPos);
			glbDouble elevate = 0.0;
			if (mpr_globe->GetView()->getUpdateCameraMode())
				elevate = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
			else
				elevate = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
			UpdateCameraElevate(elevate);

			aa.requestRedraw();
			return true;
		}
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

void CGlbPlaneManipulator::home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{ 
	if (mpr_globe)
	{ 
		mpr_minx = mpr_globe->GetTerrainDataset()->GetExtent()->GetLeft();
		mpr_miny = mpr_globe->GetTerrainDataset()->GetExtent()->GetBottom();
		mpr_maxx = mpr_globe->GetTerrainDataset()->GetExtent()->GetRight();
		mpr_maxy = mpr_globe->GetTerrainDataset()->GetExtent()->GetTop();
		mpr_globe->GetTerrainDataset()->GetExtent()->GetCenter(&mpr_focusPos.x(),&mpr_focusPos.y(),&mpr_focusPos.z()); 
		mpr_distance = max(mpr_maxx-mpr_minx,mpr_maxy-mpr_miny)*1.732;
	}
	mpr_speed = osg::Vec2d(0.0,0.0);
	mpr_yaw = 0.0;
	mpr_pitch = -90.0;
	mpr_globe->NotifyCameraIsUnderground(false);
}

GlbGlobeManipulatorTypeEnum CGlbPlaneManipulator::GetType()
{
	return GLB_MANIPULATOR_FREE;
}

glbBool CGlbPlaneManipulator::FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
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

	osg::Matrixd mat = osg::Matrixd::translate(0.0,0.0,distance)*
		osg::Matrixd::rotate(osg::PI/2 + osg::DegreesToRadians(pitch),1.0,0.0,0.0)*
		osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0)*
		osg::Matrixd::translate(pt1.focusPos);
	//��Ŀ�����������ƫ
	osg::Vec3d cameraPos = mat.getTrans();
	//�����������Ľ��㲻��,��ƫ��ʽ2�����ڽ���ľ�ƫ
	if (cameraPos.z()>mpr_maxCameraAlt || cameraPos.x()>mpr_maxx || cameraPos.x()<mpr_minx || cameraPos.y()>mpr_maxy || cameraPos.y()<mpr_miny)
	{
		if (cameraPos.z()>mpr_maxCameraAlt)
			cameraPos.z() = mpr_maxCameraAlt;
		if (cameraPos.x()>mpr_maxx)
			cameraPos.x() = mpr_maxx;
		if (cameraPos.x()<mpr_minx)
			cameraPos.x() = mpr_minx;
		if (cameraPos.y()>mpr_maxy)
			cameraPos.y() = mpr_maxy;
		if (cameraPos.y()<mpr_miny)
			cameraPos.y() = mpr_miny;
		
		osg::Vec3d lookvec = pt1.focusPos - cameraPos;
		pt1.distan = lookvec.length();
		//���pitch��Ϊ����90ʱ��mpr_yaw�򲻱�
		//if (lookvec.x() != 0.0 || lookvec.y() != 0.0)
		//mpr_yaw = osg::RadiansToDegrees(atan2(lookvec.x(),lookvec.y()));
		if (lookvec.z()/pt1.distan > 1.0)
			pt1.pitch = 90.0;
		else if (lookvec.z()/pt1.distan < -1.0)
			pt1.pitch = -90.0;
		else
			pt1.pitch = osg::RadiansToDegrees(asin(lookvec.z()/pt1.distan));
	}

	osg::Camera* pcamera = mpr_globe->GetView()->GetOsgCamera();
	if (pcamera)
	{
		if (IsFlying())
			StopFlying();
		_FlyCallback = new CGlbPlaneFlyCallback(pt0,pt1,seconds);
		pcamera->setUpdateCallback(_FlyCallback.get());
	}
	return true;
}

glbBool CGlbPlaneManipulator::FlyTo2(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
	//����ǰ�����������󽹵㣬���룬��̬
	computeObjMatrix();
	//����·������
	if (mpr_distance > 3*distance)	//��������
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
	//��Ŀ�����������ƫ
	osg::Vec3d cameraPos = mat.getTrans();
	//�����������Ľ��㲻��,��ƫ��ʽ2�����ڽ���ľ�ƫ
	if (cameraPos.z()>mpr_maxCameraAlt || cameraPos.x()>mpr_maxx || cameraPos.x()<mpr_minx || cameraPos.y()>mpr_maxy || cameraPos.y()<mpr_miny)
	{
		if (cameraPos.z()>mpr_maxCameraAlt)
			cameraPos.z() = mpr_maxCameraAlt;
		if (cameraPos.x()>mpr_maxx)
			cameraPos.x() = mpr_maxx;
		if (cameraPos.x()<mpr_minx)
			cameraPos.x() = mpr_minx;
		if (cameraPos.y()>mpr_maxy)
			cameraPos.y() = mpr_maxy;
		if (cameraPos.y()<mpr_miny)
			cameraPos.y() = mpr_miny;

		osg::Vec3d lookvec = pt1.focusPos - cameraPos;
		pt1.distan = lookvec.length();
		//���pitch��Ϊ����90ʱ��mpr_yaw�򲻱�
		//if (lookvec.x() != 0.0 || lookvec.y() != 0.0)
		//mpr_yaw = osg::RadiansToDegrees(atan2(lookvec.x(),lookvec.y()));
		if (lookvec.z()/pt1.distan > 1.0)
			pt1.pitch = 90.0;
		else if (lookvec.z()/pt1.distan < -1.0)
			pt1.pitch = -90.0;
		else
			pt1.pitch = osg::RadiansToDegrees(asin(lookvec.z()/pt1.distan));
	}

	osg::Camera* pcamera = mpr_globe->GetView()->GetOsgCamera();
	if (pcamera)
	{
		if (IsFlying())
			StopFlying();
		if (_FlyCallback)
			_FlyCallback = NULL;
		
		_FlyCallback = new CGlbPlaneFlyCallback(pt0,pt1,seconds);
		pcamera->setUpdateCallback(_FlyCallback.get());
	}
	return true;
}

glbBool CGlbPlaneManipulator::FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, bool repeat)
{  	 
	osg::Vec3d cameraPos = getMatrix().getTrans();
	osg::Vec3d focusPos(xOrlon,yOrlat,zOrAlt);
	UpdataMatrix(cameraPos,focusPos);

	//����·������
	osg::Camera* pcamera = mpr_globe->GetView()->GetOsgCamera();
	if (pcamera)
	{	
		_FlyCallback = new CGlbPlaneFlyCallback(this,seconds);
		pcamera->setUpdateCallback(_FlyCallback.get());
	}
	return true;
}

glbBool CGlbPlaneManipulator::IsFlying()
{
	if (_FlyCallback.valid())
		return !_FlyCallback->getPause();
	return false;
}

glbBool CGlbPlaneManipulator::StopFlying()
{
	if (_FlyCallback.valid() && _FlyCallback->getPause() == false)
		_FlyCallback->setPause(true);
	return true;
}

glbBool CGlbPlaneManipulator::JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch)
{
	mpr_focusPos = osg::Vec3d(xOrlon,yOrlat,zOrAlt);
	mpr_distance = distance;
	mpr_yaw = yaw;
	mpr_pitch = pitch;
	mpr_speed = osg::Vec2d(0.0,0.0);
	if (mpr_lockmode == GLB_LOCKMODE_2D)
	{
		mpr_pitch =-90.0;
	}else if (mpr_lockmode == GLB_LOCKMODEL_2DN)
	{ 
		mpr_pitch = -90.0;
		yaw = 0.0;
	}
	//�����ƫ
	osg::Vec3d cameraPos;
	GetCameraPos(cameraPos);
	
	glbDouble elevate = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevate = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
	else
		elevate = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
	UpdateCameraElevate(elevate);
	UpdateCameraElevateExt2();
	return true;
}

glbBool CGlbPlaneManipulator::SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance)
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
	UpdateCameraElevateExt2();
	if (!mpr_globe->IsUnderGroundMode())	//δ��������ģʽ
	{ 
		mpr_focusPos = old_focus;
		mpr_distance = olddistance;
		osg::Vec3d cameraPos = getMatrix().getTrans();

		glbDouble terrainElevationOfCameraPos = 0.0;
		if (mpr_globe->GetView()->getUpdateCameraMode())
			terrainElevationOfCameraPos = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
		else
			terrainElevationOfCameraPos = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
		//glbDouble terrainElevationOfCameraPos = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());

		if (cameraPos.z() - terrainElevationOfCameraPos < ElevateThreshold)
		{
			cameraPos.z() = terrainElevationOfCameraPos + ElevateThreshold;
			UpdataMatrix(cameraPos,old_focus);
		}
	}
	return true;
}

glbDouble CGlbPlaneManipulator::GetYaw()
{
	return mpr_yaw;
}

glbBool CGlbPlaneManipulator::SetPitch(glbDouble pitch,osg::Vec3d old_focus,glbDouble olddistance)
{
	if (mpr_lockmode == GLB_LOCKMODE_2D || mpr_lockmode == GLB_LOCKMODEL_2DN)
		return false;
	if (pitch > 0 || pitch < -90.0)
	{
		return false;
	}
	mpr_pitch = pitch;

	mpr_focusPos = old_focus;
	mpr_distance = olddistance;
	osg::Vec3d cameraPos = getMatrix().getTrans();
	UpdateCameraElevateExt2();
	glbDouble terrainElevationOfCameraPos = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		terrainElevationOfCameraPos = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
	else
		terrainElevationOfCameraPos = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
	//glbDouble terrainElevationOfCameraPos = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
	if (mpr_globe->IsUnderGroundMode())	//��������ģʽ
	{
		if (cameraPos.z() - mpr_globe->GetUnderGroundAltitude() < ElevateThreshold)
		{
			cameraPos.z() = mpr_globe->GetUnderGroundAltitude()+ElevateThreshold;
			UpdataMatrix(cameraPos,mpr_focusPos);
		}
	}else{	//δ��������ģʽ
		if (cameraPos.z() - terrainElevationOfCameraPos < ElevateThreshold)
		{
			cameraPos.z() = terrainElevationOfCameraPos+ElevateThreshold;
			UpdataMatrix(cameraPos,mpr_focusPos);
		}
	}
	return true;
}

glbDouble CGlbPlaneManipulator::GetPitch()
{
	return mpr_pitch;
}

glbBool CGlbPlaneManipulator::GetCameraPos(osg::Vec3d& cameraPos)
{
	cameraPos = getMatrix().getTrans();
	return true;
}

glbBool CGlbPlaneManipulator::GetFocusPos(osg::Vec3d& focusPos)
{
	focusPos = mpr_focusPos;
	return true;
}

glbDouble CGlbPlaneManipulator::GetDistance()
{
	return mpr_distance;
}

void CGlbPlaneManipulator::SetCameraParam(glbDouble yaw, glbDouble pitch, osg::Vec3d focusPos, glbDouble distance)
{
	if (pitch > 0) pitch = 0;
	if (pitch < -90.0) pitch = -90.0;

	mpr_yaw = yaw;
	mpr_pitch = pitch;
	mpr_focusPos = focusPos;
	mpr_distance = distance;
}

glbBool CGlbPlaneManipulator::Move(glbDouble dangleX,glbDouble dangleY)
{
	osg::Matrix mat = getMatrix();
	glbDouble yaw = osg::DegreesToRadians(mpr_yaw);
	glbDouble x_delt = dangleX*cos(yaw) - dangleY*sin(yaw);
	glbDouble y_delt = dangleX*sin(yaw) + dangleY*cos(yaw);
	mat.postMultTranslate(osg::Vec3d(-x_delt,y_delt,0.0));
	setByMatrix(mat);
	UpdateCameraElevateExt1();
	//�����ƫ
	osg::Vec3d cameraPos;
	GetCameraPos(cameraPos);
	glbDouble elevation = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevation = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
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
	return true;
}

glbBool CGlbPlaneManipulator::TranslateCamera(glbDouble x,glbDouble y,glbDouble z)
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

glbDouble CGlbPlaneManipulator::getMoveCoefficient(glbDouble Alt)
{
	if (abs(Alt) < 100.0)
		Alt = 100.0;
	return Alt*0.01;
}

glbBool CGlbPlaneManipulator::DetectCollision(glbBool mode)
{
	return false;	
}

glbBool CGlbPlaneManipulator::SetUnderGroundMode(glbBool mode)
{
	return mpr_globe->SetUnderGroundMode(mode);
}

glbBool CGlbPlaneManipulator::SetUndergroundDepth(glbDouble zOrAlt)
{
	return mpr_globe->SetUnderGroundAltitude(zOrAlt);
}
glbBool CGlbPlaneManipulator::UpdateFocusElevate(glbDouble elevation)
{
	return true;
}
glbBool CGlbPlaneManipulator::UpdateCameraElevate(glbDouble elevation)
{
	osg::Vec3d cameraPos = getMatrix().getTrans();
	if (mpr_globe->IsUnderGroundMode())	//��������ģʽ
	{ 
		if (abs(cameraPos.z() - elevation) < MinElevateThreshold)
		{// �������������������+-1.0��֮��
			osg::Vec3d newcameraPos = cameraPos;
			if (cameraPos.z() - elevation > 0)
				newcameraPos.z() = MinElevateThreshold+elevation;
			else
				newcameraPos.z() = -MinElevateThreshold+elevation;
			osg::Matrixd mat = getMatrix();
			mat.postMultTranslate(newcameraPos - cameraPos);
			setByMatrix(mat);
			return true;
		}
		if (cameraPos.z() - mpr_globe->GetUnderGroundAltitude() < ElevateThreshold)
		{
			cameraPos.z() = mpr_globe->GetUnderGroundAltitude() + ElevateThreshold;
			UpdataMatrix(cameraPos,mpr_focusPos);
		}
	}else{	//δ��������ģʽ
		if (cameraPos.z() - elevation < ElevateThreshold)
		{
			// ���������ͬʱ����ƽ����ͬ�ľ���,�������߽ǶȲ���,
			// �Խ�����������ܽ�ʱ������߶�ͻ��ᵼ��pitch�Ǿޱ������2015.9.15 ����
			double delta = elevation + ElevateThreshold - cameraPos.z();
			cameraPos.z() = elevation + ElevateThreshold;
			mpr_focusPos.z() += delta;
			UpdataMatrix(cameraPos,mpr_focusPos);
		}
	}
	return true;
}

// �������λ���뽹��λ�ü�����������
void CGlbPlaneManipulator::UpdataMatrix(osg::Vec3d& cameraPos,osg::Vec3d& focusPos)
{ 
	osg::Vec3d lookvec = focusPos - cameraPos;
	mpr_focusPos = focusPos;
	mpr_distance = lookvec.length();
	if (lookvec.z()/mpr_distance > 1.0)
		mpr_pitch = 90.0;
	else if (lookvec.z()/mpr_distance < -1.0)
		mpr_pitch = -90.0;
	else
		mpr_pitch = osg::RadiansToDegrees(asin(lookvec.z()/mpr_distance));
	//���pitch��Ϊ����90ʱ��mpr_yaw�򲻱�
	//if (abs(mpr_pitch) > 89.9)
	if (abs(mpr_pitch) < 89.9)
		mpr_yaw = osg::RadiansToDegrees(atan2(lookvec.x(),lookvec.y()));
}

glbBool CGlbPlaneManipulator::SetLockMode(GlbGlobeLockModeEnum glbType)
{
	if (mpr_lockmode == glbType)
		return false;
	mpr_lockmode = glbType;
	flushMouseEventStack();
	switch(glbType)
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

void CGlbPlaneManipulator::flushMouseEventStack()
{
	_ga_t1 = NULL;
	_ga_t0 = NULL;
}

void CGlbPlaneManipulator::SetCameraPitch(glbDouble pitch)
{ 
	if (mpr_lockmode == GLB_LOCKMODE_2D || mpr_lockmode == GLB_LOCKMODEL_2DN)
		pitch = -90.0;
	glbDouble old_pitch = GetCameraPitch();
	RotateCameraPitch(pitch - old_pitch);
}

glbDouble CGlbPlaneManipulator::GetCameraPitch()
{
	return mpr_pitch;
}

void CGlbPlaneManipulator::RotateCameraPitch(glbDouble pitch)
{
	//switch (mpr_lockmode)
	//{ 
	//case GLB_LOCKMODE_2D:
	//	{
	//		pitch = 0.0;
	//		break;
	//	} 
	//case GLB_LOCKMODEL_2DN:	 
	//	return;
	//default:
	//	break;
	//}
	if (abs(pitch) < ZERO_E7)		   
		return;
	glbDouble old_pitch = GetCameraPitch();
	if (abs(old_pitch + pitch) > 90.0+ZERO_E7) 
		return;

	osg::Matrix mat = getMatrix();
	osg::Vec3d cameraPos = mat.getTrans();
	osg::Vec3d rotate_vec = osg::Vec3d(1.0,0.0,0.0) * osg::Matrixd::rotate(osg::DegreesToRadians(-mpr_yaw),0.0,0.0,1.0);
	mat = mat * osg::Matrixd::translate(-cameraPos) * osg::Matrixd::rotate(osg::DegreesToRadians(pitch),rotate_vec) * osg::Matrixd::translate(cameraPos);
	setByMatrix(mat);
}

void CGlbPlaneManipulator::SetCameraYaw(glbDouble yaw)
{
	if (mpr_lockmode == GLB_LOCKMODEL_2DN)
		yaw = 0.0;
	glbDouble old_yaw = GetCameraYaw();
	RotateCameraYaw(yaw-old_yaw);
}

glbDouble CGlbPlaneManipulator::GetCameraYaw()
{
	return mpr_yaw;
}

void CGlbPlaneManipulator::RotateCameraYaw(glbDouble yaw)
{ 
	if (abs(yaw) < ZERO_E7)
		return;
	osg::Matrixd mat = getMatrix();
	osg::Vec3d cameraPos = mat.getTrans();
	mat = mat * osg::Matrixd::translate(-cameraPos) * osg::Matrixd::rotate(osg::DegreesToRadians(-yaw),0.0,0.0,1.0) * osg::Matrixd::translate(cameraPos);
	setByMatrix(mat);
} 

void CGlbPlaneManipulator::ZoomCursor(glbDouble delt, glbDouble x, glbDouble y)
{
	//����zoomdelt
	glbDouble zoomdelt = DBL_MAX;
	osg::Matrixd mat = getMatrix();
	osg::Vec3d cameraPos = mat.getTrans();
	osg::Vec3d lookvec = mpr_focusPos - cameraPos;
	osg::Vec3d ZoomCenter,ZoomCenter0;

	glbDouble _x = x;
	glbDouble _y = y;
	glbDouble _z = 1.0;	
	mpr_globe->GetView()->ScreenToWorld(_x,_y,_z);
	osg::Vec3d  newlookvec = osg::Vec3d(_x,_y,_z) - cameraPos;
	newlookvec.normalize();

	osg::Vec3d InterPoint;
	glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(x,y,InterPoint);
	//if (isObjfocus)
	//{
	//	ZoomCenter0 = InterPoint;
	//	ZoomCenter = ZoomCenter0;
	//	zoomdelt = (ZoomCenter0-cameraPos).length();
	//}
	//else
	//{
	//	osg::Viewport* viewport = mpr_globe->GetView()->GetOsgCamera()->getViewport();
	//	glbDouble centerx,centery;
	//	mpr_globe->GetView()->getScreenFocusPt(centerx,centery);
	//	isObjfocus = mpr_globe->GetView()->PickNearestObject(x,y,InterPoint);
	//	if (isObjfocus)
	//	{
	//		ZoomCenter0 = InterPoint;
	//		ZoomCenter = ZoomCenter0;
	//		zoomdelt = (ZoomCenter0-cameraPos).length();
	//	}
	//}

	glbBool isIntersect = mpr_globe->IsRayInterTerrain(cameraPos,newlookvec/*lookvec*/,ZoomCenter0);		//�����������
	if(isIntersect)
	{
		glbDouble zoomdelt1 = (ZoomCenter0-cameraPos).length();

		if (zoomdelt1 < zoomdelt)
		{// ���ν����ڶ��󽻵�ǰ��
			zoomdelt = zoomdelt1;
			ZoomCenter = ZoomCenter0;
		}
	}
	glbBool isuIntersect = mpr_globe->IsRayInterTerrain(cameraPos,newlookvec/*-lookvec*/,ZoomCenter0);	//���߷������������
	if (isuIntersect)
	{
		glbDouble uzoomdelt = (ZoomCenter0 - cameraPos).length();		
		if (uzoomdelt < zoomdelt)
		{// ���ν����ڶ��󽻵�ǰ��
			zoomdelt = uzoomdelt;
			ZoomCenter = ZoomCenter0;
		}
	}
	if (!isObjfocus && !isIntersect && !isuIntersect)
	{// �޽��㣬Ĭ��ǰ������Ϊ���泤���ƽ��ֵ��500��֮һ
		zoomdelt = ((mpr_maxx-mpr_minx)+(mpr_maxy-mpr_miny))*0.5/50;
	}

	//����zoomdelt����ZoomSpeed
	glbDouble ZoomSpeed;
	if (zoomdelt > 100000)		//100km
		ZoomSpeed = 0.3;
	else if (zoomdelt > 1000)	//1km	
		ZoomSpeed = 0.2;
	else						//1km����
		ZoomSpeed = 0.1;
	if (mpr_globe->IsUnderGroundMode())	//�����˵���ģʽ
	{	
		glbDouble zoom_break = MinElevateThreshold*10.0;
		if (abs(mpr_pitch) > 1.0)
			zoom_break /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
		else
			zoom_break *= 60;
		if (abs(zoomdelt) > zoom_break)
		{
			if (delt > 0)	//ǰ��
			{
				zoomdelt = -fabs(zoomdelt) * ZoomSpeed;
			}else{
				zoomdelt = fabs(zoomdelt) * ZoomSpeed;
			}
		}else{	//ͻ��
			if (delt > 0)	//ǰ��
				zoomdelt = - (zoomdelt * 2.4);
			else
				zoomdelt = zoomdelt * 2.4;
		}
	}else	//δ��������ģʽ
	{
		if (delt > 0)	//ǰ��
		{
			zoomdelt = -fabs(zoomdelt) * ZoomSpeed;
		}else{
			zoomdelt = fabs(zoomdelt) * ZoomSpeed;
		}
	}

	if ( (!isObjfocus && !isIntersect && !isuIntersect) || delt <= 0/*���*/ )
	{//�������Ļ���ĵ㷽���ƶ�zoomdelt����
		mat.preMultTranslate(osg::Vec3d(0.0,0.0,zoomdelt));
		setByMatrix(mat);
	}
	else
	{//�����lookvec�����ƶ�zoomdelt����
		glbDouble newdis = mpr_distance + zoomdelt;
		lookvec.normalize();
		lookvec *= newdis;
		osg::Vec3d newCamerPos = ZoomCenter - lookvec;
		UpdataMatrix(newCamerPos,ZoomCenter);
	}

	////�����ƫ
	//osg::Vec3d camPos;
	//GetCameraPos(camPos);
	//glbDouble elevation = 0.0;
	//if (mpr_globe->GetView()->getUpdateCameraMode())
	//	elevation = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
	//else
	//	elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();

	//if (mpr_globe->IsUnderGroundMode())
	//{
	//	if (camPos.z() - mpr_globe->GetUnderGroundAltitude() < ElevateThreshold)
	//	{
	//		zoomdelt = camPos.z() - mpr_globe->GetUnderGroundAltitude() - ElevateThreshold;
	//		zoomdelt /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
	//		setByMatrix(osg::Matrixd::translate(0.0,0.0,-zoomdelt) * getMatrix());
	//	}
	//}else{
	//	if (camPos.z() -  elevation< ElevateThreshold)
	//	{
	//		zoomdelt = camPos.z() - elevation - ElevateThreshold;
	//		zoomdelt /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
	//		setByMatrix(osg::Matrixd::translate(0.0,0.0,-zoomdelt) * getMatrix());
	//	}
	//}
	//UpdateCameraElevateExt2();
}

void CGlbPlaneManipulator::Zoom(glbDouble delt)
{ 
	//����zoomdelt
	glbDouble zoomdelt = DBL_MAX;
	osg::Matrixd mat = getMatrix();
	osg::Vec3d cameraPos = mat.getTrans();
	osg::Vec3d lookvec = mpr_focusPos - cameraPos;
	osg::Vec3d ZoomCenter,ZoomCenter0;

	osg::Viewport* viewport = mpr_globe->GetView()->GetOsgCamera()->getViewport();
	glbDouble centerx,centery;
	mpr_globe->GetView()->getScreenFocusPt(centerx,centery);
	//glbBool isObjfocus = mpr_globe->GetView()->Pick(centerx,centery,results,InterPoints);
	osg::Vec3d InterPoint;
	glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(centerx,centery,InterPoint);
	if (isObjfocus)
	{
		ZoomCenter0 = InterPoint;
		ZoomCenter = ZoomCenter0;
		zoomdelt = (ZoomCenter0-cameraPos).length();
	}

	glbBool isIntersect = mpr_globe->IsRayInterTerrain(cameraPos,lookvec,ZoomCenter0);		//�����������
	if(isIntersect)
	{
		glbDouble zoomdelt1 = (ZoomCenter0-cameraPos).length();
		
		if (zoomdelt1 < zoomdelt)
		{// ���ν����ڶ��󽻵�ǰ��
			zoomdelt = zoomdelt1;
			ZoomCenter = ZoomCenter0;
		}
	}
	glbBool isuIntersect = mpr_globe->IsRayInterTerrain(cameraPos,-lookvec,ZoomCenter0);	//���߷������������
	if (isuIntersect)
	{
		glbDouble uzoomdelt = (ZoomCenter0 - cameraPos).length();		
		if (uzoomdelt < zoomdelt)
		{
			zoomdelt = uzoomdelt;
			ZoomCenter = ZoomCenter0;
		}
	}

	if (mpr_bUseReferencePlane)
	{// ʹ������ο���
		// ����mpr_focus,camerapos����������ο���Ľ���ZoomCenter0
		osg::Vec3d nomalvec(0.0,0.0,1.0);
		std::vector<osg::Vec3d> IntersectPoses;
		osg::Vec3d planPos((mpr_maxx+mpr_minx)*0.5, (mpr_miny+mpr_maxy)*0.5, mpr_referencePlaneZ);
		glbInt32 IntersectResult = IntersectRayPlane(cameraPos,mpr_focusPos,planPos,nomalvec,IntersectPoses);
		// if (uzoomdelt < zoomdelt) ��¼�˽���ΪĿ�������
		if (IntersectResult)
		{
			osg::Vec3d IntersectPos = IntersectPoses[0];	//��������
			glbDouble zoomdelt1 = (IntersectPos-cameraPos).length();

			if (zoomdelt1 < zoomdelt)
			{// ��������ν�������������ǰ��
				zoomdelt = zoomdelt1;
				ZoomCenter = IntersectPos;
			}
		}
	}

	if (!isObjfocus && !isIntersect && !isuIntersect)
	{// �޽��㣬Ĭ��ǰ������Ϊ���泤���ƽ��ֵ��500��֮һ
		zoomdelt = ((mpr_maxx-mpr_minx)+(mpr_maxy-mpr_miny))*0.5/50;
	}

	//����zoomdelt����ZoomSpeed
	glbDouble ZoomSpeed;
	if (zoomdelt > 100000)		//100km
		ZoomSpeed = 0.3;
	else if (zoomdelt > 1000)	//1km	
		ZoomSpeed = 0.2;
	else						//1km����
		ZoomSpeed = 0.1;
	if (mpr_globe->IsUnderGroundMode())	//�����˵���ģʽ
	{	
		glbDouble zoom_break = MinElevateThreshold*10.0;
		if (abs(mpr_pitch) > 1.0)
			zoom_break /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
		else
			zoom_break *= 60;
		if (abs(zoomdelt) > zoom_break)
		{
			if (delt > 0)	//ǰ��
			{
				zoomdelt = -fabs(zoomdelt) * ZoomSpeed;
			}else{
				zoomdelt = fabs(zoomdelt) * ZoomSpeed;
			}
		}else{	//ͻ��
			if (delt > 0)	//ǰ��
				zoomdelt = - (zoomdelt * 2.4);
			else
				zoomdelt = zoomdelt * 2.4;
		}
	}else	//δ��������ģʽ
	{
		if (delt > 0)	//ǰ��
		{
			zoomdelt = -fabs(zoomdelt) * ZoomSpeed;
		}else{
			zoomdelt = fabs(zoomdelt) * ZoomSpeed;
		}
	}
	
	//�����lookvec�����ƶ�zoomdelt����
	mat.preMultTranslate(osg::Vec3d(0.0,0.0,zoomdelt));

	// ����������Χ����ֹ
	if (IsCameraPosInRange(mat)==false)
		return;

	setByMatrix(mat);
	//�����ƫ
	osg::Vec3d camPos;
	GetCameraPos(camPos);
	glbDouble elevation = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevation = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
	else
		elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();

	if (mpr_globe->IsUnderGroundMode())
	{
		if (camPos.z() - mpr_globe->GetUnderGroundAltitude() < ElevateThreshold)
		{
			zoomdelt = camPos.z() - mpr_globe->GetUnderGroundAltitude() - ElevateThreshold;
			zoomdelt /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
			setByMatrix(osg::Matrixd::translate(0.0,0.0,-zoomdelt) * getMatrix());
		}
	}else{
		if (camPos.z() -  elevation< ElevateThreshold)
		{
			zoomdelt = camPos.z() - elevation - ElevateThreshold;
			zoomdelt /= sin(abs(osg::DegreesToRadians(mpr_pitch)));
			setByMatrix(osg::Matrixd::translate(0.0,0.0,-zoomdelt) * getMatrix());
		}
	}
	UpdateCameraElevateExt2();
}

void CGlbPlaneManipulator::Rotate(glbDouble lonOrX,glbDouble latOrY,glbDouble altOrZ,glbDouble pitch,glbDouble yaw)
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
		pitch = -90.0 - mpr_pitch;
	else if (mpr_pitch + pitch > 90.0)
		pitch = 90.0 - mpr_pitch;

	//��תpitch,��תyaw
	osg::Matrixd mat = getMatrix();
	osg::Vec3d rotate_center(lonOrX,latOrY,altOrZ);

	osg::Vec3d rotate_yaw = osg::Vec3d(0.0,0.0,1.0);
	osg::Vec3d rotate_ptch = osg::Vec3d(1.0,0.0,0.0) * osg::Matrixd::rotate(osg::DegreesToRadians(-mpr_yaw),0.0,0.0,1.0);
	// ��rotate_centerΪ������ת��������Ҫ��ƽ�Ƶ�rotate_centerλ��
	mat = mat * osg::Matrixd::translate(-rotate_center)*
		osg::Matrixd::rotate(osg::DegreesToRadians(pitch),rotate_ptch)*
		osg::Matrixd::rotate(osg::DegreesToRadians(yaw),rotate_yaw)*
		osg::Matrixd::translate(rotate_center);

	//�����ƫ
	osg::Vec3d cameraPos = mat.getTrans();

	// ���½�����<MinElevateThresholdλ���ڸ߶�ȫ����ΪMinElevateThreshold�߶ȣ����������̫����������
	if (mpr_globe->IsUnderGroundMode())	//��������ģʽ
	{
		if (cameraPos.z() - mpr_globe->GetUnderGroundAltitude() < ElevateThreshold)
		{
			return;
		}
		else if (abs(cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos()) < MinElevateThreshold)
		{
			glbDouble deltalt = 0.0;
			if (cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() > 0)
				deltalt = cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() - MinElevateThreshold;
			else
				deltalt = cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() + MinElevateThreshold;
			mat.postMultTranslate(osg::Vec3d(0.0,0.0,-deltalt));
		}
	}else{
		if (cameraPos.z() - mpr_globe->GetView()->GetTerrainElevationOfCameraPos() < ElevateThreshold)
		{
			return;
		}
	}// end ���½�����<MinElevateThreshold......

	// �����ת���߽�λ�ã�ֹͣ��ת
	if (IsCameraPosInRange(mat)==false)
		return;

	setByMatrix(mat);

	GetCameraPos(cameraPos);
	glbDouble elevation = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevation = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
	else
		elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();
	UpdateCameraElevate(elevation);
}

void CGlbPlaneManipulator::Drag(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2)
{
	//��������밴�µ����ƽ�߼н�pushpitch
	osg::Vec3d pushcamersPos = _pushMatrix.getTrans();
	osg::Vec3d pushcamersPos_atpushPos = pushcamersPos - _pushPos;
	pushcamersPos_atpushPos.normalize();
	//pushcamersPos_atpushPos.z() = osg::clampBetween(pushcamersPos_atpushPos.z(),-1.0,1.0);
	osg::Vec3d pushcamersPos_atpushPos_xy(pushcamersPos_atpushPos.x(),pushcamersPos_atpushPos.y(),0);
	glbDouble pushpitch = acos((pushcamersPos_atpushPos*pushcamersPos_atpushPos_xy)/(pushcamersPos_atpushPos.length()*pushcamersPos_atpushPos_xy.length()));//asin(pushcamersPos_atpushPos.z());
	pushpitch = osg::RadiansToDegrees(pushpitch);

	//if(abs(mpr_pitch) < 30.0)	//����������²������ ����밴�µ����ƽ�߼нǺ�С�����
	//{
		////��������밴�µ����ƽ�߼н�
		//osg::Vec3d pushcamersPos = _pushMatrix.getTrans();
		//osg::Vec3d pushcamersPos_atpushPos = pushcamersPos - _pushPos;
		//pushcamersPos_atpushPos.normalize();
		////pushcamersPos_atpushPos.z() = osg::clampBetween(pushcamersPos_atpushPos.z(),-1.0,1.0);
		//if (pushcamersPos_atpushPos.z() > 1.0)
		//	pushcamersPos_atpushPos.z() = 1.0;
		//else if (pushcamersPos_atpushPos.z() < -1.0)
		//	pushcamersPos_atpushPos.z() = -1.0;
		//glbDouble pushpitch = asin(pushcamersPos_atpushPos.z());
		//pushpitch = osg::RadiansToDegrees(pushpitch);
		//if (abs(pushpitch) < 10.0)	//С�Ƕ����⴦��
		//{
		//	//�߼������������������ _push������ƽ����
		//	osg::Vec3d cameraPos_w = _pushMatrix.getTrans();
		//	osg::Vec3d up(0.0,0.0,1.0);
		//	osg::Vec3d east(1.0,0.0,0.0);	//east
		//	osg::Vec3d eastEx = east*osg::Matrix::rotate(osg::DegreesToRadians(-mpr_yaw),up);
		//	osg::Vec3d nomalvec = up*osg::Matrix::rotate(osg::DegreesToRadians(30.0-abs(mpr_pitch)),eastEx);
		//	nomalvec.normalize();
		//	osg::Matrixd mat = _pushMatrix;	
		//	osg::Matrixd inverseVPW;			
		//	inverseVPW.invert(_pushVPW);
		//	osg::Vec3d startPos_h(ptx2,pty2,0.0);
		//	osg::Vec3d endPos_h(ptx2,pty2,1.0);
		//	startPos_h = startPos_h * inverseVPW;
		//	endPos_h = endPos_h * inverseVPW;
		//	std::vector<osg::Vec3d> IntersectPoses;
		//	glbInt32 IntersectResult = IntersectRayPlane(startPos_h,endPos_h,_pushPos,nomalvec,IntersectPoses);
		//	if (IntersectResult)
		//	{
		//		osg::Vec3d IntersectPos = IntersectPoses[0];	//��������
		//		osg::Vec3d delt = (_pushPos-IntersectPos);
		//		mat.postMultTranslate(delt);
		//	}
		//	glbDouble deltz = mat.getTrans().z() - _pushMatrix.getTrans().z();
		//	mat.postMultTranslate(osg::Vec3d(0.0,0.0,deltz));
		//	setByMatrix(mat);
		//	UpdateCameraElevateExt1();
		//	osg::Vec3d cameraPos;
		//	GetCameraPos(cameraPos);
		//	glbDouble elevation = 0.0;
		//	if (mpr_globe->GetView()->getUpdateCameraMode())
		//		elevation = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
		//	else
		//		elevation = mpr_globe->GetView()->GetTerrainElevationOfCameraPos();

		//	UpdateCameraElevate(elevation);
		//	return;
		//}
	//}
	osg::Vec3d startPos(ptx2,pty2,0.0);
	osg::Vec3d endPos(ptx2,pty2,1.0);
	osg::Matrixd inverseVPW;
	inverseVPW.invert(_pushVPW);
	startPos = startPos * inverseVPW;
	endPos = endPos * inverseVPW;
	osg::Vec3d nomalvec(0.0,0.0,1.0);
	std::vector<osg::Vec3d> IntersectPoses;
	glbInt32 IntersectResult = IntersectRayPlane(startPos,endPos,_pushPos,nomalvec,IntersectPoses);
	//osg::Matrixd matrix;
	if (IntersectResult)
	{		
		osg::Vec3d IntersectPos = IntersectPoses[0];	//��������
		osg::Vec3d delt = (_pushPos-IntersectPos);		
		if (fabs(mpr_pitch) <= 1.0/*4.0*/ /*&& fabs(pushpitch)<15.0*/ && abs(pty2-pty1)>abs(ptx2-ptx1)*0.17632698/*��ֱ����80�Ƚ�����*/)
		{// ������밴�µ�ƽ��н�<=4��ʱ����������������ֱ�����ƶ�
			{
				// ͻȻ�ķ���ı�,��Ҫ���⴦��������Ϊ��һ�εķ���
				if (!mpr_bFirstDrag && 
					(mpr_dragDir.x()*delt.x() < 0 || mpr_dragDir.y()*delt.y() < 0 || mpr_dragDir.z()*delt.z() < 0))
				{
					delt = mpr_dragDir ;
				}
				mpr_bFirstDrag = false;
				mpr_dragDir = delt;
			}
			delt.normalize();
			double _step = min(mpr_maxx-mpr_minx,mpr_maxy-mpr_miny)*0.00005;			
			double _p = sqrt(double(pty2-pty1)*(pty2-pty1) + double(ptx2-ptx1)*(ptx2-ptx1));
			delt *= (_p * _step);

			//WCHAR buff[256];
			//swprintf(buff,L"Drag delt x:%.4lf, y:%.4lf,z:%.4lf , p:%.0lf, step:%.2lf\n",delt.x(),delt.y(),delt.z(),_p,_step);
			//OutputDebugString(buff);

			//setByMatrix(_pushMatrix * osg::Matrixd::translate(delt));			
		}
		else
		{
			//WCHAR buff[256];
			//swprintf(buff,L"Drag normal.............., pitch:%.02lf, pushpitch:%.2lf, cy:%d , cx:%d\n",mpr_pitch,pushpitch,abs(pty2-pty1),abs(ptx2-ptx1));
			//OutputDebugString(buff);
			//setByMatrix(_pushMatrix * osg::Matrixd::translate(delt));		

		}
		// ��ק����Χֱ�ӷ���
		if (IsCameraPosInRange(_pushMatrix * osg::Matrixd::translate(delt))==false)
			return;

		setByMatrix(_pushMatrix * osg::Matrixd::translate(delt));	
		//matrix = _pushMatrix * osg::Matrixd::translate(delt);
	}
	// ���������Χ
	//UpdateCameraElevateExt1();
	osg::Vec3d cameraPos;
	GetCameraPos(cameraPos);	
	glbDouble elevation = 0.0;
	if (mpr_globe->GetView()->getUpdateCameraMode())
		elevation = mpr_globe->GetElevationAt(cameraPos.x(),cameraPos.y());
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

//����������򲻱�,��ƫ��ʽ1,��������ľ�ƫ -- ����߽�Լ��
void CGlbPlaneManipulator::UpdateCameraElevateExt1()
{
	// �����߽�
	//UpdateCameraElevateExt();
	osg::Vec3d cameraPos = getMatrix().getTrans();
	//����������򲻱�,��ƫ��ʽ1,��������ľ�ƫ
	if (cameraPos.z()>mpr_maxCameraAlt || 
		cameraPos.x()>mpr_camera_maxx || cameraPos.x()<mpr_camera_minx || 
		cameraPos.y()>mpr_camera_maxy || cameraPos.y()<mpr_camera_miny)
	{// �������������Χʱ����������ڱ߽�λ��	
		glbDouble delt_x = 0.0,delt_y = 0.0,delt_z = 0.0;
		if (cameraPos.z()>mpr_maxCameraAlt)
			delt_z = cameraPos.z() - mpr_maxCameraAlt;
		if (cameraPos.x()>mpr_camera_maxx)
			delt_x = cameraPos.x() - mpr_camera_maxx;
		if (cameraPos.x()<mpr_camera_minx)
			delt_x = cameraPos.x() - mpr_camera_minx;
		if (cameraPos.y()>mpr_camera_maxy)
			delt_y = cameraPos.y() - mpr_camera_maxy;
		if (cameraPos.y()<mpr_camera_miny)
			delt_y = cameraPos.y() - mpr_camera_miny;
		setByMatrix(getMatrix() * osg::Matrix::translate(-delt_x,-delt_y,-delt_z));
	}
}

// �����������Ľ��㲻��,��ƫ��ʽ2�����ڽ���ľ�ƫ
void CGlbPlaneManipulator::UpdateCameraElevateExt2()
{
	// �����ƫǰ����������߽�
	//UpdateCameraElevateExt();
	osg::Vec3d cameraPos = getMatrix().getTrans();
	//�����������Ľ��㲻��,��ƫ��ʽ2�����ڽ���ľ�ƫ
	if (cameraPos.z()>mpr_maxCameraAlt || 
		cameraPos.x()>mpr_camera_maxx || cameraPos.x()<mpr_camera_minx || 
		cameraPos.y()>mpr_camera_maxy || cameraPos.y()<mpr_camera_miny)
	{
		if (cameraPos.z()>mpr_maxCameraAlt)
			cameraPos.z() = mpr_maxCameraAlt;
		if (cameraPos.x()>mpr_camera_maxx)
			cameraPos.x() = mpr_camera_maxx;
		if (cameraPos.x()<mpr_camera_minx)
			cameraPos.x() = mpr_camera_minx;
		if (cameraPos.y()>mpr_camera_maxy)
			cameraPos.y() = mpr_camera_maxy;
		if (cameraPos.y()<mpr_camera_miny)
			cameraPos.y() = mpr_camera_miny;
		UpdataMatrix(cameraPos,mpr_focusPos);

		//glbDouble delt = 0.0;
		//if (cameraPos.z()>mpr_maxCameraAlt)
		//{
		//	delt = cameraPos.z() - mpr_maxCameraAlt;
		//	delt /= -sin(osg::DegreesToRadians(-mpr_pitch));
		//}
		//if (cameraPos.x()>mpr_maxx)
		//{
		//	delt = cameraPos.x() - mpr_maxx;
		//	delt /= sin(osg::DegreesToRadians(mpr_yaw)); 
		//}
		//if (cameraPos.x()<mpr_minx)
		//{
		//	delt = cameraPos.x() - mpr_minx;
		//	delt /= sin(osg::DegreesToRadians(mpr_yaw));
		//}
		//if (cameraPos.y()>mpr_maxy)
		//{
		//	delt = cameraPos.x() - mpr_maxy;
		//	delt /= -cos(osg::DegreesToRadians(mpr_yaw));
		//}
		//if (cameraPos.y()<mpr_miny)
		//{
		//	delt = cameraPos.x() - mpr_miny;
		//	delt /= cos(osg::DegreesToRadians(mpr_yaw));
		//}
		//setByMatrix(osg::Matrixd::translate(0.0,0.0,delt) * getMatrix());
	}
}

// �����ƫǰ����������߽�
void CGlbPlaneManipulator::UpdateCameraElevateExt()
{
	// ceshi����߽粻���� malin 2016.3.16
	return;

	osg::Vec3d cameraPos = getMatrix().getTrans();
	mpr_camera_miny = mpr_miny;
	mpr_camera_minx = mpr_minx;
	mpr_camera_maxy = mpr_maxy; 
	mpr_camera_maxx = mpr_maxx; 
	//����ӽ�
	glbDouble angle_cam,asp,znear,zfar;
	mpr_globe->GetView()->GetOsgCamera()->getProjectionMatrixAsPerspective(angle_cam,asp,znear,zfar);
	glbDouble distan = abs(cameraPos.z() - mpr_oldfocusPos.z());
	if (abs(mpr_pitch) < 90.0 - angle_cam*0.5)	//����ӽ�һ��Ĳ��ǣ�60.0
	{
		if (mpr_yaw < mpr_angel && mpr_yaw >= 0.0)	//miny,minx
		{ 
			mpr_camera_miny = mpr_miny - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(mpr_yaw));		
			mpr_camera_minx = mpr_minx - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(mpr_yaw));
			mpr_camera_maxy = mpr_maxy - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(mpr_yaw))*1.5;
			mpr_camera_maxx = mpr_maxx - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(mpr_yaw))*1.5;
		}else if (mpr_yaw < 0.0 && mpr_yaw >= -mpr_angel) //miny,maxx
		{
			mpr_camera_miny = mpr_miny - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(-mpr_yaw));			
			mpr_camera_maxx = mpr_maxx + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(-mpr_yaw));
			mpr_camera_maxy = mpr_maxy - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(-mpr_yaw))*1.5;;
			mpr_camera_minx = mpr_minx + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(-mpr_yaw))*1.5;;
		}else if (mpr_yaw < 90+mpr_angel && mpr_yaw >= 90.0)	//minx,maxy
		{ 
			mpr_camera_minx = mpr_minx - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(mpr_yaw-90));
			mpr_camera_maxy = mpr_maxy + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(mpr_yaw-90));
			mpr_camera_maxx = mpr_maxx - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(mpr_yaw-90))*1.5;;
			mpr_camera_miny = mpr_miny + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(mpr_yaw-90))*1.5;;
		}else if (mpr_yaw < 90.0 && mpr_yaw >= mpr_angel)		//minx,miny
		{
			mpr_camera_minx = mpr_minx - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(90-mpr_yaw));
			mpr_camera_miny = mpr_miny - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(90-mpr_yaw));
			mpr_camera_maxx = mpr_maxx - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(90-mpr_yaw))*1.5;;
			mpr_camera_maxy = mpr_maxy - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(90-mpr_yaw))*1.5;;
		}else if (mpr_yaw < -90-mpr_angel && mpr_yaw >= -180)	//maxy,minx
		{
			mpr_camera_maxy = mpr_maxy + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(mpr_yaw+180));
			mpr_camera_minx = mpr_minx - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(mpr_yaw+180));
			mpr_camera_miny = mpr_miny + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(mpr_yaw+180))*1.5;;
			mpr_camera_maxx = mpr_maxx - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(mpr_yaw+180))*1.5;;
		}else if (mpr_yaw <= 180 && mpr_yaw >= 90+mpr_angel)	//maxy,maxx
		{
			mpr_camera_maxy = mpr_maxy + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(180-mpr_yaw));
			mpr_camera_maxx = mpr_maxx + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(180-mpr_yaw));
			mpr_camera_miny = mpr_miny + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(180-mpr_yaw))*1.5;;
			mpr_camera_minx = mpr_minx + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(180-mpr_yaw))*1.5;;
		}else if (mpr_yaw < -mpr_angel && mpr_yaw >= -90)	//maxx,miny
		{ 
			mpr_camera_maxx = mpr_maxx + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(mpr_yaw+90));
			mpr_camera_miny = mpr_miny - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(mpr_yaw+90));
			mpr_camera_minx = mpr_minx + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(mpr_yaw+90))*1.5;;
			mpr_camera_maxy = mpr_maxy - distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(mpr_yaw+90))*1.5;;
		}else if (mpr_yaw < -90 && mpr_yaw >= -90-mpr_angel)	//maxx,maxy
		{
			mpr_camera_maxx = mpr_maxx + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(-mpr_yaw-90));
			mpr_camera_maxy = mpr_maxy + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(-mpr_yaw-90));
			mpr_camera_minx = mpr_minx + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*cos(osg::DegreesToRadians(-mpr_yaw-90))*1.5;;
			mpr_camera_miny = mpr_miny + distan/tan(osg::DegreesToRadians(abs(mpr_pitch)+angle_cam*0.5))*sin(osg::DegreesToRadians(-mpr_yaw-90))*1.5;;
		}
	}
}

glbInt32 CGlbPlaneManipulator::IntersectRayPlane(osg::Vec3d startPos,osg::Vec3d endPos,osg::Vec3d planePos,osg::Vec3d nomalvec,std::vector<osg::Vec3d>& IntersectPos)
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
			return -1;//�ཻ��ֱ��ȫ��ƽ����
		else
			return 0;//û�н�����ֱ����ƽ��ƽ��
	}
	double t = 0.0;
	osg::Vec4d factor(nomalvec.x(),nomalvec.y(),nomalvec.z(),-nomalvec*planePos);
	t = -(factor.x() * startPos.x() + factor.y() * startPos.y() + factor.z() * startPos.z() + factor.w()) / 
		(lineDir.x() * factor.x() + lineDir.y() * factor.y() + lineDir.z() * factor.z());

	osg::Vec3d point = startPos + lineDir*t;
	IntersectPos.push_back(point);
	return 1;
}

glbBool CGlbPlaneManipulator::computeObjMatrix()
{
	//��ȡ���λ��
	osg::Matrixd matrix = getMatrix();
	osg::Vec3d cameraPos = matrix.getTrans();
	//��ȡ����
	glbDouble fx,fy;
	mpr_globe->GetView()->getScreenFocusPt(fx,fy);
	//glbBool isObjfocus = mpr_globe->GetView()->Pick(fx,fy,results,InterPoints);
	osg::Vec3d InterPoint;
	glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(fx,fy,InterPoint);
	if (isObjfocus)
	{		
		glbDouble distan = (InterPoint - cameraPos).length();
		if (distan < mpr_distance)	//������������ཻ
		{
			//����distance
			mpr_distance = distan;
			mpr_focusPos = InterPoint;
			return true;
		}
	}
	return false;
}

void CGlbPlaneManipulator::Shake(glbBool isShake)
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

void CGlbPlaneManipulator::SetShakeSope(glbDouble horizontalDegree, glbDouble verticalDegree)
{
	mpr_shakeHorDegree = horizontalDegree;
	mpr_shakeVerDegree = verticalDegree;
}

void CGlbPlaneManipulator::SetShakeTime(glbDouble seconds)
{
	mpr_shakeTime = seconds;
}

void CGlbPlaneManipulator::EnalbeVirtualReferencePlane(glbBool bEnable)
{
	mpr_bUseReferencePlane = bEnable;
}		
void CGlbPlaneManipulator::SetVirtualReferencePlane(glbDouble zOrAltitude)
{
	mpr_referencePlaneZ = zOrAltitude;
}		
glbDouble CGlbPlaneManipulator::GetVirtualReferencePlane()
{
	return mpr_referencePlaneZ;
}
void CGlbPlaneManipulator::Push(glbInt32 ptx,glbInt32 pty)
{
	// ��¼��갴��ʱ����������VPW����,�Ա�ʹ��
	_pushMatrix = getMatrix();
	osg::Camera* p_camera = mpr_globe->GetView()->GetOsgCamera();
	_pushVPW = p_camera->getViewMatrix() * p_camera->getProjectionMatrix() * p_camera->getViewport()->computeWindowMatrix();

	//��¼ѡ�е㾭γ��
	glbDouble dx = ptx;
	glbDouble dy = pty;
	//��������ϵ(���λ��) ת��Ϊ ��Ļ����
	glbBool ischoose = mpr_globe->ScreenToTerrainCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());
	if (!ischoose)
	{// �����û�н���������²ο�����
		if (mpr_globe->IsUnderGroundMode())	  //��������ģʽ 					
			ischoose = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());					
	}
	glbDouble ptx1_s = dx,pty1_s = dy;
	mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);			
	osg::Vec3d objInterPoint;
	glbBool isObjfocus = mpr_globe->GetView()->PickNearestObject(ptx1_s,pty1_s,objInterPoint);			

	if (!ischoose && !isObjfocus)
	{// ��û�н��㣬��ֱ��� ������͵����ϵ�����ο�ƽ�� �� ���²ο��棨������չ���󽻣�ȡ���������췽�����Ҿ����������ĵ���Ϊ�ο�����
		osg::Vec3d startPos(dx,dy,0.0);
		osg::Vec3d endPos(dx,dy,1.0);
		osg::Matrixd inverseVPW;
		inverseVPW.invert(_pushVPW);
		startPos = startPos * inverseVPW;
		endPos = endPos * inverseVPW;
		osg::Vec3d nomalvec(0.0,0.0,1.0);

		osg::Vec3d dir = endPos - startPos;
		dir.normalize();
		// ���û������ο������������͵�����ο�ƽ����
		osg::Vec3d centerPos((mpr_minx+mpr_maxx)*0.5,(mpr_miny+mpr_maxy)*0.5,mpr_minz);
		if (mpr_bUseReferencePlane)
		{// ʹ������ο���.
			centerPos.set((mpr_minx+mpr_maxx)*0.5,(mpr_miny+mpr_maxy)*0.5,mpr_referencePlaneZ);
		}

		std::vector<osg::Vec3d> IntersectPoses;
		glbInt32 IntersectResult = IntersectRayPlane(startPos,endPos,centerPos,nomalvec,IntersectPoses);
		osg::Vec3d interTerrain;
		osg::Vec3d interUnPlane;

		bool isInterTerrainAtFront = true;
		bool isInterUnPlaneAtFront = true;
		if (IntersectResult > 0)
		{
			interTerrain = IntersectPoses[0];
			osg::Vec3d tt = interTerrain - startPos;
			if (tt.x()*dir.x() < 0 || tt.y()*dir.y() < 0 || tt.z()*dir.z() < 0)
				isInterTerrainAtFront = false;
		}

		if (mpr_globe->IsUnderGroundMode())
		{// ���������²ο���ʱ������²ο�������ƽ����
			glbDouble alt = mpr_globe->GetUnderGroundAltitude();
			centerPos.set(centerPos.x(),centerPos.y(),alt);
			glbInt32 IntersectResult2 = IntersectRayPlane(startPos,endPos,centerPos,nomalvec,IntersectPoses);
			if (IntersectResult2 > 0)
			{
				interUnPlane = IntersectPoses[0];
				osg::Vec3d tt = interUnPlane - startPos;
				if (tt.x()*dir.x() < 0 || tt.y()*dir.y() < 0 || tt.z()*dir.z() < 0)
					isInterUnPlaneAtFront = false;

				if (isInterTerrainAtFront && isInterUnPlaneAtFront)
				{
					double dis1 = (interTerrain - startPos).length();
					double dis2 = (interUnPlane - startPos).length();								
					if (dis1 < dis2)
						_pushPos = interTerrain;
					else
						_pushPos = interUnPlane;
				}
				else
				{
					if (isInterTerrainAtFront)
						_pushPos = interTerrain;
					if (isInterUnPlaneAtFront)
						_pushPos = interUnPlane;
				}
			}
		}
		else
		{// û�п�������ģʽֱ��
			osg::Vec3d tt = interTerrain - startPos;
			if (isInterTerrainAtFront = false)
			{// ���������߷����෴����ʾ�����ڱ�������Ҫ�������Ƶ�ǰ��
				_pushPos = startPos + dir * tt.length();
			}
			else
				_pushPos = interTerrain;
		}

	}
	else if (isObjfocus && ischoose)
	{// �Ƚ϶����ϵ�pick��͵���/���²ο���/���òο����ϵ�pick���ĸ������������ѡ���ĸ�����pushPos
		osg::Vec3d cameraPos_w = getMatrix().getTrans();
		glbDouble dis1 = (cameraPos_w - objInterPoint).length();
		glbDouble dis2 = (cameraPos_w - _pushPos).length();
		if (dis1 < dis2)
		{// �����ϵ�pick���������
			_pushPos = objInterPoint;
		}
	}
	else if (isObjfocus && !ischoose)
	{
		_pushPos = objInterPoint;
	}
	mpr_bFirstDrag = true;			
}

void CGlbPlaneManipulator::Rotate(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2)
{
	//��ȡ��ק������
	glbDouble dx = ptx2 - ptx1;
	glbDouble dy = pty2 - pty1;

	glbDouble yaw = -dx*0.3;	//ϵ���ݶ�0.3
	glbDouble pitch = dy*0.3;
	Rotate(_pushPos.x(),_pushPos.y(),_pushPos.z(),pitch,yaw);	
}

void CGlbPlaneManipulator::Zoom(glbBool isScrollUp)
{
	glbFloat zoomDelta = isScrollUp ? 1.0 : -1.0;
	Zoom(zoomDelta);
}

glbBool CGlbPlaneManipulator::IsCameraPosInRange(osg::Matrixd& _mat)
{
	osg::Vec3d cameraPos = _mat.getTrans();
	//�����������Ľ��㲻��,��ƫ��ʽ2�����ڽ���ľ�ƫ
	if (cameraPos.z()>mpr_maxCameraAlt || 
		cameraPos.x()>mpr_camera_maxx || cameraPos.x()<mpr_camera_minx || 
		cameraPos.y()>mpr_camera_maxy || cameraPos.y()<mpr_camera_miny)
	{
		return false;
	}
	return true;
}