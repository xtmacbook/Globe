// Dlg_WizardSelect.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Dlg_WizardSelect.h"
#include "UsefulUtility/DlgInput.h"
#include "UsefulUtility/DlgOutput.h"
#include "Dlg_PriorProbability.h"
#include "Dlg_EvidenceMethod.h"
#include "Dlg_IndependenceTest.h"
#include "Dlg_DataStatistics.h"

// Dlg_WizardSelect �Ի���

CDlg_WizardSelect::CDlg_WizardSelect()
{
	m_isDataStatistics = false;
}
CDlg_WizardSelect::~CDlg_WizardSelect()
{

}

// Dlg_BlockFromFBody ��Ϣ�������
BOOL CDlg_WizardSelect::OnInitDialog()
{
	//���ñ���
	this->SetWindowText(_T("ѡ��Voxet"));
	//���ƶԻ���
	{
		m_pDlgInput=new CDlgInput;
		m_pDlgInput->m_pSceneView=m_pSceneView;
		m_pDlgInput->m_ObjType=cwObjTypeVoxet;
		m_pDlgInput->m_Title = _T("Voxet:");
		AddExtDialog(m_pDlgInput,CDlgInput::IDD);
	}
	CDlgMultiExt::OnInitDialog();
	return TRUE;
}

void CDlg_WizardSelect::OnOK()
{
	long nObj=0;
	if (m_pDlgInput->m_pObjSet)
	{
		m_pDlgInput->m_pObjSet->get_ItemCount(&nObj);
	}
	IcwObjectPtr pObj;	
	m_pDlgInput->m_pObjSet->GetItem(0,&pObj);
	IcwVoxetPtr pVoxet = pObj;

	CDialog::OnOK();//�رռ�ѡ����

	if(m_isDataStatistics)
	{
		IcwPropertyTablePtr pPropertyTable;
		pVoxet->get_PropTable(&pPropertyTable);
		ASSERT(pPropertyTable);
		IcwFieldPtr pField;
		IcwFieldsPtr pFields;
		pPropertyTable->get_Fields(&pFields);
		if (pFields == NULL)
			return;

		//�����Ի���
		CDlg_DataStatistics *m_voxDlg = new CDlg_DataStatistics;
		m_voxDlg->Create(CDlg_DataStatistics::IDD, NULL);
		//��ʼ��ComBox
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
			((CComboBox*)m_voxDlg->GetDlgItem(IDC_COMBO4))->AddString(name);
			((CComboBox*)m_voxDlg->GetDlgItem(IDC_COMBO5))->AddString(name);
		}
		m_voxDlg->m_pVoxet = pVoxet;
		m_voxDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		CPropertySheet Wizard(_T("�½�ģ��"));
		// ����ҳ1
		CDlg_PriorProbability m_DlgPriorProbability;
		m_DlgPriorProbability.m_pVoxet = pVoxet;

		//m_DlgPriorProbability.isNew = true;
		// ����ҳ2
		CDlg_EvidenceMethod m_DlgEvidenceMethod;
		//m_DlgEvidenceMethod.m_CLC = m_DlgPriorProbability.m_ListCtrl;
		//m_DlgEvidenceMethod.isNew = true;
		// �������ҳ
		Wizard.AddPage(&m_DlgPriorProbability);
		Wizard.AddPage(&m_DlgEvidenceMethod);
		// ��ģʽ
		Wizard.SetWizardMode();	
		Wizard.DoModal();
	}	
}
