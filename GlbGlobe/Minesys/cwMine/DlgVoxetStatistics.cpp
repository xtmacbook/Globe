// DlgVoxetStatistics.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DlgVoxetStatistics.h"
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

// CDlgVoxetStatistics �Ի���

IMPLEMENT_DYNAMIC(CDlgVoxetStatistics, CDialog)

CDlgVoxetStatistics::CDlgVoxetStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVoxetStatistics::IDD, pParent)
{

}
CDlgVoxetStatistics::~CDlgVoxetStatistics()
{
}

void CDlgVoxetStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_Statistics);
}


BEGIN_MESSAGE_MAP(CDlgVoxetStatistics, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgVoxetStatistics::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgVoxetStatistics::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDlgVoxetStatistics ��Ϣ�������
BOOL CDlgVoxetStatistics::OnInitDialog()
{	
	CDialog::OnInitDialog();

	CPropertySheet *Wizard = reinterpret_cast<CPropertySheet *>(GetParent());
	m_prior= (CDlg_PriorProbability *)Wizard->GetPage(0);

	m_Statistics.InsertColumn( 0, _T("�ҿ��־"), LVCFMT_LEFT, 80 );
	m_Statistics.InsertColumn( 1, _T("��־��ռ������"), LVCFMT_LEFT, 110 );
	m_Statistics.InsertColumn( 2, _T("��־�ں���������"), LVCFMT_LEFT, 110 );	
	m_Statistics.InsertColumn( 3, _T("�о�����������"), LVCFMT_LEFT, 110 );	

	return TRUE;
}
void CDlgVoxetStatistics::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Statistics.DeleteAllItems();
	std::vector<CString> envidenceName;
	int nItem = m_prior->m_ListBox2.GetCount();//�õ�����
	for (int i = 0;i<nItem;i++)
	{
		CString str;
		m_prior->m_ListBox2.GetText(i,str);
		m_Statistics.InsertItem(LVIF_TEXT|LVIF_STATE,i, str,0,LVIS_SELECTED,0,0);
	}	
	//m_Statistics.InsertItem(LVIF_TEXT|LVIF_STATE,nItem, _T("��������"),0,LVIS_SELECTED,0,0);

	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{
		int EvidenceExistCount = m_prior->ptF_T[SingleItem]+m_prior->ptT_T[SingleItem];	
		int EvidenceExistMineCount = m_prior->ptT_T[SingleItem];

		long SumGrid = m_prior->ResearchAreaTrueIndex.size();
		
		CString tostrEEC;
		CString tostrEEMC;
		CString strSumGrid;

		tostrEEC.Format(_T("%d"),EvidenceExistCount); 
		tostrEEMC.Format(_T("%d"),EvidenceExistMineCount);
		strSumGrid.Format(_T("%d"),SumGrid);

		//��ʾÿ�������������ֵ
		//SetItemText(�кţ��кţ��������ֵ)
		m_Statistics.SetItemText(SingleItem , 1 , (LPCTSTR)tostrEEC);
		m_Statistics.SetItemText(SingleItem , 2 ,(LPCTSTR)tostrEEMC);
		m_Statistics.SetItemText(SingleItem , 3 ,(LPCTSTR)strSumGrid);
	}
}

void FillSafeArray1(OLECHAR FAR* sz, int iRow, int iCol,
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

void FillSafeArray4(OLECHAR FAR* sz, int iRow, int iCol,
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

void FillSafeArray4_float(float sz, int iRow, int iCol,
	COleSafeArray* sa)
{
	VARIANT v;
	long index[2];

	index[0] = iRow;
	index[1] = iCol;

	VariantInit(&v);
	v.vt = VT_R4;
	v.fltVal = sz;
	sa->PutElement(index, &v.fltVal);
	//SysFreeString(v.fltVal);
	VariantClear(&v);
}

void CDlgVoxetStatistics::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString FilePathName;
	CFileDialog Dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("excel Files(*.xls)|*.xls"));	
	if(Dlg.DoModal() == IDOK)
	{
		FilePathName =  Dlg.GetPathName();		
	}
	else
	{		
		AfxMessageBox(_T("��ʧ��"),NULL,MB_OK);
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
	excel.SetSheetName(_T("����ͳ��"));

	int startRow = 1;
	CString adf;
	adf.Format(_T("%s%d"),ColumnVec[0], startRow);
	excel.SetText(adf,_T("�ҿ��־"));
	adf.Format(_T("%s%d"),ColumnVec[1], startRow);//
	excel.SetText(adf,_T("��־��ռ������"));
	adf.Format(_T("%s%d"),ColumnVec[2], startRow);
	excel.SetText(adf,_T("��־�ں���������"));
	
	int nItemCount,nColumnCount;
	nColumnCount = m_Statistics.GetHeaderCtrl()->GetItemCount();//�õ�����
	nItemCount = m_Statistics.GetItemCount();//�õ�����

	CString strListData;
	CRect rc;
	for (int j=0;j<nColumnCount;j++)
	{
		for (int i=0;i<nItemCount;i++)
		{
			startRow = 2;
			m_Statistics.GetSubItemRect(i,j,LVIR_LABEL,rc);//��������RECT��
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
	range.put_Value2(COleVariant(TEXT("����ͳ��")));
	range = sheet.get_Range(COleVariant(TEXT("A1")),COleVariant(TEXT("C1")));
	range.Merge(covOptional);

	range = sheet.get_Range(COleVariant(TEXT("B2")),COleVariant(TEXT("B2")));
	range.put_Value2(COleVariant(TEXT("��־��ռ������")));

	range = sheet.get_Range(COleVariant(TEXT("C2")),COleVariant(TEXT("C2")));
	range.put_Value2(COleVariant(TEXT("��־�ں���������")));

	//range = sheet.get_Range(COleVariant(TEXT("C2")),COleVariant(TEXT("C2")));
	//range.put_Value2(COleVariant(TEXT("�о���������")));


	//Fill A3:A6 with an array of values (Months).
	COleSafeArray saRet;
	DWORD numElements[]={nItemCount,1};   //4x1 element array
	saRet.Create(VT_BSTR, 2, numElements);

	std::vector<OLECHAR*> strvec;
	for (int i=0;i<nItemCount;i++)
	{
		m_Statistics.GetSubItemRect(i,0,LVIR_LABEL,rc);//��������RECT��
		strListData=m_Statistics.GetItemText(i,0);
		strvec.push_back(strListData.AllocSysString());	
		FillSafeArray4(strvec[i],i,0,&saRet);
	}

	CString indexStr = "A";
	CString itemStr;	
	itemStr.Format(_T("%d"), nItemCount + 2);
	indexStr.Append(itemStr);
	range = sheet.get_Range(COleVariant(TEXT("A3")), COleVariant(indexStr));
	range.put_Value2(COleVariant(saRet));
	saRet.Detach();

	saRet.Create(VT_R4,2,numElements);
	float flo;
	std::vector<float> vec;
	for (int i=0;i<nItemCount;i++)
	{
		m_Statistics.GetSubItemRect(i,1,LVIR_LABEL,rc);//��������RECT��
		strListData=m_Statistics.GetItemText(i,1);

		flo = _tstof(strListData);
		vec.push_back(flo);
		FillSafeArray4_float(vec[i], i, 0, &saRet);
	}
	indexStr = "B";
	itemStr.Format(_T("%d"), nItemCount + 2);
	indexStr.Append(itemStr);
	range = sheet.get_Range(COleVariant(TEXT("B3")), COleVariant(indexStr));
	range.put_Value2(COleVariant(saRet));
	saRet.Detach();

	saRet.Create(VT_R4,2,numElements);
	vec.clear();
	for (int i=0;i<nItemCount;i++)
	{
		m_Statistics.GetSubItemRect(i,2,LVIR_LABEL,rc);//��������RECT��
		strListData=m_Statistics.GetItemText(i,2);

		flo = _tstof(strListData);
		vec.push_back(flo);
		FillSafeArray4_float(vec[i], i, 0, &saRet);
	}
	indexStr = "C";
	itemStr.Format(_T("%d"), nItemCount + 2);
	indexStr.Append(itemStr);
	range = sheet.get_Range(COleVariant(TEXT("C3")), COleVariant(indexStr));
	range.put_Value2(COleVariant(saRet));
	saRet.Detach();

	//saRet.Create(VT_R4,2,numElements);
	//vec.clear();
	//for (int i=0;i<nItemCount;i++)
	//{
	//	m_Statistics.GetSubItemRect(i,3,LVIR_LABEL,rc);//��������RECT��
	//	strListData=m_Statistics.GetItemText(i,3);

	//	flo = _tstof(strListData);
	//	vec.push_back(flo);
	//	FillSafeArray4_float(vec[i], i, 0, &saRet);
	//}
	//indexStr = "D";
	//itemStr.Format(_T("%d"), nItemCount + 2);
	//indexStr.Append(itemStr);
	//range = sheet.get_Range(COleVariant(TEXT("D3")), COleVariant(indexStr));
	//range.put_Value2(COleVariant(saRet));
	//saRet.Detach();


	//Fill B3:C6 with values
	//range = sheet.get_Range(COleVariant(TEXT("B3")),COleVariant(TEXT("B3")));
	//range.put_Value2(COleVariant(float(vec[0])));
	//range = sheet.get_Range(COleVariant(TEXT("B4")),COleVariant(TEXT("B4")));
	//range.put_Value2(COleVariant(float(vec[1])));
	//range = sheet.get_Range(COleVariant(TEXT("B5")),COleVariant(TEXT("B5")));
	//range.put_Value2(COleVariant(float(vec[2])));
	//range = sheet.get_Range(COleVariant(TEXT("B6")),COleVariant(TEXT("B6")));
	//range.put_Value2(COleVariant(float(vec[3])));
	//range = sheet.get_Range(COleVariant(TEXT("C3")),COleVariant(TEXT("C3")));
	//range.put_Value2(COleVariant(short(vec[4])));
	//range = sheet.get_Range(COleVariant(TEXT("C4")),COleVariant(TEXT("C4")));
	//range.put_Value2(COleVariant(short(vec[5])));
	//range = sheet.get_Range(COleVariant(TEXT("C5")),COleVariant(TEXT("C5")));
	//range.put_Value2(COleVariant(short(vec[6])));
	//range = sheet.get_Range(COleVariant(TEXT("C6")),COleVariant(TEXT("C6")));
	//range.put_Value2(COleVariant(short(vec[7])));


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
