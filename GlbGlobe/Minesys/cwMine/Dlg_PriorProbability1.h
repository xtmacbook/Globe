#pragma once
//#include "UsefulUtility/DlgMultiExt.h"
//#include "afxwin.h"

#include "resource.h"
#include <vector>


// CDlg_PriorProbability �Ի���

class CDlg_PriorProbability : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlg_PriorProbability)

public:
	CDlg_PriorProbability();
	virtual ~CDlg_PriorProbability();

	//CDlgInput* m_pDlgInput;
	CDlg_PriorProbability *m_pDlgPriorProbability;
	//CListCtrl* m_list;


	bool isNew;
	IcwSceneViewPtr m_pSceneView;
	std::vector<IcwGeoObjectPtr> m_GeoObjects;
	std::vector<IcwGeoObjectPtr> m_RemoveObjects;

	// �Ի�������
	enum { IDD = IDD_DIALOG_PRIORPROBABILITY };

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	CListBox m_ListBox1;
	CListBox m_ListBox2;
	afx_msg void OnBnClickedLiulan();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	virtual BOOL OnWizardFinish();
	/*afx_msg void OnBnClickedButton1();*/
	//void RefreshControlList();
	/*afx_msg void OnBnClickedButton3();*/

	//����仯֪ͨ
	//virtual void OnInputDlgChanged(CDlgInput *m_pVoxet);

private:
	bool isFirst;
	bool controlmodify;
};







//class CDlg_PriorProbability : public CDialog
//{
//	DECLARE_DYNAMIC(CDlg_PriorProbability)
//
//public:
//	CDlg_PriorProbability(CWnd* pParent = NULL);   // ��׼���캯��
//	virtual ~CDlg_PriorProbability();
//
//// �Ի�������
//	enum { IDD = IDD_DIALOG_PRIORPROBABILITY };
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
//
//	DECLARE_MESSAGE_MAP()
//public:
//	afx_msg void OnBnClickedButton1();
//	afx_msg void OnBnClickedButton2();
//	afx_msg void OnBnClickedButton3();
//	afx_msg void OnBnClickedButton4();
//	CListBox m_ListBox1;
//	CListBox m_ListBox2;
//	afx_msg void OnBnClickedLiulan();
//};
//
//
////���������ĵ����Ի���
//class Dlg_PriorProbability :public CPropertyPage
//{
//	DECLARE_DYNAMIC(Dlg_PriorProbability)
//
//public:
//	Dlg_PriorProbability(){}
//	virtual ~Dlg_PriorProbability(){}
//
//	CDlgInput* m_pDlgInput;
//	CDlg_PriorProbability *m_pDlgPriorProbability;
//	//���غ���
//protected:
//	virtual BOOL OnInitDialog();
//	virtual void OnOK();
//
//	//����仯֪ͨ
//	virtual void OnInputDlgChanged(CDlgInput *m_pVoxet);
//};
