// Dlg_IndependenceTest.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Dlg_DataStatistics.h"

// CDlg_IndependenceTest �Ի���

IMPLEMENT_DYNAMIC(CDlg_DataStatistics, CDialog)

CDlg_DataStatistics::CDlg_DataStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_DataStatistics::IDD, pParent)
{
}

CDlg_DataStatistics::~CDlg_DataStatistics()
{
}

BEGIN_MESSAGE_MAP(CDlg_DataStatistics, CDialog)
	ON_BN_CLICKED(IDC_BUTTONStatistics, &CDlg_DataStatistics::OnBnClickedButtonstatistics)
	ON_BN_CLICKED(IDC_BUTTONCompute, &CDlg_DataStatistics::OnBnClickedButtoncompute)
	ON_BN_CLICKED(IDC_CHECKBing, &CDlg_DataStatistics::OnBnClickedCheckbing)
	ON_BN_CLICKED(IDC_CHECKJiao, &CDlg_DataStatistics::OnBnClickedCheckjiao)
	ON_BN_CLICKED(IDC_CHECKCha, &CDlg_DataStatistics::OnBnClickedCheckcha)
END_MESSAGE_MAP()

void CDlg_DataStatistics::OnBnClickedButtonstatistics()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString strField;
	((CComboBox*)GetDlgItem(IDC_COMBO1))->GetWindowText(strField);//ȡ��ǰ����

	CString strValue;
	double maxValue = 0.0,minValue = 0.0;
	bool isReplace = false,isUsedReplace = false;
	((CEdit*)GetDlgItem(IDC_EDITMaxValue))->GetWindowText(strValue);
	maxValue = _wtof(strValue.GetBuffer(0));
	((CEdit*)GetDlgItem(IDC_EDITMinValue))->GetWindowText(strValue);
	minValue = _wtof(strValue.GetBuffer(0));
	isReplace = ((CButton*)GetDlgItem(IDC_CHECKReplace))->GetCheck();
	isUsedReplace = ((CButton*)GetDlgItem(IDC_CHECKReplace2))->GetCheck();
	CString strReplaceField,strNewField,strUsedReplaceField;
	((CComboBox*)GetDlgItem(IDC_COMBO2))->GetWindowText(strReplaceField);//ȡ��ǰ����
	((CComboBox*)GetDlgItem(IDC_COMBO3))->GetWindowText(strUsedReplaceField);//ȡָ���ֶε�����
	((CEdit*)GetDlgItem(IDC_EDITNewField))->GetWindowText(strNewField);

	long nu,nv,nw;
	cwVector3F stepU,stepV,stepW,Origin;
	m_pVoxet->get_nU(&nu);
	m_pVoxet->get_nV(&nv);
	m_pVoxet->get_nW(&nw);
	m_pVoxet->get_Origin(&Origin);    
	m_pVoxet->get_StepU(&stepU);
	m_pVoxet->get_StepV(&stepV);
	m_pVoxet->get_StepW(&stepW);

	double origin[3] = {Origin.X,Origin.Y,Origin.Z};
	double vecU[3] = {stepU.X,stepU.Y,stepU.Z};
	double vecV[3] = {stepV.X,stepV.Y,stepV.Z};
	double vecW[3] = {stepW.X,stepW.Y,stepW.Z};

	IcwPropertyTablePtr pPropertyTable;
	IcwPropertyPtr pProperty;
	m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);
	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);
	if (pFields == NULL)
		return;

	CComVariant Nodatavalue;
	Nodatavalue.vt = VT_R4;
	Nodatavalue.bVal = 0.0;
	if(isReplace == false)
	{
		cwFieldType fieldtype = cwFieldTypeFloat32;
		pFields->FindField(strNewField.AllocSysString(),&pField);
		if(pField == NULL)
		{
			CoCreateInstance(CLSID_cwField,NULL,CLSCTX_INPROC_SERVER,IID_IcwField,(void**)&pField);
			pField->put_Name(strNewField.AllocSysString());
			pField->put_NoDataValue(Nodatavalue);
			pField->put_Type(fieldtype);
			pFields->AddField(pField);
		}
		else
		{
			AfxMessageBox(_T("���������ֶ��Ѵ��ڣ����޸����������ֶ�!"));
		}
	}

	//��ȡ��ǰ��Ҫͳ�Ƶ�����
	long planeindex = 0;
	IcwColumnDataPtr pColumnData;
	IcwBigColumnData3DPtr pBigColumnData3D;
	pPropertyTable->GetColumn(strField.AllocSysString(), &pColumnData);
	pBigColumnData3D = pColumnData;
	IcwDataPlanePtr pDataplane;

	//��ȡ��Ҫ�¸�ֵ������
	IcwColumnDataPtr pColumnData_new;
	IcwBigColumnData3DPtr pBigColumnData3D_new;
	if(isReplace)
		pPropertyTable->GetColumn(strReplaceField.AllocSysString(), &pColumnData_new);
	else
		pPropertyTable->GetColumn(strNewField.AllocSysString(), &pColumnData_new);
	pBigColumnData3D_new = pColumnData_new;
	IcwDataPlanePtr pDataplane_new;

	//��ȡ��ǰ��Ҫ��ȡ��ֵ����������ֶλ������ֶ�
	IcwColumnDataPtr pColumnData_repalce;
	IcwBigColumnData3DPtr pBigColumnData3D_repalce;
	pPropertyTable->GetColumn(strUsedReplaceField.AllocSysString(), &pColumnData_repalce);
	pBigColumnData3D_repalce = pColumnData_repalce;
	IcwDataPlanePtr pDataplane_replace;

	for (long u = 0;u<nu;u++)
	{
		HRESULT hr = pBigColumnData3D->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane);
		IcwDataArrayPtr pArray = pDataplane;
		void * pbuffer;
		pArray->GetBuffer(&pbuffer);
		float * pfloatArray = NULL;
		pfloatArray = (float*)pbuffer;

		hr = pBigColumnData3D_new->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane_new);
		IcwDataArrayPtr pArray_new = pDataplane_new;
		void * pbuffer_new;
		pArray_new->GetBuffer(&pbuffer_new);
		float * pfloatArray_new = NULL;
		pfloatArray_new = (float*)pbuffer_new;

		float * pfloatArray_repalce = NULL;
		if(isUsedReplace)
		{
			hr = pBigColumnData3D_repalce->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane_replace);
			IcwDataArrayPtr pArray_repalce = pDataplane_replace;
			void * pbuffer_repalce;
			pArray_repalce->GetBuffer(&pbuffer_repalce);
			pfloatArray_repalce = (float*)pbuffer_repalce;
		}

		for (long v = 0;v<nv;v++)
		{
			for (long w= 0;w<nw;w++)
			{
				planeindex = w+v*nw;
				if(pfloatArray[planeindex] > minValue && pfloatArray[planeindex] < maxValue)
				{
					if(isUsedReplace && pfloatArray_repalce)
					{
						if(fabs(pfloatArray_repalce[planeindex] - Nodatavalue.bVal) > 0.00000001)
							pfloatArray_new[planeindex] = pfloatArray_repalce[planeindex];
					}
					else
					{
						if(fabs(pfloatArray[planeindex] - Nodatavalue.bVal) > 0.00000001)
							pfloatArray_new[planeindex] = pfloatArray[planeindex];
					}
				}//�Ƚ�
			}//w
		}//v
		hr = pBigColumnData3D_new->PutDataPlane(u,0,0,cwPlane_VW,nv,nw,pDataplane_new);
		if(hr !=S_OK )
			AfxMessageBox(_T("PutDataPlane������"));
	}//u
}


void CDlg_DataStatistics::OnBnClickedButtoncompute()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString strFieldLeft;
	((CComboBox*)GetDlgItem(IDC_COMBO4))->GetWindowText(strFieldLeft);
	CString strFieldRight;
	((CComboBox*)GetDlgItem(IDC_COMBO5))->GetWindowText(strFieldRight);

	CString strNewField;
	bool isJiao = false,isBing = false,isCha = false;
	isJiao = ((CButton*)GetDlgItem(IDC_CHECKJiao))->GetCheck();
	isBing = ((CButton*)GetDlgItem(IDC_CHECKBing))->GetCheck();
	isCha = ((CButton*)GetDlgItem(IDC_CHECKCha))->GetCheck();
	((CEdit*)GetDlgItem(IDC_EDITNewField2))->GetWindowText(strNewField);

	long nu,nv,nw;
	cwVector3F stepU,stepV,stepW,Origin;
	m_pVoxet->get_nU(&nu);
	m_pVoxet->get_nV(&nv);
	m_pVoxet->get_nW(&nw);
	m_pVoxet->get_Origin(&Origin);    
	m_pVoxet->get_StepU(&stepU);
	m_pVoxet->get_StepV(&stepV);
	m_pVoxet->get_StepW(&stepW);

	double origin[3] = {Origin.X,Origin.Y,Origin.Z};
	double vecU[3] = {stepU.X,stepU.Y,stepU.Z};
	double vecV[3] = {stepV.X,stepV.Y,stepV.Z};
	double vecW[3] = {stepW.X,stepW.Y,stepW.Z};

	IcwPropertyTablePtr pPropertyTable;
	IcwPropertyPtr pProperty;
	m_pVoxet->get_PropTable(&pPropertyTable);
	ASSERT(pPropertyTable);
	IcwFieldPtr pField;
	IcwFieldsPtr pFields;
	pPropertyTable->get_Fields(&pFields);
	if (pFields == NULL)
		return;

	//�������ֶ�
	CComVariant Nodatavalue;
	Nodatavalue.vt = VT_R4;
	Nodatavalue.bVal = 0.0;
	cwFieldType fieldtype = cwFieldTypeFloat32;
	pFields->FindField(strNewField.AllocSysString(),&pField);
	if(pField == NULL)
	{
		CoCreateInstance(CLSID_cwField,NULL,CLSCTX_INPROC_SERVER,IID_IcwField,(void**)&pField);
		pField->put_Name(strNewField.AllocSysString());
		pField->put_NoDataValue(Nodatavalue);
		pField->put_Type(fieldtype);
		pFields->AddField(pField);
	}
	else
	{
		AfxMessageBox(_T("���������ֶ��Ѵ��ڣ����޸����������ֶ�!"));
	}

	//��ȡ��ǰ��Ҫͳ�Ƶ�����left
	long planeindex = 0;
	IcwColumnDataPtr pColumnData_left;
	IcwBigColumnData3DPtr pBigColumnData3D_left;
	pPropertyTable->GetColumn(strFieldLeft.AllocSysString(), &pColumnData_left);
	pBigColumnData3D_left = pColumnData_left;
	IcwDataPlanePtr pDataplane_left;

	//��ȡ��ǰ��Ҫͳ�Ƶ�����right
	IcwColumnDataPtr pColumnData_right;
	IcwBigColumnData3DPtr pBigColumnData3D_right;
	pPropertyTable->GetColumn(strFieldRight.AllocSysString(), &pColumnData_right);
	pBigColumnData3D_right = pColumnData_right;
	IcwDataPlanePtr pDataplane_right;

	//��ȡ��ǰ��Ҫ��ȡ��ֵ��������ֶ�
	IcwColumnDataPtr pColumnData_new;
	IcwBigColumnData3DPtr pBigColumnData3D_new;
	pPropertyTable->GetColumn(strNewField.AllocSysString(), &pColumnData_new);
	pBigColumnData3D_new = pColumnData_new;
	IcwDataPlanePtr pDataplane_new;

	for (long u = 0;u<nu;u++)
	{
		HRESULT hr = pBigColumnData3D_left->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane_left);
		IcwDataArrayPtr pArray_left = pDataplane_left;
		void * pbuffer_left;
		pArray_left->GetBuffer(&pbuffer_left);
		float * pfloatArray_left = NULL;
		pfloatArray_left = (float*)pbuffer_left;

		hr = pBigColumnData3D_right->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane_right);
		IcwDataArrayPtr pArray_right = pDataplane_right;
		void * pbuffer_right;
		pArray_right->GetBuffer(&pbuffer_right);
		float * pfloatArray_right = NULL;
		pfloatArray_right = (float*)pbuffer_right;

		hr = pBigColumnData3D_new->GetDataPlane(u,0,0,cwPlane_VW,nv,nw,&pDataplane_new);
		IcwDataArrayPtr pArray_new = pDataplane_new;
		void * pbuffer_new;
		pArray_new->GetBuffer(&pbuffer_new);
		float * pfloatArray_new = NULL;
		pfloatArray_new = (float*)pbuffer_new;

		bool leftIsVaild = false;
		bool rightVaild = false;
		for (long v = 0;v<nv;v++)
		{
			for (long w= 0;w<nw;w++)
			{
				planeindex = w+v*nw;
				leftIsVaild = fabs(pfloatArray_left[planeindex] - Nodatavalue.bVal) > 0.00000001;
				rightVaild = fabs(pfloatArray_right[planeindex] - Nodatavalue.bVal) > 0.00000001;
				if(isJiao)
				{
					if(leftIsVaild && rightVaild)
						pfloatArray_new[planeindex] = pfloatArray_left[planeindex];
				}
				else if(isBing)
				{
					if(leftIsVaild || rightVaild)
						pfloatArray_new[planeindex] = Nodatavalue.bVal;
				}
				else if(isCha)
				{
					if(leftIsVaild && rightVaild == false)
						pfloatArray_new[planeindex] = pfloatArray_left[planeindex];
					else
						pfloatArray_new[planeindex] = Nodatavalue.bVal;
				}
			}//w
		}//v
		hr = pBigColumnData3D_new->PutDataPlane(u,0,0,cwPlane_VW,nv,nw,pDataplane_new);
		if(hr !=S_OK )
			AfxMessageBox(_T("PutDataPlane������"));
	}//u
}


void CDlg_DataStatistics::OnBnClickedCheckbing()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bool isBing = ((CButton*)GetDlgItem(IDC_CHECKBing))->GetCheck();
	if(isBing)
	{
		((CButton*)GetDlgItem(IDC_CHECKJiao))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECKCha))->SetCheck(FALSE);
	}
}


void CDlg_DataStatistics::OnBnClickedCheckjiao()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bool isJiao = ((CButton*)GetDlgItem(IDC_CHECKJiao))->GetCheck();
	if(isJiao)
	{
		((CButton*)GetDlgItem(IDC_CHECKBing))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECKCha))->SetCheck(FALSE);
	}
}


void CDlg_DataStatistics::OnBnClickedCheckcha()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bool isCha = ((CButton*)GetDlgItem(IDC_CHECKCha))->GetCheck();
	if(isCha)
	{
		((CButton*)GetDlgItem(IDC_CHECKJiao))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CHECKBing))->SetCheck(FALSE);
	}
}
