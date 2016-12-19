// Dlg_WizardInformation.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Dlg_WizardInformation.h"
#include "UsefulUtility/DlgInput.h"
#include "UsefulUtility/DlgOutput.h"
#include "Dlg_InformationCalculate.h"
#include "Dlg_InformationCalculate2.h"
//#include "Dlg_EvidenceMethod.h"
// #include "Dlg_IndependenceTest.h"

// Dlg_WizardInformation �Ի���

CDlg_WizardInformation::CDlg_WizardInformation()
{
}
CDlg_WizardInformation::~CDlg_WizardInformation()
{
}

//Dlg_WizardInformation ��Ϣ�������
BOOL CDlg_WizardInformation::OnInitDialog()
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

void CDlg_WizardInformation::OnOK()
{
	long nObj=0;
	if (m_pDlgInput->m_pObjSet)
	{
		m_pDlgInput->m_pObjSet->get_ItemCount(&nObj);
	}
	IcwObjectPtr pObj;	
	m_pDlgInput->m_pObjSet->GetItem(0,&pObj);
	IcwVoxetPtr pVoxet = pObj;

	CDialog::OnOK();

	CPropertySheet Wizard(_T("�½�ģ��"));
	// ����ҳ1
	CDlg_InformationCalculate m_DlgInformationCalculate;
	m_DlgInformationCalculate.m_pVoxet = pVoxet;
	//m_DlgPriorProbability.isNew = true;

	// ����ҳ2
	CDlg_InformationCalculate2 m_DlgInformationCalculate2;
	//m_DlgEvidenceMethod.m_CLC = m_DlgPriorProbability.m_ListCtrl;
	//m_DlgEvidenceMethod.isNew = true;

	// �������ҳ
	Wizard.AddPage(&m_DlgInformationCalculate);
	Wizard.AddPage(&m_DlgInformationCalculate2);
	// ��ģʽ
	Wizard.SetWizardMode();
	Wizard.DoModal();
}
