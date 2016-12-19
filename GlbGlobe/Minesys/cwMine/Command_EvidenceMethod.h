// Command_EvidenceMethod.h : CCommand_EvidenceMethod 的声明

#pragma once
#include "resource.h"       // 主符号

#include "cwMine_i.h"
#include "Dlg_PriorProbability.h"
#include "Dlg_EvidenceMethod.h"
#include "UsefulUtility/DlgMultiExt.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif



// CCommand_EvidenceMethod

class ATL_NO_VTABLE CCommand_EvidenceMethod :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCommand_EvidenceMethod, &CLSID_Command_EvidenceMethod>,
	public IDispatchImpl<ICommand_EvidenceMethod, &IID_ICommand_EvidenceMethod, &LIBID_cwMineLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IcwCommand
{
public:
	CCommand_EvidenceMethod()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_COMMAND_EVIDENCEMETHOD)


	BEGIN_COM_MAP(CCommand_EvidenceMethod)
		COM_INTERFACE_ENTRY(ICommand_EvidenceMethod)
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

	CDlg_EvidenceMethod* m_pDlg;
	// IcwCommand Methods
public:
	STDMETHOD(get_Bitmap)(OLE_HANDLE * pVal)
	{
		if(pVal == NULL)
		{
			return E_POINTER;
		}
		*pVal = NULL;
		return S_OK;
	}
	STDMETHOD(get_Caption)(BSTR * pVal)
	{
		CComBSTR str = _T("证据权值");
		str.CopyTo(pVal);
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

		//初始化活动视图指针和输出窗口
		//IcwSceneViewPtr pSV;
		//IcwOutputWindowPtr pOutput;
		//IcwFrameInfoPtr pFrameInfo = NULL;
		//CoCreateInstance(__uuidof(cwFrameInfo),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwFrameInfo),(void **)&pFrameInfo);
		//if (pFrameInfo)
		//{
		//	IcwMainViewPtr pMV;
		//	pFrameInfo->get_ActiveView(&pMV);
		//	pSV=pMV;
		//	pFrameInfo->get_OutputWindow(&pOutput);
		//}
		//if (pSV==NULL)
		//{
		//	return E_FAIL;
		//}

		//if (m_pDlg)
		//{
		//	delete m_pDlg;
		//}

		////创建对话框
		//m_pDlg = new Dlg_EvidenceMethod;
		//m_pDlg->m_pSceneView=pSV;
		//m_pDlg->m_pOutput=pOutput;
		//m_pDlg->Create(Dlg_EvidenceMethod::IDD, NULL);
		//m_pDlg->ShowWindow(SW_SHOW);

		// 向导框架
		CPropertySheet Wizard(_T("新建模型"));

		// 属性页1
		CDlg_PriorProbability* m_DlgPriorProbability;
		//m_DlgSetFaultFeatures.isNew = true;

		// 属性页2
		CDlg_EvidenceMethod *m_DlgEvidenceMethod;
		//m_DlgSetFaultControls.isNew = true;

		// 添加属性页
		Wizard.AddPage(m_DlgPriorProbability);
		Wizard.AddPage(m_DlgEvidenceMethod);

		// 向导模式
		Wizard.SetWizardMode();
		Wizard.DoModal();


		return S_OK;
	}
	STDMETHOD(OnCreate)(LPUNKNOWN hook)
	{
		return E_NOTIMPL;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(Command_EvidenceMethod), CCommand_EvidenceMethod)
