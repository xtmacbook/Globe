#include "StdAfx.h"
#include "GlbGlobeElementFactory.h"
#include "GlbGlobeView.h"
using namespace GlbGlobe;

CGlbGlobeElementFactory g_elementInstance;

CGlbGlobeElementFactory::CGlbGlobeElementFactory(void)
{
	HMODULE handle = NULL;
	mpr_providers.clear();
	HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
	CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
#ifdef _DEBUG
	workdir += L"\\GlbGlobeElementd.dll";	
#else
	workdir += L"\\GlbGlobeElement.dll";	
#endif
	handle =::LoadLibrary(workdir.c_str());
}


CGlbGlobeElementFactory::~CGlbGlobeElementFactory(void)
{
	map<CGlbWString,IGlbGlobeElementProvider*>::iterator itr = mpr_providers.begin();
	map<CGlbWString,IGlbGlobeElementProvider*>::iterator itrEnd = mpr_providers.end();
	for(itr;itr!=itrEnd;itr++)
		delete itr->second;
}
CGlbGlobeElementFactory* CGlbGlobeElementFactory::GetInstance()
{
	return &g_elementInstance;
}
void CGlbGlobeElementFactory::RegisterProvider(IGlbGlobeElementProvider* provider)
{
	if(provider == NULL)return;
	CGlbWString className = provider->GetClassName();
	map<CGlbWString,IGlbGlobeElementProvider*>::iterator itr = mpr_providers.find( className);
	if(itr == mpr_providers.end())
	{
		mpr_providers.insert(std::pair<CGlbWString,IGlbGlobeElementProvider*>(className,provider));
	}
}
void CGlbGlobeElementFactory::UnRegisterProvider(const glbWChar* className)
{
	map<CGlbWString,IGlbGlobeElementProvider*>::iterator itr = mpr_providers.find(className);
	if(itr != mpr_providers.end())
	{
		delete itr->second;
		mpr_providers.erase(itr);
	}
}
IGlbGlobeElement* CGlbGlobeElementFactory::CreateElement(const glbWChar* className)
{
	if(className==NULL)return NULL;
	CGlbWString _name = className;
	map<CGlbWString,IGlbGlobeElementProvider*>::iterator itr = mpr_providers.find(_name);
	if(itr == mpr_providers.end())return NULL;
	return itr->second->Create();
}