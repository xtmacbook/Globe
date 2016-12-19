#include "StdAfx.h"
#include "GlbGlobeInstanceManager.h"
using namespace GlbGlobe;

CGlbGlobeInstance::CGlbGlobeInstance()
{
	InitializeCriticalSection(&mpr_critical);
}
CGlbGlobeInstance::~CGlbGlobeInstance()
{
	DeleteCriticalSection(&mpr_critical);
}

glbBool CGlbGlobeInstance::addInstance(glbWChar* key,osg::Node *node)
{
	if(key==NULL||node==NULL)return false;
	EnterCriticalSection(&mpr_critical);
	std::map<CGlbWString,osg::ref_ptr<osg::Node>>::iterator itr = mpr_instances.find(key);
	if(itr == mpr_instances.end())
	{
		//mpr_instances.insert(pair<wstring,GlbGlobeInstance>(key,its));
		CGlbWString wstrkey(key);
		mpr_instances[wstrkey] = node;
	}
	//else{
	//	itr->second.refcount++;
	//}
	LeaveCriticalSection(&mpr_critical);

	return true;
}
osg::Node *CGlbGlobeInstance::findInstance(glbWChar* key)
{
	if(key==NULL)return NULL;
	osg::Node* node = NULL;
	EnterCriticalSection(&mpr_critical);
	CGlbWString wstrkey(key);
	std::map<CGlbWString,osg::ref_ptr<osg::Node>>::iterator itr = mpr_instances.find(wstrkey);
	if(itr !=mpr_instances.end())
	{
		node = itr->second.get();
	}
	LeaveCriticalSection(&mpr_critical);	
	return node;
}
void CGlbGlobeInstance::clearInstance()
{
	mpr_instances.clear();
}

/////////////////////////////////////////////////////////////////////////////

//CGlbGlobeInstance* g_instance = new CGlbGlobeInstance();
CGlbGlobeInstance/** */g_instance;// = new CGlbGlobeInstance(); //modified by longtingyou

CGlbGlobeInstanceManager::CGlbGlobeInstanceManager(void)
{	
}

CGlbGlobeInstanceManager::~CGlbGlobeInstanceManager(void)
{	
}

glbBool CGlbGlobeInstanceManager::AddInstance(glbWChar* key,osg::Node *node)
{
	return g_instance.addInstance(key,node);
}

osg::Node* CGlbGlobeInstanceManager::FindInstance(glbWChar* key)
{
	return g_instance.findInstance(key);
}

void CGlbGlobeInstanceManager::ClearInstance()
{
	g_instance.clearInstance();
}