#include "StdAfx.h"
#include "GlbGlobeVisualLineAnalysis.h"

#include "CGlbGlobe.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbGlobeVisualLineSymbol.h"
#include "GlbPoint.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbGlobeFeatureLayer.h"

using namespace GlbGlobe;

//////////////////////////////////////////////////////////////////////////
CGlbGlobeVisualLineAnalysis::CGlbGlobeVisualLineAnalysisLoadDataTask::CGlbGlobeVisualLineAnalysisLoadDataTask( CGlbGlobeVisualLineAnalysis *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobeVisualLineAnalysis::CGlbGlobeVisualLineAnalysisLoadDataTask::~CGlbGlobeVisualLineAnalysisLoadDataTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeVisualLineAnalysis::CGlbGlobeVisualLineAnalysisLoadDataTask::doRequest()
{
	if(mpr_obj!=NULL)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
CGlbGlobeVisualLineAnalysis::CGlbGlobeVisualLineAnalysis(void)
{
	mpr_readData_critical.SetName(L"vline_readdata");
	mpr_addToScene_critical.SetName(L"vline_addscene");
	mpt_minVisibleDistance=0.0;
	//mpt_maxVisibleDistance = 150000000.0;
	mpr_distance = 0.0;
	mpr_renderInfo = NULL;
	mpr_watchPoint=NULL;//new CGlbPoint3D(0.0,0.0,0.0);
	mpr_targetPoint=NULL;//new CGlbPoint3D(0.0,0.0,0.0);	
	mpr_visualLine = new CGlbLine();
	mpr_visualLine->AddPoint(0.0,0.0,0.0);
	mpr_visualLine->AddPoint(0.0,0.0,0.0);
	mpr_objSize=0;
}

CGlbGlobeVisualLineAnalysis::~CGlbGlobeVisualLineAnalysis(void)
{

}

glbBool CGlbGlobeVisualLineAnalysis::Load( xmlNodePtr* node,glbWChar* prjPath )
{
	//throw std::exception("The method or operation is not implemented.");
	return false;
}

glbBool CGlbGlobeVisualLineAnalysis::Save( xmlNodePtr node,const glbWChar* prjPath )
{
	//throw std::exception("The method or operation is not implemented.");
	return false;
}

GlbGlobe::GlbGlobeObjectTypeEnum CGlbGlobeVisualLineAnalysis::GetType()
{
	return GLB_OBJECTTYPE_VISUALLINEANALYSIS;
}

glbDouble CGlbGlobeVisualLineAnalysis::GetDistance( osg::Vec3d &cameraPos,glbBool isCompute )
{
	if (mpt_globe==NULL) return 0.0;
	if (!isCompute)
		return mpr_distance;

	osg::Vec3d cameraWorldPos;
	if (mpt_globe->GetType()==GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),
			osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraWorldPos.x(),cameraWorldPos.y(),cameraWorldPos.z());
	}

	osg::Vec3d visualLineWorldPos;
	glbDouble visualLinePosX,visualLinePosY,visualLinePosZ;
	mpr_watchPoint->GetXYZ(&visualLinePosX,&visualLinePosY,&visualLinePosZ);

	if (mpt_globe->GetType()==GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(visualLinePosY),
			osg::DegreesToRadians(visualLinePosX),visualLinePosZ,visualLineWorldPos.x(),visualLineWorldPos.y(),visualLineWorldPos.z());
	}

	mpr_distance=(cameraWorldPos-visualLineWorldPos).length();
	return mpr_distance;
}

glbref_ptr<CGlbExtent> CGlbGlobeVisualLineAnalysis::GetBound( glbBool isWorld /*= true */ )
{
	if (mpr_watchPoint==NULL || mpr_targetPoint==NULL)
		return NULL;

	glbref_ptr<CGlbExtent> geoExt = new CGlbExtent;
	glbDouble x,y,z;
	mpr_watchPoint->GetXYZ(&x,&y,&z);
	geoExt->Merge(x,y,z);
	mpr_watchPoint->GetXYZ(&x,&y,&z);
	geoExt->Merge(x,y,z);
	
	if (isWorld==false)	return geoExt;

	if (mpt_globe==NULL) return NULL;
	glbref_ptr<CGlbExtent> worldExt = NULL;
	if (mpt_globe->GetType()==GLB_GLOBETYPE_GLOBE)
	{
		worldExt = new CGlbExtent;
		geoExt->GetMin(&x,&y,&z);
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,x,y,z);
		worldExt->Merge(x,y,z);
		geoExt->GetMax(&x,&y,&z);
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,x,y,z);
		worldExt->Merge(x,y,z);
	}
	else
		worldExt = geoExt;

	return worldExt;
}

void CGlbGlobeVisualLineAnalysis::LoadData( glbInt32 level )
{
	if(mpt_currLevel == level) return;
	if(mpt_preNode  !=  NULL)
	{//�ɶ���δѹ��ժ����
		/*
		*   mpt_node = node1,preNode = NULL    -----> node1_h
		*   �������ƣ���LoadData��
		*   {
		*       mpt_node = node2,preNode = node1
		*       mpt_node = node3,preNode = node2   -----> node1_h,node2_r,node2_h
		*       ���node1 ��Զ����ժ��
		*   }
		*/
		return;
	}

	glbref_ptr<GlbVisualLineSymbolInfo> visualLineSymbol = dynamic_cast<GlbVisualLineSymbolInfo *>(mpr_renderInfo.get());
	if (visualLineSymbol==NULL) return;

	if (mpt_currLevel != -1) 
		return;
	
	{//��DirectDraw����.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return;
		mpt_loadState = true;
	}		
	if(mpt_currLevel != level)
	{
		if(mpt_currLevel < level)
			level = mpt_currLevel + 1;
	}
	glbref_ptr<CGlbGlobeVisualLineAnalysisLoadDataTask> task = new CGlbGlobeVisualLineAnalysisLoadDataTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobeVisualLineAnalysis::ReadData( glbInt32 level ,glbBool isDirect)
{
	if (NULL==mpr_renderInfo || mpr_watchPoint==NULL || mpr_targetPoint==NULL)
	{mpt_loadState=false;return;}

	glbref_ptr<GlbVisualLineSymbolInfo> visualLineSymbolInfo = dynamic_cast<GlbVisualLineSymbolInfo *>(mpr_renderInfo.get());
	if (NULL==visualLineSymbolInfo) 
	{mpt_loadState=false;return;}

	osg::Group *visualLineContainer=NULL;
	osg::Node *node = NULL;

	CGlbGlobeVisualLineSymbol visualLineSymbol;
	node = visualLineSymbol.Draw(this,mpr_visualLine.get());

	if (node == NULL)
	{	mpt_loadState = false;	return;	}
	visualLineContainer=node->asGroup();

	osg::Switch *swiNode = new osg::Switch();
	swiNode->setValue(0,mpt_isShow);
	if (visualLineContainer!=NULL)
		swiNode->addChild(visualLineContainer);

	if(mpr_needReReadData)
	{
		/*
		*   �ڼ��mpr_needReReadDataʱ,�����߳�����mpr_needReReadData=true��ûִ�У�����.
		*/
		mpr_needReReadData = false;
		return ReadData(level,true);
	}

	glbInt32 objsize = this->ComputeNodeSize(swiNode);
	glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(objsize);
	if(isOK||isDirect)
	{
		mpt_preNode   = mpt_node;
		mpt_node      = swiNode;
		mpt_currLevel = level;		
		mpr_objSize   = objsize;
	}else{
		swiNode = NULL;
	}
	mpt_loadState = false;	
}

void CGlbGlobeVisualLineAnalysis::AddToScene()
{
	if(mpt_node == NULL)return;
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	bool needUpdate = false;
	if(mpt_preNode == NULL)
	{//û�в�����osg�ڵ�
		if(mpt_HangTaskNum == 0
			&& mpt_node->getNumParents() == 0
			&& mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//����ģʽ,�ڵ㲻��Ҫ��.
			//��mpt_node����
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;
			needUpdate = true;
		}			
	}
	else
	{//��������osg�ڵ�						
		{//�ɽڵ��Ѿ��ҵ�������
			//ժ�ɽڵ�mpt_preNode������
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
			task->mpr_size = this->ComputeNodeSize(mpt_preNode);
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			mpt_preNode = NULL;
			CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(task->mpr_size);
		}
		if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//���½ڵ�mpt_node����
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;	
		}
		needUpdate = true;
	}
	if(needUpdate)
	{
		//UpdateWorldExtent();
		mpt_globe->UpdateObject(this);
		if (mpt_isShow == false)
			mpt_node->asSwitch()->setValue(0,false);		
	}
}

glbInt32 CGlbGlobeVisualLineAnalysis::RemoveFromScene( glbBool isClean )
{
	{//��LoadData����.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return 0;	
		mpt_loadState = true;
		/*
		*    mpt_node = node1,pre=NULL
		*    ��:mpt_node = node2,pre=node1		
		*/
	}
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	
	glbInt32 tsize = 0;
	if(isClean == false)
	{//���Դ�ж�ض��󣬽�Լ�Դ�.
		if ((mpt_node != NULL))// && (mpt_node->getNumParents() != 0))
		{//obj���Դ�	
			/*
			     mpt_node �Ѿ��ڹҶ��У����ǻ�û�ҵ���������
				 ��ʱ�ж�getNumParents() != 0 �ǲ����Ե�.
			*/
			CGlbGlobeCallBack::CGlbGlobeCallBackTask *task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			tsize = task->mpr_size;
		}
	}
	else
	{//���ڴ���ж�ض��󣬽�Լ�ڴ�
		//ɾ�� ��һ��װ�صĽڵ�		
		if (mpt_preNode != NULL)
		{
			//if(mpt_preNode->getNumParents() > 0 )
			{				
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
				task->mpr_size = ComputeNodeSize(mpt_preNode);
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}
			tsize += ComputeNodeSize(mpt_preNode);
			mpt_preNode = NULL;
		}
		//ɾ����ǰ�ڵ�
		if (mpt_node != NULL) 
		{
			//if(mpt_node->getNumParents() > 0)
			{		
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
				task->mpr_size = this->GetOsgNodeSize();
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}
			tsize       += this->GetOsgNodeSize();			
			mpt_node    =  NULL;
			mpr_objSize =  0;
		}		
		if (mpt_feature != NULL && mpt_featureLayer != NULL)
		{
			CGlbGlobeFeatureLayer* fl = dynamic_cast<CGlbGlobeFeatureLayer*>(mpt_featureLayer);
			if (fl)
				fl->NotifyFeatureDelete(mpt_feature->GetOid());
		}
		mpt_currLevel = -1;					
	}
	mpt_loadState = false;
	//����ʹ���ڴ�
	if(tsize>0 && isClean)
	{
		CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(tsize);		
	}
	return tsize;	
}

IGlbGeometry * CGlbGlobeVisualLineAnalysis::GetOutLine()
{
	//return NULL;// ��OutLine
	return mpr_watchPoint.get();
}

glbBool CGlbGlobeVisualLineAnalysis::SetRenderInfo( GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_VISUALLINEANALYSIS)
		return false;

	//glbBool isNeedDirectDraw = false;//��ʶ�������Ҫ��������
	isNeedDirectDraw = false;//��ʶ�������Ҫ��������

	if (mpt_node!=NULL)// ��Ϊ��˵�����Ѿ���ʵ��
	{
		/*GlbPolygonSymbolInfo *polygonInfo = static_cast<GlbPolygonSymbolInfo *>(renderInfo);
		if (polygonInfo == NULL)
		return false;*/
		//if (polygonInfo->symType == GLB_POLYGONSYMBOL_2D)
		{
			GlbVisualLineSymbolInfo *visualLineSymbolInfo = dynamic_cast<GlbVisualLineSymbolInfo *>(mpr_renderInfo.get());
			if (NULL==visualLineSymbolInfo) 
				return false;
			/*if(!judgePolygonNeedDirectDraw(visualLineSymbolInfo,isNeedDirectDraw))
			return false;*/
			isNeedDirectDraw=true;
		}

	}

	//EnterCriticalSection(&mpr_createNode_critical);
	//mpr_renderInfo = renderInfo;
	//LeaveCriticalSection(&mpr_createNode_critical);
	//if (isNeedDirectDraw)//��Ҫ�����ػ�
	//	DirectDraw(0);

	mpr_renderInfo = renderInfo;
	mpt_loadState  = false;
	return true;
}

GlbRenderInfo * CGlbGlobeVisualLineAnalysis::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeVisualLineAnalysis::SetShow( glbBool isShow,glbBool isOnState/*=false */ )
{
	if (mpt_isEdit && isOnState)// �༭״̬�����ɵ��ȿ�������
		return true;
	/*
	* ���������ݿɼ���Χ���ƶ�����ʾ�����ش�ʱisOnState==true,isShow==true��false
	*/
	if(isOnState)
	{// ��������������		
		if (mpt_node)
		{// �ڵ����
			if(isShow)
			{// ���������Ҫ����ʾ���û�Ҫ�����ػ���ʾ�����û�Ϊ׼
				if(mpt_isShow == mpt_node->asSwitch()->getValue(0))
					return true;
				mpt_node->asSwitch()->setValue(0,mpt_isShow);			
			}
			else
			{// ���볬����ʾ��Χ�󣬵�����Ҫ������
				if(isShow == mpt_node->asSwitch()->getValue(0))
					return true;
				mpt_node->asSwitch()->setValue(0,isShow);		
			}	
		}	
	}
	else
	{// �û���������
		if (mpt_isShow == isShow) return true;
		mpt_isShow = isShow;
		if (mpt_node)
			mpt_node->asSwitch()->setValue(0,mpt_isShow);
	}		
	return true;
}

void CGlbGlobeVisualLineAnalysis::DirectDraw( glbInt32 level )
{
	if (mpt_parent    == NULL) return;		
	{//��LoadData����.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			    LoadData->ReadData��ѡ���˳������ǻ�����������⣺
				        LoadData->ReadData ʹ�õ��Ǿɵ���Ⱦ��ص����ԣ����ܷ�ӳ�����õ���Ⱦ����.
				������ֱ���˳������ѡ��ȴ�LoadData->ReadData�Ļ�������������̣߳���������

				��������һ����־��ReadData ִ�е�ĩβʱ�����ñ�־,�����ʾ��true��ReadData��ִ��һ��.
			*/
			mpr_needReReadData = true;
			return;
		}
		mpt_loadState = true;
	}
	if(mpt_preNode != NULL)
	{
	/*
	*    mpr_node = node2,preNode = node1  ----A�̵߳���AddToScene��
	*                                         ��û��preNode Remove��ûִ����
	*    ����ReadData �Ѿ����꣺
	*    mpr_ndoe = node3,preNode = node2   ���node1��Զ����ժ��.
	*/
		AddToScene();
	}
	/*
	*    ��Ϊ��LoadData->ReadDataʱ��ReadData װ�ض�����ֳ����ڴ��޶�
	*        �������¼��ص�osg�ڵ�.
	*    ����DirectDraw���ԣ���������µ�osg�ڵ㣬�Ͳ��ܷ�ӳ���µı仯.
	*    ���ԣ�
	*         ReadData��������һ�������������DirectDraw���õ�,�����Ƿ񳬹�
	*         �ڴ��޶�,���������µ�osg�ڵ�.
	*/
	ReadData(level,true);
	AddToScene();
}

glbInt32 CGlbGlobeVisualLineAnalysis::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		return ComputeNodeSize(mpt_node);
	return mpr_objSize;
}
glbInt32 CGlbGlobeVisualLineAnalysis::ComputeNodeSize( osg::Node *node )
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

glbBool CGlbGlobeVisualLineAnalysis::SetSightLine( CGlbPoint *watchPoint,CGlbPoint *targetPoint,glbBool &isNeedDirectDraw )
{
	if (watchPoint==NULL||targetPoint==NULL)
		return false;
	
	isNeedDirectDraw = false;
	{   		
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			 *  �������ReadData��ReadData�������µĵ���ģʽ֮ǰʹ���˾ɵ�λ��.
			 *  ���ԣ�
					��Ҫ��֪�����ߣ�����DirectDraw.
			*/
			mpr_watchPoint=watchPoint;
			mpr_targetPoint=targetPoint;
			isNeedDirectDraw = true;						
			return true;
		}
		mpt_loadState = true;
	}		

	mpr_watchPoint=watchPoint;
	mpr_targetPoint=targetPoint;
	glbDouble x,y,z;
	if (mpr_watchPoint==NULL)
	{
		mpr_watchPoint=watchPoint;
		mpr_targetPoint=targetPoint;		
		watchPoint->GetXYZ(&x,&y,&z);
		mpr_visualLine->SetPoint(0,x,y,z);
		targetPoint->GetXYZ(&x,&y,&z);
		mpr_visualLine->SetPoint(1,x,y,z);
		/*mpr_visualLineExtDirty=true;
		mpr_SizeDirty=true;
		mpr_SizePreDirty=true;*/
	}
	else
	{
		mpr_watchPoint=watchPoint;
		mpr_targetPoint=targetPoint;		
		watchPoint->GetXYZ(&x,&y,&z);
		mpr_visualLine->SetPoint(0,x,y,z);
		targetPoint->GetXYZ(&x,&y,&z);
		mpr_visualLine->SetPoint(1,x,y,z);
		/*mpr_visualLineExtDirty=true;
		mpr_SizeDirty=true;
		mpr_SizePreDirty=true;*/
	
		isNeedDirectDraw = true;
	}		
	mpt_loadState = false;
	return true;
}