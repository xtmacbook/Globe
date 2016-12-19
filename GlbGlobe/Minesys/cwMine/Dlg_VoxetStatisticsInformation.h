#pragma once
#include "afxcmn.h"
#include "Dlg_InformationCalculate.h"


// CDlgVoxetStatisticsInformation 对话框

class CDlgVoxetStatisticsInformation : public CDialog
{
	DECLARE_DYNAMIC(CDlgVoxetStatisticsInformation)

public:
	CDlgVoxetStatisticsInformation(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgVoxetStatisticsInformation();

	// 对话框数据
	enum { IDD = IDD_STATISTICS_INFORMATION};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	CListCtrl m_Statistics;
	CDlg_InformationCalculate* m_prior;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
