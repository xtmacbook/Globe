#pragma once
#include "UsefulUtility/DlgMultiExt.h"
#include "afxwin.h"
#include "Resource.h"
#include "Dlg_EvidenceMethod.h"
#include "afxcmn.h"

// CDlg_IndependenceTest 对话框

class CDlg_DataStatistics : public CDialog
{
	DECLARE_DYNAMIC(CDlg_DataStatistics)

public:
	CDlg_DataStatistics(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlg_DataStatistics();

// 对话框数据
	enum { IDD = IDD_DIALOG_DataStatistics };

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonstatistics();
public:
	IcwVoxetPtr m_pVoxet;
	afx_msg void OnBnClickedButtoncompute();
	afx_msg void OnBnClickedCheckbing();
	afx_msg void OnBnClickedCheckjiao();
	afx_msg void OnBnClickedCheckcha();
};
