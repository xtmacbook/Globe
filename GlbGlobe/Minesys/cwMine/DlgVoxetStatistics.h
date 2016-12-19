#pragma once
#include "afxcmn.h"
#include "Dlg_PriorProbability.h"


// CDlgVoxetStatistics �Ի���

class CDlgVoxetStatistics : public CDialog
{
	DECLARE_DYNAMIC(CDlgVoxetStatistics)

public:
	CDlgVoxetStatistics(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgVoxetStatistics();

// �Ի�������
	enum { IDD = IDD_STATISTICS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

public:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	CListCtrl m_Statistics;
	CDlg_PriorProbability* m_prior;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
