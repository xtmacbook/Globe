// Menu_DataStatistics.h : CMenu_DataStatistics 的声明

#pragma once
#include "resource.h"       // 主符号
#include <vector>



#include "cwMine_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CMenu_DataStatistics

class ATL_NO_VTABLE CMenu_DataStatistics :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMenu_DataStatistics, &CLSID_Menu_DataStatistics>,
	public IDispatchImpl<IMenu_DataStatistics, &IID_IMenu_DataStatistics, &LIBID_cwMineLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IcwMenuDef,
	public IcwCommand
{
public:
	CMenu_DataStatistics()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_MENU_DATASTATISTICS)


	BEGIN_COM_MAP(CMenu_DataStatistics)
		COM_INTERFACE_ENTRY(IMenu_DataStatistics)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IcwMenuDef)
		COM_INTERFACE_ENTRY(IcwCommand)
		COM_INTERFACE_ENTRY(IcwCommand)
		COM_INTERFACE_ENTRY(IcwCommand)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		//m_strVecs.push_back(_T("{2D2824DF-0E04-46E1-9CCB-078528AB74AB}"));
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	std::vector<CComBSTR> m_strVecs;



	// IcwMenuDef Methods
public:
	STDMETHOD(get_Caption)(BSTR * pVal)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_ItemCount)(long * pVal)
	{
		*pVal = m_strVecs.size();
		return S_OK;
	}
	STDMETHOD(GetItem)(long index, IcwToolbarItemDef * itemDef)
	{
		size_t nCnt = m_strVecs.size();
		if (index < 0 || index > nCnt)
		{
			return  E_FAIL;
		}
		itemDef->put_Clsid(m_strVecs[index]);
		return S_OK;
	}

	// IcwCommand Methods
public:
	STDMETHOD(get_Bitmap)(OLE_HANDLE * pVal)
	{
		return E_NOTIMPL;
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

	// IcwCommand Methods
public:

	// IcwCommand Methods
public:
};

OBJECT_ENTRY_AUTO(__uuidof(Menu_DataStatistics), CMenu_DataStatistics)
