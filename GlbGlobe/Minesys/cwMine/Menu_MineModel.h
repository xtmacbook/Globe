// Menu_MineModel.h : CMenu_MineModel ������

#pragma once
#include "resource.h"       // ������

#include "cwMine_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
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
		m_strVecs.push_back(_T("{3D66EC1E-FBEC-46DF-BA67-D697239A2E8A}"));	//֤��Ȩ����
		m_strVecs.push_back(_T("{2DBA37FC-6ECD-4842-82A2-9137C99E61B6}"));//��Ϣ������
		m_strVecs.push_back(_T("{2D2824DF-0E04-46E1-9CCB-078528AB74AB}"));//����ͳ��

		
		
		//m_strVecs.push_back(_T("{36F1C1B5-1DA9-4C9F-A73A-FA0D56A7D916}"));	//�������
		//m_strVecs.push_back(_T("{22E27E50-0256-4266-9626-BB760B7FFE35}"));	//֤��Ȩֵ
		//m_strVecs.push_back(_T("{AB193386-17DF-4AC7-91B8-9958480AC5D3}"));	//�������
		
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
		CComBSTR strCaption("���߼�");
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
