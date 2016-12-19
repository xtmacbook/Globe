#include "StdAfx.h"
#include "GlbGlobeCoordinateAxisObject.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeTypes.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbColorVisitor.h"
#include "GlbFadeInOutCallback.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbGlobeCoordinateAxisSymbol.h"

using namespace GlbGlobe;

// CGlbGlobeCoordinateAxisObject �ӿ�ʵ��
CGlbGlobeCoordinateAxisObject::CGlbGlobeCoordinateAxisTask::CGlbGlobeCoordinateAxisTask( CGlbGlobeCoordinateAxisObject *obj, glbInt32 level )
{
	mpr_obj = obj;	
	mpr_level = level;
}

CGlbGlobeCoordinateAxisObject::CGlbGlobeCoordinateAxisTask::~CGlbGlobeCoordinateAxisTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeCoordinateAxisObject::CGlbGlobeCoordinateAxisTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeCoordinateAxisObject::CGlbGlobeCoordinateAxisTask::getObject()
{
	return mpr_obj.get();
}

//////////////////////////////////////////////////////////////////////////
CGlbGlobeCoordinateAxisObject::CGlbGlobeCoordinateAxisObject(void)
{	
	mpr_readData_critical.SetName(L"coord_axis_readdata");
	mpr_addToScene_critical.SetName(L"coord_axis_addscene");
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE; //�����ᶼ�Ǿ������귽ʽ
	mpr_needReReadData = false;
}

CGlbGlobeCoordinateAxisObject::~CGlbGlobeCoordinateAxisObject(void)
{
	mpr_outline = NULL;
	mpr_renderInfo = NULL;
}

GlbGlobeObjectTypeEnum CGlbGlobeCoordinateAxisObject::GetType()
{
	return GLB_OBJECTTYPE_COORDINAT_AXIS;
}

glbBool CGlbGlobeCoordinateAxisObject::Load(xmlNodePtr* node,glbWChar* prjPath)
{
	CGlbGlobeRObject::Load(node,prjPath);
	return false;
}

glbBool CGlbGlobeCoordinateAxisObject::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Save(node,prjPath);
	return false;
}

glbDouble CGlbGlobeCoordinateAxisObject::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
{
	if(isCompute == false)
		return mpr_distance;

	if (mpt_isCameraAltitudeAsDistance)	
	{
		mpr_distance = cameraPos.z();
	}	
	else
	{
		if (mpt_globe==NULL || mpr_renderInfo==NULL)
			return DBL_MAX;		
		GlbCoordinateAxisSymbolInfo *axisInfo = dynamic_cast<GlbCoordinateAxisSymbolInfo *>(mpr_renderInfo.get());

		glbDouble xOrLon,yOrLat,zOrAlt;		
		xOrLon = axisInfo->originX;
		yOrLat = axisInfo->originY;
		zOrAlt = axisInfo->originZ;
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();	

		osg::Vec3d cameraPoint = cameraPos;
		if (globeType==GLB_GLOBETYPE_GLOBE)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yOrLat),osg::DegreesToRadians(xOrLon),zOrAlt,xOrLon,yOrLat,zOrAlt);		
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),
				osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());		
		}
		mpr_distance = (osg::Vec3d(xOrLon,yOrLat,zOrAlt)-cameraPoint).length();
	}

	return mpr_distance;
}

glbBool CGlbGlobeCoordinateAxisObject::SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (mode != GLB_ALTITUDEMODE_ABSOLUTE)
		return false;
	return true;
}

/*
*   mpt_node��mpt_preNode ���ɡ���ΪNULL �ڲ�ͬ�߳�
*   LoadData ֻ�� Dispatcher �̵߳���
*   
*   �޸�renderinfo,position ���������߳�
*/
 void CGlbGlobeCoordinateAxisObject::LoadData(glbInt32 level)
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

	// ��3Dģ�����ͣ����Ѿ�load�����Ͳ����ٴ�load
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
	glbref_ptr<CGlbGlobeCoordinateAxisTask> task = new CGlbGlobeCoordinateAxisTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
 }

 void CGlbGlobeCoordinateAxisObject::ReadData(glbInt32 level,glbBool isDirect)
 {
	glbref_ptr<GlbCoordinateAxisSymbolInfo> renderInfo = dynamic_cast<GlbCoordinateAxisSymbolInfo*>(mpr_renderInfo.get());
	if( renderInfo == NULL || mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE )
	{mpt_loadState=false;return;}

	if (renderInfo == NULL) {mpt_loadState=false;return;}
	CGlbGlobeCoordinateAxisSymbol symbol;
	osg::ref_ptr<osg::Node> node = symbol.Draw(this,NULL);
	if (node==NULL){mpt_loadState=false;return;}

	osg::ref_ptr<osg::Switch> swiNode = node->asSwitch();
	if(mpr_needReReadData)
	{
		/*
		*   �ڼ��mpr_needReReadDataʱ,�����߳�����mpr_needReReadData=true��ûִ�У�����.
		*/
		mpr_needReReadData = false;
		return ReadData(level,true);
	}

	if (swiNode==NULL)
		return;
	//if (mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
	//{// ������ģʽ��Ҫ�����ڴ�����
		//����ʹ���ڴ�		
		glbInt32 objsize = ComputeNodeSize(swiNode);
		glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(objsize);
		if(isOK||isDirect)
		{
			// Ԥ�ȼ���bound����ʡʱ��
			swiNode->getBound();

			mpt_preNode   = mpt_node;
			mpt_node      = swiNode;
			mpt_currLevel = level;		
			mpr_objSize   = objsize;			
		}else{
			swiNode = NULL;
		}
	//}
	mpt_loadState = false;	
 }

 /*
 *     Dispatcher->AddToScene()->CGlbGlobePoint::AddToScene()  //Ƶ������
 *     �ⲿ�߳�-->DirectDraw()->AddToScene() //Ƶ������
 */
void CGlbGlobeCoordinateAxisObject::AddToScene()
{
	if(mpt_node == NULL)return;
	if(mpt_isFaded)
	{
		osg::ref_ptr<CGlbFadeInOutCallback> fadeCb = 
			new CGlbFadeInOutCallback(mpt_fadeStartAlpha,mpt_fadeEndAlpha,this,mpt_fadeDurationTime,mpt_fadeColor);
		mpt_node->addUpdateCallback(fadeCb.get());
	}
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
			task->mpr_size = ComputeNodeSize(mpt_preNode);
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
		glbBool isnew = mpt_isDispShow && mpt_isShow;
		if (isnew == false)			
			mpt_node->asSwitch()->setValue(0,false);
		if (mpt_isSelected)
			DealSelected();
		if(mpt_isBlink)
			DealBlink();
	}
}
/*
*   update�߳� --->Dispatcher--->RemoveFromScene(true)         ���ڴ���ж�ض���
*                      |
*                      |---->CallBack->RemoveFromeScene(false) ���Դ���ж�ض���
*
*   ֻ��update�̻߳� ���ø÷��������Բ���Ҫ��������
*/
glbInt32 CGlbGlobeCoordinateAxisObject::RemoveFromScene(glbBool isClean)
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
/*
*    DirectDraw �� LoadData  Ҫ�����:��������
*    mpr_loadState��glbBool����Ϊ����ֿ�.
*    Dispatcher�߳�---�� AddToScene  ����.
*/
void CGlbGlobeCoordinateAxisObject::DirectDraw(glbInt32 level)
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

glbref_ptr<CGlbExtent> CGlbGlobeCoordinateAxisObject::GetBound(glbBool isWorld)
{
	if (mpt_globe == NULL && isWorld) return NULL;

	if (mpr_renderInfo==NULL) return NULL;

	GlbCoordinateAxisSymbolInfo *axisInfo = dynamic_cast<GlbCoordinateAxisSymbolInfo *>(mpr_renderInfo.get());
	osg::Vec3d	origin(axisInfo->originX,axisInfo->originY,axisInfo->originZ);					// ������ԭ��
	osg::Vec3d	axisLen(axisInfo->axisLengthX,axisInfo->axisLengthY,axisInfo->axisLengthZ);		// x,y,z������ĳ���

	glbref_ptr<CGlbExtent> geoExt = new CGlbExtent;

	GlbGlobeTypeEnum globeType = mpt_globe->GetType();
	if (globeType!=GLB_GLOBETYPE_GLOBE)	
	{
		geoExt->Merge(origin.x(),origin.y(),origin.z());
		geoExt->Merge(origin.x()+axisLen.x(),origin.y()+axisLen.y(),axisInfo->bInvertZ ? (origin.z()-axisLen.z()) : (origin.z()+axisLen.z()));
	}
	else
		geoExt->Merge(origin.x(),origin.y(),origin.z());

	if (isWorld == false)
	{//��������
		return geoExt;	
	}
	else // ��������
	{// ��������
		glbref_ptr<CGlbExtent> worldExt = new CGlbExtent();
		osg::ref_ptr<osg::Node> node = mpt_node;
		if(   node != NULL
			&&node->getBound().valid() )
		{
			//����1
			//CGlbCalculateBoundBoxVisitor bboxV;
			//mpt_node->accept(bboxV);
			//osg::BoundingBoxd bb =bboxV.getBoundBox();
			//����2
			osg::BoundingSphere bs = node->getBound();
			osg::BoundingBoxd bb;
			bb.expandBy(bs._center - osg::Vec3d(bs._radius,bs._radius,bs._radius));
			bb.expandBy(bs._center + osg::Vec3d(bs._radius,bs._radius,bs._radius));
			worldExt->SetMin(bb.xMin(),bb.yMin(),bb.zMin());
			worldExt->SetMax(bb.xMax(),bb.yMax(),bb.zMax());
		}
		else
		{//osg �ڵ㻹û����.
			
			if (globeType==GLB_GLOBETYPE_GLOBE)
			{
				double x,y,z;
				x = origin.x(); y = origin.y(); z = origin.z();	
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,x,y,z);	
				worldExt->Merge(x,y,z);			
					
				worldExt->Merge(x+axisLen.x(),y+axisLen.y(),z+axisLen.z());				
			}
			else if (globeType==GLB_GLOBETYPE_FLAT)		
				worldExt = geoExt;	
		}
		return worldExt;
	}
	return NULL;
}

IGlbGeometry *CGlbGlobeCoordinateAxisObject::GetOutLine()
{
	return NULL;
}

glbBool CGlbGlobeCoordinateAxisObject::SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_COORDINAT_AXIS)  return false;
	GlbCoordinateAxisSymbolInfo *axisInfo = dynamic_cast<GlbCoordinateAxisSymbolInfo *>(renderInfo);
	if (axisInfo == NULL) return false;		
	{//��LoadData����.
		/*
		*   ������Ϊ�� ReadDataִ������;ʱ�����ɵĽ�����ܲ��ᷴӦ �����õ�����.
		*/
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			isNeedDirectDraw = true;
			mpr_renderInfo   = renderInfo;
			return true;			
		}
		mpt_loadState = true;
	}
	if(mpt_node == NULL)
	{		
		mpr_renderInfo   = renderInfo;		
		mpt_loadState    = false;
		/*
			���isNeedDirectDraw=true�� ��ͼ����Ⱦ��renderer����SetRenderInfo�ӿڻᵼ��
			ͼ�������ж���Ҫ���أ����ͼ��������ܴ��ֱ�ӵ��±�����������
		*/
		//isNeedDirectDraw = true;
		return true;
	}
	
	glbBool rt = false;
	rt = DealCoordinateAxisRenderInfo(axisInfo,isNeedDirectDraw);
	if (rt) mpr_renderInfo   = renderInfo;
	mpt_loadState    = false;
	return rt;	
}

GlbRenderInfo *CGlbGlobeCoordinateAxisObject::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeCoordinateAxisObject::SetShow(glbBool isShow,glbBool isOnState)
{
	if (mpt_node==NULL && isOnState==true)
		return false;

	if (mpt_isEdit && isOnState)// �༭״̬�����ɵ��ȿ�������
		return true;

	glbBool isold = mpt_isDispShow && mpt_isShow;
	if(isOnState)
	{// ��������������
		mpt_isDispShow = isShow;
	}
	else
	{// �û���������
		mpt_isShow = isShow;
	}

	glbBool isnew = mpt_isDispShow && mpt_isShow;

	if(isold == isnew)
		return true;

	if (mpt_node)
	{
		if(isnew)
		{
			mpt_node->asSwitch()->setAllChildrenOn();
		}
		else
			mpt_node->asSwitch()->setAllChildrenOff();
	}

	return true;
}

glbBool CGlbGlobeCoordinateAxisObject::SetSelected(glbBool isSelected)
{
	if(mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DealSelected();
	}
	return true;
}

glbBool CGlbGlobeCoordinateAxisObject::SetBlink(glbBool isBlink)
{
	if (mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

glbInt32 CGlbGlobeCoordinateAxisObject::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

glbInt32 CGlbGlobeCoordinateAxisObject::ComputeNodeSize(osg::Node *node)
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

void CGlbGlobeCoordinateAxisObject::DealSelected()
{

}
void CGlbGlobeCoordinateAxisObject::DealBlink()
{

}

bool CGlbGlobeCoordinateAxisObject::DealCoordinateAxisRenderInfo(GlbCoordinateAxisSymbolInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	GlbCoordinateAxisSymbolInfo *tempInfo = dynamic_cast<GlbCoordinateAxisSymbolInfo *>(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;

	if (tempInfo->bShowGrid != renderInfo->bShowGrid)
	{
		DealCoordinateAxisShowGrid(renderInfo->bShowGrid);
		isNeedDirectDraw = false;
		return true;
	}

	if (tempInfo->bShowLabel != renderInfo->bShowLabel)
	{
		DealCoordinateAxisShowLabel(renderInfo->bShowLabel);
		isNeedDirectDraw = false;
		return true;
	}

	if (tempInfo->xOffset != renderInfo->xOffset || 
		tempInfo->yOffset != renderInfo->yOffset || 
		tempInfo->zOffset != renderInfo->zOffset )
	{
		osg::Vec3d orign(renderInfo->originX,renderInfo->originY,renderInfo->originZ);
		osg::Vec3d offset(renderInfo->xOffset,renderInfo->yOffset,renderInfo->zOffset);
		DealCoordinateAxisOffset(orign,offset);
		isNeedDirectDraw = false;
		return true;
	}

	isNeedDirectDraw = true;
	return true;
}

void CGlbGlobeCoordinateAxisObject::DealCoordinateAxisShowGrid(glbBool bShowGrid)
{
	if (mpt_node)
	{// �ҵ�grids�ڵ㣬��Ϊ���ɼ���
		osg::Group* grp = mpt_node->asSwitch()->getChild(0)->asGroup();
		if (grp)
		{
			for (size_t k = 0; k < grp->getNumChildren(); k++)
			{
				if ("AxisGrid" == grp->getChild(k)->getName())
				{	
					if (bShowGrid)
						grp->getChild(k)->setNodeMask(0xffffffff);
					else
						grp->getChild(k)->setNodeMask(0x0);

					break;
				}
			}
		}
	}
}

void CGlbGlobeCoordinateAxisObject::DealCoordinateAxisShowLabel(glbBool bShowLabel)
{
	if (mpt_node)
	{// �ҵ�grids�ڵ㣬��Ϊ���ɼ���
		osg::Group* grp = mpt_node->asSwitch()->getChild(0)->asGroup();
		if (grp)
		{
			for (size_t k = 0; k < grp->getNumChildren(); k++)
			{
				if ("AxisLabel" == grp->getChild(k)->getName())
				{	
					if (bShowLabel)
						grp->getChild(k)->setNodeMask(0xffffffff);
					else
						grp->getChild(k)->setNodeMask(0x0);

					break;
				}
			}
		}
	}
}

void CGlbGlobeCoordinateAxisObject::DealCoordinateAxisOffset(osg::Vec3d origin, osg::Vec3d offset)
{
	if (mpt_node && mpt_node->asGroup()->getNumChildren()>0)
	{// �ҵ�grids�ڵ㣬��Ϊ���ɼ���
		osg::MatrixTransform* mtNode = dynamic_cast<osg::MatrixTransform*>(mpt_node->asSwitch()->getChild(0));
		if (mtNode)
		{
			GlbGlobeTypeEnum globeType = GetGlobe()->GetType();
			glbDouble x,y,z;
			x = origin.x();
			y = origin.y();
			z = origin.z();
			osg::Matrixd localToWorld;
			if (globeType==GLB_GLOBETYPE_GLOBE)		
				g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,localToWorld);
			else	
				localToWorld.makeTranslate(x,y,z);	
			
			osg::Matrixd mOffset;
			{
				double xOffset = offset.x();
				double yOffset = offset.y();
				double zOffset = offset.z();

				if (globeType==GLB_GLOBETYPE_GLOBE)		
				{
					double longitude = osg::DegreesToRadians(origin.x());
					double latitude = osg::DegreesToRadians(origin.y());
					// Compute up vector
					osg::Vec3d    up      ( cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
					// Compute east vector
					osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
					// Compute north  vector = outer product up x east
					osg::Vec3d    north   = up ^ east;

					north.normalize();
					osg::Vec3 _Offset = east*xOffset + north*yOffset + up*zOffset;
					mOffset.makeTranslate(_Offset);
				}
				else
					mOffset.makeTranslate(xOffset,yOffset,zOffset);
			}
			// ����ƫ�ƾ���
			mtNode->setMatrix(localToWorld * mOffset);
		}
	}
}