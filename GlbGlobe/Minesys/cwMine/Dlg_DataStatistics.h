#pragma once
#include "UsefulUtility/DlgMultiExt.h"
#include "afxwin.h"
#include "Resource.h"
#include "Dlg_EvidenceMethod.h"
#include "afxcmn.h"

// CDlg_IndependenceTest �Ի���

class CDlg_DataStatistics : public CDialog
{
	DECLARE_DYNAMIC(CDlg_DataStatistics)

public:
	CDlg_DataStatistics(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlg_DataStatistics();

// �Ի�������
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
