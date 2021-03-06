// Dlg_EvidenceMethod.cpp : 实现文件
//

#include "stdafx.h"
#include "Dlg_EvidenceMethod.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "Dlg_IndependenceTest.h"
#include "Dlg_DataStatistics.h"
#include "ExcelMgr.h"
//#include "excel9.h"
//#include "dlgtest.h"

// CDlg_EvidenceMethod 对话框

IMPLEMENT_DYNAMIC(CDlg_EvidenceMethod, CPropertyPage)

CDlg_EvidenceMethod::CDlg_EvidenceMethod()
	: CPropertyPage(CDlg_EvidenceMethod::IDD)
{
}

CDlg_EvidenceMethod::~CDlg_EvidenceMethod()
{
}

void CDlg_EvidenceMethod::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrlEnvidecce);
}


BEGIN_MESSAGE_MAP(CDlg_EvidenceMethod, CPropertyPage)
	ON_BN_CLICKED(IDC_ENVIDENCE, &CDlg_EvidenceMethod::OnBnClickedEnvidence)
	ON_BN_CLICKED(IDC_ENVIDENCEOUT, &CDlg_EvidenceMethod::OnBnClickedEnvidenceout)
	ON_BN_CLICKED(IDC_POSTERIORPROBABILITY, &CDlg_EvidenceMethod::OnBnClickedPosteriorprobability)
	ON_BN_CLICKED(IDC_DELETEENVDENCE, &CDlg_EvidenceMethod::OnBnClickedDeleteenvdence)
	//ON_BN_CLICKED(IDC_DELETEENVDENCE2, &CDlg_EvidenceMethod::OnBnClickedDeleteenvdence2)
	ON_BN_CLICKED(IDC_INDEPENDENCETEST, &CDlg_EvidenceMethod::OnBnClickedIndependencetest)
	ON_BN_CLICKED(IDC_DataStatistics, &CDlg_EvidenceMethod::OnBnClickedDatastatistics)
END_MESSAGE_MAP()

// CDlg_EvidenceMethod 消息处理程序

BOOL CDlg_EvidenceMethod::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 获取向导
	CPropertySheet *Wizard = reinterpret_cast<CPropertySheet *>(GetParent());
	pDlgPriorProbability= (CDlg_PriorProbability *)Wizard->GetPage(0);

	//long nField;
 	int nItem = pDlgPriorProbability->m_ListBox2.GetCount();
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
 		m_ListCtrlEnvidecce.InsertItem(LVIF_TEXT|LVIF_STATE,i, str,0,LVIS_SELECTED,0,0);
 	}

	//初始化listctrl列名
	m_ListCtrlEnvidecce.InsertColumn( 0, _T("有利因子"), LVCFMT_LEFT, 150 );
	m_ListCtrlEnvidecce.InsertColumn( 1, _T("正权重值(W+)"), LVCFMT_LEFT, 150 );
	m_ListCtrlEnvidecce.InsertColumn( 2, _T("负权重值(W-)"), LVCFMT_LEFT, 150 );
	m_ListCtrlEnvidecce.InsertColumn( 3, _T("综合权重(C)"), LVCFMT_LEFT, 150 );
	m_ListCtrlEnvidecce.InsertColumn( 4, _T("相对权重(C)"), LVCFMT_LEFT, 150 );

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


BOOL CDlg_EvidenceMethod::OnSetActive()
{	
	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);  // 设置上一步按钮可用
	return CPropertyPage::OnSetActive();
}

BOOL CDlg_EvidenceMethod::OnApply()
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

LRESULT CDlg_EvidenceMethod::OnWizardNext()
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



//计算正权值
float EnvidenceMethodWPlus(long nGrid,long nMine,long nEnvidence,long nMineCrossEnvidence)
{
	float W;
	float a = (float)nMineCrossEnvidence/(float)nMine;
	float b = (float)(nEnvidence - nMineCrossEnvidence)/(float)(nGrid - nMine);
	float n = (float)a/(float)b;
	W = log((long double)n);
	return W;
}
//计算负权值
float EnvidenceMethodWMinus(long nGrid,long nMine,long nEnvidence,long nMineCrossEnvidence)
{
	float W;
	float a = (float)(nMine-nMineCrossEnvidence)/(float)nMine;
	float b = (float)(nGrid - nEnvidence - nMine + nMineCrossEnvidence)/(float)(nGrid - nMine);
	float n = (float)a/(float)b;
	W = log((long double)n);
	return W;
}

void CDlg_EvidenceMethod::OnBnClickedEnvidence()
{
	// TODO: 在此添加控件通知处理程序代码
	//计算各因子证据权值
	//研究区总网格数
	long nu,nv,nw;
	pDlgPriorProbability->m_pVoxet->get_nU(&nu);
	pDlgPriorProbability->m_pVoxet->get_nV(&nv);
	pDlgPriorProbability->m_pVoxet->get_nW(&nw);	
	long nGridCount = pDlgPriorProbability->ResearchAreaTrueIndex.size();

	//含矿点网格数
	long nMineCount = pDlgPriorProbability->MineTrueIndex.size();
	//不含矿点网格数
	long nNoneMineCount = nGridCount - nMineCount;

	//各因子所含网格数，计算证据权值
	int nItem = pDlgPriorProbability->m_ListBox2.GetCount();
	if(m_items.size() == 0)
	{
		for(int i = 0; i < nItem;i++)
			m_items.push_back(i);
	}
	std::vector<float> C;
	float Cplus = 0;
	for (int SingleItem = 0; SingleItem<m_items.size(); SingleItem++)
	{
		long nEnvidenceCount = pDlgPriorProbability->EnvidenceTrueIndex[m_items.at(SingleItem)].size();
		long nMineCrossEnvidenceCount = pDlgPriorProbability->ptT_T[m_items.at(SingleItem)];

		//对每一个因子计算正负权值
		float wplus = EnvidenceMethodWPlus(nGridCount,nMineCount,nEnvidenceCount,nMineCrossEnvidenceCount);
		float wminus = EnvidenceMethodWMinus(nGridCount,nMineCount,nEnvidenceCount,nMineCrossEnvidenceCount);
		float Counterpoise = wplus - wminus;
		C.push_back(Counterpoise);
		Cplus+=Counterpoise;

		//显示
		char* buffer5 = new char[30];
		char* buffer6 = new char[30];
		char* buffer7 = new char[30];
		//char* buffer4 = new char[30];
		//float PTtruetrue = 333.333f;
		sprintf(buffer5,"%8.6f",wplus);
		sprintf(buffer6,"%8.6f",wminus);
		sprintf(buffer7,"%8.6f",Counterpoise);
	
		CString tostrTT1 = buffer5;
		CString tostrTF2 = buffer6;
		CString tostrFT3 = buffer7;
		
		m_ListCtrlEnvidecce.SetItemText(SingleItem , 1 , (LPCTSTR)tostrTT1);
		m_ListCtrlEnvidecce.SetItemText(SingleItem , 2 ,(LPCTSTR)tostrTF2);
		m_ListCtrlEnvidecce.SetItemText(SingleItem , 3 , (LPCTSTR)tostrFT3);
		
	}

	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{
		float C_normalization = C[SingleItem]/Cplus;
		char* buffer8 = new char[30];
		sprintf(buffer8,"%8.6f",C_normalization);
		CString tostrFF4 = buffer8;
		m_ListCtrlEnvidecce.SetItemText(SingleItem ,4 , (LPCTSTR)tostrFF4);
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

void CDlg_EvidenceMethod::OnBnClickedEnvidenceout()
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

	std::vector<CString> ColumnVec;
	ColumnVec.push_back(_T("A"));
	ColumnVec.push_back(_T("B"));
	ColumnVec.push_back(_T("C"));
	ColumnVec.push_back(_T("D"));
	ColumnVec.push_back(_T("E"));

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
	adf.Format(_T("%s%d"),ColumnVec[0], startRow);
	excel.SetText(adf,_T("证据因子"));
	adf.Format(_T("%s%d"),ColumnVec[1], startRow);//
	excel.SetText(adf,_T("正权值W+"));
	adf.Format(_T("%s%d"),ColumnVec[2], startRow);
	excel.SetText(adf,_T("负权值W-"));
	adf.Format(_T("%s%d"),ColumnVec[3], startRow);
	excel.SetText(adf,_T("综合权重C"));
	adf.Format(_T("%s%d"),ColumnVec[4], startRow);
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
		adf.Format(_T("%s%d"),ColumnVec[0], startRow+i);
		excel.SetText(adf,strListData);
	}
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,1,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrlEnvidecce.GetItemText(i,1);
		adf.Format(_T("%s%d"),ColumnVec[1], startRow+i);
		excel.SetText(adf,strListData);
	}
	
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,2,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrlEnvidecce.GetItemText(i,2);
		adf.Format(_T("%s%d"),ColumnVec[2], startRow+i);
		excel.SetText(adf,strListData);
	}
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,3,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrlEnvidecce.GetItemText(i,3);
		adf.Format(_T("%s%d"),ColumnVec[3], startRow+i);
		excel.SetText(adf,strListData);
	}
	for (int i=0;i<nItemCount;i++)
	{
		startRow = 2;
		m_ListCtrlEnvidecce.GetSubItemRect(i,4,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrlEnvidecce.GetItemText(i,4);
		adf.Format(_T("%s%d"),ColumnVec[4], startRow+i);
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


void CDlg_EvidenceMethod::OnBnClickedPosteriorprobability()
{	
	//后验概率计算
	//给每一个网格单元赋值，f(x) = a1*x1+a2*x2+...
	IcwPropertyTablePtr pPropertyTable;
	pDlgPriorProbability->m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);
	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);
	if (pFields == NULL)
		return;
	CComBSTR name = _T("后验概率");
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

	//对每一个因子，获取先验概率和证据权值
	std::vector<float> itemprior;
	std::vector<float> itemenvidence;
	int itemcount = pDlgPriorProbability->m_ListCtrl.GetItemCount();
	for(int nitem = 0;nitem<itemcount; nitem++)
	{
		CString stritemprior = pDlgPriorProbability->m_ListCtrl.GetItemText(nitem,1);
		CString stritemevidence = m_ListCtrlEnvidecce.GetItemText(nitem,4);
		float f1= _wtof(stritemprior);
		float f2= _wtof(stritemevidence);
		/*f1 = 1.2*(nitem+1);
		f2 = 1.5*(nitem+1);*/
		itemprior.push_back(f1);	
		itemenvidence.push_back(f2);
	}

	//寻找证据因子网格index
	IcwColumnDataPtr pColumnData;
	IcwBigColumnData3DPtr pBigColumnData3D;
	for (int SingleItem = 0; SingleItem<itemcount; SingleItem++)
	{
		std::vector<long> singvec;
		CString str;
		pDlgPriorProbability->m_ListBox2.GetText(SingleItem,str);
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
		EvidenceExist.push_back(singvec);
	
		std::vector<long>().swap(singvec);//释放内存		
	}
	//取后验概率字段
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
				for (int SingleItem = 0; SingleItem<itemcount; SingleItem++)
				{
					std::vector<long>::iterator result = find(EvidenceExist[SingleItem].begin(),EvidenceExist[SingleItem].end(),voxetindex);
					if (result == EvidenceExist[SingleItem].end())
					{
						//没找到因子
					}
					else
					{
						//找到因子
						FillValue.fltVal += itemprior[SingleItem]*itemenvidence[SingleItem];//f(x) = a1x1+a2x2+a3x3+...
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

void CDlg_EvidenceMethod::OnBnClickedDeleteenvdence()
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
		std::vector<int>::iterator it = m_items.begin() + idx;
		m_items.erase(it);
		m_ListCtrlEnvidecce.DeleteItem(idx);
		idx = m_ListCtrlEnvidecce.GetNextSelectedItem(p); 
	}
	m_ListCtrlEnvidecce.Arrange(LVA_DEFAULT);  
	m_ListCtrlEnvidecce.RedrawItems(0, m_ListCtrlEnvidecce.GetItemCount());
}

void CDlg_EvidenceMethod::OnBnClickedIndependencetest()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlg_IndependenceTest dlg;
	dlg.pVoxet = pDlgPriorProbability->m_pVoxet;	
	dlg.DoModal();
}


void CDlg_EvidenceMethod::OnBnClickedDatastatistics()
{
	// TODO: 在此添加控件通知处理程序代码
	IcwPropertyTablePtr pPropertyTable;
	pDlgPriorProbability->m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);
	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);
	if (pFields == NULL)
		return;

	//创建对话框
	CDlg_DataStatistics *m_voxDlg = new CDlg_DataStatistics;
	m_voxDlg->Create(CDlg_DataStatistics::IDD, NULL);
	//初始化ComBox
	long fieldCount;
	CComBSTR name = _T("");
	pFields->get_FieldCount(&fieldCount);
	for(long index = 0; index < fieldCount; ++index)
	{
		pFields->get_Field(index,&pField);
		pField->get_Name(&name);
		((CComboBox*)m_voxDlg->GetDlgItem(IDC_COMBO1))->AddString(name);
		((CComboBox*)m_voxDlg->GetDlgItem(IDC_COMBO2))->AddString(name);
		((CComboBox*)m_voxDlg->GetDlgItem(IDC_COMBO3))->AddString(name);
	}
	m_voxDlg->m_pVoxet = pDlgPriorProbability->m_pVoxet;
	m_voxDlg->ShowWindow(SW_SHOW);
}
