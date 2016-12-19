// Dlg_EvidenceMethod.cpp : 实现文件
//

#include "stdafx.h"
#include "Dlg_EvidenceMethod.h"
#include "UsefulUtility/DlgInput.h"
#include "UsefulUtility/DlgOutput.h"

// CDlg_EvidenceMethod 对话框

IMPLEMENT_DYNAMIC(CDlg_EvidenceMethod, CDialog)

CDlg_EvidenceMethod::CDlg_EvidenceMethod(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_EvidenceMethod::IDD, pParent)
{

}

CDlg_EvidenceMethod::~CDlg_EvidenceMethod()
{
}

void CDlg_EvidenceMethod::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListBox1);
	DDX_Control(pDX, IDC_LIST3, m_ListBox2);
}


BEGIN_MESSAGE_MAP(CDlg_EvidenceMethod, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlg_EvidenceMethod::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlg_EvidenceMethod::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlg_EvidenceMethod::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlg_EvidenceMethod::OnBnClickedButton3)
END_MESSAGE_MAP()


// CDlg_EvidenceMethod 消息处理程序



IMPLEMENT_DYNAMIC(Dlg_EvidenceMethod, CDlgMultiExt)
BOOL Dlg_EvidenceMethod::OnInitDialog() 
{
	////设置标题
	//this->SetWindowText(_T("证据权值"));

	////定制对话框
	//{
	//	m_pDlgInput=new CDlgInput;
	//	m_pDlgInput->m_pSceneView=m_pSceneView;
	//	m_pDlgInput->m_ObjType=cwObjTypeVoxet;
	//	m_pDlgInput->m_Title = _T("输入Voxet：");
	//	m_pDlgInput->m_bSingle=true;
	//	//AddExtDialog(m_pDlgInput,CDlgInput::IDD);
	//}
	//{
	//	m_pDlgEvidenceMethod=new CDlg_EvidenceMethod;
	//	AddExtDialog(m_pDlgEvidenceMethod,CDlg_EvidenceMethod::IDD);
	//}

	//CDlgMultiExt::OnInitDialog();

	return TRUE;  
}


void Dlg_EvidenceMethod::OnOK()
{

}

void Dlg_EvidenceMethod::OnInputDlgChanged(CDlgInput *m_pVoxet)
{
	long nObj=0;
	if (m_pVoxet->m_pObjSet)
	{
		m_pVoxet->m_pObjSet->get_ItemCount(&nObj);
	}
	IcwObjectPtr pObj;	
	m_pVoxet->m_pObjSet->GetItem(0,&pObj);
	IcwVoxetPtr pVoxet = pObj;
	IcwPropertyTablePtr pPropertyTable;
	pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);

	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);
	if (pFields == NULL)
		return;
	long nField;
	IcwFieldPtr pOneField;
	pFields->get_FieldCount(&nField);
	CComBSTR FieldName;
	for (long i = 0;i<nField;i++)
	{
		pFields->get_Field(i,&pOneField);
		pOneField->get_Name(&FieldName);
		//m_pDlgEvidenceMethod->m_ListBox1.SetItemData(i,i);
		m_pDlgEvidenceMethod->m_ListBox1.AddString(FieldName);
		
	}
	






	/*cwObjType type;
	pObj->get_Type(&type);
	if (type ==cwObjTypeVoxet)
	{
		return;
	}
	IcwGeoObjectPtr pGeoObj=pObj;	
	IcwGeometryPtr pGeo;
	pGeoObj->get_Geometry(&pGeo);
	IcwFBodyPtr pFBody = pGeo;
	IcwPropertyPtr pProperty;
	pGeoObj->QueryInterface(IID_IcwProperty,(void**)&pProperty);*/
	//IcwFieldsPtr pFields;
	//pProperty->get_Fields(&pFields);
	/*IcwFieldPtr pField;
	pFields->get_Field(0,&pField);

	CComBSTR propertyname;
	pField->get_Name(&propertyname);	
	m_pCutVoxetWithBodys->m_propertyname = propertyname;*/

}

void CDlg_EvidenceMethod::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码	
	int nItem = m_ListBox1.GetSelCount();
	int *SelIndex = new int[nItem];
	m_ListBox1.GetSelItems(nItem, SelIndex); 
	for (int i = 0;i<nItem;i++)
	{
		CString str;		
		m_ListBox1.GetText(SelIndex[i], str);
		m_ListBox2.AddString(str);
	}
	m_ListBox1.RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

void CDlg_EvidenceMethod::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码	
	int nItem = m_ListBox1.GetCount();
	for (int i = 0;i<nItem;i++)
	{
		CString str;
		m_ListBox1.GetText(i,str);
		m_ListBox2.AddString(str);
	}
}

void CDlg_EvidenceMethod::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码

	//全删
	int nItem = m_ListBox2.GetCount();
	for (int i = 0;i<nItem;i++)
	{
		m_ListBox2.DeleteString(0);
	}

}

void CDlg_EvidenceMethod::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码

	//单删
	int nItem = m_ListBox2.GetSelCount();
	int *SelIndex = new int[nItem];
	m_ListBox2.GetSelItems(nItem, SelIndex); 
	for (int i = 0;i<nItem;i++)
	{		
		m_ListBox2.DeleteString(SelIndex[i]-i);
	}
}
