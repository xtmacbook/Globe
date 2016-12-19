// Dlg_PriorProbability.cpp : 实现文件
//

#include "stdafx.h"
#include "Dlg_PriorProbability.h"

#include "UsefulUtility/DlgInput.h"
#include "UsefulUtility/DlgOutput.h"



// CDlg_PriorProbability 对话框

IMPLEMENT_DYNAMIC(CDlg_PriorProbability, CPropertyPage)

CDlg_PriorProbability::CDlg_PriorProbability()
	: CPropertyPage(CDlg_PriorProbability::IDD)
{

}

CDlg_PriorProbability::~CDlg_PriorProbability()
{
}

void CDlg_PriorProbability::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListBox1);
	DDX_Control(pDX, IDC_LIST3, m_ListBox2);
}


BEGIN_MESSAGE_MAP(CDlg_PriorProbability, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlg_PriorProbability::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlg_PriorProbability::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlg_PriorProbability::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlg_PriorProbability::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_LIULAN, &CDlg_PriorProbability::OnBnClickedLiulan)
END_MESSAGE_MAP()


// CDlg_PriorProbability 消息处理程序


//IMPLEMENT_DYNAMIC(Dlg_PriorProbability, CDlgMultiExt)
BOOL CDlg_PriorProbability::OnInitDialog() 
{

	CPropertyPage::OnInitDialog();

	IcwSceneViewPtr pSV;
	IcwFrameInfoPtr pFrameInfo = NULL;
	CoCreateInstance(__uuidof(cwFrameInfo),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwFrameInfo),(void **)&pFrameInfo);
	if (pFrameInfo)
	{
		IcwMainViewPtr pMV;
		pFrameInfo->get_ActiveView(&pMV);
		pSV=pMV;
	}
	m_pSceneView=pSV;

	//m_list=(CListCtrl*)GetDlgItem(IDC_LIST1);
	LONG lStyle;
	/*lStyle=GetWindowLong(m_list->m_hWnd,GWL_STYLE);
	lStyle &=~LVS_TYPEMASK;
	lStyle |=LVS_REPORT;*/
	/*SetWindowLong(m_list->m_hWnd,GWL_STYLE,lStyle);
	DWORD dwStyle=m_list->GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	dwStyle |= LVS_EX_CHECKBOXES;*/
	/*m_list->SetExtendedStyle(dwStyle);
	m_list->InsertColumn(0, _T("名称"));
	m_list->InsertColumn(1, _T("OID"));
	m_list->InsertColumn(2, _T("类型"));
	m_list->InsertColumn(3, _T("数据集"));*/
	/*for(int j=0;j<4;j++)
		m_list->SetColumnWidth(j ,70); */

	/*IcwModelManagerPtr pModelManager = NULL;
	CoCreateInstance(__uuidof(cwModelManager),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwModelManager),(void **)&pModelManager);
	IUnknownPtr pUnkVal;
	pModelManager->get_CurrentProperty(&pUnkVal);*/
	/*pFault=pUnkVal;
	if (pFault != NULL)
	{
		long objcnt;
		pFault->get_ObjCount(&objcnt);
		for (long i=0;i<objcnt;i++)
		{
			IcwGeoObjectPtr pGeoObj;
			pFault->GetObj(i,&pGeoObj);
			m_GeoObjects.push_back(pGeoObj);
		}
		RefreshControlList();
	}*/

	return CPropertyPage::OnInitDialog();

	return TRUE;  
}

//void CDlg_PriorProbability::OnInputDlgChanged(CDlgInput *m_pVoxet)
//{
//	long nObj=0;
//	if (m_pVoxet->m_pObjSet)
//	{
//		m_pVoxet->m_pObjSet->get_ItemCount(&nObj);
//	}
//	IcwObjectPtr pObj;	
//	m_pVoxet->m_pObjSet->GetItem(0,&pObj);
//	IcwVoxetPtr pVoxet = pObj;
//	IcwPropertyTablePtr pPropertyTable;
//	pVoxet->get_PropTable(&pPropertyTable);
//	ASSERT(pPropertyTable);
//
//	IcwFieldPtr pField;
//	IcwFieldsPtr pFields;
//	pPropertyTable->get_Fields(&pFields);
//	if (pFields == NULL)
//		return;
//	long nField;
//	IcwFieldPtr pOneField;
//	pFields->get_FieldCount(&nField);
//	CComBSTR FieldName;
//	for (long i = 0;i<nField;i++)
//	{
//		pFields->get_Field(i,&pOneField);
//		pOneField->get_Name(&FieldName);
//		//m_pDlgEvidenceMethod->m_ListBox1.SetItemData(i,i);
//		m_pDlgPriorProbability->m_ListBox1.AddString(FieldName);
//
//	}
//
//}



BOOL CDlg_PriorProbability::OnApply()
{
	/*if (pFault==NULL)
	{
		return FALSE;
	}*/
	if (controlmodify)
	{
		if (m_RemoveObjects.size()>0)
		{
			for (long k=0;k<long(m_RemoveObjects.size());k++)
			{
				IcwGeoObjectPtr pGeoObject;
				pGeoObject=m_RemoveObjects[k];
				//pFault->RemoveObj(pGeoObject);
			}
		}
		if (m_GeoObjects.size()>0)
		{
			for (long i=0;i<long(m_GeoObjects.size());i++)
			{
				IcwGeoObjectPtr pGeoObject;
				pGeoObject=m_GeoObjects[i];
				//pFault->AddObj(pGeoObject);
			}
		}
	}

	controlmodify=false;

	IcwWorkSpacePtr pWorkspace = NULL;
	CoCreateInstance(__uuidof(cwWorkSpace),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwWorkSpace),(void **)&pWorkspace);
	if (pWorkspace == NULL) return FALSE ;

	pWorkspace->UpdateAllViews();
	return CPropertyPage::OnApply();
}
BOOL CDlg_PriorProbability::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);  // 设置上一步按钮可用
	return CPropertyPage::OnSetActive();
}
BOOL CDlg_PriorProbability::OnWizardFinish()
{
	UpdateData(TRUE);

	// 获取向导
	CPropertySheet *Wizard = reinterpret_cast<CPropertySheet *>(GetParent());
	CDlg_PriorProbability *p0= (CDlg_PriorProbability *)Wizard->GetPage(0);
	/*IcwModelManagerPtr pModelManager = NULL;
	CoCreateInstance(__uuidof(cwModelManager),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwModelManager),(void **)&pModelManager);
	if (pModelManager == NULL) return FALSE;*/

	//刷新树
	IUnknownPtr punkVal;
	/*pModelManager->get_CurrentProperty(&punkVal);
	if (punkVal == NULL) return FALSE;*/






	IcwWorkSpacePtr pWorkspace = NULL;
	CoCreateInstance(__uuidof(cwWorkSpace),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwWorkSpace),(void **)&pWorkspace);
	if (pWorkspace == NULL) return FALSE;

	pWorkspace->UpdateAllViews();
	return CPropertyPage::OnWizardFinish();
}
//void CDlg_PriorProbability::OnOK()
//{
//}
void CDlg_PriorProbability::OnBnClickedButton1()
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
}

void CDlg_PriorProbability::OnBnClickedButton2()
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

void CDlg_PriorProbability::OnBnClickedButton3()
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

void CDlg_PriorProbability::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	int nItem = m_ListBox2.GetCount();
	for (int i = 0;i<nItem;i++)
	{
		m_ListBox2.DeleteString(0);
	}
}

void CDlg_PriorProbability::OnBnClickedLiulan()
{
	// TODO: 在此添加控件通知处理程序代码
	/*CFileDialog dlg;
	if(dlg.DoModal() == IDOK)
	{

	}	*/
}
