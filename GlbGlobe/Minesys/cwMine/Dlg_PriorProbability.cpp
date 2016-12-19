// Dlg_PriorProbability.cpp : 实现文件
//

#include "stdafx.h"
#include "Dlg_PriorProbability.h"
#include "Dlg_EvidenceMethod.h"
#include "UsefulUtility/DlgInput.h"
#include <vector>
#include <fstream>
#include <map>
#include "ExcelMgr.h"
#include "DlgVoxetStatistics.h"

#include "CApplication.h"
#include "CFont0.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "CChart.h"
#include "CCharts.h"


// CDlg_PriorProbability 对话框

IMPLEMENT_DYNAMIC(CDlg_PriorProbability, CPropertyPage)

CDlg_PriorProbability::CDlg_PriorProbability()
	: CPropertyPage(CDlg_PriorProbability::IDD)
	, m_ObjNames(_T(""))
{
	m_ObjNames=_T("");
}

CDlg_PriorProbability::~CDlg_PriorProbability()
{

}

void CDlg_PriorProbability::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_LIST2, m_ListBox1);
	DDX_Control(pDX, IDC_LIST3, m_ListBox2);	
	DDX_Control(pDX, IDC_LIST7, m_ListCtrl);
	DDX_Control(pDX, IDC_COMBO_MINE, m_ComboBoxMine);
	DDX_Control(pDX, IDC_COMBO_RESEARCHAREA, m_ComboBoxResearchArea);
}


BEGIN_MESSAGE_MAP(CDlg_PriorProbability, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlg_PriorProbability::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlg_PriorProbability::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlg_PriorProbability::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlg_PriorProbability::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDlg_PriorProbability::OnBnClickedButton5)
	ON_CBN_SELCHANGE(IDC_COMBO_MINE, &CDlg_PriorProbability::OnCbnSelchangeComboMine)
	ON_BN_CLICKED(IDC_PRIORPROBABILITYOUT, &CDlg_PriorProbability::OnBnClickedPriorprobabilityout)
	ON_BN_CLICKED(IDC_VOXETStatistics, &CDlg_PriorProbability::OnBnClickedVoxetstatistics)
	ON_CBN_SELCHANGE(IDC_COMBO_RESEARCHAREA, &CDlg_PriorProbability::OnCbnSelchangeComboResearcharea)
END_MESSAGE_MAP()


// CDlg_PriorProbability 消息处理程序

BOOL CDlg_PriorProbability::OnInitDialog()
{	
    CPropertyPage::OnInitDialog();
	 
	IcwPropertyTablePtr pPropertyTable;
	m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);

	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);
	if (pFields == NULL)
		return true;
	long nField;
	IcwFieldPtr pOneField;
	pFields->get_FieldCount(&nField);
	CComBSTR FieldName;	
	for (long i = 0;i<nField;i++)
	{
		pFields->get_Field(i,&pOneField);
		pOneField->get_Name(&FieldName);
		m_ComboBoxMine.InsertString(i,FieldName);
		m_ComboBoxResearchArea.InsertString(i,FieldName);
	}
	//for (long i = 0;i<nField;i++)
	//{
	//	pFields->get_Field(i,&pOneField);
	//	pOneField->get_Name(&FieldName);
	//	m_ListBox1.AddString(FieldName);
	//}

	//初始化listctrl列名
	m_ListCtrl.InsertColumn( 0, _T("有利因子"), LVCFMT_LEFT, 80 );
	m_ListCtrl.InsertColumn( 1, _T("证据在矿点在"), LVCFMT_LEFT, 110 );
	m_ListCtrl.InsertColumn( 2, _T("证据在矿点不在"), LVCFMT_LEFT, 110 );
	m_ListCtrl.InsertColumn( 3, _T("证据不在矿点在"), LVCFMT_LEFT, 110 );
	m_ListCtrl.InsertColumn( 4, _T("证据不在矿点不在"), LVCFMT_LEFT, 110 );
	return true;
}

BOOL CDlg_PriorProbability::OnSetActive()
{	
	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_NEXT);  // 设置下一步按钮可用
	return CPropertyPage::OnSetActive();
}

BOOL CDlg_PriorProbability::OnApply()
{	
	IcwWorkSpacePtr pWorkspace = NULL;
	CoCreateInstance(__uuidof(cwWorkSpace),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwWorkSpace),(void **)&pWorkspace);
	if (pWorkspace == NULL) return FALSE ;

	pWorkspace->UpdateAllViews();
	return CPropertyPage::OnApply();
}

LRESULT CDlg_PriorProbability::OnWizardNext()
{	
	return CPropertyPage::OnWizardNext();
}

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
		if(-1 == m_ListBox2.FindString(0,str))
			m_ListBox2.AddString(str);
	}
	OnBnClickedButton5();
}

void CDlg_PriorProbability::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	int nItem = m_ListBox1.GetCount();
	for (int i = 0;i<nItem;i++)
	{
		CString str;
		m_ListBox1.GetText(i,str);
		if(-1 == m_ListBox2.FindString(0,str))
			m_ListBox2.AddString(str);
	}
	OnBnClickedButton5();
}

void CDlg_PriorProbability::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码	
	int nItem = m_ListBox2.GetSelCount();
	int *SelIndex = new int[nItem];
	m_ListBox2.GetSelItems(nItem, SelIndex); 
	for (int i = 0;i<nItem;i++)
	{		
		m_ListBox2.DeleteString(SelIndex[i]-i);
	}
	OnBnClickedButton5();
}

void CDlg_PriorProbability::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码	
	int nItem = m_ListBox2.GetCount();
	for (int i = 0;i<nItem;i++)
	{
		m_ListBox2.DeleteString(0);
	}
	OnBnClickedButton5();
}

UINT64 GetIndex(long u,long v,long w,IcwVoxet * pVoxet)
{
	long nu,nv,nw;
	pVoxet->get_nU(&nu);
	pVoxet->get_nV(&nv);
	pVoxet->get_nW(&nw);
	if(u >= nu || v>=nv || w>=nw)
		return 0;
	return u * nv * nw + v * nw + w; 
}


void CDlg_PriorProbability::OnBnClickedButton5()//计算先验概率
{
	// TODO: 在此添加控件通知处理程序代码
	
	m_ListCtrl.DeleteAllItems();
	std::vector<CString> envidenceName;
	int nItem = m_ListBox2.GetCount();
	//for (int i = 0;i<nItem;i++)
	for (int i = nItem-1; i>=0;i--)
	{
		CString str;
		m_ListBox2.GetText(i,str);
		envidenceName.push_back(str);
		m_ListCtrl.InsertItem(LVIF_TEXT|LVIF_STATE,0, str,0,LVIS_SELECTED,0,0);		
	}

	//对每一个有利因子，获取属性值
	IcwPropertyTablePtr pPropertyTable;
	m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);
	long nu,nv,nw;
	m_pVoxet->get_nU(&nu);
	m_pVoxet->get_nV(&nv);
	m_pVoxet->get_nW(&nw);

	CString m_ResearchArea;	
	((CComboBox*)GetDlgItem(IDC_COMBO_RESEARCHAREA))->GetWindowText(m_ResearchArea);
	CComBSTR bstr = m_ResearchArea.AllocSysString();
	IcwColumnDataPtr pColumnData;
	pPropertyTable->GetColumn(bstr, &pColumnData);
	IcwBigColumnData3DPtr pBigColumnData3D = pColumnData;
	ASSERT(pBigColumnData3D);
	ResearchAreaTrueIndex.clear();	
	long voxetindex;	
	for (long u = 0;u<nu;u++)
	{
		long planeindex = 0;
		IcwDataPlanePtr pDataplane;
		HRESULT hr = pBigColumnData3D->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane);
		if(hr != S_OK)
			AfxMessageBox(_T("GetDataPlane出错!"));
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
				ResearchAreaValue.push_back(pfloatArray[planeindex]);
				if (pfloatArray[planeindex] == 1)
					ResearchAreaTrueIndex.push_back(voxetindex);//存在矿点的index
			}
		}	
	}
	
	CString m_MineName;	
	((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetWindowText(m_MineName);//取当前内容	
	CComBSTR bstrmine = m_MineName.AllocSysString();
	//取矿点字段
	IcwColumnDataPtr pColumnDataMine;
	pPropertyTable->GetColumn(bstrmine, &pColumnDataMine);
	pBigColumnData3D = pColumnDataMine;
	ASSERT(pBigColumnData3D);
	//清空矿点index
	MineTrueIndex.clear();
	MineFalseIndex.clear();
	voxetindex=0;
	for (long u = 0;u<nu;u++)
	{
		long planeindex = 0;		
		IcwDataPlanePtr pDataplane;
		HRESULT hr = pBigColumnData3D->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane);
		if(hr != S_OK)
			AfxMessageBox(_T("GetDataPlane出错!"));
		IcwDataArrayPtr pArray = pDataplane;	
		void * pbuffer;
		pArray->GetBuffer(&pbuffer);
		float * pfloatArray = NULL;
		pfloatArray = (float*)pbuffer;		
		//char buf[255];
		//sprintf(buf,"D:\\Deb\\planeafter_%d.txt",u);
		//std::ofstream ofs2(buf);
		for (long v = 0;v<nv;v++)
		{
			for (long w= 0;w<nw;w++)
			{
				planeindex = w+v*nw;
				voxetindex = u * nv * nw + v * nw + w;
				MineValVec.push_back(pfloatArray[planeindex]);//存体素矿点值			
				//long vava = pfloatArray[planeindex];
				//if(vava != 0)			
				//	ofs2<<pfloatArray[planeindex]<<" ";
				if (pfloatArray[planeindex] == 1.0 && ResearchAreaValue[voxetindex] == 1.0)
					MineTrueIndex.push_back(voxetindex);//存在矿点的index
				else if (pfloatArray[planeindex] == 0.0 && ResearchAreaValue[voxetindex] == 1.0)
					MineFalseIndex.push_back(voxetindex);//不存在矿点的index
			}
			//ofs2<<std::endl;
		}
		//ofs2.close();
	}

	//清空因子index
	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{
		EnvidenceTrueIndex.clear();
		EnvidenceFalseIndex.clear();
	}
	
	EnvidenceTrueIndex.resize(nItem);
	EnvidenceFalseIndex.resize(nItem);

	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{
		ptT_T.push_back(SingleItem);
		ptT_T[SingleItem] = 0;
		ptT_F.push_back(SingleItem);
		ptT_F[SingleItem] = 0;
		ptF_T.push_back(SingleItem);
		ptF_T[SingleItem] = 0;
		ptF_F.push_back(SingleItem);
		ptF_F[SingleItem] = 0;
	}	

	//对每一个因子计算条件概率值，遍历nItem遍把所有因子记录下来
	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{
		CString str;
		m_ListBox2.GetText(SingleItem,str);
		CComBSTR bstr = str.AllocSysString();
		IcwColumnDataPtr pColumnData;
		//取各个因子字段
		pPropertyTable->GetColumn(bstr, &pColumnData);
		pBigColumnData3D = pColumnData;		
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
					if (pfloatArray[planeindex] == 1 && ResearchAreaValue[voxetindex] == 1)
					{
						EnvidenceTrueIndex[SingleItem].push_back(voxetindex);
						if(MineValVec[voxetindex] == 1)
						{
							ptT_T[SingleItem]++;
						}
						else
						{
							ptF_T[SingleItem]++;
						}						
					}
					else if (pfloatArray[planeindex] == 0 && ResearchAreaValue[voxetindex] == 1)
					{
						EnvidenceFalseIndex[SingleItem].push_back(voxetindex);
						if (MineValVec[voxetindex] == 1)
						{
							ptT_F[SingleItem]++;
						}
						else
						{
							ptF_F[SingleItem]++;
						}
					}
				}
			}
		}
	}
	//计算条件概率
	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{		
		float PTtruetrue,PTtruefalse,PTfalsetrue,PTfalsefalse;
		long TT = ptT_T[SingleItem],size();
		long TF = ptT_F[SingleItem],size();
		long FT = ptF_T[SingleItem],size();
		long FF = ptF_F[SingleItem],size();
		long ETI = EnvidenceTrueIndex[SingleItem].size();
		long EFI = EnvidenceFalseIndex[SingleItem].size();

		PTtruetrue = (float)TT/(float)ETI;
		PTtruefalse = (float)TF/(float)EFI;
		PTfalsetrue = (float)FT/(float)ETI;
		PTfalsefalse = (float)FF/(float)EFI;
		
		char* buffer1 = new char[30];
		char* buffer2 = new char[30];
		char* buffer3 = new char[30];
		char* buffer4 = new char[30];		
		sprintf(buffer1,"%8.6f",PTtruetrue);
		sprintf(buffer2,"%8.6f",PTtruefalse);
		sprintf(buffer3,"%8.6f",PTfalsetrue);
		sprintf(buffer4,"%8.6f",PTfalsefalse);
		CString tostrTT = buffer1;
		CString tostrTF = buffer2;
		CString tostrFT = buffer3;
		CString tostrFF = buffer4;

		//显示每个因子先验概率值
		//SetItemText(行号，列号，先验概率值)
		m_ListCtrl.SetItemText(SingleItem , 1 , (LPCTSTR)tostrTT) ;
		m_ListCtrl.SetItemText(SingleItem , 2 ,(LPCTSTR)tostrTF) ;
		m_ListCtrl.SetItemText(SingleItem , 3 , (LPCTSTR)tostrFT) ;
		m_ListCtrl.SetItemText(SingleItem , 4 , (LPCTSTR)tostrFF) ;
	}	
}


void CDlg_PriorProbability::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码

	m_ListBox1.ResetContent();//可以删除列表框中所有行

	IcwPropertyTablePtr pPropertyTable;
	m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);

	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);	
	long nField;
	IcwFieldPtr pOneField;
	pFields->get_FieldCount(&nField);	
	CComBSTR FieldName;	
	for (long i = 0;i<nField;i++)
	{
		pFields->get_Field(i,&pOneField);
		pOneField->get_Name(&FieldName);
		m_ListBox1.AddString(FieldName);
	}

	int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetCurSel();//当前选中的行
	//CString strTemp;
	//((CComboBox*)GetDlgItem(IDC_COMBO1))->GetWindowText(strTemp);//取当前内容
	m_ListBox1.DeleteString(iPos);

}

void CDlg_PriorProbability::OnCbnSelchangeComboMine()
{
	// TODO: 在此添加控件通知处理程序代码

	m_ListBox1.ResetContent();//可以删除列表框中所有行

	IcwPropertyTablePtr pPropertyTable;
	m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);

	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);	
	long nField;
	IcwFieldPtr pOneField;
	pFields->get_FieldCount(&nField);	
	CComBSTR FieldName;	
	for (long i = 0;i<nField;i++)
	{
		pFields->get_Field(i,&pOneField);
		pOneField->get_Name(&FieldName);
		m_ListBox1.InsertString(i,FieldName);
		//m_ListBox1.AddString(FieldName);
	}

	int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetCurSel();//当前选中的行	
	m_ListBox1.DeleteString(iPos);
}

void FillSafeArray3(OLECHAR FAR* sz, int iRow, int iCol,
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

void FillSafeArray3_float(float sz, int iRow, int iCol,
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

void CDlg_PriorProbability::OnBnClickedPriorprobabilityout()//导出表格
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

	std::map<const wchar_t*,CString> ColumnMap;
	ColumnMap[_T("证据因子")] = 'A';
	ColumnMap[_T("矿点在证据在")] = 'B';
	ColumnMap[_T("矿点在证据不在")] = 'C';
	ColumnMap[_T("矿点不在证据在")] = 'D';
	ColumnMap[_T("矿点不在证据不在")] = 'E';

	ExcelMgr excel; 
	excel.CreateExcel(FilePathName);	
	excel.SetCurSheet(1);	
	excel.SetSheetName(_T("各因子证据权值"));

	int startRow = 1;
	CString adf;
	adf.Format(_T("%s%d"),ColumnVec[0], startRow);
	excel.SetText(adf,_T("证据因子"));
	adf.Format(_T("%s%d"),ColumnVec[1], startRow);//
	excel.SetText(adf,_T("矿点在证据在"));
	adf.Format(_T("%s%d"),ColumnVec[2], startRow);
	excel.SetText(adf,_T("矿点在证据不在"));
	adf.Format(_T("%s%d"),ColumnVec[3], startRow);
	excel.SetText(adf,_T("矿点不在证据在"));
	adf.Format(_T("%s%d"),ColumnVec[4], startRow);
	excel.SetText(adf,_T("矿点不在证据不在"));

	int nItemCount,nColumnCount;
	nColumnCount = m_ListCtrl.GetHeaderCtrl()->GetItemCount();//得到列数
	nItemCount = m_ListCtrl.GetItemCount();//得到行数

	CString strListData;
	CRect rc;
	for (int j=0;j<nColumnCount;j++)
	{
		for (int i=0;i<nItemCount;i++)
		{
			startRow = 2;
			m_ListCtrl.GetSubItemRect(i,j,LVIR_LABEL,rc);//获得子项的RECT；
			strListData=m_ListCtrl.GetItemText(i,j);
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
	range.put_Value2(COleVariant(TEXT("条件概率")));
	range = sheet.get_Range(COleVariant(TEXT("A1")),COleVariant(TEXT("B1")));
	range.Merge(covOptional);

	range = sheet.get_Range(COleVariant(TEXT("B2")),COleVariant(TEXT("B2")));
	range.put_Value2(COleVariant(TEXT("条件概率")));

	//range = sheet.get_Range(COleVariant(TEXT("C2")),COleVariant(TEXT("C2")));
	//range.put_Value2(COleVariant(TEXT("Amsterdam")));


	//Fill A3:A6 with an array of values (Months).
	COleSafeArray saRet;
	DWORD numElements[]={nItemCount,1};   //4x1 element array
	saRet.Create(VT_BSTR, 2, numElements);

	std::vector<OLECHAR*> strvec;
	for (int i=0;i<nItemCount;i++)
	{
		m_ListCtrl.GetSubItemRect(i,0,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrl.GetItemText(i,0);
		strvec.push_back(strListData.AllocSysString());	
		FillSafeArray3(strvec[i], i, 0, &saRet);
	}

	CString indexStr = "A";
	CString itemStr;	
	itemStr.Format(_T("%d"), nItemCount + 2);
	indexStr.Append(itemStr);
	range = sheet.get_Range(COleVariant(TEXT("A3")), COleVariant(indexStr));
	range.put_Value2(COleVariant(saRet));
	saRet.Detach();

	//CString strListData;
	//CRect rc;
	saRet.Create(VT_R4,2,numElements);
	float flo;
	std::vector<float> vec;
	strvec.clear();
	for (int i=0;i<nItemCount;i++)
	{
		m_ListCtrl.GetSubItemRect(i,1,LVIR_LABEL,rc);//获得子项的RECT；
		strListData=m_ListCtrl.GetItemText(i,1);
		flo = _tstof(strListData);
		vec.push_back(flo);
		FillSafeArray3_float(vec[i], i, 0, &saRet);
	}
	
	indexStr = "B";
	itemStr.Format(_T("%d"), nItemCount + 2);
	indexStr.Append(itemStr);
	range = sheet.get_Range(COleVariant(TEXT("B3")), COleVariant(indexStr));
	range.put_Value2(COleVariant(saRet));
	saRet.Detach();

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

void CDlg_PriorProbability::OnBnClickedVoxetstatistics()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgVoxetStatistics dlg;	
	if(dlg.DoModal() == IDOK)
	{
	}
}

void CDlg_PriorProbability::OnCbnSelchangeComboResearcharea()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ListBox1.ResetContent();//可以删除列表框中所有行

	IcwPropertyTablePtr pPropertyTable;
	m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);

	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);	
	long nField;
	IcwFieldPtr pOneField;
	pFields->get_FieldCount(&nField);	
	CComBSTR FieldName;	
	for (long i = 0;i<nField;i++)
	{
		pFields->get_Field(i,&pOneField);
		pOneField->get_Name(&FieldName);
		m_ListBox1.InsertString(i,FieldName);
		//m_ListBox1.AddString(FieldName);
	}

	int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetCurSel();//当前选中的行
	CString str;
	((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetLBText(iPos,str);
	iPos = m_ListBox1.FindString(0,str);
	if(-1 != iPos)
		m_ListBox1.DeleteString(iPos);
	iPos=((CComboBox*)GetDlgItem(IDC_COMBO_RESEARCHAREA))->GetCurSel();//当前选中的行
	((CComboBox*)GetDlgItem(IDC_COMBO_RESEARCHAREA))->GetLBText(iPos,str);
	iPos = m_ListBox1.FindString(0,str);
	if(-1 != iPos)
		m_ListBox1.DeleteString(iPos);
}
