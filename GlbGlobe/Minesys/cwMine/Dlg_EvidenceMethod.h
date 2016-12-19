#pragma once
#include "afxwin.h"
#include "Dlg_PriorProbability.h"
#include "afxcmn.h"

// CDlg_EvidenceMethod 对话框

class CDlg_EvidenceMethod : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlg_EvidenceMethod)

public:
	CDlg_EvidenceMethod();
	virtual ~CDlg_EvidenceMethod();

	CDlg_PriorProbability *pDlgPriorProbability;
	CListCtrl m_CLC;

	std::vector<std::vector<long>> EnvidenceTrueIndex;
	std::vector<std::vector<long>> EnvidenceFalseIndex;

	std::vector<std::vector<long>> EvidenceExist;

	//std::vector<long> row;
	//std::vector<row> evidenceexist;
	//IcwVoxetPtr pvoxet;

// 对话框数据
	enum { IDD = IDD_DIALOG_EVIDENCEMETHOD };

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
	afx_msg void OnBnClickedDatastatistics();
private:
	std::vector<int> m_items;
};
