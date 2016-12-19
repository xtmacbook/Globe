#include "StdAfx.h"
#include "GlbGlobeRObject.h"

using namespace GlbGlobe;

CGlbGlobeRObject::CGlbGlobeRObject()
{
	mpt_isUseInstance = false;
	mpt_currLevel = -1;
	mpt_preLevel = -1;
	mpt_parent = NULL;
	mpt_node      = NULL;
	mpt_preNode   = NULL;
	mpt_loadState = false;
	mpt_maxVisibleDistance = 50000000000.0;
	mpt_minVisibleDistance = 0.0;
	mpt_renderOrder  = 0;
	mpt_loadOrder    = 0;
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE;
	mpt_isSelected   = false;
	mpt_isCliped = false;
	mpt_isShow       = true;
	mpt_isDispShow   = true;
	mpt_isBlink      = false;
	mpt_isGround     = false;
	mpt_tooltip      = L"";
	mpt_message      = L"";
	mpt_geoField     = L"";
	mpr_isInViewPort = false;
	mpr_isDestroy    = false;
	mpr_octree       = NULL;
	mpr_qtree        = NULL;
	mpt_feature      = NULL;
	mpt_featureLayer = NULL;
	mpt_isGround = true;
	mpt_HangTaskNum = 0;
	mpt_parentObj=NULL;
	mpt_isEdit = false;
	mpt_editMode = GLB_ROBJECTEDIT_DEFAULT;
	mpr_editNode = NULL;				///<编辑对象.
	mpr_customData = NULL;
	mpt_isNeedToPreCompile	= false;	//不需要预编译显示列表
	mpt_isPreCompiled		= false;	///没有编译
	mpt_isLODEnable			= false;	//不用lod
	mpt_isAllowUnLoad		= true;		//对象默认可卸载
	mpt_isCameraAltitudeAsDistance = false; //不使用相机海拔高做为相机到对象的距离
	mpt_isEnableSelect		= true;		//对象默认可以被选中
	mpt_videoIndex = -1;

	mpt_isFaded = false;
	mpt_fadeStartAlpha = 0.0;
	mpt_fadeEndAlpha = 100.0;
	mpt_fadeDurationTime = 2.0;
	mpt_fadeColor = osg::Vec4(1,1,1,1);
	mpr_clipObject = NULL;
}

CGlbGlobeRObject::~CGlbGlobeRObject()
{
	mpt_parent = NULL;
	mpt_node      = NULL;
	mpt_preNode   = NULL;
	mpt_feature      = NULL;
	mpt_featureLayer = NULL;
	mpt_parentObj=NULL;
	mpr_editNode = NULL;
	mpr_customData = NULL;
}

void CGlbGlobeRObject::SetInViewPort( glbBool isInViewPort )
{
	mpr_isInViewPort = isInViewPort;
}

glbBool CGlbGlobeRObject::IsInViewPort()
{
	return mpr_isInViewPort;
}

void CGlbGlobeRObject::SetDestroy()
{
	mpr_isDestroy = true;
}

glbBool CGlbGlobeRObject::IsDestroy()
{
	return mpr_isDestroy;
}

glbDouble CGlbGlobeRObject::GetDistance( osg::Vec3d &cameraPos,glbBool isCompute )
{
	return 0.0;
}

void CGlbGlobeRObject::SetCameraAltitudeAsDistance(glbBool isCameraAltitudeAsDistance)
{
	mpt_isCameraAltitudeAsDistance = isCameraAltitudeAsDistance;
}

glbBool CGlbGlobeRObject::IsCameraAltitudeAsDistance()
{
	return mpt_isCameraAltitudeAsDistance;
}

void CGlbGlobeRObject::LoadData( glbInt32 level )
{

}

void CGlbGlobeRObject::AddToScene()
{
	return ;
}

glbInt32 CGlbGlobeRObject::RemoveFromScene( glbBool isClean )
{
	return 0;
}

glbInt32 CGlbGlobeRObject::GetCurrLevel()
{
	return mpt_currLevel;
}

void CGlbGlobeRObject::SetUseInstance( glbBool useInstance )
{
	mpt_isUseInstance = useInstance;
}

glbBool CGlbGlobeRObject::IsUseInstance()
{
	return mpt_isUseInstance;
}

osg::Node * CGlbGlobeRObject::GetParentNode()
{
	return mpt_parent;
}

void CGlbGlobeRObject::SetParentNode( osg::Node *parent )
{
	mpt_parent = parent;
}

glbBool CGlbGlobeRObject::GetNeverLoadState()
{
	if(mpt_loadState == false && mpt_node == NULL)
		return true;
	return false;
}

void CGlbGlobeRObject::ResetLoadData()
{
	mpt_loadState = false;
}

glbBool CGlbGlobeRObject::IsLoadData()
{
	return mpt_loadState;
}
//void CGlbGlobeRObject::ResetAddToScene()
//{
//	mpt_addSceneState = false;
//}

glbDouble CGlbGlobeRObject::GetMaxVisibleDistance()
{
	return mpt_maxVisibleDistance;
}

glbDouble CGlbGlobeRObject::GetMinVisibleDistance()
{
	return mpt_minVisibleDistance;
}

glbInt32 CGlbGlobeRObject::GetRenderOrder()
{
	return mpt_renderOrder;
}

glbInt32 CGlbGlobeRObject::GetLoadOrder()
{
	return mpt_loadOrder;
}

void CGlbGlobeRObject::SetFeature( CGlbFeature *feature,const glbWChar* geoField)
{
	mpt_feature = feature;	
	mpt_geoField = geoField;
}

CGlbFeature * CGlbGlobeRObject::GetFeature()
{
	return mpt_feature.get();
}

void CGlbGlobeRObject::DrawToImage( glbByte *image/*RGBA*/, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext )
{

}

osg::Node * CGlbGlobeRObject::GetOsgNode()
{
	return mpt_node;
}

glbInt32 CGlbGlobeRObject::GetOsgNodeSize()
{
	return -1;
}

//osg::Node * CGlbGlobeRObject::GetPreOsgNode()
//{
//	return mpt_preNode;
//}

void CGlbGlobeRObject::SetFeatureLayer( CGlbGlobeLayer *featureLayer )
{
	mpt_featureLayer = featureLayer;
}

glbref_ptr<CGlbExtent> CGlbGlobeRObject::GetBound( glbBool isWorld /*= true*/ )
{
	return NULL;
}

void CGlbGlobeRObject::SetOctree( void *octree )
{
	mpr_octree = octree;
}

void * CGlbGlobeRObject::GetOctree()
{
	return mpr_octree;
}

void CGlbGlobeRObject::SetQtree( void *qtree )
{
	mpr_qtree = qtree;
}

void * CGlbGlobeRObject::GetQtree()
{
	return mpr_qtree;
}

GlbAltitudeModeEnum CGlbGlobeRObject::GetAltitudeMode()
{
	return mpt_altitudeMode;
}

void CGlbGlobeRObject::UpdateElevate()
{

}

IGlbGeometry * CGlbGlobeRObject::GetOutLine()
{
	return NULL;
}

glbBool CGlbGlobeRObject::SetRenderInfo( GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	return true;
}

GlbRenderInfo * CGlbGlobeRObject::GetRenderInfo()
{
	return NULL;
}

glbBool CGlbGlobeRObject::SetShow( glbBool isShow,glbBool isOnState/*=false*/ )
{
	return false;
}

glbBool CGlbGlobeRObject::IsShow()
{
	return mpt_isShow;
}

glbBool CGlbGlobeRObject::SetSelected( glbBool isSelected )
{
	return false;
}

glbBool CGlbGlobeRObject::IsSelected()
{
	return mpt_isSelected;
}

glbBool CGlbGlobeRObject::SetBlink( glbBool isBlink )
{
	return false;
}

glbBool CGlbGlobeRObject::IsBlink()
{
	return mpt_isBlink;
}

void CGlbGlobeRObject::DirectDraw(glbInt32 level)
{

}

const glbWChar* CGlbGlobeRObject::GetTooltip()
{
	return mpt_tooltip.c_str();
}

const glbWChar* CGlbGlobeRObject::GetRMessage()
{
	return mpt_message.c_str();
}

glbBool CGlbGlobeRObject::SetGround( glbBool ground )
{
	mpt_isGround = ground;
	return false;
}

glbBool CGlbGlobeRObject::IsGround()
{
	return mpt_isGround;
}

//glbBool CGlbGlobeRObject::IsInRemoveTasks()
//{
//	return mpt_isInRemoveTasks;
//}

//glbBool CGlbGlobeRObject::IsInHangTasks()
//{
//	return mpt_isInHangTasks;
//}

//void CGlbGlobeRObject::SetInRemoveTasks(glbBool inRemoveTasks)
//{
//	mpt_isInRemoveTasks = inRemoveTasks;
//}

void CGlbGlobeRObject::DecreaseHangTaskNum(void)
{
	mpt_HangTaskNum --;
}

//void CGlbGlobeRObject::SetAddSceneState(glbBool addScene)
//{
//	mpt_addSceneState = addScene;
//}

//void CGlbGlobeRObject::SetPreInRemoveTasks( glbBool inRemoveTasks )
//{
//	mpt_isPreInRemoveTasks = inRemoveTasks;
//}

//void CGlbGlobeRObject::SetPreInHangTasks( glbBool inHangTasks )
//{
//	mpt_isPreInHangTasks = inHangTasks;
//}

glbBool CGlbGlobeRObject::Load( xmlNodePtr* node,const glbWChar* prjPath )
{
	// object的加载
	CGlbGlobeObject::Load(node,prjPath);

	xmlChar* szKey = NULL;
	xmlNodePtr pnode = *node;	

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsUseInstance")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_isUseInstance);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsAllowUnload")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_isAllowUnLoad);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"MaxVisibleDistance")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%lf",&mpt_maxVisibleDistance);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"MinVisibleDistance")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%lf",&mpt_minVisibleDistance);	
		xmlFree(szKey);

		pnode = pnode->next;
	}	

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"RenderOrder")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_renderOrder);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"LoadOrder")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_loadOrder);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"AltitudeMode")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_altitudeMode);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsSelected")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_isSelected);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsShow")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_isShow);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsBlink")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_isBlink);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsGround")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_isGround);	
		xmlFree(szKey);

		pnode = pnode->next;
	}
	
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"ToolTip")))
	{
		szKey = xmlNodeGetContent(pnode);		
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			mpt_tooltip = glbstr.ToWString();		
			xmlFree(szKey);
			if (glbstr.length()>0)
				free(str);
		}
		pnode = pnode->next;
	}	

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Message")))
	{
		szKey = xmlNodeGetContent(pnode);		
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			mpt_message = glbstr.ToWString();		
			xmlFree(szKey);
			if (glbstr.length()>0)
				free(str);
		}
		pnode = pnode->next;
	}	

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsEnableSelect")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_isEnableSelect);
		xmlFree(szKey);

		*node = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsNeedToPreCompile")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_isNeedToPreCompile);
		xmlFree(szKey);

		*node = pnode->next;
	}	

	return true;
}

glbBool CGlbGlobeRObject::Save( xmlNodePtr node,const glbWChar* prjPath )
{
	// 来源于图层，不需要保存
	if (mpt_feature)	return false;

	// object的保存
	CGlbGlobeObject::Save(node,prjPath);
	
	char str[512];

	sprintf_s(str,"%d",mpt_isUseInstance);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseInstance", BAD_CAST str);

	sprintf_s(str,"%d",mpt_isAllowUnLoad);
	xmlNewTextChild(node, NULL, BAD_CAST "IsAllowUnload", BAD_CAST str);

	sprintf_s(str,"%.8lf",mpt_maxVisibleDistance);
	xmlNewTextChild(node, NULL, BAD_CAST "MaxVisibleDistance", BAD_CAST str);

	sprintf_s(str,"%.8lf",mpt_minVisibleDistance);
	xmlNewTextChild(node, NULL, BAD_CAST "MinVisibleDistance", BAD_CAST str);

	sprintf_s(str,"%d",mpt_renderOrder);
	xmlNewTextChild(node, NULL, BAD_CAST "RenderOrder", BAD_CAST str);

	sprintf_s(str,"%d",mpt_loadOrder);
	xmlNewTextChild(node, NULL, BAD_CAST "LoadOrder", BAD_CAST str);

	sprintf_s(str,"%d",mpt_altitudeMode);
	xmlNewTextChild(node, NULL, BAD_CAST "AltitudeMode", BAD_CAST str);

	sprintf_s(str,"%d",mpt_isSelected);
	xmlNewTextChild(node, NULL, BAD_CAST "IsSelected", BAD_CAST str);

	sprintf_s(str,"%d",mpt_isShow);
	xmlNewTextChild(node, NULL, BAD_CAST "IsShow", BAD_CAST str);

	sprintf_s(str,"%d",mpt_isBlink);
	xmlNewTextChild(node, NULL, BAD_CAST "IsBlink", BAD_CAST str);

	sprintf_s(str,"%d",mpt_isGround);
	xmlNewTextChild(node, NULL, BAD_CAST "IsGround", BAD_CAST str);

	char* szOut = g2u((char*)mpt_tooltip.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "ToolTip", BAD_CAST szOut);
	if(szOut)free(szOut);
	
	szOut = g2u((char*)mpt_message.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "Message", BAD_CAST szOut);
	if(szOut)free(szOut);	

	sprintf_s(str,"%d",mpt_isEnableSelect);
	xmlNewTextChild(node, NULL, BAD_CAST "IsEnableSelect", BAD_CAST str);	

	sprintf_s(str,"%d",mpt_isNeedToPreCompile);
	xmlNewTextChild(node, NULL, BAD_CAST "IsNeedToPreCompile", BAD_CAST str);	

	return true;
}

void CGlbGlobeRObject::SetParentObject( CGlbGlobeObject *parentObj )
{
	mpt_parentObj=parentObj;
}

CGlbGlobeObject * CGlbGlobeRObject::GetParentObject() const
{
	return mpt_parentObj;
}
glbInt32 CGlbGlobeRObject::GetCount()
{
	return 0;
}
CGlbGlobeRObject *CGlbGlobeRObject::GetRObject(glbInt32 idx)
{
	return NULL;
}
glbBool CGlbGlobeRObject::SetEdit( glbBool isEdit )
{
	mpt_isEdit = isEdit;

	return true;
}

glbBool CGlbGlobeRObject::IsEdit()
{
	return mpt_isEdit;
}

glbBool CGlbGlobeRObject::SetEditMode( GlbGlobeRObEditModeEnum editMode )
{
	mpt_editMode = editMode;
	return true;
}

GlbGlobeRObEditModeEnum CGlbGlobeRObject::GetEditMode()
{
	return mpt_editMode;
}


osg::Group * CGlbGlobeRObject::GetEditNode()
{
	return mpr_editNode;
}

void CGlbGlobeRObject::SetCustomData(osg::Object *data)
{
	mpr_customData = data;
}

osg::Object *CGlbGlobeRObject::GetCustomData()
{
	return mpr_customData;
}

void CGlbGlobeRObject::SetIsNeedToPreCompile(glbBool isNeedToPreCompile)
{
	mpt_isNeedToPreCompile = isNeedToPreCompile;
}
glbBool CGlbGlobeRObject::IsNeedToPreCompile()
{
	return mpt_isNeedToPreCompile;		 
}
void CGlbGlobeRObject::SetIsPreCompiled(glbBool isPreCompiled)
{
	mpt_isPreCompiled = isPreCompiled;
}
glbBool CGlbGlobeRObject::IsPreCompiled()
{
	return mpt_isPreCompiled;
}

void CGlbGlobeRObject::EnableLOD(glbBool enable)
{
	mpt_isLODEnable = enable;
}

glbBool CGlbGlobeRObject::IsLODEnable()
{
	return mpt_isLODEnable;
}
 glbInt32 CGlbGlobeRObject::ComputeRefLevel(glbDouble distance)
 {
	 if (mpt_isLODEnable==false)
		 return 0;

	 if (distance<mpt_minVisibleDistance || distance>mpt_maxVisibleDistance)
		 return 0;

	 glbInt32 level=0; // 默认为0级
	 if (mpt_rangeList.size()>0)
	 {// 用户设置了rangelist信息
		 glbInt32 idx=0;
		 RangeList::iterator itr = mpt_rangeList.begin();
		 while(itr != mpt_rangeList.end())
		 {
			 if (distance > itr->first && distance <= itr->second)
			 {
				 level = idx;
				 break;
			 }
			 idx++;			 
			 ++itr;
		 }
	 }
	 else
	 {// 默认方式
		 /*
		       level:3,2,1,0.  3是最精细的
			   3: modepath = dir/mdname
			   2:modepath = dir/2/mdname
			   1:modepath = dir/1/mdname
			   0:modepath = dir/0/mdname
			   
			   datalocate ------> level3 modepath
		*/
		 if (distance >= 5000)      // [5000, MAX)
			 level = 0;
		 else if (distance >= 300)  // [300, 5000)
			 level = 1;		 
		 else						// (0 , 300)
			 level = 2;
		 
		 //// 默认最多3级，等距划分
		 //int maxLOD = 3;
		 //glbDouble step = (mpt_maxVisibleDistance - mpt_minVisibleDistance) / maxLOD;
		 //for (glbInt32 idx = 0; idx < maxLOD ; idx++)
		 //{
			// glbDouble lodmin = mpt_minVisibleDistance+idx*step;
			// glbDouble lodmax = mpt_minVisibleDistance+(idx+1)*step;
			// if (distance>lodmin && distance<=lodmax)
			// {
			//	 level = idx;
			//	 break;
			// }
		 //}
	 }

	 return level;
 }

 void CGlbGlobeRObject::AddLODRange(glbInt32 level, glbDouble min, glbDouble max)
 {
	 if (level>mpt_rangeList.size()) mpt_rangeList.resize(level,MinMaxPair(min,min));
	 mpt_rangeList[level-1].first = min;
	 mpt_rangeList[level-1].second = max;
 }

 void CGlbGlobeRObject::SetIsAllowUnLoad(glbBool isAllowUnload)
 {
	 mpt_isAllowUnLoad = isAllowUnload;
 }

 glbBool CGlbGlobeRObject::IsAllowUnLoad()
 {
	 return mpt_isAllowUnLoad;
 }

 glbInt32 CGlbGlobeRObject::GetVideoIndex()
 {
	 return mpt_videoIndex;
 }

 glbBool CGlbGlobeRObject::EnableFade( glbBool isFaded )
 {
	 mpt_isFaded = isFaded;
	 return true;
 }

 glbBool CGlbGlobeRObject::IsFaded()
 {
	 return mpt_isFaded;
 }

 void CGlbGlobeRObject::SetFadeParameter( glbFloat startAlpha,glbFloat endAlpha )
 {
	 mpt_fadeStartAlpha = startAlpha;
	 if(abs(mpt_fadeEndAlpha - 100.0) < 1)//当obj的alpha被修改的时候，mpr_fadeEndAlpha应该为obj的alpha
		 mpt_fadeEndAlpha = endAlpha;
 }

 void CGlbGlobeRObject::SetFadeDurationTime( glbDouble durationTime )
 {
	 mpt_fadeDurationTime = durationTime;
 }

 void CGlbGlobeRObject::GetFadeParameter( glbFloat *startAlpha,glbFloat *endAlpha )
 {
	 if(startAlpha == NULL || endAlpha == NULL)
		 return;
	 *startAlpha = mpt_fadeStartAlpha;
	 *endAlpha = mpt_fadeEndAlpha;
 }

 glbDouble CGlbGlobeRObject::GetFadeDurationTime()
 {
	 return mpt_fadeDurationTime;
 }

 void CGlbGlobeRObject::SetCustomDouble( glbDouble customDouble )
 {
	 mpr_customDouble = customDouble;
 }

 glbDouble CGlbGlobeRObject::GetCustomDouble()
 {
	 return mpr_customDouble;
 }

 glbBool GlbGlobe::CGlbGlobeRObject::Load2( xmlNodePtr* node,const glbWChar* prjPath )
 {
	 // object的加载
	 CGlbGlobeObject::Load2(node,prjPath);

	 xmlChar* szKey = NULL;
	 xmlNodePtr pnode = *node;

	 if(node && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	 {
		 pnode = pnode->next;
	 }

	 if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"GetAltitudeMode")))
	 {
		 szKey = xmlNodeGetContent(pnode);
		 char* str = u2g((char*)szKey);
		 CGlbString tempValue = (char*)str;
		 if(tempValue == "NGLB_ALTITUDEMODE_ABSOLUTE")
			 mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE;
		 else if(tempValue == "NGLB_ALTITUDEMODE_RELATIVETERRAIN")
			 mpt_altitudeMode = GLB_ALTITUDEMODE_RELATIVETERRAIN;
		 else
			 mpt_altitudeMode = GLB_ALTITUDEMODE_ONTERRAIN;
		 xmlFree(szKey);
		 pnode = pnode->next;
	 }

	 if(node && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	 {
		 pnode = pnode->next;
	 }

	 if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"GetCurrLevel")))
	 {
		 szKey = xmlNodeGetContent(pnode);
		 //sscanf_s((char*)szKey,"%d",&mpt_currLevel);	
		 xmlFree(szKey);

		 pnode = pnode->next;
	 }

	 if(node && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	 {
		 pnode = pnode->next;
	 }

	 if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"GetEditMode")))
	 {
		 szKey = xmlNodeGetContent(pnode);
		 char* str = u2g((char*)szKey);
		 CGlbString tempValue = (char*)str;
		 if(tempValue == "NGLB_ROBJECTEDIT_HORIZONTAL")
			 mpt_editMode = GLB_ROBJECTEDIT_HORIZONTAL;
		 else if(tempValue == "NGLB_ROBJECTEDIT_VERTICAL")
			 mpt_editMode = GLB_ROBJECTEDIT_VERTICAL;
		 else if(tempValue == "NGLB_ROBJECTEDIT_SHAPE")
			 mpt_editMode = GLB_ROBJECTEDIT_SHAPE;
		 else
			 mpt_editMode = GLB_ROBJECTEDIT_DEFAULT;
		 xmlFree(szKey);
		 pnode = pnode->next;
	 }

	 if(node && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	 {
		 pnode = pnode->next;
	 }

	 if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"GetLoadOrder")))
	 {
		 szKey = xmlNodeGetContent(pnode);
		 sscanf_s((char*)szKey,"%d",&mpt_loadOrder);	
		 xmlFree(szKey);

		 pnode = pnode->next;
	 }

	 if(node && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	 {
		 pnode = pnode->next;
	 }

	 if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"GetMaxVisibleDistance")))
	 {
		 szKey = xmlNodeGetContent(pnode);
		 sscanf_s((char*)szKey,"%lf",&mpt_maxVisibleDistance);	
		 xmlFree(szKey);

		 pnode = pnode->next;
	 }

	 if(node && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	 {
		 pnode = pnode->next;
	 }

	 if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"GetMessage")))
	 {
		 szKey = xmlNodeGetContent(pnode);		
		 char* str = u2g((char*)szKey);
		 if(str)
		 {
			 CGlbString glbstr = str;
			 mpt_message = glbstr.ToWString();		
			 xmlFree(szKey);
			 if (glbstr.length()>0)
				 free(str);
		 }
		 pnode = pnode->next;
	 }

	 if(node && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	 {
		 pnode = pnode->next;
	 }

	 if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"GetMinVisibleDistance")))
	 {
		 szKey = xmlNodeGetContent(pnode);
		 sscanf_s((char*)szKey,"%lf",&mpt_minVisibleDistance);	
		 xmlFree(szKey);

		 pnode = pnode->next;
	 }
	 return true;
 }

 glbBool GlbGlobe::CGlbGlobeRObject::interset(osg::Vec3d vtStart, osg::Vec3d vtEnd, osg::Vec3d& interPoint)
 {
	 return false;
 }

 glbBool GlbGlobe::CGlbGlobeRObject::SetCliped( glbBool isCliped )
 {
	 mpt_isCliped = isCliped;
	 if(isCliped == false && mpr_clipObject)
	 {
		 mpr_clipObject->Clip(this,false);
	 }
	 return true;
 }

 glbBool GlbGlobe::CGlbGlobeRObject::IsCliped()
 {
	 return mpt_isCliped;
 }

 void GlbGlobe::CGlbGlobeRObject::SetClipObject( CGlbClipObject *clipObject )
 {
	 mpr_clipObject = clipObject;
 }

void CGlbGlobeRObject::EnableSelect(glbBool isEnableSelect)
{
	mpt_isEnableSelect = isEnableSelect;
}
glbBool CGlbGlobeRObject::IsSelectEnable()
{
	return mpt_isEnableSelect;
}