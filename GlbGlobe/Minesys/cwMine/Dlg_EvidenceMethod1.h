#pragma once
#include "UsefulUtility/DlgMultiExt.h"
#include "afxwin.h"


// CDlg_EvidenceMethod 对话框

class CDlg_EvidenceMethod : public CDialog
{
	DECLARE_DYNAMIC(CDlg_EvidenceMethod)

public:
	CDlg_EvidenceMethod(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlg_EvidenceMethod();

// 对话框数据
	enum { IDD = IDD_DIALOG_EVIDENCEMETHOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ListBox1;
	CListBox m_ListBox2;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
};


//定义真正的弹出对话框
class Dlg_EvidenceMethod :public CPropertyPage
{
	DECLARE_DYNAMIC(Dlg_EvidenceMethod)

public:
	Dlg_EvidenceMethod(){}
	virtual ~Dlg_EvidenceMethod(){}

	CDlgInput* m_pDlgInput;
	CDlg_EvidenceMethod *m_pDlgEvidenceMethod;
	//重载函数
protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//输入变化通知
	virtual void OnInputDlgChanged(CDlgInput *m_pVoxet);
};