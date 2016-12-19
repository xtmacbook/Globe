#pragma once
#include "IGlbDataEngine.h"
class  CGlbFileDSProvider : public IGlbDataSourceProvider
{
public:
	CGlbFileDSProvider(void);
	~CGlbFileDSProvider(void);

	const glbWChar*	GetName();
	IGlbDataSource*	CreateDataSource(const glbWChar* url, 
		                             const glbWChar* user,
		                             const glbWChar* password);
	IGlbDataSource*	OpenDataSource(  const glbWChar* url, 
		                             const glbWChar* user,
		                             const glbWChar* password);
	glbWChar*       GetLastError();

};
extern "C"__declspec(dllexport) IGlbDataSourceProvider* GetProvider();

