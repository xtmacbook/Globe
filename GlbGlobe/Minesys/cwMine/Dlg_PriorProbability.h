#pragma once


#include "stdafx.h"
#include "UsefulUtility/DlgMultiExt.h"
#include "UsefulUtility/SelectionEvent.h"
#include "afxcmn.h"
#include <vector>
#include "afxwin.h"


// CDlg_PriorProbability �Ի���

class CDlg_PriorProbability : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlg_PriorProbability)

public:
	CDlg_PriorProbability();
	virtual ~CDlg_PriorProbability();
	

// �Ի�������
	enum { IDD = IDD_DIALOG_PRIORPROBABILITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	IcwVoxetPtr m_pVoxet;
	std::vector<float> MineValVec;
	std::vector<long> ResearchAreaTrueIndex;
	std::vector<long> ResearchAreaValue;
	std::vector<long> MineTrueIndex;//�洢�����ڵ�index
	std::vector<long> MineFalseIndex;//�洢��㲻���ڵ�index
	std::vector<long> ptT_T;//�����֤����
	std::vector<long> ptT_F;//�����֤�ݲ���
	std::vector<long> ptF_T;//��㲻��֤����
	std::vector<long> ptF_F;//��㲻��֤�ݲ���	
	std::vector<std::vector<long>> EnvidenceTrueIndex;//�洢�������Ӵ��ڵ�index
	std::vector<std::vector<long>> EnvidenceFalseIndex;//�洢�������Ӳ����ڵ�index

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

