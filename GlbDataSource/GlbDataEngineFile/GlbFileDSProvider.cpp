#pragma warning(disable:4251)
#include "StdAfx.h"
#include "GlbFileDSProvider.h"
#include "GlbFileDataSource.h"
#include "GlbPath.h"

IGlbDataSourceProvider* GetProvider()
{
	return new CGlbFileDSProvider(); 
}

CGlbFileDSProvider::CGlbFileDSProvider(void)
{
}


CGlbFileDSProvider::~CGlbFileDSProvider(void)
{
}

const glbWChar*	CGlbFileDSProvider::GetName()
{
	return L"file";
}

IGlbDataSource*	CGlbFileDSProvider::CreateDataSource(const glbWChar* url, 
		                             const glbWChar* user,
		                             const glbWChar* password)
{
	if(NULL == url)
	{
		GlbSetLastError(L"urlĿ¼��NULL");
		return NULL;
	}
	if(!CGlbPath::CreateDir(url))
	{
		GlbSetLastError(L"����Ŀ¼ʧ��");
		return NULL;
	}

	CGlbFileDataSource* fileRDS = new CGlbFileDataSource();
	if (!fileRDS->Initialize(url))
	{
		delete fileRDS;
		return NULL;
	}
	return dynamic_cast<IGlbDataSource*>(fileRDS);
}

IGlbDataSource*	CGlbFileDSProvider::OpenDataSource(  const glbWChar* url, 
	                             const glbWChar* user,
	                             const glbWChar* password)
{
	if(url == NULL)
	{
		GlbSetLastError(L"urlĿ¼��NULL");
		return NULL;
	}
	CGlbFileDataSource* fileRDS = new CGlbFileDataSource();
	if (!fileRDS->Initialize(url))
	{
		delete fileRDS;
		return NULL;
	}
	return dynamic_cast<IGlbDataSource*>(fileRDS);
}
glbWChar* CGlbFileDSProvider::GetLastError()
{
	return GlbGetLastError();
}