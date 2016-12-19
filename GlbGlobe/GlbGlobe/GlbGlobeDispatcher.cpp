// This is the main DLL file.

#include "StdAfx.h"
#include "GlbGlobeDispatcher.h"
#include "GlbGlobeRObject.h"
#include "GlbGlobeTaskManager.h"
#include "GlbGlobeCallBack.h"
#include "GlbGlobeView.h"
#include "CGlbGlobe.h"
#include "GlbGlobeMemCtrl.h"


//预留内存阈值50M
#define SURPLUSMEMORY 0

//*   定义显卡单帧处理的数据量   */
//10*1024*1024
#define FRAME_DEAL_SIZE    10485760  //单帧流量. 
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
	//如果相机位置或姿态变化超过阈值
	//应增加时间的判断，例如，0.2秒以内，即使视点变化也不对队列进行更新
	if ( ( abs(w_eyePos.x() - w_newEyePos.x()) > 0.01
		|| abs(w_eyePos.y() - w_newEyePos.y()) > 0.01
		|| abs(w_eyePos.z() - w_newEyePos.z()) > 0.01 
		|| abs(mpr_pitch - pitch) > 0.1 
		|| abs(mpr_yaw - yaw)     > 0.1) ) 	
	{		
		// 自动漫游操控器时，由于相机位置会连续变化，应该有一个时间片阀值才能保证相机移动过程中有对象加载

		mpr_needTerminateUpdate = true;
	}

	mpr_preUpateTime = osg::Timer::instance()->tick();
	//else if ( objs.size() != mpr_numOfObjs) //objs数目有变化
	//	mpr_needTerminateUpdate = true;
	//}
}

void CGlbGlobeDispatcher::DestoryObjects()
{
	std::vector<glbref_ptr<CGlbGlobeRObject>>	tempTerrainObjs;	//临时贴地形绘制对象
	std::vector<glbref_ptr<CGlbGlobeRObject>>	tempRTerrainObjs;	//临时相对地形绘制对象
	std::vector<glbref_ptr<CGlbGlobeRObject>>	tempMTerrainObjs;	//临时地形修改对象

	std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::iterator mit;
	std::vector<glbref_ptr<CGlbGlobeObject>>::iterator oit;
	CGlbGlobeRObject* obj;
	////1. 移除已删除对象
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
	////2. 归并贴地对象等.
	mit = mpr_objlist.begin();
	while (mit != mpr_objlist.end())
	{
		oit = mit->second.begin();
		while (oit != mit->second.end())
		{	
			obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());			
			{// 调度器 对象可见距离相关的显隐控制
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
			{//复合对象: 需要取出子对象【首先处理】
				glbInt32 cnt = obj->GetCount();
				for(glbInt32 i=0;i<cnt;i++)
				{
					CGlbGlobeRObject* childObj = obj->GetRObject(i);
					if(childObj->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN
					   /*&& childObj->GetOsgNode() != NULL*/)
					{//贴地形子对象.
						tempTerrainObjs.push_back(childObj);
					}else if (childObj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
					{//相对地形子对象.
						tempRTerrainObjs.push_back(childObj);
					}
					else if (childObj->GetType()==GLB_OBJECTTYPE_MODIFYTERRIN)
					{//地形修改子对象.
						tempMTerrainObjs.push_back(childObj);
					} 
				}
			}
			else if(   obj->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN
				      /* && obj->GetOsgNode() != NULL*/)
			{//贴地形绘制对象
				if (obj->IsShow())
					tempTerrainObjs.push_back(obj);
			}
			else if (obj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
			{//相对地形对象.
				if (obj->IsShow())
					tempRTerrainObjs.push_back(obj);
			}
			else if (obj->GetType()==GLB_OBJECTTYPE_MODIFYTERRIN)
			{//地形修改对象.
				if (obj->IsShow())
					tempMTerrainObjs.push_back(obj);
			} 
			else if (obj->GetAltitudeMode() == GLB_ALTITUDEMODE_ABSOLUTE)
			{// 绝对高度绘制的图片对象也需要调用UpdateElevate()，用于更新外包，因为当设置比例和限制增长后其外包是变化的 2016.6.7 malin
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

	// 交换
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	mpr_terrainObjs  = tempTerrainObjs;
	mpr_rTerrainObjs = tempRTerrainObjs;
	mpr_mTerrainObjs = tempMTerrainObjs;
	///追加
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
	//如果相机位置或姿态变化超过阈值
	//应增加时间的判断，例如，0.2秒以内，即使视点变化也不对队列进行更新
	if ( ( abs(w_eyePos.x() - w_newEyePos.x()) > 0.01
		|| abs(w_eyePos.y() - w_newEyePos.y()) > 0.01
		|| abs(w_eyePos.z() - w_newEyePos.z()) > 0.01 
		|| abs(mpr_pitch - pitch) > 0.1 
		|| abs(mpr_yaw - yaw)     > 0.1) ) 	
	{
		mpr_eyePosition    = eye;   //先更新mpr_eyePostion，后面排序用到眼睛位置
		mpr_pitch          = pitch;
		mpr_yaw            = yaw;
		mpr_numOfObjs      = objs.size();	
		mpr_lastLoadObj    = NULL;		
		////1. 通知callback清空挂队列【CleanQueue】
		if(mpr_globe->mpr_p_callback)
		mpr_globe->mpr_p_callback->CleanQueue();		
		////2. 通知 装载任务管理器，清空对象装载队列【CleanObjTasks】
		if(mpr_globe->mpr_taskmanager)
		mpr_globe->mpr_taskmanager->CleanObjectTasks();
		////3.1 移除旧对象中没有加载过的对象.
		mit = mpr_objlist.begin();
		while (mit != mpr_objlist.end())
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return;
			}
			std::vector<glbref_ptr<CGlbGlobeObject>> objvector;
			bool hasNeverLoadObj = false;
			oit = mit->second.begin();			
			while (oit != mit->second.end())
			{				
				if(mpr_needTerminateUpdate)
				{//视点变化 超过阈值
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
		////3. 重新计算旧对象的距离.
		mit = mpr_objlist.begin();
		while (mit != mpr_objlist.end())
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return;
			}
			oit = mit->second.begin();			
			while (oit != mit->second.end())
			{				
				if(mpr_needTerminateUpdate)
				{//视点变化 超过阈值
					return;
				}
				obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
				obj->SetInViewPort(false);
				obj->GetDistance(mpr_eyePosition,true);
				oit++;
			}
			mit++;
		}		
		////4. 计算新对象的距离
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>> cmpObjMaps; //复合对象.
		oit = objs.begin();
		for (; oit != objs.end(); )
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return;
			}
			obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
			if(obj->GetParentObject()!=NULL)
			{
				/*
					复合对象的子对象，不参与调度；复合对象才参与调度
				*/
				CGlbGlobeRObject* cmpObj = dynamic_cast<CGlbGlobeRObject*>(obj->GetParentObject());				
				cmpObjMaps[cmpObj->GetId()] = cmpObj;
				objs.erase(oit);
				oit = objs.begin();
				continue;
			}			
			obj->SetInViewPort(true);
			obj->GetDistance(mpr_eyePosition,true);//需要预先重新计算一遍，后面可不计算
			oit ++;
		}
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator cmpItr    = cmpObjMaps.begin();
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator cmpItrEnd = cmpObjMaps.end();
		for(cmpItr;cmpItr!=cmpItrEnd;cmpItr++)
		{//把复合对象，加入到objs
			obj = dynamic_cast<CGlbGlobeRObject*> (cmpItr->second.get());
			obj->SetInViewPort(true);
			obj->GetDistance(mpr_eyePosition,true);//需要预先重新计算一遍，后面可不计算
			objs.push_back(obj);
		}
		////5.合并新对象到旧对象
		combineTwoList(mpr_objlist,objs);
		////6. 把对象按装载顺序，到相机距离由近到远排序
		mit = mpr_objlist.begin();
		while (mit != mpr_objlist.end())
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return;
			}
			bigHeapSort(mit->second);
			mit++;
		}	
		////7.处理最后一次卸载的可见对象
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
		}////7.处理最后一次卸载的可见对象
	}
	else if ( objs.size() != mpr_numOfObjs) //objs数目有变化
	{
		mpr_eyePosition    = eye;   //先更新mpr_eyePostion，后面排序用到眼睛位置		
		mpr_pitch          = pitch;
		mpr_yaw            = yaw;
		mpr_numOfObjs      = objs.size();	
		mpr_lastLoadObj    = NULL;		
				
		////1.计算新对象距离.
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>> cmpObjMaps; //复合对象.
		oit = objs.begin();
		for ( ; oit != objs.end(); )
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return;
			}
			obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
			if(NULL == obj)continue;
			if(obj->GetParentObject()!=NULL)
			{
				/*
					复合对象的子对象，不参与调度；复合对象才参与调度
				*/
				CGlbGlobeRObject* cmpObj = dynamic_cast<CGlbGlobeRObject*>(obj->GetParentObject());				
				cmpObjMaps[cmpObj->GetId()] = cmpObj;
				oit = objs.erase(oit);
				oit = objs.begin();
				continue;
			}
			obj->SetInViewPort(true);
			obj->GetDistance(mpr_eyePosition,true);//需要预先重新计算一遍，后面可不计算
			oit++;
		}
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator cmpItr    = cmpObjMaps.begin();
		std::map<long/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator cmpItrEnd = cmpObjMaps.end();
		for(cmpItr;cmpItr!=cmpItrEnd;cmpItr++)
		{//把复合对象，加入到objs
			obj = dynamic_cast<CGlbGlobeRObject*> (cmpItr->second.get());
			obj->SetInViewPort(true);
			obj->GetDistance(mpr_eyePosition,true);//需要预先重新计算一遍，后面可不计算
			objs.push_back(obj);
		}		
		////2.合并新对象到旧对象		
		combineTwoList(mpr_objlist,objs);				
		////3.mpr_objlist排序
		mit = mpr_objlist.begin();
		while (mit != mpr_objlist.end())
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
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
	//开始卸载，首先卸载不可见物体。
	std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::reverse_iterator rmit;
	rmit = mpr_objlist.rbegin();//反向访问，先访问ORDER最大，优先级最低的
	while (rmit != mpr_objlist.rend())
	{//反向访问，先访问距离最大，优先级最低
		if(mpr_needTerminateUpdate)
		{//视点变化 超过阈值
			return 0;
		}
		std::vector<glbref_ptr<CGlbGlobeObject>>::reverse_iterator roit = rmit->second.rbegin();
		while (roit != rmit->second.rend())
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return 0;
			}
			removeObj = dynamic_cast<CGlbGlobeRObject*> ((*roit).get());
			if (  removeObj->IsInViewPort() == false
				&& (removeObj->IsAllowUnLoad() == true) /*允许卸载*/
				&& obj != removeObj
				&&
				( 
				  (  removeObj->GetOsgNode()   != NULL
				     &&removeObj->GetAltitudeMode()!= GLB_ALTITUDEMODE_ONTERRAIN)
				  )//非复合对象
				  ||
				  (
					removeObj->GetType() == GLB_OBJECTTYPE_COMPLEX
				  )//复合对象，复合对象osgNode 永远是NULL.
				
				)

			{//卸载不可见物体（且非贴地形绘制对象）
				unloadSize += removeObj->RemoveFromScene(true);//从内存和显存卸载可见和不可见对象。								
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
	{//不可见的已经空出一部分内存
		//卸载可见物体
		glbInt64 objsize = 0;
		glbInt64 hasUsedMemory = CGlbGlobeMemCtrl::GetInstance()->GetUsedMem();
		std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::reverse_iterator rmit;
		rmit = mpr_objlist.rbegin();//反向访问，先访问ORDER最大，优先级最低的
		while (rmit != mpr_objlist.rend())
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return 0;
			}
			if(rmit->first < obj->GetLoadOrder())
			{
				rmit++;
				continue;
			}
			std::vector<glbref_ptr<CGlbGlobeObject>>::reverse_iterator roit = rmit->second.rbegin();//反向访问，先访问距离最大，优先级最低
			while (roit != rmit->second.rend())
			{
				if(mpr_needTerminateUpdate)
				{//视点变化 超过阈值
					return 0;
				}
				/*
					*    假设上次装载 最后一个对象A 3M【从399M 达到402】
					*                       5 5M,6 1M,7 1M,A 3M
					*    如果 视点做很少变化，对象5,6,7,A都在可见区域
					*    这时：装载第一个对象N，不管装载的对象是否成功，都卸载内存：
					*          则对象5,6,7,A都会被卸载
					*    持续装载,5,6,7,A都会重新装载,产生震荡.
					*
					*    所以：可见对象的卸载只要 卸载A，使内存小于阈值即可
					*          如仅这样，还是会刚卸载A，又装载上A，产生震荡.
					*    所以：还要记录最后一次卸载的对象，当再次装载到A时就不再装载A
				*/	
				/*
					*    可以移除的对象A，有可能正在加载，这时会卸载比A近的可见对象，
					*    假设比A近的对象有1，2，3，这时把1,2,3卸载了,装载了比1近的对象M
					*    如果 在装载1时，对象A已经准备好，这时会卸载A，就可以装载1,2,3了
					*
					*    这样，在视点没有变化的情况下,1,2,3刚卸载，又给加载上了.
					*    所以：
					*         如果RemoveObject正在装载数据，则结束卸载.
				*/
				removeObj = dynamic_cast<CGlbGlobeRObject*> ((*roit).get());
				if (   removeObj->IsInViewPort() == true
					&& obj != removeObj
					&& (removeObj->IsAllowUnLoad() == true) /*允许卸载*/
					&& 
					(
						( removeObj->GetOsgNode()   != NULL						  
						  && removeObj->GetAltitudeMode() != GLB_ALTITUDEMODE_ONTERRAIN)
						)//非复合对象
						||
						(
						   removeObj->GetType() == GLB_OBJECTTYPE_COMPLEX					
						)//复合对象，复合对象osgNode 永远是NULL.					
					)
				{//卸载可见物体(且非贴地形绘制对象)
					if ((removeObj->GetLoadOrder() > obj->GetLoadOrder())
						|| 
						(removeObj->GetLoadOrder() == obj->GetLoadOrder()
						&& removeObj->GetDistance(mpr_eyePosition,false) > obj->GetDistance(mpr_eyePosition,false)))						
					{//removeObj的距离较远，优先级较差						
						if(removeObj->IsLoadData())
						{
							return unloadSize;
						}
						objsize    =  removeObj->RemoveFromScene(true);
						unloadSize += objsize;						
						if((hasUsedMemory - unloadSize) < g_memThreshold)
						{
							//记录最近卸载的可见对象.
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
		{//视点变化 超过阈值
			return;
		}
		if(  mpr_lastLoadObj
		   &&mit->first < mpr_lastLoadObj->GetLoadOrder() )
		{//比lastLoadObj优先级小的对象，已经Load，所以Continue;
			mit++;
			continue;
		}
		oit = mit->second.begin();
		while (oit != mit->second.end())
		{			
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return;
			}
			if(mpr_globe->mpr_taskmanager->GetObjectTaskNum() >= GLB_MAX_OBJTASKNUM)
			{
				/*
				*    开始时: hasUsedMemory = 0 ，如果不限制，会在任务队列加入大量任务
				*                                有可能超过内存限额.
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
			{//最近卸载的对象 以及距离比它远的，不装载.
				oit++;
				continue;
			}	
			if(    mpr_lastLoadObj
				&& mit->first == mpr_lastLoadObj->GetLoadOrder()
				&& distance < mpr_lastLoadObj->GetDistance(mpr_eyePosition,false)
				&& obj->IsLoadData() == true) // 由 <=  变为  < ，有距离相同而对象不同的情况
			{//与lastLoadObj 优先级相同的、且距离<=lastLoadObj的对象，已经Load，所以continue  --- （不一定呀，合并重排以后可能中间有没有加载的obj？）
				oit++;
				continue;
			}						
			if (obj->IsInViewPort() == true)//对象在视椎体内
			{//此处，obj未载入内存，有可能在mpr_taskmanager队列中，也可能不在。											
				if (   (distance > obj->GetMinVisibleDistance())
					&& (distance < obj->GetMaxVisibleDistance()) 
					&& obj->IsShow()/*对象需要显示*/)
				{//对象在最大最小范围内
					if(g_isMemCtrl)
					{						
						hasUsedMemory = CGlbGlobeMemCtrl::GetInstance()->GetUsedMem();
						if(hasUsedMemory > g_memThreshold)
						{
							glbInt64 unloadSize = UnLoadObject(obj);						
							/*
							    return，再次由近---->远加载对象.
							*/
							return;
						}
					}
					{//可以装载
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
					}//可以装载.
				}//在视距范围之内.
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
	rmit = mpr_objlist.rbegin();//反向访问，先访问ORDER最大，优先级最低的
	glbInt32 addObjSize = obj->GetOsgNodeSize();

	while (rmit != mpr_objlist.rend())
	{//卸载不可见	
		if(mpr_needTerminateUpdate)
		{//视点变化 超过阈值
			return 0;
		}
		std::vector<glbref_ptr<CGlbGlobeObject>>::reverse_iterator roit = rmit->second.rbegin();//反向访问，先访问距离最大，优先级最低
		while (roit != rmit->second.rend())
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return 0;
			}
			removeObj                        = dynamic_cast<CGlbGlobeRObject*> ((*roit).get());
			osg::ref_ptr<osg::Node> rosgNode = removeObj->GetOsgNode();
			if (  removeObj->IsInViewPort() == false
				  && obj != removeObj
				  && (removeObj->IsAllowUnLoad() == true) /*允许卸载*/
				  &&
				  (
				     (    rosgNode != NULL
					   && removeObj->GetAltitudeMode() != GLB_ALTITUDEMODE_ONTERRAIN
					   && rosgNode->getNumParents()!=0
					 )//非复合对象
					 ||
					 (
					   removeObj->GetType() == GLB_OBJECTTYPE_COMPLEX
					 )//复合对象，osgNode永远是NULL
				  )
				 
				)  
			{//卸载不可见物体
				removeSize += removeObj->RemoveFromScene(false);									
			}			
			if(removeSize > GLB_MAX_UNLOADSIZE)break;
			roit++;
		}
		if(removeSize > GLB_MAX_UNLOADSIZE)break;
		rmit++;
	}
	if (removeSize == 0)
	{//卸载可见		
		glbInt64 hasUsedDMem = mpr_globe->mpr_p_callback->GetUsedDislayMemory();
		std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>::reverse_iterator rmit;
		rmit = mpr_objlist.rbegin();//反向访问，先访问ORDER最大，优先级最低的
		while (rmit != mpr_objlist.rend())
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return 0;
			}
			std::vector<glbref_ptr<CGlbGlobeObject>>::reverse_iterator roit = rmit->second.rbegin();//反向访问，先访问距离最大，优先级最低
			while (roit != rmit->second.rend())
			{
				if(mpr_needTerminateUpdate)
				{//视点变化 超过阈值
					return 0;
				}
				removeObj                        = dynamic_cast<CGlbGlobeRObject*> ((*roit).get());
				osg::ref_ptr<osg::Node> rosgNode = removeObj->GetOsgNode();
				if (removeObj->IsInViewPort() == true
					&& (removeObj->IsAllowUnLoad() == true) /*允许卸载*/
					&& 
					(
					   (
					      removeObj->GetAltitudeMode() != GLB_ALTITUDEMODE_ONTERRAIN
						  && rosgNode != NULL
						  && rosgNode->getNumParents()>0
					   )//非复合对象
					   ||
					   (
					     removeObj->GetType() == GLB_OBJECTTYPE_COMPLEX
					   )//复合对象,osgNode永远是NULL
					)					
				   )
				{//卸载可见物体
					if(removeObj == obj)
					{//从远 --->近 卸载到obj，就应该结束
						return removeSize;
					}
					//removeobj的优先级低，距离远，应该卸载
					if ((removeObj->GetLoadOrder() > obj->GetLoadOrder())
						|| 
						(removeObj->GetLoadOrder() == obj->GetLoadOrder()
						&& removeObj->GetDistance(mpr_eyePosition,false) >obj->GetDistance(mpr_eyePosition,false)))
					{						
						glbInt32 removeObjSize = removeObj->GetOsgNodeSize();						
						if (hasUsedDMem+addObjSize-removeSize-removeObjSize > g_totalDMem)
						{// 只有当添加的-移除的超显存阈值，才卸载
							removeSize += removeObj->RemoveFromScene(false);
							break;
						}						
						else
						{// 不需要卸载的话只计数，认为可以卸载							
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

	glbInt32 hasUsedDMem = 0;//当前已使用显存总量
	glbInt32 hangSize    = 0;//待挂接osg节点尺寸.	
	glbDouble distance   =0;
	//////////////////////////////////////////////
	mit = mpr_objlist.begin();
	while (mit != mpr_objlist.end())
	{		
		if(mpr_needTerminateUpdate)
		{//视点变化 超过阈值
			return;
		}
		oit = mit->second.begin();
		while (oit != mit->second.end())
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return;
			}
			obj = dynamic_cast<CGlbGlobeRObject*> ((*oit).get());
			if(obj->IsDestroy())
			{				
				oit++;
				continue;
			}
			if (obj->IsInViewPort()==false)
			{//如果不在视口内，取下一个
				oit++;
				continue;
			}
			osg::ref_ptr<osg::Node> osgNode = obj->GetOsgNode();
			if(   osgNode == NULL
			   && obj->GetType() != GLB_OBJECTTYPE_COMPLEX)
			{//复合对象，osgNode永远是NULL，所以：复合对象需要继续.
				oit++;
				continue;
			}
			if (   osgNode != NULL 
				&& osgNode->getNumParents() != 0
				&& obj->GetType() != GLB_OBJECTTYPE_COMPLEX)
			{//已经在显存，continue
			 //复合对象，osgNode永远是NULL，所以：复合对象需要继续.
				oit++;
				continue;
			}			
			distance = obj->GetDistance(mpr_eyePosition,false);
			if ((distance<=obj->GetMinVisibleDistance()) || (distance >= obj->GetMaxVisibleDistance()) )
			{//距离不满足,continue
				oit++;
				continue;
			}
			hangSize = mpr_globe->mpr_p_callback->GetHangSize();
			if (hangSize>=FRAME_DEAL_SIZE)
			{//待挂osg节点超过 单帧流量限制				
				return;
			}			
			hasUsedDMem = mpr_globe->mpr_p_callback->GetUsedDislayMemory();							
			if(g_isDMemCtrl)
			{//显存总量控制				
				if (hasUsedDMem > g_totalDMem)
				{//已使用+待使用-待摘除>总量阈值
					glbInt64 rmsize = RemoveObjFromScene(obj);
					if (rmsize > 0)
					{//可以往场景树上挂对象
						obj->AddToScene();
					}
					return;
				}	
			}			
			{//可以往场景树上挂对象
				obj->AddToScene();				
			}
			oit++;
		}
		mit++;
	}
}
/*
*   快速排序
*   递归调用，有可能造成堆栈溢出！！！！.
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
			{//视点变化 超过阈值
				return;
			}
			CGlbGlobeRObject* jObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[j].get());

			while((i < j) 
				   && (compareTo(xObj,jObj ) == 1))//对象x优先级<j 或x距离 <j且x优先级=j【x应该排在j的前面】
			{
				if(mpr_needTerminateUpdate)
				{//视点变化 超过阈值
					return;
				}
				j--; 
				jObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[j].get());
			}

			if(i < j)
				(*objs)[i++] = (*objs)[j];

			CGlbGlobeRObject* iObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[i].get());

			while((i < j) &&  (compareTo(iObj, xObj)==1))//对象x优先级<j 或x距离 <j且x优先级=j【x应该排在j的前面】
			{
				if(mpr_needTerminateUpdate)
				{//视点变化 超过阈值
					return;
				}
				i++; 
				iObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[i].get());
			}
			if(i < j)

				(*objs)[j--] = (*objs)[i];

		}

		(*objs)[i] = x;
		quickSort(objs, l, i - 1); // 递归调用
		quickSort(objs, i + 1, r);

	}

}
/*
*   isComputeDistance 缺省是false
*   1: A 优先级 < B 或 A 距离 <B,且A优先级=B
*   -1:A 优先级 > B 或 A 距离 >B,且A优先级=B
*   0 ：A优先级 = B 且 A 距离 = B
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
//		{//视点变化 超过阈值
//			return;
//		}
//		newObj =  dynamic_cast<CGlbGlobeRObject*> ((*newItr).get());
//		glbInt32 newOrder = newObj->GetLoadOrder();
//		if (objmap.find(newOrder) != objmap.end())
//		{//渲染优先级存在
//			oldItr = objmap[newOrder].begin();
//			while (   (newItr!=objs.end()) 
//				   && (newObj->GetLoadOrder() == newOrder))  				
//			{//可见队列尚有未处理对象&&优先级与map本次优先级相同
//				if(mpr_needTerminateUpdate)
//				{//视点变化 超过阈值
//					return;
//				}
//				newObj =  dynamic_cast<CGlbGlobeRObject*> ((*newItr).get());
//				if (oldItr == objmap[newOrder].end())   //已到map本优先级末尾
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
//				//未到map被优先级末尾，需对比
//				{
//					if (newObj->GetId() == oldObj->GetId()) 
//					{						
//						newItr++;
//						oldItr++;
//					}
//					else if (newDistance < oldDistance)//可见对象优先级高，插入
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
//		else//rObjInObjs的优先级不在map中，新增该优先级并复制可见列表中同优先级的对象
//		{
//			while ((newItr!=objs.end()))  //可见队列还有对象&&优先级与新增优先级相同
//			{
//				if(mpr_needTerminateUpdate)
//				{//视点变化 超过阈值
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
		{//视点变化 超过阈值
			return;
		}
		mapVItr = mit->second.begin();
		for(mapVItr;mapVItr != mit->second.end();mapVItr++)
		{
			if(mpr_needTerminateUpdate)
			{//视点变化 超过阈值
				return;
			}			
			idmap[(*mapVItr).get()] = 0;
		}
	}

	objVItr = objs.begin();
	for(objVItr; objVItr != objs.end(); objVItr++)
	{
		if(mpr_needTerminateUpdate)
		{//视点变化 超过阈值
			return;
		}		
		if(idmap.find((*objVItr).get())!=idmap.end())
			continue;
		CGlbGlobeRObject* obj =  dynamic_cast<CGlbGlobeRObject*> ((*objVItr).get());
		glbInt32 loadOrder    =  obj->GetLoadOrder();
		mit = objmap.find(loadOrder);
		if(mit != objmap.end())
		{//装载优先级对应的序列已经存在.
			mit->second.push_back(obj);
		}
		else
		{//装载优先级对应的序列不存在.
			objmap[loadOrder].push_back(obj);
		}
	}
}

/*
	大顶-堆排序算法.
*/
//void CGlbGlobeDispatcher::BigHeapAdjust(std::vector<glbref_ptr<CGlbGlobeObject>>& objs,glbInt32 r,glbInt32 len)
//{
//	glbref_ptr<CGlbGlobeRObject> tmpObj = dynamic_cast<CGlbGlobeRObject*>(objs[r].get());
//	int j;
//	int cmp;
//	for(j=2*r+1; j<=len-1; j=2*j+1) 
//	{//沿节点值较大的儿子往下层筛选，2*r+1是左儿子，2*(r+1)是右儿子	
//		CGlbGlobeRObject* obj_j = dynamic_cast<CGlbGlobeRObject*>(objs[j].get());
//		if(j<len-1)
//		{//因为j<len-1时,p[j+1]才不会越界 
//			CGlbGlobeRObject* obj_j_1 = dynamic_cast<CGlbGlobeRObject*>(objs[j+1].get());
//			//cmp = compareTo(obj_j_1,obj_j);
//			//if(cmp == -1/* || cmp == 0*/)
//			if(obj_j_1->GetDistance(mpr_eyePosition,false) >= obj_j->GetDistance(mpr_eyePosition,false))
//				++j; //如果右儿子大于左儿子,j++转移到右儿子,让j等于左右儿子中较大的那个 
//		}
//		//cmp = compareTo(tmpObj,obj_j);
//		//if(cmp == -1/*||cmp == 0*/)break;	
//		if(tmpObj->GetDistance(mpr_eyePosition,false) >= obj_j->GetDistance(mpr_eyePosition,false))break;
//		objs[r] = objs[j].get(); //较大的儿子向父节点平移,并更新r节点的位置
//		r = j; 
//	}
//	objs[r] = tmpObj; 
//}
void CGlbGlobeDispatcher::bigHeapAdjust(std::vector<glbref_ptr<CGlbGlobeObject>>& objs,glbInt32 r,glbInt32 len)
{
	glbInt32 nChild;
	for(;2*r+1<len;r=nChild)
	{
		//子结点的位置=2*（父结点位置）+1
		nChild=2*r+1;
		//得到子结点中较大的结点
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
		{//如果较大的子结点大于父结点那么把较大的子结点往上移动，替换它的父结点
			/*nTemp=array[i];
　			array[i]=array[nChild];
　			array[nChild]=nTemp;*/
			objs[r]      = obj_child;
			objs[nChild] = obj_r;
		}
		else
		{//否则退出循环
			break;
		}
	}
}
void CGlbGlobeDispatcher::bigHeapSort(std::vector<glbref_ptr<CGlbGlobeObject>>& objs)
{
	glbInt32 len = objs.size();
	if(len<=1)return;
	
	for(int i=len/2 - 1;i>=0;--i)
	{//构建大顶
		if(mpr_needTerminateUpdate)
		{//视点变化 超过阈值
			return;
		}
		bigHeapAdjust(objs,i,len);
	}
	glbref_ptr<CGlbGlobeObject> obj;
	for(int i=len-1;i>0;--i)
	{
		if(mpr_needTerminateUpdate)
		{//视点变化 超过阈值
			return;
		}
		obj       = objs[i].get();
		objs[i]   = objs[0].get();
		objs[0]   = obj.get();
		bigHeapAdjust(objs,0,i);		
	}		
}