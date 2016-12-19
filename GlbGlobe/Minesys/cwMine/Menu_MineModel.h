// Menu_MineModel.h : CMenu_MineModel 的声明

#pragma once
#include "resource.h"       // 主符号

#include "cwMine_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

#include <vector>

// CMenu_MineModel

class ATL_NO_VTABLE CMenu_MineModel :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMenu_MineModel, &CLSID_Menu_MineModel>,
	public IDispatchImpl<IMenu_MineModel, &IID_IMenu_MineModel, &LIBID_cwMineLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IcwMenuDef
{
public:
	CMenu_MineModel()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_MENU_MINEMODEL)


	BEGIN_COM_MAP(CMenu_MineModel)
		COM_INTERFACE_ENTRY(IMenu_MineModel)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IcwMenuDef)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_strVecs.push_back(_T("{3D66EC1E-FBEC-46DF-BA67-D697239A2E8A}"));	//证据权方法
		m_strVecs.push_back(_T("{2DBA37FC-6ECD-4842-82A2-9137C99E61B6}"));//信息量方法
		m_strVecs.push_back(_T("{2D2824DF-0E04-46E1-9CCB-078528AB74AB}"));//数据统计

		
		
		//m_strVecs.push_back(_T("{36F1C1B5-1DA9-4C9F-A73A-FA0D56A7D916}"));	//先验概率
		//m_strVecs.push_back(_T("{22E27E50-0256-4266-9626-BB760B7FFE35}"));	//证据权值
		//m_strVecs.push_back(_T("{AB193386-17DF-4AC7-91B8-9958480AC5D3}"));	//后验概率
		
		return S_OK;
	}

	void FinalRelease()
	{
		m_strVecs.clear();
	}

private:
	std::vector<CComBSTR> m_strVecs;

public:


	// IcwMenuDef Methods
public:
	STDMETHOD(get_Caption)(BSTR * pVal)
	{
		CComBSTR strCaption("工具集");
		strCaption.CopyTo(pVal);
		return S_OK;
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
		/*if (m_strVecs[index] ==_T("{36F1C1B5-1DA9-4C9F-A73A-FA0D56A7D916}"))			
		{
			itemDef->put_Group(VARIANT_TRUE);
		}
		else
		{
			itemDef->put_Group(VARIANT_FALSE);
		}*/	

		return S_OK;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(Menu_MineModel), CMenu_MineModel)
