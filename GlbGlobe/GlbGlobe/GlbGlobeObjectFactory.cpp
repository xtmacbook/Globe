#include "StdAfx.h"
#include "GlbGlobeObjectFactory.h"

IGlbGlobeObjectProvider::IGlbGlobeObjectProvider()
{
}

IGlbGlobeObjectProvider::~IGlbGlobeObjectProvider()
{
}

CGlbGlobeObjectFactory::CGlbGlobeObjectFactory(void)
{
	HMODULE handle = ::LoadLibraryW(L"GlbGlobeObject.dll");
}

CGlbGlobeObjectFactory::~CGlbGlobeObjectFactory(void)
{

}

//CGlbGlobeObjectFactory* g_objectFactory = new CGlbGlobeObjectFactory;
CGlbGlobeObjectFactory g_objectFactory;

CGlbGlobeObjectFactory* CGlbGlobeObjectFactory::GetInstance()
{
	/*
			LoadLibry("GlbGlobeObject.dll")
			HMODULE handle = ::LoadLibraryW(dllName);
	*/
	//HMODULE handle = ::LoadLibraryW(L"GlbGlobeObject.dll");
	return &g_objectFactory;
}

void CGlbGlobeObjectFactory::Register(IGlbGlobeObjectProvider* provider,glbWChar* objclassname)
{
	CGlbWString _ocn = objclassname;
	//std::map<std::wstring,IGlbGlobeObjectProvider*>::iterator itr = g_objectFactory.mpr_providers.find(_ocn);
	//if (itr == g_objectFactory.mpr_providers.end())
	//{
		g_objectFactory.mpr_providers[_ocn] = provider;
	//}	
}

void CGlbGlobeObjectFactory::UnRegister(glbWChar* objclassname)
{
	CGlbWString _ocn = objclassname;
	std::map<CGlbWString,IGlbGlobeObjectProvider*>::iterator itr = g_objectFactory.mpr_providers.find(_ocn);
	if (itr != g_objectFactory.mpr_providers.end())
	{
		delete itr->second;
		g_objectFactory.mpr_providers.erase(itr);
	}
}

CGlbGlobeObject* CGlbGlobeObjectFactory::GetObject(glbWChar* objclassname)
{
	CGlbWString _ocn = objclassname;
	std::map<CGlbWString,IGlbGlobeObjectProvider*>::iterator itr = g_objectFactory.mpr_providers.find(_ocn);
	if (itr != g_objectFactory.mpr_providers.end())
	{
		return itr->second->Create();
	}

	return NULL;
}

