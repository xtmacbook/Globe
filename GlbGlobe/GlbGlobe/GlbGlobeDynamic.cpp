#include "StdAfx.h"
#include "GlbGlobeDynamic.h"

using namespace GlbGlobe;

CGlbGlobeDynamic::CGlbGlobeDynamic(void)
{
	mpr_dymicmode = GLB_DYNAMICMODE_VIRTUAL;
	mpr_playmode = GLB_DYNAMICPLAYMODE_CAR;
	mpr_tractmode = GLB_DYNAMICTRACMODE_NONE;
	mpr_ditan = 1000.0;
	mpr_pitch = -30.0;
	mpr_isrepeat = false;
} 

CGlbGlobeDynamic::~CGlbGlobeDynamic(void)
{
}

void CGlbGlobeDynamic::SetDynamicObjMode(GlbGlobeDynamicObjModeEnum mode)
{
	mpr_dymicmode = mode;
}

GlbGlobeDynamicObjModeEnum CGlbGlobeDynamic::GetDynamicObjMode()
{
	return mpr_dymicmode;
}

void CGlbGlobeDynamic::SetTraceMode(GlbGlobeDynamicTractModeEnum mode)
{
	mpr_tractmode = mode;
}

GlbGlobeDynamicTractModeEnum CGlbGlobeDynamic::GetTraceMode()
{
	return mpr_tractmode;
}

void CGlbGlobeDynamic::SetPlayMode(GlbGlobeDynamicPlayModeEnum mode)
{
	mpr_playmode = mode;
}

GlbGlobeDynamicPlayModeEnum CGlbGlobeDynamic::GetPlayMode()
{
	return mpr_playmode;
}

void CGlbGlobeDynamic::SetThirdModeDistance(glbDouble distan)
{
	mpr_ditan = distan;
}

glbDouble CGlbGlobeDynamic::GetThirdModeDistance()
{
	return mpr_ditan;
}

void CGlbGlobeDynamic::SetThirdModePitch(glbDouble pitch)
{
	if (pitch > 0)
		pitch = -pitch;
	mpr_pitch = pitch;
}

glbDouble CGlbGlobeDynamic::GetThirdModePitch()
{
	return mpr_pitch;
}

void CGlbGlobeDynamic::SetRepeat(glbBool isrepeat)
{
	mpr_isrepeat = isrepeat;
}

glbBool CGlbGlobeDynamic::GetRepeat()
{
	return mpr_isrepeat;
}
