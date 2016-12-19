#pragma once

#include "stdafx.h"
#include "UsefulUtility/DlgMultiExt.h"
#include "UsefulUtility/SelectionEvent.h"
#include "afxcmn.h"
#include <vector>
#include "afxwin.h"

// CDlg_InformationCalculate 对话框
class CDlg_InformationCalculate : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlg_InformationCalculate)

public:
	CDlg_InformationCalculate();
	virtual ~CDlg_InformationCalculate();

	// 对话框数据
	enum { IDD = IDD_DIALOG_INFORMATIONCALCULATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()

public:
	IcwVoxetPtr m_pVoxet;
	std::vector<float> MineValVec;	
	std::vector<float> ResearchAreaValue;
	std::vector<long> vecMine_Factor;//含矿含因子体素	
	std::vector<long> vecFactor;//含因子体素
	long nResearch;//研究区体素
	long nMine;//研究区含矿体素

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
	
	CListCtrl m_ListCtrl;	
	CComboBox m_combobox;
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox m_ComboBoxMine;
	afx_msg void OnCbnSelchangeComboMine();
	afx_msg void OnBnClickedInformationout();
	afx_msg void OnBnClickedVoxetstatistics();
	CComboBox m_ComboBoxResearchArea;
	afx_msg void OnCbnSelchangeComboResearcharea();
};

