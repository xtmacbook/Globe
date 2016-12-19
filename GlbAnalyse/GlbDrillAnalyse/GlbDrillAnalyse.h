// GlbDrillAnalyse.h

#pragma once

#include "IGlbDataEngine.h"
#include "GlbReference.h"
#include "GlbDrillAnalyseExport.h"

namespace GlbAnalysis
{
	class GLBDRILLANALYSE_API CGlbDrillAnalyse : public CGlbReference	
	{
		// TODO: Add your methods for this class here.
	public:
		//CGlbDrillAnalyse();
		//~CGlbDrillAnalyse();

		static bool CreateDrill(double pts[], int pt_num, IGlbDataset* tin_ds, IGlbDrillDataset* drill_ds, bool isGlobeMode = true);		
		static bool CreateDrill(double x, double y, double z, CGlbWString drill_id, IGlbDataset* tin_ds, IGlbDrillDataset* drill_ds, bool isGlobeMode = true);		
	};
}
