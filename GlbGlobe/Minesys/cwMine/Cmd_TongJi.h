// Cmd_TongJi.h : CCmd_TongJi 的声明

#pragma once
#include "resource.h"       // 主符号

#include "cwMine_i.h"
#include "Dlg_WizardSelect.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CCmd_TongJi

class ATL_NO_VTABLE CCmd_TongJi :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCmd_TongJi, &CLSID_Cmd_TongJi>,
	public IDispatchImpl<ICmd_TongJi, &IID_ICmd_TongJi, &LIBID_cwMineLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IcwCommand
{
public:
	CCmd_TongJi()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_CMD_TONGJI)


	BEGIN_COM_MAP(CCmd_TongJi)
		COM_INTERFACE_ENTRY(ICmd_TongJi)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IcwCommand)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_pDlg=NULL;
		return S_OK;
	}

	void FinalRelease()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		if (m_pDlg)
		{
			delete m_pDlg;
			m_pDlg=NULL;
		}
	}

public:

	CDlg_WizardSelect* m_pDlg;


	// IcwCommand Methods
public:
	STDMETHOD(get_Bitmap)(OLE_HANDLE * pVal)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_Caption)(BSTR * pVal)
	{
		*pVal = SysAllocString(_T("数据运算"));
		return S_OK;
	}
	STDMETHOD(get_Category)(BSTR * pVal)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_Checked)(VARIANT_BOOL * pVal)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_Enabled)(VARIANT_BOOL * pVal)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_Message)(BSTR * pVal)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_Tooltip)(BSTR * pVal)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(OnClick)()
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		IcwSceneViewPtr pSV;
		IcwOutputWindowPtr pOutput;
		IcwFrameInfoPtr pFrameInfo = NULL;
		CoCreateInstance(__uuidof(cwFrameInfo),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwFrameInfo),(void **)&pFrameInfo);
		if (pFrameInfo)
		{
			IcwMainViewPtr pMV;
			pFrameInfo->get_ActiveView(&pMV);
			pSV=pMV;
			pFrameInfo->get_OutputWindow(&pOutput);
		}
		if (pSV==NULL)
		{
			return E_FAIL;
		}
		if (m_pDlg)
		{
			delete m_pDlg;
		}
		//创建对话框
		m_pDlg = new CDlg_WizardSelect();
		m_pDlg->m_isDataStatistics = true;
		m_pDlg->m_pSceneView=pSV;
		//m_pDlg->m_pOutput=pOutput;
		m_pDlg->Create(CDlg_WizardSelect::IDD, NULL);
		m_pDlg->ShowWindow(SW_SHOW);

		return S_OK;
	}
	STDMETHOD(OnCreate)(LPUNKNOWN hook)
	{
		return E_NOTIMPL;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(Cmd_TongJi), CCmd_TongJi)
