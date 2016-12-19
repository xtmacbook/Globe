#pragma once


#include "stdafx.h"
#include "UsefulUtility/DlgMultiExt.h"
#include "UsefulUtility/SelectionEvent.h"
#include "afxcmn.h"
#include <vector>
#include "afxwin.h"


// CDlg_PriorProbability 对话框

class CDlg_PriorProbability : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlg_PriorProbability)

public:
	CDlg_PriorProbability();
	virtual ~CDlg_PriorProbability();
	

// 对话框数据
	enum { IDD = IDD_DIALOG_PRIORPROBABILITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	IcwVoxetPtr m_pVoxet;
	std::vector<float> MineValVec;
	std::vector<long> ResearchAreaTrueIndex;
	std::vector<long> ResearchAreaValue;
	std::vector<long> MineTrueIndex;//存储矿点存在的index
	std::vector<long> MineFalseIndex;//存储矿点不存在的index
	std::vector<long> ptT_T;//矿点在证据在
	std::vector<long> ptT_F;//矿点在证据不在
	std::vector<long> ptF_T;//矿点不在证据在
	std::vector<long> ptF_F;//矿点不在证据不在	
	std::vector<std::vector<long>> EnvidenceTrueIndex;//存储有利因子存在的index
	std::vector<std::vector<long>> EnvidenceFalseIndex;//存储有利因子不存在的index

public:
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual LRESULT OnWizardNext();

	//对象名称集合
	CString m_ObjNames;
	CListBox m_ListBox1;
	CListBox m_ListBox2;
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	
	afx_msg void OnBnClickedButton5();
	//CListCtrl m_ListC;
	//CListCtrl reqwrw;
	CListCtrl m_ListCtrl;
	//CString m_MineName;
	CComboBox m_combobox;
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox m_ComboBoxMine;
	afx_msg void OnCbnSelchangeComboMine();
	afx_msg void OnBnClickedPriorprobabilityout();
	afx_msg void OnBnClickedVoxetstatistics();
	CComboBox m_ComboBoxResearchArea;
	afx_msg void OnCbnSelchangeComboResearcharea();
};

