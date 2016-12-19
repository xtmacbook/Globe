#include "GlbFileRasterBand.h"

CGlbFileRasterBand::CGlbFileRasterBand(GDALRasterBandH band)
{
	mpr_band = band;
}


CGlbFileRasterBand::~CGlbFileRasterBand(void)
{	
}

glbInt32 CGlbFileRasterBand::GetID()
{
	if (mpr_band != NULL)
	{
		return GDALGetBandNumber(mpr_band);
	}
	return -1;
}

const glbWChar* CGlbFileRasterBand::GetName()
{
	if (mpr_band != NULL)
	{
		//GDALGetDescription()
	}
	return NULL;
}
