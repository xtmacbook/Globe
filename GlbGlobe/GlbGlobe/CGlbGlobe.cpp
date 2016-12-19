#include "stdafx.h"
#include "CGlbGlobe.h"
#include "GlbDataSourceFactory.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobeTiltPhotographyLayer.h"
#include <algorithm>
#include <osg/Matrixd>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osg/LightSource>
#include <osg/LightModel>
#include "GlbGlobeView.h"
#include "GlbGlobeDispatcher.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbLog.h"
//#include "GlbGlobeSettings.h"
//#define _GLIBCXX_USE_WSTRING
//#include <string>

static osg::Timer_t startFrameTick = -1;

using namespace GlbGlobe;
CGlbGlobe::CGlbGlobe(GlbGlobeTypeEnum globeType)
{
	if (globeType==GLB_GLOBETYPE_GLOBE)
		mpr_sceneobjIdxManager = new CGlbGlobeSceneObjIdxManager(L"Globe");
	mpr_taskmanager             = new CGlbGlobeTaskManager();		
	mpr_objectid_base           = 0;
	mpr_layerid_base            = 0;
	mpr_groupid_base            = 0;
	mpr_terrain                 = NULL;
	mpr_undergroundmode         = false;
	mpr_undergroundaltitude     = -2000.0;//���²ο���
	mpr_name                    = L"�¹���";//"NewProject";//"�¹���";//"D:\\��������\\��������-Model.grj"
	mpr_prjFile                 = L"";
	mpr_spatialreference        = L"";
	mpr_undergroundobjsOpactity = 100;
	mpr_groundobjsOpactity      = 100;
	mpr_defaultLocationID       = -1;
	mpr_defaultlocationobj      = NULL;
	mpr_prjPassword             = L"";
	mpr_is_initialized          = false;
	mpr_globeview               = NULL;
	mpr_type                    = globeType;
	mpr_dispatcher              = new CGlbGlobeDispatcher(this);
	mpr_root                    = new osg::Switch;
	mpr_root->setName("SceneRoot");
	mpr_root->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
	mpr_root->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	mpr_p_objects_groundnode    = NULL;
	mpr_p_objects_waternode    = NULL;
	mpr_p_objects_undergroundnode = NULL;
	mpr_TerrainNodeUpdate         = true;
	mpr_critical.SetName(L"globe_critical");
	mpr_updateAndDispatchCritical.SetName(L"globe_updatedispatch_critical");
	mpr_backColor = osg::Vec4f(0,0,0,1);
	mpr_undergroundBackColor = osg::Vec4f(0,0.5,1,1); //0, 128, 255 ����ɫ

	mpr_settings = new CGlbGlobeSettings();	
}


CGlbGlobe::~CGlbGlobe(void)
{	
	mpr_root               = NULL;
	mpr_terrain            = NULL;
	mpr_defaultlocationobj = NULL;
	delete mpr_dispatcher;
	mpr_dispatcher = NULL;
	mpr_settings = NULL;
	mpr_sceneobjIdxManager = NULL;
	mpr_p_callback         = NULL;		
	mpr_taskmanager        = NULL;	
}

/*********************************************************************/
const glbWChar* CGlbGlobe::GetName()
{
	return mpr_name.c_str();
}

void CGlbGlobe::SetName(const glbWChar* name)
{
	mpr_name = name;
}

GlbGlobeTypeEnum CGlbGlobe::GetType()
{
	return mpr_type;
}
/*
*   �ⲿ�߳�---->AddObject(obj,true)
*          |
*          |---->OpenPrj()------>AddObject(obj,false)
*
*/
glbBool CGlbGlobe::AddObject(CGlbGlobeObject* obj,glbBool needsetId)
{		    
	if (!obj)
		return false;
	if(needsetId)
	{
		obj->SetId(mpr_objectid_base);
		mpr_objectid_base++;
	}
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
		mpr_objects[obj->GetId()] = obj;	
		mpr_objids.push_back(obj->GetId());
	}


	if (!mpr_p_objects_groundnode)
	{
		mpr_p_objects_groundnode = new osg::Switch;
		mpr_p_objects_groundnode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
		//mpr_p_objects_groundnode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		mpr_root->addChild(mpr_p_objects_groundnode.get());
	}

	if (!mpr_p_objects_waternode)
	{
		mpr_p_objects_waternode = new osg::Switch;
		mpr_p_objects_waternode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
		//mpr_p_objects_groundnode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		mpr_root->addChild(mpr_p_objects_waternode.get());
	}

	if (!mpr_p_objects_undergroundnode)
	{
		mpr_p_objects_undergroundnode = new osg::Switch;	
		mpr_p_objects_undergroundnode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
		//mpr_p_objects_undergroundnode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		mpr_root->addChild(mpr_p_objects_undergroundnode.get());
	}
	/*
	*    dynamic_cast ���RTTI������ʱ���ͼ��
	*    ����ʱ����򿪣�����ѡ��GR���������ͼ��
	*    ���� --->���࣬����������麯��
	*/
	CGlbGlobeRObject* p_renderableobj = dynamic_cast<CGlbGlobeRObject*> (obj);
	if(p_renderableobj)
	{
		p_renderableobj->SetGlobe(this);

		GlbRenderInfo *renderInfo = p_renderableobj->GetRenderInfo();
		GlbPolygonLakeSymbolInfo *lakeRenderInfo = NULL;
		if(renderInfo)
			lakeRenderInfo = dynamic_cast<GlbPolygonLakeSymbolInfo *>(renderInfo);

		if(lakeRenderInfo)
			p_renderableobj->SetParentNode(mpr_p_objects_waternode);
		else if(p_renderableobj->IsGround())
			p_renderableobj->SetParentNode(mpr_p_objects_groundnode);
		else
			p_renderableobj->SetParentNode(mpr_p_objects_undergroundnode);		
		mpr_sceneobjIdxManager->AddObject(p_renderableobj);	   
	}
	return true;
}

glbBool CGlbGlobe::RemoveObject(glbInt32 id)
{
	bool res = false;
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr_find = mpr_objects.find(id);
	if (itr_find!=mpr_objects.end())
	{
		CGlbGlobeObject* p_obj = itr_find->second.get();
		//�ӳ���������ɾ������
		CGlbGlobeRObject* p_renderable_obj = dynamic_cast<CGlbGlobeRObject*>(p_obj);
		if(mpr_sceneobjIdxManager && p_renderable_obj)
		{
			mpr_sceneobjIdxManager->RemoveObject(p_renderable_obj);
			// �������ٱ�־
			p_renderable_obj->SetDestroy();
		}

		// ������mpr_objids�е�objid
		std::vector<glbInt32>::iterator itr = std::find(mpr_objids.begin(),mpr_objids.end(),p_obj->GetId());
		if (itr != mpr_objids.end())
		{
			mpr_objids.erase(itr);
		}		

		// ����obj
		mpr_objects.erase(itr_find);
		res = true;
	}
	else{//û�ҵ�,����false
	}					
	return res;
}
glbBool CGlbGlobe::MoveObject(glbInt32 src_id,glbInt32 target_id)
{
	if(src_id == target_id)return true;
	bool rt = false;
	bool   src_find = false;
	bool   target_find=false;
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::vector<glbInt32>::iterator itr_src = std::find(mpr_objids.begin(),mpr_objids.end(),src_id);
	std::vector<glbInt32>::iterator itr_target = std::find(mpr_objids.begin(),mpr_objids.end(),target_id);
	if (itr_src!=mpr_objids.end())
		src_find = true;
	if (itr_target!=mpr_objids.end())
		target_find = true;
	if (src_find && target_find)
	{
		mpr_objids.erase(itr_src);
		itr_target = std::find(mpr_objids.begin(),mpr_objids.end(),target_id);
		mpr_objids.insert(itr_target,src_id);
		rt = true;
	}

	return rt;
}
glbInt32 CGlbGlobe::GetObjectCount()
{
	return mpr_objects.size();
}

CGlbGlobeObject* CGlbGlobe::GetObjectByIndex(glbInt32 idx)
{
	CGlbGlobeObject* resObj = NULL;
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	glbInt32 objId = mpr_objids[idx];
	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr_find = mpr_objects.find(objId);
	if (itr_find!=mpr_objects.end())
	{
		resObj = itr_find->second.get();
	}

	return resObj;
}

CGlbGlobeObject* CGlbGlobe::GetObjectById(glbInt32 id)
{
	CGlbGlobeObject* resObj = NULL;
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::map<glbInt32,glbref_ptr<CGlbGlobeObject>>::iterator itr_find = mpr_objects.find(id);
	if (itr_find!=mpr_objects.end())
	{
		resObj = itr_find->second.get();
	}

	return resObj;
}

void CGlbGlobe::UpdateObject(CGlbGlobeObject* obj)
{
	CGlbGlobeRObject* p_renderableobj = dynamic_cast<CGlbGlobeRObject*> (obj);
	if(p_renderableobj)
	{
		mpr_sceneobjIdxManager->RemoveObject(p_renderableobj);	 
		if(p_renderableobj->IsDestroy()==false)
			mpr_sceneobjIdxManager->AddObject(p_renderableobj);	 
	}
}

glbBool CGlbGlobe::AddLayer(CGlbGlobeLayer* layer)
{
	if(layer == NULL) return false;
	layer->SetGlobe(this);// ����globe����
	layer->SetParentNode(mpr_root);
	osg::Node* p_layer_node = layer->GetOsgNode();
	if (p_layer_node)
	{
		if (mpr_p_callback)
		{
			glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> hangtask = NULL;
			if (layer->GetType()==GLB_GLOBELAYER_TILTPHOTO)
			{
				mpr_terrain->AddTiltPhotographLayer(layer);
				hangtask = new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpr_terrain->GetNode(),p_layer_node);
			}
			else
				hangtask = new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpr_root.get(),p_layer_node);
			mpr_p_callback->AddHangTask(hangtask.get());
		}
		else
		{	// ��б��Ӱͼ���ɵ��ζ�����й���
			if (layer->GetType()==GLB_GLOBELAYER_TILTPHOTO)
			{
				mpr_terrain->AddTiltPhotographLayer(layer);
				mpr_terrain->GetNode()->asGroup()->addChild(p_layer_node);
			}
			else
				mpr_root->addChild(p_layer_node);// ���layer�ڵ�
		}
	}	

	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	layer->SetId(mpr_layerid_base);
	mpr_layerid_base++;
	mpr_layers.push_back(layer);

	return true;
}
glbBool CGlbGlobe::RemoveLayer(glbInt32 id)
{
	bool rt = false;
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	vector<glbref_ptr<CGlbGlobeLayer>>::iterator itr = mpr_layers.begin();
	vector<glbref_ptr<CGlbGlobeLayer>>::iterator itrEnd = mpr_layers.end();
	for(itr;itr!=itrEnd;itr++)
	{
		if ((*itr)->GetId() == id)
		{				
			osg::Node* p_layer_node = (*itr)->GetOsgNode();
			glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> removetask = NULL;
			if ((*itr)->GetType()==GLB_GLOBELAYER_TILTPHOTO)
			{
				mpr_terrain->RemoveTiltPhotographLayer((*itr).get());
				removetask = new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpr_terrain->GetNode(),p_layer_node);
			}
			else
				removetask = new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpr_root.get(),p_layer_node);

			mpr_p_callback->AddRemoveTask(removetask.get());			

			(*itr)->SetDestroy();
			mpr_layers.erase(itr);						
			rt = true;
			break;
		}
	}	
	return rt;
}
glbBool CGlbGlobe::MoveLayer(glbInt32 src_id, glbInt32 target_id)
{
	if(src_id == target_id)return true;
	bool rt = false;
	size_t src_index = 0;
	size_t target_index=0;
	bool   src_find = false;
	bool   target_find=false;

	size_t count = mpr_layers.size();
	for(size_t i=0;i<count;i++)
	{
		if(mpr_layers[i]->GetId()==src_id)
		{
			src_index = i;
			src_find  = true;
		}
		if(mpr_layers[i]->GetId()==target_id)
		{
			target_index=i;
			target_find =true;
		}
		if(src_find && target_find)break;
	}
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	if(src_find && target_find)
	{
		glbref_ptr<CGlbGlobeLayer> src_tmp    = mpr_layers[src_index];
		mpr_layers.insert(mpr_layers.begin()+target_index,src_tmp);
		if(target_index>src_index)
			mpr_layers.erase(mpr_layers.begin()+src_index);
		else
			mpr_layers.erase(mpr_layers.begin()+src_index+1);
		rt = true;
	}

	return rt;
}
CGlbGlobeLayer* CGlbGlobe::GetLayer(glbInt32 layer_idx)
{

	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	if(layer_idx>=0 && layer_idx<(long)mpr_layers.size())
		return mpr_layers[layer_idx].get();
	return NULL;
}
CGlbGlobeLayer* CGlbGlobe::GetLayerById(glbInt32 layer_id)
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	for(long i = 0; i < (long)mpr_layers.size(); i++)
	{
		if (mpr_layers[i]->GetId() == layer_id)
		{
			return mpr_layers[i].get();
		}		
	}
	return NULL;
}
glbInt32 CGlbGlobe::GetLayerCount()
{
	return mpr_layers.size();
}

glbInt32 CGlbGlobe::CreateGroup(glbInt32 parent_grpid, glbWChar* group_name)
{
	GlbGroup new_group;
	new_group.id = mpr_groupid_base;
	new_group.name = group_name;
	new_group.parentId = parent_grpid;
	new_group.visible = true;
	mpr_groupid_base++;
	mpr_groups.push_back(new_group);
	return new_group.id;
}

glbBool CGlbGlobe::RemoveGroup(glbInt32 grp_id)
{
	std::vector<GlbGroup>::iterator itr = mpr_groups.begin();
	std::vector<GlbGroup>::iterator itrEnd=mpr_groups.end();
	long parentid = 0;
	bool rt = false;
	for(itr;itr!=itrEnd;itr++)
	{
		if((*itr).id == grp_id)
		{
			parentid = (*itr).parentId;
			mpr_groups.erase(itr);
			rt = true;
			break;
		}
	}

	if(rt)
	{
		//������PareantId == layer_id�����ParentId��Ϊlayer_id��parentId
		std::vector<GlbGroup>::iterator itr = mpr_groups.begin();
		std::vector<GlbGroup>::iterator itrEnd=mpr_groups.end();
		for(itr;itr!=itrEnd;itr++)
		{
			if((*itr).parentId == grp_id)
				(*itr).parentId = parentid;
		}

		// ɾ�����ڴ�������е�ͼ��Ͷ���
		std::vector<glbref_ptr<CGlbGlobeLayer>>::iterator itrlyr = mpr_layers.begin();
		while(itrlyr != mpr_layers.end())
		{
			if((*itrlyr)->GetGroupId()==grp_id)
			{
				RemoveLayer((*itrlyr)->GetId());
			}	
			++itrlyr;
		}
		std::map<glbInt32/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator itrobj = mpr_objects.begin();
		while(itrobj != mpr_objects.end())
		{
			if(itrobj->second->GetGroupId()==grp_id)
			{
				RemoveObject(itrobj->second->GetId());
			}	
			++itrobj;
		}		
	}
	return rt;
}
glbBool CGlbGlobe::MoveGroup(glbInt32 src_id, glbInt32 target_id)
{
	if(src_id == target_id)return true;
	bool rt = false;
	size_t src_index = 0;
	size_t target_index=0;
	bool   src_find = false;
	bool   target_find=false;

	size_t count = mpr_groups.size();
	for(size_t i=0;i<count;i++)
	{
		if(mpr_groups[i].id==src_id)
		{
			src_index = i;
			src_find  = true;
		}
		if(mpr_groups[i].id==target_id)
		{
			target_index=i;
			target_find =true;
		}
		if(src_find && target_find)break;
	}
	if(src_find && target_find)
	{
		GlbGroup src_group    = mpr_groups[src_index];
		mpr_groups.insert(mpr_groups.begin()+target_index,src_group);
		if(target_index>src_index)
			mpr_groups.erase(mpr_groups.begin()+src_index);
		else
			mpr_groups.erase(mpr_groups.begin()+src_index+1);
		rt = true;
	}	
	return rt;
}	

glbInt32 CGlbGlobe::GetGroupCount()
{
	return mpr_groups.size();
}
glbBool CGlbGlobe::GetGroup(glbInt32 grp_idx, glbInt32* grp_id, const glbWChar** grp_name,  glbInt32* parent_id )
{
	if(grp_idx>=0 && grp_idx<(long)mpr_groups.size())
	{
		if(grp_id) *grp_id=mpr_groups[grp_idx].id;
		if(parent_id)*parent_id=mpr_groups[grp_idx].parentId;
		if(grp_name) *grp_name = (glbWChar*)mpr_groups[grp_idx].name.c_str();

		return true;
	}
	return false;
}
glbBool CGlbGlobe::GetGroupById(glbInt32 grp_id, const glbWChar** grp_name,  glbInt32* parent_id)
{
	for(size_t i=0;i<mpr_groups.size();i++)
	{
		if(mpr_groups[i].id==grp_id)
		{
			if(parent_id)*parent_id=mpr_groups[i].parentId;
			if(grp_name) *grp_name = (glbWChar*)mpr_groups[i].name.c_str();
			return true;
		}
	}
	return false;
}
glbBool CGlbGlobe::SetGroupNameById(glbInt32 grp_id, glbWChar* group_name)
{
	for(size_t i=0;i<mpr_groups.size();i++)
	{
		if(mpr_groups[i].id==grp_id)
		{
			mpr_groups[i].name = group_name;
			return true;
		}
	}	
	return false;
}
glbBool CGlbGlobe::SetGroupVisible(glbInt32 grp_id , glbBool visible)
{
	for(size_t i=0;i<mpr_groups.size();i++)
	{
		if(mpr_groups[i].id==grp_id)
		{
			mpr_groups[i].visible = visible;
			return true;
		}
	}
	return false;
}
glbBool CGlbGlobe::GetGroupVisible(glbInt32 grp_id)
{
	for(size_t i=0;i<mpr_groups.size();i++)
	{
		if(mpr_groups[i].id==grp_id)
		{
			return mpr_groups[i].visible;
		}
	}
	return true;
}
CGlbGlobeTerrain* CGlbGlobe::GetTerrain()
{
	return mpr_terrain.get();
}
glbBool CGlbGlobe::SetTerrainDataset(IGlbTerrainDataset* dataset)
{	
	if(dataset==NULL)return false;	
	if (!mpr_terrain)
		mpr_terrain = new CGlbGlobeTerrain(this);
	mpr_type = (dataset->IsGlobe()==true) ? GLB_GLOBETYPE_GLOBE : GLB_GLOBETYPE_FLAT;		
	// ע�����õ������ݼ��͵��θ���֮��Ķ��߳�ͬ������,���ٽ���
	glbBool res = mpr_terrain->SetTerrainDataset(dataset);

	// �Ƶ�SetTerrainDataset��
	//if(mpr_type == GLB_GLOBETYPE_FLAT)
	//{// ƽ��ģʽ�ڴ�ʱ���ó����������ĳ�ʼ��ΧΪ���ε���Ч���� 2013.8.22 ����
	//	glbref_ptr<CGlbExtent> _extent = (CGlbExtent*)dataset->GetExtent();
	//	if(mpr_sceneobjIdxManager) mpr_sceneobjIdxManager=NULL;

	//	mpr_sceneobjIdxManager = new CGlbGlobeSceneObjIdxManager(L"CreatarGlobe SceneManager",8/*Ĭ������8��*/,_extent.get());
	//}

	return res;
}
IGlbTerrainDataset* CGlbGlobe::GetTerrainDataset()
{
	return mpr_terrain->GetTerrainDataset();
}
glbBool CGlbGlobe::SetTerrainOpacity(glbInt32 opacity)
{
	// �Լ��ռ���ǰ��Ҫ��ʾ��domlayers��demlayers
	std::vector<glbref_ptr<CGlbGlobeDomLayer>> domLayers;
	std::vector<glbref_ptr<CGlbGlobeDemLayer>> demLayers;
	glbBool bsucc = GetVisibleLayers(domLayers,demLayers);

	//�ӵ������õ������λ��ƶ��� terrainObjs
	//�ӵ������õ������޸Ķ���   mTerrainObjs
	std::vector<glbref_ptr<CGlbGlobeRObject>> onterrainobjs;
	std::vector<glbref_ptr<CGlbGlobeRObject>> mterrainobjs;
	std::vector<glbref_ptr<CGlbGlobeRObject>> rterrainobjs;
	mpr_dispatcher->GetTerrainObjects(onterrainobjs,rterrainobjs,mterrainobjs);

	return mpr_terrain->SetOpacity(domLayers,onterrainobjs,opacity);
}
glbInt32 CGlbGlobe::GetTerrainOpacity()
{
	return mpr_terrain->GetOpacity();
}
glbBool CGlbGlobe::SetTerrainVisbile(glbBool visible)
{
	return mpr_terrain->SetVisible(visible);
}	
glbBool CGlbGlobe::IsTerrainVisbile()
{
	return mpr_terrain->GetVisible();
}
glbBool CGlbGlobe::SetTerrainExaggrate(glbDouble exaggrate)
{
	return mpr_terrain->SetExaggrate(exaggrate);
}
glbDouble CGlbGlobe::GetTerrainExaggrate()
{
	return mpr_terrain->GetExaggrate();
}
void  CGlbGlobe::EnableTerrainUpdate(glbBool enable)
{
	mpr_terrain->EnableUpdate(enable);

	mpr_TerrainNodeUpdate = false;
}

glbBool CGlbGlobe::IsTerrainUpdateEnable()
{
	return mpr_terrain->IsUpdateEnable();
}

glbBool CGlbGlobe::SetUnderGroundMode(glbBool mode)
{
	if (mpr_undergroundmode==mode)
		return true;

	mpr_undergroundmode = mode;
	// �������� ��:֪ͨglobeview���ڵ�ģʽ

	return true;
}
glbBool CGlbGlobe::IsUnderGroundMode()
{
	return mpr_undergroundmode;
}
glbBool CGlbGlobe::SetUnderGroundAltitude(glbDouble altitude)
{
	if (mpr_undergroundaltitude == altitude)
		return true;

	mpr_undergroundaltitude = altitude;
	// ֪ͨ���²ο��棬��߶ȱ仯��
	mpr_ugplane->SetAltitude(altitude);
	return true;
}
glbDouble CGlbGlobe::GetUnderGroundAltitude()
{
	return mpr_undergroundaltitude;
}

glbBool CGlbGlobe::Open(const glbWChar* prjFile)
{
	CGlbWString wPrjFile(prjFile);
	CGlbString szDocName = wPrjFile.ToString();
	xmlDocPtr doc = xmlReadFile(szDocName.c_str(),"GB2312",XML_PARSE_RECOVER);
	if (NULL == doc)
	{  	
		CGlbWString err = L"�򿪹����ļ�:";
		err += wPrjFile;
		err += L" ʧ��";
		GlbSetLastError(err.c_str());
		return false;
	}

	if(doc->children && (!xmlStrcmp(doc->children->name, (const xmlChar *)"SerialProject")))
	{
		return OpenProjectSaveFile(prjFile);
	}
	else
	{
		return OpenGlobeSaveFile(prjFile);
	}
}

glbBool CGlbGlobe::Save()
{
	//<?xml version="1.0"?>
	//	<root>
	//	<newNode1>newNode1 content</newNode1>
	//	<newNode2>newNode2 content</newNode2>
	//	<newNode3>newNode3 content</newNode3>
	//	<node2 attribute="yes">NODE CONTENT</node2>
	//	<son>
	//	<grandson>This is a grandson node</grandson>
	//	</son>
	//	</root>
	if(mpr_prjFile.length()==0)
	{
		GlbSetLastError(L"��Ŀ�ļ���");
		return false;
	}
	setlocale(LC_ALL,"");
	//mpr_prjFile = L"D:\\����.xml";	
	char str[128];
	// create xml document  
	xmlNewDoc(BAD_CAST"1.0");  
	//�����ĵ��ͽڵ�ָ��
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"Globe");  
	//���ø��ڵ�
	xmlDocSetRootElement(doc,root_node);
	//����globe������	
	//xmlNewProp(root_node,BAD_CAST "PrjName",BAD_CAST mpr_name.ToString().c_str());
	//һ�������ַ���ת��ΪUTF-8�ַ�����Ȼ��д��
	char* szOut = g2u((char*)mpr_name.ToString().c_str());
	xmlNewTextChild(root_node, NULL, BAD_CAST "ProjName", BAD_CAST szOut/*mpr_name.ToString().c_str()*/);
	free(szOut);
	//xmlNewProp(root_node,BAD_CAST "PrjPassword", BAD_CAST mpr_prjPassword.ToString().c_str());
	szOut = g2u((char*)mpr_prjPassword.ToString().c_str());
	xmlNewTextChild(root_node, NULL, BAD_CAST "PrjPassword", BAD_CAST szOut /*mpr_prjPassword.ToString().c_str()*/);
	free(szOut);
	sprintf(str,"%d",mpr_type);
	//xmlNewProp(root_node,BAD_CAST "GlobeType", BAD_CAST str);
	xmlNewTextChild(root_node, NULL, BAD_CAST "GlobeType", BAD_CAST  str);
	sprintf(str,"%d",mpr_groupid_base);
	//xmlNewProp(root_node,BAD_CAST "GroupIdBase", BAD_CAST str);
	xmlNewTextChild(root_node, NULL, BAD_CAST "GroupIdBase", BAD_CAST  str);
	sprintf(str,"%d",mpr_objectid_base);
	//xmlNewProp(root_node,BAD_CAST "ObjectIdBase", BAD_CAST str);
	xmlNewTextChild(root_node, NULL, BAD_CAST "ObjectIdBase", BAD_CAST  str);
	sprintf(str,"%d",mpr_layerid_base);
	//xmlNewProp(root_node,BAD_CAST "LayerIdBase", BAD_CAST str);
	xmlNewTextChild(root_node, NULL, BAD_CAST "LayerIdBase", BAD_CAST  str);
	sprintf(str,"%d",mpr_undergroundmode);
	//xmlNewProp(root_node,BAD_CAST "UndergroundMode", BAD_CAST str);
	xmlNewTextChild(root_node, NULL, BAD_CAST "UndergroundMode", BAD_CAST  str);
	sprintf(str,"%.6lf",mpr_undergroundaltitude);
	//xmlNewProp(root_node,BAD_CAST "UndergroundAltitude", BAD_CAST str);	
	xmlNewTextChild(root_node, NULL, BAD_CAST "UndergroundAltitude", BAD_CAST  str);
	//xmlNewProp(root_node,BAD_CAST "SpatialReference", BAD_CAST mpr_spatialreference.ToString().c_str());	
	xmlNewTextChild(root_node, NULL, BAD_CAST "SpatialReference", BAD_CAST  mpr_spatialreference.ToString().c_str());
	sprintf(str,"%d",mpr_defaultLocationID);
	//xmlNewProp(root_node,BAD_CAST "DefaultLocationObjectID", BAD_CAST str);	
	xmlNewTextChild(root_node, NULL, BAD_CAST "DefaultLocationObjectID", BAD_CAST  str);
	sprintf(str,"%d",mpr_groundobjsOpactity);
	//xmlNewProp(root_node,BAD_CAST "GroundObjOpacity", BAD_CAST str);
	xmlNewTextChild(root_node, NULL, BAD_CAST "GroundObjOpacity", BAD_CAST  str);
	sprintf(str,"%d",mpr_undergroundobjsOpactity);
	//xmlNewProp(root_node,BAD_CAST "UndergroundObjOpacity", BAD_CAST str);
	xmlNewTextChild(root_node, NULL, BAD_CAST "UndergroundObjOpacity", BAD_CAST  str);

	// ����
	xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"Group");
	xmlAddChild(root_node,node);
	size_t grpCount = mpr_groups.size();
	sprintf(str,"%d",grpCount);
	//xmlNewProp(node,BAD_CAST "GroupNum", BAD_CAST str);	
	xmlNewTextChild(node, NULL, BAD_CAST "GroupNum", BAD_CAST  str);
	for(size_t i=0;i<grpCount;i++)
	{
		szOut = g2u((char*)mpr_groups[i].name.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "GroupName", BAD_CAST szOut);
		free(szOut);
		sprintf(str,"%d",mpr_groups[i].id);
		//xmlNewProp(node,BAD_CAST "GroupId", BAD_CAST str);	
		xmlNewTextChild(node, NULL, BAD_CAST "GroupId", BAD_CAST  str);
		sprintf(str,"%c",mpr_groups[i].visible);
		//xmlNewProp(node,BAD_CAST "GroupVisible", BAD_CAST str);	
		xmlNewTextChild(node, NULL, BAD_CAST "GroupVisible", BAD_CAST  str);
		sprintf(str,"%d",mpr_groups[i].parentId);
		//xmlNewProp(node,BAD_CAST "GroupParentId", BAD_CAST str);
		xmlNewTextChild(node, NULL, BAD_CAST "GroupParentId", BAD_CAST  str);
	}

	//���·��
	CGlbWString rpath = CGlbPath::GetDir(mpr_prjFile);
	/*szOut = g2u((char*)rpath.ToString().c_str());
	xmlNewTextChild(root_node, NULL, BAD_CAST "RelativePath", BAD_CAST szOut);
	free(szOut);*/
	//����Դ
	node = xmlNewNode(NULL,BAD_CAST "DataSources");
	xmlAddChild(root_node,node);
	CGlbDataSourceFactory *ft = CGlbDataSourceFactory::GetInstance();
	ft->Save(node,(glbWChar*)rpath.c_str(),mpr_prjPassword.c_str());
	//����ͼ��
	node = xmlNewNode(NULL,BAD_CAST "Terrain");
	xmlAddChild(root_node,node);
	mpr_terrain->Save(node,(glbWChar*)rpath.c_str());

	////ͼ��
	node = xmlNewNode(NULL,BAD_CAST "Layers");
	xmlAddChild(root_node,node);

	size_t layerCount = mpr_layers.size();
	sprintf(str,"%d",layerCount);
	//xmlNewProp(node,BAD_CAST "LayerNum", BAD_CAST str);	
	xmlNewTextChild(node, NULL, BAD_CAST "LayerNum", BAD_CAST  str);

	for(size_t i=0;i<layerCount;i++)
	{
		xmlNodePtr layernode = xmlNewNode(NULL,BAD_CAST "Layer");
		xmlAddChild(node,layernode);

		GlbGlobeLayerTypeEnum lyrType = mpr_layers.at(i)->GetType();
		sprintf(str,"%d",lyrType);			
		xmlNewTextChild(layernode, NULL, BAD_CAST "LayerType", BAD_CAST  str);

		if (lyrType==GLB_GLOBELAYER_TILTPHOTO)
		{// ������б��Ӱģ��ͼ����Ҫ��¼��tpf�ļ�·��
			CGlbGlobeTiltPhotographyLayer* tiltLayer = dynamic_cast<CGlbGlobeTiltPhotographyLayer*>(mpr_layers.at(i).get());
			const glbWChar* filepath = tiltLayer->GetTPFFilePath();
			CGlbWString wstrTPF = filepath;
			szOut = g2u((char*)wstrTPF.ToString().c_str());
			xmlNewTextChild(layernode, NULL, BAD_CAST "TPFPath", BAD_CAST szOut);
			free(szOut);
		}

		mpr_layers.at(i)->Save(layernode,(glbWChar*)rpath.c_str());
	}

	//����
	node = xmlNewNode(NULL,BAD_CAST "Objects");
	xmlAddChild(root_node,node);
	size_t objCount = mpr_objects.size();
	sprintf(str,"%d",objCount);
	//xmlNewProp(node,BAD_CAST "ObjectNum", BAD_CAST str);	
	xmlNewTextChild(node, NULL, BAD_CAST "ObjectNum", BAD_CAST  str);
	/*	for(size_t i=0;i<objCount;i++)
	{
	xmlNodePtr objnode = xmlNewNode(NULL,BAD_CAST "Object");
	xmlAddChild(node,objnode);

	GlbGlobeObjectTypeEnum objType = mpr_objects.at(i)->GetType();
	sprintf(str,"%d",objType);	
	xmlNewTextChild(objnode, NULL, BAD_CAST "ObjectType", BAD_CAST  str);		
	mpr_objects.at(i)->Save(objnode,(wchar_t*)rpath.c_str());
	}*/	

	std::map<glbInt32/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator objItr = mpr_objects.begin();
	while(objItr != mpr_objects.end())
	{
		xmlNodePtr objnode = xmlNewNode(NULL,BAD_CAST "Object");
		xmlAddChild(node,objnode);

		glbref_ptr<CGlbGlobeObject> obj = objItr->second;

		GlbGlobeObjectTypeEnum objType = obj->GetType();
		sprintf(str,"%d",objType);	

		xmlNewTextChild(objnode, NULL, BAD_CAST "ObjectType", BAD_CAST  str);		
		obj->Save(objnode,(wchar_t*)rpath.c_str());

		++objItr;
	}


	//�洢xml�ĵ�
	int nRel = xmlSaveFile(mpr_prjFile.ToString().c_str(),doc);
	//int nRel = xmlSaveFormatFileEnc(mpr_prjFile.ToString().c_str(),doc,"GB2312",1);	
	//�ͷ��ĵ��ڽڵ㶯̬������ڴ�
	xmlFreeDoc(doc);
	return true;
}
glbBool CGlbGlobe::SaveAs(glbWChar* newPrjFile)
{
	mpr_prjFile = newPrjFile;
	return Save();
}
const glbWChar* CGlbGlobe::GetPrjFile()
{
	return mpr_prjFile.c_str();
}
CGlbGlobeView* CGlbGlobe::GetView()
{
	return mpr_globeview;
}
const glbWChar* CGlbGlobe::GetSRS()
{
	return mpr_spatialreference.c_str();
}

glbDouble CGlbGlobe::GetElevationAt(glbDouble lonOrX, glbDouble latOrY)
{
	if(!mpr_terrain || mpr_terrain->IsInitialized()==false)
		return 0.0;
	return mpr_terrain->GetElevationAt(lonOrX,latOrY);
}
glbDouble CGlbGlobe::GetOnTerrainDistance(glbDouble xOrLonStart, glbDouble yOrLatStart, glbDouble xOrLonEnd, glbDouble yOrLatEnd, glbDouble step)
{
	if(!mpr_terrain || mpr_terrain->IsInitialized()==false)
		return 0.0;
	return mpr_terrain->ComputeOnTerrainDistance(xOrLonStart,yOrLatStart,xOrLonEnd,yOrLatEnd,step);
}
void CGlbGlobe::AddDomDirtyExtent(CGlbExtent &ext,glbBool  isDomLayerDirty)
{
	if(mpr_dispatcher)
		mpr_dispatcher->AddDomDirtyExtent(ext,isDomLayerDirty);	
}
void CGlbGlobe::RefreshTerrainTexture(std::vector<glbref_ptr<CGlbGlobeRObject >> objs)
{// �û�����Ҫˢ��ʱ����
	// 1. �ռ�domlayer����domlayers
	std::vector<glbref_ptr<CGlbGlobeDomLayer>> domLayers;
	std::vector<glbref_ptr<CGlbGlobeDemLayer>> demLayers;
	glbBool bsucc = GetVisibleLayers(domLayers,demLayers);
	// 2. �ӵ�������ȡ�����λ��ƶ��󼯺�
	std::vector<glbref_ptr<CGlbGlobeRObject>> onterrainobjs;
	std::vector<glbref_ptr<CGlbGlobeRObject>> mterrainobjs;
	std::vector<glbref_ptr<CGlbGlobeRObject>> rterrainobjs;
	mpr_dispatcher->GetTerrainObjects(onterrainobjs,rterrainobjs,mterrainobjs);
	// 3. objs �ŵ�terrainObjs
	for (size_t i = 0; i < objs.size(); i++)
	{
		onterrainobjs.push_back(objs[i]);
	}

	// �����λ��ƶ�������[����Ⱦ˳��]
	SortRObjsByRenderOrder(onterrainobjs);

	// 4. ����ˢ��
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::vector<CGlbExtent> domDirtyExts = mpr_domDirtyExts;
	std::vector<CGlbExtent> objDirtyExts = mpr_objDirtyExts;
	// �����Ⱦ����
	mpr_domDirtyExts.clear();
	mpr_objDirtyExts.clear();

	mpr_terrain->RefreshTexture(objDirtyExts,
		domDirtyExts,
		domLayers,
		onterrainobjs);
}

void CGlbGlobe::AddDemDirtyExtent(CGlbExtent &ext,glbBool  isDemLayerDirty)
{
	if(mpr_dispatcher)
		mpr_dispatcher->AddDemDirtyExtent(ext,isDemLayerDirty);
}

void CGlbGlobe::RefreshTerrainDem(std::vector<glbref_ptr<CGlbGlobeRObject >> mtobjs)
{// �û�������Ҫ��ʵʱ��������ˢ��
	// 1. �ռ�demlayer����demlayers
	std::vector<glbref_ptr<CGlbGlobeDomLayer>> domLayers;
	std::vector<glbref_ptr<CGlbGlobeDemLayer>> demLayers;
	glbBool bsucc = GetVisibleLayers(domLayers,demLayers);
	// 2. �ӵ�������ȡ�����λ��ƶ��󼯺�mterrainObjs��GetMTerrainObjects��
	std::vector<glbref_ptr<CGlbGlobeRObject>> onterrainobjs;
	std::vector<glbref_ptr<CGlbGlobeRObject>> mterrainobjs;
	std::vector<glbref_ptr<CGlbGlobeRObject>> rterrainobjs;
	mpr_dispatcher->GetTerrainObjects(onterrainobjs,rterrainobjs,mterrainobjs);
	// 3. objs �ŵ�mterrainObjs
	for (size_t i = 0; i < mtobjs.size(); i++)
	{
		mterrainobjs.push_back(mtobjs[i]);
	}
	// �����޸Ķ������� ������Ⱦ˳��
	SortRObjsByRenderOrder(mterrainobjs);

	// 4. ����ˢ��
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::vector<CGlbExtent> demDirtyExts = mpr_demDirtyExts;
	std::vector<CGlbExtent> mtobjDirtyExts = mpr_mtobjDirtyExts;
	// �����Ⱦ����
	demDirtyExts.clear();
	mtobjDirtyExts.clear();


	// ���θ߳�ˢ��
	mpr_terrain->RefreshDem(mtobjDirtyExts,
		demDirtyExts,
		demLayers,
		mterrainobjs);
}

glbBool CGlbGlobe::SetGroundObjectsOpacity(glbInt32 opacity)
{
	if (mpr_groundobjsOpactity == opacity)
		return true;
	mpr_groundobjsOpactity = opacity;

	osg::StateSet* stateset = mpr_p_objects_groundnode->getOrCreateStateSet();
	osg::StateAttribute::GLModeValue modeblend = stateset->getMode(GL_BLEND);
	if(modeblend != 1)
	{
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
		osg::ref_ptr<osg::BlendColor> pBlenColor = new osg::BlendColor(osg::Vec4(1.0,1.0,1.0,(float)opacity / 100));  
		osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::CONSTANT_ALPHA,osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
		stateset->setAttributeAndModes(pBlenColor.get(),osg::StateAttribute::ON);
		stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	}
	else
	{
		osg::ref_ptr<osg::BlendColor> pBlenColor=new osg::BlendColor(osg::Vec4(1.0,1.0,1.0,(float)opacity/ 100));  
		stateset->setAttributeAndModes(pBlenColor,osg::StateAttribute::ON);
	}
	return true;
}
glbInt32 CGlbGlobe::GetGroundObjectsOpacity()
{
	return mpr_groundobjsOpactity;
}
glbBool CGlbGlobe::SetUnderGroundObjectsOpacity(glbInt32 opacity)
{
	if (mpr_undergroundobjsOpactity == opacity)
		return true;
	mpr_undergroundobjsOpactity = opacity;

	osg::StateSet* stateset = mpr_p_objects_undergroundnode->getOrCreateStateSet();
	osg::StateAttribute::GLModeValue modeblend = stateset->getMode(GL_BLEND);
	if(modeblend != 1)
	{
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
		osg::ref_ptr<osg::BlendColor> pBlenColor = new osg::BlendColor(osg::Vec4(1.0,1.0,1.0,(float)opacity / 100));  
		osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::CONSTANT_ALPHA,osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
		stateset->setAttributeAndModes(pBlenColor.get(),osg::StateAttribute::ON);
		stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	}
	else
	{
		osg::ref_ptr<osg::BlendColor> pBlenColor=new osg::BlendColor(osg::Vec4(1.0,1.0,1.0,(float)opacity/ 100));  
		stateset->setAttributeAndModes(pBlenColor,osg::StateAttribute::ON);
	}
	return true;
}
glbInt32 CGlbGlobe::GetUnderGroundObjectsOpacity()
{
	return mpr_undergroundobjsOpactity;
}

void CGlbGlobe::NotifyCameraIsUnderground(glbBool underground)
{
	if(mpr_terrain)
		mpr_terrain->NotifyCameraIsUnderground(underground);

	glbFloat r,g,b,a;
	if (underground)
	{
		GetView()->GetUnderGroundBackColor(r,g,b,a);
		if(mpr_undergroundBackColor != osg::Vec4f(r,g,b,a))
		{
			GetView()->SetUnderGroundBackColor(r,g,b,a);
			mpr_undergroundBackColor = osg::Vec4f(r,g,b,a);
		}
	}
	else
	{
		GetView()->GetBackColor(r,g,b,a);
		if(mpr_backColor != osg::Vec4f(r,g,b,a))
		{
			GetView()->SetBackColor(r,g,b,a);
			mpr_backColor = osg::Vec4f(r,g,b,a);
		}
	}
}
void CGlbGlobe::ComputeElevation()
{
	if (!mpr_is_initialized)
		return;
	if (!mpr_globeview)
		return;
	osg::Vec3d camerpos,focuspos;
	mpr_globeview->GetCameraPos(camerpos);
	glbDouble elevation = mpr_terrain->GetElevationAt(camerpos.x(),camerpos.y());
	mpr_globeview->UpdateTerrainElevationOfCameraPos(elevation);

	//osg::Camera* pOsgCamera = mpr_globeview->GetOsgCamera();
	//if (pOsgCamera)
	//{
	//	osg::Vec3d eye,center,up;
	//	pOsgCamera->getViewMatrixAsLookAt(eye,center,up,1000.0);
	//	osg::Vec3d rayDir = center - eye;
	//	osg::Vec3d InterPos;
	//	glbBool bInter = mpr_terrain->IsRayInterTerrain(eye,rayDir,InterPos);	
	//	mpr_globeview->UpdateSightLineInterTerrain(bInter,InterPos);
	//}

	osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
	double frameTime = osg::Timer::instance()->delta_m(startFrameTick, endFrameTick);	

	if (frameTime>200.0)
	{// 0.2�����һ��
		mpr_globeview->UpdateFocusElevate();
		startFrameTick = osg::Timer::instance()->tick();	
	}
	GlbGlobeManipulatorTypeEnum mpType = mpr_globeview->GetManipulatorType();
	// �Զ�����ģʽ����ķ�ʽ��Ҫ����state
	if (mpType != GlbGlobeManipulatorTypeEnum::GLB_MANIPULATOR_ANIMATIONPATH)
		mpr_dispatcher->UpdateState();
}

glbBool CGlbGlobe::ScreenToTerrainCoordinate(glbDouble x,glbDouble y,glbDouble& lonOrX, glbDouble& latOrY, glbDouble& altOrZ)
{
	if(!mpr_terrain)
		return false;

#ifdef _DEBUG
	osg::Timer_t startFrameTick = osg::Timer::instance()->tick();	
#endif

	glbBool rt = mpr_terrain->ScreenToTerrainCoordinate(x,y,lonOrX,latOrY,altOrZ);
	//GlbLogWOutput(GLB_LOGTYPE_INFO,L"sx  %lf sy %lf ----> lon %lf lat %lf alt %lf\n",
	//	                           x,y,lonOrX,latOrY,altOrZ);
#ifdef _DEBUG
	osg::Timer_t currTime = osg::Timer::instance()->tick();
	double pickTime = osg::Timer::instance()->delta_m(startFrameTick,currTime);
	if (pickTime > 5)
	{
		wchar_t buff[128];
		swprintf(buff,L"ScreenToTerrainCoordinate use time %lf ms. isInterTerrian %d.\n ", pickTime, rt ? 1 : 0);
		OutputDebugString(buff);
	}

	CGlbGlobeLog::OutputLogString(L"ScreenToTerrainCoordinate use time %lf ms. isInterTerrian %d.\n ", pickTime, rt ? 1 : 0);
#endif	
	return rt;
}

glbBool CGlbGlobe::IsRayInterTerrain(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos)
{
	if(!mpr_terrain)
		return false;
	return mpr_terrain->IsRayInterTerrain(ln_pt1,ln_dir,InterPos);	 
}

glbBool CGlbGlobe::ScreenToUGPlaneCoordinate(glbInt32 x,glbInt32 y,glbDouble &lonOrx,glbDouble &latOry,glbDouble &altOrz)
{
	if (!mpr_ugplane)
		return false;
	return mpr_ugplane->ScreenToUGPlaneCoordinate(x,y,lonOrx,latOry,altOrz);
}

glbBool CGlbGlobe::IsRayInterUGPlane(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos)
{
	if(!mpr_ugplane)
		return false;
	return mpr_ugplane->IsRayInterUGPlane(ln_pt1,ln_dir,InterPos);	 
}

glbBool CGlbGlobe::SetDefaultLocation(glbInt32 lctID)
{
	std::map<glbInt32/*id*/,glbref_ptr<CGlbGlobeObject>>::iterator itr = mpr_objects.find(lctID);
	if (itr != mpr_objects.end())
	{// �ҵ�lctID�Ķ���
		mpr_defaultlocationobj = itr->second;
		return true;
	}	
	return false;
}
CGlbGlobeObject* CGlbGlobe::GetDefaultLocation()
{
	return mpr_defaultlocationobj.get();
}

glbBool CGlbGlobe::SetPrjPassword(glbWChar* oldPassword,glbWChar* newPassword)
{	
	glbWChar* prjPassword = (glbWChar*)(mpr_prjPassword.c_str());
	if (wcscmp(prjPassword,oldPassword)==0)
	{// �ַ�����ͬ
		mpr_prjPassword = newPassword;
	}
	return false;
}
glbWChar* CGlbGlobe::GetLastError()
{
	return NULL;
}

//#include <osg/ShapeDrawable>
glbBool CGlbGlobe::Initialize()
{
	// ֻ��ʼ��һ��
	if (mpr_is_initialized)
		return true;

	// ����һ��������dataset�����ζ���ָ��϶���Ϊ��
	if (!mpr_terrain)
		return false;

	glbBool res = mpr_terrain->Initialize();
	if (res)
	{// ��Terrian�ڵ�ҵ�mpr_root��		
		osg::Node* pTerrianNode = mpr_terrain->GetNode();
		if (!pTerrianNode) return false;	

		// �����νڵ�ŵ�root�ڵ�ĵ�һ���ӽڵ�λ��
		if (mpr_root->getNumChildren()<=0)
			mpr_root->addChild(pTerrianNode);
		else
			mpr_root->insertChild(0,pTerrianNode);
	}	

	if (!mpr_p_objects_groundnode)
	{
		mpr_p_objects_groundnode = new osg::Switch;
		mpr_p_objects_groundnode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON| osg::StateAttribute::OVERRIDE);
		//mpr_p_objects_groundnode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		mpr_root->addChild(mpr_p_objects_groundnode.get());
	}

	if (!mpr_p_objects_waternode)
	{
		mpr_p_objects_waternode = new osg::Switch;
		mpr_p_objects_waternode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON| osg::StateAttribute::OVERRIDE);
		//mpr_p_objects_waternode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		mpr_root->addChild(mpr_p_objects_waternode.get());
	}

	if (!mpr_p_objects_undergroundnode)
	{
		mpr_p_objects_undergroundnode = new osg::Switch;	
		mpr_p_objects_undergroundnode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
		//mpr_p_objects_undergroundnode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		mpr_root->addChild(mpr_p_objects_undergroundnode.get());
	}

	mpr_p_callback = new CGlbGlobeCallBack();
	mpr_root->setUpdateCallback(mpr_p_callback);
	//���²ο���
	mpr_ugplane = new CGlbGlobeUGPlane(this);
	mpr_root->addChild(mpr_ugplane->getNode());

	// ����save��open
	//Save();
	//Open(mpr_prjFile.c_str());
	// end ���� save , open

	//#ifdef _DEBUG
	//	osg::Geode* geode = new osg::Geode;
	//	osg::Vec3d pos;
	//	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(39.9),osg::DegreesToRadians(116.3),5000,pos.x(),pos.y(),pos.z());
	//	float radius = 200.0f;	
	//	osg::TessellationHints* hints = new osg::TessellationHints;
	//	hints->setDetailRatio(0.8f);
	//	geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(pos,radius),hints));	
	//	mpr_p_objects_groundnode->addChild(geode);
	//#endif

	mpr_is_initialized = true;
	return true;
}

void CGlbGlobe::Update()
{	
	osg::Timer_t preTime = 0;
	if(true)
	{//����ʱ��.
		preTime = osg::Timer::instance()->tick();
	}
	if (mpr_is_initialized==false)
		return;	
	// 1. �Լ��ռ���ǰ��Ҫ��ʾ��domlayers��demlayers
	std::vector<glbref_ptr<CGlbGlobeDomLayer>> domLayers;
	std::vector<glbref_ptr<CGlbGlobeDemLayer>> demLayers;
	glbBool bsucc = GetVisibleLayers(domLayers,demLayers);

	//�ӵ������õ������λ��ƶ��� terrainObjs
	//�ӵ������õ������޸Ķ���   mTerrainObjs
	std::vector<glbref_ptr<CGlbGlobeRObject>> onterrainobjs;
	std::vector<glbref_ptr<CGlbGlobeRObject>> mterrainobjs;
	std::vector<glbref_ptr<CGlbGlobeRObject>> rterrainobjs;
	//mpr_dispatcher->GetTerrainObjects(onterrainobjs,rterrainobjs,mterrainobjs);
	std::vector<CGlbExtent> objDirtyExts;
	std::vector<CGlbExtent> mtobjDirtyExts;
	std::vector<CGlbExtent> domDirtyExts;
	std::vector<CGlbExtent> demDirtyExts;
	mpr_dispatcher->GetTerrainObjectsAndDirtyExts(onterrainobjs,rterrainobjs,mterrainobjs,
		objDirtyExts,domDirtyExts,mtobjDirtyExts,demDirtyExts);
	if(true)
	{//����ʱ��
		osg::Timer_t currTime = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_m(preTime,currTime);
		//GlbLogWOutput(GLB_LOGTYPE_INFO,L"***** get ted objs %ld using time %lf\n",onterrainobjs.size(),frameTime);
	}
	// �����λ��ƶ�������[����Ⱦ˳��]
	if(true)
	{//����ʱ��.
		preTime = osg::Timer::instance()->tick();
	}
	SortRObjsByRenderOrder(onterrainobjs);
	if(true)
	{//����ʱ��
		osg::Timer_t currTime = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_m(preTime,currTime);
		//GlbLogWOutput(GLB_LOGTYPE_INFO,L"***** sort ted objs %ld using time %lf\n",onterrainobjs.size(),frameTime);
	}
	// �����޸Ķ������� ������Ⱦ˳��
	SortRObjsByRenderOrder(mterrainobjs);

	// 2. ���θ���
	if(true)
	{//����ʱ��.
		preTime = osg::Timer::instance()->tick();
	}
	if (mpr_terrain)
	{		
		mpr_terrain->Update(domLayers,demLayers,onterrainobjs,mterrainobjs);
	}
	if(true)
	{//����ʱ��
		osg::Timer_t currTime = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_m(preTime,currTime);
		//GlbLogWOutput(GLB_LOGTYPE_INFO,L"***** update ted objs %ld using time %lf\n",onterrainobjs.size(),frameTime);
		//LOG2("***** update ted objs %ld using time",frameTime);
	}
	// 3. ��Ե��ζ����������θ߶� [�����λ��ƶ����Ƿ�Ҳ��Ҫ��������θ߶��Ա������������ľ���]
	// �ӵ������õ���Ե��λ��ƶ��� rTerrainObjs	
	std::vector<glbref_ptr<CGlbGlobeRObject>>::iterator itr = rterrainobjs.begin();
	while(itr != rterrainobjs.end())
	{		
		// UpdateElevate���˶���һ������CGlbGlobe::GetElevationAt��ʱ��
		(*itr)->UpdateElevate();		
		++itr;
	}

	// 4. ���㵱ǰ����ɼ�����
	/*
	��ȡ���οɼ�����[�������]
	GLB_GLOBE	x : ����(��) y : γ�ȣ��ȣ� z : �߶�(��)
	GLB_FLAT	x ,y,z (��)
	*/
	CGlbExtent e1,e2;	
	//osg::Timer_t startFrameTick = osg::Timer::instance()->tick();		
	glbInt32 res = mpr_terrain->ComputeVisibleExtents(e1,e2,-66.5,72.0);/*,-66.5,66.5);*/

	// 5. Ҫ��ͼ�����
	if(true)
	{//����ʱ��.
		preTime = osg::Timer::instance()->tick();
	}
	long layer_cnt = GetLayerCount();
	for (long i = 0; i < layer_cnt; i++)
	{
		glbref_ptr<CGlbGlobeLayer> pLayer = GetLayer(i);
		CGlbGlobeFeatureLayer* pFL = dynamic_cast<CGlbGlobeFeatureLayer*>(pLayer.get());
		if (pFL){							
			pFL->SetVisibleTerrainExtent(&e1,&e2);									
			pFL->Update();			
		}
	}	
	if(true)
	{//����ʱ��
		osg::Timer_t currTime = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_m(preTime,currTime);
		//GlbLogWOutput(GLB_LOGTYPE_INFO,L"***** flayer update objs %ld using time %lf\n",onterrainobjs.size(),frameTime);
		//LOG2("***** featurelayer update objs %ld using time",frameTime);
	}
#if 1 // �����볡�����·��뵽2���߳���
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_updateAndDispatchCritical);
		mpr_visibleExt.clear();
		if (e1.IsValid())
			mpr_visibleExt.push_back(e1);
		if (e2.IsValid())
			mpr_visibleExt.push_back(e2);
	}
#else
	// 6. �ӳ�����������ȡ�ɼ����󼯺�
	std::vector<glbref_ptr<CGlbGlobeObject>> visible_objects;
	if (mpr_sceneobjIdxManager)
	{
		osg::Camera* p_osgcamera = mpr_globeview->GetOsgCamera();
		osg::Matrixd _modelView = p_osgcamera->getViewMatrix();
		osg::Matrixd _projection = p_osgcamera->getProjectionMatrix();
		osg::Polytope cv;
		cv.setToUnitFrustum();
		cv.transformProvidingInverse((_modelView)*(_projection));	
		bool res=false;
		// �ȼ����ػ��ƵĶ���
		if (e1.IsValid())
			res = mpr_sceneobjIdxManager->QueryOnTerrainObjects(&e1,visible_objects);
		if (e2.IsValid())
			res = mpr_sceneobjIdxManager->QueryOnTerrainObjects(&e2,visible_objects);
		//OutputDebugString(L"query begin.\n");
		// ��Ӿ��Ժ���Ե��θߵĶ���
		res = mpr_sceneobjIdxManager->Query(cv,visible_objects,true);		
		//OutputDebugString(L"query end.\n");
	}
	// end ���ݿɼ�����....

	//*
	//	7. ������update��visible_objects,visible_onterrain_objects��
	//*/
	mpr_dispatcher->Update(visible_objects);
#endif

	//std::vector<CGlbExtent> objDirtyExts;
	//std::vector<CGlbExtent> mtobjDirtyExts;
	//std::vector<CGlbExtent> domDirtyExts;
	//std::vector<CGlbExtent> demDirtyExts;
	//mpr_dispatcher->GetTerrainObjectsAndDirtyExts(onterrainobjs,rterrainobjs,mterrainobjs,
	//											objDirtyExts,domDirtyExts,mtobjDirtyExts,demDirtyExts);

	// 8. ����������dom��Ⱦ�ĵ������򣬴����Ͽ�	
	if (domDirtyExts.size()>0 || objDirtyExts.size()>0)
	{		
		// �����λ��ƶ�������[����Ⱦ˳��]
		//SortRObjsByRenderOrder(onterrainobjs);		
		// ��������ˢ��
		if(true)
		{//����ʱ��.
			preTime = osg::Timer::instance()->tick();
		}
		mpr_terrain->RefreshTexture(objDirtyExts,
			domDirtyExts,
			domLayers,
			onterrainobjs);
		if(false)
		{//����ʱ��
			osg::Timer_t currTime = osg::Timer::instance()->tick();
			double frameTime = osg::Timer::instance()->delta_m(preTime,currTime);
			//GlbLogWOutput(GLB_LOGTYPE_INFO,L"***** refresh ted objs %ld using time %lf\n",onterrainobjs.size(),frameTime);
			//char buff[128];
			//sprintf(buff,"***** refresh ted objs %lf using time",frameTime);
			//LOG(buff);
		}
	}


	// 9. ���µ���Dem��Ⱦ���򣬴����Ͽ�
	if (demDirtyExts.size()>0 ||mtobjDirtyExts.size()>0)
	{		
		// �����޸Ķ������� ������Ⱦ˳��
		//SortRObjsByRenderOrder(mterrainobjs);

		// ���θ߳�ˢ��
		mpr_terrain->RefreshDem(mtobjDirtyExts,
			demDirtyExts,
			demLayers,
			mterrainobjs);
	}	


}

void CGlbGlobe::Dispatch()
{	
	osg::Timer_t preTime = 0;
	if(true)
	{//����ʱ��.
		preTime = osg::Timer::instance()->tick();
	}
	// 1. �����ٽ������� �ɼ�����e1��e2
	CGlbExtent e1,e2;
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		for (size_t i = 0; i < mpr_visibleExt.size(); i++)
		{
			if (i==0)
				e1 = mpr_visibleExt.at(i);
			else
				e2 = mpr_visibleExt.at(i);
		}
	}	
	// 2. �ӳ�����������ȡ�ɼ����󼯺�	
	std::vector<glbref_ptr<CGlbGlobeObject>> visible_objects;
	if (mpr_sceneobjIdxManager)
	{
		osg::Camera* p_osgcamera = mpr_globeview->GetOsgCamera();
		osg::Matrixd _modelView = p_osgcamera->getViewMatrix();
		osg::Matrixd _projection = p_osgcamera->getProjectionMatrix();
		osg::Polytope cv;
		cv.setToUnitFrustum();
		cv.transformProvidingInverse((_modelView)*(_projection));	
		bool res=false;
		// �ȼ����ػ��ƵĶ���
		if (e1.IsValid())
			res = mpr_sceneobjIdxManager->QueryOnTerrainObjects(&e1,visible_objects);
		if (e2.IsValid())
			res = mpr_sceneobjIdxManager->QueryOnTerrainObjects(&e2,visible_objects);
		//OutputDebugString(L"query begin.\n");
		// ��Ӿ��Ժ���Ե��θߵĶ���
		res = mpr_sceneobjIdxManager->Query(cv,visible_objects,true);		
		//OutputDebugString(L"query end.\n");
	}	
	// end ���ݿɼ�����....	
	/*
	3. ������update��visible_objects,visible_onterrain_objects��
	*/
	mpr_dispatcher->Update(visible_objects);
	if(true)
	{//����ʱ��
		osg::Timer_t currTime = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_m(preTime,currTime);
		//GlbLogWOutput(GLB_LOGTYPE_INFO,L"**** dispatch obj %ld using time %lf\n",visible_objects.size(),frameTime);
	}
}

glbBool CGlbGlobe::GetVisibleLayers(std::vector<glbref_ptr<CGlbGlobeDomLayer>>& domlayers,
	std::vector<glbref_ptr<CGlbGlobeDemLayer>>& demlayers)
{
	bool bsucc = false;
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	for(size_t	k = 0; k < mpr_layers.size(); k++)
	{
		CGlbGlobeLayer* p_layer = mpr_layers.at(k).get();
		CGlbGlobeDomLayer* p_domLayer = dynamic_cast<CGlbGlobeDomLayer*>(p_layer);
		CGlbGlobeDemLayer* p_demLayer = dynamic_cast<CGlbGlobeDemLayer*>(p_layer);
		if ( (p_domLayer && p_domLayer->IsShow()) ||  (p_demLayer && p_demLayer->IsShow()) )
		{					
			osg::Vec3d eye,center,up;
			osg::Camera* p_osgcamera = mpr_globeview->GetOsgCamera();
			p_osgcamera->getViewMatrixAsLookAt(eye,center,up);								
			double x,y,z;
			glbref_ptr<CGlbExtent> p_extent = p_layer->GetBound(true);
			if (p_extent)
				p_extent->GetCenter(&x,&y,&z);			

			osg::Vec3d layer_center(x,y,z);
			//if (mpr_type==GLB_GLOBETYPE_GLOBE)//����ģʽ�洢 ��γ����λ���ȣ��� ����
			//	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,
			//				layer_center.x(),layer_center.y(),layer_center.z());

			double dis = (eye - layer_center).length();						
			double defaultvisibledistance = p_layer->GetDefaultVisibleDistance();
			if (dis <= defaultvisibledistance)// �������λ���ж��Ƿ���Ҫ��ʾ��
			{
				if (p_domLayer)
				{	// ������Ⱦ˳���������
					glbInt32 renderOrder = p_domLayer->GetRenderOrder();
					glbInt32 index_pos=-1;
					for(size_t k = 0; k < domlayers.size(); k++)
					{
						glbInt32 ro = domlayers.at(k)->GetRenderOrder();
						if (ro>renderOrder){
							index_pos = k;
							break;
						}
					}
					if (index_pos==-1)
						domlayers.push_back(p_domLayer);
					else
						domlayers.insert(domlayers.begin()+index_pos,1,p_domLayer);
				}
				else
				{// ������Ⱦ˳���������
					glbInt32 renderOrder = p_demLayer->GetRenderOrder();
					glbInt32 index_pos=-1;
					for(size_t k = 0; k < domlayers.size(); k++)
					{
						glbInt32 ro = domlayers.at(k)->GetRenderOrder();
						if (ro>renderOrder){
							index_pos = k;
							break;
						}
					}
					if (index_pos==-1)
						demlayers.push_back(p_demLayer);
					else
						demlayers.insert(demlayers.begin()+index_pos,1,p_demLayer);
				}
				bsucc = true;

				p_layer->Show(true,true);
			}
			else
			{
				p_layer->Show(false,true);
			}
		}	
	}
	return bsucc;
}


void CGlbGlobe::EnableTerrainWireMode(glbBool wiremode)
{
	if (mpr_terrain)
		mpr_terrain->EnableWireMode(wiremode);
}

glbBool CGlbGlobe::IsTerrainWireMode()
{
	if (mpr_terrain)
		return mpr_terrain->IsWireMode();
	return false;
}


glbBool CGlbGlobe::SortRObjsByRenderOrder(std::vector<glbref_ptr<CGlbGlobeRObject>>& rObjs)
{
	if (rObjs.size()<=0) return true;

	quickSort(&rObjs,0,rObjs.size()-1);
	//std::map<glbInt32,std::vector<glbref_ptr<CGlbGlobeRObject>>> mapObjs;

	//std::vector<glbref_ptr<CGlbGlobeRObject>>::iterator itr = rObjs.begin();
	//while(itr != rObjs.end())
	//{
	//	glbInt32 renderOrder = (*itr)->GetRenderOrder();
	//	if (GLB_OBJECTTYPE_DIGHOLE==(*itr)->GetType())
	//	{// �ڶ�����Ҫ�����
	//		renderOrder=99999;
	//	}

	//	std::map<glbInt32,std::vector<glbref_ptr<CGlbGlobeRObject>>>::iterator mapItr = mapObjs.find(renderOrder);
	//	if (mapItr==mapObjs.end())
	//	{// û�д�renderorder
	//		std::vector<glbref_ptr<CGlbGlobeRObject>> robjs_order;
	//		robjs_order.push_back(*itr);

	//		mapObjs[renderOrder] = robjs_order;
	//	}
	//	else
	//	{// ���д�renderorder,���ݶ����id�������������λ�ã�id��Խ��Խ������	
	//		glbInt32 objid = (*itr)->GetId();
	//		std::vector<glbref_ptr<CGlbGlobeRObject>>::iterator itr_iner = mapItr->second.begin();
	//		while(itr_iner != mapItr->second.end())
	//		{
	//			glbInt32 id = (*itr_iner)->GetId();
	//			if (objid<id)
	//			{
	//				itr_iner = mapItr->second.insert(itr_iner,*itr);
	//				break;
	//			}
	//			++itr_iner;
	//		}
	//		if (itr_iner==mapItr->second.end())
	//		{
	//			mapItr->second.push_back(*itr);
	//		}			
	//	}
	//	++itr;
	//}

	//rObjs.clear();

	//std::map<glbInt32,std::vector<glbref_ptr<CGlbGlobeRObject>>>::iterator mapItr = mapObjs.begin();
	//while(mapItr!=mapObjs.end())
	//{
	//	std::vector<glbref_ptr<CGlbGlobeRObject>> objs = mapItr->second;
	//	for(size_t i = 0 ;i < objs.size(); i++)
	//	{
	//		rObjs.push_back(objs.at(i));
	//	}
	//	++mapItr;
	//}

	return true;
}

CGlbGlobeSettings *CGlbGlobe::GetGlobeSettings()
{
	return mpr_settings.get();
}

void CGlbGlobe::quickSort(std::vector<glbref_ptr<CGlbGlobeRObject>>* objs, glbInt32 l, glbInt32 r)
{
	if (l < r)
	{      
		glbInt32 i = l, j = r;
		glbref_ptr<CGlbGlobeRObject> x = (*objs)[l];
		CGlbGlobeRObject* xObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[l].get());

		while (i < j)
		{		
			CGlbGlobeRObject* jObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[j].get());

			while((i < j) 
				&& (compareTo(xObj,jObj ) == 1))//����x���ȼ�<j ��x���� <j��x���ȼ�=j��xӦ������j��ǰ�桿
			{				
				j--; 
				jObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[j].get());
			}

			if(i < j)
				(*objs)[i++] = (*objs)[j];

			CGlbGlobeRObject* iObj = dynamic_cast<CGlbGlobeRObject*> ((*objs)[i].get());

			while((i < j) &&  (compareTo(iObj, xObj)==1))//����x���ȼ�<j ��x���� <j��x���ȼ�=j��xӦ������j��ǰ�桿
			{				
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
*   1: A ���ȼ� < B �� A.id < B.id,��A���ȼ�=B
*   -1:A ���ȼ� > B �� A.id > B.id,��A���ȼ�=B
*   0 ��A���ȼ� = B �� A ���� = B
*/
glbInt8 CGlbGlobe::compareTo(CGlbGlobeRObject *objA, CGlbGlobeRObject *objB)
{
	if (objA->GetRenderOrder() < objB->GetRenderOrder()) return 1;
	else if (objA->GetRenderOrder() > objB->GetRenderOrder()) return -1;
	else if (objA->GetId() < objB->GetId()) return 1;
	else if (objA->GetId() > objB->GetId()) return -1;
	else return 0;
}

#include <osgDB/WriteFile>
bool CGlbGlobe::Export3dsFile(CGlbGlobeObject* obj, CGlbWString filePath)
{
	if (obj == NULL || filePath.size()<=0)
		return false;

	CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(obj);
	if (robj == NULL)
		return false;
	osg::ref_ptr<osg::Node> node = robj->GetOsgNode();
	if (node==NULL) return false;

	CGlbString _str = filePath.ToString();
	return osgDB::writeNodeFile(*node.get(),_str);
}

glbBool CGlbGlobe::OpenGlobeSaveFile( const glbWChar* prjFile )
{
	xmlNodePtr curNode;      //������ָ��(����Ҫ��Ϊ���ڸ��������ƶ�) 
	xmlChar *szKey=NULL;     //��ʱ�ַ�������
	CGlbGlobeMemCtrl::GetInstance()->ClearUsedMem();
	//�����ļ�
	CGlbWString wPrjFile(prjFile);
	CGlbString szDocName = wPrjFile.ToString();
	xmlDocPtr doc = xmlReadFile(szDocName.c_str(),"GB2312",XML_PARSE_RECOVER);
	mpr_root->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//�������ĵ��Ƿ�ɹ���������ɹ���libxml��ָһ��ע��Ĵ���ֹͣ��
	//һ�����������ǲ��ʵ��ı��롣XML��׼�ĵ�������UTF-8��UTF-16�⻹�����������뱣�档
	//����ĵ���������libxml���Զ���Ϊ��ת����UTF-8���������XML������Ϣ������XML��׼��.
	if (NULL == doc)
	{  	
		CGlbWString err = L"�򿪹����ļ�:";
		err += wPrjFile;
		err += L" ʧ��";
		GlbSetLastError(err.c_str());
		return false;
	} 

	curNode = xmlDocGetRootElement(doc); //ȷ���ĵ���Ԫ��
	/*���ȷ�ϵ�ǰ�ĵ��а�������*/
	if (NULL == curNode)
	{
		//fprintf(stderr,"empty document"n");
		xmlFreeDoc(doc);
		return false;
	} 

	curNode = curNode->xmlChildrenNode;
	//ȡ���ڵ��е�����
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"ProjName")))
	{
		szKey = xmlNodeGetContent(curNode);		
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		mpr_name = glbstr.ToWString();		
		xmlFree(szKey);

		if(str)free(str);
		curNode = curNode->next;
	}	
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"PrjPassword")))
	{
		szKey = xmlNodeGetContent(curNode);		
		char* str = u2g((char*)szKey);
		CGlbString glbstr = (char*)str;
		mpr_prjPassword = glbstr.ToWString();
		xmlFree(szKey);
		if(str)free(str);
		curNode = curNode->next;
	}	
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"GlobeType")))
	{
		szKey = xmlNodeGetContent(curNode);	
		sscanf((char*)szKey,"%d",&mpr_type);		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"GroupIdBase")))
	{
		szKey = xmlNodeGetContent(curNode);	
		sscanf((char*)szKey,"%d",&mpr_groupid_base);		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"ObjectIdBase")))
	{
		szKey = xmlNodeGetContent(curNode);	
		sscanf((char*)szKey,"%d",&mpr_objectid_base);		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"LayerIdBase")))
	{
		szKey = xmlNodeGetContent(curNode);	
		sscanf((char*)szKey,"%d",&mpr_layerid_base);		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"UndergroundMode")))
	{
		szKey = xmlNodeGetContent(curNode);	
		sscanf((char*)szKey,"%d",&mpr_undergroundmode);		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"UndergroundAltitude")))
	{
		szKey = xmlNodeGetContent(curNode);	
		sscanf((char*)szKey,"%lf",&mpr_undergroundaltitude);		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"SpatialReference")))
	{
		szKey = xmlNodeGetContent(curNode);	
		CGlbString glbstr = (char*)szKey;
		mpr_spatialreference = glbstr.ToWString();	
		xmlFree(szKey);
		curNode = curNode->next;
	}
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"DefaultLocationObjectID")))
	{
		szKey = xmlNodeGetContent(curNode);	
		sscanf((char*)szKey,"%d",&mpr_defaultLocationID);		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"GroundObjOpacity")))
	{
		szKey = xmlNodeGetContent(curNode);	
		sscanf((char*)szKey,"%d",&mpr_groundobjsOpactity);		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"UndergroundObjOpacity")))
	{
		szKey = xmlNodeGetContent(curNode);	
		sscanf((char*)szKey,"%d",&mpr_undergroundobjsOpactity);		
		xmlFree(szKey);
		curNode = curNode->next;
	}

	// ��
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"Group")))
	{
		xmlNodePtr groupNode = curNode->xmlChildrenNode;
		int groupCnt=0;
		if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"GroupNum")))
		{
			szKey = xmlNodeGetContent(groupNode);	
			sscanf((char*)szKey,"%d",&groupCnt);		
			xmlFree(szKey);
			groupNode = groupNode->next;
		}
		for (int i = 0; i < groupCnt; i++)
		{
			GlbGroup grp;
			if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"GroupName")))
			{
				szKey = xmlNodeGetContent(groupNode);
				char* str = u2g((char*)szKey);
				CGlbString glbstr = str;				
				grp.name = glbstr.ToWString();
				xmlFree(szKey);
				if (glbstr.length()>0)
					free(str);
				groupNode = groupNode->next;
			}
			if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"GroupId")))
			{
				szKey = xmlNodeGetContent(groupNode);
				sscanf((char*)szKey,"%d",&grp.id);	
				xmlFree(szKey);
				groupNode = groupNode->next;
			}
			if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"GroupVisible")))
			{
				szKey = xmlNodeGetContent(groupNode);
				sscanf((char*)szKey,"%c",&grp.visible);	
				xmlFree(szKey);
				groupNode = groupNode->next;
			}
			if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"GroupParentId")))
			{
				szKey = xmlNodeGetContent(groupNode);
				sscanf((char*)szKey,"%d",&grp.parentId);
				xmlFree(szKey);
				groupNode = groupNode->next;
			}			

			mpr_groups.push_back(grp);
		}
		curNode = curNode->next;
	}

	//���·��
	CGlbWString rpath;
	/*if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"RelativePath")))
	{
	szKey = xmlNodeGetContent(curNode);
	char* str = u2g((char*)szKey);
	CGlbString glbstr = str;
	rpath = glbstr.ToWString();	
	xmlFree(szKey);
	free(str);
	curNode = curNode->next;
	}*/
	rpath = CGlbPath::GetDir(wPrjFile);

	// ����Դ
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"DataSources")))
	{
		CGlbDataSourceFactory* factory = CGlbDataSourceFactory::GetInstance();
		glbInt32 count =  factory->GetCount();
		for(glbInt32 i = 0; i < count; i++)
		{
			IGlbDataSource* ds = factory->GetDataSource(0);
			factory->ReleaseDataSource(ds);
		}
		factory->Load(curNode,rpath.c_str(),mpr_prjPassword.c_str());
		curNode = curNode->next;
	}

	//����ͼ��
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"Terrain")))
	{
		if (!mpr_terrain)
			mpr_terrain = new CGlbGlobeTerrain(this);
		xmlNodePtr terrain_node = curNode->xmlChildrenNode;
		mpr_terrain->Load(terrain_node,(glbWChar*)rpath.c_str());
		curNode = curNode->next;
	}

	////ͼ��
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"Layers")))
	{
		xmlNodePtr layerNode = curNode->xmlChildrenNode;
		int LayerCnt=0;
		if (layerNode && (!xmlStrcmp(layerNode->name, (const xmlChar *)"LayerNum")))
		{
			szKey = xmlNodeGetContent(layerNode);	
			sscanf((char*)szKey,"%d",&LayerCnt);		
			xmlFree(szKey);
			layerNode = layerNode->next;
		}
		for( int i = 0; i < LayerCnt; i++)
		{
			if (layerNode && (!xmlStrcmp(layerNode->name, (const xmlChar *)"Layer")))
			{
				xmlNodePtr node = layerNode->xmlChildrenNode;

				GlbGlobeLayerTypeEnum lyrType;
				if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LayerType")))
				{
					szKey = xmlNodeGetContent(node);	
					sscanf((char*)szKey,"%d",&lyrType);		
					xmlFree(szKey);
					CGlbGlobeLayer* layer = NULL;
					switch(lyrType)
					{
					case GLB_GLOBELAYER_DOM:
						layer = new CGlbGlobeDomLayer();
						break;
					case GLB_GLOBELAYER_DEM:
						layer = new CGlbGlobeDemLayer();
						break;
					case GLB_GLOBELAYER_FEATURE:
						layer = new CGlbGlobeFeatureLayer();
						break;
					case GLB_GLOBELAYER_TILTPHOTO:
						{
							node = node->next;
							if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TPFPath")))
							{
								// TPF�ļ�·��
								szKey = xmlNodeGetContent(node);
								char* str = u2g((char*)szKey);
								CGlbString glbstr = str;				
								CGlbWString FTPFilePath = glbstr.ToWString();
								xmlFree(szKey);
								free(str);
								layer = new CGlbGlobeTiltPhotographyLayer(FTPFilePath);
							}						
						}						
						break;
					}
					if (layer)
					{
						node = node->next;
						layer->Load(node,(glbWChar*)rpath.c_str());					
						layer->SetGlobe(this);// ����globe����
						layer->SetParentNode(mpr_root);
						osg::Node* p_layernode = layer->GetOsgNode();
						if (layer->GetType()==GLB_GLOBELAYER_TILTPHOTO)
						{// ��бͼ��ӵ�������
							mpr_terrain->AddTiltPhotographLayer(layer);
							if (p_layernode)
								mpr_terrain->GetNode()->asGroup()->addChild(p_layernode);
						}
						else
						{							
							if (p_layernode)
								mpr_root->addChild(p_layernode);// ���layer�ڵ�											
						}
						mpr_layers.push_back(layer);
					}	
				}
			}//������һ��ͼ��
			layerNode = layerNode->next;
		}//layercnt	
		curNode = curNode->next;
	}	

	//����
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"Objects")))
	{
		xmlNodePtr objNode = curNode->xmlChildrenNode;
		int objCnt=0;
		if (objNode && (!xmlStrcmp(objNode->name, (const xmlChar *)"ObjectNum")))
		{
			szKey = xmlNodeGetContent(objNode);	
			sscanf((char*)szKey,"%d",&objCnt);		
			xmlFree(szKey);
			objNode = objNode->next;
		}
		for( int i = 0; i < objCnt; i++)
		{			 
			if (objNode && (!xmlStrcmp(objNode->name, (const xmlChar *)"Object")))
			{
				xmlNodePtr node = objNode->xmlChildrenNode;

				GlbGlobeObjectTypeEnum objtype;
				CGlbWString objclsname = L"";
				if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ObjectType")))
				{
					szKey = xmlNodeGetContent(node);
					sscanf((char*)szKey,"%d",&objtype);
					xmlFree(szKey);
					switch(objtype)
					{
					case GLB_OBJECTTYPE_POINT:
						objclsname = L"GlbGlobePoint";break;
					case GLB_OBJECTTYPE_LINE:
						objclsname = L"GlbGlobeLine";break;
					case GLB_OBJECTTYPE_POLYGON:
						objclsname = L"GlbGlobePolygon";break;
					case GLB_OBJECTTYPE_TIN:
						objclsname = L"GlbGlobeTin";break;
					case GLB_OBJECTTYPE_DYNAMIC:
						objclsname = L"GlbGlobeDynamic";break;
					case GLB_OBJECTTYPE_MODIFYTERRIN:
						objclsname = L"GlbGlobeModifyTerrain";break;
					case GLB_OBJECTTYPE_DIGHOLE:
						objclsname = L"GlbGlobeDigHole";break;
					case GLB_OBJECTTYPE_SECTION:
						objclsname = L"GlbGlobeSection";break;
					case GLB_OBJECTTYPE_DRILL:
						objclsname = L"GlbGlobeDrill";break;
					case GLB_OBJECTTYPE_VIDEO:
						objclsname = L"GlbGlobeVideo";break;
					case GLB_OBJECTTYPE_NETWORK:
						objclsname = L"GlbGlobeNetwork";break;
					case GLB_OBJECTTYPE_COORDINAT_AXIS:
						objclsname = L"GlbGlobeCoordinateAxis";break;
					default:
						break;
					}
				}

				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					node = node->next;
					obj->Load(&node,(glbWChar*)rpath.c_str());					
					AddObject(obj,false);				
				}
			}//������һ������
			objNode = objNode->next;
		}
		curNode = curNode->next;
	}

	xmlFreeDoc(doc);
	return true;
}

glbBool CGlbGlobe::OpenProjectSaveFile( const glbWChar* prjFile )
{
	xmlNodePtr curNode;      //������ָ��(����Ҫ��Ϊ���ڸ��������ƶ�) 
	xmlChar *szKey=NULL;     //��ʱ�ַ�������
	CGlbGlobeMemCtrl::GetInstance()->ClearUsedMem();
	//���·��
	CGlbWString rpath;
	//�����ļ�
	CGlbWString wPrjFile(prjFile);
	CGlbString szDocName = wPrjFile.ToString();
	xmlDocPtr doc = xmlReadFile(szDocName.c_str(),"UTF-8",XML_PARSE_RECOVER);
	mpr_root->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//�������ĵ��Ƿ�ɹ���������ɹ���libxml��ָһ��ע��Ĵ���ֹͣ��
	//һ�����������ǲ��ʵ��ı��롣XML��׼�ĵ�������UTF-8��UTF-16�⻹�����������뱣�档
	//����ĵ���������libxml���Զ���Ϊ��ת����UTF-8���������XML������Ϣ������XML��׼��.
	if (NULL == doc)
	{  	
		CGlbWString err = L"�򿪹����ļ�:";
		err += wPrjFile;
		err += L" ʧ��";
		GlbSetLastError(err.c_str());
		return false;
	} 

	curNode = xmlDocGetRootElement(doc); //ȷ���ĵ���Ԫ��
	/*���ȷ�ϵ�ǰ�ĵ��а�������*/
	if (NULL == curNode)
	{
		//fprintf(stderr,"empty document"n");
		xmlFreeDoc(doc);
		return false;
	} 

	if(curNode->xmlChildrenNode)
	{
		curNode = curNode->xmlChildrenNode;
	}

	//ȡ���ڵ��е�����
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"text")))
	{
		curNode = curNode->next;
	}
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"GetType")))
	{
		szKey = xmlNodeGetContent(curNode);		
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		if(glbstr == "NGLB_GLOBETYPE_GLOBE")
			mpr_type = GLB_GLOBETYPE_GLOBE;
		else if(glbstr == "NGLB_GLOBETYPE_FLAT")
			mpr_type = GLB_GLOBETYPE_FLAT;
		xmlFree(szKey);

		if(str)free(str);
		curNode = curNode->next;
	}

	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"text")))
	{
		curNode = curNode->next;
	}

	// ����Դ
	if (curNode && (!xmlStrcmp(curNode->name, (const xmlChar *)"GetTerrainDataset")))
	{
		//terrainName
		xmlNodePtr groupNode = NULL;
		if(curNode->xmlChildrenNode)
			groupNode = curNode->xmlChildrenNode;
		szKey = groupNode->next->xmlChildrenNode->content;
		char* str = u2g((char*)szKey);
		CGlbString terrainName = (char*)str;

		//terrainPath
		groupNode = groupNode->next;//Name
		groupNode = groupNode->next;//text
		groupNode = groupNode->next;//Alias
		groupNode = groupNode->next;//text
		groupNode = groupNode->next;//GetDataSource
		groupNode = groupNode->xmlChildrenNode;//text
		groupNode = groupNode->next;//"GetAlias"
		groupNode = groupNode->next;//text
		groupNode = groupNode->next;//Url
		szKey = groupNode->xmlChildrenNode->content;
		str = u2g((char*)szKey);
		CGlbString terrPath = (char*)str;
		CGlbWString terrainPath = terrPath.ToWString();

		glbInt32 index = terrainPath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			terrainPath = execDir + terrainPath.substr(1,terrainPath.size());
		}

		groupNode = groupNode->next;//text
		groupNode = groupNode->next;//User

		groupNode = groupNode->next;//text
		groupNode = groupNode->next;//Password

		groupNode = groupNode->next;//text
		groupNode = groupNode->next;//ProviderName
		szKey = groupNode->xmlChildrenNode->content;
		str = u2g((char*)szKey);
		CGlbString terrainType = (char*)str;

		CGlbDataSourceFactory* factory = CGlbDataSourceFactory::GetInstance();
		IGlbDataSource * ds = factory->OpenDataSource(terrainPath.c_str(),NULL,NULL,terrainType.ToWString().c_str());
		if(ds == NULL)
		{
			GlbLogWOutput(GlbLogTypeEnum::GLB_LOGTYPE_ERR,L"��������Դ��ʧ��,�����������Դ·��: %s �Ƿ���ȷ��",terrainPath.c_str());
			return false;
		}
		IGlbRasterDataSource *rds = NULL;
		ds->QueryInterface(L"IGlbRasterDataSource",(void**)&rds);
		IGlbTerrainDataset* tdset = rds->GetTerrainDataset(terrainName.ToWString().c_str());
		
		if (!mpr_terrain)
			mpr_terrain = new CGlbGlobeTerrain(this);	
		mpr_terrain->SetTerrainDataset(tdset);
	}

	curNode = curNode->next;//text
	curNode = curNode->next;//root

	{
		glbInt32 groupID = 0;
		glbInt32 parentID = -1;
		GlbGroup grp;
		xmlNodePtr groupNode = curNode->xmlChildrenNode;//text
		groupNode = groupNode->next;//Name����Ŀ
		if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"Name")))
		{
			szKey = xmlNodeGetContent(groupNode);
			char* str = u2g((char*)szKey);
			CGlbString glbstr = str;				
			grp.name = glbstr.ToWString();
			xmlFree(szKey);
			if (glbstr.length()>0)
				free(str);
		}
		groupNode = groupNode->next;//text
		groupNode = groupNode->next;//Checked
		if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"Checked")))
		{
			szKey = xmlNodeGetContent(groupNode);
			char* str = u2g((char*)szKey);
			CGlbString glbstr = str;
			if(glbstr == "false")
				grp.visible = false;
			else
				grp.visible = true;
			xmlFree(szKey);
		}
		grp.id = groupID;
		grp.parentId = parentID;
		mpr_groups.push_back(grp);
		groupNode = groupNode->next;//text
		groupNode = groupNode->next;//Nodes
		if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"Nodes")))
		{
			ParseProjectNodes(groupNode,groupID,groupID,rpath);
		}//Nodes
		//groupID++;
		//parentID++;
	}

	xmlFreeDoc(doc);
	return true;
}

glbBool CGlbGlobe::ParseProjectNodes( xmlNodePtr groupNode,glbInt32 &groupID,glbInt32 parentID,const CGlbWString rpath )
{
	groupNode = groupNode->xmlChildrenNode;//text
	if(groupNode == NULL)//��ʾû��nodes
		return true;
	xmlNodePtr treeViewNode = groupNode->next;//SerialTreeViewNode
	xmlNodePtr infoNode = NULL;
	xmlNodePtr childNode = NULL;
	xmlChar *szKey=NULL;     //��ʱ�ַ�������
	glbBool isLoaded = false;
	xmlNodePtr checkNode = NULL;
	glbBool isGroup = false;
	while(!xmlStrcmp(treeViewNode->name, (const xmlChar *)"SerialTreeViewNode"))
	{
		//��������
		GlbGroup grp;
		groupNode = treeViewNode->xmlChildrenNode;//text
		groupNode = groupNode->next;//Name

		//�ж��Ƿ�����ڵ�
		if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"Name")))
		{
			checkNode = groupNode->next;//text
			checkNode = checkNode->next;//Checked
		}
		if(checkNode && (!xmlStrcmp(checkNode->name, (const xmlChar *)"Checked")))
		{
			checkNode = checkNode->next;//text
			checkNode = checkNode->next;//Nodes
		}
		if(checkNode && (!xmlStrcmp(checkNode->name, (const xmlChar *)"Nodes")))
			isGroup = true;

		if(isGroup)
		{
			if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"Name")))
			{
				szKey = xmlNodeGetContent(groupNode);
				char* str = u2g((char*)szKey);
				CGlbString glbstr = str;				
				grp.name = glbstr.ToWString();
				xmlFree(szKey);
				if (glbstr.length()>0)
					free(str);
			}
		}
		groupNode = groupNode->next;//text
		groupNode = groupNode->next;//Checked
		if(isGroup)
		{
			if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"Checked")))
			{
				szKey = xmlNodeGetContent(groupNode);
				char* str = u2g((char*)szKey);
				CGlbString glbstr = str;
				if(glbstr == "false")
					grp.visible = false;
				else
					grp.visible = true;
				xmlFree(szKey);
			}
			grp.id = ++groupID;
			grp.parentId = parentID;
			mpr_groups.push_back(grp);
		}
		groupNode = groupNode->next;//text
		groupNode = groupNode->next;//SerialObject
		if(!xmlStrcmp(groupNode->name, (const xmlChar *)"Nodes"))
		{
			if(ParseProjectNodes(groupNode,groupID,grp.id,rpath) == false)
				break;
			else
			{
				treeViewNode = treeViewNode->next;//text
				treeViewNode = treeViewNode->next;//SerialTreeViewNode
				if(!treeViewNode)
					break;
				continue;
			}
		}

		if(!xmlStrcmp(groupNode->properties->children->content, (const xmlChar *)"SerialGlobeFeatureLayer"))
		{
			CGlbGlobeFeatureLayer *layer = new CGlbGlobeFeatureLayer();
			isLoaded = layer->Load2(groupNode->xmlChildrenNode,(glbWChar*)rpath.c_str());
			if(isLoaded == false)
			{
				treeViewNode = treeViewNode->next;//text
				treeViewNode = treeViewNode->next;//SerialTreeViewNode
				if(!treeViewNode)
					break;
				continue;
			}
			layer->SetGlobe(this);// ����globe����
			layer->SetParentNode(mpr_root);
			layer->SetGroupId(groupID);
			osg::Node* p_layernode = layer->GetOsgNode();
			p_layernode->setName("layerGroup");
			if (p_layernode)
				mpr_root->addChild(p_layernode);// ���layer�ڵ�											
			mpr_layers.push_back(layer);
		}
		else if(!xmlStrcmp(groupNode->properties->children->content, (const xmlChar *)"SerialGlobeDomLayer"))
		{
			CGlbGlobeDomLayer *layer = new CGlbGlobeDomLayer();
			isLoaded = layer->Load2(groupNode->xmlChildrenNode,(glbWChar*)rpath.c_str());
			if(isLoaded == false)
			{
				treeViewNode = treeViewNode->next;//text
				treeViewNode = treeViewNode->next;//SerialTreeViewNode
				if(!treeViewNode)
					break;
				continue;
			}
			layer->SetGlobe(this);// ����globe����
			layer->SetParentNode(mpr_root);
			layer->SetGroupId(groupID);
			osg::Node* p_layernode = layer->GetOsgNode();
			p_layernode->setName("layerGroup");
			if (p_layernode)
				mpr_root->addChild(p_layernode);// ���layer�ڵ�											
			mpr_layers.push_back(layer);
		}
		else if(!xmlStrcmp(groupNode->properties->children->content, (const xmlChar *)"SerialGlobeDemLayer"))
		{
			CGlbGlobeDemLayer *layer = new CGlbGlobeDemLayer();
			isLoaded = layer->Load2(groupNode->xmlChildrenNode,(glbWChar*)rpath.c_str());
			if(isLoaded == false)
			{
				treeViewNode = treeViewNode->next;//text
				treeViewNode = treeViewNode->next;//SerialTreeViewNode
				if(!treeViewNode)
					break;
				continue;
			}
			layer->SetGlobe(this);// ����globe����
			layer->SetParentNode(mpr_root);
			layer->SetGroupId(groupID);
			osg::Node* p_layernode = layer->GetOsgNode();
			p_layernode->setName("layerGroup");
			if (p_layernode)
				mpr_root->addChild(p_layernode);// ���layer�ڵ�											
			mpr_layers.push_back(layer);
		}
		else if(!xmlStrcmp(groupNode->properties->children->content, (const xmlChar *)"SerialGlobeDynamicObject"))
		{
			treeViewNode = treeViewNode->next;//text
			treeViewNode = treeViewNode->next;//SerialTreeViewNode
			if(!treeViewNode)
				break;
			continue;
		}
		else if(!xmlStrcmp(groupNode->properties->children->content, (const xmlChar *)"SerialTerrainModify"))
		{
			groupNode = groupNode->xmlChildrenNode;//text
			groupNode = groupNode->next;//GetActiveAction
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//Name
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetAltitudeMode
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetCurrLevel
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetEditMode
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetLoadOrder
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetMaxVisibleDistance
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetMessage
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetMinVisibleDistance
			groupNode = groupNode->next;//text

			CGlbWString objclsname = L"GlbGlobeModifyTerrain";
			CGlbGlobeObject* obj = NULL;
			obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
			if (obj)
			{
				obj->SetGroupId(groupID);
				obj->Load2(&groupNode,rpath.c_str());					
				AddObject(obj,true);				
			}
		}
		else
		{
			groupNode = groupNode->xmlChildrenNode;//text
			groupNode = groupNode->next;//GetActiveAction
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//Name
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetAltitudeMode
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetCurrLevel
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetEditMode
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetLoadOrder
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetMaxVisibleDistance
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetMessage
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetMinVisibleDistance
			groupNode = groupNode->next;//text
			groupNode = groupNode->next;//GetRenderInfo
			//����������Ϣ
			infoNode = groupNode->xmlChildrenNode;//text
			infoNode = infoNode->next;//type;

			szKey = infoNode->xmlChildrenNode->content;
			char* str = u2g((char*)szKey);
			CGlbString type = (char*)str;

			if(type == "NGLB_OBJECTTYPE_POINT")
			{
				CGlbWString objclsname = L"GlbGlobePoint";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_LINE")
			{
				CGlbWString objclsname = L"GlbGlobeLine";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_POLYGON")
			{
				CGlbWString objclsname = L"GlbGlobePolygon";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_TIN")
			{
				CGlbWString objclsname = L"GlbGlobeTin";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_DYNAMIC")
			{
				CGlbWString objclsname = L"GlbGlobeDynamic";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_MODIFYTERRIN")
			{
				CGlbWString objclsname = L"GlbGlobeModifyTerrain";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_DIGHOLE")
			{
				CGlbWString objclsname = L"GlbGlobeDigHole";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_VISUALLINEANALYSIS")
			{
				CGlbWString objclsname = L"GlbGlobeVisualLineAnalysis";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_VIEWANALYSIS")
			{
				CGlbWString objclsname = L"GlbGlobeViewAnalysis";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_VIDEO")
			{
				CGlbWString objclsname = L"GlbGlobeVideo";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_DRILL")
			{
				CGlbWString objclsname = L"GlbGlobeDrill";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_SECTION")
			{
				CGlbWString objclsname = L"GlbGlobeSection";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
			else if(type == "NGLB_OBJECTTYPE_NETWORK")
			{
				CGlbWString objclsname = L"GlbGlobeNetwork";
				CGlbGlobeObject* obj = NULL;
				obj = g_objectFactory.GetObject((glbWChar*)objclsname.c_str());	
				if (obj)
				{
					obj->SetGroupId(groupID);
					infoNode = infoNode->next;//text;
					obj->Load2(&infoNode,rpath.c_str());					
					AddObject(obj,true);				
				}
			}
		}//����
		treeViewNode = treeViewNode->next;//text
		treeViewNode = treeViewNode->next;//SerialTreeViewNode
		if(!treeViewNode)
			break;
	}//SerialTreeViewNode
	return true;
}

glbBool CGlbGlobe::SetCullisionMode( glbBool mode )
{
	mpr_cullisionMode = mode;
	return true;
}

glbBool CGlbGlobe::CheckCullisionModel() const
{
	return mpr_cullisionMode;
}