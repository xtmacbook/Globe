#include "StdAfx.h"
#include "GlbGlobeMemCtrl.h"

CGlbGlobeMemCtrl CGlbGlobeMemCtrl::mpr_instance;

CGlbGlobeMemCtrl::CGlbGlobeMemCtrl(void)
{
	mpr_critical.SetName(L"globe_memctrl");
	mpr_hasUsedMem = 0;
	mpr_timeToCompileCost = 0.0;
}


CGlbGlobeMemCtrl::~CGlbGlobeMemCtrl(void)
{
}

glbBool CGlbGlobeMemCtrl::IncrementUsedMem(glbInt64 msize)
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	if(g_isMemCtrl)
	{
		if(mpr_hasUsedMem >= g_memThreshold)
			return false;
	}
	mpr_hasUsedMem += msize;
	if(false)
	{//≤‚ ‘ ‰≥ˆ
		double um = msize;
		um = um/1024.0/1024.0;
		if(um == 0.0)
		{
			int kk = 0;
		}
		double hm = mpr_hasUsedMem;
		hm = hm/1024.0/1024.0;
		//GlbLogWOutput(GLB_LOGTYPE_INFO,L"IncrementUsedMem %lf,Has Used %lf\n",um,hm);
	}
	return true;
}
void CGlbGlobeMemCtrl::DecrementUsedMem(glbInt64 msize)
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	mpr_hasUsedMem -= msize;
	if(false)
	{//≤‚ ‘ ‰≥ˆ
		double um = msize;
		um = um/1024.0/1024.0;		
		double hm = mpr_hasUsedMem;
		hm = hm/1024.0/1024.0;
		//GlbLogWOutput(GLB_LOGTYPE_INFO,L"DecrementUsedMem %lf,Has Used %lf\n",um,hm);
	}
}

void CGlbGlobeMemCtrl::IncrementTimeToCompileCost(glbDouble t)
{
	mpr_timeToCompileCost+=t;

}

glbDouble CGlbGlobeMemCtrl::GetTimeToCompileCost()
{
	return mpr_timeToCompileCost;
}

void CGlbGlobeMemCtrl::ClearTimeToCompileCost()
{
	mpr_timeToCompileCost = 0.0;
}

glbInt64 CGlbGlobeMemCtrl::GetUsedMem()
{
	return mpr_hasUsedMem;
}
CGlbGlobeMemCtrl* CGlbGlobeMemCtrl::GetInstance()
{
	return &mpr_instance;
}

void CGlbGlobeMemCtrl::ClearUsedMem()
{
	mpr_hasUsedMem = 0.0;
}
