#pragma once


// NewDlg2 �Ի���

class NewDlg2 : public CDialogEx
{
	DECLARE_DYNAMIC(NewDlg2)

public:
	NewDlg2(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~NewDlg2();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	void SetBackDlg(CDialogEx *dlg);
private:
	CDialogEx *m_backDlg;
	CString m_strSavePath;
};
