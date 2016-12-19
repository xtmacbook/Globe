#pragma once
#include "afxcmn.h"
#include "Dlg_PriorProbability.h"


// CDlgVoxetStatistics 对话框

class CDlgVoxetStatistics : public CDialog
{
	DECLARE_DYNAMIC(CDlgVoxetStatistics)

public:
	CDlgVoxetStatistics(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgVoxetStatistics();

// 对话框数据
	enum { IDD = IDD_STATISTICS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	CListCtrl m_Statistics;
	CDlg_PriorProbability* m_prior;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
