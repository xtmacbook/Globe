#include "StdAfx.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbDataSourceFactory.h"
#include "CGlbGlobe.h"
#include "GlbGlobeREObject.h"
#include "GlbGlobeView.h"
#include <osg/StateSet>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/Material>

using namespace GlbGlobe;
#define GLB_FEATURE_SELECTED 1
#define GLB_FEATURE_BLINK    2
#define GLB_FEATURE_SHOW     4

class CGlbFeatureLayerCallback : public osg::NodeCallback
{
public:
	CGlbFeatureLayerCallback(osg::Node* node=NULL)
	{
		mpr_isChangeFrame = false;
		mpr_markNum = 0;					
		mpr_node = node;		
	}	

	void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		osg::ref_ptr<osg::StateSet> ss = mpr_node->getOrCreateStateSet();		

		osg::Material *material  = 
			dynamic_cast<osg::Material *>(ss->getAttribute(osg::StateAttribute::MATERIAL));
		if (!material)
			material = new osg::Material;

		if (nv->getFrameStamp()->getFrameNumber() - mpr_markNum > 20)
		{
			mpr_markNum = nv->getFrameStamp()->getFrameNumber();
			mpr_isChangeFrame = !mpr_isChangeFrame;
		} 		
		if (mpr_isChangeFrame)
		{
			ss->setMode(GL_LIGHTING,osg::StateAttribute::ON);
			material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));			

			ss->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			ss->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			ss->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}
		else
		{
			//osg::Material *material  = 
			//	dynamic_cast<osg::Material *>(mpr_stateset->getAttribute(osg::StateAttribute::MATERIAL));
			//mpr_stateset->removeAttribute(material);
			//mpr_stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
			//mpr_stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
			mpr_node->setStateSet(NULL);
			//mpr_node->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
		}
	}
private:
	glbBool mpr_isChangeFrame;
	unsigned int mpr_markNum;
	osg::ref_ptr<osg::Node> mpr_node;
};


CGlbGlobeFeatureLayer::CGlbGlobeFeatureLayer(void)
{
	mpr_critical.SetName(L"globe_featurelayer");
	mpr_queryfilter_critical.SetName(L"globe_featurelayer_queryfilter");
	mpr_layer_type = GLB_GLOBELAYER_FEATURE;
	mpr_p_cursor = NULL;	
	mpr_isground = true;								
	mpr_query_filter = new CGlbQueryFilter();
	mpr_query_filter->SetFields(L"*");
	mpr_opacity=100;
	mpr_layer_node = new osg::Switch;
	mpr_layer_node->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	mpr_p_visible_extent_onterrain = new CGlbExtent();
	mpr_temp_query_filter = new CGlbQueryFilter();
	mpr_visible_extent = new CGlbExtent();	
	mpr_isBlink = false;
	mpr_isClip = false;
	mpr_isAllowUnLoad = true;
	mpr_isCameraAltitudeAsDistance = false;
	mpr_featureCnt = -1;
	mpr_isEOF	= false;
	mpr_clipObject = NULL;
}

CGlbGlobeFeatureLayer::~CGlbGlobeFeatureLayer(void)
{
	mpr_renderers.clear();
	mpr_p_visible_extent_onterrain = NULL;
	mpr_visible_extent = NULL;
	mpr_temp_query_filter = NULL;
}

const glbWChar*	CGlbGlobeFeatureLayer::GetName()
{
	return mpr_layer_name.c_str();
}
glbBool CGlbGlobeFeatureLayer::SetName(glbWChar* name)
{
	mpr_layer_name = name;
	return true;
}
GlbGlobeLayerTypeEnum CGlbGlobeFeatureLayer::GetType()
{
	return mpr_layer_type;
}

glbBool CGlbGlobeFeatureLayer::Show(glbBool show,glbBool isOnState)
{
	if (mpr_layer_node==NULL) return false;
	if (mpr_show==show) return false;
	mpr_show = show;
	if (mpr_show)
		mpr_layer_node->asSwitch()->setAllChildrenOn();
	else
		mpr_layer_node->asSwitch()->setAllChildrenOff();

	// 通知渲染器显隐
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
	glbInt32 renderCnt = GetRendererCount();
	for (glbInt32 idx = 0; idx < renderCnt; idx++)
	{
		glbref_ptr<CGlbGlobeRenderer> pRenderer =  mpr_renderers.at(idx);
		pRenderer->SetShow(mpr_show);
	}
	return true;
}
glbBool CGlbGlobeFeatureLayer::Blink(glbBool isBlink)
{
	if (mpr_layer_node==NULL) return false;
	if (mpr_isBlink == isBlink)
		return true;

	if(isBlink)
	{		
		osg::ref_ptr<CGlbFeatureLayerCallback> flCallback = new CGlbFeatureLayerCallback(mpr_layer_node.get());
		mpr_layer_node->addUpdateCallback(flCallback);
	}
	else
	{
		osg::NodeCallback *callback = mpr_layer_node->getUpdateCallback();
		if (callback)
		{
			mpr_layer_node->removeUpdateCallback(callback);			
		}

		//osg::Material *material  = 
		//	dynamic_cast<osg::Material *>(mpr_layer_node->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
		//mpr_layer_node->getOrCreateStateSet()->removeAttribute(material);
		//mpr_layer_node->getOrCreateStateSet()->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
		//mpr_layer_node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		mpr_layer_node->setStateSet(NULL);
		//mpr_layer_node->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	}

	mpr_isBlink = isBlink;
	return true;
}
glbBool CGlbGlobeFeatureLayer::IsBlink()
{
	return mpr_isBlink;
}
glbBool CGlbGlobeFeatureLayer::SetOpacity(glbInt32 opacity)
{
	if (mpr_opacity==opacity)
		return true;
	mpr_opacity = opacity;
	osg::StateSet* ss = mpr_layer_node->getOrCreateStateSet();
	if (mpr_opacity<100)
	{// 透明			
		//ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
		if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

		osg::BlendColor* bc = dynamic_cast<osg::BlendColor*>(ss->getAttribute(osg::StateAttribute::BLENDCOLOR));
		osg::BlendFunc* bf = dynamic_cast<osg::BlendFunc*>(ss->getAttribute(osg::StateAttribute::BLENDFUNC));
		if (!bc)
		{
			bc=new osg::BlendColor(osg::Vec4(1.0f,1.0f,1.0f,(float)opacity/ 100));  
			ss->setAttributeAndModes(bc,osg::StateAttribute::ON);
		}
		else
		{
			bc->setConstantColor(osg::Vec4(1.0f,1.0f,1.0f,(float)opacity/ 100));
		}

		if (!bf)
		{
			bf = new osg::BlendFunc();			
			ss->setAttributeAndModes(bf,osg::StateAttribute::ON);
			bf->setSource(osg::BlendFunc::CONSTANT_ALPHA);
			bf->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
		}
	}
	else
	{		
		ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);	
		osg::BlendColor* bc = dynamic_cast<osg::BlendColor*>(ss->getAttribute(osg::StateAttribute::BLENDCOLOR));
		if (bc)
		{
			bc=new osg::BlendColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));  
			ss->setAttributeAndModes(bc,osg::StateAttribute::ON);
		}
		if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );
		//ss->setAttributeAndModes(new osg::CullFace,osg::StateAttribute::ON);
	}

	return true;
}
glbref_ptr<CGlbExtent> CGlbGlobeFeatureLayer::GetBound(glbBool isWorld)
{
	IGlbGeoDataset* geodataset = dynamic_cast<IGlbGeoDataset*>(mpr_dataset.get());
	if (geodataset)
	{
		glbref_ptr<CGlbExtent> ext = (CGlbExtent*)(geodataset->GetExtent());
		if(ext==NULL)return NULL;	
		glbref_ptr<CGlbExtent> _bound = new CGlbExtent;
		//glbWChar* srs = (glbWChar*)(geodataset->GetSRS());
		//// 不能new出来！！！
		//glbref_ptr<CGlbSpatialReference> p_sr = CGlbSpatialReference::CreateFromWKT(geodataset->GetSRS());
		//if( p_sr && ( (p_sr->IsGeographic() && !isWorld)  || (p_sr->IsGeocentric()==false && isWorld) ) ) 
		//{// 没有renference 或者 坐标系统相同，直接返回外包			
		//	_bound->Merge(*(ext.get()));
		//	return _bound;
		//}

		if (!isWorld)
		{// 地理坐标
			_bound->Merge(*(ext.get()));
			return _bound;
		}
		else
		{// world坐标
			glbDouble minX,maxX,minY,maxY,minZ,maxZ;
			osg::Vec3d _minVec,_maxVec;
			ext->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);

			// 地理坐标转世界坐标
			if (mpr_globe->GetType()==GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsmodel->convertLatLongHeightToXYZ(osg::RadiansToDegrees(minY),osg::RadiansToDegrees(minX),minZ,_minVec.x(),_minVec.y(),_minVec.z());
				g_ellipsmodel->convertLatLongHeightToXYZ(osg::RadiansToDegrees(maxY),osg::RadiansToDegrees(maxX),maxZ,_maxVec.x(),_maxVec.y(),_maxVec.z());
				_bound->Merge(_minVec.x(),_minVec.y(),_minVec.z());
				_bound->Merge(_maxVec.x(),_maxVec.y(),_maxVec.z());
			}		
			else
				_bound->Merge(*ext);
		}
		
		return _bound;
	}
	return NULL;
}

/* 要素图层接口 */

glbBool CGlbGlobeFeatureLayer::SetGround(glbBool isground)
{
	if (mpr_isground==isground) return true;	
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
	mpr_isground = isground;
	glbInt32 cnt = GetRendererCount();
	for (glbInt32 i = 0; i < cnt; i++)
	{
		GetRenderer(i)->SetGround(isground);
	}
	return true;
}
glbBool CGlbGlobeFeatureLayer::IsGround()
{
	return mpr_isground;
}
glbBool CGlbGlobeFeatureLayer::SetFilter(CGlbQueryFilter* filter)
{
	if (mpr_query_filter==NULL)
		mpr_query_filter = filter;
	else
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_queryfilter_critical);	

		// 清除所有现有的renderer内已有的对象
		mpr_p_cursor = NULL;
		glbInt32 rendererCnt = GetRendererCount();
		std::map<glbInt32/*feature oid*/,glbInt32/*feature oid*/>::iterator itr = mpr_features.begin();
		while(itr!=mpr_features.end())
		{
			for (glbInt32 i = 0;i < rendererCnt; i++)
			{
				GetRenderer(i)->RemoveObject(itr->first);
			}
			itr++;
		}		
		mpr_features.clear();
		mpr_query_filter = filter;
	}
	return true;
}
CGlbQueryFilter* CGlbGlobeFeatureLayer::GetFilter()
{
	return mpr_query_filter.get();
}
glbBool CGlbGlobeFeatureLayer::SetVisibleTerrainExtent(CGlbExtent* extent1, CGlbExtent* extent2)
{
	if (extent1->IsValid()==false && extent2->IsValid()==false)
	{
		mpr_p_visible_extent_onterrain->Set(DBL_MAX,-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,-DBL_MAX);
		return false;
	}
	//glbref_ptr<CGlbExtent> p_layer_extent = GetBound(false);
	//if (p_layer_extent==NULL) return false;	
	mpr_p_visible_extent_onterrain->Set(DBL_MAX,-DBL_MAX,DBL_MAX,-DBL_MAX,DBL_MAX,-DBL_MAX);
#if 1		
	if (extent1->IsValid())
		mpr_p_visible_extent_onterrain->Merge(*extent1);
	
	if (extent2->IsValid())
		mpr_p_visible_extent_onterrain->Merge(*extent2);
		
	glbDouble minX,maxX,minY,maxY,minZ,maxZ;
	mpr_p_visible_extent_onterrain->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);	
	mpr_p_visible_extent_onterrain->Set(minX,maxX,minY,maxY,-30000.0,100000.0);			
#else	
	////CGlbGlobeRenderer* p_renderer0 = GetRenderer(0);
	////if (p_renderer0->GetAltitudeMode()==GLB_ALTITUDEMODE_ONTERRAIN)	// 计算可见区域
	//{
	//	glbDouble minX,maxX,minY,maxY,minZ,maxZ;
	//	p_layer_extent->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);	
	//	CGlbExtent layerExt;
	//	layerExt.Set(minX,maxX,minY,maxY,-30000,100000);

	//	if (extent1 && extent1->IsValid())
	//	{
	//		extent1->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);	
	//		extent1->Set(minX,maxX,minY,maxY,-30000,100000);

	//		glbref_ptr<CGlbExtent> inter = extent1->Intersect(layerExt);
	//		if (inter)
	//		{				
	//			mpr_p_visible_extent_onterrain->Merge(*inter.get());
	//		}
	//	}

	//	if (extent2 && extent2->IsValid())
	//	{
	//		extent2->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);	
	//		extent2->Set(minX,maxX,minY,maxY,-30000,100000);

	//		glbref_ptr<CGlbExtent> inter = extent2->Intersect(layerExt);	
	//		if (inter)
	//			mpr_p_visible_extent_onterrain->Merge(*inter.get());
	//	}
	//}
#endif
	return true;
}
void CGlbGlobeFeatureLayer::NotifyFeatureAdd(CGlbFeature* feature)
{
	// 1. 如果要素图层的要素不是来源于数据库中的数据集
	if (feature==NULL)		return;
	glbInt32 oid = feature->GetOid();

	std::map<glbInt32,glbInt32>::iterator  itr_find = mpr_features.find(oid);
	if (itr_find !=mpr_features.end())
	{// 找到其id,表示已经加上了
		return;
	}	

GlbScopedLock<GlbCriticalSection> lock(mpr_critical);

	// 2. 根据feature创建featureobject，加入场景管理树octreemanager
	glbInt32 renderer_cnt = GetRendererCount();
	bool bsucc = false;
	for (glbInt32 i = 0; i < renderer_cnt; i++)
	{
		CGlbGlobeRenderer* p_renderer = mpr_renderers.at(i).get();		
		CGlbGlobeRObject* p_robj = dynamic_cast<CGlbGlobeRObject*>(p_renderer->CreateObject(feature)); 
		if (p_robj && mpr_globe)
		{
			p_robj->SetGlobe(mpr_globe);	
			p_robj->SetGround(mpr_isground);	
			p_robj->SetShow(mpr_show);
			p_robj->SetIsAllowUnLoad(mpr_isAllowUnLoad);
			CGlbWString geofield = p_renderer->GetGeoField();
			if (geofield==L"GLBGEO")
			{// 主几何字段名字为GLBGEO
				p_robj->SetFeature(feature);
			}
			else
				p_robj->SetFeature(feature,p_renderer->GetGeoField());
			p_robj->SetFeatureLayer(this);
			/*
			*  对象不设置Id,缺省为-1;
			*  2014.9.16 代码走查,直接加到场景索引，
			*  不加入CGlbGlobe::对象集合的
			*/
			p_robj->SetId(oid);
			p_renderer->AddObject(p_robj);		
			mpr_globe->mpr_sceneobjIdxManager->AddObject(p_robj);
			bsucc = true;
		}		
	}	
	if (bsucc)
		mpr_features[oid] = oid;
	return;
}
void CGlbGlobeFeatureLayer::NotifyFeatureDelete(glbInt32 feature_oid)
{
	std::map<glbInt32,glbInt32>::iterator  itr_find = mpr_features.find(feature_oid);
	if (itr_find ==mpr_features.end())
	{// 没有找到其id
		return;
	}
	mpr_features.erase(itr_find);

GlbScopedLock<GlbCriticalSection> lock(mpr_critical);

	glbInt32 renderer_cnt = GetRendererCount();
	for( glbInt32 i = 0; i < renderer_cnt; i++)
	{
		CGlbGlobeRenderer* p_renderer = mpr_renderers.at(i).get();	
		if (i == 0){//主几何字段所使用的渲染器
			CGlbGlobeObject* p_object = p_renderer->GetObjectById(feature_oid);
			CGlbGlobeRObject* p_renderableobj = dynamic_cast<CGlbGlobeRObject*>(p_object);
			if (p_renderableobj && mpr_globe)
			{
				mpr_globe->mpr_sceneobjIdxManager->RemoveObject(p_renderableobj);
				// 设置删除标志
				p_renderableobj->SetDestroy();
			}
		}
		p_renderer->RemoveObject(feature_oid);
	}
}
void CGlbGlobeFeatureLayer::NotifyFeatureModity(glbInt32 feature_oid)
{//删除现有的，等待重新生成
	std::map<glbInt32,glbInt32>::iterator  itr_find = mpr_features.find(feature_oid);
	if (itr_find == mpr_features.end())
	{// 没有找到其id,直接返回
		return;
	}	
// 删除现有的对象
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);

	glbInt32 renderer_cnt = GetRendererCount();
	for( glbInt32 i = 0; i < renderer_cnt; i++)
	{
		CGlbGlobeRenderer* p_renderer = mpr_renderers.at(i).get();			
		p_renderer->RemoveObject(feature_oid);
	}

}
glbBool CGlbGlobeFeatureLayer::SetGlobe(CGlbGlobe* globe)
{
	if(CGlbGlobeLayer::SetGlobe(globe))
	{
		std::vector<glbref_ptr<CGlbGlobeRenderer>>::iterator itr = mpr_renderers.begin();
		for(itr;itr!=mpr_renderers.end();itr++)
			(*itr)->mpr_globe = globe;
		return true;
	}
	return false;
}
glbBool CGlbGlobeFeatureLayer::AddRenderer(CGlbGlobeRenderer* renderer)  
{
	if (renderer==NULL)return false;
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);

	glbref_ptr<CGlbGlobeRenderer> rd = renderer; 
	rd->mpr_globe = mpr_globe;
	mpr_renderers.push_back(rd);	
	rd->SetShow(mpr_show);
	//mpr_layer_node->asGroup()->addChild(renderer->GetNode());
	//生成一个挂P-C 放到挂的队列

	if (mpr_globe && mpr_globe->mpr_p_callback)
	{
		glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpr_layer_node,renderer->GetNode());
		//task->mpr_size = ComputeNodeSize(mpt_node);		//加入挂队列之前，计算node size
		mpr_globe->mpr_p_callback->AddHangTask(task.get());
	}
	else
		mpr_layer_node->asGroup()->addChild(renderer->GetNode());
	return true;
}
glbBool CGlbGlobeFeatureLayer::RemoveRenderer(glbInt32 idx)
{
	glbInt32 renderCnt = GetRendererCount();
	if (idx <0 || idx>renderCnt-1)
		return false;

GlbScopedLock<GlbCriticalSection> lock(mpr_critical);

	osg::Node* renderNode = mpr_renderers.at(idx)->GetNode();

	//生成一个摘P-C 放到摘的队列
	glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpr_layer_node,renderNode);
	//task->mpr_size = ComputeNodeSize(mpt_node);		//加入摘队列之前，计算node size
	mpr_globe->mpr_p_callback->AddRemoveTask(task.get());

	mpr_renderers.erase(mpr_renderers.begin()+idx);

	return true;
}
glbBool CGlbGlobeFeatureLayer::RemoveAllRenderers()
{
	size_t cnt = mpr_renderers.size();
	if (cnt <= 0) return true;

GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
	for (size_t idx = 0; idx < cnt; idx++)
	{
		glbref_ptr<CGlbGlobeRenderer> renderer = mpr_renderers.at(idx);

		osg::Node* renderNode = renderer->GetNode();

		//生成一个摘P-C 放到摘的队列
		glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpr_layer_node,renderNode);
		//task->mpr_size = ComputeNodeSize(mpt_node);		//加入摘队列之前，计算node size
		mpr_globe->mpr_p_callback->AddRemoveTask(task.get());
	}
	mpr_renderers.clear();
	return true;
}
glbInt32 CGlbGlobeFeatureLayer::GetRendererCount()
{
	return mpr_renderers.size();
}
CGlbGlobeRenderer* CGlbGlobeFeatureLayer::GetRenderer(glbInt32 idx) 
{
	glbInt32 renderCnt = GetRendererCount();
	if (idx <0 || idx>renderCnt-1)
		return NULL;
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
	glbref_ptr<CGlbGlobeRenderer> pRenderer =  mpr_renderers.at(idx);
	return pRenderer.release();
}

#define FRAME_ADD_FEATURECOUNT 100

void CGlbGlobeFeatureLayer::Update()
{
	if(mpr_isDestory || mpr_show==false)
	{
		if (mpr_isDestory)
			RemoveAllRenderers();
		return;
	}
	// 判断是否已经将数据集中所有的对象解析完毕？ 如果已经解析完毕则没有必要再进行解析了。
	if (mpr_isEOF==true)
		return;

	// 网络FS数据集的GetFeatureCount（）接口永远返回0所以不能用此方法判断是否遍历完毕 2016-10-31
	//if (mpr_featureCnt == -1)
	//	mpr_featureCnt = p_featureclass->GetFeatureCount();
	//if (mpr_features.size() >= mpr_featureCnt && mpr_featureCnt > 0)
	//	return;

	if (mpr_globe==NULL)return;
	glbInt32 renderer_cnt = GetRendererCount();
	if (renderer_cnt<=0)	return;

	IGlbDataset* p_geodataset = GetDataset();	
	IGlbFeatureClass* p_featureclass = dynamic_cast<IGlbFeatureClass*>(p_geodataset);
	if (p_featureclass==NULL)	return;

	//// 网络数据集 2016.2.29
	//IGlbNetworkDataset* p_networkDs = dynamic_cast<IGlbNetworkDataset*>(p_geodataset);
	//IGlbFeatureClass* p_networkNodeFtCls = NULL; 
	//if (p_networkDs)
	//{
	//	p_networkNodeFtCls = const_cast<IGlbFeatureClass*>(p_networkDs->GetNodeClass());
	//}
	
	// 1. 主renderer的可见距离跟 相机到图层距离相比较 ， 判断图层是否可见
	glbref_ptr<CGlbGlobeRenderer> p_renderer0 = GetRenderer(0);
	osg::BoundingBoxd boundbox;	
	if (p_renderer0->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN || mpr_globe->GetType()==GLB_GLOBETYPE_GLOBE)
	{		
		glbref_ptr<CGlbExtent> layerExtent = GetBound(false); // 这句话里有内存泄露！！！！
		//glbref_ptr<CGlbExtent> layerExtent = new CGlbExtent(98.53796,125.2105100,37.44373,52.188856999,0,0);

		if (mpr_p_visible_extent_onterrain && mpr_p_visible_extent_onterrain->IsValid() && layerExtent)
		{
			glbref_ptr<CGlbExtent> vExtent = mpr_p_visible_extent_onterrain->Intersect(*layerExtent);
			if (vExtent)
			{				
				double minX,maxX,minY,maxY,minZ,maxZ;
				vExtent->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);		
				// 可视区域只能使用x,y范围，z需要使用图层的z范围
				double tmp;
				layerExtent->GetMin(&tmp,&tmp,&minZ);
				layerExtent->GetMax(&tmp,&tmp,&maxZ);
				boundbox.expandBy(osg::Vec3d(minX,minY,minZ));
				boundbox.expandBy(osg::Vec3d(maxX,maxY,maxZ));
			}
		}		
	}
	else
	{	
		osg::Camera* p_osgcamera = mpr_globe->GetView()->GetOsgCamera();
		osg::Matrixd _modelView = p_osgcamera->getViewMatrix();
		osg::Matrixd _projection = p_osgcamera->getProjectionMatrix();
		osg::Polytope cv;
		cv.setToUnitFrustum();
		cv.transformProvidingInverse((_modelView)*(_projection));

		// 计算可见区域	
		glbref_ptr<CGlbExtent> p_layer_extent = GetBound(false);
		// 根据相机与图层垂直距离来计算最合适的递归深度
		glbInt32 loopdepth = ComputeFitLoopdepth(mpr_globe->GetType(),p_osgcamera,p_layer_extent.get());

		boundbox = ComputeVisibleExtent(cv,p_layer_extent.get(),mpr_globe->GetType(),loopdepth);	
	}

	GlbScopedLock<GlbCriticalSection> lock(mpr_queryfilter_critical);

	if (boundbox.valid())
	{
		glbref_ptr<CGlbExtent> p_visible_extent = new CGlbExtent;
		p_visible_extent->Set(boundbox.xMin(),boundbox.xMax(),boundbox.yMin(),boundbox.yMax(),boundbox.zMin(),boundbox.zMax());
		if ( IsEqual(p_visible_extent.get(),mpr_visible_extent.get())==false )
		{		
			if (p_visible_extent)
			{
				mpr_visible_extent->Set(boundbox.xMin(),boundbox.xMax(),boundbox.yMin(),boundbox.yMax(),boundbox.zMin(),boundbox.zMax());
				// 1. 空间查询功能

				IGlbGeometry* spatial_geo = mpr_query_filter->GetSpatialFilter(0L);

				glbref_ptr<CGlbTin> ptin = NULL;	
				if (spatial_geo)
				{// 空间约束区域有效-南宁项目多边形约束2014.1.27
					glbref_ptr<CGlbExtent> extent = const_cast<CGlbExtent*>(spatial_geo->GetExtent());
					glbref_ptr<CGlbExtent> result = extent->Intersect(*mpr_visible_extent.get());
					ptin = ExtentToTin(result.get());
				}
				else
				{
					// 无空间约束区域
					double minX,maxX,minY,maxY,minZ,maxZ;
					mpr_visible_extent->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);
					mpr_visible_extent->Set(minX - 0.1,maxX + 0.1,minY - 0.1,maxY + 0.1,minZ - 0.1,maxZ + 0.1);
					ptin = ExtentToTin(mpr_visible_extent.get());						
					mpr_temp_query_filter->PutSpatialFilter(ptin.get(),1);
					//{// 测试，全查 2014.11.20 
					//	mpr_temp_query_filter->SetFields(L"*");
					//}					
				}

				BuildQueryfilterPostfixClause(mpr_temp_query_filter.get());

				// 2. 查询获取feature	
				mpr_p_cursor = p_featureclass->Query(mpr_temp_query_filter.get());
				/*if(mpr_p_cursor == NULL)
				{
					std::wstring err = p_featureclass->GetLastError();
					err = L"";
				}*/
			}		
		}			
	}
	else
	{
		mpr_p_cursor = NULL;
	}
	
	glbInt32 added_feature_count = 0;
	glbBool is_not_eof = true;
	if (mpr_p_cursor.get())
	{		
		do
		{		
			if(mpr_isDestory)
			{
				RemoveAllRenderers();
				return;
			}
			glbref_ptr<CGlbFeature> p_feature = mpr_p_cursor->GetFeature();
			if(p_feature == NULL)break;
			// 获取feature的Name，如果有的话
			CGlbWString featureName;
			const GLBVARIANT* va = p_feature->GetValueByName(L"NAME");
			if (va && va->strVal)
				featureName = va->strVal;

			glbInt32 oid = p_feature->GetOid();				
			// 3. 由于feature所占内存位置是覆盖性的，所以，
			// 如果需要取出一个feature后需要立即将它转为featureobject
			std::map<glbInt32,glbInt32>::iterator  itr_find = mpr_features.find(oid);
			if (itr_find ==mpr_features.end())
			{// 没找到 ,需要添加				
				bool bSucc = false;

				GlbScopedLock<GlbCriticalSection> lock(mpr_critical);

				renderer_cnt = GetRendererCount();
				// 遍历renderers添加feature
				for (glbInt32  j = 0; j < renderer_cnt; j++)
				{
					glbref_ptr<CGlbGlobeRenderer> p_renderer = GetRenderer(j);		
					// 创建featureobject，加入到场景管理树中以便调度				
					CGlbGlobeREObject* p_reobject = dynamic_cast<CGlbGlobeREObject*>(p_renderer->CreateObject(p_feature.get()));
					if (p_reobject){																
						p_reobject->SetGlobe(mpr_globe);	
						p_reobject->SetGround(mpr_isground);	
						p_reobject->SetShow(mpr_show);
						p_reobject->SetIsAllowUnLoad(mpr_isAllowUnLoad);
						p_reobject->SetCameraAltitudeAsDistance(mpr_isCameraAltitudeAsDistance);
						p_reobject->SetName(featureName.c_str());

						CGlbWString geofield = p_renderer->GetGeoField();
						if (geofield==L"GLBGEO")
						{// 主几何字段名字为GLBGEO
							p_reobject->SetFeature(p_feature.get());
						}
						else
							p_reobject->SetFeature(p_feature.get(),p_renderer->GetGeoField());
						p_reobject->SetFeatureLayer(this);
						/*
						*  对象不设置Id,缺省为-1;
						*  2014.9.16 代码走查,直接加到场景索引，
						*  不加入CGlbGlobe::对象集合的
						*/
						p_reobject->SetId(oid);
						p_renderer->AddObject(p_reobject);						
						mpr_globe->mpr_sceneobjIdxManager->AddObject(p_reobject);
						/*
						 *  设置选中、闪烁、显隐状态
						 */
						std::map<glbInt32/*feature oid*/,glbByte/*state*/>::iterator itr = mpr_ftstate.find(oid);
						if(itr != mpr_ftstate.end())
						{
							glbBool isSelected = (itr->second & GLB_FEATURE_SELECTED)==GLB_FEATURE_SELECTED?true:false;
							glbBool isBlink    = (itr->second & GLB_FEATURE_BLINK)==GLB_FEATURE_BLINK?true:false;
							glbBool isShow     = (itr->second & GLB_FEATURE_SHOW)==GLB_FEATURE_SHOW?true:false;
							p_reobject->SetSelected(isSelected);
							p_reobject->SetBlink(isBlink);
							p_reobject->SetShow(isShow);
						}

						// 设置碰撞检测标志
						if (mpr_isCollisionDetected)
							mpr_globe->GetView()->AddCollisionLaysObject(mpr_layer_id,p_reobject);

						bSucc = true;
					}	
				}

				if (bSucc)
				{// 添加成功
					mpr_features[oid]=oid;
					added_feature_count++;
				}

				//// 网络数据集，绘制edge时同时需要绘制边的两个端点 移到CGlbNetworkObject::ReadData中
				//if (p_networkNodeFtCls)
				//{
				//	glbInt32 edgeID,fromNodeID,toNodeID;
				//	edgeID = fromNodeID = toNodeID = -1;
				//	const GLBVARIANT* va = p_feature->GetValueByName(L"GlbEdgeID");
				//	if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				//		edgeID = va->lVal;
				//	
				//	va = p_feature->GetValueByName(L"GlbFNodeID");
				//	if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				//		fromNodeID = va->lVal;
				//	
				//	va = p_feature->GetValueByName(L"GlbTNodeID");
				//	if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				//		toNodeID = va->lVal;	

				//	CGlbFeature* fromNodeFeature = p_networkNodeFtCls->GetFeature(fromNodeID);
				//	CGlbFeature* toNodeFeature = p_networkNodeFtCls->GetFeature(toNodeID);
				//	// 创建点渲染renderer？

				//}
			}// end 3
			is_not_eof = mpr_p_cursor.get()->MoveNext();
			if(!is_not_eof)
			{
				mpr_p_cursor = NULL;
				mpr_isEOF = true;
				break;
			}			
			if(added_feature_count>=FRAME_ADD_FEATURECOUNT)
				break;
		}while(1);		
	}
}

glbInt32 CGlbGlobeFeatureLayer::ComputeFitLoopdepth(GlbGlobeTypeEnum globe_type, osg::Camera* p_osgcamera, CGlbExtent* p_layer_extent)
{
	glbInt32 loopdepth = 0;
	if (!p_layer_extent)
		return 2;

	osg::Vec3d eye,center,up;
	p_osgcamera->getViewMatrixAsLookAt(eye,center,up);
	glbDouble fovy,aspectRatio,zNear,zFar;
	p_osgcamera->getProjectionMatrixAsPerspective(fovy,aspectRatio,zNear,zFar);

	glbDouble west,east,south,north,height_min,height_max;
	p_layer_extent->Get(&west,&east,&south,&north,&height_min,&height_max);	

	glbDouble noscale_distanc;
	glbDouble vertical_disance;
	if (globe_type==GLB_GLOBETYPE_GLOBE)
	{
		glbDouble height_average = (height_min + height_max) * 0.5;
		osg::Vec3d quad_verts[4];
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(south),osg::DegreesToRadians(west),height_average,quad_verts[0].x(),quad_verts[0].y(),quad_verts[0].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(south),osg::DegreesToRadians(east),height_average,quad_verts[1].x(),quad_verts[1].y(),quad_verts[1].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(north),osg::DegreesToRadians(east),height_average,quad_verts[2].x(),quad_verts[2].y(),quad_verts[2].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(north),osg::DegreesToRadians(west),height_average,quad_verts[3].x(),quad_verts[3].y(),quad_verts[3].z());

		glbDouble l = (quad_verts[1] - quad_verts[0]).length();
		glbDouble w = (quad_verts[3] - quad_verts[0]).length();
		glbDouble min_len = max(l,w);

		noscale_distanc = (min_len*0.5)/tan(osg::DegreesToRadians(fovy*0.5));	

		glbDouble longitude,latitude,height;
		g_ellipsmodel->convertXYZToLatLongHeight( eye.x(),eye.y(),eye.z(),
			latitude, longitude, height);

		vertical_disance = max(fabs(height-height_min),fabs(height-height_max));			
	}
	else if (globe_type==GLB_GLOBETYPE_FLAT)
	{		
		glbDouble min_len = max(fabs(east-west),fabs(north-south));
		noscale_distanc = (min_len*0.5)/tan(osg::DegreesToRadians(fovy*0.5));

		vertical_disance = max(fabs(eye.z()-height_min),fabs(eye.z()-height_max));	
	}

	glbDouble _multiple = noscale_distanc / vertical_disance ;

	if (vertical_disance==0.0)
		return 0;

	while(_multiple>1)
	{
		_multiple /= 2.0;
		loopdepth++;
	}
	
	return loopdepth;
}

osg::BoundingBoxd CGlbGlobeFeatureLayer::ComputeVisibleExtent(osg::Polytope cv, CGlbExtent* extent, GlbGlobeTypeEnum globe_type, glbInt32 loopdepth)
{
	osg::BoundingBoxd bb;
	if (extent==NULL)
		return bb;
	glbDouble west,east,south,north,height_min,height_max;
	extent->Get(&west,&east,&south,&north,&height_min,&height_max);

	std::vector<osg::Vec3d> pointvec;	
	if (loopdepth<=0)
	{// 剩余迭代次数<=0
		bb.set(west,south,height_min,east,north,height_max);
		return bb;
	}

	osg::Vec3d corners[8];
	if (globe_type==GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(north),osg::DegreesToRadians(west),height_min,corners[0].x(),corners[0].y(),corners[0].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(south),osg::DegreesToRadians(west),height_min,corners[1].x(),corners[1].y(),corners[1].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(south),osg::DegreesToRadians(east),height_min,corners[2].x(),corners[2].y(),corners[2].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(north),osg::DegreesToRadians(east),height_min,corners[3].x(),corners[3].y(),corners[3].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(north),osg::DegreesToRadians(west),height_max,corners[4].x(),corners[4].y(),corners[4].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(south),osg::DegreesToRadians(west),height_max,corners[5].x(),corners[5].y(),corners[5].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(south),osg::DegreesToRadians(east),height_max,corners[6].x(),corners[6].y(),corners[6].z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(north),osg::DegreesToRadians(east),height_max,corners[7].x(),corners[7].y(),corners[7].z());
	}
	else
	{
		corners[0].set(west,north,height_min);
		corners[1].set(west,south,height_min);
		corners[2].set(east,north,height_min);
		corners[3].set(east,south,height_min);
		corners[4].set(west,north,height_max);
		corners[5].set(west,south,height_max);
		corners[6].set(east,north,height_max);
		corners[7].set(east,south,height_max);	
	}	

	for (long i = 0; i < 8; i++){		
		bb.expandBy(corners[i]);		
	}

	osg::BoundingBoxd _bb;
	// 判断boundBox是否都在视锥体内
	osg::BoundingBox bbf;
	bbf.set(osg::Vec3(bb._min),osg::Vec3f(bb._max));
	glbBool bAllIn = cv.containsAllOf(bbf);	
	if (bAllIn )//&& bFaceCircle)
	{// 8个点都在视域范围内，则不需要细分子节点		
		_bb.set(west,south,height_min,east,north,height_max);
		return _bb;
	}
	glbBool bPartIn = cv.contains(bbf);
	if ( bPartIn )
	{// 部分在
		double CenterLat = 0.5 * (south + north);
		double CenterLon = 0.5 * (east + west);

		long cur_loopdepthd = loopdepth-1;
		CGlbExtent temp_extent;
		temp_extent.Set(west,CenterLon,CenterLat,north,height_min,height_max);
		osg::BoundingBoxd northeast_bb = ComputeVisibleExtent(cv,&temp_extent,globe_type,cur_loopdepthd);

		temp_extent.Set(CenterLon,east,CenterLat,north,height_min,height_max);
		osg::BoundingBoxd northwest_bb = ComputeVisibleExtent(cv,&temp_extent,globe_type,cur_loopdepthd);

		temp_extent.Set(west,CenterLon,south,CenterLat,height_min,height_max);
		osg::BoundingBoxd southwest_bb = ComputeVisibleExtent(cv,&temp_extent,globe_type,cur_loopdepthd);

		temp_extent.Set(CenterLon,east,south,CenterLat,height_min,height_max);
		osg::BoundingBoxd southeast_bb = ComputeVisibleExtent(cv,&temp_extent,globe_type,cur_loopdepthd);

		if (northeast_bb.valid())
			_bb.expandBy(northeast_bb);
		if (northwest_bb.valid())
			_bb.expandBy(northwest_bb);
		if (southeast_bb.valid())
			_bb.expandBy(southeast_bb);
		if (southwest_bb.valid())
			_bb.expandBy(southwest_bb);
	}
	else
	{
	}

	return _bb;
}

glbBool CGlbGlobeFeatureLayer::IsEqual(CGlbExtent* extent, CGlbExtent* extent_other)
{
	if (extent==NULL && extent_other==NULL)
		return true;
	if ( (extent && !extent_other) || (!extent && extent_other))
		return false;
	glbDouble minX,maxX,minY,maxY,minZ,maxZ;
	extent->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);	
	glbDouble minX_other,maxX_other,minY_other,maxY_other,minZ_other,maxZ_other;
	extent_other->Get(&minX_other,&maxX_other,&minY_other,&maxY_other,&minZ_other,&maxZ_other);

	if (minX!=minX_other)
		return false;

	if (minY!=minY_other)
		return false;

	/*if (minZ!=minZ_other)
		return false;*/

	if (maxX!=maxX_other)
		return false;

	if (maxY!=maxY_other)
		return false;

	/*if (maxZ!=maxZ_other)
		return false;*/
	return true;		 
}

CGlbTin* CGlbGlobeFeatureLayer::ExtentToTin(CGlbExtent* p_extent)
{
	if (p_extent==NULL)
		return NULL;

	CGlbTin* pTin = new CGlbTin();
	glbDouble minX,minY,minZ,maxX,maxY,maxZ;	
	p_extent->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);
	{
		//oracle number 小数位 到12位，12后的四舍五入.		
		minX -= 0.000000000001;
		maxX += 0.0000000000009;//0.0000000000009;
		minY -= 0.000000000001;
		maxY += 0.0000000000009;//0.0000000000009;
		minZ -= 0.000000000001;
		maxZ += 0.0000000000009;//0.0000000000009;
	}
	if (maxZ<=minZ)
	{
		maxZ = minZ + 2000;
	}

	pTin->AddVertex(minX,minY,minZ);
	pTin->AddVertex(maxX,minY,minZ);
	pTin->AddVertex(maxX,maxY,minZ);
	pTin->AddVertex(minX,maxY,minZ);

	pTin->AddVertex(minX,minY,maxZ);
	pTin->AddVertex(maxX,minY,maxZ);
	pTin->AddVertex(maxX,maxY,maxZ);
	pTin->AddVertex(minX,maxY,maxZ);

	pTin->AddTriangle(0,3,1);
	pTin->AddTriangle(1,2,3);
	pTin->AddTriangle(4,5,7);
	pTin->AddTriangle(5,7,6);

	pTin->AddTriangle(0,4,3);
	pTin->AddTriangle(3,4,7);
	pTin->AddTriangle(1,2,5);
	pTin->AddTriangle(5,2,6);
	pTin->AddTriangle(0,1,4);
	pTin->AddTriangle(4,1,5);
	pTin->AddTriangle(2,3,7);
	pTin->AddTriangle(2,7,6);
	return pTin;
}

glbBool CGlbGlobeFeatureLayer::Load(xmlNodePtr node, glbWChar* relativepath )
{
	xmlChar* szKey = NULL;
	IGlbDataSource* dsource = NULL;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"DatasourceID")))
	{		
		szKey = xmlNodeGetContent(node);
		glbInt32 dsid;
		sscanf((char*)szKey,"%d",&dsid);	
		CGlbDataSourceFactory* factory = CGlbDataSourceFactory::GetInstance();
		dsource = factory->GetDataSourceByID(dsid);
		xmlFree(szKey);
		node = node->next;
	}			

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"DatasetName")))
	{		
		szKey = xmlNodeGetContent(node);		
		if (dsource)
		{
			char* str = u2g((char*)szKey);
			CGlbString glbstr = str;
			CGlbWString datsetname = glbstr.ToWString();		
			IGlbDataset* dataset = dsource->GetDatasetByName(datsetname.c_str());	
			if (dataset)
				SetDataset(dataset);
			else
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"Load FeatureLayer dataset file error, Check the file path is right ? \r\n");
				GlbLogWOutput(GLB_LOGTYPE_ERR,(glbWChar*)datsetname.c_str());	
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"\r\n");	
			}
			free(str);
		}
		xmlFree(szKey);
		node = node->next;
	}			

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LayerID")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_layer_id);	
		xmlFree(szKey);
		node = node->next;
	}			
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LayerName")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		mpr_layer_name = glbstr.ToWString();	
		xmlFree(szKey);
		free(str);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LayerGroupID")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_layergroup_id);
		xmlFree(szKey);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_opacity);	
		xmlFree(szKey);
		node = node->next;
	}			

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Visible")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%c",&mpr_show);
		xmlFree(szKey);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RenderOrder")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_renderorder);	
		xmlFree(szKey);
		node = node->next;
	}		

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LoadOrder")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_loadorder);
		xmlFree(szKey);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActiveAction")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_activeaction);
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionYaw")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionyaw);	
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionPitch")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionpitch);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionRoll")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionroll);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionLonOrX")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionLonOrX);
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionLatOrY")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionLatOrY);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionAltOrZ")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionAltOrZ);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"DefaultVisDis")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_defaultvisibledist);	
		xmlFree(szKey);
		node = node->next;
	}

	// 渲染器
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RendererNum")))
	{		
		szKey = xmlNodeGetContent(node);
		int renderCnt;
		sscanf((char*)szKey,"%d",&renderCnt);
		for (int i = 0 ; i < renderCnt; i++)
		{
			node = node->next;
			CGlbGlobeRenderer* renderer = new CGlbGlobeRenderer();
			if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Renderer")))
			{
				xmlNodePtr rendererNode = node->children;
				renderer->Load(rendererNode,relativepath);
				AddRenderer(renderer);
			}
		}
		node = node->next;
	}
	return true;
}
glbBool CGlbGlobeFeatureLayer::Save(xmlNodePtr node, glbWChar* relativepath )
{
	IGlbDataSource *p_datasource = (IGlbDataSource *)mpr_dataset->GetDataSource();	
	glbInt32 dsid = p_datasource->GetID();
	char str[128];
	sprintf(str,"%d",dsid);
	xmlNewTextChild(node, NULL, BAD_CAST "DatasourceID", BAD_CAST str);
	
	const glbWChar* datasetname = mpr_dataset->GetName();
	CGlbWString dsnameString = datasetname;
	char* szOut = g2u((char*)dsnameString.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "DatasetName", BAD_CAST szOut);
	free(szOut);

	sprintf(str,"%d",mpr_layer_id);
	xmlNewTextChild(node, NULL, BAD_CAST "LayerID", BAD_CAST str);

	szOut = g2u((char*)mpr_layer_name.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "LayerName", BAD_CAST szOut);
	free(szOut);

	sprintf(str,"%d",mpr_layergroup_id);
	xmlNewTextChild(node, NULL, BAD_CAST "LayerGroupID", BAD_CAST str);
	sprintf(str,"%d",mpr_opacity);
	xmlNewTextChild(node, NULL, BAD_CAST "Opacity", BAD_CAST str);
	sprintf(str,"%d",mpr_show);
	xmlNewTextChild(node, NULL, BAD_CAST "Visible", BAD_CAST str);

	sprintf(str,"%d",mpr_renderorder);
	xmlNewTextChild(node, NULL, BAD_CAST "RenderOrder", BAD_CAST str);
	sprintf(str,"%d",mpr_loadorder);
	xmlNewTextChild(node, NULL, BAD_CAST "LoadOrder", BAD_CAST str);

	sprintf(str,"%d",mpr_activeaction);//图层激活动作
	xmlNewTextChild(node, NULL, BAD_CAST "ActiveAction", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionyaw);//图层飞向跳转时的偏航角【绕y轴旋转】
	xmlNewTextChild(node, NULL, BAD_CAST "ActionYaw", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionpitch);//图层飞向跳转时的倾斜角【绕x轴旋转】
	xmlNewTextChild(node, NULL, BAD_CAST "ActionPitch", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionroll);//图层飞向跳转时的旋转角【绕z轴旋转】
	xmlNewTextChild(node, NULL, BAD_CAST "ActionRoll", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionLonOrX);//图层飞向跳转时的视点经度或X
	xmlNewTextChild(node, NULL, BAD_CAST "ActionLonOrX", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionLatOrY);//图层飞向跳转时的视点纬度或Y
	xmlNewTextChild(node, NULL, BAD_CAST "ActionLatOrY", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionAltOrZ);//图层飞向跳转时的视点高度或Z
	xmlNewTextChild(node, NULL, BAD_CAST "ActionAltOrZ", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_defaultvisibledist);//图层飞向跳转时的视点高度或Z
	xmlNewTextChild(node, NULL, BAD_CAST "DefaultVisDis", BAD_CAST str);

	// 渲染器
	size_t rendererCnt = mpr_renderers.size();
	sprintf(str,"%d",rendererCnt);
	xmlNewTextChild(node, NULL, BAD_CAST "RendererNum", BAD_CAST  str);
	for (size_t i = 0 ; i < mpr_renderers.size(); i++)
	{
		xmlNodePtr renderer_node = xmlNewNode(NULL,BAD_CAST"Renderer");
		xmlAddChild(node,renderer_node);
		mpr_renderers.at(i)->Save(renderer_node,relativepath);
	}
	return true;
}

void CGlbGlobeFeatureLayer::SetIsAllowUnLoad(glbBool isAllowUnload)
{
	if (mpr_isAllowUnLoad == isAllowUnload)
		return;

	mpr_isAllowUnLoad = isAllowUnload;
	// 通知渲染器显隐
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
	glbInt32 renderCnt = GetRendererCount();
	for (glbInt32 idx = 0; idx < renderCnt; idx++)
	{
		glbref_ptr<CGlbGlobeRenderer> pRenderer =  mpr_renderers.at(idx);
		pRenderer->SetIsAllowUnLoad(mpr_isAllowUnLoad);
	}
}

glbBool CGlbGlobeFeatureLayer::IsAllowUnLoad()
{
	return mpr_isAllowUnLoad;
}

//void CGlbGlobeFeatureLayer::SetCameraAltitudeAsDistance(glbBool isCameraAltitudeAsDistance)
//{
//	if (mpr_isCameraAltitudeAsDistance == isCameraAltitudeAsDistance)
//		return ;
//
//	mpr_isCameraAltitudeAsDistance = isCameraAltitudeAsDistance;
//	// 通知渲染器显隐
//	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
//	glbInt32 renderCnt = GetRendererCount();
//	for (glbInt32 idx = 0; idx < renderCnt; idx++)
//	{
//		glbref_ptr<CGlbGlobeRenderer> pRenderer =  mpr_renderers.at(idx);
//		pRenderer->SetCameraAltitudeAsDistance(mpr_isCameraAltitudeAsDistance);
//	}
//}
//
//glbBool CGlbGlobeFeatureLayer::IsCameraAltitudeAsDistance()
//{
//	return mpr_isCameraAltitudeAsDistance;
//}

void CGlbGlobeFeatureLayer::SetFeatureSelected(glbInt32 ftOID,glbBool isSelected)
{
	if(GetDataset() == NULL)return;
	std::map<glbInt32/*feature oid*/,glbByte/*state*/>::iterator itr = mpr_ftstate.find(ftOID);
	if(itr == mpr_ftstate.end())
		mpr_ftstate[ftOID] = 0;
	if(isSelected)
		mpr_ftstate[ftOID] |= GLB_FEATURE_SELECTED;
	else
		mpr_ftstate[ftOID] &= (255 - GLB_FEATURE_SELECTED);
	glbInt32 renderer_cnt = GetRendererCount();
	for (glbInt32  j = 0; j < renderer_cnt; j++)
	{
		glbref_ptr<CGlbGlobeRenderer> p_renderer = GetRenderer(j);
		CGlbGlobeObject* obj = p_renderer->GetObjectById(ftOID);
		if(obj!=NULL)
		{
			CGlbGlobeREObject* p_reobject = dynamic_cast<CGlbGlobeREObject*>(obj);
			if(p_reobject)p_reobject->SetSelected(isSelected);
		}
	}
}
glbBool CGlbGlobeFeatureLayer::IsFeatureSelected(glbInt32 ftOID)
{
	if(GetDataset() == NULL)return false;
	glbInt32 renderer_cnt = GetRendererCount();
	for (glbInt32  j = 0; j < renderer_cnt; j++)
	{
		glbref_ptr<CGlbGlobeRenderer> p_renderer = GetRenderer(j);
		CGlbGlobeObject* obj = p_renderer->GetObjectById(ftOID);
		if(obj!=NULL)
		{
			CGlbGlobeREObject* p_reobject = dynamic_cast<CGlbGlobeREObject*>(obj);
			if(p_reobject)
				return p_reobject->IsSelected();
		}
	}
	std::map<glbInt32/*feature oid*/,glbByte/*state*/>::iterator itr = mpr_ftstate.find(ftOID);
	if(itr == mpr_ftstate.end())
		return false;
	return (itr->second & GLB_FEATURE_SELECTED)==GLB_FEATURE_SELECTED?true:false;
}
void CGlbGlobeFeatureLayer::SetFeatureBlink(glbInt32 ftOID,glbBool isBlink)
{
	if(GetDataset() == NULL)return;
	std::map<glbInt32/*feature oid*/,glbByte/*state*/>::iterator itr = mpr_ftstate.find(ftOID);
	if(itr == mpr_ftstate.end())
		mpr_ftstate[ftOID] = 0;
	if(isBlink)
		mpr_ftstate[ftOID] |= GLB_FEATURE_BLINK;
	else
		mpr_ftstate[ftOID] &= (255 - GLB_FEATURE_BLINK);
	glbInt32 renderer_cnt = GetRendererCount();
	for (glbInt32  j = 0; j < renderer_cnt; j++)
	{
		glbref_ptr<CGlbGlobeRenderer> p_renderer = GetRenderer(j);
		CGlbGlobeObject* obj = p_renderer->GetObjectById(ftOID);
		if(obj!=NULL)
		{
			CGlbGlobeREObject* p_reobject = dynamic_cast<CGlbGlobeREObject*>(obj);
			if(p_reobject)p_reobject->SetBlink(isBlink);
		}
	}
}
glbBool CGlbGlobeFeatureLayer::IsFeatureBlink(glbInt32 ftOID)
{
	if(GetDataset() == NULL)return false;
	glbInt32 renderer_cnt = GetRendererCount();
	for (glbInt32  j = 0; j < renderer_cnt; j++)
	{
		glbref_ptr<CGlbGlobeRenderer> p_renderer = GetRenderer(j);
		CGlbGlobeObject* obj = p_renderer->GetObjectById(ftOID);
		if(obj!=NULL)
		{
			CGlbGlobeREObject* p_reobject = dynamic_cast<CGlbGlobeREObject*>(obj);
			if(p_reobject)
				return p_reobject->IsBlink();
		}
	}
	std::map<glbInt32/*feature oid*/,glbByte/*state*/>::iterator itr = mpr_ftstate.find(ftOID);
	if(itr == mpr_ftstate.end())
		return false;
	return (itr->second & GLB_FEATURE_BLINK)==GLB_FEATURE_BLINK?true:false;
}
void CGlbGlobeFeatureLayer::SetFeatureShow(glbInt32 ftOID,glbBool isShow)
{
	if(GetDataset() == NULL)return;
	std::map<glbInt32/*feature oid*/,glbByte/*state*/>::iterator itr = mpr_ftstate.find(ftOID);
	if(itr == mpr_ftstate.end())
		mpr_ftstate[ftOID] = 0;
	if(isShow)
		mpr_ftstate[ftOID] |= GLB_FEATURE_SHOW;
	else
		mpr_ftstate[ftOID] &= (255 - GLB_FEATURE_SHOW);
	glbInt32 renderer_cnt = GetRendererCount();
	for (glbInt32  j = 0; j < renderer_cnt; j++)
	{
		glbref_ptr<CGlbGlobeRenderer> p_renderer = GetRenderer(j);
		CGlbGlobeObject* obj = p_renderer->GetObjectById(ftOID);
		if(obj!=NULL)
		{
			CGlbGlobeREObject* p_reobject = dynamic_cast<CGlbGlobeREObject*>(obj);
			if(p_reobject)p_reobject->SetShow(isShow);
		}
	}
}
glbBool CGlbGlobeFeatureLayer::IsFeatureShow(glbInt32 ftOID)
{
	if(GetDataset() == NULL)return false;
	glbInt32 renderer_cnt = GetRendererCount();
	for (glbInt32  j = 0; j < renderer_cnt; j++)
	{
		glbref_ptr<CGlbGlobeRenderer> p_renderer = GetRenderer(j);
		CGlbGlobeObject* obj = p_renderer->GetObjectById(ftOID);
		if(obj!=NULL)
		{
			CGlbGlobeREObject* p_reobject = dynamic_cast<CGlbGlobeREObject*>(obj);
			if(p_reobject)
				return p_reobject->IsShow();
		}
	}
	std::map<glbInt32/*feature oid*/,glbByte/*state*/>::iterator itr = mpr_ftstate.find(ftOID);
	if(itr == mpr_ftstate.end())
		return true;
	return (itr->second & GLB_FEATURE_SHOW)==GLB_FEATURE_SHOW?true:false;
}

glbBool CGlbGlobeFeatureLayer::Load2( xmlNodePtr node, glbWChar* relativepath )
{
	CGlbGlobeLayer::Load2(node,relativepath);
	xmlChar* szKey = NULL;
	IGlbDataSource* dsource = NULL;
	xmlNodePtr childNode = NULL;
	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionAltOrZ")))
	{
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionAltOrZ);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionLatOrY")))
	{
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionLatOrY);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionLonOrX")))
	{
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionLonOrX);
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionPitch")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionpitch);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionRoll")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionroll);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionYaw")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionyaw);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActiveAction")))
	{
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString activeAction = (char*)str;
		if(activeAction == "NGLB_GLOBE_FLYTO")
		{
			mpr_activeaction = GlbGlobeActiveActionEnum::GLB_GLOBE_FLYTO;
		}
		else
		{
			mpr_activeaction = GlbGlobeActiveActionEnum::GLB_GLOBE_JUMPTO;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetDataset")))
	{
		childNode = node->xmlChildrenNode;//text
		childNode = childNode->next;//Name
		szKey = xmlNodeGetContent(childNode);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		CGlbWString datsetname = glbstr.ToWString();		
		xmlFree(szKey);
		childNode = childNode->next;//text
		childNode = childNode->next;//Alias
		childNode = childNode->next;//text
		childNode = childNode->next;//GetDataSource
		childNode = childNode->xmlChildrenNode;//text
		childNode = childNode->next;//GetAlias
		childNode = childNode->next;//text
		childNode = childNode->next;//Url
		szKey = xmlNodeGetContent(childNode);
		str = u2g((char*)szKey);
		glbstr = str;
		CGlbWString url = glbstr.ToWString();		
		xmlFree(szKey);
		childNode = childNode->next;//text
		childNode = childNode->next;//User
		szKey = xmlNodeGetContent(childNode);
		str = u2g((char*)szKey);
		glbstr = str;
		CGlbWString user = glbstr.ToWString();		
		xmlFree(szKey);
		childNode = childNode->next;//text
		childNode = childNode->next;//Password
		szKey = xmlNodeGetContent(childNode);
		str = u2g((char*)szKey);
		glbstr = str;
		CGlbWString password = glbstr.ToWString();		
		xmlFree(szKey);
		childNode = childNode->next;//text
		childNode = childNode->next;//ProviderName
		szKey = xmlNodeGetContent(childNode);
		str = u2g((char*)szKey);
		glbstr = str;
		CGlbWString providerName = glbstr.ToWString();		
		xmlFree(szKey);

		CGlbDataSourceFactory* factory = CGlbDataSourceFactory::GetInstance();
		dsource = factory->OpenDataSource(url.c_str(),user.c_str(),password.c_str(),providerName.c_str());
		if(dsource == NULL)
			return false;
		IGlbDataset* dataset = dsource->GetDatasetByName(datsetname.c_str());
		SetDataset(dataset);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetDefaultVisibleDistance")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_defaultvisibledist);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetLoadOrder")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_loadorder);
		xmlFree(szKey);
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
		mpr_layer_name = glbstr.ToWString();	
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetOpacity")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_opacity);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetRenderOrder")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_renderorder);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsShow")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%c",&mpr_show);
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	// 渲染器
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Renderers")))
	{		
		childNode = node->xmlChildrenNode;//text
		childNode = childNode->next;//SerialGlobeRenderer
		while(!xmlStrcmp(childNode->name, (const xmlChar *)"SerialGlobeRenderer"))
		{
			CGlbGlobeRenderer* renderer = new CGlbGlobeRenderer();
			renderer->Load2(childNode->xmlChildrenNode,relativepath);
			AddRenderer(renderer);
			childNode = childNode->next;//text
			childNode = childNode->next;//SerialGlobeRenderer
			if(childNode == NULL)
				break;
		}
	}
	return true;
}

void CGlbGlobeFeatureLayer::SetCollisionDetect(glbBool isCollisionDetect)
{
	if (isCollisionDetect == mpr_isCollisionDetected)
		return;

	mpr_isCollisionDetected = isCollisionDetect;
	// 通知渲染器显隐
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
	glbInt32 renderCnt = GetRendererCount();
	for (glbInt32 idx = 0; idx < renderCnt; idx++)
	{
		glbref_ptr<CGlbGlobeRenderer> pRenderer =  mpr_renderers.at(idx);
		pRenderer->SetCollisionDetect(mpr_isCollisionDetected,mpr_layer_id);
	}
}

glbBool CGlbGlobeFeatureLayer::IsCollisionDectect()
{
	return mpr_isCollisionDetected;
}
bool CGlbGlobeFeatureLayer::BuildQueryfilterPostfixClause(CGlbQueryFilter* queryFilter)
{
	if (queryFilter==NULL) return true;
	
	osg::Vec3d cameraPos, focusPos;
	mpr_globe->GetView()->GetCameraPos(cameraPos);
	mpr_globe->GetView()->GetFocusPos(focusPos);
	osg::Vec3d _dir = focusPos - cameraPos;
	queryFilter->SetCameraDir(_dir.x(),_dir.y(),_dir.z());
	return true;

	//CGlbWString sqlOrderBy = L"ORDER BY ";
	//IGlbDataset* p_geodataset = GetDataset();
	//IGlbFeatureClass* p_featureclass = dynamic_cast<IGlbFeatureClass*>(p_geodataset);
	//sqlOrderBy += p_featureclass->GetName();
	//sqlOrderBy += L"_geo gg ";

	//if (fabs(_dir.x()) > fabs(_dir.y()))
	//{
	//	if (_dir.x() > 0)
	//		sqlOrderBy += L"gg.glbminx ASC";
	//	else
	//		sqlOrderBy += L"gg.glbmaxx DESC";

	//	if (_dir.y() > 0)
	//		sqlOrderBy += L", gg.glbminy ASC";
	//	else
	//		sqlOrderBy += L", gg.glbmaxy DESC";

	//	// 暂时不用z排序
	//	//if (_dir.z() > 0)
	//	//	sqlOrderBy += L", _geo.glbminz ASC";
	//	//else
	//	//	sqlOrderBy += L", _geo.glbminz DESC";
	//}
	//else
	//{
	//	if (_dir.y() > 0)
	//		sqlOrderBy += L"gg.glbminy ASC";
	//	else
	//		sqlOrderBy += L"gg.glbmaxy DESC";

	//	if (_dir.x() > 0)
	//		sqlOrderBy += L", gg.glbminx ASC";
	//	else
	//		sqlOrderBy += L", gg.glbmaxx DESC";

	//	// 暂时不用z排序
	//	//if (_dir.z() > 0)
	//	//	sqlOrderBy += L", _geo.glbminz ASC";
	//	//else
	//	//	sqlOrderBy += L", _geo.glbminz DESC";

	//}

	//queryFilter->SetPostfixClause(sqlOrderBy.c_str());
	//return true;
}

glbBool GlbGlobe::CGlbGlobeFeatureLayer::Clip( glbBool isClip )
{
	mpr_isClip = isClip;
	if(isClip == false && mpr_clipObject.valid())
		mpr_clipObject->Clip(this,false);
	return true;
}

glbBool GlbGlobe::CGlbGlobeFeatureLayer::IsClip()
{
	return mpr_isClip;
}

void GlbGlobe::CGlbGlobeFeatureLayer::SetClipObject( CGlbClipObject *clipObject )
{
	mpr_clipObject = clipObject;
}
