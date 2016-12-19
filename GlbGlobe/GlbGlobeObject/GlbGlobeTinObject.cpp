#include "StdAfx.h"
#include "GlbGlobeTinObject.h"

#include "CGlbGlobe.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbCalculateBoundBoxVisitor.h"
#include "GlbGlobeSymbolCommon.h"
#include "GlbGlobePolygonWaterSymbol.h"

#include "GlbGlobeTinSymbol.h"

#include "GlbTin.h"
#include "IGlbGeometry.h"
#include "GlbExtent.h"
#include "GlbString.h"
#include "GlbGlobeMemCtrl.h"

#include "osg/Material"
#include "GlbOpacityVisitor.h"
#include "GlbCompareValue.h"
#include "osgDB/ReadFile"
#include "GlbFadeInOutCallback.h"
#include "osg/CullFace"

#include <osgUtil/LineSegmentIntersector>


using namespace GlbGlobe;

//#define  _TEST_INT_GRID //������ʾ����
#ifdef _TEST_INT_GRID
osg::ref_ptr<osg::Node>  testNode = NULL;
#endif


CGlbGlobeTinObject::CGlbGlobeTinLoadDataTask::CGlbGlobeTinLoadDataTask( CGlbGlobeTinObject *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobeTinObject::CGlbGlobeTinLoadDataTask::~CGlbGlobeTinLoadDataTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeTinObject::CGlbGlobeTinLoadDataTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeTinObject::CGlbGlobeTinLoadDataTask::getObject(){return mpr_obj.get();}
//////////////////////////////////////////////////////////////////////////
CGlbGlobeTinObject::CGlbGlobeTinObject(void)
{
	mpr_readData_critical.SetName(L"tin_readdata");
	mpr_addToScene_critical.SetName(L"tin_addscene");
	mpr_distance = 0.0;
	mpr_outline = NULL;
	mpr_renderInfo = NULL;
	//mpr_altitude = 0.0;

	mpr_elevation=DBL_MAX;
	mpt_maxVisibleDistance = 15000000.0;
	mpr_tinGeo=NULL;
	mpr_objSize=0;
	mpr_TexRotation=0.0f;
	mpr_needReReadData=false;	
	mpt_isLODEnable = false;
}

CGlbGlobeTinObject::~CGlbGlobeTinObject(void)
{
}

glbBool CGlbGlobeTinObject::SetAltitudeMode( GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw )
{
	return false;
}

void CGlbGlobeTinObject::SetRenderOrder( glbInt32 order )
{
	if (mpt_renderOrder == order) 
		return;
	mpt_renderOrder = order;
	if (mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DirtyOnTerrainObject();
	}
	CGlbGlobeREObject::SetRenderOrder(order);
}

glbDouble CGlbGlobeTinObject::GetDistance( osg::Vec3d &cameraPos,glbBool isCompute )
{
	if (!isCompute)
		return mpr_distance;

	if (!mpt_globe)	return DBL_MAX;

	if (mpt_isCameraAltitudeAsDistance)	
		mpr_distance = cameraPos.z();	
	else
	{
		osg::Vec3d position;
		if (mpt_feature)
		{
			CGlbExtent* ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
			ext->GetCenter(&position.x(),&position.y(),&position.z());	
		}else{
			if (mpr_tinGeo == NULL)
				return DBL_MAX;
			if (mpr_tinGeo->GetType()==GLB_GEO_POINT)
			{
				glbref_ptr<CGlbExtent> polyExtent = const_cast<CGlbExtent *>(mpr_tinGeo->GetExtent());
				polyExtent->GetCenter(&position.x(),&position.y(),&position.z());
			}
			else
			{
				CGlbExtent* polyExtent = const_cast<CGlbExtent *>(mpr_tinGeo->GetExtent());
				polyExtent->GetCenter(&position.x(),&position.y(),&position.z());
			}				
		}
		ComputePosByAltitudeAndGloleType(position);
		osg::Vec3d cameraPoint = cameraPos;
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),
			osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());	
		mpr_distance = (position - cameraPoint).length();
	}
	return mpr_distance;
}

void CGlbGlobeTinObject::LoadData( glbInt32 level )
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
	glbref_ptr<GlbTinSymbolInfo> tinSymbolInfo = dynamic_cast<GlbTinSymbolInfo *>(mpr_renderInfo.get());
	if(tinSymbolInfo == NULL)
		return;
	// ��3Dģ�����ͣ����Ѿ�load�����Ͳ����ٴ�load  -------> ���ֶ�����ж�level������
	if (/*polygonSymbol->type != GLB_OBJECTTYPE_POLYGON &&*/ mpt_currLevel != -1) 	
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
	glbref_ptr<CGlbGlobeTinLoadDataTask> task = new CGlbGlobeTinLoadDataTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobeTinObject::AddToScene()
{
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)return;
	/*
	*   mpt_isInHangTasks = true ------ ���� AddToScene���߳� (A)
	*   mpt_isInHangTasks = false ----- frame �߳�            (B)
	*   
	*/
	if(mpt_node == NULL)return;
	if(true)//mpt_isFaded
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
			&& mpt_node->getNumParents() == 0)
		{
			//��mpt_node����
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			
			//mpt_globe->mpr_p_callback->AddHangTask(new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_globe->mpr_root,mpr_MeshBox.testNode()));

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

glbInt32 CGlbGlobeTinObject::RemoveFromScene( glbBool isClean )
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
	//����������	
	if(    mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
	{		
		DirtyOnTerrainObject();
	}
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

glbref_ptr<CGlbExtent> CGlbGlobeTinObject::GetBound( glbBool isWorld /*= true */ )
{
	if (mpt_globe == NULL && isWorld) return NULL;
	glbref_ptr<CGlbExtent> geoExt = NULL;
	if(    mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN
		&& mpr_outline != NULL)
	{//�����λ���ģʽ
		/*
		*   ��������ѯ��qtree������׼ȷ. z == 0.
		*/
		geoExt = const_cast<CGlbExtent *>(mpr_outline->GetExtent());
	}else{
		if(mpt_feature)		
			geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());		
		else if(mpr_tinGeo)		
			geoExt = const_cast<CGlbExtent *>(mpr_tinGeo->GetExtent());				
	}
	if(isWorld == false)
	{//��������
		/*
		�������ʱ���������ζ���,�ڳ����������õĵ�������!!!!
		�����λ��ƣ�GetDistance���õ�mpr_elevate.
		���ԣ�
		����UpdateElevate().
		*/
		if( mpr_elevation == DBL_MAX
			&& mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
			UpdateElevate();
		/*
		*   �������꣬����Ӧ��Ե��λ��Ƶĵ��κ���.
		*/
		return geoExt;
	}
	else
	{//��������		
		if(geoExt == NULL)return NULL;
		glbref_ptr<CGlbExtent> worldExt = new CGlbExtent();
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();
		if(mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
		{//�����λ���
			double x,y,z;
			geoExt->GetMin(&x,&y,&z);
			z = mpt_globe->GetElevationAt(x,y);
			if(globeType == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
					osg::DegreesToRadians(x),z,x,y,z);
			}
			worldExt->Merge(x,y,z);
			geoExt->GetMax(&x,&y,&z);
			z = mpt_globe->GetElevationAt(x,y);
			if(globeType == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
					osg::DegreesToRadians(x),z,x,y,z);
			}
			worldExt->Merge(x,y,z);			
		}
		else
		{//��������.
			osg::ref_ptr<osg::Node> node = mpt_node;
			if(   node != NULL
				&&node->getBound().valid() )
			{
				//osg::ref_ptr<osg::Node> node = mpt_node;
				CGlbCalculateBoundBoxVisitor bboxV;
				node->accept(bboxV);
				osg::BoundingBoxd bb =bboxV.getBoundBox();
				worldExt->SetMin(bb.xMin(),bb.yMin(),bb.zMin());
				worldExt->SetMax(bb.xMax(),bb.yMax(),bb.zMax());
			}
			else
			{//osg �ڵ㻹û����.				
				double x,y,z;
				geoExt->GetMin(&x,&y,&z);
				if( mpr_elevation == DBL_MAX)
					UpdateElevate();
				if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				{
					z += mpr_elevation;
				}
				if(globeType == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
						osg::DegreesToRadians(x),z,x,y,z);
				}
				worldExt->Merge(x,y,z);

				geoExt->GetMax(&x,&y,&z);
				if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				{					
					z += mpr_elevation;
				}
				if(globeType == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
						osg::DegreesToRadians(x),z,x,y,z);
				}
				worldExt->Merge(x,y,z);
			}
		}//������ģʽ
		return worldExt;
	}//��������	
	return NULL;
}

void CGlbGlobeTinObject::UpdateElevate()
{
	if (mpt_globe==NULL)	return;
	if(mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE)//ֻ���Ǿ��Ե���
		return;

	glbDouble elevation = 0.0;
	CGlbExtent* ext = NULL;
	if (mpt_feature)
		ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());	
	else if (mpr_tinGeo)
		ext = const_cast<CGlbExtent *>(mpr_tinGeo->GetExtent());

	if (!ext)
		return;

	glbDouble xOrLon,yOrLat;
	ext->GetCenter(&xOrLon,&yOrLat);
	elevation = mpt_globe->GetElevationAt(xOrLon,yOrLat);	

	if(fabs(elevation - mpr_elevation) < 0.0001)
		return;

	mpr_elevation=elevation;
	//����Maxtrixform
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)//����ִ�д˹���
		DealModePosByChangeAltitudeOrChangePos();
}

IGlbGeometry * CGlbGlobeTinObject::GetOutLine()
{
	if (mpr_outline!=NULL)
		return mpr_outline.get();	
	if(mpt_feature!=NULL)
	{
		CGlbExtent *pExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		if (pExt)
		{
			glbDouble xx,yy,zz;
			pExt->GetCenter(&xx,&yy,&zz);
			CGlbPoint *outLint=new CGlbPoint(xx,yy,zz);
			mpr_outline=outLint;
		}
	}
	else if (mpr_tinGeo!=NULL)
	{
		CGlbExtent *pExt = const_cast<CGlbExtent *>(mpr_tinGeo->GetExtent());
		if (pExt)
		{
			glbDouble xx,yy,zz;
			pExt->GetCenter(&xx,&yy,&zz);
			CGlbPoint *outLint=new CGlbPoint(xx,yy,zz);
			mpr_outline=outLint;
		}
		//return mpr_tinGeo;
	}
	return mpr_outline.get();
}

glbBool CGlbGlobeTinObject::SetRenderInfo( GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw )
{
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_TIN)
		return false;
	GlbTinSymbolInfo *tinInfo = static_cast<GlbTinSymbolInfo *>(renderInfo);
	if (tinInfo == NULL) return false;		
	isNeedDirectDraw = false;
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
		return true;
	}
	GlbTinSymbolInfo *tempInfo = dynamic_cast<GlbTinSymbolInfo *>(mpr_renderInfo.get());
	// ���ͱ仯��ֱ���ػ�
	if (tinInfo->symType != tempInfo->symType)
	{
		isNeedDirectDraw = true;
		mpr_renderInfo   = renderInfo;
		ParseObjectFadeColor();
		mpt_loadState = false;
		return true;
	}

	if (tinInfo->color != NULL && tempInfo->color != NULL)
	{
		if(!CGlbCompareValue::CompareValueColorEqual(tinInfo->color,tempInfo->color,mpt_feature.get()) ||
			tinInfo->color->bUseField != tempInfo->color->bUseField ||
			tinInfo->color->field != tempInfo->color->field)
		{
			isNeedDirectDraw = true;
			mpr_renderInfo   = renderInfo;
			mpt_loadState = false;
			return true;
		}
	}
		
	if (tinInfo->locate != NULL && tempInfo->locate != NULL)
	{
		if(!CGlbCompareValue::CompareValueStringEqual(tinInfo->locate,tempInfo->locate,mpt_feature.get()) ||
			tinInfo->locate->bUseField != tempInfo->locate->bUseField ||
			tinInfo->locate->field != tempInfo->locate->field)
		{
			isNeedDirectDraw = true;
			mpr_renderInfo   = renderInfo;
			mpt_loadState = false;
			ParseObjectFadeColor();
			return true;
		}
	}		

	if (tinInfo->bSmoothing != NULL && tempInfo->bSmoothing != NULL)
	{// �Ƿ�ƽ������
		if(!CGlbCompareValue::CompareValueBoolEqual(tinInfo->bSmoothing,tempInfo->bSmoothing,mpt_feature.get()) ||
			tinInfo->bSmoothing->bUseField != tempInfo->bSmoothing->bUseField ||
			tinInfo->bSmoothing->field != tempInfo->bSmoothing->field)
		{
			isNeedDirectDraw = true;
			mpr_renderInfo   = renderInfo;
			mpt_loadState = false;
			return true;
		}
	}
		
	if (tinInfo->zScale != NULL && tempInfo->zScale != NULL)
	{// ���ű���
		glbDouble oldZScale = tempInfo->zScale->GetValue(mpt_feature.get());
		glbDouble newZScale = tinInfo->zScale->GetValue(mpt_feature.get());
		CGlbWString locate = L"";
		if(tinInfo && tinInfo->locate)
			locate = tinInfo->locate->GetValue(mpt_feature.get());

		if (oldZScale != newZScale							// zScale�����仯
			&& mpt_globe->GetType()==GLB_GLOBETYPE_GLOBE)	// ����ģʽ
			//&& locate.empty())							// �������ݷ���Դ���ļ�-2016.1.11֧���ļ���ʽ ����
		{// ����ϵ���仯ֱ���ػ�
			isNeedDirectDraw = true;
			mpr_renderInfo   = renderInfo;
			mpt_loadState = false;
			ParseObjectFadeColor();
			return true;
		}
	}


	glbBool rt = false;
	switch(tinInfo->symType)
	{
	case GLB_TINSYMBOL_SIMPLE:
		{
			osg::MatrixTransform *tinContainer=(osg::MatrixTransform *)mpt_node->asSwitch()->getChild(0)->asTransform();
			osg::StateSet *ss=tinContainer->getOrCreateStateSet();
			rt = DealTinModelSymbol(tinContainer,mpt_currLevel,tinInfo,isNeedDirectDraw);
		}
		break;		
	case GLB_TINSYMBOL_WATER:
		{
			GlbTinWaterSymbolInfo *tinWaterInfo = dynamic_cast<GlbTinWaterSymbolInfo *>(renderInfo);
			if(tinWaterInfo != NULL)
				rt = judgeTinWaterNeedDirectDraw(tinWaterInfo,isNeedDirectDraw);
		}
		break;
	}		

	if(rt)
	{
		mpr_renderInfo = renderInfo;
		ParseObjectFadeColor();

		/*�������÷ָ�ľ��������*/
		if(tinInfo->symType == GLB_TINSYMBOL_SIMPLE)
		{
			osg::MatrixTransform *tinContainer=(osg::MatrixTransform *)mpt_node->asSwitch()->getChild(0)->asTransform();
			updateMeshBox(tinContainer->getMatrix(),tinInfo,mpt_feature,(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE));

#ifdef _TEST_INT_GRID

			if(testNode != NULL)
			{
				mpt_globe->mpr_p_callback->AddRemoveTask(new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_globe->mpr_root,testNode));
			}

			testNode = mpr_MeshBox.testShowGridNode();
			mpt_globe->mpr_p_callback->AddHangTask(new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_globe->mpr_root,testNode));

			mpt_HangTaskNum++;
#endif

		}
	}
	mpt_loadState  = false;
	return rt;
}

GlbRenderInfo * CGlbGlobeTinObject::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeTinObject::SetShow( glbBool isShow,glbBool isOnState/*=false */ )
{
	if (mpt_node==NULL && isOnState) return false;

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

	glbref_ptr<GlbTinSymbolInfo> tinSymbolInfo = dynamic_cast<GlbTinSymbolInfo *>(mpr_renderInfo.get());
	if (tinSymbolInfo==NULL)	return false;

	//if( mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
	//{// ��������ػ��ƶ�����Ҫˢ����������
	//	/*
	//	* mpr_isCanDrawImage������DrawToImageʱ�Ƿ���Ҫ����
	//	*/
	//	if (mpr_isCanDrawImage != isnew)
	//	{
	//		mpr_isCanDrawImage = isnew;
	//		// ˢ����������
	//		DirtyOnTerrainObject();		
	//	}
	//}
	//else
	if (mpt_node)
	{ 
		if(isnew)
		{
			mpt_node->asSwitch()->setAllChildrenOn();
			if(true)//mpt_isFaded
			{
				osg::ref_ptr<CGlbFadeInOutCallback> fadeCb = 
					new CGlbFadeInOutCallback(mpt_fadeStartAlpha,mpt_fadeEndAlpha,this,mpt_fadeDurationTime,mpt_fadeColor);
				mpt_node->addUpdateCallback(fadeCb.get());
			}
		}
		else
			mpt_node->asSwitch()->setAllChildrenOff();

	}

	return true;
}

glbBool CGlbGlobeTinObject::SetSelected( glbBool isSelected )
{
	if (mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL)
	{
		if(GLB_ALTITUDEMODE_ONTERRAIN == mpt_altitudeMode)
			DirtyOnTerrainObject();		
		else
			DealSelected();
	}
	return true;
}

glbBool CGlbGlobeTinObject::SetBlink( glbBool isBlink )
{
	if (mpt_isBlink == isBlink) 
		return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

void CGlbGlobeTinObject::DirectDraw( glbInt32 level )
{
	if (mpt_parent == NULL) return;	
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

void CGlbGlobeTinObject::DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext )
{
	return;
	throw std::exception("The method or operation is not implemented.");
}

glbInt32 CGlbGlobeTinObject::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

glbBool CGlbGlobeTinObject::Load( xmlNodePtr* node,const glbWChar* prjPath )
{
	CGlbGlobeRObject::Load(node,prjPath);

	xmlChar *szKey = NULL;
	xmlNodePtr pnode = *node;

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"RenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		xmlNodePtr rdchild = pnode->xmlChildrenNode;

		if(rdchild && (!xmlStrcmp(rdchild->name,(const xmlChar *)"RenderType")))
		{
			szKey = xmlNodeGetContent(rdchild);
			sscanf((char*)szKey,"%d",&rdtype);
			xmlFree(szKey);

			rdchild = rdchild->next;
		}

		glbref_ptr<GlbTinSymbolInfo> renderInfo = new GlbTinSymbolInfo();
		renderInfo->Load(rdchild,prjPath);
		mpr_renderInfo = renderInfo;
		ParseObjectFadeColor();

		CGlbWString datalocate = renderInfo->locate->value;
		glbInt32 index = datalocate.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			datalocate = execDir + datalocate.substr(1,datalocate.size());
		}
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(datalocate.ToString());
		if(node.valid() && node->getBound().valid())
		{
			glbDouble x = node->getBound().center().x();
			glbDouble y = node->getBound().center().y();
			glbDouble z = node->getBound().center().z();
			glbDouble lon = 0.0,lat = 0.0,alt = 0.0;
			g_ellipsoidModel->convertXYZToLatLongHeight(x,y,z,lat,lon,alt);
			glbref_ptr<CGlbPoint> point = new CGlbPoint(
				osg::RadiansToDegrees(lon),osg::RadiansToDegrees(lat),alt);
			mpr_tinGeo = point;
			return true;
		}
	}
	return false;
	//return false;
	//throw std::exception("The method or operation is not implemented.");
}

glbBool CGlbGlobeTinObject::Save( xmlNodePtr node,const glbWChar* prjPath )
{
	CGlbGlobeRObject::Save(node,prjPath);
	xmlNodePtr rdnode = xmlNewNode(NULL,BAD_CAST "RenderInfo");
	xmlAddChild(node,rdnode);
	if(mpr_renderInfo)
	{
		char str[32];
		sprintf_s(str,"%d",mpr_renderInfo->type);
		xmlNewTextChild(rdnode,NULL,BAD_CAST "RenderType",BAD_CAST str);

		GlbTinSymbolInfo *renderInfo = dynamic_cast<GlbTinSymbolInfo*>(mpr_renderInfo.get());
		if(renderInfo)
			renderInfo->Save(rdnode,prjPath);
	}
	return true;
	//return false;
	//throw std::exception("The method or operation is not implemented.");
}

glbBool CGlbGlobeTinObject::SetEdit( glbBool isEdit )
{
	return false;
	throw std::exception("The method or operation is not implemented.");
}

GlbGlobe::GlbGlobeObjectTypeEnum CGlbGlobeTinObject::GetType(){return GLB_OBJECTTYPE_TIN;}

glbBool CGlbGlobeTinObject::SetGeo( IGlbGeometry *tinGeo,glbBool &isNeedDirectDraw )
{
	if (tinGeo==NULL)
		return false;

	isNeedDirectDraw = false;
	if(mpt_feature)
	{// �����Ҫ�أ�λ����Ϣ��Դ��Ҫ��.
		mpr_tinGeo = tinGeo;
		return true;
	}

	{   		
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			*  �������ReadData��ReadData�������µĵ���ģʽ֮ǰʹ���˾ɵ�λ��.
			*  ���ԣ�
			��Ҫ��֪�����ߣ�����DirectDraw.
			*/
			mpr_tinGeo = tinGeo;
			isNeedDirectDraw = true;						
			return true;
		}
		mpt_loadState = true;
	}

	mpr_tinGeo = tinGeo;
	if(mpt_globe)
	{
		/*
		���mpr_polygonGeo==NULL,�����ǲ��ڳ������ϵ�.
		�����UpdateObject��������Զ���ڳ������ϣ�������Զ������ȸö���.
		*/
		mpt_globe->UpdateObject(this);
	}
	if (mpt_node       != NULL)
	{// lineGeo�ı�---->�ػ�
		/*
		mpt_node == NULL,Ҳ����Ҫ���»��Ƶ�!!!.
		*/
		isNeedDirectDraw = true;
		/*
		��Ϊ�� ����ⲿ����û����᣺isNeedDirectDraw
		�ң������levelҲû�з����仯�����,����Ҳ�������osgNode
		���ԣ�mpt_currLevel = -1��ʹ���ȿ���LoadData.

		*/
		mpt_currLevel = -1;
	}
	mpt_loadState = false;
	return true;
}
//////////////////////////////////////////////////////////////////////////
void CGlbGlobeTinObject::ReadData( glbInt32 level ,glbBool isDirect/*=false*/ )
{
	glbref_ptr<GlbTinSymbolInfo> tinSymbolInfo = dynamic_cast<GlbTinSymbolInfo *>(mpr_renderInfo.get());
	if (NULL==tinSymbolInfo || (mpt_feature==NULL&&mpr_tinGeo==NULL))	
	{mpt_loadState=false;return;}

	// ˢ�¾ɵ�����
	if(	mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )//����ģʽ������ִ�д˹���
		DirtyOnTerrainObject();

	osg::MatrixTransform *tinContainer=NULL;
	//osg::Group *tinContainer=NULL;
	if (mpt_feature)
	{// feature
		IGlbGeometry *geo=NULL;
		glbBool result = false;
		result = mpt_feature->GetGeometry(&geo,level);
		if(!result || geo == NULL)  {mpt_loadState=false;return;}			

		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)//ֻ���Ǿ���ģʽ������ִ�д˹���
		{// ����
			// ˢ���µ�����
			DirtyOnTerrainObject();
		} 
		else
		{// ������
			osg::Node *tinNode=NULL;
			switch(tinSymbolInfo->symType)
			{
			case GLB_TINSYMBOL_SIMPLE:
				{
					CGlbGlobeTinSymbol _tinSymbol;
					// ����outline
					mpr_outline = _tinSymbol.GetOutline(this,mpr_tinGeo.get());

					tinNode = _tinSymbol.Draw(this,geo);
					//tinNode->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE),osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
					if (tinNode!=NULL)
					{
						if (tinContainer==NULL)	tinContainer=new osg::MatrixTransform();
						tinContainer->addChild(tinNode);
					}		
					else{
						mpt_loadState=false;
						return;
					}
				}
				break;
			case GLB_TINSYMBOL_WATER:
				{
					GlbTinWaterSymbolInfo *tinWaterSymbolInfo = dynamic_cast<GlbTinWaterSymbolInfo *>(tinSymbolInfo.get());
					if (tinWaterSymbolInfo)
					{
						CGlbGlobePolygonWaterSymbol _polyWaterSymbol;
						tinNode = _polyWaterSymbol.Draw(this,geo);
						if (tinNode!=NULL)
						{
							if (tinContainer==NULL)	tinContainer=new osg::MatrixTransform();
							tinContainer->addChild(tinNode);
						}
						else{
							mpt_loadState=false;
							return;
						}							
					}
				}
				break;
			}
							
		}
	}
	else 
	{// tinGeo
		CGlbGlobeTinSymbol _tinSymbol;
		mpr_outline = _tinSymbol.GetOutline(this,mpr_tinGeo.get());	
		osg::Node *node = NULL;
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)//ֻ���Ǿ���ģʽ������ִ�д˹���
		{//����
			DirtyOnTerrainObject();						
		} 
		else
		{//������
			switch(tinSymbolInfo->symType)
			{
			case GLB_TINSYMBOL_SIMPLE:
				{
					node = _tinSymbol.Draw(this,mpr_tinGeo.get());
					if (NULL!=node)
					{
						if (tinContainer==NULL)
							tinContainer=new osg::MatrixTransform();
						tinContainer->addChild(node);
					}
					else{
						mpt_loadState=false;
						return;
					}
				}
				break;
			case GLB_TINSYMBOL_WATER:
				{
					GlbTinWaterSymbolInfo *tinWaterSymbolInfo = dynamic_cast<GlbTinWaterSymbolInfo *>(tinSymbolInfo.get());
					if (tinWaterSymbolInfo)
					{
						CGlbGlobePolygonWaterSymbol _polyWaterSymbol;
						node = _polyWaterSymbol.Draw(this,mpr_tinGeo.get());
						if (NULL!=node)
						{
							if (tinContainer==NULL)	tinContainer=new osg::MatrixTransform();
							tinContainer->addChild(node);
						}
						else{
							mpt_loadState=false;
							return;
						}							
					}
				}
				break;
			}
		}
	}

	osg::ref_ptr<osg::Switch> swiNode = new osg::Switch();
	if (tinContainer!=NULL)
	{
		swiNode->addChild(tinContainer);

		osg::StateSet *ss=tinContainer->getOrCreateStateSet();
		glbBool isNeedDirectDraw = false;
		DealTinModelSymbol(tinContainer,level,tinSymbolInfo.get(),isNeedDirectDraw);			
	}

	if(mpr_needReReadData)
	{
		/*
		*   �ڼ��mpr_needReReadDataʱ,�����߳�����mpr_needReReadData=true��ûִ�У�����.
		*/
		mpr_needReReadData = false;
		return ReadData(level,true);
	}

	//����ʹ���ڴ�
	glbInt32 objsize = this->ComputeNodeSize(swiNode);
	glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(objsize);
	if(isOK||isDirect)
	{
		// Ԥ�ȼ���bound����ʡʱ��
		swiNode->getBound();

		mpt_preNode   = mpt_node;
		mpt_node      = swiNode;
		mpt_currLevel = level;
		mpr_objSize   = objsize;
		//if(true)
		//{//�������
		//	double um = mpr_objSize;
		//	um = um/1024.0/1024.0;		
		//	GlbLogWOutput(GLB_LOGTYPE_INFO,L"obj %s: used %lf\n",GetName(),um);
		//}
	}else{
		swiNode = NULL;
	}

	//�ָ�
	MeshTinToGrids(level);

	mpt_loadState = false;
}

glbBool CGlbGlobeTinObject::DealTinModelSymbol(osg::MatrixTransform *tinContainer,glbInt32 level,
	GlbTinSymbolInfo *tinInfo,glbBool &isNeedDirectDraw)
{
	if (tinContainer==NULL||level<0)
		return false;

	if ( ! mpr_tinGeo && !mpt_feature )
		return false;

	osg::StateSet *stateset=tinContainer->getOrCreateStateSet();	
	// ���ڶ�tinʱ���¶������غϣ�Ϊ����z-buffer����Ҫ���������޳� ml 2016.8.29
	//stateset->setMode(GL_CULL_FACE,osg::StateAttribute::ON);
	//stateset->setAttributeAndModes(
	//	new osg::CullFace(osg::CullFace::FRONT),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	// ͸����
	double dopacity=1.0;

	CGlbWString locate = L"";
	if(tinInfo && tinInfo->locate)
		locate = tinInfo->locate->GetValue(mpt_feature.get());
	// ��ɫ
	if ( tinInfo && tinInfo->color)
	{// ������tin�������tinͼ��û������tin�ļ�·����.osg  .ive ...��ʱʹ����ɫ���� 2015.10.14 ����
		if ( locate.empty() )					
		{
			osg::Vec4 tinColor(1.0,1.0,1.0,1.0);
			glbInt32 plyClr=tinInfo->color->GetValue(mpt_feature.get());
			tinColor.set(GetRValueInBGRA(plyClr) / 255.0,GetGValueInBGRA(plyClr) / 255.0,GetBValueInBGRA(plyClr) / 255.0,dopacity);

			osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
			if(material == NULL)
			{
				material = new osg::Material;
				stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			}
			material->setDiffuse(osg::Material::FRONT_AND_BACK,tinColor);
		}
	}	
	if(tinInfo && tinInfo->opacity)
	{
		glbInt32 opacity = tinInfo->opacity->GetValue(mpt_feature.get());		
		CGlbOpacityVisitor nodeVisiter(opacity);
		tinContainer->accept(nodeVisiter);
		if (opacity<100)
		{
			stateset->setMode(GL_BLEND,osg::StateAttribute::ON );
			stateset->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
			if (stateset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
				stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		}
		else
		{
			stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
			stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
		}
	}
	//pos scale
	{
		glbDouble xOffset=0;glbDouble yOffset=0;glbDouble zOffset=0;
		if (mpt_feature != NULL)
		{
			IGlbGeometry *geo = NULL;
			glbBool result = false;
			result = mpt_feature->GetGeometry(&geo,level);
			if(!result || geo == NULL) 
				return false;

			CGlbExtent *pExt = const_cast<CGlbExtent *>(geo->GetExtent());
			if (pExt)
				pExt->GetCenter(&xOffset,&yOffset,&zOffset);
		} 
		else
		{
			CGlbExtent *pExt = const_cast<CGlbExtent *>(mpr_tinGeo->GetExtent());
			if (pExt)
				pExt->GetCenter(&xOffset,&yOffset,&zOffset);
		}
		//osg::Vec3d posOffset(xOffset,yOffset,zOffset);
		//if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		//	g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(posOffset.y()),osg::DegreesToRadians(posOffset.x()),posOffset.z(),
		//	posOffset.x(),posOffset.y(),posOffset.z());
		osg::Matrix mOffset=osg::Matrix::identity();
		//if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		//	g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(posOffset.x(),posOffset.y(),posOffset.z(),mOffset);
		//else
		//	mOffset.makeTranslate(posOffset);
		//osg::Matrix mOffsetInv=osg::Matrix::inverse(mOffset);

		glbDouble xScale = 1 , yScale = 1 , zScale = 1;
		osg::Matrix mScale;
		if (mpt_globe->GetType()!=GLB_GLOBETYPE_GLOBE) //������ģʽ	
		{// ����ģʽ�µ�tin����ֱ���������
			if(tinInfo->xScale)
				xScale = tinInfo->xScale->GetValue(mpt_feature.get());
			if(tinInfo->yScale)
				yScale = tinInfo->yScale->GetValue(mpt_feature.get());
			if(tinInfo->zScale)
				zScale = tinInfo->zScale->GetValue(mpt_feature.get());				
			mScale.makeScale(xScale,yScale,zScale);
		}	
		//tinContainer->setMatrix(mOffsetInv*mScale*mOffset);
		// OpenGL�ķ��������ͬʱ�任ģ�͵ķ��ߣ�����Ӱ�����Ч������˷Ŵ�ʱ�������ģ�ͱ�����չ�ǿ����Ҫ��GL_NORMALIZE		
		tinContainer->setMatrix(mScale);
		stateset->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );

		double xCenter = xOffset;
		double yCenter = yOffset;
		double zCenter = zOffset;
		xOffset = yOffset = zOffset = 0.0;
		if (tinInfo->xOffset)
			xOffset = tinInfo->xOffset->GetValue((mpt_feature.get()));
		if (tinInfo->yOffset)
			yOffset = tinInfo->yOffset->GetValue((mpt_feature.get()));
		if (tinInfo->zOffset)
			zOffset = tinInfo->zOffset->GetValue((mpt_feature.get()));	

		if (mpt_globe->GetType()==GLB_GLOBETYPE_GLOBE)
		{
			double longitude = osg::DegreesToRadians(xCenter);
			double latitude = osg::DegreesToRadians(yCenter);
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
		{
			mOffset.makeTranslate(xOffset,yOffset,zOffset);	
		}		
		tinContainer->setMatrix(tinContainer->getMatrix() * mOffset);
	}

	return true;
}

void CGlbGlobeTinObject::DealSelected()
{
	if (mpt_node==NULL)
		return;
	osg::MatrixTransform *tinContainer=(osg::MatrixTransform *)mpt_node->asSwitch()->getChild(0)->asTransform();
	//osg::StateSet *stateset=tinContainer->getOrCreateStateSet();
	osg::StateSet *stateset = mpt_node->getOrCreateStateSet();
	glbref_ptr<GlbTinSymbolInfo> tinInfo = dynamic_cast<GlbTinSymbolInfo *>(mpr_renderInfo.get());

	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	if (mpt_isSelected)
	{
		if(tinInfo && tinInfo->type == GLB_OBJECTTYPE_TIN)
		{
			material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));
		}
		else
		{
			material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
			material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
			material->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
			material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,0.1));
		}
		stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	}
	else
	{
		//if (material)
		//	stateset->removeAttribute(material);

		if(mpt_feature.valid())
			mpt_node->setStateSet(NULL);
		else
		{
			bool isNeedDirectDraw=false;
			mpt_node->setStateSet(NULL);
			DealTinModelSymbol(tinContainer,mpt_currLevel,tinInfo.get(),isNeedDirectDraw);
		}

		//if(tinInfo && tinInfo->type == GLB_OBJECTTYPE_TIN)
		//{
		//	bool isNeedDirectDraw=false;
		//	DealTinModelSymbol(tinContainer,mpt_currLevel,tinInfo.get(),isNeedDirectDraw);
		//}
	}
}
class TinObjectBlinkCallback : public osg::NodeCallback
{
public:
	TinObjectBlinkCallback(osg::StateSet *ss,GlbRenderInfo *tinInfo)
	{
		mpr_isChangeFrame = false;
		mpr_markNum = 0;
		mpr_renderInfo = (GlbTinSymbolInfo *)tinInfo;
		mpr_stateset = ss;

		if(mpr_stateset)
		{
			osg::Material *material  = 
				dynamic_cast<osg::Material *>(mpr_stateset->getAttribute(osg::StateAttribute::MATERIAL));
			if (!material)
			{
				mpr_material = new osg::Material;
				mpr_stateset->setAttribute(mpr_material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			}
			else
				mpr_material = material;
		}
	}
	~TinObjectBlinkCallback()
	{
		//if(mpr_stateset)
		//{
		//	if (mpr_material)
		//	{
		//		mpr_stateset->removeAttribute(mpr_material);
		//	}
		//}
	}

	void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		if (nv->getFrameStamp()->getFrameNumber() - mpr_markNum > 10)
		{
			mpr_markNum = nv->getFrameStamp()->getFrameNumber();
			mpr_isChangeFrame = !mpr_isChangeFrame;
		} 

		if (mpr_isChangeFrame)
		{
			mpr_material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));
			mpr_stateset->setAttribute(mpr_material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			//mpr_stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
			node->setStateSet(mpr_stateset);
		}
		else
		{
			node->setStateSet(NULL);
		}
	}
private:
	osg::ref_ptr<osg::Material> mpr_material;
	osg::ref_ptr<osg::StateSet> mpr_stateset;
	glbref_ptr<GlbTinSymbolInfo> mpr_renderInfo;
	glbBool mpr_isChangeFrame;
	unsigned int mpr_markNum;
};
void CGlbGlobeTinObject::DealBlink()
{
	if (mpt_node==NULL)
		return;
	if(mpt_isBlink)
	{
		osg::MatrixTransform *tinContainer=(osg::MatrixTransform *)mpt_node->asSwitch()->getChild(0)->asTransform();
		osg::StateSet *ss = mpt_node->getOrCreateStateSet();//tinContainer->getOrCreateStateSet();
		osg::ref_ptr<TinObjectBlinkCallback> poCallback = new TinObjectBlinkCallback(ss,mpr_renderInfo.get());
		mpt_node->addUpdateCallback(poCallback);
	}
	else
	{
		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
			mpt_node->setStateSet(NULL);
			DealSelected();
		}
	}
}

void CGlbGlobeTinObject::ComputePosByAltitudeAndGloleType( osg::Vec3d &position )
{
	if (mpt_globe==NULL) return;
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			//��xOrLon��yOrLat��zOrAlt��ֵת�����������긳ֵ��position��x��y��z
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
				osg::DegreesToRadians(position.x()),position.z(),position.x(),position.y(),position.z());
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			//��xOrLon��yOrLat��zOrAlt��ֵת�����������긳ֵ��position��x��y��z
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
				osg::DegreesToRadians(position.x()),position.z()+mpr_elevation,position.x(),position.y(),position.z());
			//position.z() = position.z() + mpt_globe->GetElevationAt(mpr_xOrLon,mpr_yOrLat);			 
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
				osg::DegreesToRadians(position.x()),mpr_elevation,position.x(),position.y(),position.z());
		}
	}
	else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if(mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
			position = osg::Vec3d(position.x(),position.y(),position.z());
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			position = osg::Vec3d(position.x(),position.y(),position.z()+mpr_elevation);			
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{
			position = osg::Vec3d(position.x(),position.y(),mpr_elevation);
		}
	}
}

void CGlbGlobeTinObject::DealModePosByChangeAltitudeOrChangePos()
{
	if(mpt_node == NULL || mpt_globe == NULL)
		return;	

	if (mpt_feature!=NULL)
	{
		IGlbGeometry *geo=NULL;
		bool result = false;
		result = mpt_feature->GetGeometry(&geo,mpt_currLevel);
		if(!result || geo == NULL)  
			return;
		osg::Group *pContainer=mpt_node->asSwitch()->getChild(0)->asGroup();
		CGlbExtent *pGeoExt = const_cast<CGlbExtent *>(geo->GetExtent());
		if (pGeoExt==NULL) return;
		glbDouble xOrLon,yOrLat,zOrAlt;
		pGeoExt->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
		osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
		ComputePosByAltitudeAndGloleType(position);

		osg::Group *pGroupTop=pContainer->getChild(0)->asGroup();
		osg::Transform *pTransform=pGroupTop->getChild(0)->asTransform();
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(pTransform);
		osg::Matrix &m = const_cast<osg::Matrix &>(mt->getMatrix());
		m.setTrans(position);
		mt->setMatrix(m);
	} 
	else if (mpr_tinGeo!=NULL)
	{
		CGlbExtent *pGeoExt = const_cast<CGlbExtent *>(mpr_tinGeo->GetExtent());
		if (pGeoExt==NULL)
			return;
		glbDouble xOrLon,yOrLat,zOrAlt;
		pGeoExt->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
		osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
		ComputePosByAltitudeAndGloleType(position);

		osg::Group *pGroupTop=mpt_node->asSwitch()->getChild(0)->asGroup();
		osg::Transform *pTransform=pGroupTop->getChild(0)->asTransform();
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(pTransform);
		osg::Matrix &m = const_cast<osg::Matrix &>(mt->getMatrix());
		m.setTrans(position);
		mt->setMatrix(m);
	}

	mpt_globe->UpdateObject(this);
}

glbInt32 CGlbGlobeTinObject::ComputeNodeSize( osg::Node *node )
{
	if (node==NULL)
		return 0;
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

void CGlbGlobeTinObject::DirtyOnTerrainObject()
{
	if (!mpt_globe) return;
	CGlbExtent* cglbExtent = GetBound(false).get();	
	if (cglbExtent)
		mpt_globe->AddDomDirtyExtent(*cglbExtent);
}

void CGlbGlobeTinObject::SetPartSelected( glbInt32 partId,glbBool isSelected )
{
	glbBool curSelected = false;
	std::map<glbInt32,glbBool>::iterator it = mpr_partSelect.find(partId);
	if(it == mpr_partSelect.end())
		mpr_partSelect[partId] = isSelected;
	else
		curSelected = it->second;
	if(curSelected == isSelected)
		return;

	osg::Node *partNode = FindPartNodeByID(partId);
	if(partNode == NULL) return;
	it->second = isSelected;

	osg::ref_ptr<osg::StateSet> stateset = partNode->getOrCreateStateSet();	
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	if (isSelected)
	{
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));
		stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	}
	else
	{
		if (material)
			stateset->removeAttribute(material);
		//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		//stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
	}
}

osg::Node* CGlbGlobeTinObject::FindPartNodeByID( glbInt32 partId )
{
	if(mpt_node == NULL) return NULL;
	char strName[32];
	sprintf_s(strName,"Part%d",partId);
	std::string partName(strName);
	osg::Group *tinContainer = mpt_node->asSwitch()->getChild(0)->asGroup();
	unsigned int numChild = tinContainer->asGroup()->getNumChildren();
	for(unsigned int i = 0; i <numChild; i++)
	{
		unsigned int numMt = tinContainer->getChild(i)->asGroup()->getNumChildren();
		for(unsigned int j = 0; j < numMt;j++)
		{
			osg::Node *node = tinContainer->getChild(i)->asGroup()->getChild(j);
			std::string name = node->getName();
			if (name == partName)
			{// �ҵ� 				
				return node;
			}
		}
	}
	return NULL;
}

bool CGlbGlobeTinObject::judgeTinWaterNeedDirectDraw( GlbTinWaterSymbolInfo *tinWaterSmblInfo, bool &isNeedDirectDraw )
{
	glbFloat texRotation=mpr_TexRotation;
	if (tinWaterSmblInfo->refractTexRotation)
		texRotation=tinWaterSmblInfo->refractTexRotation->value;
	if (!isFloatEqual(texRotation,mpr_TexRotation))
	{
		mpr_TexRotation = texRotation;
		isNeedDirectDraw=true;//���¼�����������
		return true;
	}

	if (mpt_node==NULL)
		return false;
	osg::Group *pGroupTop=mpt_node->asSwitch()->getChild(0)->asGroup()->getChild(0)->asGroup();// is CWaterScene
	IWaterScene *pWaterScene=dynamic_cast<IWaterScene *>(pGroupTop);
	osg::Transform *pTransform=pGroupTop->getChild(0)->asTransform();
	osg::Switch *pSW=pTransform->getChild(0)->asSwitch();
	osg::Geode *pGeode=pSW->getChild(0)->asGeode();
	osg::Drawable *pDrawable=pGeode->getDrawable(0);
	osg::StateSet *sset = pDrawable->getOrCreateStateSet();


	// ͸����
	glbInt32 polygonOpacity=100;
	if (tinWaterSmblInfo->opacity)
	{
		polygonOpacity=tinWaterSmblInfo->opacity->value;
	}
	if (polygonOpacity!=100)//͸��
		sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	else
		sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	sset->getUniform("PolygonOpacity")->set(polygonOpacity);//

	// ˮ��Ա��� ��ɫ
	osg::Vec4f polygonColor(0.0,255.0,0.0,1.0);
	if (tinWaterSmblInfo->color)
	{
		glbInt32 plyClr=tinWaterSmblInfo->color->value;
		polygonColor.set(GetRValueInBGRA(plyClr),LOBYTE(((plyClr & 0xFFFF)) >> 8),GetBValueInBGRA(plyClr),1.0);
	}
	sset->getUniform("PolygonColor")->set(polygonColor);
	bool buseTexColor=false;
	if (tinWaterSmblInfo->refractionImageData)// ����������ͼ
	{
		CGlbWString imageMapFilePath=tinWaterSmblInfo->refractionImageData->value;
		osg::Texture2D *tex2D = 
			dynamic_cast<osg::Texture2D *>(sset->getTextureAttribute(3,osg::StateAttribute::TEXTURE));
		glbInt32 index = imageMapFilePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			imageMapFilePath = execDir + imageMapFilePath.substr(1,imageMapFilePath.size());
		}
		osg::Image *pImg=loadImage(imageMapFilePath);
		if (pImg!=NULL)
		{
			tex2D->setImage(pImg);
			tex2D->dirtyTextureObject();
			buseTexColor=true;
		}
	}
	sset->getUniform("PolygonUseTex")->set(buseTexColor);

	bool enableReflect=false;
	if (tinWaterSmblInfo->enableReflections)
	{
		enableReflect=tinWaterSmblInfo->enableReflections->value;
	}
	sset->getUniform("waterPolygon_EnableReflections")->set( enableReflect);
	pWaterScene->enableReflections(enableReflect);

	bool enableRefract=false;
	if (tinWaterSmblInfo->enableRefractions)
	{
		enableRefract=tinWaterSmblInfo->enableRefractions->value;
	}
	sset->getUniform("waterPolygon_EnableRefractions")->set( enableRefract);
	pWaterScene->enableRefractions(enableRefract);


	// ���١�����
	float windSpeed=1.0f;
	if (tinWaterSmblInfo->windStrong)
	{
		windSpeed=tinWaterSmblInfo->windStrong->value;
	}
	sset->getUniform("windSpeed")->set(windSpeed);
	windSpeed=0.0f;
	if (tinWaterSmblInfo->windDirection)
	{
		windSpeed=tinWaterSmblInfo->windDirection->value;
	}
	sset->getUniform("windDirection")->set(windSpeed);


	isNeedDirectDraw=false;
	return true;
}

//����ֱ����OperatorVisitor���˺������ڽ���endAlpha��ֵ
void GlbGlobe::CGlbGlobeTinObject::ParseObjectFadeColor()
{
	if(!mpr_renderInfo.valid()) return;

	GlbTinSymbolInfo *tinInfo = static_cast<GlbTinSymbolInfo *>(mpr_renderInfo.get());
	if (tinInfo == NULL) return;
	
	mpt_fadeEndAlpha = tinInfo->opacity->GetValue(mpt_feature.get());
	return;

	mpt_fadeColor = GetColor(tinInfo->color->GetValue(mpt_feature.get()));
}

glbBool GlbGlobe::CGlbGlobeTinObject::Load2( xmlNodePtr* node,const glbWChar* prjPath )
{
	xmlNodePtr pnode = *node;
	CGlbGlobeRObject::Load2(node,prjPath);
	xmlChar *szKey = NULL;
	xmlNodePtr groupNode = pnode->parent;
	xmlNodePtr childNode;
	CGlbString bUse;

	if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"GetRenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		xmlNodePtr rdchild = groupNode->xmlChildrenNode;
		rdchild = rdchild->next;//text
		rdchild = rdchild->next;//type
		rdchild = rdchild->next;//text
		GlbTinSymbolInfo* tinInfo = new GlbTinSymbolInfo();
		tinInfo->Load2(rdchild,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)tinInfo;
		ParseObjectFadeColor();
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetRenderOrder
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&mpt_renderOrder);
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetRObject
	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetTooltip

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsBlink
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isBlink = false;
		else
			mpt_isBlink = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsEdit
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isEdit = false;
		else
			mpt_isEdit = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsGround
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isGround = false;
		else
			mpt_isGround = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsSelected
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isSelected = false;
		else
			mpt_isSelected = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsShow
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isShow = false;
		else
			mpt_isShow = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsUseInstance
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isUseInstance = false;
		else
			mpt_isUseInstance = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//x
	glbDouble x = 0.0, y = 0.0,z = 0.0;
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%lf",&x);
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//y
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%lf",&y);
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//z
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%lf",&z);
		xmlFree(szKey);
	}

	mpr_tinGeo = new CGlbPoint(x,y,z);
	return true;
}

glbBool GlbGlobe::CGlbGlobeTinObject::interset(osg::Vec3d vtStart, osg::Vec3d vtEnd, osg::Vec3d& interPoint)
{
	glbBool haveIntersect = false;

	if(!mpr_MeshBox.valid()) //��ʱ��û�и���
	{
		osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(vtStart,vtEnd);
		osgUtil::IntersectionVisitor iv(lsi.get());

		osg::Node * tinNode = this->GetOsgNode();
		if(!tinNode) return false;

		tinNode->accept(iv);

		if (lsi->containsIntersections())
		{
			interPoint = lsi->getIntersections().begin()->getWorldIntersectPoint();
			haveIntersect = true;
		}
	}
	else
	{
		haveIntersect = mpr_MeshBox.intersect(vtStart,vtEnd,interPoint);
	}

	return haveIntersect;

}

void GlbGlobe::CGlbGlobeTinObject::updateMeshBox(const osg::Matrix parentMatrix,const GlbTinSymbolInfo * tinSmblInfo,
	glbref_ptr<CGlbFeature> feature,GLboolean globeMode)
{
	mpr_MeshBox.updateGrids(parentMatrix,tinSmblInfo,feature,globeMode);
}


void GlbGlobe::CGlbGlobeTinObject::MeshTinToGrids(glbInt32 level)
{
	/*�ָ���,����û�зָ� ֻ����������Ƭ����10000ʱ�ŷ�*/
	osg::Timer_t startTime = osg::Timer::instance()->tick();

	glbref_ptr<GlbTinSymbolInfo> tinSymbolInfo = dynamic_cast<GlbTinSymbolInfo *>(mpr_renderInfo.get());

	if(mpt_feature)
	{
		IGlbGeometry *geo=NULL;
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{

		}
		else
		{	// ������
			if(tinSymbolInfo->symType == GLB_TINSYMBOL_SIMPLE)
			{
				mpt_feature->GetGeometry(&geo,level);

				//����������Z�����������ػ��ƣ���������ڹ���
				mpr_MeshBox.clearGrids();

				mpr_MeshBox.buildMeshBox(this,geo);
			}
		}
	}
	else
	{
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)//ֻ���Ǿ���ģʽ������ִ�д˹���
		{//����
		} 
		else
		{
			if(tinSymbolInfo->symType == GLB_TINSYMBOL_SIMPLE)
			{
				//����������Z�����������ػ��ƣ���������ڹ���
				mpr_MeshBox.clearGrids();

				mpr_MeshBox.buildMeshBox(this,mpr_tinGeo.get());

			}
		}
	}

	osg::Timer_t afterTime = osg::Timer::instance()->tick();

	//mpt_globe->testInitGridTime += osg::Timer::instance()->delta_m(startTime,afterTime);

	//��mpt_node����
#ifdef _TEST_INT_GRID
	if(testNode != NULL)
	{
		mpt_globe->mpr_p_callback->AddRemoveTask(new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_globe->mpr_root,testNode));
	}

	testNode = mpr_MeshBox.testShowGridNode();
	mpt_globe->mpr_p_callback->AddHangTask(new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_globe->mpr_root,testNode));

	mpt_HangTaskNum++;
#endif

}

