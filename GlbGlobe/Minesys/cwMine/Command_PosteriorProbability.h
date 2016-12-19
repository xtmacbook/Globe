// Command_PosteriorProbability.h : CCommand_PosteriorProbability ������

#pragma once
#include "resource.h"       // ������

#include "cwMine_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif



// CCommand_PosteriorProbability

class ATL_NO_VTABLE CCommand_PosteriorProbability :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCommand_PosteriorProbability, &CLSID_Command_PosteriorProbability>,
	public IDispatchImpl<ICommand_PosteriorProbability, &IID_ICommand_PosteriorProbability, &LIBID_cwMineLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IcwCommand
{
public:
	CCommand_PosteriorProbability()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_COMMAND_POSTERIORPROBABILITY)


	BEGIN_COM_MAP(CCommand_PosteriorProbability)
		COM_INTERFACE_ENTRY(ICommand_PosteriorProbability)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IcwCommand)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:


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
		CComBSTR str = _T("�������");
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
		return E_NOTIMPL;
	}
	STDMETHOD(OnCreate)(LPUNKNOWN hook)
	{
		return E_NOTIMPL;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(Command_PosteriorProbability), CCommand_PosteriorProbability)
