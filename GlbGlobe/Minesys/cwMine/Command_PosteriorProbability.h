// Command_PosteriorProbability.h : CCommand_PosteriorProbability 的声明

#pragma once
#include "resource.h"       // 主符号

#include "cwMine_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
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
		CComBSTR str = _T("后验概率");
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
