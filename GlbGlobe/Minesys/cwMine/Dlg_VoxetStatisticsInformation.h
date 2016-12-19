#pragma once
#include "afxcmn.h"
#include "Dlg_InformationCalculate.h"


// CDlgVoxetStatisticsInformation �Ի���

class CDlgVoxetStatisticsInformation : public CDialog
{
	DECLARE_DYNAMIC(CDlgVoxetStatisticsInformation)

public:
	CDlgVoxetStatisticsInformation(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgVoxetStatisticsInformation();

	// �Ի�������
	enum { IDD = IDD_STATISTICS_INFORMATION};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

public:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	CListCtrl m_Statistics;
	CDlg_InformationCalculate* m_prior;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
