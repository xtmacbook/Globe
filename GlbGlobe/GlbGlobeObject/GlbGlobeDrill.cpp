#include "StdAfx.h"
#include "GlbGlobeDrill.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeTypes.h"
#include "GlbPolygon.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobeDrillSymbol.h"
#include "GlbDrill.h"
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/PolytopeIntersector>
#include <osg/Material>
#include "GlbGlobeAutoDrill.h"
#include "GlbColorVisitor.h"
#include "GlbFadeInOutCallback.h"

using namespace GlbGlobe;

class DrillObjectCallback : public osg::NodeCallback
{
public:
	DrillObjectCallback(CGlbGlobeRObject *obj, osg::Node* stratumNode=NULL)
	{
		mpr_isChangeFrame = false;
		mpr_markNum = 0;	
		CGlbGlobeDrill *drillObject = dynamic_cast<CGlbGlobeDrill *>(obj);		
		mpr_stateset = NULL;
		if (stratumNode==NULL)
		{
			if(drillObject)			
				mpr_stateset = drillObject->GetOsgNode()->getOrCreateStateSet();					
		}
		else
		{
			mpr_stateset = stratumNode->getOrCreateStateSet();
		}

		if(mpr_stateset)
		{

			osg::Material *material  = 
				dynamic_cast<osg::Material *>(mpr_stateset->getAttribute(osg::StateAttribute::MATERIAL));
			if (!material)			
				mpr_material = new osg::Material;			
			else
				mpr_material = material;
			mpr_stateset->setAttribute(mpr_material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}
	}	

	void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		if (nv->getFrameStamp()->getFrameNumber() - mpr_markNum > 20)
		{
			mpr_markNum = nv->getFrameStamp()->getFrameNumber();
			mpr_isChangeFrame = !mpr_isChangeFrame;
		} 
		
		if (mpr_isChangeFrame)
		{			
			mpr_stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);

			mpr_material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));					
			mpr_stateset->setAttribute(mpr_material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			mpr_stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		}
		else
		{
			osg::Material *material  = 
				dynamic_cast<osg::Material *>(mpr_stateset->getAttribute(osg::StateAttribute::MATERIAL));
			mpr_stateset->removeAttribute(material);
			mpr_stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
			//mpr_stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		}
	}
private:
	osg::ref_ptr<osg::Material> mpr_material;
	osg::ref_ptr<osg::StateSet> mpr_stateset;
	glbBool mpr_isChangeFrame;
	unsigned int mpr_markNum;
	osg::ref_ptr<osg::Node> stratumNode;
};

// CGlbGlobeDrill �ӿ�ʵ��
CGlbGlobeDrill::CGlbGlobeDrillTask::CGlbGlobeDrillTask( CGlbGlobeDrill *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobeDrill::CGlbGlobeDrillTask::~CGlbGlobeDrillTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeDrill::CGlbGlobeDrillTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeDrill::CGlbGlobeDrillTask::getObject()
{
	return mpr_obj.get();
}
//////////////////////////////////////////////////////////////////////////

CGlbGlobeDrill::CGlbGlobeDrill(void)
{
	mpr_readData_critical.SetName(L"drill_readdata");
	mpr_addToScene_critical.SetName(L"drill_addscene");
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE; //��׶��Ǿ������귽ʽ
	mpr_needReReadData = false;
	mpr_isAuto = false;
}

CGlbGlobeDrill::~CGlbGlobeDrill(void)
{	
	mpr_outline = NULL;
	mpr_renderInfo = NULL;
}

GlbGlobeObjectTypeEnum CGlbGlobeDrill::GetType()
{
	return GLB_OBJECTTYPE_DRILL;
}

glbBool CGlbGlobeDrill::Load(xmlNodePtr* node,glbWChar* prjPath)
{
	CGlbGlobeRObject::Load(node,prjPath);

	xmlChar* szKey = NULL;
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"RenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		xmlNodePtr rdchild = pnode->xmlChildrenNode;

		if (rdchild && (!xmlStrcmp(rdchild->name, (const xmlChar *)"RenderType")))
		{
			szKey = xmlNodeGetContent(rdchild);
			sscanf((char*)szKey,"%d",&rdtype);	
			xmlFree(szKey);

			rdchild = rdchild->next;
		}

		GlbDrillRenderInfo* renderInfo = new GlbDrillRenderInfo();
		renderInfo->Load(rdchild,prjPath);
		mpr_renderInfo = renderInfo;
		ParseObjectFadeColor();
	}
	return true;
}

glbBool CGlbGlobeDrill::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Save(node,prjPath);	

	xmlNodePtr rdnode = xmlNewNode(NULL,BAD_CAST "RenderInfo");
	xmlAddChild(node,rdnode);
	if(mpr_renderInfo)
	{
		char str[32];
		sprintf_s(str,"%d",mpr_renderInfo->type);
		xmlNewTextChild(rdnode, NULL, BAD_CAST "RenderType", BAD_CAST str);

		GlbDrillRenderInfo* renderInfo = dynamic_cast<GlbDrillRenderInfo*>(mpr_renderInfo.get());
		//if (renderInfo)		
		renderInfo->Save(rdnode,prjPath);				
	}
	return true;
}
glbDouble CGlbGlobeDrill::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
{
	if(isCompute == false)
		return mpr_distance;

	if (mpt_feature==NULL || mpt_globe==NULL)
		return DBL_MAX;

	glbref_ptr<CGlbExtent> ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
	if (ext==NULL)
		return DBL_MAX;

	if (mpt_isCameraAltitudeAsDistance)	
		mpr_distance = cameraPos.z();	
	else
	{
		glbDouble xOrLon,yOrLat,zOrAlt;
		ext->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
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

glbref_ptr<CGlbExtent> CGlbGlobeDrill::GetBound(glbBool isWorld)
{
	if (mpt_globe == NULL && isWorld) return NULL;

	if (mpt_feature==NULL || mpt_globe==NULL)
		return NULL;
	glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
	if (geoExt==NULL)
		return NULL;

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
			GlbGlobeTypeEnum globeType = mpt_globe->GetType();
			if (globeType==GLB_GLOBETYPE_GLOBE)
			{
				double x,y,z;
				double xMin,xMax,yMin,yMax,zMin,zMax;
				geoExt->Get(&xMin,&xMax,&yMin,&yMax,&zMin,&zMax);			
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMin),osg::DegreesToRadians(xMin),zMin,x,y,z);	
				worldExt->Merge(x,y,z);			
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMax),osg::DegreesToRadians(xMax),zMax,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMin),osg::DegreesToRadians(xMax),zMin,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMin),osg::DegreesToRadians(xMax),zMax,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMin),osg::DegreesToRadians(xMin),zMax,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMax),osg::DegreesToRadians(xMin),zMin,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMax),osg::DegreesToRadians(xMin),zMax,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMax),osg::DegreesToRadians(xMax),zMin,x,y,z);	
				worldExt->Merge(x,y,z);
			}
			else if (globeType==GLB_GLOBETYPE_FLAT)		
				worldExt = geoExt;	
		}
		return worldExt;
	}
	return NULL;
}

IGlbGeometry* CGlbGlobeDrill::GetOutLine()
{
	if (mpt_feature==NULL || mpt_globe==NULL)
		return NULL;

	if (mpr_outline==NULL)
	{
#if 0
		glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		if (geoExt==NULL)
			return NULL;
		glbDouble minX,maxX,minY,maxY;
		geoExt->Get(&minX,&maxX,&minY,&maxY);

		CGlbPolygon* poly = new CGlbPolygon();
		CGlbLine* extRing = new CGlbLine();
		extRing->AddPoint(minX,minY);
		extRing->AddPoint(maxX,minY);
		extRing->AddPoint(maxX,maxY);
		extRing->AddPoint(minX,maxY);

		poly->SetExtRing(extRing);

		CGlbMultiPolygon* mpoly = new CGlbMultiPolygon();
		mpoly->AddPolygon(poly);
		mpr_outline = mpoly;
#else
		glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		if (geoExt==NULL)
			return NULL;
		glbDouble cx,cy,cz;
		geoExt->GetCenter(&cx,&cy,&cz);
		CGlbPoint* pt = new CGlbPoint(cx,cy,cz);
		mpr_outline = pt;
#endif
	}
	return mpr_outline.get();
}

/*
*   mpt_node��mpt_preNode ���ɡ���ΪNULL �ڲ�ͬ�߳�
*   LoadData ֻ�� Dispatcher �̵߳���
*   
*   �޸�renderinfo,position ���������߳�
*/
void CGlbGlobeDrill::LoadData(glbInt32 level)
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
	glbref_ptr<GlbDrillRenderInfo> renderInfo = dynamic_cast<GlbDrillRenderInfo *>(mpr_renderInfo.get());
	if(renderInfo == NULL)
		return;
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
	glbref_ptr<CGlbGlobeDrillTask> task = new CGlbGlobeDrillTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobeDrill::ReadData(glbInt32 level,glbBool isDirect)
{
	//static bool first = true;
	//if(first == false)
	//	return;
	glbref_ptr<GlbDrillRenderInfo> renderInfo = dynamic_cast<GlbDrillRenderInfo*>(mpr_renderInfo.get());
	if( renderInfo == NULL || mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE )
	{mpt_loadState=false;return;}

	if (renderInfo == NULL) {mpt_loadState=false;return;}
	if (mpt_feature == NULL){mpt_loadState=false;return;}	
	CGlbDrill* drill = dynamic_cast<CGlbDrill*>(mpt_feature.get());
	if (drill==NULL) {mpt_loadState=false;return;}

	CGlbGlobeDrillSymbol symbol;
	osg::ref_ptr<osg::Node> node = symbol.Draw(this,NULL);
	if (node==NULL){mpt_loadState=false;return;}

	osg::ref_ptr<osg::Switch> swiNode = NULL;
	if(mpr_isAuto)
	{
		osg::Vec3d point;
		glbref_ptr<CGlbExtent> extent = GetBound(false);
		extent->GetCenter(&point.x(),&point.y(),&point.z());

		osg::Vec3d position;	
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
			osg::DegreesToRadians(point.x()),point.z(),position.x(),position.y(),position.z());
		osg::Matrix mTrans2;
		g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),mTrans2);

		swiNode = new osg::Switch;
		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		osg::Matrix mTrans=osg::Matrix::inverse(mTrans2);
		mt->setMatrix(mTrans);
		mt->addChild(node);

		osg::ref_ptr<GlbGlobeAutoDrill> autoTransform = new GlbGlobeAutoDrill;
		autoTransform->setAutoRotateMode(GlbGlobeAutoDrill::ROTATE_TO_SCREEN);
		autoTransform->setAutoScaleToScreen(true);
		autoTransform->setMinimumScale(0.0);
		autoTransform->setMaximumScale(FLT_MAX);
		autoTransform->addChild(mt);

		osg::ref_ptr<osg::MatrixTransform> mt2 = new osg::MatrixTransform;
		mt2->setMatrix(mTrans2);
		mt2->addChild(autoTransform);
		swiNode->addChild(mt2.get());
	}
	else
	{
		swiNode = node->asSwitch();
		if (swiNode==NULL){mpt_loadState=false;return;};
	}

	// Ĭ�Ϲرչ��գ�ʹ��drill�����������ɫ����
	//swiNode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

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

			{// ����͸����<100��״�� 2015.6.30 malin
				if (renderInfo->baselineOpacity)
				{
					glbInt32 blOpacity = renderInfo->baselineOpacity->GetValue(mpt_feature.get());
					if (blOpacity<100)
						DealDrillBaselineOpacity(renderInfo->baselineOpacity);
				}
				if (renderInfo->stratumOpacity)
				{
					glbInt32 stOpacity = renderInfo->stratumOpacity->GetValue(mpt_feature.get());
					if (stOpacity<100)
						DealDrillStratumOpacity(renderInfo->stratumOpacity);
				}	
			}
		}else{
			swiNode = NULL;
		}
	//}
	mpt_loadState = false;	

	//mpt_globe->GetView()->JumpTo(point.x(),point.y(),point.z(),1000,0,-90);
	//first = false;
}
/*
*     Dispatcher->AddToScene()->CGlbGlobePoint::AddToScene()  //Ƶ������
*     �ⲿ�߳�-->DirectDraw()->AddToScene() //Ƶ������
*/
void CGlbGlobeDrill::AddToScene()
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
glbInt32 CGlbGlobeDrill::RemoveFromScene(glbBool isClean)
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
/*
*    DirectDraw �� LoadData  Ҫ�����:��������
*    mpr_loadState��glbBool����Ϊ����ֿ�.
*    Dispatcher�߳�---�� AddToScene  ����.
*/
void CGlbGlobeDrill::DirectDraw(glbInt32 level)
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

glbBool CGlbGlobeDrill::SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_DRILL)  return false;
	GlbDrillRenderInfo *drillRenderInfo = dynamic_cast<GlbDrillRenderInfo *>(renderInfo);
	if (drillRenderInfo == NULL) return false;		
	{//��LoadData����.
		/*
		*   ������Ϊ�� ReadDataִ������;ʱ�����ɵĽ�����ܲ��ᷴӦ �����õ�����.
		*/
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			isNeedDirectDraw = true;
			mpr_renderInfo   = renderInfo;
			ParseObjectFadeColor();
			return true;			
		}
		mpt_loadState = true;
	}
	if(mpt_node == NULL)
	{		
		mpr_renderInfo   = renderInfo;
		ParseObjectFadeColor();
		mpt_loadState    = false;
		/*
			���isNeedDirectDraw=true�� ��ͼ����Ⱦ��renderer����SetRenderInfo�ӿڻᵼ��
			ͼ�������ж���Ҫ���أ����ͼ��������ܴ��ֱ�ӵ��±�����������
		*/
		//isNeedDirectDraw = true;
		return true;
	}
	
	glbBool rt = false;
	rt = DealDrillRenderInfo(drillRenderInfo,isNeedDirectDraw);
	if (rt)
	{
		mpr_renderInfo   = renderInfo;
		ParseObjectFadeColor();
	}
	mpt_loadState    = false;
	return rt;
}

glbBool CGlbGlobeDrill::SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (mode!=GLB_ALTITUDEMODE_ABSOLUTE)
		return false;
	return true;
}

GlbRenderInfo *CGlbGlobeDrill::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeDrill::SetShow(glbBool isShow,glbBool isOnState)
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

	glbref_ptr<GlbDrillRenderInfo>drillRenderInfo = dynamic_cast<GlbDrillRenderInfo *>(mpr_renderInfo.get());
	if (!drillRenderInfo) return false;

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

glbBool CGlbGlobeDrill::SetSelected(glbBool isSelected)
{
	if(mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DealSelected();
	}
	return true;
}

glbBool CGlbGlobeDrill::SetBlink(glbBool isBlink)
{
	if (mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

void CGlbGlobeDrill::DealSelected()
{
	glbref_ptr<GlbDrillRenderInfo> renderInfo = dynamic_cast<GlbDrillRenderInfo *>(mpr_renderInfo.get());
	if (!renderInfo) return;

	osg::ref_ptr<osg::StateSet> stateset = mpt_node->getOrCreateStateSet();	
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	if (mpt_isSelected)
	{
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));		
		//{
		//	material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
		//	material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
		//	material->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
		//	material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,0.1));
		//}
		stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	}
	else
	{
		if (material)
			stateset->removeAttribute(material);		
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
	}
}
void CGlbGlobeDrill::DealBlink()
{
	if(mpt_isBlink)
	{
		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
		}

		osg::ref_ptr<DrillObjectCallback> drillCallback = new DrillObjectCallback(this);
		mpt_node->addUpdateCallback(drillCallback);
	}
	else
	{
		// �رչ���
		//mpt_node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		osg::Material *material  = 
			dynamic_cast<osg::Material *>(mpt_node->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
		mpt_node->getOrCreateStateSet()->removeAttribute(material);
		mpt_node->getOrCreateStateSet()->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);

		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
			//ȡ����˸֮�󣬴���������ʾ
			DealSelected();
		}		
	}
}

glbInt32 CGlbGlobeDrill::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

glbBool CGlbGlobeDrill::Pick(glbInt32 mx,glbInt32 my,glbInt32& stratumId)
{
	if (mpt_node==NULL) return false;

	glbDouble _x = mx;
	glbDouble _y = my;

	CGlbGlobeView* globeView = GetGlobe()->GetView();
	globeView->WindowToScreen(_x,_y);

	osg::Vec3d vStart(_x,_y,0.0);
	osg::Vec3d vEnd(_x,_y,1.0);	
	globeView->ScreenToWorld(vStart.x(),vStart.y(),vStart.z());
	globeView->ScreenToWorld(vEnd.x(),vEnd.y(),vEnd.z());
	
	//����������
	osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(vStart, vEnd);
	osgUtil::IntersectionVisitor iv(picker.get());
	bool bFindStratum = false;
	mpt_node->accept(iv);
	if (picker->containsIntersections())
	{
		osgUtil::LineSegmentIntersector::Intersections& intersections = picker->getIntersections();		
		const osgUtil::LineSegmentIntersector::Intersection& hit = *(intersections.begin());
		//osg::Vec3d hitPt = hit.getWorldIntersectPoint();
		//double dis = (ln_pt1-hitPt).length();
		const osg::NodePath& nodePath = hit.nodePath;
		for(osg::NodePath::const_iterator nitr=nodePath.begin();
			nitr!=nodePath.end();
			++nitr)
		{
			osg::Switch* _switch = dynamic_cast<osg::Switch*>(*nitr);
			if (_switch){
				std::string ndName = _switch->getName();
				if (ndName.size()>7)
				{//"Stratum"
					std::string headStr = ndName.substr(0,7);					
					if (headStr=="Stratum")
					{
						std::string numStr = ndName.substr(7,ndName.length()-7);
						stratumId = atoi(numStr.c_str());
						//sscanf_s(ndName.c_str(),"Stratum%d",&stratumId);
						bFindStratum = true;
						break;
					}
				}
			}
		}				
	}	

	if (bFindStratum==false)
	{//���û�н��㣬����polytope��[������]
		// ����׶���󽻵ķ�������ȡ ������
		double xMin = _x - 3;
		double yMin = _y - 3;
		double xMax = _x + 3;
		double yMax = _y + 3;
		double zNear = 0.0;//window coord
		osg::Polytope _polytope;
		_polytope.add(osg::Plane(1.0, 0.0, 0.0, -xMin));
		_polytope.add(osg::Plane(-1.0,0.0 ,0.0, xMax));
		_polytope.add(osg::Plane(0.0, 1.0, 0.0,-yMin));
		_polytope.add(osg::Plane(0.0,-1.0,0.0, yMax));
		_polytope.add(osg::Plane(0.0,0.0,1.0, -zNear));

		osg::Polytope transformedPolytope;
		osg::Camera* p_camera = globeView->GetOsgCamera();
		osg::Matrix VPW = p_camera->getViewMatrix() *
			p_camera->getProjectionMatrix() *
			p_camera->getViewport()->computeWindowMatrix();	

		transformedPolytope.setAndTransformProvidingInverse(_polytope, VPW);
		// end ����polytope	

		osg::ref_ptr<osgUtil::PolytopeIntersector > picker = new osgUtil::PolytopeIntersector(_polytope);
		osgUtil::IntersectionVisitor iv(picker.get());

		mpt_node->accept(iv);
		if (picker->containsIntersections())
		{// ����Ҫ���Ǿ���
			const osg::NodePath& nodePath = picker->getFirstIntersection().nodePath;	
			for(osg::NodePath::const_iterator nitr=nodePath.begin();
				nitr!=nodePath.end();
				++nitr)
			{
				osg::Switch* _switch = dynamic_cast<osg::Switch*>(*nitr);
				if (_switch){
					std::string ndName = _switch->getName();
					if (ndName.size()>7)
					{//"Stratum"
						std::string headStr = ndName.substr(0,7);
						if (headStr=="Stratum")
						{
							std::string numStr = ndName.substr(7,ndName.length()-7);
							stratumId = atoi(numStr.c_str());
							//char n[7];
							//sscanf_s(ndName.c_str(),"%s%d",n,&stratumId);
							bFindStratum = true;
							break;
						}
					}
				}
			}		
		}		
	}
	return bFindStratum;
}
void CGlbGlobeDrill::SetStratumSelected(glbInt32 stratumId,glbBool isSelected)
{
	glbBool curSelected = false;
	std::map<glbInt32,glbBool>::iterator itr = mpr_stratumSelect.find(stratumId);
	if (itr==mpr_stratumSelect.end())
	{// û���ҵ�
		mpr_stratumSelect[stratumId] = isSelected;
	}
	else
		curSelected = itr->second;
	// ѡ��״̬û�б仯
	if (curSelected==isSelected)
		return;

	osg::Node* stratumNode = FindStratumNodeByID(stratumId);
	if (stratumNode==NULL) return;

	itr->second = isSelected;

	osg::ref_ptr<osg::StateSet> stateset = stratumNode->getOrCreateStateSet();	
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	if (isSelected)
	{
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));		
		stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	}
	else
	{
		if (material)
			stateset->removeAttribute(material);	
		//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
		//stratumNode->setStateSet(NULL);
	}
}
void CGlbGlobeDrill::SetStratumBlink(glbInt32 stratumId,glbBool isBlink)
{
	if (mpt_node==NULL) return;
	osg::Node* node = FindStratumNodeByID(stratumId);
	if (node==NULL)
		return;

	if(isBlink)
	{				
		osg::NodeCallback *callback = node->getUpdateCallback();
		if (callback)
		{
			node->removeUpdateCallback(callback);
		}

		osg::ref_ptr<DrillObjectCallback> drillCallback = new DrillObjectCallback(NULL,node);		
		node->addUpdateCallback(drillCallback);		
	}
	else
	{	
		// �رչ���
		//node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		osg::Material *material  = 
			dynamic_cast<osg::Material *>(node->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
		node->getOrCreateStateSet()->removeAttribute(material);
		node->getOrCreateStateSet()->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);

		osg::NodeCallback *callback = node->getUpdateCallback();
		if (callback)
		{
			node->removeUpdateCallback(callback);
			//ȡ����˸֮�󣬴���������ʾ
			std::map<glbInt32,glbBool>::iterator itr = mpr_stratumSelect.find(stratumId);
			if (itr!=mpr_stratumSelect.end())			
				SetStratumSelected(stratumId,itr->second);			
		}
	}
}

void CGlbGlobeDrill::SetStratumShow(glbInt32 stratumId,glbBool isShow)
{
	if (mpt_node==NULL) return;	
	if (mpt_node->asGroup()->getNumChildren()<1)	return;	
	osg::Group* mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if (mtNode==NULL) return;

	char strName[16];
	sprintf_s(strName,"Stratum%d",stratumId);
	std::string stratumName(strName);
	unsigned int numChild = mtNode->getNumChildren();
	for (unsigned int i = 0; i <numChild; i++)
	{
		osg::Node* node = mtNode->getChild(i);
		std::string name = node->getName();
		if (name == stratumName)
		{// �ҵ� 
/*			if(isShow != node->asSwitch()->getValue(0))			
				node->asSwitch()->setValue(i,isShow);*/		
			if (isShow)
				node->asSwitch()->setAllChildrenOn();
			else
				node->asSwitch()->setAllChildrenOff();
			break;
		}
	}	
}

glbInt32 CGlbGlobeDrill::ComputeNodeSize(osg::Node *node)
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

#include "GlbCompareValue.h"
glbBool CGlbGlobeDrill::DealDrillRenderInfo(GlbDrillRenderInfo* newDrillRenderInfo, glbBool &isNeedDirectDraw)
{
	GlbDrillRenderInfo *tempInfo = dynamic_cast<GlbDrillRenderInfo *>(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;
	//tempInfo���������е���Ⱦ��Ϣ
	//newDrillRenderInfo�������õ���Ⱦ��Ϣ
	if (tempInfo->isRenderBaseLine != newDrillRenderInfo->isRenderBaseLine || 
		tempInfo->isRenderStratum != newDrillRenderInfo->isRenderStratum )
	{// ��������߻�ز���ƿ��ر��޸�
		DealDrillBLOrStRenderOrNot(newDrillRenderInfo->isRenderBaseLine,newDrillRenderInfo->isRenderStratum);
		isNeedDirectDraw = false;
		return true;
	}

	if (tempInfo->isRenderBaseLine == true &&
		(!CGlbCompareValue::CompareValueStringEqual(tempInfo->baselineModelLocate,newDrillRenderInfo->baselineModelLocate,NULL) ||
		tempInfo->baselineModelLocate->bUseField != newDrillRenderInfo->baselineModelLocate->bUseField ||
		tempInfo->baselineModelLocate->field != newDrillRenderInfo->baselineModelLocate->field))
	{//ģ�͵�ַ���޸�.
		isNeedDirectDraw = true;
		return true;
	}

	if (tempInfo->isRenderStratum==true  &&
		(!CGlbCompareValue::CompareValueStringEqual(tempInfo->stratumModelLocate,newDrillRenderInfo->stratumModelLocate,NULL) ||
		tempInfo->stratumModelLocate->bUseField != newDrillRenderInfo->stratumModelLocate->bUseField ||
		tempInfo->stratumModelLocate->field != newDrillRenderInfo->stratumModelLocate->field))
	{//ģ�͵�ַ���޸�.
		isNeedDirectDraw = true;
		return true;
	}

	if ( tempInfo->isRenderBaseLine==true  &&		
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->baselineColor,newDrillRenderInfo->baselineColor,mpt_feature.get()))
	{// ��������ɫ�仯
		DealDrillBaselineColor(newDrillRenderInfo->baselineColor);
		isNeedDirectDraw=false;
		return true;
	}

	if (tempInfo->isRenderBaseLine==true  &&
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->baselineOpacity,newDrillRenderInfo->baselineOpacity,mpt_feature.get()))
	{// ������[ģ��]��͸���ȱ仯
		DealDrillBaselineOpacity(newDrillRenderInfo->baselineOpacity);
		isNeedDirectDraw = false;
		return true;
	}

	if ( tempInfo->isRenderBaseLine==true  &&		
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->baselineWidth,newDrillRenderInfo->baselineWidth,mpt_feature.get()))
	{// �����߿�ȷ����仯		
		DealDrillBaselineWidth(newDrillRenderInfo->baselineWidth);
		isNeedDirectDraw = false;
		return true;
	}

	if ( tempInfo->isRenderStratum==true  &&		
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->stratumColor,newDrillRenderInfo->stratumColor,mpt_feature.get()))
	{// �ز���ɫ�仯
		if (newDrillRenderInfo->stratumColor->bUseField && newDrillRenderInfo->stratumColor->field.length()>0)		
			isNeedDirectDraw = true;		
		else
		{
			DealDrillStratumColor(newDrillRenderInfo->stratumColor);
			isNeedDirectDraw = false;
		}

		return true;
	}

	if ( tempInfo->isRenderStratum==true  &&
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->stratumOpacity,newDrillRenderInfo->stratumOpacity,mpt_feature.get()))
	{// �ز㲻͸���ȱ仯
		if (newDrillRenderInfo->stratumOpacity->bUseField && newDrillRenderInfo->stratumOpacity->field.length()>0)		
			isNeedDirectDraw = true;		
		else
		{
			DealDrillStratumOpacity(newDrillRenderInfo->stratumOpacity);
			isNeedDirectDraw = false;
		}
		return true;
	}

	if (tempInfo->isRenderStratum==true  &&		
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->stratumWidth,newDrillRenderInfo->stratumWidth,mpt_feature.get()))
	{// �ز��߿�ȷ����仯
		//if (newDrillRenderInfo->stratumWidth->bUseField && newDrillRenderInfo->stratumWidth->field.length()>0)		
			isNeedDirectDraw = true;		
		//else
		//{
		//	DealDrillStratumWidth(newDrillRenderInfo->stratumWidth);
		//	isNeedDirectDraw = false;
		//}
		return true;
	}

	if(!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xOffset,newDrillRenderInfo->xOffset,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yOffset,newDrillRenderInfo->yOffset,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zOffset, newDrillRenderInfo->zOffset, mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xScale, newDrillRenderInfo->xScale, mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yScale, newDrillRenderInfo->yScale, mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zScale, newDrillRenderInfo->zScale, mpt_feature.get()))
	{
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(mpt_node->asGroup()->getChild(0));
		glbDouble xCenter,yCenter;
		mpt_feature->GetExtent()->GetCenter(&xCenter,&yCenter);

		osg::Matrix mOffset;

		glbDouble xOffset = 0;
		if(newDrillRenderInfo->xOffset)
			xOffset = newDrillRenderInfo->xOffset->GetValue(mpt_feature.get());
		glbDouble yOffset = 0;
		if (newDrillRenderInfo->yOffset)
			yOffset = newDrillRenderInfo->yOffset->GetValue(mpt_feature.get());
		glbDouble zOffset = 0;
		if (newDrillRenderInfo->zOffset)
			zOffset = newDrillRenderInfo->zOffset->GetValue(mpt_feature.get());

		osg::Matrix scaleMt;
		glbDouble xScale = (newDrillRenderInfo->xScale) ? (newDrillRenderInfo->xScale->GetValue(mpt_feature.get())) : 1.0;
		glbDouble yScale = (newDrillRenderInfo->yScale) ? (newDrillRenderInfo->yScale->GetValue(mpt_feature.get())) : 1.0;
		glbDouble zScale = (newDrillRenderInfo->zScale) ? (newDrillRenderInfo->zScale->GetValue(mpt_feature.get())) : 1.0;
		scaleMt.makeScale(xScale, yScale, zScale);

		if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
		{
			glbDouble xCenter, yCenter, zCenter;
			mpt_feature->GetExtent()->GetCenter(&xCenter, &yCenter, &zCenter);
			
			osg::Matrix localToWorld;
			localToWorld.makeTranslate(xCenter, yCenter, zCenter);

			scaleMt.makeScale(xScale, yScale, zScale);
			mOffset.makeTranslate(xOffset, yOffset, zOffset);
			mt->setMatrix(osg::Matrix::inverse(localToWorld)* scaleMt *  mOffset * localToWorld);

		}
		else
		{
			glbDouble xCenter, yCenter, zCenter;
			mpt_feature->GetExtent()->GetCenter(&xCenter, &yCenter, &zCenter);

			double longitude = osg::DegreesToRadians(xCenter);
			double latitude = osg::DegreesToRadians(yCenter);

			osg::Vec3d localOrigin;//�ֲ�ԭ��
			g_ellipsoidModel->convertLatLongHeightToXYZ(latitude, longitude, zCenter, localOrigin.x(), localOrigin.y(), localOrigin.z());

			osg::Matrix localToWorld;
			g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(localOrigin.x(), localOrigin.y(), localOrigin.z(), localToWorld);

			scaleMt.makeScale(xScale, yScale, zScale);
			mOffset.makeTranslate(xOffset, yOffset, zOffset);
			//�ֲ�ƽ������ �ٻ���������ϵ��
			mt->setMatrix(osg::Matrix::inverse(localToWorld)* scaleMt *  mOffset * localToWorld);
		}
		
	}

	return true;
}

osg::Node* CGlbGlobeDrill::FindStratumNodeByID(glbInt32 stratumId)
{
	if (mpt_node==NULL || mpt_node->asGroup()->getNumChildren()<1) return NULL;
	osg::Group* mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if (mtNode==NULL) return NULL;

	char strName[16];
	sprintf_s(strName,"Stratum%d",stratumId);
	std::string stratumName(strName);
	unsigned int numChild = mtNode->getNumChildren();
	for (unsigned int i = 0; i <numChild; i++)
	{
		osg::Node* node = mtNode->getChild(i);
		std::string name = node->getName();
		if (name == stratumName)
		{// �ҵ� 				
			return node;
		}
	}
	return NULL;
}

osg::Node* CGlbGlobeDrill::FindBaselineNode()
{
	if (mpt_node==NULL || mpt_node->asGroup()->getNumChildren()<1) return NULL;
	osg::Group* mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if (mtNode==NULL) return NULL;
	unsigned int numChild = mtNode->getNumChildren();
	for (unsigned int i = 0; i <numChild; i++)
	{
		osg::Node* node = mtNode->getChild(i);
		std::string name = node->getName();
		if (name == "BaseLine")
		{// �ҵ� 				
			return node;
		}
	}
	return NULL;
}

void CGlbGlobeDrill::DealDrillBaselineColor(GlbRenderColor* clr)
{
	osg::Node* blNode = FindBaselineNode();
	if (blNode==NULL || clr==NULL) return;
	glbInt32 color = clr->GetValue(mpt_feature.get());

	glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
	glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
	glbInt32 blue = LOBYTE(color);//GetRValue(color);
	CGlbColorVisitor nodeVisiter(red,green,blue);
	blNode->accept(nodeVisiter);	
}

#include "GlbOpacityVisitor.h"
void CGlbGlobeDrill::DealDrillBaselineOpacity(GlbRenderInt32* opacity)
{
	osg::Node* blNode = FindBaselineNode();
	if (blNode==NULL || opacity==NULL) return;
	glbInt32 opt = opacity->GetValue(mpt_feature.get());
	
	CGlbOpacityVisitor nodeVisiter(opt);
	blNode->accept(nodeVisiter);

	osg::StateSet* ss = blNode->getOrCreateStateSet();
	//����Ƿ�����
	if (opt<100)
	{// ���û��
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);
		ss->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
		if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );		
		//ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	}
	else
	{// ���û��						
		ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);			
		if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
	}	
}
#include <osg/LineWidth>
void CGlbGlobeDrill::DealDrillBaselineWidth(GlbRenderDouble* wid)
{
	osg::Node* blNode = FindBaselineNode();
	if (blNode==NULL || wid==NULL)	return;

	CGlbFeature* feature = GetFeature();
	glbDouble blWidth = wid->GetValue(feature);

	//�߿�
	osg::LineWidth *lineWidth = new osg::LineWidth;
	lineWidth->setWidth(blWidth);
	osg::StateSet* ss = blNode->getOrCreateStateSet();
	ss->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
}

void CGlbGlobeDrill::DealDrillStratumColor(GlbRenderColor* clr)
{
	if (mpt_node==NULL || clr==NULL)			return;
	if (mpt_node->asGroup()->getNumChildren()<1)	return;	
	osg::Group* mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if (mtNode==NULL) return;

	glbInt32 color = clr->GetValue(mpt_feature.get());
	glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
	glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
	glbInt32 blue = LOBYTE(color);//GetRValue(color);
	size_t cnt = mtNode->getNumChildren();
	for (size_t i = 0; i < cnt; i++)
	{
		osg::Switch* swt = mtNode->getChild(i)->asSwitch();
		if (swt)
		{
			if (swt->getName()!="BaseLine")
			{
				CGlbColorVisitor nodeVisiter(red,green,blue);
				swt->accept(nodeVisiter);				
			}			
		}
	}
}

void CGlbGlobeDrill::DealDrillStratumOpacity(GlbRenderInt32* opacity)
{
	if (mpt_node==NULL || opacity==NULL)			return;
	if (mpt_node->asGroup()->getNumChildren()<1)	return;	
	osg::Group* mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if (mtNode==NULL) return;

	glbInt32 opt = opacity->GetValue(mpt_feature.get());
	size_t cnt = mtNode->getNumChildren();
	for (size_t i = 0; i < cnt; i++)
	{ 
		osg::Switch* swt = mtNode->getChild(i)->asSwitch();
		if (swt)
		{
			if (swt->getName()!="BaseLine")
			{
				CGlbOpacityVisitor nodeVisiter(opt);
				swt->accept(nodeVisiter);

				osg::StateSet* ss = swt->getOrCreateStateSet();
				//����Ƿ�����
				if (opt<100)
				{// ���û��
					ss->setMode(GL_BLEND, osg::StateAttribute::ON);
					ss->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
					if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
						ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );							
				}
				else
				{// ���û��						
					ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);			
					if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
						ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
				}
			}			
		}
	}
}

void CGlbGlobeDrill::DealDrillStratumWidth(GlbRenderDouble* wid)
{// ���ֶν������޸ģ�ҪӦ�õ����еĵز�drawable�ϣ������� ƥ�������÷Ѿ�ѽ������	
	if (mpt_node==NULL || wid==NULL)			return;
	if (mpt_node->asGroup()->getNumChildren()<1)	return;	
	osg::Group* mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if (mtNode==NULL) return;

	CGlbFeature* feature = GetFeature();
	glbDouble blWidth = wid->GetValue(feature);
	//�߿�
	osg::LineWidth *lineWidth = new osg::LineWidth;
	lineWidth->setWidth(blWidth);
	
	size_t cnt = mtNode->getNumChildren();
	for (size_t i = 0; i < cnt; i++)
	{
		osg::Switch* swt = mtNode->getChild(i)->asSwitch();
		if (swt)
		{
			if (swt->getName()!="BaseLine")
			{// ��������
				osg::StateSet* ss = swt->getOrCreateStateSet();				
				ss->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
			}			
		}
	}
}

void CGlbGlobeDrill::DealDrillBLOrStRenderOrNot(bool renderBaseline, bool renderStratum)
{
	if (mpt_node==NULL)		return;
	if (mpt_node->asGroup()->getNumChildren()<1)	return;	
	osg::Group* mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if (mtNode==NULL) return;

	size_t cnt = mtNode->getNumChildren();
	for (size_t i = 0; i < cnt; i++)
	{
		osg::Switch* swt = mtNode->getChild(i)->asSwitch();
		if (swt)
		{
			if (swt->getName()=="BaseLine")
			{
				if (renderBaseline)
					swt->setAllChildrenOn();
				else
					swt->setAllChildrenOff();
			}
			else
			{
				if (renderStratum)
					swt->setAllChildrenOn();
				else
					swt->setAllChildrenOff();
			}
		}
	}
}

void CGlbGlobeDrill::SetStratumAuto( glbBool isAuto )
{
	if(mpr_isAuto == isAuto)
		return;
	mpr_isAuto = isAuto;
	DirectDraw(0);
}

//����ֱ����OperatorVisitor���˺������ڽ���endAlpha��ֵ
void CGlbGlobeDrill::ParseObjectFadeColor()
{
	if(!mpr_renderInfo.valid()) return;
	GlbDrillRenderInfo *drillRenderInfo = dynamic_cast<GlbDrillRenderInfo *>(mpr_renderInfo.get());
	if (drillRenderInfo == NULL) return;

	if (drillRenderInfo->isRenderStratum)
	{
		//mpt_fadeColor = GetColor(drillRenderInfo->stratumColor->GetValue(mpt_feature.get()));
		mpt_fadeEndAlpha = drillRenderInfo->stratumOpacity->GetValue(mpt_feature.get());
	}
	else if(drillRenderInfo->isRenderBaseLine)
	{
		//mpt_fadeColor = GetColor(drillRenderInfo->baselineColor->GetValue(mpt_feature.get()));
		mpt_fadeEndAlpha = drillRenderInfo->baselineOpacity->GetValue(mpt_feature.get());
	}
}

glbBool GlbGlobe::CGlbGlobeDrill::Load2( xmlNodePtr* node,glbWChar* prjPath )
{
	return true;
}
