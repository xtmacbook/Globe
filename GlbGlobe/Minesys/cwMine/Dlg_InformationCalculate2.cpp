// Dlg_EvidenceMethod.cpp : 实现文件
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

// CDlg_InformationCalculate2 对话框

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

// CDlg_InformationCalculate2 消息处理程序

//////////////////////////////////////////////////////////////////////////
DWORD  dwSelColID =0; //选择的列
BOOL   bASC = FALSE;     //是否升序
bool findfactor = false;


BOOL CDlg_InformationCalculate2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 获取向导
	CPropertySheet *Wizard = reinterpret_cast<CPropertySheet *>(GetParent());
	pDlgPriorProbability= (CDlg_InformationCalculate *)Wizard->GetPage(0);

	//long nField;
 	int nItem = pDlgPriorProbability->m_ListBox2.GetCount();//行数
	int nColumnCount = pDlgPriorProbability->m_ListCtrl.GetHeaderCtrl()->GetItemCount();//列数
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

	

	//初始化listctrl列名
	m_ListCtrlEnvidecce.InsertColumn( 0, _T("有利因素"), LVCFMT_LEFT, 90 );
	m_ListCtrlEnvidecce.InsertColumn( 1, _T("含矿含因子体素"), LVCFMT_LEFT, 90 );
	m_ListCtrlEnvidecce.InsertColumn( 2, _T("含因子体素"), LVCFMT_LEFT, 90 );
	m_ListCtrlEnvidecce.InsertColumn( 3, _T("含矿体素"), LVCFMT_LEFT, 90 );
	m_ListCtrlEnvidecce.InsertColumn( 4, _T("研究区体素"), LVCFMT_LEFT, 90 );
	m_ListCtrlEnvidecce.InsertColumn( 5, _T("信息量值"), LVCFMT_LEFT, 90 );


	//显示每个因子信息量值
	//SetItemText(行号，列号，信息量值)
	CString strListData;
	for (int i=0;i<nItem;i++)
	{
		for(int j = 0;j<nColumnCount;j++)
		{
			//pDlgPriorProbability->m_ListCtrl.GetSubItemRect(i,0,LVIR_LABEL,rc);//获得子项的RECT；
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
	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);  // 设置上一步按钮可用
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
		AfxMessageBox(_T("请输入断层名称!"));
		return -1;
	}*/
	return CPropertyPage::OnWizardNext();
}



////计算正权值
//float EnvidenceMethodWPlus(long nGrid,long nMine,long nEnvidence,long nMineCrossEnvidence)
//{
//	float W;
//	float a = (float)nMineCrossEnvidence/(float)nMine;
//	float b = (float)(nEnvidence - nMineCrossEnvidence)/(float)(nGrid - nMine);
//	float n = (float)a/(float)b;
//	W = log((long double)n);
//	return W;
//}
////计算负权值
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
	// TODO: 在此添加控件通知处理程序代码
	//计算各因子证据权值
	//研究区总网格数
	long nu,nv,nw;
	pDlgPriorProbability->m_pVoxet->get_nU(&nu);
	pDlgPriorProbability->m_pVoxet->get_nV(&nv);
	pDlgPriorProbability->m_pVoxet->get_nW(&nw);	
// 	long nGridCount = pDlgPriorProbability->ResearchAreaTrueIndex.size();

	//含矿点网格数
// 	long nMineCount = pDlgPriorProbability->MineTrueIndex.size();
	//不含矿点网格数
// 	long nNoneMineCount = nGridCount - nMineCount;

	//各因子所含网格数，计算证据权值
	int nItem = pDlgPriorProbability->m_ListBox2.GetCount();
	std::vector<float> C;
	float Cplus = 0;
	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{
// 		long nEnvidenceCount = pDlgPriorProbability->EnvidenceTrueIndex[SingleItem].size();
// 		long nMineCrossEnvidenceCount = pDlgPriorProbability->ptT_T[SingleItem];

		//对每一个因子计算正负权值
//		float wplus = EnvidenceMethodWPlus(nGridCount,nMineCount,nEnvidenceCount,nMineCrossEnvidenceCount);
//		float wminus = EnvidenceMethodWMinus(nGridCount,nMineCount,nEnvidenceCount,nMineCrossEnvidenceCount);
//		float Counterpoise = wplus - wminus;
//		C.push_back(Counterpoise);
//		Cplus+=Counterpoise;
//
//		//显示
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
//		MessageBox("创建Excel服务失败!","提示",MB_OK|MB_ICONWARNING);
//		return;
//	} 
//
//	m_appExcel.SetVisible(false); 
//	m_wbExcels.AttachDispatch(m_appExcel.GetWorkbooks(),true);
//	m_wbExcelSingle.AttachDispatch(m_wbExcels.Add(covOptional));
//	//得到Worksheets 
//	m_wsExcelSingle.AttachDispatch(m_wbExcelSingle.GetWorksheets(),true);
//	//删除多余的表
//	//m_wsExcelSingle.AttachDispatch(m_wsExcels.GetItem(COleVariant((short)3)));
//	//m_wsExcelSingle.Delete();
//	//m_wsExcelSingle.AttachDispatch(m_wsExcels.GetItem(COleVariant((short)2)));
//	//m_wsExcelSingle.Delete();
//	//表改名
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
//		//写列表头
//		for(i=0;pList->GetColumn(i,&columnData);i++)
//		{
//			m_rangeExcel.SetItem( _variant_t( (long)(1) ), _variant_t( (long)(i+1) ),COleVariant(columnData.pszText) );
//		}
//		columnName.ReleaseBuffer ();
//
//		// 向Excel中写数据
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
//		//此方法对于不同的Excel版本参数个数可能不能，具体参看MSDN
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
//		//没有数据
//		MessageBox("没有数据，不能导出!","提示",MB_OK|MB_ICONWARNING|MB_TOPMOST);
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
	// 输出证据权值表
	//TCHAR szFilters[]= _T("txt Files (*.txt)|*.txt");
	//CFileDialog fileDlg(FALSE, _T("txt"), _T("*txt"),OFN_FILEMUSTEXIST |OFN_HIDEREADONLY, szFilters);

	//if (fileDlg.DoModal()==IDOK)
	//{
	//	//保存文件
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
	
	//先选择路径
	CString FilePathName;//文件名参数定义
	CFileDialog Dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("excel Files(*.xls)|*.xls"));
	//打开文件
	if(Dlg.DoModal() == IDOK)//是否打开成功
	{
		FilePathName =  Dlg.GetPathName();//取得文件路径及文件名
		//SetDlgItemText(IDC_OutPutFile,FilePathName);//在一个地址为IDC_OutPutFile的editbox中显示文件名
	}
	else//打开失败处理
	{
		//打开失败处理
		MessageBox(_T("打开失败"),NULL,MB_OK);
	}
	//CFileDialog filedlg(FALSE, _T("xls"), _T("*.xls"), OFN_ALLOWMULTISELECT);
	//filedlg.DoModal();

	OnOK();
	//////////////////////////////////////////////////////////////////////////
	// 以下是写excel [10/8/2009 mubin]

	std::map<const wchar_t*,CString> ColumnMap;
	ColumnMap[_T("证据因子")] = 'A';
	ColumnMap[_T("正权值W+")] = 'B';
	ColumnMap[_T("负权值W-")] = 'C';
	ColumnMap[_T("综合权重C")] = 'D';
	ColumnMap[_T("相对权重C")] = 'E';

	ExcelMgr excel; 
	excel.CreateExcel(FilePathName);	
	excel.SetCurSheet(1);
	////	GridToExcel(excel);
	excel.SetSheetName(_T("各因子证据权值"));

	int startRow = 1;// 指显示列名的行 [10/8/2009 mubin]
	////strCaptial= (char)(j-k-m+64);	
	CString adf;
	adf.Format(_T("%s%d"),ColumnMap[_T("证据因子")], startRow);
	excel.SetText(adf,_T("证据因子"));
	adf.Format(_T("%s%d"),ColumnMap[_T("正权值W+")], startRow);//
	excel.SetText(adf,_T("正权值W+"));
	adf.Format(_T("%s%d"),ColumnMap[_T("负权值W-")], startRow);
	excel.SetText(adf,_T("负权值W-"));
	adf.Format(_T("%s%d"),ColumnMap[_T("综合权重C")], startRow);
	excel.SetText(adf,_T("综合权重C"));
	adf.Format(_T("%s%d"),ColumnMap[_T("相对权重C")], startRow);
	excel.SetText(adf,_T("相对权重C"));

	//if (m_weightCtrl.GetCurSel()==0)
	//	{
	//	adf.Format(_T("%s%d"),ColumnMap[_T("矿石量(吨)")], startRow);
	//	excel.SetText(adf,_T("矿石量(吨)"));//weight
	//	adf.Format(_T("%s%d"),ColumnMap[_T("金属量(吨)")], startRow);
	//	excel.SetText(adf,_T("金属量(吨)"));//material
	//	}
	//else
	//	{
	//	adf.Format(_T("%s%d"),ColumnMap[_T("矿石量(吨)")], startRow);
	//	excel.SetText(adf,_T("矿石量(万吨)"));//weight
	//	adf.Format(_T("%s%d"),ColumnMap[_T("金属量(吨)")], startRow);
	//	excel.SetText(adf,_T("金属量(万吨)"));//material
	//	}
	//adf.Format(_T("%s%d"),ColumnMap[_T("平均品位")], startRow);
	////excel.SetText(adf,_T("平均品位"));//MedianPinWei
	//if (m_tenorCtrl.GetCurSel()==0)
	//	{
	//	excel.SetText(adf,_T("平均品位(百分比)"));
	//	}
	//else
	//	excel.SetText(adf,_T("平均品位(克/吨)"));
	//adf.Format(_T("%s%d"),ColumnMap[_T("体积(立方米)")], startRow);
	//if (m_volCtrl.GetCurSel()==0)
	//	{
	//	excel.SetText(adf,_T("体积(立方米)"));//volume
	//	}
	//else
	//	excel.SetText(adf,_T("体积(万立方米)"));//volume

	//adf.Format(_T("%s%d"),ColumnMap[_T("矿块数")], startRow);
	//excel.SetText(adf,_T("矿块数"));//MineBlockStat
	//adf.Format(_T("%s%d"),ColumnMap[_T("备注")], startRow);
	//excel.SetText(adf,_T("备注"));
	////区间体积，区间吨位，区间平均品位，区间金属量,区间矿块数

	//CString str1,str2 ;
	////m_ListCtrlEnvidecce.GetItemText();
	int nItemCount,nColumnCount;
	nColumnCount = m_ListCtrlEnvidecce.GetHeaderCtrl()->GetItemCount();//得到列数
	nItemCount = m_ListCtrlEnvidecce.GetItemCount();//得到行数
	//if(nItemCount >0)
	//{
	//	LVITEM lvitem;
	//	lvitem.mask = LVIF_TEXT | LVIF_PARAM;
	//	for(int i = 0; i < nItemCount; ++ i)
	//	{
	//		lvitem.iItem = i;
	//		lvitem.iSubItem = 0; // 通常取“第一列”
	//		m_ListCtrlEnvidecce.GetItem(&lvitem);
	//		//m_ListCtrlEnvidecce.GetItemText();
	//	}
	//}

	CString strListData;
	CRect rc;
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,0,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrlEnvidecce.GetItemText(i,0);
		adf.Format(_T("%s%d"),ColumnMap[_T("证据因子")], startRow+i);
		excel.SetText(adf,strListData);
	}
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,1,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrlEnvidecce.GetItemText(i,1);
		adf.Format(_T("%s%d"),ColumnMap[_T("正权值W+")], startRow+i);
		excel.SetText(adf,strListData);
	}
	
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,2,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrlEnvidecce.GetItemText(i,2);
		adf.Format(_T("%s%d"),ColumnMap[_T("负权值W-")], startRow+i);
		excel.SetText(adf,strListData);
	}
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,3,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrlEnvidecce.GetItemText(i,3);
		adf.Format(_T("%s%d"),ColumnMap[_T("综合权重C")], startRow+i);
		excel.SetText(adf,strListData);
	}
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,4,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrlEnvidecce.GetItemText(i,4);
		adf.Format(_T("%s%d"),ColumnMap[_T("相对权重C")], startRow+i);
		excel.SetText(adf,strListData);
	}
	//adf.Format(_T("%s%d"),ColumnMap[_T("C")], startRow+1);
	//excel.SetText(adf,_T("C"));
	//int RowOfFeature = SingleCnt + FieldAry.size();
	//str1.Format(_T("%s%d"),ColumnMap[_T("矿体")],  startRow+1);

	////	CComBSTR ccvstr = _T("矿体号");
	////CComVariant cvar;
	//for (int cc = 0;cc<feaVecSize;cc++) 
	//	{ 
	//	int temp2 = cc*FieldAry.size();
	//	//m_pFeatureVec[cc].pFeature->get_ValueByName(ccvstr,&cvar);

	//	str2.Format(_T("%s%d"), ColumnMap[_T("矿体")],(cc+1)*RowOfFeature+startRow);
	//excel.Merge(str1,str2);
	//	//	str2 = _T("矿体1") ;// cvar.bstrVal;[10/7/2009 mubin]
	//	//	str2.Format(_T("矿体%d"),cc);
	//	str2 =m_pFeatureVec[cc].LayerName;

	//excel.SetText(str1,str2);

	//	str1.Format(_T("%s%d"),ColumnMap[_T("元素类型")], cc*RowOfFeature+startRow+1);
	// int MergeStep = recordcount+1;
	//	for (int k= 0;k<FieldAry.size();k++)
	//		{
	//		str2.Format(_T("%s%d"),ColumnMap[_T("元素类型")],(k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		excel.Merge(str1,str2);
	//		excel.SetText(str1,FieldAry[k]);

	//		for (int i = 0 ;i<recordcount;i++)
	//			{
	//			str1.Format(_T("%s%d"),ColumnMap[_T("品位区间")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			CString str,str3,str4,str5;
	//			str3.Format(_T("%f"),fromary[i]);
	//			str4.Format(_T("%f"),toary[i]);
	//			str5 = str3+_T("~")+str4;
	//			excel.SetText(str1,str5);

	//			str1.Format(_T("%s%d"),ColumnMap[_T("矿石量(吨)")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			str.Format(_T("%f"),weight[cc*SingleCnt+ k*recordcount+i]);
	//			excel.SetText(str1,str);

	//			str1.Format(_T("%s%d"),ColumnMap[_T("金属量(吨)")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			str.Format(_T("%f"),material[cc*SingleCnt+k*recordcount+i]);
	//			excel.SetText(str1,str);

	//			str1.Format(_T("%s%d"),ColumnMap[_T("平均品位")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			str.Format(_T("%f"),MedianPinWei[cc*SingleCnt+k*recordcount+i]);
	//			excel.SetText(str1,str);

	//			str1.Format(_T("%s%d"),ColumnMap[_T("体积(立方米)")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			str.Format(_T("%f"),volume[cc*SingleCnt+k*recordcount+i]);
	//			excel.SetText(str1,str);

	//			str1.Format(_T("%s%d"),ColumnMap[_T("矿块数")],k*MergeStep+1+i+startRow+cc*RowOfFeature);
	//			str.Format(_T("%ld"),MineBlockStat[cc*SingleCnt+k*recordcount+i]);
	//			excel.SetText(str1,str);
	//			}

	//		str1.Format(_T("%s%d"),ColumnMap[_T("品位区间")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		excel.SetText(str1,_T("总计"));
	//		str1.Format(_T("%s%d"),ColumnMap[_T("矿石量(吨)")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		str2.Format(_T("%f"),Accumulate_weight[temp2+k]);
	//		excel.SetText(str1,str2);
	//		str1.Format(_T("%s%d"),ColumnMap[_T("金属量(吨)")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		str2.Format(_T("%f"),Accumulate_material[temp2+k]);
	//		excel.SetText(str1,str2);
	//		str1.Format(_T("%s%d"),ColumnMap[_T("平均品位")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		str2.Format(_T("%f"),Accumulate_MedianPinWei[temp2+k]);
	//		excel.SetText(str1,str2);
	//		str1.Format(_T("%s%d"),ColumnMap[_T("体积(立方米)")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		str2.Format(_T("%f"),Accumulate_volume[temp2+k]);
	//		excel.SetText(str1,str2);
	//		str1.Format(_T("%s%d"),ColumnMap[_T("矿块数")], (k+1)*MergeStep+startRow+cc*RowOfFeature);
	//		str2.Format(_T("%ld"),Accumulate_MineBlockStat[temp2+k]);
	//		excel.SetText(str1,str2);

	//		//////////////////////////////////////////////////////////////////////////
	//		//下一个元素
	//		str1.Format(_T("%s%d"),ColumnMap[_T("元素类型")],  (k+1)*MergeStep+startRow+cc*RowOfFeature+1);

	//		}
	//	str1.Format(_T("%s%d"),ColumnMap[_T("矿体")],  startRow+(cc+1)*RowOfFeature+1);

	//	}
}


void CDlg_InformationCalculate2::OnBnClickedPosteriorprobability()
{	
	//后验概率计算
	//给每一个网格单元赋值，f(x) = a1*x1+a2*x2+...
	
}

void CDlg_InformationCalculate2::OnBnClickedDeleteenvdence()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	CDlg_IndependenceTest dlg;
	dlg.pVoxet = pDlgPriorProbability->m_pVoxet;	
	dlg.DoModal();
}

void CDlg_InformationCalculate2::OnBnClickedAscertainfactor()
{
	// TODO: 在此添加控件通知处理程序代码

	//确定有利找矿标志
	if (findfactor == false)
	{		
		int nItem = m_ListCtrlEnvidecce.GetItemCount();//得到listctrl行数
		int nColumnCount = pDlgPriorProbability->m_ListCtrl.GetHeaderCtrl()->GetItemCount();//得到listctrl列数

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

		//累计临界值
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
	// TODO: 在此添加控件通知处理程序代码

	//按信息量值排序
	int nItem = pDlgPriorProbability->m_ListBox2.GetCount();//行数
	int nColumnCount = pDlgPriorProbability->m_ListCtrl.GetHeaderCtrl()->GetItemCount();//列数

	CString str;
	int nId;
	POSITION pos = pDlgPriorProbability->m_ListCtrl.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{
		//return;
	}
	//得到行号，通过POSITION转化
	nId=(int)pDlgPriorProbability->m_ListCtrl.GetNextSelectedItem(pos);
	//得到列中的内容（0表示第一列，同理1,2,3...表示第二，三，四...列）
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
	//升序
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
	//if(bASC)//升序
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
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(dwSelColID != pNMLV->iSubItem)//选择的不是相同列
	{

		dwSelColID= pNMLV->iSubItem;//得到新的列号
		bASC = true;  //第一次选择是升序排列
	}
	else
	{
		bASC = !bASC;//改变排序规则
	}

	m_ListCtrlEnvidecce.SortItems(CompareFunc,(LPARAM)(&m_ListCtrlEnvidecce)); 
	//return 0;
}

void CDlg_InformationCalculate2::OnCbnSelchangeComboInformationLevel()
{
	// TODO: 在此添加控件通知处理程序代码
	int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_INFORMATION_LEVEL))->GetCurSel();//当前选中的行	
	int n = m_ComboBoxInformationLevel.GetLBTextLen(iPos);
	m_ComboBoxInformationLevel.GetLBText(iPos, m_InformationLevel.GetBuffer(n));
	m_InformationLevel.ReleaseBuffer();
}

void CDlg_InformationCalculate2::OnBnClickedInformationInputvoxet()
{
	// TODO: 在此添加控件通知处理程序代码

	//信息量计算并赋值到网格
	IcwPropertyTablePtr pPropertyTable;
	pDlgPriorProbability->m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);
	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);
	if (pFields == NULL)
		return;
	CComBSTR name = _T("信息量");
	cwFieldType fieldtype = cwFieldTypeFloat32;
	CComVariant Nodatavalue;
	Nodatavalue.vt = VT_R4;
	Nodatavalue.bVal = 0.0;	
	pFields->FindField(name,&pField);
	if (pField == NULL)
	{
		//创建field
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
			AfxMessageBox(_T("输入属性在Voxet已存在，且类型不同，请修改输入属性!"));
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

	int nRow = m_ListCtrlEnvidecce.GetItemCount(); //行
	int nColumn = m_ListCtrlEnvidecce.GetHeaderCtrl()->GetItemCount(); //列


	//对每一个因子，获取信息量
	std::vector<float> InformationValueVec;	
	for(int nitem = 0;nitem<nRow; nitem++)
	{
		CString strInformationValue = m_ListCtrlEnvidecce.GetItemText(nitem,nColumn-1);		
		float fTemp= _wtof(strInformationValue);	
		InformationValueVec.push_back(fTemp);	
	}
	
	//寻找有利标志网格index
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
						//网格有证据因子，存储index
						singvec.push_back(voxetindex);						
					}
				}
			}
		}
		InformationExist.push_back(singvec);
	
		std::vector<long>().swap(singvec);//释放内存		
	}
	//取信息量字段
	pPropertyTable->GetColumn(name, &pColumnData);
	pBigColumnData3D = pColumnData;
	ASSERT(pBigColumnData3D);


	//遍历一遍voxet，对每一个网格单元找存在的因子,并对网格单元赋值fx
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
						//没找到因子
					}
					else
					{
						//找到因子
						FillValue.fltVal += InformationValueVec[SingleItem];//f(x) = a1x1+a2x2+a3x3+...
					}
				}
				//给每一个网格单元赋值fx
				pfloatArray[planeindex] = FillValue.fltVal;
			}
		}
		hr = pBigColumnData3D->PutDataPlane(u,0,0,cwPlane_VW,nv,nw,pDataplane);
		if(hr !=S_OK )
			AfxMessageBox(_T("PutDataPlane出错！！"));
	}	

}
