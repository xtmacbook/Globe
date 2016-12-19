// NewDlg2.cpp : 实现文件
//

#include "stdafx.h"
#include "CreatarMfc.h"
#include "NewDlg2.h"
#include "afxdialogex.h"


// NewDlg2 对话框

IMPLEMENT_DYNAMIC(NewDlg2, CDialogEx)

NewDlg2::NewDlg2(CWnd* pParent /*=NULL*/)
	: CDialogEx(NewDlg2::IDD, pParent)
{
	m_backDlg = NULL;
}

NewDlg2::~NewDlg2()
{
}

void NewDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(NewDlg2, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &NewDlg2::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &NewDlg2::OnBnClickedOk)
END_MESSAGE_MAP()


// NewDlg2 消息处理程序
void NewDlg2::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	WCHAR szDir[MAX_PATH]; //依旧ansi和unicode更换类型
	BROWSEINFO bi;
	ITEMIDLIST *pidl;
	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = L"请选择目录";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	pidl = SHBrowseForFolder(&bi);
	if(pidl == NULL)
		return;
	if(SHGetPathFromIDList(pidl, szDir))
	{
		m_strSavePath.Format(L"%s", szDir);
		GetDlgItem(IDC_EDIT3)->SetWindowText(m_strSavePath);
	}
}
#include "NewDlg.h"
void NewDlg2::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_backDlg)
	{
		NewDlg *dlg = dynamic_cast<NewDlg *>(m_backDlg);
		if(dlg)
		{
			CString text;
			m_backDlg->GetDlgItem(IDC_STATIC)->GetWindowText(text);
			if(text == "选择一个要素数据集")
				text = "shp";
			else if(text == "选择影像图层")
				text = "jpg.pyd";
			else if(text == "选择高程图层")
				text = "tif.pyd";
			dlg->FillCtrlList(m_strSavePath,text);
		}
	}
	CDialogEx::OnOK();
}

void NewDlg2::SetBackDlg( CDialogEx *dlg )
{
	if(dlg)
		m_backDlg = dlg;
}