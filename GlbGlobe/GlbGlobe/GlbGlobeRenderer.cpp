#include "StdAfx.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeRenderer.h"
#include "GlbGlobeREObject.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbDrill.h"
#include "GlbSection.h"
using namespace GlbGlobe;

CGlbGlobeRenderer::CGlbGlobeRenderer(void)
{
	mpr_critical.SetName(L"globe_renderer");
	mpr_name         = L"";
	mpr_geofield     = L"";
	mpr_labelfield   = L"";
	mpr_isground     = true;
	mpr_renderinfo   = NULL;
	mpr_mindistance  = NULL;	
	mpr_maxdistance  = NULL;	
	mpr_loadorder    = NULL;		
	mpr_renderorder  = NULL;	
	mpr_message      = NULL;		
	mpr_tooltips     = NULL;				
	mpr_altitudemode = GLB_ALTITUDEMODE_ABSOLUTE;
	mpr_node         = new osg::Switch;	
	mpr_globe        = NULL;
	mpr_isShow		 = true;
	mpr_isCameraAltitudeAsDistance = false;

	mpr_isFaded = false;
	mpr_fadeStartAlpha = 0.0;
	mpr_fadeEndAlpha = 100.0;
	mpr_fadeDurationTime = 2.0;

	mpr_isNeedToPreCompile = true; //启动预编译显示列表
}

CGlbGlobeRenderer::~CGlbGlobeRenderer(void)
{
	mpr_node = NULL;
	if (mpr_mindistance)
		delete mpr_mindistance;
	if (mpr_maxdistance)
		delete mpr_maxdistance;
	if (mpr_loadorder)
		delete mpr_loadorder;
	if (mpr_renderorder)
		delete mpr_renderorder;
	if (mpr_message)
		delete mpr_message;
	if (mpr_tooltips)
		delete mpr_tooltips;
	//清理对象.
	if(mpr_globe!=NULL)
	{
		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
		for(itr;itr!=mpr_objects.end();itr++)
		{
			CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(itr->second.get());
			if(robj)
			{							
				/*
					对象销毁时,会RemoveFromScene,而RemoveFromScene会用到featurelayer.
					但是： 在此析构时,对象销毁之时,featurelayer可能已经不存在了.
					所以：
					     robj->SetFeatureLayer(NULL);
				*/
				robj->SetFeatureLayer(NULL);
				robj->SetDestroy();	
				if(mpr_globe->mpr_sceneobjIdxManager)mpr_globe->mpr_sceneobjIdxManager->RemoveObject(robj);
			}
		}
	}
	mpr_objects.clear();
	mpr_renderinfo = NULL;		
}

const glbWChar* CGlbGlobeRenderer::GetName()
{
	return mpr_name.c_str();
}
glbBool CGlbGlobeRenderer::SetName(glbWChar* name)
{
	if (name==NULL)
		return false;
	mpr_name = name;
	return true;
}
glbBool CGlbGlobeRenderer::SetGeoField(glbWChar* field)
{// 建议不开放这个功能，如果需要修改则新建一个renderer，将旧的renderer删除
	if (mpr_geofield.empty())
		mpr_geofield = field;
	else
	{// 重新生成obj? 如果是多个renderer的话就有问题了

	}
	return true;
}
const glbWChar* CGlbGlobeRenderer::GetGeoField()
{
	return mpr_geofield.c_str();
}
glbBool CGlbGlobeRenderer::SetLabelField(glbWChar* field)
{//建议不开放这个功能，如果需要修改则新建一个renderer，将旧的renderer删除
	if (mpr_labelfield.empty())
		mpr_labelfield = field;
	else
	{// 重新生成obj?

	}
	return true;
}
const glbWChar* CGlbGlobeRenderer::GetLabelField()
{
	return mpr_labelfield.c_str();
}
glbBool CGlbGlobeRenderer::SetRenderInfo(GlbRenderInfo* rinfo)
{
	if (!rinfo) return false;

	if (mpr_renderinfo==NULL)
		mpr_renderinfo = rinfo;
	else
	{// 遍历所有的object，重新设置renderer		
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);

		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
		glbBool isNeedDirectDraw = false;
		for(; itr != mpr_objects.end(); ++itr)
		{// 找到
			CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(itr->second.get());
			if (robj)
			{
				robj->SetRenderInfo(rinfo,isNeedDirectDraw);
				if(isNeedDirectDraw)robj->DirectDraw(max(robj->GetCurrLevel(),0));
				isNeedDirectDraw = false;
			}
		}		
		mpr_renderinfo = rinfo;
	
	}
	return true;
}	

GlbRenderInfo* CGlbGlobeRenderer::GetRenderInfo()
{
	return mpr_renderinfo.get();
}
GlbRenderDouble* CGlbGlobeRenderer::GetMinDistance()
{
	return mpr_mindistance;
}
glbBool CGlbGlobeRenderer::SetMinDistance(GlbRenderDouble* dis)
{
	if (!dis) return false;

	if (!mpr_mindistance)
		mpr_mindistance = dis;
	else
	{
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
		for(; itr != mpr_objects.end(); ++itr)
		{// 找到
			CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
			if (robj)
			{			
				robj->SetMinVisibleDistance(dis->GetValue(robj->GetFeature()));
			}
		}
		mpr_mindistance = dis;
	}

	return true;
}
GlbRenderDouble* CGlbGlobeRenderer::GetMaxDistance()
{
	return mpr_maxdistance;
}
glbBool CGlbGlobeRenderer::SetMaxDistance(GlbRenderDouble* dis)
{
	if (!dis) return false;
	if (!mpr_maxdistance)
		mpr_maxdistance = dis;
	else
	{
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
		for(; itr != mpr_objects.end(); ++itr)
		{// 找到
			CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
			if (robj)
			{			
				robj->SetMaxVisibleDistance(dis->GetValue(robj->GetFeature()));
			}			
		}
		mpr_maxdistance = dis;
	}

	return true;
}
GlbRenderInt32* CGlbGlobeRenderer::GetRenderOrder()
{
	return mpr_renderorder;
}
glbBool CGlbGlobeRenderer::SetRenderOrder(GlbRenderInt32* order)
{
	if (!order) return false;

	if (!mpr_renderorder)
		mpr_renderorder = order;
	else
	{
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
		for(; itr != mpr_objects.end(); ++itr)
		{// 找到
			CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
			if (robj)
			{			
				robj->SetRenderOrder(order->GetValue(robj->GetFeature()));
			}			
		}
		mpr_renderorder = order;
	}

	return true;
}
GlbRenderInt32* CGlbGlobeRenderer::GetLoadOrder()
{
	return mpr_loadorder;
}
glbBool CGlbGlobeRenderer::SetLoadOrder(GlbRenderInt32* order)
{
	if (!order) return false;

	if(!mpr_loadorder)
		mpr_loadorder = order;
	else
	{
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
		for(; itr != mpr_objects.end(); ++itr)
		{// 找到
			CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
			if (robj)
			{			
				robj->SetLoadOrder(order->GetValue(robj->GetFeature()));
			}			
		}
		mpr_loadorder = order;
	}

	return true;
}
GlbRenderString* CGlbGlobeRenderer::GetRMessage()
{
	return mpr_message;
}
glbBool CGlbGlobeRenderer::SetRMessage(GlbRenderString* msg)
{
	if (!msg) return false;
	if (!mpr_message)
		mpr_message = msg;
	else
	{
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
		for(; itr != mpr_objects.end(); ++itr)
		{// 找到
			CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
			if (robj)
			{			
				robj->SetRMessage(msg->GetValue(robj->GetFeature()));
			}
		}
		mpr_message = msg;
	}

	return true;
}
GlbRenderString* CGlbGlobeRenderer::GetToolTip()
{
	return mpr_tooltips;
}
glbBool CGlbGlobeRenderer::SetToolTip(GlbRenderString* tips)
{
	if (!tips) return false;
	if (!mpr_tooltips)
		mpr_tooltips = tips;
	else
	{
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
		for(; itr != mpr_objects.end(); ++itr)
		{// 找到
			CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
			if (robj)
			{			
				robj->SetToolTip(tips->GetValue(robj->GetFeature()));
			}
		}
		mpr_tooltips = tips;
	}

	return true;
}

glbBool CGlbGlobeRenderer::SetAltitudeMode(GlbAltitudeModeEnum mode)
{
	if (mpr_altitudemode==mode)
		return true;
	mpr_altitudemode = mode;

GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
	glbBool isNeedDirectDraw = false;
	for(; itr != mpr_objects.end(); ++itr)
	{
		CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
		if (robj)
		{			
			robj->SetAltitudeMode(mode,isNeedDirectDraw);
			if(isNeedDirectDraw)robj->DirectDraw(max(robj->GetCurrLevel(),0));
			isNeedDirectDraw = false;
		}
	}

	return true;
}
GlbAltitudeModeEnum CGlbGlobeRenderer::GetAltitudeMode()
{
	return mpr_altitudemode;
}
glbBool CGlbGlobeRenderer::SetGround(glbBool isground)
{
	if (isground==mpr_isground)return true;

	mpr_isground = isground;
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
	for(; itr != mpr_objects.end(); ++itr)
	{// 找到
		CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
		if (robj)
		{			
			robj->SetGround(isground);
		}		
	}
	return true;
}
glbBool CGlbGlobeRenderer::IsGround()
{
	return mpr_isground;
}

glbBool CGlbGlobeRenderer::SetShow(glbBool isShow)
{
	if (mpr_isShow==isShow)
		return true;
	mpr_isShow = isShow;

GlbScopedLock<GlbCriticalSection> lock(mpr_critical);

	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
	while (itr != mpr_objects.end())
	{// 找到
		CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>((itr->second).get());
		if (robj)
			robj->SetShow(mpr_isShow,false);

		itr++;
	}
	return true;
}

CGlbGlobeObject* CGlbGlobeRenderer::GetObjectById(glbInt32 objId)
{
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);

	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.find(objId);
	if (itr != mpr_objects.end())
	{// 找到
		return itr->second.get();
	}
	return NULL;
}
glbBool CGlbGlobeRenderer::AddObject(CGlbGlobeObject* obj)
{
	{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		glbInt32 id = obj->GetId();
		mpr_objects[id] = obj;
		if (mpr_node)
		{
			CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(obj);
			if (robj)
			{
				robj->SetParentNode(mpr_node.get());
			}
		}
	}

	// 拷贝renderer属性
	UpdateAttribute(obj);
	return true;
}
void CGlbGlobeRenderer::UpdateAttribute(CGlbGlobeObject* obj)
{
	CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(obj); 
	if (robj)
	{
		CGlbFeature* feature = robj->GetFeature();	
		// MinDistance
		if (mpr_mindistance)
		{
			glbDouble minDistance = mpr_mindistance->GetValue(feature);
			robj->SetMinVisibleDistance(minDistance);
		}
		// MaxDistance
		if (mpr_maxdistance)
		{
			glbDouble maxDistance = mpr_maxdistance->GetValue(feature);
			robj->SetMaxVisibleDistance(maxDistance);				 
		}
		// LoadOrder
		if (mpr_loadorder)
		{
			glbInt32 loadorder = mpr_loadorder->GetValue(feature);
			robj->SetLoadOrder(loadorder);
		}
		// RenderOrder
		if (mpr_renderorder)
		{
			glbInt32 renderorder = mpr_renderorder->GetValue(feature);
			robj->SetRenderOrder(renderorder);
		}
		// renderInfo
		glbBool isNeedDirectDraw = false;
		robj->SetRenderInfo(mpr_renderinfo.get(),isNeedDirectDraw);
		// message
		if (mpr_message)
		{
			glbWChar* message = mpr_message->GetValue(feature);
			robj->SetRMessage(message);
		}
		// tooltip
		if (mpr_tooltips)
		{
			glbWChar* tooltip = mpr_tooltips->GetValue(feature);
			robj->SetToolTip(tooltip);
		}

		// altitudemode
		robj->SetAltitudeMode(mpr_altitudemode,isNeedDirectDraw);

		// show
		robj->SetShow(mpr_isShow);
		// 是否相机高度做为距离
		robj->SetCameraAltitudeAsDistance(mpr_isCameraAltitudeAsDistance);

		// fade
		//if (mpr_renderinfo && mpr_renderinfo->type==GLB_OBJECTTYPE_POINT)
		//{
		//	GlbMarkerModelSymbolInfo *modeInfo = dynamic_cast<GlbMarkerModelSymbolInfo *>(mpr_renderinfo.get());
		//	if (modeInfo)
		//		mpr_isFaded = true;							
		//}
		robj->EnableFade(mpr_isFaded);
		robj->SetFadeParameter(mpr_fadeStartAlpha,mpr_fadeEndAlpha);
		robj->SetFadeDurationTime(mpr_fadeDurationTime);

		robj->SetIsNeedToPreCompile(mpr_isNeedToPreCompile);
	}
}
glbBool CGlbGlobeRenderer::RemoveObject(glbInt32 objId)
{
	glbBool res = false;
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.find(objId);
	if (itr != mpr_objects.end())
	{// 找到
		CGlbGlobeObject* p_obj = itr->second.get();
		//从场景管理器删除对象
		CGlbGlobeRObject* p_renderable_obj = dynamic_cast<CGlbGlobeRObject*>(p_obj);
		if(p_renderable_obj)
		{			
			CGlbGlobe* p_globe = p_renderable_obj->GetGlobe();
			if (p_globe)
			{
				p_globe->mpr_sceneobjIdxManager->RemoveObject(p_renderable_obj);
				p_renderable_obj->SetFeature(NULL);
				// 设置销毁标志
				p_renderable_obj->SetDestroy();

				// 删除
				itr = mpr_objects.erase(itr);
			}
		}		
		res = true;
	}

	return res;
}

glbBool CGlbGlobeRenderer::RemoveAllObjects()
{
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
	while(itr != mpr_objects.end())
	{
		CGlbGlobeObject* p_obj = itr->second.get();
		//从场景管理器删除对象
		CGlbGlobeRObject* p_renderable_obj = dynamic_cast<CGlbGlobeRObject*>(p_obj);
		if(p_renderable_obj)
		{			
			CGlbGlobe* p_globe = p_renderable_obj->GetGlobe();
			if (p_globe)
			{
				p_globe->mpr_sceneobjIdxManager->RemoveObject(p_renderable_obj);
				p_renderable_obj->SetFeature(NULL);
				// 设置销毁标志
				p_renderable_obj->SetDestroy();
			}
		}		
		itr++;
	}

	mpr_objects.clear();

	return true;
}
glbInt32 CGlbGlobeRenderer::GetObjectCount()
{
	return mpr_objects.size();
}
osg::Node* CGlbGlobeRenderer::GetNode()
{
	return mpr_node.get();
}

CGlbGlobeObject* CGlbGlobeRenderer::CreateObject(CGlbFeature* feature)
{
	CGlbGlobeObject* obj = NULL;	
	CGlbDrill   *drill   = dynamic_cast<CGlbDrill*>(feature);
	if (drill)
	{// 钻孔数据
		obj = g_objectFactory.GetObject(L"GlbGlobeDrill");	
		return obj;
	}

	CGlbSection	*section = dynamic_cast<CGlbSection*>(feature);	
	if (section)
	{// 剖面
		obj = g_objectFactory.GetObjectW(L"GlbGlobeSection");
		return obj;
	}	

	{// 网络对象 - 通过查询GlbEdgeID 是否存在来判断是否是网络对象 ？？ 
		const GLBVARIANT* va = feature->GetValueByName(L"GlbEdgeID");
		if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
		{
			obj = g_objectFactory.GetObjectW(L"GlbGlobeNetwork");
			return obj;
		}
	}
	
	CGlbFields* p_fields = const_cast<CGlbFields*>(feature->GetFields());
	CGlbField* p_field = p_fields->GetFieldByName(mpr_geofield.c_str());
	if (p_field == NULL) // 没有Geo字段，直接返回NULL
		return NULL;

	GlbGeometryTypeEnum geotype = p_field->GetGeoType();
	switch(geotype)
	{
	case GLB_GEO_POINT:				///<二维点	
	case GLB_GEO_MULTIPOINT:		///<二维多点
		obj = g_objectFactory.GetObject(L"GlbGlobePoint");		
		break;
	case GLB_GEO_LINE:				///<二维线	
	case GLB_GEO_MULTILINE:			///<二维多线	
		obj = g_objectFactory.GetObject(L"GlbGlobeLine");	
		break;
	case GLB_GEO_POLYGON:			///<二维多边形	
	case GLB_GEO_MULTIPOLYGON:		///<二维多多边形
		obj = g_objectFactory.GetObject(L"GlbGlobePolygon");	
		break;
	case GLB_GEO_TIN:				///<三角网
	case GLB_GEO_MULTITIN:
		obj = g_objectFactory.GetObject(L"GlbGlobeTin");	
		break;
	case GLB_GEO_TEXT:				///<二维文字
	//case GLB_GEO_MODEL:				///<模型
	case GLB_GEO_COLLECT:			 ///<几何集合
		break;
	case GLB_GEO_UNKNOWN:
		break;
	}	

	//CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(obj);
	//if (robj)
	//	robj->SetFeature(feature);
	return obj;
}

glbBool CGlbGlobeRenderer::Load(xmlNodePtr node,glbWChar* relativepath )
{
	xmlChar* szKey = NULL;	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Name")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		mpr_name = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}			
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GeoField")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		mpr_geofield = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LabelField")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			mpr_labelfield = glbstr.ToWString();
			xmlFree(szKey);
			if (glbstr.length()>0)
				free(str);
		}
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsGround")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_isground);
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"AltitudeMode")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_altitudemode);
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"MinDistance")))
	{		
		xmlNodePtr minDis_node = node->children;
		if (!mpr_mindistance)
			mpr_mindistance = new GlbRenderDouble;
		mpr_mindistance->Load(minDis_node);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"MaxDistance")))
	{		
		xmlNodePtr maxDis_node = node->children;
		if (!mpr_maxdistance)
			mpr_maxdistance = new GlbRenderDouble;
		mpr_maxdistance->Load(maxDis_node);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LoadOrder")))
	{		
		xmlNodePtr loadorder_node = node->children;
		if (!mpr_loadorder)
			mpr_loadorder = new GlbRenderInt32;
		mpr_loadorder->Load(loadorder_node);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RenderOrder")))
	{		
		xmlNodePtr renderorder_node = node->children;
		if (!mpr_renderorder)
			mpr_renderorder = new GlbRenderInt32;
		mpr_renderorder->Load(renderorder_node);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Message")))
	{		
		xmlNodePtr message_node = node->children;
		if (!mpr_message)
			mpr_message = new GlbRenderString;
		mpr_message->Load(message_node);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Tooltips")))
	{		
		xmlNodePtr tooltips_node = node->children;
		if (!mpr_tooltips)
			mpr_tooltips = new GlbRenderString;
		mpr_tooltips->Load(tooltips_node);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RendererInfo")))
	{
		xmlNodePtr rendererinfo_node = node->children;		
		mpr_renderinfo = glbLoadRenderInfo(rendererinfo_node,relativepath);		
	}
	return true;
}
glbBool CGlbGlobeRenderer::Save(xmlNodePtr node, glbWChar* relativepath)
{
	char* szOut = g2u((char*)mpr_name.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "Name", BAD_CAST mpr_name.ToString().c_str());
	free(szOut);

	szOut = g2u((char*)mpr_geofield.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "GeoField", BAD_CAST mpr_geofield.ToString().c_str());
	free(szOut);

	szOut = g2u((char*)mpr_labelfield.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "LabelField", BAD_CAST mpr_labelfield.ToString().c_str());
	free(szOut);

	char str[128];
	sprintf(str,"%d",mpr_isground);
	xmlNewTextChild(node, NULL, BAD_CAST "IsGround", BAD_CAST str);

	sprintf(str,"%d",mpr_altitudemode);
	xmlNewTextChild(node, NULL, BAD_CAST "AltitudeMode", BAD_CAST str);

	if (mpr_mindistance)
	{
		xmlNodePtr minDis_node = xmlNewNode(NULL,BAD_CAST "MinDistance");
		xmlAddChild(node,minDis_node);		

		mpr_mindistance->Save(minDis_node);	
	}

	if (mpr_maxdistance)
	{
		xmlNodePtr maxDis_node = xmlNewNode(NULL,BAD_CAST "MaxDistance");
		xmlAddChild(node,maxDis_node);		

		mpr_maxdistance->Save(maxDis_node);
	}

	if (mpr_loadorder)
	{
		xmlNodePtr loadorder_node = xmlNewNode(NULL,BAD_CAST "LoadOrder");
		xmlAddChild(node,loadorder_node);		

		mpr_loadorder->Save(loadorder_node);
	}

	if (mpr_renderorder)
	{
		xmlNodePtr renderorder_node = xmlNewNode(NULL,BAD_CAST "RenderOrder");
		xmlAddChild(node,renderorder_node);		

		mpr_renderorder->Save(renderorder_node);
	}

	if (mpr_message)
	{
		xmlNodePtr message_node = xmlNewNode(NULL,BAD_CAST "Message");
		xmlAddChild(node,message_node);		

		mpr_message->Save(message_node);
	}

	if (mpr_tooltips)
	{
		xmlNodePtr tooltips_node = xmlNewNode(NULL,BAD_CAST "Tooltips");
		xmlAddChild(node,tooltips_node);		

		mpr_tooltips->Save(tooltips_node);
	}
	if (mpr_renderinfo)
	{
		xmlNodePtr renderinfo_node = xmlNewNode(NULL,BAD_CAST "RendererInfo");
		xmlAddChild(node,renderinfo_node);

		char str[32];
		sprintf_s(str,"%d",mpr_renderinfo->type);
		xmlNewTextChild(renderinfo_node, NULL, BAD_CAST "RenderType", BAD_CAST str);

		glbInt32 symbolType = 0;
		switch (mpr_renderinfo->type)
		{
		case GLB_OBJECTTYPE_POINT:
			{
				GlbMarkerModelSymbolInfo *modeInfo = dynamic_cast<GlbMarkerModelSymbolInfo *>(mpr_renderinfo.get());
				GlbMarker2DShapeSymbolInfo *p2DInfo = dynamic_cast<GlbMarker2DShapeSymbolInfo *>(mpr_renderinfo.get());
				GlbMarker3DShapeSymbolInfo *p3DInfo = dynamic_cast<GlbMarker3DShapeSymbolInfo *>(mpr_renderinfo.get());
				GlbMarkerPixelSymbolInfo *pixelInfo = dynamic_cast<GlbMarkerPixelSymbolInfo *>(mpr_renderinfo.get());
				GlbMarkerLabelSymbolInfo *labelInfo = dynamic_cast<GlbMarkerLabelSymbolInfo *>(mpr_renderinfo.get());
				GlbMarkerImageSymbolInfo *imageInfo = dynamic_cast<GlbMarkerImageSymbolInfo *>(mpr_renderinfo.get());
				if(modeInfo)
					symbolType = 0;
				else if(p2DInfo)
					symbolType = 1;
				else if(p3DInfo)
					symbolType = 2;
				else if(pixelInfo)
					symbolType = 3;
				else if(labelInfo)
					symbolType = 4;
				else
					symbolType = 5;
			}
			break;
		case GLB_OBJECTTYPE_LINE:
			{
				GlbLinePixelSymbolInfo *pixelInfo = dynamic_cast<GlbLinePixelSymbolInfo *>(mpr_renderinfo.get());
				GlbLine2DSymbolInfo *l2DInfo = dynamic_cast<GlbLine2DSymbolInfo *>(mpr_renderinfo.get());
				GlbLine3DSymbolInfo *l3DInfo = dynamic_cast<GlbLine3DSymbolInfo *>(mpr_renderinfo.get());
				GlbLineArrowSymbolInfo *arrowInfo = dynamic_cast<GlbLineArrowSymbolInfo *>(mpr_renderinfo.get());
				if(pixelInfo)
					symbolType = 0;
				else if(l2DInfo)
					symbolType = 1;
				else if(l3DInfo)
					symbolType = 2;
				else
					symbolType = 3;
			}
			break;
		case GLB_OBJECTTYPE_POLYGON:
			{
				GlbPolygon2DSymbolInfo *p2DInfo = dynamic_cast<GlbPolygon2DSymbolInfo *>(mpr_renderinfo.get());
				GlbPolyhedronSymbolInfo *polyhedronInfo = dynamic_cast<GlbPolyhedronSymbolInfo *>(mpr_renderinfo.get());
				GlbPolygonBuildingSymbolInfo *buildingInfo = dynamic_cast<GlbPolygonBuildingSymbolInfo *>(mpr_renderinfo.get());
				GlbPolygonWaterSymbolInfo *waterInfo = dynamic_cast<GlbPolygonWaterSymbolInfo *>(mpr_renderinfo.get());
				if(p2DInfo)
					symbolType = 0;
				else if(polyhedronInfo)
					symbolType = 1;
				else if(buildingInfo)
					symbolType = 2;
				else
					symbolType = 3;
			}
			break;
		case GLB_OBJECTTYPE_DRILL:
			mpr_renderinfo->Save(renderinfo_node,relativepath);
			return true;
		case GLB_OBJECTTYPE_TIN:
			mpr_renderinfo->Save(renderinfo_node,relativepath);
			return true;
		case GLB_OBJECTTYPE_SECTION:
			mpr_renderinfo->Save(renderinfo_node,relativepath);
			return true;
		case GLB_OBJECTTYPE_NETWORK:
			mpr_renderinfo->Save(renderinfo_node,relativepath);
			return true;
		default:
			return false;
		}
		sprintf_s(str,"%d",symbolType);
		xmlNewTextChild(renderinfo_node, NULL, BAD_CAST "SymbolType", BAD_CAST str);

		mpr_renderinfo->Save(renderinfo_node,relativepath);
	}	

	//sprintf(str,"%d",mpr_isNeedToPreCompile);
	//xmlNewTextChild(node, NULL, BAD_CAST "IsNeedToPreCompile", BAD_CAST str);
	return true;
}

void CGlbGlobeRenderer::SetIsAllowUnLoad(glbBool isAllowUnload)
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
	for(; itr != mpr_objects.end(); ++itr)
	{// 找到
		CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
		if (robj)
		{			
			robj->SetIsAllowUnLoad(isAllowUnload);
		}			
	}		
}

void CGlbGlobeRenderer::SetCameraAltitudeAsDistance(glbBool isCameraAltitudeAsDistance)
{
	if (mpr_isCameraAltitudeAsDistance == isCameraAltitudeAsDistance)
		return;
	mpr_isCameraAltitudeAsDistance = isCameraAltitudeAsDistance;

	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
	for(; itr != mpr_objects.end(); ++itr)
	{// 找到
		CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
		if (robj)
		{			
			robj->SetCameraAltitudeAsDistance(isCameraAltitudeAsDistance);
		}			
	}		
}

glbBool CGlbGlobeRenderer::IsCameraAltitudeAsDistance()
{
	return mpr_isCameraAltitudeAsDistance;
}

void CGlbGlobeRenderer::SetIsFade(glbBool isFaded)
{
	mpr_isFaded = isFaded;
}

glbBool CGlbGlobeRenderer::IsFaded()
{
	return mpr_isFaded;
}

void CGlbGlobeRenderer::SetFadeParameter( glbFloat startAlpha,glbFloat endAlpha )
{
	mpr_fadeStartAlpha = startAlpha;
	mpr_fadeEndAlpha = endAlpha;
}

void CGlbGlobeRenderer::SetFadeDurationTime( glbDouble durationTime )
{
	mpr_fadeDurationTime = durationTime;
}

void CGlbGlobeRenderer::GetFadeParameter( glbFloat *startAlpha,glbFloat *endAlpha )
{
	if(startAlpha == NULL || endAlpha == NULL)
		return;
	*startAlpha = mpr_fadeStartAlpha;
	*endAlpha = mpr_fadeEndAlpha;
}

glbDouble CGlbGlobeRenderer::GetFadeDurationTime()
{
	return mpr_fadeDurationTime;
}

glbBool GlbGlobe::CGlbGlobeRenderer::Load2( xmlNodePtr node,glbWChar* relativepath )
{
	xmlChar* szKey = NULL;	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetAltitudeMode")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		if(glbstr == "NGLB_ALTITUDEMODE_RELATIVETERRAIN")
			mpr_altitudemode = GLB_ALTITUDEMODE_RELATIVETERRAIN;
		else if(glbstr == "NGLB_ALTITUDEMODE_ONTERRAIN")
			mpr_altitudemode = GLB_ALTITUDEMODE_ONTERRAIN;
		else
			mpr_altitudemode = GLB_ALTITUDEMODE_ABSOLUTE;
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetGeoField")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		mpr_geofield = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetLabelField")))
	{
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			mpr_labelfield = glbstr.ToWString();
			xmlFree(szKey);
			if (glbstr.length()>0)
				free(str);
		}
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetLoadOrder")))
	{		
		xmlNodePtr loadorder_node = node->children;
		if (!mpr_loadorder)
			mpr_loadorder = new GlbRenderInt32;
		mpr_loadorder->Load2(loadorder_node);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetMaxDistance")))
	{		
		xmlNodePtr maxDis_node = node->children;
		if (!mpr_maxdistance)
			mpr_maxdistance = new GlbRenderDouble;
		mpr_maxdistance->Load2(maxDis_node);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetMessage")))
	{		
		xmlNodePtr message_node = node->children;
		if (!mpr_message)
			mpr_message = new GlbRenderString;
		mpr_message->Load2(message_node);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetMinDistance")))
	{		
		xmlNodePtr minDis_node = node->children;
		if (!mpr_mindistance)
			mpr_mindistance = new GlbRenderDouble;
		mpr_mindistance->Load2(minDis_node);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetName")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		mpr_name = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetRenderInfo")))
	{
		xmlNodePtr rendererinfo_node = node->children;//text
		mpr_renderinfo = glbLoadRenderInfo2(rendererinfo_node,relativepath);	
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetRenderOrder")))
	{		
		xmlNodePtr renderorder_node = node->children;
		if (!mpr_renderorder)
			mpr_renderorder = new GlbRenderInt32;
		mpr_renderorder->Load2(renderorder_node);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetToolTip")))
	{		
		xmlNodePtr tooltips_node = node->children;
		if (!mpr_tooltips)
			mpr_tooltips = new GlbRenderString;
		mpr_tooltips->Load2(tooltips_node);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsGround")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "false")
			mpr_isground = false;
		else
			mpr_isground = true;
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsCameraAltitudeAsDistance")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "false")
			mpr_isCameraAltitudeAsDistance = false;
		else
			mpr_isCameraAltitudeAsDistance = true;
		xmlFree(szKey);
		node = node->next;
	}
	return true;
}

void CGlbGlobeRenderer::SetCollisionDetect(glbBool isCollisionDetected,glbInt32 layerId)
{	
	if (mpr_globe==NULL) return;
	// 通知渲染器显隐
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	if (isCollisionDetected)
	{// 如果需要做碰撞检测
		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.begin();
		for(; itr != mpr_objects.end(); ++itr)
		{// 找到
			CGlbGlobeREObject* robj = dynamic_cast<CGlbGlobeREObject*>(itr->second.get());
			if (robj)
			{			
				mpr_globe->GetView()->AddCollisionLaysObject(layerId,robj);
			}			
		}
	}		
}

void CGlbGlobeRenderer::SetIsNeedToPreCompile(glbBool isNeedToPreCompile)
{
	mpr_isNeedToPreCompile = isNeedToPreCompile;
}

glbBool CGlbGlobeRenderer::IsNeedToPreCompile()
{
	return mpr_isNeedToPreCompile;
}