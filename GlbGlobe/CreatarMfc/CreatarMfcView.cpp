// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// CreatarMfcView.cpp : CCreatarMfcView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "CreatarMfc.h"
#endif

#include "CreatarMfcDoc.h"
#include "CreatarMfcView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCreatarMfcView

IMPLEMENT_DYNCREATE(CCreatarMfcView, CView)

BEGIN_MESSAGE_MAP(CCreatarMfcView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCreatarMfcView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_loadModel, &CCreatarMfcView::LoadModel)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_BUTTON5, &CCreatarMfcView::OnCreateDynamic)
	ON_COMMAND(ID_FeatureLayer, &CCreatarMfcView::OnFeaturelayer)
	ON_COMMAND(ID_BUTTON8, &CCreatarMfcView::OnButton8)
	ON_COMMAND(ID_BUTTON9, &CCreatarMfcView::OnCreateDomLayer)
	ON_COMMAND(ID_BUTTON10, &CCreatarMfcView::OnCreateDemLayer)
	ON_COMMAND(ID_TerrBuilder, &CCreatarMfcView::OnTerrBuilder)
END_MESSAGE_MAP()

// CCreatarMfcView ����/����

CCreatarMfcView::CCreatarMfcView()
{
	// TODO: �ڴ˴���ӹ������
	mpr_globe = NULL;
	mpr_globeview = NULL;
	mpr_globeviewHwnd = NULL;
}

CCreatarMfcView::~CCreatarMfcView()
{
	mpr_globe     = NULL;
	mpr_globeview = NULL;
}

BOOL CCreatarMfcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	return CView::PreCreateWindow(cs);
}

// CCreatarMfcView ����

void CCreatarMfcView::OnDraw(CDC* /*pDC*/)
{
	CCreatarMfcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CCreatarMfcView ��ӡ


void CCreatarMfcView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCreatarMfcView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CCreatarMfcView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CCreatarMfcView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CCreatarMfcView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCreatarMfcView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCreatarMfcView ���

#ifdef _DEBUG
void CCreatarMfcView::AssertValid() const
{
	CView::AssertValid();
}

void CCreatarMfcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCreatarMfcDoc* CCreatarMfcView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCreatarMfcDoc)));
	return (CCreatarMfcDoc*)m_pDocument;
}
#endif //_DEBUG


// CCreatarMfcView ��Ϣ�������
#include <iostream>
using std::wcout;
using std::endl;

#include "fstream"
#include "GlbConvert.h"
#include "GlbGlobePoint.h"
#include "GlbGlobeDynamicObject.h"

bool OpenFile(wchar_t* fileName,CGlbGlobe * globe)
{
	wcout.imbue( locale( "chs" ) );//���ñ��뷽��
	wifstream in;  
	in.open(fileName);                 //�����Լ���Ҫ�����ʵ���ѡȡ  

	if (!in.is_open())
		return false;	

	int count=0;
	//in >> count >> '/n';
	wchar_t newline[256];
	in.getline(newline,256);

	count = _wtoi(newline);

	//count = 300;
	wstring newLine;
	double lon,lat,alt;
	glbBool isNeedDirectDraw;	
	for (int i = 0; i < count; i++)
	{
		in.getline(newline,256);
		in.getline(newline,256);
		in.getline(newline,256);	
		std::wstring mdname(newline);

		in.getline(newline,256);	
		in.getline(newline,256);	
		in.getline(newline,256);	
		std::wstring mdlocate(newline);

		in.getline(newline,256);
		//wstringstream ss;
		//ss << std::wstring(newline);
		in >> lat >> lon >> alt;

		for (int j = 0; j <= 15; j++)
			in.getline(newline,256);	
		{
			CGlbGlobePoint* point = new CGlbGlobePoint();
			point->SetPosition(lon,lat,alt,isNeedDirectDraw);
			point->SetName((glbWChar*)(mdname.c_str()));			
			point->SetMinVisibleDistance(0.0);
			point->SetMaxVisibleDistance(10000.0);
			point->SetGroupId(-1);

			GlbMarkerModelSymbolInfo* info = new GlbMarkerModelSymbolInfo();		
			info->locate = new GlbRenderString();
			info->locate->bUseField = false;
			info->locate->value = mdlocate;
			info->opacity = new GlbRenderInt32();
			info->opacity->bUseField = false;
			info->opacity->value = 100;
			info->pitch = new GlbRenderDouble();
			info->pitch->bUseField = false;
			info->pitch->value = 0.0;
			info->roll = new GlbRenderDouble();
			info->roll->bUseField = false;
			info->roll->value = 0.0;
			info->yaw = new GlbRenderDouble();
			info->yaw->bUseField = false;
			info->yaw->value = 0.0;
			info->xScale = new GlbRenderDouble();
			info->xScale->bUseField = false;
			info->xScale->value = 1.0;
			info->yScale = new GlbRenderDouble();
			info->yScale->bUseField = false;
			info->yScale->value = 1.0;
			info->zScale = new GlbRenderDouble();
			info->zScale->bUseField = false;
			info->zScale->value = 1.0;

			point->SetRenderInfo(info,isNeedDirectDraw);
			globe->AddObject(point);

		}
	}
	in.close();
	return true;		 
}

void CCreatarMfcView::LoadModel()
{
	// TODO: �ڴ���������������
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
	}

	OpenFile(FilePathName.GetBuffer(FilePathName.GetLength()),mpr_globe.get());
}
#include "GlbDataSourceFactory.h"
int CCreatarMfcView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;

	// TODO:  �ڴ������ר�õĴ�������
	mpr_globeview = new CGlbGlobeView();	
	mpr_globeviewHwnd = mpr_globeview->Create(this->GetSafeHwnd());
	mpr_globe     = new CGlbGlobe(GLB_GLOBETYPE_GLOBE);

	CGlbDataSourceFactory* factory = CGlbDataSourceFactory::GetInstance();
	IGlbDataSource * ds = factory->OpenDataSource(L"D:\\CGlobe\\trunck_new\\bin\\X86_32\\debug\\res\\",NULL,NULL,L"file");/*file*/
	if (ds)
	{
		IGlbRasterDataSource *rds = NULL;
		ds->QueryInterface(L"IGlbRasterDataSource",(void**)&rds);
		IGlbTerrainDataset* tdset = rds->GetTerrainDataset(L"globeWorld.ted");
		if (tdset)
			mpr_globe->SetTerrainDataset(tdset);
	}
	mpr_globeview->SetGlobe(mpr_globe.get());

	return 0;
}


BOOL CCreatarMfcView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	switch(pMsg->message)
	{
	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
		{
			if(mpr_globeviewHwnd)
			{
				::SendMessage(mpr_globeviewHwnd,pMsg->message,pMsg->wParam,pMsg->lParam);
				return TRUE;
			}
		}
	}
	return CView::PreTranslateMessage(pMsg);
}


void CCreatarMfcView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	if(mpr_globeview)
	{
		mpr_globeview->SetWinPosition(0,0,cx,cy);
	}
}


void CCreatarMfcView::OnCreateDynamic()
{
	// TODO: �ڴ���������������
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		glbref_ptr<CGlbGlobeDynamicObject> obj = new CGlbGlobeDynamicObject();
		obj->SetGlobe(mpr_globe.get());
		obj->SetDynamicObjMode(GLB_DYNAMICMODE_VIRTUAL);
		obj->SetTraceMode(GLB_DYNAMICTRACMODE_FIRST);
		obj->testDynamicObject(FilePathName.GetBuffer(0));
		mpr_globeview->SetPathMode(obj.get());
	}
}

#include "NewDlg.h"
void CCreatarMfcView::OnFeaturelayer()
{
	// TODO: �ڴ���������������
	//AfxMessageBox(L"OnFeaturelayer");
	NewDlg* nDlg = new NewDlg;	
	nDlg->Create(IDD_DIALOG1);
	nDlg->ShowWindow(SW_SHOW);
}


void CCreatarMfcView::OnButton8()
{
	// TODO: �ڴ���������������
	//AfxMessageBox(L"OnButton8");
}

void CCreatarMfcView::OnCreateDomLayer()
{
	//AfxMessageBox(L"OnCreateDomLayer");
	NewDlg* nDlg = new NewDlg;	
	nDlg->Create(IDD_DIALOG1);
	nDlg->ShowWindow(SW_SHOW);
	nDlg->FillCtrlList(_T("D:\\CGlobe\\trunck_new\\bin\\X86_32\\debug\\res"));
	nDlg->SetWindowText(_T("���Ӱ��ͼ��"));
	nDlg->GetDlgItem(IDC_STATIC)->SetWindowText(_T("ѡ��Ӱ��ͼ��"));
}

void CCreatarMfcView::OnCreateDemLayer()
{
	//AfxMessageBox(L"OnCreateDemLayer");
	NewDlg* nDlg = new NewDlg;
	nDlg->Create(IDD_DIALOG1);
	nDlg->ShowWindow(SW_SHOW);
	nDlg->FillCtrlList(_T("D:\\CGlobe\\trunck_new\\bin\\X86_32\\debug\\res"));
	nDlg->SetWindowText(_T("��Ӹ߳�ͼ��"));
	nDlg->GetDlgItem(IDC_STATIC)->SetWindowText(_T("ѡ��߳�ͼ��"));
}

void CCreatarMfcView::OnTerrBuilder()
{
	// TODO: �ڴ���������������
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		int which = FilePathName.ReverseFind('\\');
		CString name = FilePathName.Right(FilePathName.GetLength() - which - 1);
		CString msg = L"�Ƿ񴴽�����\"" + name + L"\"������";
		if(::MessageBox(m_hWnd,msg,_T("����������"),MB_OKCANCEL) == IDCANCEL)
			return;

		CGlbDataSourceFactory* factory = CGlbDataSourceFactory::GetInstance();
		if(!factory)
			return;
		CString path = FilePathName.Left(which + 1);
		IGlbDataSource* dataSource = factory->OpenDataSource(path,NULL,NULL);
		if(dataSource)
		{
			IGlbRasterDataSource* rds = NULL;
			dataSource->QueryInterface(L"IGlbRasterDataSource",(void**)&rds);
			IGlbRasterDataset* rdset = rds->GetRasterDataset(name);
			if(rdset)
			{
				glbInt32 dsCols,dsRows;
				rdset->GetSize(&dsCols,&dsRows);
				glbInt32 cols = 5000,rows = 5000;
				glbInt32 colTimes = dsCols/cols + 1;
				glbInt32 rowTimes= dsRows/rows + 1;
				for(glbInt32 r = 0;r < rowTimes;r++)
				{
					for(glbInt32 c = 0;c < colTimes;c++)
						rdset->BuildPyramid(c * cols,r * rows,
									 (c + 1)*cols,(r + 1)*rows,GLB_RASTER_NEAREST,256,256);
				}
			}
		}
	}
}
