#pragma once
#include "UsefulUtility/DlgMultiExt.h"
#include "afxwin.h"


// CDlg_EvidenceMethod �Ի���

class CDlg_EvidenceMethod : public CDialog
{
	DECLARE_DYNAMIC(CDlg_EvidenceMethod)

public:
	CDlg_EvidenceMethod(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlg_EvidenceMethod();

// �Ի�������
	enum { IDD = IDD_DIALOG_EVIDENCEMETHOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ListBox1;
	CListBox m_ListBox2;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
};


//���������ĵ����Ի���
class Dlg_EvidenceMethod :public CPropertyPage
{
	DECLARE_DYNAMIC(Dlg_EvidenceMethod)

public:
	Dlg_EvidenceMethod(){}
	virtual ~Dlg_EvidenceMethod(){}

	CDlgInput* m_pDlgInput;
	CDlg_EvidenceMethod *m_pDlgEvidenceMethod;
	//���غ���
protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//����仯֪ͨ
	virtual void OnInputDlgChanged(CDlgInput *m_pVoxet);
};