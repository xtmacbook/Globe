// This is the main DLL file.

#include "stdafx.h"

#include "GlbSectionAnalyse.h"
#include "SectionAnalyse.h"
using namespace GlbAnalysis;

bool CGlbSectionAnalyse::CreateSection(double pts[] , int pt_num, CGlbWString sec_id, IGlbDataset* tin_ds, IGlbSectionDataset* sec_ds, bool isGlobeMode)
{
	SectionAnalyse sa;
	return sa.CreateSection(pts,pt_num, sec_id, tin_ds, sec_ds, isGlobeMode);
}