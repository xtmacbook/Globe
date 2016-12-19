// Command_InformationMethod.h : CCommand_InformationMethod ������

#pragma once
#include "resource.h"       // ������

#include "cwMine_i.h"
#include "Dlg_WizardInformation.h"
#include "UsefulUtility/DlgMultiExt.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif



// CCommand_InformationMethod

class ATL_NO_VTABLE CCommand_InformationMethod :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCommand_InformationMethod, &CLSID_Command_InformationMethod>,
	public IDispatchImpl<ICommand_InformationMethod, &IID_ICommand_InformationMethod, &LIBID_cwMineLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IcwCommand
{
public:
	CCommand_InformationMethod()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_COMMAND_INFORMATIONMETHOD)


	BEGIN_COM_MAP(CCommand_InformationMethod)
		COM_INTERFACE_ENTRY(ICommand_InformationMethod)
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
	CDlg_WizardInformation* m_pDlg;

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
		CComBSTR str = _T("��Ϣ������");
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

		//CPropertySheet Wizard(_T("�½�ģ��"));
		//// ����ҳ1
		//CDlg_PriorProbability m_DlgPriorProbability;
		////m_DlgPriorProbability.isNew = true;
		//// ����ҳ2
		//CDlg_EvidenceMethod m_DlgEvidenceMethod;
		////m_DlgEvidenceMethod.isNew = true;
		//// �������ҳ
		//Wizard.AddPage(&m_DlgPriorProbability);
		//Wizard.AddPage(&m_DlgEvidenceMethod);
		//// ��ģʽ
		//Wizard.SetWizardMode();
		//Wizard.DoModal();


		//��ʼ�����ͼָ����������
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
		//�����Ի���
		m_pDlg = new CDlg_WizardInformation;
		m_pDlg->m_pSceneView=pSV;
		//m_pDlg->m_pOutput=pOutput;
		m_pDlg->Create(CDlg_WizardInformation::IDD, NULL);
		m_pDlg->ShowWindow(SW_SHOW);

		return S_OK;
	}
	STDMETHOD(OnCreate)(LPUNKNOWN hook)
	{
		return E_NOTIMPL;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(Command_InformationMethod), CCommand_InformationMethod)
