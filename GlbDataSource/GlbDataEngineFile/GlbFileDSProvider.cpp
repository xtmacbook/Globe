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
		GlbSetLastError(L"url目录是NULL");
		return NULL;
	}
	if(!CGlbPath::CreateDir(url))
	{
		GlbSetLastError(L"创建目录失败");
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
		GlbSetLastError(L"url目录是NULL");
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