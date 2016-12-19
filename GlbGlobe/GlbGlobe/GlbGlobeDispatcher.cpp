// This is the main DLL file.

#include "StdAfx.h"
#include "GlbGlobeDispatcher.h"
#include "GlbGlobeRObject.h"
#include "GlbGlobeTaskManager.h"
#include "GlbGlobeCallBack.h"
#include "GlbGlobeView.h"
#include "CGlbGlobe.h"
#include "GlbGlobeMemCtrl.h"


//Ԥ���ڴ���ֵ50M
#define SURPLUSMEMORY 0

//*   �����Կ���֡�����������   */
//10*1024*1024
#define FRAME_DEAL_SIZE    10485760  //��֡����. 
#define GLB_MAX_OBJTASKNUM 5
//10*1024*1024
#define GLB_MAX_UNLOADSIZE 10485760

using namespace GlbGlobe;

CGlbGlobeDispatcher::CGlbGlobeDispatcher(CGlbGlobe* globe)
{
	mpr_globe               = globe;
	mpr_eyePosition         = osg::Vec3d(0.0,0.0,0.0);
	mpr_yaw                 = 0.0;
	mpr_pitch               = 0.0;
	mpr_numOfObjs           = 0;	
	mpr_needTerminateUpdate = false;
	mpr_lastLoadObj         = NULL;	
	mpr_lastRemoveObj       = NULL;
	mpr_lastRemoveTime      = osg::Timer::instance()->tick();
	mpr_preUpateTime        = osg::Timer::instance()->tick();
	mpr_critical.SetName(L"globe_dispatcher");
}

CGlbGlobeDispatcher::~CGlbGlobeDispatcher()
{
}

void CGlbGlobeDispatcher::GetTerrainObjects(std::vector<glbref_ptr<CGlbGlobeRObject>> &tobjs,
	                                        std::vector<glbref_ptr<CGlbGlobeRObject>> &robjs,
	                                        std::vector<glbref_ptr<CGlbGlobeRObject>> &mobjs)
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	tobjs = mpr_terrainObjs;
	robjs = mpr_rTerrainObjs;
	mobjs = mpr_mTerrainObjs;
}

void CGlbGlobeDispatcher::GetTerrainObjectsAndDirtyExts(std::vector<glbref_ptr<CGlbGlobeRObject>> &tobjs,
														std::vector<glbref_ptr<CGlbGlobeRObject>> &robjs,
														std::vector<glbref_ptr<CGlbGlobeRObject>> &mobjs,
														std::vector<CGlbExtent>	&objDirtyExts,
														std::vector<CGlbExtent>	&domrDirtyExts,
														std::vector<CGlbExtent>	&mtobjDirtyExts,
														std::vector<CGlbExtent>	&demDirtyExts)
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	tobjs = mpr_terrainObjs;
	robjs = mpr_rTerrainObjs;
	mobjs = mpr_mTerrainObjs;

	objDirtyExts   = mpr_objDirtyExts;  mpr_objDirtyExts.clear();
	domrDirtyExts  = mpr_domDirtyExts;  mpr_domDirtyExts.clear();
	mtobjDirtyExts = mpr_mtobjDirtyExts;mpr_mtobjDirtyExts.clear();
	demDirtyExts   = mpr_demDirtyExts;  mpr_demDirtyExts.clear();


}

void CGlbGlobeDispatcher::AddDomDirtyExtent(CGlbExtent &ext,glbBool isDomLayerDirty)
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	if (isDomLayerDirty)
	{
		mpr_tempDomDirtyExts.push_back(ext);
	}
	else
		mpr_tempObjDirtyExts.push_back(ext);
}

void CGlbGlobeDispatcher::AddDemDirtyExtent(CGlbExtent &ext,glbBool  isDemLayerDirty)
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	if (isDemLayerDirty)
	{
		mpr_tempDemDirtyExts.push_back(ext);
	}
	else
		mpr_tempMtobjDirtyExts.push_back(ext);	
}

void CGlbGlobeDispatcher::UpdateState()
{
	if(!mpr_globe) return;
	CGlbGlobeView* p_view = mpr_globe->GetView();
	if(p_view==NULL)return;	

	osg::Timer_t thisUpdateTime = osg::Timer::instance()->tick();
	double frameTime = osg::Timer::instance()->delta_m(mpr_preUpateTime,thisUpdateTime);
	if(frameTime <200.0)return;

	osg::Vec3d eye;
	p_view->GetCameraPos(eye);
	glbDouble pitch = p_view->GetPitch() - 90.0;
	glbDouble yaw   = p_view->GetYaw();

	osg::Vec3d w_eyePos = mpr_eyePosition;	
	osg::Vec3d w_newEyePos=eye;
	if (mpr_globe->GetType()==GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(w_eyePos.y()),osg::DegreesToRadians(w_eyePos.x()),w_eyePos.z(),
			w_eyePos.x(),w_eyePos.y(),w_eyePos.z()	);	
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(w_newEyePos.y()),osg::DegreesToRadians(w_newEyePos.x()),w_newEyePos.z(),
			w_newEyePos.x(),w_newEyePos.y(),w_newEyePos.z());	
	}
	//������λ�û���̬�仯������ֵ
	//Ӧ����ʱ����жϣ����磬0.2�����ڣ���ʹ�ӵ�仯Ҳ���Զ��н��и���
	if ( ( abs(w_eyePos.x() - w_newEyePos.x()) > 0.01
		|| abs(w_eyePos.y() - w_newEyePos.y()) > 0.01
		|| abs(w_eyePos.z() - w_newEyePos.z()) > 0.01 
		|| abs(mpr_pitch - pitch) > 0.1 
		|| abs(mpr_yaw - yaw)     > 0.1) ) 	
	{		
		// �Զ����βٿ���ʱ���������λ�û������仯��Ӧ����һ��ʱ��Ƭ��ֵ���ܱ�֤����ƶ��������ж������

		mpr_needTerminateUpdate = true;
	}

	mpr_preUpateTime = osg::Timer::instance()->tick();
	//else if ( objs.size() != mpr_numOfObjs) //objs��Ŀ�б仯
	//	mpr_needTerminateUpdate = true;
	//}
}

void CGlbGlobeDispatcher::DestoryObjects()
{
	std::vector<glbref_ptr<CGlbGlobeRObject>>	tempTerrainObjs;	//��ʱ�����λ��ƶ���
	std::vector<glbref_ptr<CGlbGlobeRObject>>	tempRTerrainObjs;	//��ʱ��Ե��λ��ƶ���
	std::vector<glbref_ptr<CGlbGlobeRObject>>	tempMTerrainObjs;	//��ʱ�����޸Ķ���

	std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::iterator mit;
	std::vector<glbref_ptr<CGlbGlobeObject>>::iterator oit;
	CGlbGlobeRObject* obj;
	////1. �Ƴ���ɾ������
	mit = mpr_objlist.begin();
	while (mit != mpr_objlist.end())
	{
		std::vector<glbref_ptr<CGlbGlobeObject>> objvector;
		bool hasDestoryObj = false;
		oit = mit->second.begin();
		while (oit != mit->second.end())
		{	
			obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
			if(obj->IsDestroy())
			{
				obj->RemoveFromScene(true);
				hasDestoryObj = true;
			}else{
				objvector.push_back(obj);
			}
			oit++;
		}
		if(hasDestoryObj)
			mit->second = objvector;
		mit++;
	}	
	////2. �鲢���ض����.
	mit = mpr_objlist.begin();
	while (mit != mpr_objlist.end())
	{
		oit = mit->second.begin();
		while (oit != mit->second.end())
		{	
			obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());			
			{// ������ ����ɼ�������ص���������
				glbDouble dis = obj->GetDistance(mpr_eyePosition,false);
				glbDouble minDis = obj->GetMinVisibleDistance();
				glbDouble maxDis = obj->GetMaxVisibleDistance();
				//if ((dis-minDis)*(dis-maxDis)<=0)	
				if ((dis-minDis)*(dis-maxDis)<=0 && obj->IsInViewPort())	
					obj->SetShow(true,true);				
				else
					obj->SetShow(false,true);
			}
			if(obj->IsInViewPort() == false &&  obj->GetAltitudeMode() != GLB_ALTITUDEMODE_ONTERRAIN)		
			{
				oit++;
				continue;
			}
			if(obj->GetType() == GLB_OBJECTTYPE_COMPLEX)
			{//���϶���: ��Ҫȡ���Ӷ������ȴ���
				glbInt32 cnt = obj->GetCount();
				for(glbInt32 i=0;i<cnt;i++)
				{
					CGlbGlobeRObject* childObj = obj->GetRObject(i);
					if(childObj->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN
					   /*&& childObj->GetOsgNode() != NULL*/)
					{//�������Ӷ���.
						tempTerrainObjs.push_back(childObj);
					}else if (childObj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
					{//��Ե����Ӷ���.
						tempRTerrainObjs.push_back(childObj);
					}
					else if (childObj->GetType()==GLB_OBJECTTYPE_MODIFYTERRIN)
					{//�����޸��Ӷ���.
						tempMTerrainObjs.push_back(childObj);
					} 
				}
			}
			else if(   obj->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN
				      /* && obj->GetOsgNode() != NULL*/)
			{//�����λ��ƶ���
				if (obj->IsShow())
					tempTerrainObjs.push_back(obj);
			}
			else if (obj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
			{//��Ե��ζ���.
				if (obj->IsShow())
					tempRTerrainObjs.push_back(obj);
			}
			else if (obj->GetType()==GLB_OBJECTTYPE_MODIFYTERRIN)
			{//�����޸Ķ���.
				if (obj->IsShow())
					tempMTerrainObjs.push_back(obj);
			} 
			else if (obj->GetAltitudeMode() == GLB_ALTITUDEMODE_ABSOLUTE)
			{// ���Ը߶Ȼ��Ƶ�ͼƬ����Ҳ��Ҫ����UpdateElevate()�����ڸ����������Ϊ�����ñ���������������������Ǳ仯�� 2016.6.7 malin
				if (obj->GetType()==GLB_OBJECTTYPE_POINT  && obj->IsShow())
				{					
					glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo*>(obj->GetRenderInfo());
					if (markerInfo && markerInfo->symType == GLB_MARKERSYMBOL_IMAGE)
						tempRTerrainObjs.push_back(obj);					
				}
			}
			oit++;
		}
		mit++;
	}

	// ����
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	mpr_terrainObjs  = tempTerrainObjs;
	mpr_rTerrainObjs = tempRTerrainObjs;
	mpr_mTerrainObjs = tempMTerrainObjs;
	///׷��
	std::vector<CGlbExtent>::iterator itr = mpr_tempObjDirtyExts.begin();
	for(itr;itr!=mpr_tempObjDirtyExts.end();itr++)
		mpr_objDirtyExts.push_back((*itr));
	mpr_tempObjDirtyExts.clear();

	itr = mpr_tempDomDirtyExts.begin();
	for(itr;itr!=mpr_tempDomDirtyExts.end();itr++)
		mpr_domDirtyExts.push_back((*itr));
	mpr_tempDomDirtyExts.clear();

	itr = mpr_tempMtobjDirtyExts.begin();
	for(itr;itr!=mpr_tempMtobjDirtyExts.end();itr++)
		mpr_mtobjDirtyExts.push_back((*itr));
	mpr_tempMtobjDirtyExts.clear();

	itr = mpr_tempDemDirtyExts.begin();
	for(itr;itr!=mpr_tempDemDirtyExts.end();itr++)
		mpr_demDirtyExts.push_back((*itr));
	mpr_tempDemDirtyExts.clear();
}

void CGlbGlobeDispatcher::Update(std::vector<glbref_ptr<CGlbGlobeObject>> &objs)
{	
	if(!mpr_globe) return;	
	CGlbGlobeView* p_view = mpr_globe->GetView();
	if(p_view==NULL)return;

	mpr_needTerminateUpdate = false;

	std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::iterator mit;
	std::vector<glbref_ptr<CGlbGlobeObject>> objVector;
	std::vector<glbref_ptr<CGlbGlobeObject>>::iterator oit;
	CGlbGlobeRObject* obj;
	osg::Vec3d eye;

	p_view->GetCameraPos(eye);
	glbDouble pitch = p_view->GetPitch() - 90.0;
	glbDouble yaw   = p_view->GetYaw();

	osg::Vec3d w_eyePos = mpr_eyePosition;	
	osg::Vec3d w_newEyePos=eye;
	if (mpr_globe->GetType()==GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(w_eyePos.y()),osg::DegreesToRadians(w_eyePos.x()),w_eyePos.z(),
			                                     w_eyePos.x(),w_eyePos.y(),w_eyePos.z()	);	
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(w_newEyePos.y()),osg::DegreesToRadians(w_newEyePos.x()),w_newEyePos.z(),
			                                     w_newEyePos.x(),w_newEyePos.y(),w_newEyePos.z());	
	}
	//������λ�û���̬�仯������ֵ
	//Ӧ����ʱ����жϣ����磬0.2�����ڣ���ʹ�ӵ�仯Ҳ���Զ��н��и���
	if ( ( abs(w_eyePos.x() - w_newEyePos.x()) > 0.01
		|| abs(w_eyePos.y() - w_newEyePos.y()) > 0.01
		|| abs(w_eyePos.z() - w_newEyePos.z()) > 0.01 
		|| abs(mpr_pitch - pitch) > 0.1 
		|| abs(mpr_yaw - yaw)     > 0.1) ) 	
	{
		mpr_eyePosition    = eye;   //�ȸ���mpr_eyePostion�����������õ��۾�λ��
		mpr_pitch          = pitch;
		mpr_yaw            = yaw;
		mpr_numOfObjs      = objs.size();	
		mpr_lastLoadObj    = NULL;		
		////1. ֪ͨcallback��չҶ��С�CleanQueue��
		if(mpr_globe->mpr_p_callback)
		mpr_globe->mpr_p_callback->CleanQueue();		
		////2. ֪ͨ װ���������������ն���װ�ض��С�CleanObjTasks��
		if(mpr_globe->mpr_taskmanager)
		mpr_globe->mpr_taskmanager->CleanObjectTasks();
		////3.1 �Ƴ��ɶ�����û�м��ع��Ķ���.
		mit = mpr_objlist.begin();
		while (mit != mpr_objlist.end())
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return;
			}
			std::vector<glbref_ptr<CGlbGlobeObject>> objvector;
			bool hasNeverLoadObj = false;
			oit = mit->second.begin();			
			while (oit != mit->second.end())
			{				
				if(mpr_needTerminateUpdate)
				{//�ӵ�仯 ������ֵ
					return;
				}
				obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
				if (obj->GetNeverLoadState() == false)
					objvector.push_back(obj);
				else
					hasNeverLoadObj = true;
				oit++;
			}
			if(hasNeverLoadObj)mit->second = objvector;
			mit++;
		}
		////3. ���¼���ɶ���ľ���.
		mit = mpr_objlist.begin();
		while (mit != mpr_objlist.end())
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return;
			}
			oit = mit->second.begin();			
			while (oit != mit->second.end())
			{				
				if(mpr_needTerminateUpdate)
				{//�ӵ�仯 ������ֵ
					return;
				}
				obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
				obj->SetInViewPort(false);
				obj->GetDistance(mpr_eyePosition,true);
				oit++;
			}
			mit++;
		}		
		////4. �����¶���ľ���
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>> cmpObjMaps; //���϶���.
		oit = objs.begin();
		for (; oit != objs.end(); )
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return;
			}
			obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
			if(obj->GetParentObject()!=NULL)
			{
				/*
					���϶�����Ӷ��󣬲�������ȣ����϶���Ų������
				*/
				CGlbGlobeRObject* cmpObj = dynamic_cast<CGlbGlobeRObject*>(obj->GetParentObject());				
				cmpObjMaps[cmpObj->GetId()] = cmpObj;
				objs.erase(oit);
				oit = objs.begin();
				continue;
			}			
			obj->SetInViewPort(true);
			obj->GetDistance(mpr_eyePosition,true);//��ҪԤ�����¼���һ�飬����ɲ�����
			oit ++;
		}
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator cmpItr    = cmpObjMaps.begin();
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator cmpItrEnd = cmpObjMaps.end();
		for(cmpItr;cmpItr!=cmpItrEnd;cmpItr++)
		{//�Ѹ��϶��󣬼��뵽objs
			obj = dynamic_cast<CGlbGlobeRObject*> (cmpItr->second.get());
			obj->SetInViewPort(true);
			obj->GetDistance(mpr_eyePosition,true);//��ҪԤ�����¼���һ�飬����ɲ�����
			objs.push_back(obj);
		}
		////5.�ϲ��¶��󵽾ɶ���
		combineTwoList(mpr_objlist,objs);
		////6. �Ѷ���װ��˳�򣬵���������ɽ���Զ����
		mit = mpr_objlist.begin();
		while (mit != mpr_objlist.end())
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return;
			}
			bigHeapSort(mit->second);
			mit++;
		}	
		////7.�������һ��ж�صĿɼ�����
		if(mpr_lastRemoveObj)
		{
			if(mpr_lastRemoveObj->IsInViewPort() == false)
				mpr_lastRemoveObj = NULL;
			else{
				osg::Timer_t _currTime = osg::Timer::instance()->tick();
				double frameTime = osg::Timer::instance()->delta_m(mpr_lastRemoveTime,_currTime);
				if( frameTime>2000)
					mpr_lastRemoveObj = NULL;
			}
		}////7.�������һ��ж�صĿɼ�����
	}
	else if ( objs.size() != mpr_numOfObjs) //objs��Ŀ�б仯
	{
		mpr_eyePosition    = eye;   //�ȸ���mpr_eyePostion�����������õ��۾�λ��		
		mpr_pitch          = pitch;
		mpr_yaw            = yaw;
		mpr_numOfObjs      = objs.size();	
		mpr_lastLoadObj    = NULL;		
				
		////1.�����¶������.
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>> cmpObjMaps; //���϶���.
		oit = objs.begin();
		for ( ; oit != objs.end(); )
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return;
			}
			obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
			if(NULL == obj)continue;
			if(obj->GetParentObject()!=NULL)
			{
				/*
					���϶�����Ӷ��󣬲�������ȣ����϶���Ų������
				*/
				CGlbGlobeRObject* cmpObj = dynamic_cast<CGlbGlobeRObject*>(obj->GetParentObject());				
				cmpObjMaps[cmpObj->GetId()] = cmpObj;
				oit = objs.erase(oit);
				oit = objs.begin();
				continue;
			}
			obj->SetInViewPort(true);
			obj->GetDistance(mpr_eyePosition,true);//��ҪԤ�����¼���һ�飬����ɲ�����
			oit++;
		}
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator cmpItr    = cmpObjMaps.begin();
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator cmpItrEnd = cmpObjMaps.end();
		for(cmpItr;cmpItr!=cmpItrEnd;cmpItr++)
		{//�Ѹ��϶��󣬼��뵽objs
			obj = dynamic_cast<CGlbGlobeRObject*> (cmpItr->second.get());
			obj->SetInViewPort(true);
			obj->GetDistance(mpr_eyePosition,true);//��ҪԤ�����¼���һ�飬����ɲ�����
			objs.push_back(obj);
		}		
		////2.�ϲ��¶��󵽾ɶ���		
		combineTwoList(mpr_objlist,objs);				
		////3.mpr_objlist����
		mit = mpr_objlist.begin();
		while (mit != mpr_objlist.end())
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return;
			}
			bigHeapSort(mit->second);
			mit++;
		}			
	}		
	LoadData();		
	AddToScene();	
	DestoryObjects();
	return;
}

glbInt64 CGlbGlobeDispatcher::UnLoadObject(CGlbGlobeRObject* obj)
{
	CGlbGlobeRObject* removeObj=NULL;
	glbInt64 unloadSize = 0;
	//��ʼж�أ�����ж�ز��ɼ����塣
	std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::reverse_iterator rmit;
	rmit = mpr_objlist.rbegin();//������ʣ��ȷ���ORDER������ȼ���͵�
	while (rmit != mpr_objlist.rend())
	{//������ʣ��ȷ��ʾ���������ȼ����
		if(mpr_needTerminateUpdate)
		{//�ӵ�仯 ������ֵ
			return 0;
		}
		std::vector<glbref_ptr<CGlbGlobeObject>>::reverse_iterator roit = rmit->second.rbegin();
		while (roit != rmit->second.rend())
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return 0;
			}
			removeObj = dynamic_cast<CGlbGlobeRObject*> ((*roit).get());
			if (  removeObj->IsInViewPort() == false
				&& (removeObj->IsAllowUnLoad() == true) /*����ж��*/
				&& obj != removeObj
				&&
				( 
				  (  removeObj->GetOsgNode()   != NULL
				     &&removeObj->GetAltitudeMode()!= GLB_ALTITUDEMODE_ONTERRAIN)
				  )//�Ǹ��϶���
				  ||
				  (
					removeObj->GetType() == GLB_OBJECTTYPE_COMPLEX
				  )//���϶��󣬸��϶���osgNode ��Զ��NULL.
				
				)

			{//ж�ز��ɼ����壨�ҷ������λ��ƶ���
				unloadSize += removeObj->RemoveFromScene(true);//���ڴ���Դ�ж�ؿɼ��Ͳ��ɼ�����								
				if (unloadSize >GLB_MAX_UNLOADSIZE)
					break;
			}			
			roit++;
		}
		if (unloadSize >GLB_MAX_UNLOADSIZE)
			break;
		rmit++;
	}
	if(    unloadSize  == 0/* 
		&& unloadSize < GLB_MAX_UNLOADSIZE*/)
	{//���ɼ����Ѿ��ճ�һ�����ڴ�
		//ж�ؿɼ�����
		glbInt64 objsize = 0;
		glbInt64 hasUsedMemory = CGlbGlobeMemCtrl::GetInstance()->GetUsedMem();
		std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::reverse_iterator rmit;
		rmit = mpr_objlist.rbegin();//������ʣ��ȷ���ORDER������ȼ���͵�
		while (rmit != mpr_objlist.rend())
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return 0;
			}
			if(rmit->first < obj->GetLoadOrder())
			{
				rmit++;
				continue;
			}
			std::vector<glbref_ptr<CGlbGlobeObject>>::reverse_iterator roit = rmit->second.rbegin();//������ʣ��ȷ��ʾ���������ȼ����
			while (roit != rmit->second.rend())
			{
				if(mpr_needTerminateUpdate)
				{//�ӵ�仯 ������ֵ
					return 0;
				}
				/*
					*    �����ϴ�װ�� ���һ������A 3M����399M �ﵽ402��
					*                       5 5M,6 1M,7 1M,A 3M
					*    ��� �ӵ������ٱ仯������5,6,7,A���ڿɼ�����
					*    ��ʱ��װ�ص�һ������N������װ�صĶ����Ƿ�ɹ�����ж���ڴ棺
					*          �����5,6,7,A���ᱻж��
					*    ����װ��,5,6,7,A��������װ��,������.
					*
					*    ���ԣ��ɼ������ж��ֻҪ ж��A��ʹ�ڴ�С����ֵ����
					*          ������������ǻ��ж��A����װ����A��������.
					*    ���ԣ���Ҫ��¼���һ��ж�صĶ��󣬵��ٴ�װ�ص�Aʱ�Ͳ���װ��A
				*/	
				/*
					*    �����Ƴ��Ķ���A���п������ڼ��أ���ʱ��ж�ر�A���Ŀɼ�����
					*    �����A���Ķ�����1��2��3����ʱ��1,2,3ж����,װ���˱�1���Ķ���M
					*    ��� ��װ��1ʱ������A�Ѿ�׼���ã���ʱ��ж��A���Ϳ���װ��1,2,3��
					*
					*    ���������ӵ�û�б仯�������,1,2,3��ж�أ��ָ���������.
					*    ���ԣ�
					*         ���RemoveObject����װ�����ݣ������ж��.
				*/
				removeObj = dynamic_cast<CGlbGlobeRObject*> ((*roit).get());
				if (   removeObj->IsInViewPort() == true
					&& obj != removeObj
					&& (removeObj->IsAllowUnLoad() == true) /*����ж��*/
					&& 
					(
						( removeObj->GetOsgNode()   != NULL						  
						  && removeObj->GetAltitudeMode() != GLB_ALTITUDEMODE_ONTERRAIN)
						)//�Ǹ��϶���
						||
						(
						   removeObj->GetType() == GLB_OBJECTTYPE_COMPLEX					
						)//���϶��󣬸��϶���osgNode ��Զ��NULL.					
					)
				{//ж�ؿɼ�����(�ҷ������λ��ƶ���)
					if ((removeObj->GetLoadOrder() > obj->GetLoadOrder())
						|| 
						(removeObj->GetLoadOrder() == obj->GetLoadOrder()
						&& removeObj->GetDistance(mpr_eyePosition,false) > obj->GetDistance(mpr_eyePosition,false)))						
					{//removeObj�ľ����Զ�����ȼ��ϲ�						
						if(removeObj->IsLoadData())
						{
							return unloadSize;
						}
						objsize    =  removeObj->RemoveFromScene(true);
						unloadSize += objsize;						
						if((hasUsedMemory - unloadSize) < g_memThreshold)
						{
							//��¼���ж�صĿɼ�����.
							mpr_lastRemoveObj  = removeObj;
							mpr_lastRemoveTime = osg::Timer::instance()->tick();
							break;
						}
					}
				}				
				roit++;
			}
			//if (unloadSize >GLB_MAX_UNLOADSIZE)
			if((hasUsedMemory - unloadSize) < g_memThreshold)
				break;
			rmit++;
		}	
	}
	return unloadSize;
}

void CGlbGlobeDispatcher::LoadData()
{
	std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::iterator mit;
	std::vector<glbref_ptr<CGlbGlobeObject>>::iterator oit;
	CGlbGlobeRObject* obj;		
	glbInt64  hasUsedMemory = 0;	
	glbDouble distance=0;
	//////////////////////////////
	mit = mpr_objlist.begin();
	while (mit != mpr_objlist.end())
	{
		if(mpr_needTerminateUpdate)
		{//�ӵ�仯 ������ֵ
			return;
		}
		if(  mpr_lastLoadObj
		   &&mit->first < mpr_lastLoadObj->GetLoadOrder() )
		{//��lastLoadObj���ȼ�С�Ķ����Ѿ�Load������Continue;
			mit++;
			continue;
		}
		oit = mit->second.begin();
		while (oit != mit->second.end())
		{			
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return;
			}
			if(mpr_globe->mpr_taskmanager->GetObjectTaskNum() >= GLB_MAX_OBJTASKNUM)
			{
				/*
				*    ��ʼʱ: hasUsedMemory = 0 ����������ƣ�����������м����������
				*                                �п��ܳ����ڴ��޶�.
				*/
				return;
			}
			obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
			if(obj->IsDestroy())
			{	
				oit++;
				continue;
			}
			distance = obj->GetDistance(mpr_eyePosition,false);	
			if(mpr_lastRemoveObj
				&&
				(
				   obj == mpr_lastRemoveObj.get()
				   ||
				   distance >= mpr_lastRemoveObj->GetDistance(mpr_eyePosition,false)
				)
			  )
			{//���ж�صĶ��� �Լ��������Զ�ģ���װ��.
				oit++;
				continue;
			}	
			if(    mpr_lastLoadObj
				&& mit->first == mpr_lastLoadObj->GetLoadOrder()
				&& distance < mpr_lastLoadObj->GetDistance(mpr_eyePosition,false)
				&& obj->IsLoadData() == true) // �� <=  ��Ϊ  < ���о�����ͬ������ͬ�����
			{//��lastLoadObj ���ȼ���ͬ�ġ��Ҿ���<=lastLoadObj�Ķ����Ѿ�Load������continue  --- ����һ��ѽ���ϲ������Ժ�����м���û�м��ص�obj����
				oit++;
				continue;
			}						
			if (obj->IsInViewPort() == true)//��������׵����
			{//�˴���objδ�����ڴ棬�п�����mpr_taskmanager�����У�Ҳ���ܲ��ڡ�											
				if (   (distance > obj->GetMinVisibleDistance())
					&& (distance < obj->GetMaxVisibleDistance()) 
					&& obj->IsShow()/*������Ҫ��ʾ*/)
				{//�����������С��Χ��
					if(g_isMemCtrl)
					{						
						hasUsedMemory = CGlbGlobeMemCtrl::GetInstance()->GetUsedMem();
						if(hasUsedMemory > g_memThreshold)
						{
							glbInt64 unloadSize = UnLoadObject(obj);						
							/*
							    return���ٴ��ɽ�---->Զ���ض���.
							*/
							return;
						}
					}
					{//����װ��
						glbInt32 level = obj->ComputeRefLevel(distance);
						/*if (distance <= 100) level =3;
						else if (distance<=1000) level =2;
						else if (distance<=5000) level =1;*/
						glbInt32 curlevel = obj->GetCurrLevel();
						if (curlevel != level)
						{
							obj->LoadData(level);	
							mpr_lastLoadObj = obj;							
						}											
					}//����װ��.
				}//���Ӿ෶Χ֮��.
			}			
			oit++;
		}
		mit++;
	}	
}

glbInt64 CGlbGlobeDispatcher::RemoveObjFromScene(CGlbGlobeRObject* obj)
{
	CGlbGlobeRObject* removeObj = NULL;
	glbInt64          removeSize= 0;
	std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::reverse_iterator rmit;
	rmit = mpr_objlist.rbegin();//������ʣ��ȷ���ORDER������ȼ���͵�
	glbInt32 addObjSize = obj->GetOsgNodeSize();

	while (rmit != mpr_objlist.rend())
	{//ж�ز��ɼ�	
		if(mpr_needTerminateUpdate)
		{//�ӵ�仯 ������ֵ
			return 0;
		}
		std::vector<glbref_ptr<CGlbGlobeObject>>::reverse_iterator roit = rmit->second.rbegin();//������ʣ��ȷ��ʾ���������ȼ����
		while (roit != rmit->second.rend())
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return 0;
			}
			removeObj                        = dynamic_cast<CGlbGlobeRObject*> ((*roit).get());
			osg::ref_ptr<osg::Node> rosgNode = removeObj->GetOsgNode();
			if (  removeObj->IsInViewPort() == false
				  && obj != removeObj
				  && (removeObj->IsAllowUnLoad() == true) /*����ж��*/
				  &&
				  (
				     (    rosgNode != NULL
					   && removeObj->GetAltitudeMode() != GLB_ALTITUDEMODE_ONTERRAIN
					   && rosgNode->getNumParents()!=0
					 )//�Ǹ��϶���
					 ||
					 (
					   removeObj->GetType() == GLB_OBJECTTYPE_COMPLEX
					 )//���϶���osgNode��Զ��NULL
				  )
				 
				)  
			{//ж�ز��ɼ�����
				removeSize += removeObj->RemoveFromScene(false);									
			}			
			if(removeSize > GLB_MAX_UNLOADSIZE)break;
			roit++;
		}
		if(removeSize > GLB_MAX_UNLOADSIZE)break;
		rmit++;
	}
	if (removeSize == 0)
	{//ж�ؿɼ�		
		glbInt64 hasUsedDMem = mpr_globe->mpr_p_callback->GetUsedDislayMemory();
		std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::reverse_iterator rmit;
		rmit = mpr_objlist.rbegin();//������ʣ��ȷ���ORDER������ȼ���͵�
		while (rmit != mpr_objlist.rend())
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return 0;
			}
			std::vector<glbref_ptr<CGlbGlobeObject>>::reverse_iterator roit = rmit->second.rbegin();//������ʣ��ȷ��ʾ���������ȼ����
			while (roit != rmit->second.rend())
			{
				if(mpr_needTerminateUpdate)
				{//�ӵ�仯 ������ֵ
					return 0;
				}
				removeObj                        = dynamic_cast<CGlbGlobeRObject*> ((*roit).get());
				osg::ref_ptr<osg::Node> rosgNode = removeObj->GetOsgNode();
				if (removeObj->IsInViewPort() == true
					&& (removeObj->IsAllowUnLoad() == true) /*����ж��*/
					&& 
					(
					   (
					      removeObj->GetAltitudeMode() != GLB_ALTITUDEMODE_ONTERRAIN
						  && rosgNode != NULL
						  && rosgNode->getNumParents()>0
					   )//�Ǹ��϶���
					   ||
					   (
					     removeObj->GetType() == GLB_OBJECTTYPE_COMPLEX
					   )//���϶���,osgNode��Զ��NULL
					)					
				   )
				{//ж�ؿɼ�����
					if(removeObj == obj)
					{//��Զ --->�� ж�ص�obj����Ӧ�ý���
						return removeSize;
					}
					//removeobj�����ȼ��ͣ�����Զ��Ӧ��ж��
					if ((removeObj->GetLoadOrder() > obj->GetLoadOrder())
						|| 
						(removeObj->GetLoadOrder() == obj->GetLoadOrder()
						&& removeObj->GetDistance(mpr_eyePosition,false) >obj->GetDistance(mpr_eyePosition,false)))
					{						
						glbInt32 removeObjSize = removeObj->GetOsgNodeSize();						
						if (hasUsedDMem+addObjSize-removeSize-removeObjSize > g_totalDMem)
						{// ֻ�е���ӵ�-�Ƴ��ĳ��Դ���ֵ����ж��
							removeSize += removeObj->RemoveFromScene(false);
							break;
						}						
						else
						{// ����Ҫж�صĻ�ֻ��������Ϊ����ж��							
							removeSize += removeObjSize;
						}
						if (hasUsedDMem+addObjSize-removeSize > g_totalDMem) break;						
					}
				}
				roit++;
			}			
			if (hasUsedDMem+addObjSize-removeSize > g_totalDMem && removeSize>0) break;
			rmit++;
		}
	}
	return removeSize;
}
void CGlbGlobeDispatcher::AddToScene()
{
	std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::iterator mit;	
	std::vector<glbref_ptr<CGlbGlobeObject>>::iterator oit;
	CGlbGlobeRObject* obj;

	glbInt32 hasUsedDMem = 0;//��ǰ��ʹ���Դ�����
	glbInt32 hangSize    = 0;//���ҽ�osg�ڵ�ߴ�.	
	glbDouble distance   =0;
	//////////////////////////////////////////////
	mit = mpr_objlist.begin();
	while (mit != mpr_objlist.end())
	{		
		if(mpr_needTerminateUpdate)
		{//�ӵ�仯 ������ֵ
			return;
		}
		oit = mit->second.begin();
		while (oit != mit->second.end())
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return;
			}
			obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
			if(obj->IsDestroy())
			{				
				oit++;
				continue;
			}
			if (obj->IsInViewPort()==false)
			{//��������ӿ��ڣ�ȡ��һ��
				oit++;
				continue;
			}
			osg::ref_ptr<osg::Node> osgNode = obj->GetOsgNode();
			if(   osgNode == NULL
			   && obj->GetType() != GLB_OBJECTTYPE_COMPLEX)
			{//���϶���osgNode��Զ��NULL�����ԣ����϶�����Ҫ����.
				oit++;
				continue;
			}
			if (   osgNode != NULL 
				&& osgNode->getNumParents() != 0
				&& obj->GetType() != GLB_OBJECTTYPE_COMPLEX)
			{//�Ѿ����Դ棬continue
			 //���϶���osgNode��Զ��NULL�����ԣ����϶�����Ҫ����.
				oit++;
				continue;
			}			
			distance = obj->GetDistance(mpr_eyePosition,false);
			if ((distance<=obj->GetMinVisibleDistance()) || (distance >= obj->GetMaxVisibleDistance()) )
			{//���벻����,continue
				oit++;
				continue;
			}
			hangSize = mpr_globe->mpr_p_callback->GetHangSize();
			if (hangSize>=FRAME_DEAL_SIZE)
			{//����osg�ڵ㳬�� ��֡��������				
				return;
			}			
			hasUsedDMem = mpr_globe->mpr_p_callback->GetUsedDislayMemory();							
			if(g_isDMemCtrl)
			{//�Դ���������				
				if (hasUsedDMem > g_totalDMem)
				{//��ʹ��+��ʹ��-��ժ��>������ֵ
					glbInt64 rmsize = RemoveObjFromScene(obj);
					if (rmsize > 0)
					{//�������������ϹҶ���
						obj->AddToScene();
					}
					return;
				}	
			}			
			{//�������������ϹҶ���
				obj->AddToScene();				
			}
			oit++;
		}
		mit++;
	}
}
/*
*   ��������
*   �ݹ���ã��п�����ɶ�ջ�����������.
*/
void CGlbGlobeDispatcher::quickSort(std::vector<glbref_ptr<CGlbGlobeObject>>* objs, glbInt32 l, glbInt32 r)
{
	if (l < r)
	{      
		glbInt32 i = l, j = r;
		glbref_ptr<CGlbGlobeObject> x = (*objs)[l];
		CGlbGlobeRObject* xObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[l].get());

		while (i < j)
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return;
			}
			CGlbGlobeRObject* jObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[j].get());

			while((i < j) 
				   && (compareTo(xObj,jObj ) == 1))//����x���ȼ�<j ��x���� <j��x���ȼ�=j��xӦ������j��ǰ�桿
			{
				if(mpr_needTerminateUpdate)
				{//�ӵ�仯 ������ֵ
					return;
				}
				j--; 
				jObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[j].get());
			}

			if(i < j)
				(*objs)[i++] = (*objs)[j];

			CGlbGlobeRObject* iObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[i].get());

			while((i < j) &&  (compareTo(iObj, xObj)==1))//����x���ȼ�<j ��x���� <j��x���ȼ�=j��xӦ������j��ǰ�桿
			{
				if(mpr_needTerminateUpdate)
				{//�ӵ�仯 ������ֵ
					return;
				}
				i++; 
				iObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[i].get());
			}
			if(i < j)

				(*objs)[j--] = (*objs)[i];

		}

		(*objs)[i] = x;
		quickSort(objs, l, i - 1); // �ݹ����
		quickSort(objs, i + 1, r);

	}

}
/*
*   isComputeDistance ȱʡ��false
*   1: A ���ȼ� < B �� A ���� <B,��A���ȼ�=B
*   -1:A ���ȼ� > B �� A ���� >B,��A���ȼ�=B
*   0 ��A���ȼ� = B �� A ���� = B
*/
glbInt8 CGlbGlobeDispatcher::compareTo(CGlbGlobeRObject *objA, CGlbGlobeRObject *objB,  glbBool isComputeDistance)
{
	if (objA->GetLoadOrder() < objB->GetLoadOrder()) return 1;
	else if (objA->GetLoadOrder() > objB->GetLoadOrder()) return -1;
	else if (objA->GetDistance(mpr_eyePosition,isComputeDistance) < objB->GetDistance(mpr_eyePosition,isComputeDistance)) return 1;
	else if (objA->GetDistance(mpr_eyePosition,isComputeDistance) > objB->GetDistance(mpr_eyePosition,isComputeDistance)) return -1;
	else return 0;
}

//void CGlbGlobeDispatcher::combineTwoList(std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>& objmap, vector<glbref_ptr<CGlbGlobeObject>> &objs)
//{
//	std::vector<glbref_ptr<CGlbGlobeObject>>::iterator oldItr,newItr;
//	CGlbGlobeRObject* newObj;
//	CGlbGlobeRObject* oldObj;
//	double newDistance;
//	double oldDistance;
//
//	newItr = objs.begin();
//	while (newItr!=objs.end())
//	{
//		if(mpr_needTerminateUpdate)
//		{//�ӵ�仯 ������ֵ
//			return;
//		}
//		newObj =  dynamic_cast<CGlbGlobeRObject*> ((*newItr).get());
//		glbInt32 newOrder = newObj->GetLoadOrder();
//		if (objmap.find(newOrder) != objmap.end())
//		{//��Ⱦ���ȼ�����
//			oldItr = objmap[newOrder].begin();
//			while (   (newItr!=objs.end()) 
//				   && (newObj->GetLoadOrder() == newOrder))  				
//			{//�ɼ���������δ�������&&���ȼ���map�������ȼ���ͬ
//				if(mpr_needTerminateUpdate)
//				{//�ӵ�仯 ������ֵ
//					return;
//				}
//				newObj =  dynamic_cast<CGlbGlobeRObject*> ((*newItr).get());
//				if (oldItr == objmap[newOrder].end())   //�ѵ�map�����ȼ�ĩβ
//				{
//					objmap[newOrder].push_back(newObj);
//					oldItr = objmap[newOrder].end();
//					newItr++;
//					if (newItr!=objs.end()) newObj =  dynamic_cast<CGlbGlobeRObject*> ((*newItr).get());
//					continue;
//				}
//
//				oldObj =  dynamic_cast<CGlbGlobeRObject*> ((*oldItr).get());
//				newDistance = newObj->GetDistance(mpr_eyePosition,false);
//				oldDistance = oldObj->GetDistance(mpr_eyePosition,false);
//				//δ��map�����ȼ�ĩβ����Ա�
//				{
//					if (newObj->GetId() == oldObj->GetId()) 
//					{						
//						newItr++;
//						oldItr++;
//					}
//					else if (newDistance < oldDistance)//�ɼ��������ȼ��ߣ�����
//					{
//						oldItr = objmap[newOrder].insert(oldItr,newObj);
//						newItr++;
//					}
//					else if (newDistance >= oldDistance)
//					{
//						oldItr++;
//					}
//				}
//				if (newItr!=objs.end())               newObj =  dynamic_cast<CGlbGlobeRObject*> ((*newItr).get());
//				if (oldItr != objmap[newOrder].end()) oldObj =  dynamic_cast<CGlbGlobeRObject*> ((*oldItr).get());
//			}
//		}
//		else//rObjInObjs�����ȼ�����map�У����������ȼ������ƿɼ��б���ͬ���ȼ��Ķ���
//		{
//			while ((newItr!=objs.end()))  //�ɼ����л��ж���&&���ȼ����������ȼ���ͬ
//			{
//				if(mpr_needTerminateUpdate)
//				{//�ӵ�仯 ������ֵ
//					return;
//				}
//				newObj =  dynamic_cast<CGlbGlobeRObject*> ((*newItr).get());
//				if (newObj->GetLoadOrder() != newOrder) break;
//				objmap[newOrder].push_back(newObj);
//				newItr++;
//			}
//		}
//
//	}
//}

void CGlbGlobeDispatcher::combineTwoList(std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>& objmap, vector<glbref_ptr<CGlbGlobeObject>> &objs)
{
	if(objs.size() == 0)return;
	std::vector<glbref_ptr<CGlbGlobeObject>>::iterator mapVItr;
	std::vector<glbref_ptr<CGlbGlobeObject>>::iterator objVItr;
	std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::iterator mit;

	std::map<CGlbGlobeObject*,glbByte> idmap;	
	mit = objmap.begin();
	for(mit;mit != objmap.end();mit++)
	{
		if(mpr_needTerminateUpdate)
		{//�ӵ�仯 ������ֵ
			return;
		}
		mapVItr = mit->second.begin();
		for(mapVItr;mapVItr != mit->second.end();mapVItr++)
		{
			if(mpr_needTerminateUpdate)
			{//�ӵ�仯 ������ֵ
				return;
			}			
			idmap[(*mapVItr).get()] = 0;
		}
	}

	objVItr = objs.begin();
	for(objVItr; objVItr != objs.end(); objVItr++)
	{
		if(mpr_needTerminateUpdate)
		{//�ӵ�仯 ������ֵ
			return;
		}		
		if(idmap.find((*objVItr).get())!=idmap.end())
			continue;
		CGlbGlobeRObject* obj =  dynamic_cast<CGlbGlobeRObject*> ((*objVItr).get());
		glbInt32 loadOrder    =  obj->GetLoadOrder();
		mit = objmap.find(loadOrder);
		if(mit != objmap.end())
		{//װ�����ȼ���Ӧ�������Ѿ�����.
			mit->second.push_back(obj);
		}
		else
		{//װ�����ȼ���Ӧ�����в�����.
			objmap[loadOrder].push_back(obj);
		}
	}
}

/*
	��-�������㷨.
*/
//void CGlbGlobeDispatcher::BigHeapAdjust(std::vector<glbref_ptr<CGlbGlobeObject>>& objs,glbInt32 r,glbInt32 len)
//{
//	glbref_ptr<CGlbGlobeRObject> tmpObj = dynamic_cast<CGlbGlobeRObject*>(objs[r].get());
//	int j;
//	int cmp;
//	for(j=2*r+1; j<=len-1; j=2*j+1) 
//	{//�ؽڵ�ֵ�ϴ�Ķ������²�ɸѡ��2*r+1������ӣ�2*(r+1)���Ҷ���	
//		CGlbGlobeRObject* obj_j = dynamic_cast<CGlbGlobeRObject*>(objs[j].get());
//		if(j<len-1)
//		{//��Ϊj<len-1ʱ,p[j+1]�Ų���Խ�� 
//			CGlbGlobeRObject* obj_j_1 = dynamic_cast<CGlbGlobeRObject*>(objs[j+1].get());
//			//cmp = compareTo(obj_j_1,obj_j);
//			//if(cmp == -1/* || cmp == 0*/)
//			if(obj_j_1->GetDistance(mpr_eyePosition,false) >= obj_j->GetDistance(mpr_eyePosition,false))
//				++j; //����Ҷ��Ӵ��������,j++ת�Ƶ��Ҷ���,��j�������Ҷ����нϴ���Ǹ� 
//		}
//		//cmp = compareTo(tmpObj,obj_j);
//		//if(cmp == -1/*||cmp == 0*/)break;	
//		if(tmpObj->GetDistance(mpr_eyePosition,false) >= obj_j->GetDistance(mpr_eyePosition,false))break;
//		objs[r] = objs[j].get(); //�ϴ�Ķ����򸸽ڵ�ƽ��,������r�ڵ��λ��
//		r = j; 
//	}
//	objs[r] = tmpObj; 
//}
void CGlbGlobeDispatcher::bigHeapAdjust(std::vector<glbref_ptr<CGlbGlobeObject>>& objs,glbInt32 r,glbInt32 len)
{
	glbInt32 nChild;
	for(;2*r+1<len;r=nChild)
	{
		//�ӽ���λ��=2*�������λ�ã�+1
		nChild=2*r+1;
		//�õ��ӽ���нϴ�Ľ��
		if(nChild<len-1/*&&array[nChild+1]>array[nChild]*/)
		{
			glbref_ptr<CGlbGlobeRObject> obj_j_1 = dynamic_cast<CGlbGlobeRObject*>(objs[nChild+1].get());
			glbref_ptr<CGlbGlobeRObject> obj_j   = dynamic_cast<CGlbGlobeRObject*>(objs[nChild].get());
			if(obj_j_1->GetDistance(mpr_eyePosition,false) > obj_j->GetDistance(mpr_eyePosition,false))
				++nChild;
		}
		glbref_ptr<CGlbGlobeRObject> obj_r       = dynamic_cast<CGlbGlobeRObject*>(objs[r].get());
		glbref_ptr<CGlbGlobeRObject> obj_child   = dynamic_cast<CGlbGlobeRObject*>(objs[nChild].get());
		if(obj_r->GetDistance(mpr_eyePosition,false) < obj_child->GetDistance(mpr_eyePosition,false)/*array[i]<array[nChild]*/)
		{//����ϴ���ӽ����ڸ������ô�ѽϴ���ӽ�������ƶ����滻���ĸ����
			/*nTemp=array[i];
��			array[i]=array[nChild];
��			array[nChild]=nTemp;*/
			objs[r]      = obj_child;
			objs[nChild] = obj_r;
		}
		else
		{//�����˳�ѭ��
			break;
		}
	}
}
void CGlbGlobeDispatcher::bigHeapSort(std::vector<glbref_ptr<CGlbGlobeObject>>& objs)
{
	glbInt32 len = objs.size();
	if(len<=1)return;
	
	for(int i=len/2 - 1;i>=0;--i)
	{//������
		if(mpr_needTerminateUpdate)
		{//�ӵ�仯 ������ֵ
			return;
		}
		bigHeapAdjust(objs,i,len);
	}
	glbref_ptr<CGlbGlobeObject> obj;
	for(int i=len-1;i>0;--i)
	{
		if(mpr_needTerminateUpdate)
		{//�ӵ�仯 ������ֵ
			return;
		}
		obj       = objs[i].get();
		objs[i]   = objs[0].get();
		objs[0]   = obj.get();
		bigHeapAdjust(objs,0,i);		
	}		
}