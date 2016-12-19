// NewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CreatarMfc.h"
#include "NewDlg.h"
#include "afxdialogex.h"


// NewDlg 对话框

IMPLEMENT_DYNAMIC(NewDlg, CDialogEx)

NewDlg::NewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(NewDlg::IDD, pParent)
{

}

NewDlg::~NewDlg()
{
}

void NewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_ctrlList);
}


BEGIN_MESSAGE_MAP(NewDlg, CDialogEx)
	ON_BN_CLICKED(IDOK3, &NewDlg::OnBnClickedOk3)
	ON_BN_CLICKED(IDOK, &NewDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// NewDlg 消息处理程序

#include "NewDlg2.h"
void NewDlg::OnBnClickedOk3()
{
	// TODO: 在此添加控件通知处理程序代码
	NewDlg2 nDlg;
	nDlg.SetBackDlg(this);
	nDlg.DoModal();
}

BOOL NewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ctrlList.InsertColumn( 0, _T("名称"), LVCFMT_LEFT,200 );//插入列
	m_ctrlList.InsertColumn( 1, _T("类型"), LVCFMT_LEFT, 500 );
	//int nRow = m_ctrlList.InsertItem(0, _T("11"));//插入行
	//m_ctrlList.SetItemText(nRow, 1, _T("jacky"));//设置数据

	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void NewDlg::FillCtrlList(CString path,CString extension)
{
	if(path == _T(""))
		return;
	else
	{
		if(path.Right(1) != _T("//"))
			path += L"//";
		m_filePath = path;
		path += _T("*.");
		path += extension;
	}
	CFileFind finder;
	CString name;
	int nRow = 0;
	bool find = finder.FindFile(path);
	m_ctrlList.DeleteAllItems();
	while(find)
	{
		find = finder.FindNextFile();
		name = finder.GetFileName();
		nRow = m_ctrlList.InsertItem(0, name);//插入行
		m_ctrlList.SetItemText(nRow, 1, _T("数据集"));//设置数据
	}
}


void NewDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	CString str;
	int nItemIndex = -1;
	for(int i=0; i<m_ctrlList.GetItemCount(); i++)
	{
		if( m_ctrlList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED )
		{
			str.Format(_T("选中了第%d行"), i);
			nItemIndex = i;
			//AfxMessageBox(str);
		}
	}

	TCHAR szBuf[1024];
	LVITEM lvi;
	lvi.iItem = nItemIndex;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szBuf;
	lvi.cchTextMax = 1024;
	m_ctrlList.GetItem(&lvi);

	//AfxMessageBox(lvi.pszText);

	DealLayer(m_filePath + lvi.pszText);
	CDialogEx::OnOK();
}

void NewDlg::DealLayer( CString file )
{
	int Which = file.ReverseFind('.');  
	CString ExtendName=file.Right(file.GetLength()-Which-1);
	CString tile;
	GetWindowText(tile);
	if(ExtendName == "shp" && tile == "添加要素图层")
		DealFeatureLayer(file);
	if(ExtendName == "pyd")
	{
		ExtendName=file.Left(file.GetLength() - 4);
		Which = ExtendName.ReverseFind('.');
		ExtendName = ExtendName.Right(ExtendName.GetLength()-Which-1);
		if(ExtendName == "tif" && tile == "添加高程图层")
			DealDemLayer(file);
		else if(ExtendName == "jpg" && tile == "添加影像图层")
			DealDomLayer(file);
	}
}

void NewDlg::DealFeatureLayer( CString file )
{
	AfxMessageBox(_T("DealFeatureLayer"));
	//CGlbGlobeFeatureLayer* p_featurelayer = new CGlbGlobeFeatureLayer();
	//IGlbFeatureDataSource* fds = NULL;
	//ds->QueryInterface(L"IGlbFeatureDataSource",(void**)&fds);
	//IGlbDataset* _dataset = fds->GetDatasetByName(L"CITIES.shp");
	//IGlbFeatureClass* fclss = dynamic_cast<IGlbFeatureClass*>(_dataset);
	//p_featurelayer->SetDataset(fclss);
	//p_featurelayer->SetDefaultVisibleDistance(100000000.0);
	//CGlbGlobeRenderer* renderer = new CGlbGlobeRenderer();
	//renderer->SetAltitudeMode(GLB_ALTITUDEMODE_ABSOLUTE);
	//renderer->SetGeoField(L"GLBGEO");
	//renderer->SetGround(true);
	//GlbRenderInt32* loadorder = new GlbRenderInt32;
	//loadorder->bUseField=false;
	//loadorder->value = 1;
	//renderer->SetLoadOrder(loadorder);
	//GlbRenderDouble* maxDistance = new GlbRenderDouble;
	//maxDistance->bUseField = false;
	//maxDistance->value = 100000000.0;
	//renderer->SetMaxDistance(maxDistance);
	//GlbRenderDouble* minDistance = new GlbRenderDouble;
	//minDistance->bUseField = false;
	//minDistance->value = 0.0;
	//renderer->SetMinDistance(minDistance);
	//GlbRenderInt32* renderorder = new GlbRenderInt32;
	//renderorder->bUseField=false;
	//renderorder->value = 1;
	//renderer->SetRenderOrder(renderorder);
	//GlbRenderString* message = new GlbRenderString;
	//message->bUseField=false;
	//message->value=L"";
	//renderer->SetRMessage(message);
	//GlbRenderString* tooltip = new GlbRenderString;
	//tooltip->bUseField=false;
	//tooltip->value=L"";
	//renderer->SetToolTip(tooltip);

	//GlbMarker3DShapeSymbolInfo* renderInfo = new GlbMarker3DShapeSymbolInfo;
	//renderInfo->type = GLB_OBJECTTYPE_POINT;
	//GlbSphereInfo* shapeinfo = new GlbSphereInfo;
	//shapeinfo->shapeType = GLB_MARKERSHAPE_SPHERE;
	//shapeinfo->radius = new GlbRenderDouble;
	//shapeinfo->radius->bUseField = false;
	//shapeinfo->radius->value = 3000.0;
	//shapeinfo->radius->field=L"";
	//renderInfo->shapeInfo = shapeinfo;

	//GlbPolygon2DSymbolInfo* fillinfo = new GlbPolygon2DSymbolInfo;
	//fillinfo->color = new GlbRenderColor;
	//fillinfo->color->bUseField = false;
	//fillinfo->color->value = RGB(255,0,0);
	//fillinfo->opacity = new GlbRenderInt32;
	//fillinfo->opacity->bUseField = false;
	//fillinfo->opacity->value = 100;
	//fillinfo->outlineInfo=new GlbLinePixelSymbolInfo;
	//fillinfo->outlineInfo->lineColor = new GlbRenderColor;
	//fillinfo->outlineInfo->lineColor->value = RGB(255,255,0);
	//fillinfo->outlineInfo->lineOpacity = new GlbRenderInt32;
	//fillinfo->outlineInfo->lineOpacity->value = 100;
	//fillinfo->outlineInfo->linePattern = new GlbRenderLinePattern;
	//fillinfo->outlineInfo->linePattern->value = GLB_LINE_SOLID;
	//fillinfo->outlineInfo->lineWidth=new GlbRenderDouble;
	//fillinfo->outlineInfo->lineWidth->value = 20.0;
	//fillinfo->texRepeatMode=new GlbRenderTexRepeatMode;
	//fillinfo->textureData=new GlbRenderString;
	//fillinfo->textureData->value=L"";
	//fillinfo->textureRotation=new GlbRenderDouble;
	//fillinfo->textureRotation->value = 0.0;
	//fillinfo->tilingU=new GlbRenderInt32;
	//fillinfo->tilingV=new GlbRenderInt32;
	//renderInfo->fillInfo = fillinfo;

	//renderInfo->yaw = new GlbRenderDouble;
	//renderInfo->yaw->bUseField=false;
	//renderInfo->yaw->value=0;
	//renderInfo->roll = new GlbRenderDouble;
	//renderInfo->roll->bUseField=false;
	//renderInfo->roll->value=0;
	//renderInfo->pitch = new GlbRenderDouble;
	//renderInfo->pitch->bUseField=false;
	//renderInfo->pitch->value=-90;
	//renderer->SetRenderInfo(renderInfo);
	//p_featurelayer->SetGlobe(globe);
	//p_featurelayer->AddRenderer(renderer);
	//globe->AddLayer(p_featurelayer);
}

void NewDlg::DealDemLayer( CString file )
{
	AfxMessageBox(_T("DealDemLayer"));
	//CGlbGlobeDemLayer* p_demlayer = new CGlbGlobeDemLayer;
	//rsdset = rds->GetRasterDataset(L"baogangGlobeDem.tif.pyd");
	//p_demlayer->SetDataset(rsdset);
	//p_demlayer->SetDefaultVisibleDistance(100000000.0);
	//globe->AddLayer(p_demlayer);
}

void NewDlg::DealDomLayer( CString file )
{
	AfxMessageBox(_T("DealDomLayer"));
	//CGlbGlobeDomLayer* p_domlayer = new CGlbGlobeDomLayer;
	//IGlbRasterDataset* rsdset = rds->GetRasterDataset(L"baogangGlobe.jpg.pyd");
	//p_domlayer->SetDataset(rsdset);
	//p_domlayer->SetDefaultVisibleDistance(100000000.0);
	//globe->AddLayer(p_domlayer);
}
