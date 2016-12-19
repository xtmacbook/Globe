// NewDlg2.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CreatarMfc.h"
#include "NewDlg2.h"
#include "afxdialogex.h"


// NewDlg2 �Ի���

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


// NewDlg2 ��Ϣ�������
void NewDlg2::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	WCHAR szDir[MAX_PATH]; //����ansi��unicode��������
	BROWSEINFO bi;
	ITEMIDLIST *pidl;
	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = L"��ѡ��Ŀ¼";
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_backDlg)
	{
		NewDlg *dlg = dynamic_cast<NewDlg *>(m_backDlg);
		if(dlg)
		{
			CString text;
			m_backDlg->GetDlgItem(IDC_STATIC)->GetWindowText(text);
			if(text == "ѡ��һ��Ҫ�����ݼ�")
				text = "shp";
			else if(text == "ѡ��Ӱ��ͼ��")
				text = "jpg.pyd";
			else if(text == "ѡ��߳�ͼ��")
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