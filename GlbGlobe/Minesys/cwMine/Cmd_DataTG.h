// Cmd_DataTG.h : CCmd_DataTG ������

#pragma once
#include "resource.h"       // ������



#include "cwMine_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif

using namespace ATL;


// CCmd_DataTG

class ATL_NO_VTABLE CCmd_DataTG :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCmd_DataTG, &CLSID_Cmd_DataTG>,
	public IDispatchImpl<ICmd_DataTG, &IID_ICmd_DataTG, &LIBID_cwMineLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CCmd_DataTG()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CMD_DATATG)


BEGIN_COM_MAP(CCmd_DataTG)
	COM_INTERFACE_ENTRY(ICmd_DataTG)
	COM_INTERFACE_ENTRY(IDispatch)
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



};

OBJECT_ENTRY_AUTO(__uuidof(Cmd_DataTG), CCmd_DataTG)
