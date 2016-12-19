#include "StdAfx.h"
#include "GlbGlobeTerrainModify.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbGlobeFeatureLayer.h"
#include <osg/Point>
//#include "GlbCalculateNodeSizeVisitor.h"

using namespace GlbGlobe;

CGlbGlobeTerrainModify::CGlbGlobeTerrainModify(void)
{	
	mpr_readData_critical.SetName(L"terrainmodify_readdata");
	mpr_addToScene_critical.SetName(L"terrainmodify_addscene");
	mpr_outline = NULL;
	mpr_region = NULL;
	mpr_osgOutlineGeometry = NULL;
	mpr_lnPtsGeometry = NULL;
	mpr_currPtGeometry = NULL;
	mpr_terrainProvider = NULL;
	mpr_isFlat = true;
	mpr_modifymode = GLB_TERRAINMODIFY_REPLACE;
	mpr_height = 0.0;
	mpt_isShow = true;
	mpr_isShowOutline = true;
}

CGlbGlobeTerrainModify::~CGlbGlobeTerrainModify(void)
{
	mpr_outline = NULL;
	mpr_region = NULL;	
	mpr_terrainProvider = NULL;
}

GlbGlobeObjectTypeEnum CGlbGlobeTerrainModify::GetType()
{
	return GLB_OBJECTTYPE_MODIFYTERRIN;
}

glbBool CGlbGlobeTerrainModify::Load( xmlNodePtr* node,const glbWChar* prjPath )
{
	CGlbGlobeRObject::Load(node,prjPath);
	
	xmlChar* szKey = NULL;
	xmlNodePtr pnode = *node;
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Region")))
	{
		xmlNodePtr region_node = pnode->children;
		if (region_node && (!xmlStrcmp(region_node->name, (const xmlChar *)"PointCount")))
		{
			int ptCnt;
			szKey = xmlNodeGetContent(region_node);
			sscanf_s((char*)szKey,"%d",&ptCnt);	
			xmlFree(szKey);

			region_node = region_node->next;

			if (mpr_region) mpr_region=NULL;

			mpr_region = new CGlbLine(3);
			glbDouble ptx,pty,ptz;
			for (int i = 0; i < ptCnt; i++)
			{
				if (region_node && (!xmlStrcmp(region_node->name, (const xmlChar *)"Coordinate")))
				{
					szKey = xmlNodeGetContent(region_node);
					sscanf_s((char*)szKey,"%lf %lf %lf",&ptx,&pty,&ptz);
					mpr_region->AddPoint(ptx,pty,ptz);
					xmlFree(szKey);

					region_node = region_node->next;
				}
			}			
		}
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsFlat")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpr_isFlat);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"ModifyMode")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpr_modifymode);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Height")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%lf",&mpr_height);	
		xmlFree(szKey);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsShowOutline")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpr_isShowOutline);	
		xmlFree(szKey);
		pnode = pnode->next;
	}
	return true;
}

glbBool CGlbGlobeTerrainModify::Save( xmlNodePtr node,const glbWChar* prjPath )
{
	CGlbGlobeRObject::Save(node,prjPath);

	char str[128];
	if (mpr_region)
	{
		xmlNodePtr region_node = xmlNewNode(NULL,BAD_CAST "Region");
		xmlAddChild(node,region_node);
		glbInt32 ptCnt = mpr_region->GetCount();
		sprintf_s(str,"%d",ptCnt);
		xmlNewTextChild(region_node, NULL, BAD_CAST "PointCount", BAD_CAST str);
		glbDouble ptx,pty,ptz;
		for (glbInt32 i = 0; i < ptCnt; i++)
		{
			mpr_region->GetPoint(i,&ptx,&pty,&ptz);
			sprintf_s(str,"%.5lf %.5lf %.5lf",ptx,pty,ptz);
			xmlNewTextChild(region_node, NULL, BAD_CAST "Coordinate", BAD_CAST str);
		}
	}

	sprintf_s(str,"%d",mpr_isFlat);
	xmlNewTextChild(node, NULL, BAD_CAST "IsFlat", BAD_CAST str);
	
	sprintf_s(str,"%d",mpr_modifymode);
	xmlNewTextChild(node, NULL, BAD_CAST "ModifyMode", BAD_CAST str);
	
	sprintf_s(str,"%.5lf",mpr_height);
	xmlNewTextChild(node, NULL, BAD_CAST "Height", BAD_CAST str);	

	sprintf_s(str,"%d",mpr_isShowOutline);
	xmlNewTextChild(node, NULL, BAD_CAST "IsShowOutline", BAD_CAST str);

	return true;
}

glbDouble CGlbGlobeTerrainModify::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
{
	if (!mpr_region || !mpt_globe) return DBL_MAX;

	if(isCompute == false)
		return mpr_distance;

	if (mpt_isCameraAltitudeAsDistance)
		mpr_distance = cameraPos.z();	
	else
	{
		osg::Vec3d _center;
		mpr_region->GetExtent()->GetCenter(&_center.x(),&_center.y(),&_center.z());
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();
		if (GLB_GLOBETYPE_GLOBE==globeType)
		{
			osg::Vec3d worldCenter;
			osg::Vec3d cameraCenter;
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_center.y()),osg::DegreesToRadians(_center.x()),_center.z(),worldCenter.x(),worldCenter.y(),worldCenter.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraCenter.x(),cameraCenter.y(),cameraCenter.z());
			mpr_distance = (worldCenter-cameraCenter).length();
		}
		else if (GLB_GLOBETYPE_FLAT==globeType)
		{
			mpr_distance = (_center-cameraPos).length();
		}
	}	
	return mpr_distance;
}

glbref_ptr<CGlbExtent> CGlbGlobeTerrainModify::GetBound(glbBool isWorld)
{	
	if(mpr_region==NULL)return NULL;

	glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent*>(mpr_region->GetExtent());
	if (isWorld==false)	return geoExt;

	if (mpt_globe==NULL) return NULL;
	glbref_ptr<CGlbExtent> worldExt = NULL;
	if (mpt_globe->GetType()==GLB_GLOBETYPE_GLOBE)
	{
		glbDouble x,y,z;
		worldExt = new CGlbExtent;
		geoExt->GetMin(&x,&y,&z);
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,x,y,z);
		worldExt->Merge(x,y,z);
		geoExt->GetMax(&x,&y,&z);
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,x,y,z);
		worldExt->Merge(x,y,z);
	}
	else
		worldExt = geoExt;

	return worldExt;
}

void CGlbGlobeTerrainModify::LoadData(glbInt32 level)
{
	if(mpt_currLevel == level) return;
	if(mpt_preNode  !=  NULL)
	{//旧对象还未压入摘队列
		/*
		*   mpt_node = node1,preNode = NULL    -----> node1_h
		*   不做控制，且LoadData快
		*   {
		*       mpt_node = node2,preNode = node1
		*       mpt_node = node3,preNode = node2   -----> node1_h,node2_r,node2_h
		*       造成node1 永远不会摘除
		*   }
		*/
		return;
	}

	if (mpt_currLevel != -1) 
		return;
	
	{//与DirectDraw互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return;
		mpt_loadState = true;
	}		
	if(mpt_currLevel != level)
	{
		if(mpt_currLevel < level)
			mpt_currLevel = level;
	}

	// 生成mpt_node
	ReadData();

	mpt_loadState = false;
}

void CGlbGlobeTerrainModify::AddToScene()
{
	if(mpt_node == NULL)return;
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	bool needUpdate = false;
	if(mpt_preNode == NULL)
	{//没有产生新osg节点
		if(mpt_HangTaskNum == 0
			&& mpt_node->getNumParents() == 0
			&& mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//贴地模式,节点不需要挂.
			//挂mpt_node任务
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;
			needUpdate = true;
		}			
	}
	else
	{//产生了新osg节点						
		{//旧节点已经挂到场景树
			//摘旧节点mpt_preNode的任务
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
			task->mpr_size = this->ComputeNodeSize(mpt_preNode);
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			mpt_preNode = NULL;
			CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(task->mpr_size);
		}
		if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//挂新节点mpt_node任务
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
		if (isnew && mpr_isShowOutline)
			mpt_node->asSwitch()->setAllChildrenOn();
		else	
			mpt_node->asSwitch()->setAllChildrenOff();				
	}
}

#include "osg/LineWidth"
glbBool CGlbGlobeTerrainModify::ReadData()
{// 有问题!
	if (!mpt_globe || !mpr_region)	return false;	
	glbInt32 ptCnt = mpr_region->GetCount();
	if (ptCnt<3)	return false;

	// 生成outline边框线 geometry
	mpr_osgOutlineGeometry=NULL;
	mpr_osgOutlineGeometry = new osg::Geometry;
	mpr_osgOutlineGeometry->setName("OutLine");
	osg::Vec3Array* vertexs = new osg::Vec3Array;
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	mpr_osgOutlineGeometry->setVertexArray(vertexs);
	mpr_osgOutlineGeometry->setColorArray(colors);
	mpr_osgOutlineGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	// 偏移基准点(中心点)
	osg::Vec3 origin;
	osg::Vec3d _center;
	mpr_region->GetExtent()->GetCenter(&_center.x(),&_center.y(),&_center.z());
	GlbGlobeTypeEnum globeType = mpt_globe->GetType();
	if (GLB_GLOBETYPE_GLOBE==globeType)
	{
		osg::Vec3d worldCenter;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_center.y()),osg::DegreesToRadians(_center.x()),_center.z(),worldCenter.x(),worldCenter.y(),worldCenter.z());			
	}
	origin = osg::Vec3(_center.x(),_center.y(),_center.z());

	glbDouble ptx,pty,ptz;
	for(glbInt32 i = 0; i < ptCnt; i++)
	{
		mpr_region->GetPoint(i,&ptx,&pty,&ptz);
		if (GLB_GLOBETYPE_GLOBE==globeType)
		{
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
		}
		vertexs->push_back(osg::Vec3(ptx,pty,ptz)-origin);
	}
	mpr_osgOutlineGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP,0,ptCnt));
	osg::Geode* geode = new osg::Geode;
	geode->addDrawable(mpr_osgOutlineGeometry);

	mpr_osgOutlineGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(2),osg::StateAttribute::ON);

	osg::MatrixTransform* matnode = new osg::MatrixTransform;
	osg::Matrix mt;
	mt.makeTranslate(origin);
	matnode->setMatrix(mt);
	matnode->addChild(geode);

	osg::ref_ptr<osg::Switch> swiNode = new osg::Switch;
	// 关闭光照，使用颜色数组
	swiNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	swiNode->addChild(matnode);

	glbInt32 objsize = this->ComputeNodeSize(swiNode);
	glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(objsize);
	if(isOK)
	{
		// 预先计算bound，节省时间
		swiNode->getBound();

		mpt_preNode   = mpt_node;
		mpt_node      = swiNode;	
		mpr_objSize   = objsize;
	}else{
		swiNode = NULL;
	}
	
	return true;
}

glbInt32 CGlbGlobeTerrainModify::RemoveFromScene(glbBool isClean)
{
	{//与LoadData互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return 0;	
		mpt_loadState = true;
		/*
		*    mpt_node = node1,pre=NULL
		*    读:mpt_node = node2,pre=node1		
		*/
	}
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	
	glbInt32 tsize = 0;
	if(isClean == false)
	{//从显存卸载对象，节约显存.
		if ((mpt_node != NULL))// && (mpt_node->getNumParents() != 0))
		{//obj在显存	
			/*
			     mpt_node 已经在挂队列，但是还没挂到场景树上
				 这时判断getNumParents() != 0 是不可以的.
			*/
			CGlbGlobeCallBack::CGlbGlobeCallBackTask *task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			tsize = task->mpr_size;
		}
	}
	else
	{//从内存中卸载对象，节约内存
		//删除 上一次装载的节点		
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
		//删除当前节点
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
	//减少使用内存
	if(tsize>0 && isClean)
	{
		CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(tsize);		
	}

	if (mpt_globe)
	{// 刷新		
		CGlbExtent* ext = const_cast<CGlbExtent*>(mpr_region->GetExtent());
		mpt_globe->AddDemDirtyExtent(*ext);			
	}

	return tsize;	
}

IGlbGeometry *CGlbGlobeTerrainModify::GetOutLine()
{
	if (mpr_region)
		return mpr_region.get();
	return NULL;
}

glbBool CGlbGlobeTerrainModify::SetShow(glbBool isShow,glbBool isOnState)
{
	if (mpt_isEdit && isOnState)// 编辑状态不再由调度控制显隐
		return true;
	/*
	* 调度器根据可见范围控制对象显示和隐藏此时isOnState==true,isShow==true或false
	*/
	glbBool isold = mpt_isDispShow && mpt_isShow;
	if(isOnState)
	{// 调度器控制显隐
		mpt_isDispShow = isShow;
	}
	else
	{// 用户控制显隐
		mpt_isShow = isShow;
	}

	glbBool isnew = mpt_isDispShow && mpt_isShow;

	if(isold == isnew)
		return true;

	if(isOnState)
	{// 调度器控制显隐		
		
	}
	else
	{// 用户控制显隐
		if (mpt_isShow == isShow) return true;
		mpt_isShow = isShow;
		//if (mpt_node)
		//	mpt_node->asSwitch()->setValue(0,mpt_isShow);
	}		

	if (mpt_node)
	{// 节点存在
		if (isShow && mpr_isShowOutline)
			mpt_node->asSwitch()->setAllChildrenOn();
		else
			mpt_node->asSwitch()->setAllChildrenOff();
		//mpt_node->asSwitch()->(0,(isShow&&mpr_isShowOutline) );		
	}	

	if (mpt_globe && mpr_region)
	{// 刷新		
		CGlbExtent* ext = const_cast<CGlbExtent*>(mpr_region->GetExtent());
		mpt_globe->AddDemDirtyExtent(*ext);			
	}
	return true;
}

glbBool CGlbGlobeTerrainModify::SetSelected(glbBool isSelected)
{	
	return CGlbGlobeREObject::SetSelected(isSelected);
}

void CGlbGlobeTerrainModify::DirectDraw(glbInt32 level)
{
	if (mpt_parent    == NULL) return;		
	LoadData(level);
	AddToScene();
}

glbInt32 CGlbGlobeTerrainModify::GetOsgNodeSize()
{
	if (mpt_node==NULL) return 0;
	if (mpr_objSize==0)
		return ComputeNodeSize(mpt_node);		
	return mpr_objSize;
}

glbInt32 CGlbGlobeTerrainModify::ComputeNodeSize( osg::Node *node )
{
	//CGlbCalculateNodeSizeVisitor cnsv;
	//node->accept(cnsv);
	//return cnsv.getTextureSize() + cnsv.getNodeMemSize();

	return 0;
}

glbBool CGlbGlobeTerrainModify::SetRegion(CGlbLine* region )
{
	if (region==NULL)	return false;
	GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);

	if (mpt_preNode!=NULL)
	{// 处理尚未处理的节点
		AddToScene();
	}

	mpt_loadState = true;
	if (mpr_region && mpt_node && mpt_globe)
	{// 刷新旧的区域
		CGlbExtent* oldExt = const_cast<CGlbExtent*>(mpr_region->GetExtent());				
		mpt_globe->AddDemDirtyExtent(*oldExt);		
	}
	mpr_region  = region;	

	if (mpt_globe && mpt_node)
	{// 刷新
		mpt_globe->mpr_sceneobjIdxManager->UpdateObject(this);

		CGlbExtent* ext = const_cast<CGlbExtent*>(mpr_region->GetExtent());
		mpt_globe->AddDemDirtyExtent(*ext);			
	}

	// 重新生成outlineGeometry	
	ReadData();
	AddToScene();

	mpt_loadState = false;
	return true;
}

CGlbLine* CGlbGlobeTerrainModify::GetRegion()
{
	return mpr_region.get();
}

glbBool CGlbGlobeTerrainModify::SetFlat(glbBool isFlat)
{
	if (mpr_isFlat==isFlat)	return true;	
	GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
	mpt_loadState = true;	
	mpr_isFlat = isFlat;	
	if (mpt_node && mpr_region && mpt_globe)
	{// 添加刷新区域		
		CGlbExtent* ext = const_cast<CGlbExtent*>(mpr_region->GetExtent());
		mpt_globe->AddDemDirtyExtent(*ext);
	}
	mpt_loadState = false;
	return true;
}

glbBool CGlbGlobeTerrainModify::IsFlat()
{ 
	return mpr_isFlat;
}

glbBool CGlbGlobeTerrainModify::SetModifyMode(GlbGlobeTerrainModifyModeEnum mode)
{
	if (mpr_modifymode==mode)
		return true;
	GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
	mpt_loadState = true;	
	mpr_modifymode = mode;
	
	// 添加刷新区域
	if (mpt_node && mpr_region && mpt_globe)
	{
		CGlbExtent* ext = const_cast<CGlbExtent*>(mpr_region->GetExtent());
		mpt_globe->AddDemDirtyExtent(*ext);		
	}
	mpt_loadState = false;	
	return true;
}

GlbGlobeTerrainModifyModeEnum CGlbGlobeTerrainModify::GetModifyMode()
{
	return mpr_modifymode;
}

glbBool CGlbGlobeTerrainModify::SetHeight(glbDouble hei)
{
	if (mpr_height==hei)
		return true;
	GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
	mpt_loadState = true;	
	mpr_height = hei;

	if (mpt_node && mpr_region && mpt_globe)
	{// 添加刷新区域		
		CGlbExtent* ext = const_cast<CGlbExtent*>(mpr_region->GetExtent());
		mpt_globe->AddDemDirtyExtent(*ext);
	}

	mpt_loadState = false;	
	return true;
}

glbDouble CGlbGlobeTerrainModify::GetHeight()
{
	return mpr_height;
}

#include "GlbGlobeMath.h"
glbBool CGlbGlobeTerrainModify::GetElevate(glbDouble xOrLon,glbDouble yOrLat,glbDouble &zOrAlt)
{
	if (mpr_region==NULL)return false;	
	glbDouble ptx,pty,ptz;
	int ptCnt = mpr_region->GetCount();
	double* pts = new double[ptCnt*2];
	for(glbInt32 i = 0; i < ptCnt; i++)
	{
		mpr_region->GetPoint(i,&ptx,&pty,&ptz);
		pts[i*2] = ptx;
		pts[i*2+1] = pty;
	}

	glbBool bSucc = PtInPolygonExt(xOrLon,yOrLat,pts,ptCnt,2);
	// 释放申请的内存
	if (pts)	delete[] pts;
	if (bSucc)
	{// (xOrLon,yOrLat)在mpr_region范围内
		if (mpr_terrainProvider)
		{
			mpr_terrainProvider->GetElevate(xOrLon,yOrLat,zOrAlt);
		}
		else
		{
			if (mpr_isFlat)
			{// 平坦
				switch(mpr_modifymode)
				{
				case GLB_TERRAINMODIFY_REPLACE:
					zOrAlt = mpr_height;
					break;
				case GLB_TERRAINMODIFY_BELOW:// 裁剪下方											
					if (zOrAlt < mpr_height)		zOrAlt = mpr_height;					
					break;
				case GLB_TERRAINMODIFY_UP:// 裁剪上方											
					if (zOrAlt > mpr_height)		zOrAlt = mpr_height;					
					break;
				}
			}
			else
			{// 不平坦
				glbDouble Mx,My,Mz,MaxX,MaxY,MaxZ;
				mpr_region->GetExtent()->GetMin(&Mx,&My,&Mz);
				mpr_region->GetExtent()->GetMax(&MaxX,&MaxY,&MaxZ);
				switch(mpr_modifymode)
				{
				case GLB_TERRAINMODIFY_REPLACE:											
					//zOrAlt = zOrAlt - Mz + mpr_height - Mz;		
					zOrAlt = mpr_height + zOrAlt - (Mz+MaxZ)*0.5;
					break;
				case GLB_TERRAINMODIFY_BELOW:// 裁剪下方											
					//if (zOrAlt + mpr_height - 2* Mz < mpr_height)	zOrAlt = mpr_height;		
					if (zOrAlt < mpr_height)		zOrAlt = mpr_height + zOrAlt - (Mz+MaxZ)*0.5;	
					break;
				case GLB_TERRAINMODIFY_UP:// 裁剪上方						
					//if (zOrAlt + mpr_height- 2*Mz > mpr_height)		zOrAlt = mpr_height;		
					if (zOrAlt > mpr_height)		zOrAlt = mpr_height + zOrAlt - (Mz+MaxZ)*0.5;
					break;
				}
			}	
		}	
		return true;
	}	

	return false;
}

glbBool CGlbGlobeTerrainModify::SetElevateOp(IGlbGlobeTerrainProvider* op)
{
	if (!op) return false;
	GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
	mpt_loadState = true;	
	mpr_terrainProvider = op;
	mpr_terrainProvider->SetRegion(mpr_region.get());

	if (mpt_node && mpr_region && mpt_globe)
	{// 添加刷新区域		
		CGlbExtent* ext = const_cast<CGlbExtent*>(mpr_region->GetExtent());
		mpt_globe->AddDemDirtyExtent(*ext);
	}
	mpt_loadState = false;	
	return true;
}

glbBool CGlbGlobeTerrainModify::ModifyDem(glbFloat* demdata, glbInt32 demsize, CGlbExtent* tileExtent)
{
	glbBool doModify = false;
	if (!demdata || !mpr_region|| !mpt_globe)
		return doModify;

	//// 对象不显示时不需要修改地形
	//if (mpt_node && mpt_node->asSwitch()->getValue(0)==false)
	//	return doModify;

	// 提取到调用前来做判断
	glbDouble minx,miny,maxx,maxy;
	tileExtent->Get(&minx,&maxx,&miny,&maxy);

	glbDouble xOrLon,yOrLat,zOrAlt;
	glbBool bSucc=false;
	int step_cnt = demsize - demsize%2; 
	glbDouble xStep = (maxx-minx) / step_cnt;
	glbDouble yStep = (maxy-miny) / step_cnt;
	for (glbInt32 i = 0;i < demsize*demsize; i++)
	{
		int row = i / demsize;
		int col = i % demsize;

		xOrLon = minx + xStep*col;
		yOrLat = maxy - yStep*row;
		zOrAlt = demdata[i];
		bSucc = GetElevate(xOrLon,yOrLat,zOrAlt);
		if (bSucc)
		{// 在修改范围内，替换改点高程值
			demdata[i] = zOrAlt;
			doModify = true;
		}
	}		

	return doModify;
}

glbBool CGlbGlobeTerrainModify::SetEdit(glbBool isEdit)
{
	if (mpt_isEdit==isEdit) return true;
	mpt_isEdit = isEdit;	
	DealEdit();
	return true;
}

void CGlbGlobeTerrainModify::DealEdit()
{////？？？？

}

glbBool CGlbGlobeTerrainModify::DrawPoints(glbInt32 color)
{
	if (!mpt_globe || !mpr_region || !mpr_osgOutlineGeometry)
		return false;

	osg::Geode* geode = mpr_osgOutlineGeometry->getParent(0)->asGeode();

	if (!geode)
		return false;

	glbInt32 ptCnt = mpr_region->GetCount();
	mpr_lnPtsGeometry = new osg::Geometry;
	mpr_lnPtsGeometry->setName("LinePoints");
	osg::Vec3Array* vertexs = new osg::Vec3Array;
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(osg::Vec4(GetRValue(color)/255.0f,LOBYTE(((color & 0xFFFF)) >> 8)/255.0f,GetBValue(color)/255.0f,1.0f)));
	mpr_lnPtsGeometry->setVertexArray(vertexs);
	mpr_lnPtsGeometry->setColorArray(colors);
	mpr_lnPtsGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	// 偏移基准点(中心点)
	osg::Vec3 origin;
	osg::Vec3d _center;
	mpr_region->GetExtent()->GetCenter(&_center.x(),&_center.y(),&_center.z());
	GlbGlobeTypeEnum globeType = mpt_globe->GetType();
	if (GLB_GLOBETYPE_GLOBE==globeType)
	{
		osg::Vec3d worldCenter;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_center.y()),osg::DegreesToRadians(_center.x()),_center.z(),worldCenter.x(),worldCenter.y(),worldCenter.z());	
		_center = worldCenter;
	}
	origin = osg::Vec3(_center.x(),_center.y(),_center.z());

	glbDouble ptx,pty,ptz;
	for(glbInt32 i = 0; i < ptCnt; i++)
	{
		mpr_region->GetPoint(i,&ptx,&pty,&ptz);
		if (GLB_GLOBETYPE_GLOBE==globeType)
		{
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
		}
		vertexs->push_back(osg::Vec3(ptx,pty,ptz)-origin);
	}
	mpr_lnPtsGeometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS,0,ptCnt));
		
	// 设置点大小 默认为5个像素
	osg::Point* ptsize = new osg::Point(5);
	mpr_lnPtsGeometry->getOrCreateStateSet()->setAttributeAndModes(ptsize,osg::StateAttribute::ON);
	
	// 挂drawable
	//geode->addDrawable(lnptsGeom);
	//glbInt32 size = 0;
	//glbref_ptr<CGlbGlobeCallBack::CGlbGlobeDrawableTask> task = 
	//	new CGlbGlobeCallBack::CGlbGlobeDrawableTask(geode,mpr_lnPtsGeometry.get(),size);
	//mpt_globe->mpr_p_callback->AddHangTask(task);		

	// 绘制当前点
	DrawCurrPoint();

	// 默认要显示点
	mpt_node->asSwitch()->setAllChildrenOn();

	return true;
}
glbBool CGlbGlobeTerrainModify::SetCurrPoint(glbInt32 idx, glbInt32 color)
{
	if (!mpr_region) return false;
	glbInt32 ptCnt = mpr_region->GetCount();
	if (idx < 0 || idx > ptCnt-1)
		return false;	
	if (idx==mpr_curPointIndex && color==mpr_curPointColor)
		return true;

	mpr_curPointIndex = idx;
	mpr_curPointColor = color;

	// 修改顶点和颜色
	if (mpr_currPtGeometry)
	{// 如果已经有"CurrPoint"可绘制对象
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();

		osg::Vec3Array* vertexs = new osg::Vec3Array;
		glbDouble ptx,pty,ptz;
		mpr_region->GetPoint(mpr_curPointIndex,&ptx,&pty,&ptz);
		if (GLB_GLOBETYPE_GLOBE==globeType)
		{
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
		}
		osg::Vec3d pt(ptx,pty,ptz);
		// 偏移基准点(中心点)
		osg::Vec3 origin;
		osg::Vec3d _center;
		mpr_region->GetExtent()->GetCenter(&_center.x(),&_center.y(),&_center.z());
		if (GLB_GLOBETYPE_GLOBE==globeType)
		{
			osg::Vec3d worldCenter;
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_center.y()),osg::DegreesToRadians(_center.x()),_center.z(),worldCenter.x(),worldCenter.y(),worldCenter.z());	
			_center = worldCenter;
		}
		origin = osg::Vec3(_center.x(),_center.y(),_center.z());
		vertexs->push_back(osg::Vec3(pt.x(),pt.y(),pt.z())-origin);		
		osg::Vec4Array* colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(GetRValue(mpr_curPointColor)/255.0f,LOBYTE(((mpr_curPointColor & 0xFFFF)) >> 8)/255.0f,GetBValue(mpr_curPointColor)/255.0f,1.0f));
		
		mpr_currPtGeometry->setVertexArray(vertexs);
		mpr_currPtGeometry->setColorArray(colors);
		mpr_currPtGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	}
	return true;
}
glbBool CGlbGlobeTerrainModify::DrawCurrPoint()
{
	if (mpr_curPointIndex==-1 || !mpt_globe || !mpr_osgOutlineGeometry)
		return false;
	osg::Geode* geode = mpr_osgOutlineGeometry->getParent(0)->asGeode();

	if (!geode)
		return false;

	GlbGlobeTypeEnum globeType = mpt_globe->GetType();

	osg::Vec3Array* vertexs = new osg::Vec3Array;
	glbDouble ptx,pty,ptz;
	mpr_region->GetPoint(mpr_curPointIndex,&ptx,&pty,&ptz);
	if (GLB_GLOBETYPE_GLOBE==globeType)
	{
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
	}
	osg::Vec3d pt(ptx,pty,ptz);
	// 偏移基准点(中心点)
	osg::Vec3 origin;
	osg::Vec3d _center;
	mpr_region->GetExtent()->GetCenter(&_center.x(),&_center.y(),&_center.z());
	if (GLB_GLOBETYPE_GLOBE==globeType)
	{
		osg::Vec3d worldCenter;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_center.y()),osg::DegreesToRadians(_center.x()),_center.z(),worldCenter.x(),worldCenter.y(),worldCenter.z());	
		_center = worldCenter;
	}
	origin = osg::Vec3(_center.x(),_center.y(),_center.z());
	vertexs->push_back(osg::Vec3(pt.x(),pt.y(),pt.z())-origin);
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(GetRValue(mpr_curPointColor)/255.0f,LOBYTE(((mpr_curPointColor & 0xFFFF)) >> 8)/255.0f,GetBValue(mpr_curPointColor)/255.0f,1.0f));

	//用像素点绘制
	if (mpr_currPtGeometry==NULL)
	{
		mpr_currPtGeometry = new osg::Geometry;
		mpr_currPtGeometry->setName("CurrPoint");
		
		mpr_currPtGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		mpr_currPtGeometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS,0,vertexs->size()));

		// 设置点大小 默认为5个像素
		osg::Point* ptsize = new osg::Point(5);
		mpr_currPtGeometry->getOrCreateStateSet()->setAttributeAndModes(ptsize,osg::StateAttribute::ON);

		//// 挂drawable
		////geode->addDrawable(tempGeom);
		//glbInt32 size = 0;
		//CGlbGlobeCallBack::CGlbGlobeDrawableTask* task = 
		//	new CGlbGlobeCallBack::CGlbGlobeDrawableTask(geode,mpr_currPtGeometry.get(),size);
		//mpt_globe->mpr_p_callback->AddHangTask(task);		
	}
	else
	{
		mpr_currPtGeometry->setVertexArray(vertexs);
		mpr_currPtGeometry->setColorArray(colors);
	}
	return true;
}
glbBool CGlbGlobeTerrainModify::DrawTemp(glbDouble xOrLon, glbDouble yOrLat, glbDouble zOrAlt, glbInt32 color)
{
	if (!mpt_globe || !mpr_osgOutlineGeometry)
		return false;

	osg::Geode* geode = mpr_osgOutlineGeometry->getParent(0)->asGeode();

	if (!geode)
		return false;
	
	GlbGlobeTypeEnum globeType = mpt_globe->GetType();

	osg::Vec3Array* vertexs = new osg::Vec3Array;
	osg::Vec3d pt(xOrLon,yOrLat,zOrAlt);
	osg::Vec3 origin;
	osg::Vec3d _center;
	mpr_region->GetExtent()->GetCenter(&_center.x(),&_center.y(),&_center.z());
	if(GLB_GLOBETYPE_GLOBE==globeType)
	{
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yOrLat),osg::DegreesToRadians(xOrLon),zOrAlt,pt.x(),pt.y(),pt.z());
	}
	origin = osg::Vec3(_center.x(),_center.y(),_center.z());
	vertexs->push_back(osg::Vec3(pt.x(),pt.y(),pt.z())-origin);
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(GetRValue(color)/255.0f,LOBYTE(((color & 0xFFFF)) >> 8)/255.0f,GetBValue(color)/255.0f,1.0f));

	//用像素点绘制
	if (mpr_tempPtGeometry==NULL)
	{
		mpr_tempPtGeometry = new osg::Geometry;
		mpr_tempPtGeometry->setName("TempPoint");

		// 设置点大小 默认为5个像素
		osg::Point* ptsize = new osg::Point(5);
		mpr_tempPtGeometry->getOrCreateStateSet()->setAttributeAndModes(ptsize,osg::StateAttribute::ON);

		mpr_tempPtGeometry->setVertexArray(vertexs);
		mpr_tempPtGeometry->setColorArray(colors);
		mpr_tempPtGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		mpr_tempPtGeometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS,0,vertexs->size()));

		//// 挂drawable
		////geode->addDrawable(tempGeom);
		//glbInt32 size = 0;
		//CGlbGlobeCallBack::CGlbGlobeDrawableTask* task = 
		//	new CGlbGlobeCallBack::CGlbGlobeDrawableTask(geode,mpr_tempPtGeometry.get(),size);
		//mpt_globe->mpr_p_callback->AddHangTask(task);		
	}
	else
	{
		mpr_tempPtGeometry->setVertexArray(vertexs);
		mpr_tempPtGeometry->setColorArray(colors);
		mpr_tempPtGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	}
	return true;
}
glbBool CGlbGlobeTerrainModify::RemovePoint(glbInt32 idx)
{
	if (!mpr_region)
		return false;
	glbInt32 ptCnt = mpr_region->GetCount();
	if (idx < 0 || idx > ptCnt-1)
		return false;
	mpr_region->DeletePoint(idx);

	// 偏移基准点(中心点)
	osg::Vec3 origin;
	osg::Vec3d _center;
	mpr_region->GetExtent()->GetCenter(&_center.x(),&_center.y(),&_center.z());
	GlbGlobeTypeEnum globeType = mpt_globe->GetType();
	if (GLB_GLOBETYPE_GLOBE==globeType)
	{
		osg::Vec3d worldCenter;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_center.y()),osg::DegreesToRadians(_center.x()),_center.z(),worldCenter.x(),worldCenter.y(),worldCenter.z());			
	}
	origin = osg::Vec3(_center.x(),_center.y(),_center.z());
	// 重新生成geometry的vertexarray
	if (mpr_osgOutlineGeometry)
	{
		osg::Vec3Array* vertexs = new osg::Vec3Array;
		glbDouble ptx,pty,ptz;
		for(glbInt32 i = 0; i < ptCnt; i++)
		{
			mpr_region->GetPoint(i,&ptx,&pty,&ptz);
			if (GLB_GLOBETYPE_GLOBE==globeType)
			{
				g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
			}
			vertexs->push_back(osg::Vec3(ptx,pty,ptz)-origin);
		}

		mpr_osgOutlineGeometry->setVertexArray(vertexs);
		mpr_osgOutlineGeometry->removePrimitiveSet(0,mpr_osgOutlineGeometry->getNumPrimitiveSets());
		mpr_osgOutlineGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP,0,ptCnt));
	}

	osg::Geode* geode = mpr_osgOutlineGeometry->getParent(0)->asGeode();
	if (!geode)
		return false;
	// 重新生成线上顶点points
	if (mpr_osgOutlineGeometry)
	{
		osg::Vec3Array* vertexs = new osg::Vec3Array;				
		glbDouble ptx,pty,ptz;
		for(glbInt32 i = 0; i < ptCnt; i++)
		{
			mpr_region->GetPoint(i,&ptx,&pty,&ptz);
			if (GLB_GLOBETYPE_GLOBE==globeType)
			{
				g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
			}
			vertexs->push_back(osg::Vec3(ptx,pty,ptz)-origin);
		}
		mpr_osgOutlineGeometry->setVertexArray(vertexs);
		mpr_osgOutlineGeometry->removePrimitiveSet(0,mpr_osgOutlineGeometry->getNumPrimitiveSets());		
		mpr_osgOutlineGeometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS,0,ptCnt));
	}

	// 处理当前点
	if (mpr_curPointIndex==idx && mpr_currPtGeometry)
	{// 如果删除点是当前点
		glbInt32 size = 0;
		//CGlbGlobeCallBack::CGlbGlobeDrawableTask* task = 
		//	new CGlbGlobeCallBack::CGlbGlobeDrawableTask(geode,mpr_currPtGeometry.get(),size);
		//mpt_globe->mpr_p_callback->AddRemoveTask(task);	
		mpr_currPtGeometry=NULL;
		mpr_curPointIndex = -1;
	}
	return true;
}
glbBool CGlbGlobeTerrainModify::ClearDraw()
{
	if (!mpr_osgOutlineGeometry)
		return true;

	osg::Geode* geode = mpr_osgOutlineGeometry->getParent(0)->asGeode();
	if (!geode)
		return false;

	unsigned int num = geode->getNumDrawables();
	// 摘除所有的点drawble
	for (unsigned int i = 0; i < num; i++)
	{
		osg::Drawable* drawable = geode->getDrawable(i);
		if (drawable && drawable != mpr_osgOutlineGeometry)
		{// 非外轮廓线drawable，外轮廓drawable依靠SetSelect来控制显隐
			glbInt32 size = 0;
			//glbref_ptr<CGlbGlobeCallBack::CGlbGlobeDrawableTask> task = 
			//	new CGlbGlobeCallBack::CGlbGlobeDrawableTask(geode,drawable,size);
			//mpt_globe->mpr_p_callback->AddRemoveTask(task);	
		}
	}

	mpr_lnPtsGeometry = NULL;
	mpr_currPtGeometry = NULL;
	mpr_tempPtGeometry = NULL;
	return true;
}

glbBool CGlbGlobeTerrainModify::IsShowOutline()
{
	return mpr_isShowOutline;
}
glbBool CGlbGlobeTerrainModify::SetOutlineShow(glbBool isShow)
{
	mpr_isShowOutline = isShow;
	if (mpt_node && mpt_node->asSwitch())
	{
		if (isShow)
			mpt_node->asSwitch()->setAllChildrenOn();
		else
			mpt_node->asSwitch()->setAllChildrenOff();
	}
	return true;
}

glbBool GlbGlobe::CGlbGlobeTerrainModify::Load2( xmlNodePtr* node,const glbWChar* prjPath )
{
	xmlChar* szKey = NULL;
	xmlNodePtr pnode = *node;
	CGlbGlobeRObject::Load2(node,prjPath);
	xmlNodePtr childNode = NULL;
	xmlNodePtr groupNode = pnode->parent;

	CGlbString bUse;
	CGlbWString field;
	CGlbString value;

	pnode = pnode->next;//GetRenderOrder
	if (pnode->xmlChildrenNode && (!xmlStrcmp(pnode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(pnode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&mpt_renderOrder);
		xmlFree(szKey);
	}

	pnode = pnode->next;//text
	pnode = pnode->next;//GetRObject
	pnode = pnode->next;//text
	pnode = pnode->next;//GetTooltip

	pnode = pnode->next;//text
	pnode = pnode->next;//IsBlink
	if (pnode->xmlChildrenNode && (!xmlStrcmp(pnode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(pnode->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isBlink = false;
		else
			mpt_isBlink = true;
		xmlFree(szKey);
	}

	pnode = pnode->next;//text
	pnode = pnode->next;//IsEdit
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(pnode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(pnode->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isEdit = false;
		else
			mpt_isEdit = true;
		xmlFree(szKey);
	}

	pnode = pnode->next;//text
	pnode = pnode->next;//IsGround
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(pnode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(pnode->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isGround = false;
		else
			mpt_isGround = true;
		xmlFree(szKey);
	}

	pnode = pnode->next;//text
	pnode = pnode->next;//IsSelected
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(pnode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(pnode->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isSelected = false;
		else
			mpt_isSelected = true;
		xmlFree(szKey);
	}

	pnode = pnode->next;//text
	pnode = pnode->next;//IsShow
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(pnode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(pnode->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isShow = false;
		else
			mpt_isShow = true;
		xmlFree(szKey);
	}

	pnode = pnode->next;//text
	pnode = pnode->next;//IsUseInstance
	if (pnode->xmlChildrenNode && (!xmlStrcmp(pnode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(pnode->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isUseInstance = false;
		else
			mpt_isUseInstance = true;
		xmlFree(szKey);
	}

	pnode = pnode->next;//text
	pnode = pnode->next;//isFlat
	if (pnode->xmlChildrenNode && (!xmlStrcmp(pnode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(pnode->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpr_isFlat = false;
		else
			mpr_isFlat = true;
		xmlFree(szKey);
	}

	pnode = pnode->next;//text
	pnode = pnode->next;//hei
	if (pnode->xmlChildrenNode && (!xmlStrcmp(pnode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(pnode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%lf",&mpr_height);
		xmlFree(szKey);
	}

	pnode = pnode->next;//text
	pnode = pnode->next;//mode

	szKey = pnode->xmlChildrenNode->content;
	char* str = u2g((char*)szKey);
	CGlbString symType = (char*)str;

	if(symType == "NGLB_TERRAINMODIFY_REPLACE")
	{
		mpr_modifymode = GLB_TERRAINMODIFY_REPLACE;
	}
	else if(symType == "NGLB_TERRAINMODIFY_BELOW")
	{
		mpr_modifymode = GLB_TERRAINMODIFY_BELOW;
	}
	else
	{
		mpr_modifymode = GLB_TERRAINMODIFY_UP;
	}

	pnode = pnode->next;//text
	pnode = pnode->next;//region
	childNode = pnode->xmlChildrenNode;//text
	childNode = childNode->next;//coordDimension
	glbInt32 coordDimension = -1;
	if (childNode->xmlChildrenNode && (!xmlStrcmp(childNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(childNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&coordDimension);
		xmlFree(szKey);
	}
	childNode = childNode->next;//text
	childNode = childNode->next;//hasM
	glbBool ishasM = false;
	if (childNode)
	{
		szKey = xmlNodeGetContent(childNode->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			ishasM = false;
		else
			ishasM = true;
		xmlFree(szKey);
	}
	childNode = childNode->next;//text
	childNode = childNode->next;//points
	childNode = childNode->xmlChildrenNode;//text
	childNode = childNode->next;//tagPoint3
	mpr_region = new CGlbLine(3,false);
	glbDouble tempX,tempY,tempZ;
	while(!xmlStrcmp(childNode->name, (const xmlChar *)"tagPoint3"))//组建ExtRing
	{
		childNode = childNode->xmlChildrenNode;//text
		childNode = childNode->next;//x
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%lf",&tempX);

		childNode = childNode->next;//text
		childNode = childNode->next;//y
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%lf",&tempY);

		childNode = childNode->next;//text
		childNode = childNode->next;//z
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%lf",&tempZ);
		xmlFree(szKey);
		mpr_region->AddPoint(tempX,tempY,tempZ);
		childNode = childNode->parent;
		childNode = childNode->next;//text
		childNode = childNode->next;
		if(!childNode)
			break;
	}

	return true;
}
