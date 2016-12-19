#pragma once
#include "afxwin.h"
#include "Dlg_InformationCalculate.h"
#include "afxcmn.h"


// CDlg_InformationCalculate2 对话框

class CDlg_InformationCalculate2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlg_InformationCalculate2)

public:
	CDlg_InformationCalculate2();
	virtual ~CDlg_InformationCalculate2();

	CDlg_InformationCalculate *pDlgPriorProbability;
	CListCtrl m_CLC;

	std::vector<std::vector<long>> EnvidenceTrueIndex;
	std::vector<std::vector<long>> EnvidenceFalseIndex;

	std::vector<std::vector<long>> InformationExist;


	

	//std::vector<long> row;
	//std::vector<row> evidenceexist;
	//IcwVoxetPtr pvoxet;

	// 对话框数据
	enum { IDD = IDD_DIALOG_INFORMATIONCALCULATE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual LRESULT OnWizardNext();
	CListBox m_ListBox1;
	CListBox m_ListBox2;
	CListCtrl m_ListCtrlEnvidecce;
	afx_msg void OnBnClickedEnvidence();
	afx_msg void OnBnClickedEnvidenceout();
	afx_msg void OnBnClickedPosteriorprobability();
	afx_msg void OnBnClickedDeleteenvdence();	
	afx_msg void OnBnClickedIndependencetest();
	afx_msg void OnBnClickedAscertainfactor();
	afx_msg void OnBnClickedSortbyinformation();
	afx_msg void OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_InformationLevel;
	afx_msg void OnCbnSelchangeComboInformationLevel();
	CComboBox m_ComboBoxInformationLevel;
	afx_msg void OnBnClickedInformationInputvoxet();
};
