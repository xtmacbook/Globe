#pragma once

#include "stdafx.h"
#include "UsefulUtility/DlgMultiExt.h"
#include "UsefulUtility/SelectionEvent.h"
#include "afxcmn.h"
#include <vector>
#include "afxwin.h"

// CDlg_InformationCalculate �Ի���
class CDlg_InformationCalculate : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlg_InformationCalculate)

public:
	CDlg_InformationCalculate();
	virtual ~CDlg_InformationCalculate();

	// �Ի�������
	enum { IDD = IDD_DIALOG_INFORMATIONCALCULATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()

public:
	IcwVoxetPtr m_pVoxet;
	std::vector<float> MineValVec;	
	std::vector<float> ResearchAreaValue;
	std::vector<long> vecMine_Factor;//������������	
	std::vector<long> vecFactor;//����������
	long nResearch;//�о�������
	long nMine;//�о�����������

public:
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual LRESULT OnWizardNext();

	//�������Ƽ���
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

