#pragma once
#include "afxcmn.h"


// NewDlg 对话框

class NewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(NewDlg)

public:
	NewDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~NewDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk3();
	virtual BOOL OnInitDialog();
	void FillCtrlList(CString path,CString extension = _T("*"));
private:
	CListCtrl m_ctrlList;
	CString m_filePath;
public:
	afx_msg void OnBnClickedOk();
private:
	void DealLayer(CString file);
	void DealFeatureLayer(CString file);
	void DealDemLayer(CString file);
	void DealDomLayer(CString file);
};
