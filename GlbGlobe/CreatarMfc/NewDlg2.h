#pragma once


// NewDlg2 对话框

class NewDlg2 : public CDialogEx
{
	DECLARE_DYNAMIC(NewDlg2)

public:
	NewDlg2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~NewDlg2();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	void SetBackDlg(CDialogEx *dlg);
private:
	CDialogEx *m_backDlg;
	CString m_strSavePath;
};
