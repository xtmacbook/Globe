// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "cwMine_i.h"
#include "dllmain.h"
#include "compreg.h"
#include "dlldatax.h"

CcwMineModule _AtlModule;

class CcwMineApp : public CWinApp
{
public:

// 重写
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CcwMineApp, CWinApp)
END_MESSAGE_MAP()

CcwMineApp theApp;

BOOL CcwMineApp::InitInstance()
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
#endif
	return CWinApp::InitInstance();
}

int CcwMineApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
