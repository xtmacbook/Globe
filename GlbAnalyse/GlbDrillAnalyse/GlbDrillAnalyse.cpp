// This is the main DLL file.

#include "stdafx.h"

#include "GlbDrillAnalyse.h"
#include "DrillAnalyse.h"
using namespace GlbAnalysis;

//CGlbDrillAnalyse::CGlbDrillAnalyse()
//{
//
//}
//CGlbDrillAnalyse::~CGlbDrillAnalyse()
//{
//
//}

bool CGlbDrillAnalyse::CreateDrill(double pts[], int pt_num, IGlbDataset* tin_ds, IGlbDrillDataset* drill_ds, bool isGlobeMode)
{
	GlbDrillAnalyse da;// = new GlbDrillAnalyse();
	//return da.CreateDrill(pts, pt_num,tin_ds,drill_ds);
	return da.CreateDrill(pts[0], pts[1], pts[2],L"drillnew",tin_ds,drill_ds, isGlobeMode);

	return false;
}

bool CGlbDrillAnalyse::CreateDrill(double x, double y, double z, CGlbWString drill_id, IGlbDataset* tin_ds, IGlbDrillDataset* drill_ds, bool isGlobeMode)
{
	GlbDrillAnalyse da;
	return da.CreateDrill(x, y, z, drill_id, tin_ds, drill_ds, isGlobeMode);
}
