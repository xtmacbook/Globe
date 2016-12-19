// DlgVoxetStatistics.cpp : 实现文件
//

#include "stdafx.h"
#include "Dlg_VoxetStatisticsInformation.h"
#include "ExcelMgr.h"


#include "CApplication.h"
#include "CFont0.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "CChart.h"
#include "CCharts.h"

// CDlgVoxetStatisticsInformation 对话框

IMPLEMENT_DYNAMIC(CDlgVoxetStatisticsInformation, CDialog)

CDlgVoxetStatisticsInformation::CDlgVoxetStatisticsInformation(CWnd* pParent /*=NULL*/)
: CDialog(CDlgVoxetStatisticsInformation::IDD, pParent)
{

}
CDlgVoxetStatisticsInformation::~CDlgVoxetStatisticsInformation()
{
}

void CDlgVoxetStatisticsInformation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_Statistics);
}


BEGIN_MESSAGE_MAP(CDlgVoxetStatisticsInformation, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgVoxetStatisticsInformation::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgVoxetStatisticsInformation::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDlgVoxetStatistics 消息处理程序
BOOL CDlgVoxetStatisticsInformation::OnInitDialog()
{	
	CDialog::OnInitDialog();

	CPropertySheet *Wizard = reinterpret_cast<CPropertySheet *>(GetParent());
	m_prior= (CDlg_InformationCalculate *)Wizard->GetPage(0);

	m_Statistics.InsertColumn( 0, _T("找矿标志"), LVCFMT_LEFT, 80 );
	m_Statistics.InsertColumn( 1, _T("标志所占网格数"), LVCFMT_LEFT, 110 );
	m_Statistics.InsertColumn( 2, _T("标志内含矿网格数"), LVCFMT_LEFT, 110 );	
	m_Statistics.InsertColumn( 3, _T("研究区网格总数"), LVCFMT_LEFT, 110 );	

	return TRUE;
}
void CDlgVoxetStatisticsInformation::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	std::vector<CString> envidenceName;
	int nItem = m_prior->m_ListBox2.GetCount();//得到行数
	for (int i = 0;i<nItem;i++)
	{
		CString str;
		m_prior->m_ListBox2.GetText(i,str);
		m_Statistics.InsertItem(LVIF_TEXT|LVIF_STATE,i, str,0,LVIS_SELECTED,0,0);
	}	
	//m_Statistics.InsertItem(LVIF_TEXT|LVIF_STATE,nItem, _T("网格总数"),0,LVIS_SELECTED,0,0);

	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{
		//int EvidenceExistCount = m_prior->ptF_T[SingleItem]+m_prior->ptT_T[SingleItem];	
		//int EvidenceExistMineCount = m_prior->ptT_T[SingleItem];

		//long SumGrid = m_prior->ResearchAreaTrueIndex.size();

		CString tostrEEC;
		CString tostrEEMC;
		CString strSumGrid;

		//tostrEEC.Format(_T("%d"),EvidenceExistCount); 
		//tostrEEMC.Format(_T("%d"),EvidenceExistMineCount);
		//strSumGrid.Format(_T("%d"),SumGrid);

		////显示每个因子先验概率值
		////SetItemText(行号，列号，先验概率值)
		//m_Statistics.SetItemText(SingleItem , 1 , (LPCTSTR)tostrEEC);
		//m_Statistics.SetItemText(SingleItem , 2 ,(LPCTSTR)tostrEEMC);
		//m_Statistics.SetItemText(SingleItem , 3 ,(LPCTSTR)strSumGrid);
	}
}

void FillSafeArray2(OLECHAR FAR* sz, int iRow, int iCol,
					COleSafeArray* sa)
{
	VARIANT v;
	long index[2];

	index[0] = iRow;
	index[1] = iCol;

	VariantInit(&v);
	v.vt = VT_BSTR;
	v.bstrVal = SysAllocString(sz);
	sa->PutElement(index, v.bstrVal);
	SysFreeString(v.bstrVal);
	VariantClear(&v);

}

void CDlgVoxetStatisticsInformation::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	CString FilePathName;
	CFileDialog Dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("excel Files(*.xls)|*.xls"));	
	if(Dlg.DoModal() == IDOK)
	{
		FilePathName =  Dlg.GetPathName();		
	}
	else
	{		
		AfxMessageBox(_T("打开失败"),NULL,MB_OK);
	}

	std::vector<CString> ColumnVec;
	ColumnVec.push_back(_T("A"));
	ColumnVec.push_back(_T("B"));
	ColumnVec.push_back(_T("C"));
	ColumnVec.push_back(_T("D"));
	ColumnVec.push_back(_T("E"));	

	ExcelMgr excel; 
	excel.CreateExcel(FilePathName);	
	excel.SetCurSheet(1);	
	excel.SetSheetName(_T("网格统计"));

	int startRow = 1;
	CString adf;
	adf.Format(_T("%s%d"),ColumnVec[0], startRow);
	excel.SetText(adf,_T("找矿标志"));
	adf.Format(_T("%s%d"),ColumnVec[1], startRow);//
	excel.SetText(adf,_T("标志所占网格数"));
	adf.Format(_T("%s%d"),ColumnVec[2], startRow);
	excel.SetText(adf,_T("标志内含矿网格数"));

	int nItemCount,nColumnCount;
	nColumnCount = m_Statistics.GetHeaderCtrl()->GetItemCount();//得到列数
	nItemCount = m_Statistics.GetItemCount();//得到行数

	CString strListData;
	CRect rc;
	for (int j=0;j<nColumnCount;j++)
	{
		for (int i=0;i<nItemCount;i++)
		{
			startRow = 2;
			m_Statistics.GetSubItemRect(i,j,LVIR_LABEL,rc);//获得子项的RECT；
			strListData=m_Statistics.GetItemText(i,j);
			adf.Format(_T("%s%d"),ColumnVec[j], startRow+i);
			excel.SetText(adf,strListData);
		}
	}


	// Commonly used OLE variants.
	COleVariant
		covTrue((short)TRUE),
		covFalse((short)FALSE),
		covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	CApplication app;

	// Start Excel and get an Application object.
	if(!app.CreateDispatch(TEXT("Excel.Application")))
	{
		AfxMessageBox(TEXT("Couldn't start Excel and get Application object."));
		return;
	}
	CWorkbooks books;
	CWorkbook book;
	CWorksheets sheets;
	CWorksheet sheet;
	CRange range;
	CFont0 font;

	books = app.get_Workbooks();
	book = books.Add (covOptional);

	//Get the first sheet.
	sheets =book.get_Sheets();
	sheet = sheets.get_Item(COleVariant((short)1));

	range = sheet.get_Range(COleVariant(TEXT("A1")),COleVariant(TEXT("A1")));
	range.put_Value2(COleVariant(TEXT("网格统计")));
	range = sheet.get_Range(COleVariant(TEXT("A1")),COleVariant(TEXT("C1")));
	range.Merge(covOptional);

	range = sheet.get_Range(COleVariant(TEXT("B2")),COleVariant(TEXT("B2")));
	range.put_Value2(COleVariant(TEXT("标志所占网格数")));

	range = sheet.get_Range(COleVariant(TEXT("C2")),COleVariant(TEXT("C2")));
	range.put_Value2(COleVariant(TEXT("标志内含矿网格数")));

	//range = sheet.get_Range(COleVariant(TEXT("C2")),COleVariant(TEXT("C2")));
	//range.put_Value2(COleVariant(TEXT("研究区网格数")));


	//Fill A3:A6 with an array of values (Months).
	COleSafeArray saRet;
	DWORD numElements[]={4,1};   //4x1 element array
	saRet.Create(VT_BSTR, 2, numElements);

	std::vector<OLECHAR*> strvec;
	for (int i=0;i<nItemCount;i++)
	{
		m_Statistics.GetSubItemRect(i,0,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_Statistics.GetItemText(i,0);
		strvec.push_back(strListData.AllocSysString());	
	}
	FillSafeArray2(strvec[0], 0, 0, &saRet);
	FillSafeArray2(strvec[1], 1, 0, &saRet);
	FillSafeArray2(strvec[2], 2, 0, &saRet);
	FillSafeArray2(strvec[3], 3, 0, &saRet);

	range = sheet.get_Range(COleVariant(TEXT("A3")), COleVariant(TEXT("A6")));
	range.put_Value2(COleVariant(saRet));
	saRet.Detach();

	//CString strListData;
	//CRect rc;
	float flo;
	std::vector<float> vec;
	for(int j = 1;j<3;j++)
	{
		for (int i=0;i<nItemCount;i++)
		{
			m_Statistics.GetSubItemRect(i,j,LVIR_LABEL,rc);//获得子项的RECT；
			strListData=m_Statistics.GetItemText(i,j);

			flo = _tstof(strListData);
			//strListData.ReleaseBuffer();
			vec.push_back(flo);
		}

	}



	//Fill B3:C6 with values
	range = sheet.get_Range(COleVariant(TEXT("B3")),COleVariant(TEXT("B3")));
	range.put_Value2(COleVariant(float(vec[0])));
	range = sheet.get_Range(COleVariant(TEXT("B4")),COleVariant(TEXT("B4")));
	range.put_Value2(COleVariant(float(vec[1])));
	range = sheet.get_Range(COleVariant(TEXT("B5")),COleVariant(TEXT("B5")));
	range.put_Value2(COleVariant(float(vec[2])));
	range = sheet.get_Range(COleVariant(TEXT("B6")),COleVariant(TEXT("B6")));
	range.put_Value2(COleVariant(float(vec[3])));
	range = sheet.get_Range(COleVariant(TEXT("C3")),COleVariant(TEXT("C3")));
	range.put_Value2(COleVariant(short(vec[4])));
	range = sheet.get_Range(COleVariant(TEXT("C4")),COleVariant(TEXT("C4")));
	range.put_Value2(COleVariant(short(vec[5])));
	range = sheet.get_Range(COleVariant(TEXT("C5")),COleVariant(TEXT("C5")));
	range.put_Value2(COleVariant(short(vec[6])));
	range = sheet.get_Range(COleVariant(TEXT("C6")),COleVariant(TEXT("C6")));
	range.put_Value2(COleVariant(short(vec[7])));


	//Format A1:C1 as bold, vertical alignment = center.
	range = sheet.get_Range(COleVariant(TEXT("A1")), COleVariant(TEXT("C1")));
	font = range.get_Font();
	font.put_Bold(covTrue);
	range.put_VerticalAlignment(COleVariant((short)-4108));   //xlVAlignCenter = -4108

	//AutoFit columns A:D.
	range = sheet.get_Range(COleVariant(TEXT("A1")), COleVariant(TEXT("D1")));
	CRange cols;
	cols = range.get_EntireColumn();
	cols.AutoFit();

	//Adding Chart
	CCharts charts;
	CChart chart;
	charts = book.get_Charts();
	chart = charts.Add(covOptional, covOptional, covOptional);

	app.put_Visible(TRUE);
	app.put_UserControl(TRUE);
}
