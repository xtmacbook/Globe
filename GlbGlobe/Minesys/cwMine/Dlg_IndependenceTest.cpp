// Dlg_IndependenceTest.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Dlg_IndependenceTest.h"

// CDlg_IndependenceTest �Ի���

IMPLEMENT_DYNAMIC(CDlg_IndependenceTest, CDialog)

CDlg_IndependenceTest::CDlg_IndependenceTest(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_IndependenceTest::IDD, pParent)
{
}

CDlg_IndependenceTest::~CDlg_IndependenceTest()
{
}

void CDlg_IndependenceTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTINDEPENDENCE, m_Listindependence);
	DDX_Control(pDX, IDC_LISTINDEPENDENCESELECT, m_Listindependenceselect);
	DDX_Control(pDX, IDC_LISTTEST, m_ListConditionTest);
}


BEGIN_MESSAGE_MAP(CDlg_IndependenceTest, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlg_IndependenceTest::OnBnClickedButton1)
	//ON_WM_INITMENU()
	ON_BN_CLICKED(IDC_BUTTON2, &CDlg_IndependenceTest::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlg_IndependenceTest::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlg_IndependenceTest::OnBnClickedButton4)
	ON_CBN_SELCHANGE(IDC_SIGNIFICANCELEVEL, &CDlg_IndependenceTest::OnCbnSelchangeSignificancelevel)
	ON_BN_CLICKED(IDC_INDEPENDENCECALCULATE, &CDlg_IndependenceTest::OnBnClickedIndependencecalculate)
END_MESSAGE_MAP()


// CDlg_IndependenceTest ��Ϣ�������

//BOOL CDlg_IndependenceTest::OnInitDialog()
//{
//	CPropertySheet *Wizard = reinterpret_cast<CPropertySheet *>(GetParent());
//	pDlgPriorProbability= (CDlg_PriorProbability *)Wizard->GetPage(0);
//
//	return TRUE;
//}
	//m_Listindependence.ResetContent();//����ɾ���б����������
	//IcwPropertyTablePtr pPropertyTable;
	//pVoxet->get_PropTable(&pPropertyTable);
	//ASSERT(pPropertyTable);
	//IcwFieldPtr pField;
	//IcwFieldsPtr pFields;
	//pPropertyTable->get_Fields(&pFields);	
	//long nField;
	//IcwFieldPtr pOneField;
	//pFields->get_FieldCount(&nField);	
	//CComBSTR FieldName;	
	//for (long i = 0;i<nField;i++)
	//{
	//	pFields->get_Field(i,&pOneField);
	//	pOneField->get_Name(&FieldName);
	//	m_Listindependence.InsertString(i,FieldName);
	//	//m_ListBox1.AddString(FieldName);
	//}

	//int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetCurSel();//��ǰѡ�е���
	//CString strTemp;
	//((CComboBox*)GetDlgItem(IDC_COMBO1))->GetWindowText(strTemp);//ȡ��ǰ����
	//m_Listindependence.DeleteString(iPos);

// 	IcwPropertyTablePtr pPropertyTable;
// 	pVoxet->get_PropTable(&pPropertyTable);
// 	ASSERT(pPropertyTable);
// 
// 	IcwFieldPtr pField;
// 	IcwFieldsPtr pFields;
// 	pPropertyTable->get_Fields(&pFields);	
// 	long nField;
// 	IcwFieldPtr pOneField;
// 	pFields->get_FieldCount(&nField);	
// 	CComBSTR FieldName;	
// 	for (long i = 0;i<nField;i++)
// 	{
// 		pFields->get_Field(i,&pOneField);
// 		pOneField->get_Name(&FieldName);
// 		m_Listindependence.AddString(FieldName);
// 	}
// 
// 	int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetCurSel();//��ǰѡ�е���
// 	//CString strTemp;
// 	//((CComboBox*)GetDlgItem(IDC_COMBO1))->GetWindowText(strTemp);//ȡ��ǰ����
// 	m_Listindependence.DeleteString(iPos);
	//return true;

//}

void CDlg_IndependenceTest::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nItem = m_Listindependence.GetSelCount();
	int *SelIndex = new int[nItem];
	m_Listindependence.GetSelItems(nItem, SelIndex); 
	for (int i = 0;i<nItem;i++)
	{
		CString str;		
		m_Listindependence.GetText(SelIndex[i], str);
		m_Listindependenceselect.AddString(str);
	}
}

void CDlg_IndependenceTest::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nItem = m_Listindependence.GetCount();
	for (int i = 0;i<nItem;i++)
	{
		CString str;
		m_Listindependence.GetText(i,str);
		m_Listindependenceselect.AddString(str);
	}
}

void CDlg_IndependenceTest::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nItem = m_Listindependenceselect.GetSelCount();
	int *SelIndex = new int[nItem];
	m_Listindependenceselect.GetSelItems(nItem, SelIndex); 
	for (int i = 0;i<nItem;i++)
	{		
		m_Listindependenceselect.DeleteString(SelIndex[i]-i);
	}
}

void CDlg_IndependenceTest::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nItem = m_Listindependenceselect.GetCount();
	for (int i = 0;i<nItem;i++)
	{
		m_Listindependenceselect.DeleteString(0);
	}
}

void CDlg_IndependenceTest::OnCbnSelchangeSignificancelevel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_Listindependence.ResetContent();//����ɾ���б����������

	IcwPropertyTablePtr pPropertyTable;
	pVoxet->get_PropTable(&pPropertyTable);
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
		m_Listindependence.InsertString(i,FieldName);
		//m_ListBox1.AddString(FieldName);
	}

	//int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetCurSel();//��ǰѡ�е���
	//CString strTemp;
	//((CComboBox*)GetDlgItem(IDC_COMBO1))->GetWindowText(strTemp);//ȡ��ǰ����
	//m_Listindependence.DeleteString(iPos);

	// 	IcwPropertyTablePtr pPropertyTable;
	// 	pVoxet->get_PropTable(&pPropertyTable);
	// 	ASSERT(pPropertyTable);
	// 
	// 	IcwFieldPtr pField;
	// 	IcwFieldsPtr pFields;
	// 	pPropertyTable->get_Fields(&pFields);	
	// 	long nField;
	// 	IcwFieldPtr pOneField;
	// 	pFields->get_FieldCount(&nField);	
	// 	CComBSTR FieldName;	
	// 	for (long i = 0;i<nField;i++)
	// 	{
	// 		pFields->get_Field(i,&pOneField);
	// 		pOneField->get_Name(&FieldName);
	// 		m_Listindependence.AddString(FieldName);
	// 	}
	// 
	// 	int iPos=((CComboBox*)GetDlgItem(IDC_COMBO_MINE))->GetCurSel();//��ǰѡ�е���
	// 	//CString strTemp;
	// 	//((CComboBox*)GetDlgItem(IDC_COMBO1))->GetWindowText(strTemp);//ȡ��ǰ����
	// 	m_Listindependence.DeleteString(iPos);
	//return true;
}

void CDlg_IndependenceTest::OnBnClickedIndependencecalculate()//���������Լ���
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPropertySheet *Wizard = reinterpret_cast<CPropertySheet *>(GetParent());
	pDlgPriorProbability= (CDlg_PriorProbability *)Wizard->GetPage(0);
	
	m_ListConditionTest.InsertColumn(0, _T(" "), LVCFMT_LEFT, 100 );	
	int nItem = m_Listindependenceselect.GetCount();//�õ�����
	for (int i = 0;i<nItem;i++)
	{
		CString str;
		m_Listindependenceselect.GetText(i,str);		
		m_ListConditionTest.InsertColumn(i+1, str, LVCFMT_LEFT, 100 );		
		m_ListConditionTest.InsertItem(i,str);
	}

	//�ĸ��������
	//����һ������ͳ�ƶ�ÿ�������е�����a������b
	//A��00��B��01��C��10��D��11

	IcwPropertyTablePtr pPropertyTable;
	pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);
	long nu,nv,nw;
	pVoxet->get_nU(&nu);
	pVoxet->get_nV(&nv);
	pVoxet->get_nW(&nw);
	double sumgrid = nu*nv*nw;	
	for (int Itemone = 0; Itemone<nItem; Itemone++)
	{
		for (int Itemtwo = 0; Itemtwo<nItem; Itemtwo++)
		{
			if (Itemone == Itemtwo)
			{
				break;
			}
			else
			{
				//��������һ�飬���������
				double A=0;
				double B=0;
				double C=0;
				double D=0;

				CString str1,str2;
				m_Listindependenceselect.GetText(Itemone,str1);
				m_Listindependenceselect.GetText(Itemtwo,str2);
				CComBSTR bstr1 = str1.AllocSysString();
				CComBSTR bstr2 = str2.AllocSysString();
				IcwColumnDataPtr pColumnData1,pColumnData2;
				//ȡ���������ֶ�
				pPropertyTable->GetColumn(bstr1, &pColumnData1);
				pPropertyTable->GetColumn(bstr2, &pColumnData2);
				IcwBigColumnData3DPtr pBigColumnData3D1 = pColumnData1;
				IcwBigColumnData3DPtr pBigColumnData3D2 = pColumnData2;
				long voxetindex = 0;
				for (long u = 0;u<nu;u++)
				{
					long planeindex = 0;
					IcwDataPlanePtr pDataplane1,pDataplane2;
					HRESULT hr = pBigColumnData3D1->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane1);
					hr = pBigColumnData3D2->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane2);
					IcwDataArrayPtr pArray1 = pDataplane1;
					IcwDataArrayPtr pArray2 = pDataplane2;
					void * pbuffer1;
					void* pbuffer2;
					pArray1->GetBuffer(&pbuffer1);
					pArray2->GetBuffer(&pbuffer2);
					float * pfloatArray1 = NULL;
					float * pfloatArray2 = NULL;
					pfloatArray1 = (float*)pbuffer1;
					pfloatArray2 = (float*)pbuffer2;
					for (long v = 0;v<nv;v++)
					{
						for (long w= 0;w<nw;w++)
						{
							planeindex = w+v*nw;
							voxetindex = u * nv * nw + v * nw + w;
							if (pDlgPriorProbability->ResearchAreaValue[voxetindex] == 1)
							{
								if (pfloatArray1[planeindex] == 0 && pfloatArray2[planeindex] == 0)
								{
									A++;								
								}
								else if (pfloatArray1[planeindex] == 0 && pfloatArray2[planeindex] == 1)
								{
									B++;
								}
								else if (pfloatArray1[planeindex] == 1 && pfloatArray2[planeindex] == 0)
								{
									C++;
								}
								else if (pfloatArray1[planeindex] == 1 && pfloatArray2[planeindex] == 1)
								{
									D++;
								}
							}
						}
					}
				}
				//����ABCD���㿨��ֵ

				//����ֵT11 T12 T21 T22
				/*double T11 = ((AA+CC)*(A+B))/(A+B+C+D);
				double T12 = (B+D)*(A+B)/(A+B+C+D);
				double T21 = (A+C)*(C+D)/(A+B+C+D);
				double T22 = (B+D)*(C+D)/(A+B+C+D);

				double a11 = (T11-A)*(T11-A)/T11;
				double a12 = (T12-B)*(T12-B)/T12;
				double a21 = (T21-C)*(T21-C)/T21;
				double a22 = (T22-D)*(T22-D)/T22;*/

				/*double X2TEST = a11+a12+a21+a22;*/
				/*AA = 85;
				BB = 8;
				CC = 4;
				DD = 1;*/

				double temp1 = (A*D-B*C);
				sumgrid = A+B+C+D;
				double up = sumgrid*temp1*temp1;
				double down = (A+B)*(C+D)*(A+C)*(B+D);

				double X2TEST = up/down;

				char* buffer = new char[30];				
				sprintf(buffer,"%8.2f",X2TEST);
				CString X2 = buffer;
				m_ListConditionTest.SetItemText( Itemone, Itemtwo+1, (LPCTSTR)X2);
			}
		}
		//double temp1 = (A*D-B*C);
		//double up = sumgrid*temp1*temp1;
		//double down = (A+B)*(C+D)*(A+C)*(B+D);
		//X2TEST = up/down;
	}
}
