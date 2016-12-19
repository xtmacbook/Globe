// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// CreatarMfcView.cpp : CCreatarMfcView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
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

// CCreatarMfcView 构造/析构

CCreatarMfcView::CCreatarMfcView()
{
	// TODO: 在此处添加构造代码
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
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	return CView::PreCreateWindow(cs);
}

// CCreatarMfcView 绘制

void CCreatarMfcView::OnDraw(CDC* /*pDC*/)
{
	CCreatarMfcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CCreatarMfcView 打印


void CCreatarMfcView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCreatarMfcView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCreatarMfcView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCreatarMfcView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
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


// CCreatarMfcView 诊断

#ifdef _DEBUG
void CCreatarMfcView::AssertValid() const
{
	CView::AssertValid();
}

void CCreatarMfcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCreatarMfcDoc* CCreatarMfcView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCreatarMfcDoc)));
	return (CCreatarMfcDoc*)m_pDocument;
}
#endif //_DEBUG


// CCreatarMfcView 消息处理程序
#include <iostream>
using std::wcout;
using std::endl;

#include "fstream"
#include "GlbConvert.h"
#include "GlbGlobePoint.h"
#include "GlbGlobeDynamicObject.h"

bool OpenFile(wchar_t* fileName,CGlbGlobe * globe)
{
	wcout.imbue( locale( "chs" ) );//设置编码方案
	wifstream in;  
	in.open(fileName);                 //根据自己需要进行适当的选取  

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
	// TODO: 在此添加命令处理程序代码
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
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

	// TODO:  在此添加您专用的创建代码
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
	// TODO: 在此添加专用代码和/或调用基类
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

	// TODO: 在此处添加消息处理程序代码
	if(mpr_globeview)
	{
		mpr_globeview->SetWinPosition(0,0,cx,cy);
	}
}


void CCreatarMfcView::OnCreateDynamic()
{
	// TODO: 在此添加命令处理程序代码
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
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
	// TODO: 在此添加命令处理程序代码
	//AfxMessageBox(L"OnFeaturelayer");
	NewDlg* nDlg = new NewDlg;	
	nDlg->Create(IDD_DIALOG1);
	nDlg->ShowWindow(SW_SHOW);
}


void CCreatarMfcView::OnButton8()
{
	// TODO: 在此添加命令处理程序代码
	//AfxMessageBox(L"OnButton8");
}

void CCreatarMfcView::OnCreateDomLayer()
{
	//AfxMessageBox(L"OnCreateDomLayer");
	NewDlg* nDlg = new NewDlg;	
	nDlg->Create(IDD_DIALOG1);
	nDlg->ShowWindow(SW_SHOW);
	nDlg->FillCtrlList(_T("D:\\CGlobe\\trunck_new\\bin\\X86_32\\debug\\res"));
	nDlg->SetWindowText(_T("添加影像图层"));
	nDlg->GetDlgItem(IDC_STATIC)->SetWindowText(_T("选择影像图层"));
}

void CCreatarMfcView::OnCreateDemLayer()
{
	//AfxMessageBox(L"OnCreateDemLayer");
	NewDlg* nDlg = new NewDlg;
	nDlg->Create(IDD_DIALOG1);
	nDlg->ShowWindow(SW_SHOW);
	nDlg->FillCtrlList(_T("D:\\CGlobe\\trunck_new\\bin\\X86_32\\debug\\res"));
	nDlg->SetWindowText(_T("添加高程图层"));
	nDlg->GetDlgItem(IDC_STATIC)->SetWindowText(_T("选择高程图层"));
}

void CCreatarMfcView::OnTerrBuilder()
{
	// TODO: 在此添加命令处理程序代码
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		int which = FilePathName.ReverseFind('\\');
		CString name = FilePathName.Right(FilePathName.GetLength() - which - 1);
		CString msg = L"是否创建关于\"" + name + L"\"金字塔";
		if(::MessageBox(m_hWnd,msg,_T("创建金字塔"),MB_OKCANCEL) == IDCANCEL)
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
