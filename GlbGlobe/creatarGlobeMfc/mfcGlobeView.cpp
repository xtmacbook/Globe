
// GlobeView.cpp : implementation of the CGlobeView class
//

#include "stdafx.h"
#include "creatarGlobeMfc.h"
#include "mfcGlobeView.h"
#include "fstream"
#include "GlbConvert.h"
#include "GlbGlobePoint.h"
#include "GlbGlobeDynamicObject.h"
#include "GlbDataSourceFactory.h"
#include "GlbWString.h"
#include "GlbPath.h"
#include "GlbFeature.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGlobeView

CGlobeView::CGlobeView()
{
	mpr_globe     = NULL;
	mpr_globeview = NULL;
	mpr_globeviewHwnd=NULL;

	std::vector<int> vv;
	vv.reserve(100);
	size_t kk = vv.size();
	kk = 0;
}

CGlobeView::~CGlobeView()
{
}


BEGIN_MESSAGE_MAP(CGlobeView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_CGGLOBE_LOADMODEL, &CGlobeView::OnCgglobeLoadmodel)
	ON_COMMAND(ID_CGGLOBE_SAVESCENE, &CGlobeView::OnCgglobeSavescene)
	ON_COMMAND(ID_CGGLOBE_OPENSCENE, &CGlobeView::OnCgglobeOpenscene)
	ON_COMMAND(ID_CGGLOBE_CREATED, &CGlobeView::OnCgglobeCreated)
	ON_COMMAND(ID_FileDS, &CGlobeView::OnFileds)
END_MESSAGE_MAP()



// CGlobeView message handlers

BOOL CGlobeView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CGlobeView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}



int CGlobeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	mpr_globeview       = new CGlbGlobeView();	
	mpr_globeviewHwnd   = mpr_globeview->Create(this->GetSafeHwnd());
	mpr_globe           = new CGlbGlobe(GLB_GLOBETYPE_GLOBE);
	//CGlbWString tedPath = CGlbPath::GetExecDir();
	CGlbWString tedPath = L"D:\\CGlobe\\trunck_new\\bin\\X86_32\\release\\";
	tedPath.append(L"res\\");
	//tedPath = L"D:\\CreatarGlobe\\trunck2014\\trunck_new\\bin\\X86_32\\release\\";
	CGlbDataSourceFactory* factory = CGlbDataSourceFactory::GetInstance();
	IGlbDataSource * ds = factory->OpenDataSource(tedPath.c_str(),NULL,NULL,L"file");
	if (ds)
	{
		IGlbRasterDataSource *rds = NULL;
		ds->QueryInterface(L"IGlbRasterDataSource",(void**)&rds);
		IGlbTerrainDataset* tdset = rds->GetTerrainDataset(L"globeWorld.ted");
		if (tdset)
			mpr_globe->SetTerrainDataset(tdset);
	}
	mpr_globeview->SetGlobe(mpr_globe.get());
	
	mpr_globeview->Run();
	return 0;
}


void CGlobeView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(mpr_globeview)
	{
		mpr_globeview->SetWinPosition(0,0,cx,cy);
	}
}


BOOL CGlobeView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message)
	{
	case WM_MOUSEWHEEL:
	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_SYSKEYUP:	
	case WM_SYSKEYDOWN:	
	//case WM_MOUSEWHEEL:
		{
			if(mpr_globeviewHwnd)
			{
				::SendMessage(mpr_globeviewHwnd,pMsg->message,pMsg->wParam,pMsg->lParam);
				return TRUE;
			}
		}
		break;
	}
	return CWnd::PreTranslateMessage(pMsg);
}

//将单字节char*转化为宽字节wchar_t*  
wchar_t* AnsiToUnicode( const char* szStr )  
{  
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );  
	if (nLen == 0)  
	{  
		return NULL;  
	}  
	wchar_t* pResult = new wchar_t[nLen];  
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );  
	return pResult;  
}  

//将宽字节wchar_t*转化为单字节char*  
inline char* UnicodeToAnsi( const wchar_t* szStr )  
{  
	int nLen = WideCharToMultiByte( CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL );  
	if (nLen == 0)  
	{  
		return NULL;  
	}  
	char* pResult = new char[nLen];  
	WideCharToMultiByte( CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL );  
	return pResult;  
}

#include <iostream>
using std::wcout;
using std::endl;

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

void CGlobeView::OnCgglobeLoadmodel()
{
	// TODO: 在此添加命令处理程序代码
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
	}

	//OpenFile(FilePathName.GetBuffer(FilePathName.GetLength()),mpr_globe.get());

	glbBool isNeedDirectDraw = false;
	CGlbGlobePoint* point = new CGlbGlobePoint();
	point->SetPosition(0,0,0,isNeedDirectDraw);
	point->SetMinVisibleDistance(0.0);
	point->SetMaxVisibleDistance(10000000000000.0);
	point->SetGroupId(-1);

	GlbMarkerModelSymbolInfo* info = new GlbMarkerModelSymbolInfo();		
	info->locate = new GlbRenderString();
	info->locate->bUseField = false;
	info->locate->value = L"E:/juminqumoxingosgb/juminqumoxing_23w-GEODE.osgb";
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
	mpr_globe->AddObject(point);
}


void CGlobeView::OnCgglobeSavescene()
{
	// TODO: 在此添加命令处理程序代码
	CString FilePathName;
	CFileDialog dlg(FALSE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		mpr_globe->SaveAs(FilePathName.GetBuffer(0));
	}
}


void CGlobeView::OnCgglobeOpenscene()
{
	// TODO: 在此添加命令处理程序代码
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		mpr_globe->Open(FilePathName.GetBuffer(0));
	}
}


void CGlobeView::OnCgglobeCreated()
{
	// TODO: 在此添加命令处理程序代码
	CString FilePathName;
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()==IDOK)
	{
		FilePathName=dlg.GetPathName();
		glbref_ptr<CGlbGlobeDynamicObject> obj = new CGlbGlobeDynamicObject();
		////载入模型，点模型
		//glbref_ptr<GlbMarkerModelSymbolInfo> modelInfo = new GlbMarkerModelSymbolInfo;
		//modelInfo->type = GLB_OBJECTTYPE_POINT;
		//modelInfo->symType = GLB_MARKERSYMBOL_MODEL;
		//modelInfo->xScale = new GlbRenderDouble;
		//modelInfo->xScale->value = 5;
		//modelInfo->yScale = new GlbRenderDouble;
		//modelInfo->yScale->value = 5;
		//modelInfo->zScale = new GlbRenderDouble;
		//modelInfo->zScale->value = 5;
		//modelInfo->yaw = new GlbRenderDouble;
		//modelInfo->yaw->value = 0.0;
		//modelInfo->pitch = new GlbRenderDouble;
		//modelInfo->pitch->value = 0.0;
		//modelInfo->roll = new GlbRenderDouble;
		//modelInfo->roll->value = 0.0;
		//modelInfo->locate = new GlbRenderString;
		//modelInfo->locate->value = L"";
		//modelInfo->opacity = new GlbRenderInt32;
		//modelInfo->opacity->value = 100;
		glbBool isNeedDirectDraw;
		obj->SetAltitudeMode(GLB_ALTITUDEMODE_ABSOLUTE,isNeedDirectDraw);	
		//obj->SetRenderInfo(modelInfo.get(),isNeedDirectDraw);
		obj->SetInViewPort(true);
		obj->SetGlobe(mpr_globe.get());

		mpr_globe->AddObject(obj.get());		
		obj->SetDynamicObjMode(GLB_DYNAMICMODE_VIRTUAL);
		obj->SetTraceMode(GLB_DYNAMICTRACMODE_FIRST);

		obj->read_3dmaxpath(FilePathName.GetBuffer(0));
		//开启动画回调
		obj->SetRepeat(true);		
		obj->SetPlayMode(GLB_DYNAMICPLAYMODE_CAR);				
		obj->Run();
		mpr_globeview->SetPathMode(obj.get());
	}
}

/*
	文件数据源
*/
void CGlobeView::OnFileds()
{
	CGlbLine line;
	{
		double coords[4];
		coords[0] = coords[1] = 1.0;
		coords[2] = coords[3] = 2.0;
		line.InsertPoints(0,coords,2);
		
		coords[0] = coords[1] = -1.0;
		coords[2] = coords[3] = 0.0;
		line.InsertPoints(0,coords,2);

		coords[0] = coords[1] = 3.0;
		coords[2] = coords[3] = 4.0;
		line.InsertPoints(4,coords,2);

		coords[0] = coords[1] = 5.0;
		coords[2] = coords[3] = 6.0;
		line.InsertPoints(2,coords,2);

		glbInt32 dv = 1;
		CGlbField* field = new CGlbField();
		field->SetName(L"f1");
		field->SetType(GLB_DATATYPE_INT32);
		field->SetCanNull(false);
		field->SetDefaultNumber(&dv);
		CGlbGeometryClass* gcls = new CGlbGeometryClass();
		gcls->AddField(field);

		//line.SetPointClass(gcls);		

		//GLBVARIANT var;
		//line.GetPointsValue(0,var);
		//glbVariantClean(var);
	}
	CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
	if(dlg.DoModal()!= IDOK)return;
	
	CGlbWString filepath = dlg.GetPathName();
	CGlbWString dir      = CGlbPath::GetDir(filepath);
	//测试打开.
	CGlbDataSourceFactory* dsf = CGlbDataSourceFactory::GetInstance();
	IGlbDataSource*        ds  = dsf->OpenDataSource(dir.c_str(),
		                                             NULL,
													 NULL,
													 L"file");
	//2. shape 文件
	IGlbFeatureClass* dataset = 
		            dynamic_cast<IGlbFeatureClass*>(ds->GetDatasetByName(L"市区杂路.shp"));
	CGlbFields*       fields  = (CGlbFields*)dataset->GetFields();
	for(int i=0;i<fields->GetFieldCount();i++)
	{
		CGlbField* field = (CGlbField*)fields->GetField(i);
		field->GetName();
	}
	IGlbFeatureCursor* cursor = dataset->Query(NULL);
	CGlbFeature* feature = cursor->GetFeature();
}
