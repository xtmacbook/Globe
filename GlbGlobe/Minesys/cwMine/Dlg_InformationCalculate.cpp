// Dlg_PriorProbability.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Dlg_InformationCalculate.h"
//#include "Dlg_EvidenceMethod.h"
#include "UsefulUtility/DlgInput.h"
#include <vector>
#include <fstream>
#include <map>
#include "ExcelMgr.h"
#include "DlgVoxetStatistics.h"
#include "math.h"


#include "CApplication.h"
#include "CFont0.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "CChart.h"
#include "CCharts.h"

// CDlg_PriorProbability �Ի���

IMPLEMENT_DYNAMIC(CDlg_InformationCalculate, CPropertyPage)

CDlg_InformationCalculate::CDlg_InformationCalculate()
: CPropertyPage(CDlg_InformationCalculate::IDD)
, m_ObjNames(_T(""))
{
	m_ObjNames=_T("");
}

CDlg_InformationCalculate::~CDlg_InformationCalculate()
{

}

void CDlg_InformationCalculate::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_LIST2, m_ListBox1);
	DDX_Control(pDX, IDC_LIST3, m_ListBox2);	
	DDX_Control(pDX, IDC_LIST7, m_ListCtrl);
	DDX_Control(pDX, IDC_COMBO_MINE, m_ComboBoxMine);
	DDX_Control(pDX, IDC_COMBO_RESEARCHAREA, m_ComboBoxResearchArea);
}


BEGIN_MESSAGE_MAP(CDlg_InformationCalculate, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlg_InformationCalculate::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlg_InformationCalculate::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlg_InformationCalculate::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlg_InformationCalculate::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDlg_InformationCalculate::OnBnClickedButton5)
	ON_CBN_SELCHANGE(IDC_COMBO_MINE, &CDlg_InformationCalculate::OnCbnSelchangeComboMine)
	ON_BN_CLICKED(IDC_PRIORPROBABILITYOUT, &CDlg_InformationCalculate::OnBnClickedInformationout)
	ON_BN_CLICKED(IDC_VOXETStatistics, &CDlg_InformationCalculate::OnBnClickedVoxetstatistics)
	ON_CBN_SELCHANGE(IDC_COMBO_RESEARCHAREA, &CDlg_InformationCalculate::OnCbnSelchangeComboResearcharea)
END_MESSAGE_MAP()


// CDlg_PriorProbability ��Ϣ�������

BOOL CDlg_InformationCalculate::OnInitDialog()
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

	//��ʼ��listctrl����
	m_ListCtrl.InsertColumn( 0, _T("��������"), LVCFMT_LEFT, 80 );
	m_ListCtrl.InsertColumn( 1, _T("������������"), LVCFMT_LEFT, 110 );
	m_ListCtrl.InsertColumn( 2, _T("����������"), LVCFMT_LEFT, 80 );
	m_ListCtrl.InsertColumn( 3, _T("��������"), LVCFMT_LEFT, 80 );
	m_ListCtrl.InsertColumn( 4, _T("�о�������"), LVCFMT_LEFT, 80 );
	m_ListCtrl.InsertColumn( 5, _T("��Ϣ��ֵ"), LVCFMT_LEFT, 80 );
	return true;
}

BOOL CDlg_InformationCalculate::OnSetActive()
{	
	((CPropertySheet *)GetParent())->SetWizardButtons(PSWIZB_NEXT);  // ������һ����ť����
	return CPropertyPage::OnSetActive();
}

BOOL CDlg_InformationCalculate::OnApply()
{	
	IcwWorkSpacePtr pWorkspace = NULL;
	CoCreateInstance(__uuidof(cwWorkSpace),NULL,CLSCTX_INPROC_SERVER,__uuidof(IcwWorkSpace),(void **)&pWorkspace);
	if (pWorkspace == NULL) return FALSE ;

	pWorkspace->UpdateAllViews();
	return CPropertyPage::OnApply();
}

LRESULT CDlg_InformationCalculate::OnWizardNext()
{	
	return CPropertyPage::OnWizardNext();
}

void CDlg_InformationCalculate::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

void CDlg_InformationCalculate::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nItem = m_ListBox1.GetCount();
	for (int i = 0;i<nItem;i++)
	{
		CString str;
		m_ListBox1.GetText(i,str);
		m_ListBox2.AddString(str);
	}
}

void CDlg_InformationCalculate::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	
	int nItem = m_ListBox2.GetSelCount();
	int *SelIndex = new int[nItem];
	m_ListBox2.GetSelItems(nItem, SelIndex); 
	for (int i = 0;i<nItem;i++)
	{		
		m_ListBox2.DeleteString(SelIndex[i]-i);
	}
}

void CDlg_InformationCalculate::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	
	int nItem = m_ListBox2.GetCount();
	for (int i = 0;i<nItem;i++)
	{
		m_ListBox2.DeleteString(0);
	}
}

//UINT64 GetIndex(long u,long v,long w,IcwVoxet * pVoxet)
//{
//	long nu,nv,nw;
//	pVoxet->get_nU(&nu);
//	pVoxet->get_nV(&nv);
//	pVoxet->get_nW(&nw);
//	if(u >= nu || v>=nv || w>=nw)
//		return 0;
//	return u * nv * nw + v * nw + w; 
//}


void CDlg_InformationCalculate::OnBnClickedButton5()//��Ϣ������
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//ͳ�Ƹ�����N,N(J),S,S(J)
	//N->�о�������Ԫ��
	//S->�о�����Ԫ��
	//N(J)->�о����������ӵ�Ԫ��
	//S(J)->�о��������ӵ�Ԫ��
	//I = log((N(J)/N) / (S(J)/S))

	m_ListCtrl.DeleteAllItems();
	std::vector<CString> envidenceName;
	int nItem = m_ListBox2.GetCount();	
	for (int i = nItem-1; i>=0;i--)
	{
		CString str;
		m_ListBox2.GetText(i,str);
		envidenceName.push_back(str);
		m_ListCtrl.InsertItem(LVIF_TEXT|LVIF_STATE,0, str,0,LVIS_SELECTED,0,0);		
	}

	for (int OneItem = 0; OneItem<nItem; OneItem++)
	{
		vecMine_Factor.push_back(OneItem);
		vecMine_Factor[OneItem] = 0;
		vecFactor.push_back(OneItem);
		vecFactor[OneItem] = 0;
	}

	nResearch = 0;
	nMine = 0;

	//��ÿһ���������ӣ���ȡ����ֵ
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
	long voxetindex;	
	for (long u = 0;u<nu;u++)
	{
		long planeindex = 0;
		IcwDataPlanePtr pDataplane;
		HRESULT hr = pBigColumnData3D->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane);
		if(hr != S_OK)
			AfxMessageBox(_T("GetDataPlane����!"));
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
					nResearch++;//�о�����ռ����
			}
		}	
	}

	CString m_MineName;	
	((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetWindowText(m_MineName);//ȡ��ǰ����	
	CComBSTR bstrmine = m_MineName.AllocSysString();
	//ȡ����ֶ�
	IcwColumnDataPtr pColumnDataMine;
	pPropertyTable->GetColumn(bstrmine, &pColumnDataMine);
	pBigColumnData3D = pColumnDataMine;
	ASSERT(pBigColumnData3D);
	
	voxetindex=0;
	for (long u = 0;u<nu;u++)
	{
		long planeindex = 0;		
		IcwDataPlanePtr pDataplane;
		HRESULT hr = pBigColumnData3D->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane);
		if(hr != S_OK)
			AfxMessageBox(_T("GetDataPlane����!"));
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
				MineValVec.push_back(pfloatArray[planeindex]);//�����ؿ��ֵ				
				if (pfloatArray[planeindex] == 1.0 && ResearchAreaValue[voxetindex] == 1.0)
					nMine++;	//��������
			}		
		}	
	}

	//�������index
	//for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	//{
	//	EnvidenceTrueIndex.clear();
	//	EnvidenceFalseIndex.clear();
	//}

	//EnvidenceTrueIndex.resize(nItem);
	//EnvidenceFalseIndex.resize(nItem);

	//for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	//{
	//	ptT_T.push_back(SingleItem);
	//	ptT_T[SingleItem] = 0;
	//	MineTrueIndex.push_back(SingleItem);
	//	MineTrueIndex[SingleItem] = 0;
	//	ptF_T.push_back(SingleItem);
	//	ptF_T[SingleItem] = 0;
	//	ptF_F.push_back(SingleItem);
	//	ptF_F[SingleItem] = 0;
	//}

	//��ÿһ�����Ӽ�����������ֵ������nItem����������Ӽ�¼����
	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{
		CString str;
		m_ListBox2.GetText(SingleItem,str);
		CComBSTR bstr = str.AllocSysString();
		IcwColumnDataPtr pColumnData;
		//ȡ���������ֶ�
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
						vecFactor[SingleItem]++;
						if(MineValVec[voxetindex] == 1)
						{
							vecMine_Factor[SingleItem]++;
						}												
					}					
				}
			}
		}
	}

	//�����������Ϣ��
	long N,Nj,S,Sj;
	float up,down,InformationVal;
	S = nResearch;//�о�������
	N = nMine;//�о�����������
	CString strN;
	strN.Format(_T("%ld"),N);
	CString strS;
	strS.Format(_T("%ld"),S);
	for (int SingleItem = 0; SingleItem<nItem; SingleItem++)
	{		
		Nj = vecMine_Factor[SingleItem];
		Sj = vecFactor[SingleItem];
		up = (float)Nj/(float)N;
		down = (float)Sj/(float)S;
		InformationVal = log(up/down);
		CString strInformation;
		strInformation.Format(_T("%0.3f"),InformationVal);
		CString strNj;
		strNj.Format(_T("%ld"),Nj);
		CString strSj;
		strSj.Format(_T("%ld"),Sj);

		//��ʾÿ��������Ϣ��ֵ
		//SetItemText(�кţ��кţ���Ϣ��ֵ)
		m_ListCtrl.SetItemText(SingleItem , 1 , (LPCTSTR)strNj);
		m_ListCtrl.SetItemText(SingleItem , 2 ,(LPCTSTR)strSj);
		m_ListCtrl.SetItemText(SingleItem , 3 , (LPCTSTR)strN);
		m_ListCtrl.SetItemText(SingleItem , 4 , (LPCTSTR)strS);
		m_ListCtrl.SetItemText(SingleItem , 5 , (LPCTSTR)strInformation);
	}
	std::vector<float> temp1;
	temp1.swap(MineValVec);
	temp1.swap(ResearchAreaValue);
	std::vector<long> temp2;
	temp2.swap(vecMine_Factor);
	temp2.swap(vecFactor);
}


void CDlg_InformationCalculate::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ListBox1.ResetContent();//����ɾ���б����������
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

	int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetCurSel();//��ǰѡ�е���
	//CString strTemp;
	//((CComboBox*)GetDlgItem(IDC_COMBO1))->GetWindowText(strTemp);//ȡ��ǰ����
	m_ListBox1.DeleteString(iPos);
}

void CDlg_InformationCalculate::OnCbnSelchangeComboMine()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	m_ListBox1.ResetContent();//����ɾ���б����������
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

	int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetCurSel();//��ǰѡ�е���	
	m_ListBox1.DeleteString(iPos);
}

void FillSafeArray(OLECHAR FAR* sz, int iRow, int iCol,
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

void CDlg_InformationCalculate::OnBnClickedInformationout()
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
	ColumnVec.push_back(_T("F"));

	std::map<const wchar_t*,CString> ColumnMap;
	ColumnMap[_T("��������")] = 'A';
	ColumnMap[_T("��������")] = 'B';
	ColumnMap[_T("��������")] = 'C';
	ColumnMap[_T("����������")] = 'D';
	ColumnMap[_T("�о�������")] = 'E';
	ColumnMap[_T("��Ϣ��ֵ")] = 'F';

	ExcelMgr excel; 
	excel.CreateExcel(FilePathName);	
	excel.SetCurSheet(1);	
	excel.SetSheetName(_T("��������Ϣ��ֵ"));

	int startRow = 1;
	CString adf;
	adf.Format(_T("%s%d"),ColumnVec[0], startRow);
	excel.SetText(adf,_T("��������"));
	adf.Format(_T("%s%d"),ColumnVec[1], startRow);//
	excel.SetText(adf,_T("��������"));
	adf.Format(_T("%s%d"),ColumnVec[2], startRow);
	excel.SetText(adf,_T("��������"));
	adf.Format(_T("%s%d"),ColumnVec[3], startRow);
	excel.SetText(adf,_T("����������"));
	adf.Format(_T("%s%d"),ColumnVec[4], startRow);
	excel.SetText(adf,_T("�о�������"));
	adf.Format(_T("%s%d"),ColumnVec[5], startRow);
	excel.SetText(adf,_T("��Ϣ��ֵ"));

	int nItemCount,nColumnCount;
	nColumnCount = m_ListCtrl.GetHeaderCtrl()->GetItemCount();//�õ�����
	nItemCount = m_ListCtrl.GetItemCount();//�õ�����

	CString strListData;
	CRect rc;
	for (int j=0;j<nColumnCount;j++)
	{
		for (int i=0;i<nItemCount;i++)
		{
			startRow = 2;
			m_ListCtrl.GetSubItemRect(i,j,LVIR_LABEL,rc);//��������RECT��
			strListData=m_ListCtrl.GetItemText(i,j);
			adf.Format(_T("%s%d"),ColumnVec[j], startRow+i);
			excel.SetText(adf,strListData);
		}
	}


	//////////////////////////////////////////////////////////////////////////
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
	range.put_Value2(COleVariant(TEXT("��Ϣ��ֵ")));
	range = sheet.get_Range(COleVariant(TEXT("A1")),COleVariant(TEXT("B1")));
	range.Merge(covOptional);

	range = sheet.get_Range(COleVariant(TEXT("B2")),COleVariant(TEXT("B2")));
	range.put_Value2(COleVariant(TEXT("��Ϣ��ֵ")));

	//range = sheet.get_Range(COleVariant(TEXT("C2")),COleVariant(TEXT("C2")));
	//range.put_Value2(COleVariant(TEXT("Amsterdam")));


	//Fill A3:A6 with an array of values (Months).
	COleSafeArray saRet;
	DWORD numElements[]={4,1};   //4x1 element array
	saRet.Create(VT_BSTR, 2, numElements);

	std::vector<OLECHAR*> strvec;
	for (int i=0;i<nItemCount;i++)
	{
		m_ListCtrl.GetSubItemRect(i,0,LVIR_LABEL,rc);//��������RECT��
		strListData=m_ListCtrl.GetItemText(i,0);
		strvec.push_back(strListData.AllocSysString());	
	}
	FillSafeArray(strvec[0], 0, 0, &saRet);
	FillSafeArray(strvec[1], 1, 0, &saRet);
	FillSafeArray(strvec[2], 2, 0, &saRet);
	FillSafeArray(strvec[3], 3, 0, &saRet);

	range = sheet.get_Range(COleVariant(TEXT("A3")), COleVariant(TEXT("A6")));
	range.put_Value2(COleVariant(saRet));
	saRet.Detach();

	//CString strListData;
	//CRect rc;
	float flo;
	std::vector<float> vec;
	for (int i=0;i<nItemCount;i++)
	{
		m_ListCtrl.GetSubItemRect(i,5,LVIR_LABEL,rc);//��������RECT��
		strListData=m_ListCtrl.GetItemText(i,5);

		flo = _tstof(strListData);
		//strListData.ReleaseBuffer();
		vec.push_back(flo);
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
	//range = sheet.get_Range(COleVariant(TEXT("C3")),COleVariant(TEXT("C3")));
	//range.put_Value2(COleVariant(short(208)));
	//range = sheet.get_Range(COleVariant(TEXT("C4")),COleVariant(TEXT("C4")));
	//range.put_Value2(COleVariant(short(76)));
	//range = sheet.get_Range(COleVariant(TEXT("C5")),COleVariant(TEXT("C5")));
	//range.put_Value2(COleVariant(short(145)));
	//range = sheet.get_Range(COleVariant(TEXT("C6")),COleVariant(TEXT("C6")));
	//range.put_Value2(COleVariant(short(74)));


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

void CDlg_InformationCalculate::OnBnClickedVoxetstatistics()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDlgVoxetStatistics dlg;	
	if(dlg.DoModal() == IDOK)
	{
	}
}

void CDlg_InformationCalculate::OnCbnSelchangeComboResearcharea()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ListBox1.ResetContent();//����ɾ���б����������

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

	int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetCurSel();//��ǰѡ�е���
	int iPos2=((CComboBox*)GetDlgItem(IDC_COMBO_RESEARCHAREA))->GetCurSel();//��ǰѡ�е���	
	m_ListBox1.DeleteString(iPos);
	m_ListBox1.DeleteString(iPos2);
}
