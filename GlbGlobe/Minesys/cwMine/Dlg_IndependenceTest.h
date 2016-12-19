#pragma once
#include "UsefulUtility/DlgMultiExt.h"
#include "afxwin.h"
#include "Resource.h"
#include "Dlg_EvidenceMethod.h"
#include "afxcmn.h"

// CDlg_IndependenceTest 对话框

class CDlg_IndependenceTest : public CDialog
{
	DECLARE_DYNAMIC(CDlg_IndependenceTest)

public:
	CDlg_IndependenceTest(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlg_IndependenceTest();

// 对话框数据
	enum { IDD = IDD_DIALOG_INDEPENDENCETEST };

public:
	IcwVoxetPtr pVoxet;
	CDlg_PriorProbability *pDlgPriorProbability;
	//CDlg_PriorProbability *pDlgPriorProbability;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	CListBox m_Listindependence;
	CListBox m_Listindependenceselect;
	
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnCbnSelchangeSignificancelevel();
	CListCtrl m_ListConditionTest;
	afx_msg void OnBnClickedIndependencecalculate();
};
