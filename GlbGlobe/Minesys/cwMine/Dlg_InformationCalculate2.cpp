// Dlg_EvidenceMethod.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Dlg_InformationCalculate2.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "Dlg_IndependenceTest.h"
#include "ExcelMgr.h"
//#include "excel9.h"
//#include "dlgtest.h"

// CDlg_InformationCalculate2 �Ի���

IMPLEMENT_DYNAMIC(CDlg_InformationCalculate2, CPropertyPage)

CDlg_InformationCalculate2::CDlg_InformationCalculate2()
	: CPropertyPage(CDlg_InformationCalculate2::IDD)
	, m_InformationLevel(_T(""))
{
}

CDlg_InformationCalculate2::~CDlg_InformationCalculate2()
{
}

void CDlg_InformationCalculate2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrlEnvidecce);
	DDX_CBString(pDX, IDC_COMBO_INFORMATION_LEVEL, m_InformationLevel);
	DDX_Control(pDX, IDC_COMBO_INFORMATION_LEVEL, m_ComboBoxInformationLevel);
}


BEGIN_MESSAGE_MAP(CDlg_InformationCalculate2, CPropertyPage)
	ON_BN_CLICKED(IDC_ENVIDENCE, &CDlg_InformationCalculate2::OnBnClickedEnvidence)
	ON_BN_CLICKED(IDC_ENVIDENCEOUT, &CDlg_InformationCalculate2::OnBnClickedEnvidenceout)
	ON_BN_CLICKED(IDC_POSTERIORPROBABILITY, &CDlg_InformationCalculate2::OnBnClickedPosteriorprobability)
	ON_BN_CLICKED(IDC_DELETEENVDENCE, &CDlg_InformationCalculate2::OnBnClickedDeleteenvdence)
	//ON_BN_CLICKED(IDC_DELETEENVDENCE2, &CDlg_InformationCalculate2::OnBnClickedDeleteenvdence2)
	ON_BN_CLICKED(IDC_INDEPENDENCETEST, &CDlg_InformationCalculate2::OnBnClickedIndependencetest)
	ON_BN_CLICKED(IDC_ASCERTAINFACTOR, &CDlg_InformationCalculate2::OnBnClickedAscertainfactor)
	ON_BN_CLICKED(IDC_SORTBYINFORMATION, &CDlg_InformationCalculate2::OnBnClickedSortbyinformation)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &CDlg_InformationCalculate2::OnLvnColumnclickList1)
	ON_CBN_SELCHANGE(IDC_COMBO_INFORMATION_LEVEL, &CDlg_InformationCalculate2::OnCbnSelchangeComboInformationLevel)
	ON_BN_CLICKED(IDC_INFORMATION_INPUTVOXET, &CDlg_InformationCalculate2::OnBnClickedInformationInputvoxet)
END_MESSAGE_MAP()

// CDlg_InformationCalculate2 ��Ϣ�������

//////////////////////////////////////////////////////////////////////////
DWORD  dwSelColID =0; //ѡ�����
BOOL   bASC = FALSE;     //�Ƿ�����
bool findfactor = false;


BOOL CDlg_InformationCalculate2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// ��ȡ��
	CPropertySheet *Wizard = reinterpret_cast<CPropertySheet *>(GetParent());
	pDlgPriorProbability= (CDlg_InformationCalculate *)Wizard->GetPage(0);

	//long nField;
 	int nItem = pDlgPriorProbability->m_ListBox2.GetCount();//����
	int nColumnCount = pDlgPriorProbability->m_ListCtrl.GetHeaderCtrl()->GetItemCount();//����
	//int ncolumn = pDlgPriorProbability->m_ListCtrl.GetItemCount();
	/*while (nItem>=0)
	{
		CString str;
		pDlgPriorProbability->m_ListBox2.GetText(--nItem,str);
		m_ListCtrlEnvidecce.InsertItem(LVIF_TEXT|LVIF_STATE,0, str,0,LVIS_SELECTED,0,0);
	}*/
 	for (int i = 0;i<nItem;i++)
 	{	
		CString str;
		pDlgPriorProbability->m_ListBox2.GetText(i,str);
		//m_ListCtrlEnvidecce.SetItemData()
		m_ListCtrlEnvidecce.InsertItem(LVIF_TEXT|LVIF_STATE,i, str,0,LVIS_SELECTED,0,0);		
 	}

	int i = m_ListCtrlEnvidecce.GetItemCount();   
	for (int k=0;k<i;k++)   
	{   
		m_ListCtrlEnvidecce.SetItemData(k,k);   
	} 

	

	//��ʼ��listctrl����
	m_ListCtrlEnvidecce.InsertColumn( 0, _T("��������"), LVCFMT_LEFT, 90 );
	m_ListCtrlEnvidecce.InsertColumn( 1, _T("������������"), LVCFMT_LEFT, 90 );
	m_ListCtrlEnvidecce.InsertColumn( 2, _T("����������"), LVCFMT_LEFT, 90 );
	m_ListCtrlEnvidecce.InsertColumn( 3, _T("��������"), LVCFMT_LEFT, 90 );
	m_ListCtrlEnvidecce.InsertColumn( 4, _T("�о�������"), LVCFMT_LEFT, 90 );
	m_ListCtrlEnvidecce.InsertColumn( 5, _T("��Ϣ��ֵ"), LVCFMT_LEFT, 90 );


	//��ʾÿ��������Ϣ��ֵ
	//SetItemText(�кţ��кţ���Ϣ��ֵ)
	CString strListData;
	for (int i=0;i<nItem;i++)
	{
		for(int j = 0;j<nColumnCount;j++)
		{
			//pDlgPriorProbability->m_ListCtrl.GetSubItemRect(i,0,LVIR_LABEL,rc);//��������RECT��
			strListData=pDlgPriorProbability->m_ListCtrl.GetItemText(i,j);
			m_ListCtrlEnvidecce.SetItemText(i,j,(LPCTSTR)strListData);
			//strvec.push_back(strListData.AllocSysString());	
		}
	}
	//for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	//{
	//	m_ListCtrlEnvidecce.GetItemText()
	//	m_ListCtrlEnvidecce.SetItemText(SingleItem , 1 , (LPCTSTR)strNj);
	//	m_ListCtrlEnvidecce.SetItemText(SingleItem , 2 ,(LPCTSTR)strSj);
	//	m_ListCtrlEnvidecce.SetItemText(SingleItem , 3 , (LPCTSTR)strN);
	//	m_ListCtrlEnvidecce.SetItemText(SingleItem , 4 , (LPCTSTR)strS);
	//	m_ListCtrlEnvidecce.SetItemText(SingleItem , 5 , (LPCTSTR)strInformation);

	//}


	//int nItem = m_CLC.GetItemCount();
	//for (int i = 0;i<nItem;i++)
	//{
	//	CString str;
	//	m_CLC.GetItemText(i,str);
	//	//envidenceName.push_back(str);
	//	m_ListCtrlEnvidecce.InsertItem(LVIF_TEXT|LVIF_STATE,0, str,0,LVIS_SELECTED,0,0);		
	//}
	//pvoxet = pDlgPriorProbability->m_pVoxet;

	return true;
}


BOOL CDlg_InformationCalculate2::OnSetActive()
{	
	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);  // ������һ����ť����
	return CPropertyPage::OnSetActive();
}

BOOL CDlg_InformationCalculate2::OnApply()
{
	/*IcwModelManagerPtr pModelManager = NULL;
	CoCreateInstance(__uuidof(cwModelManager),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwModelManager),(void **)&pModelManager);
	IUnknownPtr pUnkVal;
	pModelManager->get_CurrentProperty(&pUnkVal);
	IcwFaultPtr pFault = NULL;
	pFault=pUnkVal;
	if (pFault==NULL)
	{
		return FALSE;
	}*/
	
	IcwWorkSpacePtr pWorkspace = NULL;
	CoCreateInstance(__uuidof(cwWorkSpace),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwWorkSpace),(void **)&pWorkspace);
	if (pWorkspace == NULL) return FALSE ;

	pWorkspace->UpdateAllViews();
	return CPropertyPage::OnApply();
}

LRESULT CDlg_InformationCalculate2::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class
	/*GetDlgItemText(IDC_EDIT1,m_FaultName);
	if (m_FaultName==_T(""))
	{
		AfxMessageBox(_T("������ϲ�����!"));
		return -1;
	}*/
	return CPropertyPage::OnWizardNext();
}



////������Ȩֵ
//float EnvidenceMethodWPlus(long nGrid,long nMine,long nEnvidence,long nMineCrossEnvidence)
//{
//	float W;
//	float a = (float)nMineCrossEnvidence/(float)nMine;
//	float b = (float)(nEnvidence - nMineCrossEnvidence)/(float)(nGrid - nMine);
//	float n = (float)a/(float)b;
//	W = log((long double)n);
//	return W;
//}
////���㸺Ȩֵ
//float EnvidenceMethodWMinus(long nGrid,long nMine,long nEnvidence,long nMineCrossEnvidence)
//{
//	float W;
//	float a = (float)(nMine-nMineCrossEnvidence)/(float)nMine;
//	float b = (float)(nGrid - nEnvidence - nMine + nMineCrossEnvidence)/(float)(nGrid - nMine);
//	float n = (float)a/(float)b;
//	W = log((long double)n);
//	return W;
//}

void CDlg_InformationCalculate2::OnBnClickedEnvidence()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���������֤��Ȩֵ
	//�о�����������
	long nu,nv,nw;
	pDlgPriorProbability->m_pVoxet->get_nU(&nu);
	pDlgPriorProbability->m_pVoxet->get_nV(&nv);
	pDlgPriorProbability->m_pVoxet->get_nW(&nw);	
// 	long nGridCount = pDlgPriorProbability->ResearchAreaTrueIndex.size();

	//�����������
// 	long nMineCount = pDlgPriorProbability->MineTrueIndex.size();
	//�������������
// 	long nNoneMineCount = nGridCount - nMineCount;

	//����������������������֤��Ȩֵ
	int nItem = pDlgPriorProbability->m_ListBox2.GetCount();
	std::vector<float> C;
	float Cplus = 0;
	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{
// 		long nEnvidenceCount = pDlgPriorProbability->EnvidenceTrueIndex[SingleItem].size();
// 		long nMineCrossEnvidenceCount = pDlgPriorProbability->ptT_T[SingleItem];

		//��ÿһ�����Ӽ�������Ȩֵ
//		float wplus = EnvidenceMethodWPlus(nGridCount,nMineCount,nEnvidenceCount,nMineCrossEnvidenceCount);
//		float wminus = EnvidenceMethodWMinus(nGridCount,nMineCount,nEnvidenceCount,nMineCrossEnvidenceCount);
//		float Counterpoise = wplus - wminus;
//		C.push_back(Counterpoise);
//		Cplus+=Counterpoise;
//
//		//��ʾ
//		char* buffer5 = new char[30];
//		char* buffer6 = new char[30];
//		char* buffer7 = new char[30];
//		//char* buffer4 = new char[30];
//		//float PTtruetrue = 333.333f;
//		sprintf(buffer5,"%8.6f",wplus);
//		sprintf(buffer6,"%8.6f",wminus);
//		sprintf(buffer7,"%8.6f",Counterpoise);
//	
//		CString tostrTT1 = buffer5;
//		CString tostrTF2 = buffer6;
//		CString tostrFT3 = buffer7;
//		
//		m_ListCtrlEnvidecce.SetItemText(SingleItem , 1 , (LPCTSTR)tostrTT1);
//		m_ListCtrlEnvidecce.SetItemText(SingleItem , 2 ,(LPCTSTR)tostrTF2);
//		m_ListCtrlEnvidecce.SetItemText(SingleItem , 3 , (LPCTSTR)tostrFT3);
//		
//	}
//
//	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
//	{
//		float C_normalization = C[SingleItem]/Cplus;
//		char* buffer8 = new char[30];
//		sprintf(buffer8,"%8.6f",C_normalization);
//		CString tostrFF4 = buffer8;
//		m_ListCtrlEnvidecce.SetItemText(SingleItem ,4 , (LPCTSTR)tostrFF4);
//	}
//}
	}
}
//void ExportListToExcel(CListCtrl *pList, CString sExcelFile, CString sSheetName)
//{
//	_Worksheet m_wsExcelSingle;
//	//Worksheets m_wsExcels;
//	_Workbook m_wbExcelSingle;
//	Workbooks m_wbExcels;
//	_Application m_appExcel;
//	Range m_rangeExcel;
//
//	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
//	if (!m_appExcel.CreateDispatch("Excel.Application",NULL)) 
//	{
//		MessageBox("����Excel����ʧ��!","��ʾ",MB_OK|MB_ICONWARNING);
//		return;
//	} 
//
//	m_appExcel.SetVisible(false); 
//	m_wbExcels.AttachDispatch(m_appExcel.GetWorkbooks(),true);
//	m_wbExcelSingle.AttachDispatch(m_wbExcels.Add(covOptional));
//	//�õ�Worksheets 
//	m_wsExcelSingle.AttachDispatch(m_wbExcelSingle.GetWorksheets(),true);
//	//ɾ������ı�
//	//m_wsExcelSingle.AttachDispatch(m_wsExcels.GetItem(COleVariant((short)3)));
//	//m_wsExcelSingle.Delete();
//	//m_wsExcelSingle.AttachDispatch(m_wsExcels.GetItem(COleVariant((short)2)));
//	//m_wsExcelSingle.Delete();
//	//�����
//	//m_wsExcelSingle.AttachDispatch(m_wsExcels.GetItem(COleVariant((short)1)));
//	m_wsExcelSingle.SetName(sSheetName);
//	m_rangeExcel.AttachDispatch(m_wsExcelSingle.GetUsedRange(), TRUE);
//
//	if (pList->GetItemCount ()>0)   
//	{ 
//		int nItemCount,nColumnCount;
//		nColumnCount = pList->GetHeaderCtrl()->GetItemCount();
//		nItemCount = pList->GetItemCount();
//		int i;
//		LVCOLUMN columnData;
//		CString columnName;
//		int columnNum = 0;
//		CString strH;
//		CString strV;
//
//		columnData.mask = LVCF_TEXT;
//		columnData.cchTextMax =100;
//		columnData.pszText = columnName.GetBuffer (100);
//		//д�б�ͷ
//		for(i=0;pList->GetColumn(i,&columnData);i++)
//		{
//			m_rangeExcel.SetItem( _variant_t( (long)(1) ), _variant_t( (long)(i+1) ),COleVariant(columnData.pszText) );
//		}
//		columnName.ReleaseBuffer ();
//
//		// ��Excel��д����
//		for (i = 0; i < nItemCount; i++ )             
//		{
//			for ( int j = 0; j < nColumnCount; j++ )
//			{
//				m_rangeExcel.SetItem( _variant_t( (long)(i+2) ), 
//					_variant_t( (long)(j+1) ),
//					COleVariant(pList->GetItemText(i,j)) );
//			}
//		}
//
//		//�˷������ڲ�ͬ��Excel�汾�����������ܲ��ܣ�����ο�MSDN
//		m_wbExcelSingle.SaveAs( COleVariant( sExcelFile ),
//			_variant_t(vtMissing),
//			_variant_t(vtMissing),
//			_variant_t(vtMissing),
//			_variant_t(vtMissing),
//			_variant_t(vtMissing),
//			0,
//			_variant_t(vtMissing),
//			_variant_t(vtMissing),
//			_variant_t(vtMissing),
//			_variant_t(vtMissing));
//
//	}
//	else
//	{
//		//û������
//		MessageBox("û�����ݣ����ܵ���!","��ʾ",MB_OK|MB_ICONWARNING|MB_TOPMOST);
//
//	}
//
//
//	m_wbExcelSingle.Close( covOptional, COleVariant( sExcelFile ), covOptional );
//	m_wbExcels.Close();
//	m_rangeExcel.ReleaseDispatch(); 
//	m_wsExcelSingle.ReleaseDispatch(); 
//	//m_wsExcels.ReleaseDispatch(); 
//	m_wbExcelSingle.ReleaseDispatch(); 
//	m_wbExcels.ReleaseDispatch(); 
//	m_appExcel.ReleaseDispatch(); 
//	m_appExcel.Quit();
//}

void CDlg_InformationCalculate2::OnBnClickedEnvidenceout()
{
	// ���֤��Ȩֵ��
	//TCHAR szFilters[]= _T("txt Files (*.txt)|*.txt");
	//CFileDialog fileDlg(FALSE, _T("txt"), _T("*txt"),OFN_FILEMUSTEXIST |OFN_HIDEREADONLY, szFilters);

	//if (fileDlg.DoModal()==IDOK)
	//{
	//	//�����ļ�
	//	CString pathname = fileDlg.GetPathName();
	//	CString filename = fileDlg.GetFileName();
	//	//pathname.GetBuffer()
	//	/*char szName;
	//	strcpy( szName, ( const char*)filename);*/
	//	//LPCTSTR lpszText = (LPCTSTR)filename;		
	//	//strcpy(ww,filename); 
	//	//const char* ww =filename.GetBuffer(sizeof(filename));
	//	//std::ofstream outfile(pathname,std::ios::in);
	//	//CFile file(pathname,CFile::modeCreate | CFile::modeWrite);
	//	/*char *p; 
	//	CString str="hello"; 
	//	p=str.GetBuffer(str.GetLength()); 
	//	str.ReleaseBuffer(); */
	//	/*CString str="hello"; 
	//	char ch[20]; 
	//	memcpy(ch,str,str.GetLength()); */

	//	char ch[20];
	//	std::ofstream ofs;
	//	ofs.open(pathname);
	//	for(long nitemindex=0;nitemindex<m_ListCtrlEnvidecce.GetItemCount();nitemindex++)
	//	{
	//		for (int j = 0;j<5;j++)
	//		{
	//			CString str = m_ListCtrlEnvidecce.GetItemText(nitemindex,j);
	//			memset(ch,0,20);
	//			memcpy(ch,str,str.GetLength()); 
	//			//ch = str.GetBuffer(str.GetLength());
	//			//ofs<<(LPCTSTR)sss;
	//			ofs.write(ch,strlen(ch));
	//			//const char* aaa =(const char*)sss.GetBuffer();
	//			//ofs.write(aaa,sss.GetLength());
	//			//file.Write(sss,sss.GetLength());				
	//			//ofs<<"  ";
	//		}
	//		ofs<<std::endl;
	//	}
	//	//ofs.close();
	//	ofs.close();
	//}

	//////////////////////////////////////////////////////////////////////////
	
	//��ѡ��·��
	CString FilePathName;//�ļ�����������
	CFileDialog Dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("excel Files(*.xls)|*.xls"));
	//���ļ�
	if(Dlg.DoModal() == IDOK)//�Ƿ�򿪳ɹ�
	{
		FilePathName =  Dlg.GetPathName();//ȡ���ļ�·�����ļ���
		//SetDlgItemText(IDC_OutPutFile,FilePathName);//��һ����ַΪIDC_OutPutFile��editbox����ʾ�ļ���
	}
	else//��ʧ�ܴ���
	{
		//��ʧ�ܴ���
		MessageBox(_T("��ʧ��"),NULL,MB_OK);
	}
	//CFileDialog filedlg(FALSE, _T("xls"), _T("*.xls"), OFN_ALLOWMULTISELECT);
	//filedlg.DoModal();

	OnOK();
	//////////////////////////////////////////////////////////////////////////
	// ������дexcel [10/8/2009 mubin]

	std::map<const wchar_t*,CString> ColumnMap;
	ColumnMap[_T("֤������")] = 'A';
	ColumnMap[_T("��ȨֵW+")] = 'B';
	ColumnMap[_T("��ȨֵW-")] = 'C';
	ColumnMap[_T("�ۺ�Ȩ��C")] = 'D';
	ColumnMap[_T("���Ȩ��C")] = 'E';

	ExcelMgr excel; 
	excel.CreateExcel(FilePathName);	
	excel.SetCurSheet(1);
	////	GridToExcel(excel);
	excel.SetSheetName(_T("������֤��Ȩֵ"));

	int startRow = 1;// ָ��ʾ�������� [10/8/2009 mubin]
	////strCaptial= (char)(j-k-m+64);	
	CString adf;
	adf.Format(_T("%s%d"),ColumnMap[_T("֤������")], startRow);
	excel.SetText(adf,_T("֤������"));
	adf.Format(_T("%s%d"),ColumnMap[_T("��ȨֵW+")], startRow);//
	excel.SetText(adf,_T("��ȨֵW+"));
	adf.Format(_T("%s%d"),ColumnMap[_T("��ȨֵW-")], startRow);
	excel.SetText(adf,_T("��ȨֵW-"));
	adf.Format(_T("%s%d"),ColumnMap[_T("�ۺ�Ȩ��C")], startRow);
	excel.SetText(adf,_T("�ۺ�Ȩ��C"));
	adf.Format(_T("%s%d"),ColumnMap[_T("���Ȩ��C")], startRow);
	excel.SetText(adf,_T("���Ȩ��C"));

	//if (m_weightCtrl.GetCurSel()==0)
	//	{
	//	adf.Format(_T("%s%d"),ColumnMap[_T("��ʯ��(��)")], startRow);
	//	excel.SetText(adf,_T("��ʯ��(��)"));//weight
	//	adf.Format(_T("%s%d"),ColumnMap[_T("������(��)")], startRow);
	//	excel.SetText(adf,_T("������(��)"));//material
	//	}
	//else
	//	{
	//	adf.Format(_T("%s%d"),ColumnMap[_T("��ʯ��(��)")], startRow);
	//	excel.SetText(adf,_T("��ʯ��(���)"));//weight
	//	adf.Format(_T("%s%d"),ColumnMap[_T("������(��)")], startRow);
	//	excel.SetText(adf,_T("������(���)"));//material
	//	}
	//adf.Format(_T("%s%d"),ColumnMap[_T("ƽ��Ʒλ")], startRow);
	////excel.SetText(adf,_T("ƽ��Ʒλ"));//MedianPinWei
	//if (m_tenorCtrl.GetCurSel()==0)
	//	{
	//	excel.SetText(adf,_T("ƽ��Ʒλ(�ٷֱ�)"));
	//	}
	//else
	//	excel.SetText(adf,_T("ƽ��Ʒλ(��/��)"));
	//adf.Format(_T("%s%d"),ColumnMap[_T("���(������)")], startRow);
	//if (m_volCtrl.GetCurSel()==0)
	//	{
	//	excel.SetText(adf,_T("���(������)"));//volume
	//	}
	//else
	//	excel.SetText(adf,_T("���(��������)"));//volume

	//adf.Format(_T("%s%d"),ColumnMap[_T("�����")], startRow);
	//excel.SetText(adf,_T("�����"));//MineBlockStat
	//adf.Format(_T("%s%d"),ColumnMap[_T("��ע")], startRow);
	//excel.SetText(adf,_T("��ע"));
	////��������������λ������ƽ��Ʒλ�����������,��������

	//CString str1,str2 ;
	////m_ListCtrlEnvidecce.GetItemText();
	int nItemCount,nColumnCount;
	nColumnCount = m_ListCtrlEnvidecce.GetHeaderCtrl()->GetItemCount();//�õ�����
	nItemCount = m_ListCtrlEnvidecce.GetItemCount();//�õ�����
	//if(nItemCount >0)
	//{
	//	LVITEM lvitem;
	//	lvitem.mask = LVIF_TEXT | LVIF_PARAM;
	//	for(int i = 0; i < nItemCount; ++ i)
	//	{
	//		lvitem.iItem = i;
	//		lvitem.iSubItem = 0; // ͨ��ȡ����һ�С�
	//		m_ListCtrlEnvidecce.GetItem(&lvitem);
	//		//m_ListCtrlEnvidecce.GetItemText();
	//	}
	//}

	CString strListData;
	CRect rc;
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,0,LVIR_LABEL,rc);//��������RECT��
		strListData=m_ListCtrlEnvidecce.GetItemText(i,0);
		adf.Format(_T("%s%d"),ColumnMap[_T("֤������")], startRow+i);
		excel.SetText(adf,strListData);
	}
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,1,LVIR_LABEL,rc);//��������RECT��
		strListData=m_ListCtrlEnvidecce.GetItemText(i,1);
		adf.Format(_T("%s%d"),ColumnMap[_T("��ȨֵW+")], startRow+i);
		excel.SetText(adf,strListData);
	}
	
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,2,LVIR_LABEL,rc);//��������RECT��
		strListData=m_ListCtrlEnvidecce.GetItemText(i,2);
		adf.Format(_T("%s%d"),ColumnMap[_T("��ȨֵW-")], startRow+i);
		excel.SetText(adf,strListData);
	}
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,3,LVIR_LABEL,rc);//��������RECT��
		strListData=m_ListCtrlEnvidecce.GetItemText(i,3);
		adf.Format(_T("%s%d"),ColumnMap[_T("�ۺ�Ȩ��C")], startRow+i);
		excel.SetText(adf,strListData);
	}
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,4,LVIR_LABEL,rc);//��������RECT��
		strListData=m_ListCtrlEnvidecce.GetItemText(i,4);
		adf.Format(_T("%s%d"),ColumnMap[_T("���Ȩ��C")], startRow+i);
		excel.SetText(adf,strListData);
	}
	//adf.Format(_T("%s%d"),ColumnMap[_T("C")], startRow+1);
	//excel.SetText(adf,_T("C"));
	//int RowOfFeature = SingleCnt + FieldAry.size();
	//str1.Format(_T("%s%d"),ColumnMap[_T("����")],  startRow+1);

	////	CComBSTR ccvstr = _T("�����");
	////CComVariant cvar;
	//for (int cc = 0;cc<feaVecSize;cc++) 
	//	{ 
	//	int temp2 = cc*FieldAry.size();
	//	//m_pFeatureVec[cc].pFeature->get_ValueByName(ccvstr,&cvar);

	//	str2.Format(_T("%s%d"), ColumnMap[_T("����")],(cc+1)*RowOfFeature+startRow);
	//excel.Merge(str1,str2);
	//	//	str2 = _T("����1") ;// cvar.bstrVal;[10/7/2009 mubin]
	//	//	str2.Format(_T("����%d"),cc);
	//	str2 =m_pFeatureVec[cc].LayerName;

	//excel.SetText(str1,str2);

	//	str1.Format(_T("%s%d"),ColumnMap[_T("Ԫ������")], cc*RowOfFeature+startRow+1);
	// int MergeStep = recordcount+1;
	//	for (int k= 0;k<FieldAry.size();k++)
	//		{
	//		str2.Format(_T("%s%d"),ColumnMap[_T("Ԫ������")],(k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		excel.Merge(str1,str2);
	//		excel.SetText(str1,FieldAry[k]);

	//		for (int i = 0 ;i<recordcount;i++)
	//			{
	//			str1.Format(_T("%s%d"),ColumnMap[_T("Ʒλ����")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			CString str,str3,str4,str5;
	//			str3.Format(_T("%f"),fromary[i]);
	//			str4.Format(_T("%f"),toary[i]);
	//			str5 = str3+_T("~")+str4;
	//			excel.SetText(str1,str5);

	//			str1.Format(_T("%s%d"),ColumnMap[_T("��ʯ��(��)")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			str.Format(_T("%f"),weight[cc*SingleCnt+ k*recordcount+i]);
	//			excel.SetText(str1,str);

	//			str1.Format(_T("%s%d"),ColumnMap[_T("������(��)")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			str.Format(_T("%f"),material[cc*SingleCnt+k*recordcount+i]);
	//			excel.SetText(str1,str);

	//			str1.Format(_T("%s%d"),ColumnMap[_T("ƽ��Ʒλ")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			str.Format(_T("%f"),MedianPinWei[cc*SingleCnt+k*recordcount+i]);
	//			excel.SetText(str1,str);

	//			str1.Format(_T("%s%d"),ColumnMap[_T("���(������)")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			str.Format(_T("%f"),volume[cc*SingleCnt+k*recordcount+i]);
	//			excel.SetText(str1,str);

	//			str1.Format(_T("%s%d"),ColumnMap[_T("�����")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			str.Format(_T("%ld"),MineBlockStat[cc*SingleCnt+k*recordcount+i]);
	//			excel.SetText(str1,str);
	//			}

	//		str1.Format(_T("%s%d"),ColumnMap[_T("Ʒλ����")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		excel.SetText(str1,_T("�ܼ�"));
	//		str1.Format(_T("%s%d"),ColumnMap[_T("��ʯ��(��)")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		str2.Format(_T("%f"),Accumulate_weight[temp2+k]);
	//		excel.SetText(str1,str2);
	//		str1.Format(_T("%s%d"),ColumnMap[_T("������(��)")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		str2.Format(_T("%f"),Accumulate_material[temp2+k]);
	//		excel.SetText(str1,str2);
	//		str1.Format(_T("%s%d"),ColumnMap[_T("ƽ��Ʒλ")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		str2.Format(_T("%f"),Accumulate_MedianPinWei[temp2+k]);
	//		excel.SetText(str1,str2);
	//		str1.Format(_T("%s%d"),ColumnMap[_T("���(������)")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		str2.Format(_T("%f"),Accumulate_volume[temp2+k]);
	//		excel.SetText(str1,str2);
	//		str1.Format(_T("%s%d"),ColumnMap[_T("�����")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		str2.Format(_T("%ld"),Accumulate_MineBlockStat[temp2+k]);
	//		excel.SetText(str1,str2);

	//		//////////////////////////////////////////////////////////////////////////
	//		//��һ��Ԫ��
	//		str1.Format(_T("%s%d"),ColumnMap[_T("Ԫ������")],  (k+1)*MergeStep+startRow+cc*RowOfFeature+1);

	//		}
	//	str1.Format(_T("%s%d"),ColumnMap[_T("����")],  startRow+(cc+1)*RowOfFeature+1);

	//	}
}


void CDlg_InformationCalculate2::OnBnClickedPosteriorprobability()
{	
	//������ʼ���
	//��ÿһ������Ԫ��ֵ��f(x) = a1*x1+a2*x2+...
	
}

void CDlg_InformationCalculate2::OnBnClickedDeleteenvdence()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	POSITION p=m_ListCtrlEnvidecce.GetFirstSelectedItemPosition();
	int idx = m_ListCtrlEnvidecce.GetNextSelectedItem(p); 
	while( idx != -1)
	{
		LVITEM lvi;
		lvi.iItem = idx;
		lvi.iSubItem = 0;
		//lvi.mask = LVIF_IMAGE;
		if (m_ListCtrlEnvidecce.GetItem(&lvi)){
		}
		m_ListCtrlEnvidecce.DeleteItem(idx);
		idx = m_ListCtrlEnvidecce.GetNextSelectedItem(p); 
	}
	m_ListCtrlEnvidecce.Arrange(LVA_DEFAULT);  
	m_ListCtrlEnvidecce.RedrawItems(0, m_ListCtrlEnvidecce.GetItemCount());
}

void CDlg_InformationCalculate2::OnBnClickedIndependencetest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDlg_IndependenceTest dlg;
	dlg.pVoxet = pDlgPriorProbability->m_pVoxet;	
	dlg.DoModal();
}

void CDlg_InformationCalculate2::OnBnClickedAscertainfactor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//ȷ�������ҿ��־
	if (findfactor == false)
	{		
		int nItem = m_ListCtrlEnvidecce.GetItemCount();//�õ�listctrl����
		int nColumnCount = pDlgPriorProbability->m_ListCtrl.GetHeaderCtrl()->GetItemCount();//�õ�listctrl����

		CString strInformationData;
		float tempdata = 0;
		float SumInformationData = 0;
		for (int i=0;i<nItem;i++)
		{		
			strInformationData = m_ListCtrlEnvidecce.GetItemText(i,nColumnCount-1);
			tempdata =_wtof(strInformationData);
			if (tempdata >= 0)
			{
				SumInformationData+=tempdata;
			}
		}

		//�ۼ��ٽ�ֵ
		float delta_I = SumInformationData * _wtof(m_InformationLevel);

		float ftemp = 0;
		SumInformationData = 0;
		int nitemsave = 0;
		for (int i=0;i<nItem;i++)
		{
			strInformationData = m_ListCtrlEnvidecce.GetItemText(i,nColumnCount-1);
			tempdata =_wtof(strInformationData);
			if (tempdata >= 0)
			{
				SumInformationData+=tempdata;
			}
			ftemp = delta_I- SumInformationData;
			if (ftemp<0)
			{				
				for (int j=nItem;j>i;j--)
				{
					m_ListCtrlEnvidecce.DeleteItem(j);
				}
				break;
			}
		}
	

		findfactor = true;
	}
	
}

void CDlg_InformationCalculate2::OnBnClickedSortbyinformation()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//����Ϣ��ֵ����
	int nItem = pDlgPriorProbability->m_ListBox2.GetCount();//����
	int nColumnCount = pDlgPriorProbability->m_ListCtrl.GetHeaderCtrl()->GetItemCount();//����

	CString str;
	int nId;
	POSITION pos = pDlgPriorProbability->m_ListCtrl.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{
		//return;
	}
	//�õ��кţ�ͨ��POSITIONת��
	nId=(int)pDlgPriorProbability->m_ListCtrl.GetNextSelectedItem(pos);
	//�õ����е����ݣ�0��ʾ��һ�У�ͬ��1,2,3...��ʾ�ڶ���������...�У�
	str=pDlgPriorProbability->m_ListCtrl.GetItemText(nId,0);
	str=pDlgPriorProbability->m_ListCtrl.GetItemText(nId,1);

	CString strInformationData;
	float tempdata = 0;
	float SumInformationData = 0;
	for (int i=0;i<nItem;i++)
	{
		strInformationData = pDlgPriorProbability->m_ListCtrl.GetItemText(i,nColumnCount-1);
		tempdata =_wtof(strInformationData);
	}

}

int comparefloat(CString str1,CString str2)
{
	//����
	float f1 = _wtof(str1);
	float f2 = _wtof(str2);
	if (f1 == f2)
	{
		return 0;
	}
	else if (f1>f2)
	{
		return 1;
	}
	else if (f1<f2)
	{
		return 0;
	}
}




static int CALLBACK CompareFunc(LPARAM   lParam1,   LPARAM   lParam2,LPARAM   lParamSort)
{	
	CListCtrl*   pListCtrl   =   (CListCtrl*)   lParamSort; 

	LVFINDINFO findInfo; 
	findInfo.flags = LVFI_PARAM; 
	findInfo.lParam = lParam1; 
	int iItem1 = pListCtrl->FindItem(&findInfo, -1); 
	findInfo.lParam = lParam2; 
	int iItem2 = pListCtrl->FindItem(&findInfo, -1);

	CString strItem1,strItem2;
	strItem1 = pListCtrl->GetItemText(iItem1, dwSelColID );   
	strItem2 = pListCtrl->GetItemText(iItem2, dwSelColID );  
	//if(bASC)//����
	//	return strcmp((LPSTR)(LPCTSTR)strItem1,(LPSTR)(LPCTSTR)strItem2);
	//return strcmp((LPSTR)(LPCTSTR)strItem2,(LPSTR)(LPCTSTR)strItem1);
	if (bASC)
	{
		return comparefloat(strItem1,strItem2);
	}
	return comparefloat(strItem2,strItem1);
}

void CDlg_InformationCalculate2::OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	if(dwSelColID != pNMLV->iSubItem)//ѡ��Ĳ�����ͬ��
	{

		dwSelColID= pNMLV->iSubItem;//�õ��µ��к�
		bASC = true;  //��һ��ѡ������������
	}
	else
	{
		bASC = !bASC;//�ı��������
	}

	m_ListCtrlEnvidecce.SortItems(CompareFunc,(LPARAM)(&m_ListCtrlEnvidecce)); 
	//return 0;
}

void CDlg_InformationCalculate2::OnCbnSelchangeComboInformationLevel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_INFORMATION_LEVEL))->GetCurSel();//��ǰѡ�е���	
	int n = m_ComboBoxInformationLevel.GetLBTextLen(iPos);
	m_ComboBoxInformationLevel.GetLBText(iPos, m_InformationLevel.GetBuffer(n));
	m_InformationLevel.ReleaseBuffer();
}

void CDlg_InformationCalculate2::OnBnClickedInformationInputvoxet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//��Ϣ�����㲢��ֵ������
	IcwPropertyTablePtr pPropertyTable;
	pDlgPriorProbability->m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);
	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);
	if (pFields == NULL)
		return;
	CComBSTR name = _T("��Ϣ��");
	cwFieldType fieldtype = cwFieldTypeFloat32;
	CComVariant Nodatavalue;
	Nodatavalue.vt = VT_R4;
	Nodatavalue.bVal = 0.0;	
	pFields->FindField(name,&pField);
	if (pField == NULL)
	{
		//����field
		CoCreateInstance(CLSID_cwField,NULL,CLSCTX_INPROC_SERVER,IID_IcwField,(void**)&pField);
		pField->put_Name(name);
		pField->put_NoDataValue(Nodatavalue);
		pField->put_Type(fieldtype);
		pFields->AddField(pField);
	}
	else
	{
		cwFieldType knowntype;
		pField->get_Type(&knowntype);
		if (knowntype != fieldtype)
		{
			AfxMessageBox(_T("����������Voxet�Ѵ��ڣ������Ͳ�ͬ�����޸���������!"));
			return;
		}
	}
	
	long nu,nv,nw;
	cwVector3F stepU,stepV,stepW,Origin;		
	pDlgPriorProbability->m_pVoxet->get_nU(&nu);
	pDlgPriorProbability->m_pVoxet->get_nV(&nv);
	pDlgPriorProbability->m_pVoxet->get_nW(&nw);
	pDlgPriorProbability->m_pVoxet->get_Origin(&Origin);
	pDlgPriorProbability->m_pVoxet->get_StepU(&stepU);
	pDlgPriorProbability->m_pVoxet->get_StepV(&stepV);
	pDlgPriorProbability->m_pVoxet->get_StepW(&stepW);

	double origin[3] = {Origin.X,Origin.Y,Origin.Z};
	double vecU[3] = {stepU.X,stepU.Y,stepU.Z};
	double vecV[3] = {stepV.X,stepV.Y,stepV.Z};
	double vecW[3] = {stepW.X,stepW.Y,stepW.Z};

	int nRow = m_ListCtrlEnvidecce.GetItemCount(); //��
	int nColumn = m_ListCtrlEnvidecce.GetHeaderCtrl()->GetItemCount(); //��


	//��ÿһ�����ӣ���ȡ��Ϣ��
	std::vector<float> InformationValueVec;	
	for(int nitem = 0;nitem<nRow; nitem++)
	{
		CString strInformationValue = m_ListCtrlEnvidecce.GetItemText(nitem,nColumn-1);		
		float fTemp= _wtof(strInformationValue);	
		InformationValueVec.push_back(fTemp);	
	}
	
	//Ѱ��������־����index
	IcwColumnDataPtr pColumnData;
	IcwBigColumnData3DPtr pBigColumnData3D;
	for (int SingleItem = 0; SingleItem<nRow; SingleItem++)
	{
		std::vector<long> singvec;
		CString str;
		str = m_ListCtrlEnvidecce.GetItemText(SingleItem,0);
		CComBSTR bstr = str.AllocSysString();
		pPropertyTable->GetColumn(bstr, &pColumnData);
		pBigColumnData3D = pColumnData;
		long voxetindex;
		for (long u = 0;u<nu;u++)
		{
			long planeindex = 0;
			IcwDataPlanePtr pDataplane;
			HRESULT hr = pBigColumnData3D->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane);
			IcwDataArrayPtr pArray = pDataplane;	
			void * pbuffer;
			pArray->GetBuffer(&pbuffer);
			float * pfloatArray = NULL;
			pfloatArray = (float*)pbuffer;
			for (long v = 0;v<nv;v++)
			{
				for (long w= 0;w<nw;w++)
				{
					planeindex = w+v*nw;
					voxetindex = u * nv * nw + v * nw + w;
					if (pfloatArray[planeindex] == 1)
					{
						//������֤�����ӣ��洢index
						singvec.push_back(voxetindex);						
					}
				}
			}
		}
		InformationExist.push_back(singvec);
	
		std::vector<long>().swap(singvec);//�ͷ��ڴ�		
	}
	//ȡ��Ϣ���ֶ�
	pPropertyTable->GetColumn(name, &pColumnData);
	pBigColumnData3D = pColumnData;
	ASSERT(pBigColumnData3D);


	//����һ��voxet����ÿһ������Ԫ�Ҵ��ڵ�����,��������Ԫ��ֵfx
	CComVariant FillValue;	
	FillValue.vt = VT_R4;	
	long voxetindex;	
	HRESULT hr;
	for (long u = 0;u<nu;u++)
	{
		long planeindex = 0;
		IcwDataPlanePtr pDataplane;
		hr = pBigColumnData3D->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane);
		IcwDataArrayPtr pArray = pDataplane;	
		void * pbuffer;
		pArray->GetBuffer(&pbuffer);
		float * pfloatArray = NULL;
		pfloatArray = (float*)pbuffer;
		for (long v = 0;v<nv;v++)
		{
			for (long w= 0;w<nw;w++)
			{
				planeindex = w+v*nw;
				voxetindex = u * nv * nw + v * nw + w;
				FillValue.fltVal = 0.0;
				for (int SingleItem = 0; SingleItem<nRow; SingleItem++)
				{
					std::vector<long>::iterator result = find(InformationExist[SingleItem].begin(),InformationExist[SingleItem].end(),voxetindex);
					if (result == InformationExist[SingleItem].end())
					{
						//û�ҵ�����
					}
					else
					{
						//�ҵ�����
						FillValue.fltVal += InformationValueVec[SingleItem];//f(x) = a1x1+a2x2+a3x3+...
					}
				}
				//��ÿһ������Ԫ��ֵfx
				pfloatArray[planeindex] = FillValue.fltVal;
			}
		}
		hr = pBigColumnData3D->PutDataPlane(u,0,0,cwPlane_VW,nv,nw,pDataplane);
		if(hr !=S_OK )
			AfxMessageBox(_T("PutDataPlane������"));
	}	

}
