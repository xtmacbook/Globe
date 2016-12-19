// Dlg_WizardSelect.cpp : 实现文件
//

#include "stdafx.h"
#include "Dlg_WizardSelect.h"
#include "UsefulUtility/DlgInput.h"
#include "UsefulUtility/DlgOutput.h"
#include "Dlg_PriorProbability.h"
#include "Dlg_EvidenceMethod.h"
#include "Dlg_IndependenceTest.h"
#include "Dlg_DataStatistics.h"

// Dlg_WizardSelect 对话框

CDlg_WizardSelect::CDlg_WizardSelect()
{
	m_isDataStatistics = false;
}
CDlg_WizardSelect::~CDlg_WizardSelect()
{

}

// Dlg_BlockFromFBody 消息处理程序
BOOL CDlg_WizardSelect::OnInitDialog()
{
	//设置标题
	this->SetWindowText(_T("选择Voxet"));
	//定制对话框
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

	CDialog::OnOK();//关闭拣选窗口

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
			((CComboBox*)m_voxDlg->GetDlgItem(IDC_COMBO4))->AddString(name);
			((CComboBox*)m_voxDlg->GetDlgItem(IDC_COMBO5))->AddString(name);
		}
		m_voxDlg->m_pVoxet = pVoxet;
		m_voxDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		CPropertySheet Wizard(_T("新建模型"));
		// 属性页1
		CDlg_PriorProbability m_DlgPriorProbability;
		m_DlgPriorProbability.m_pVoxet = pVoxet;

		//m_DlgPriorProbability.isNew = true;
		// 属性页2
		CDlg_EvidenceMethod m_DlgEvidenceMethod;
		//m_DlgEvidenceMethod.m_CLC = m_DlgPriorProbability.m_ListCtrl;
		//m_DlgEvidenceMethod.isNew = true;
		// 添加属性页
		Wizard.AddPage(&m_DlgPriorProbability);
		Wizard.AddPage(&m_DlgEvidenceMethod);
		// 向导模式
		Wizard.SetWizardMode();	
		Wizard.DoModal();
	}	
}
