#include "StdAfx.h"
#include "GlbGlobalConfig.h"

glbref_ptr<CGlbGlobalConfig> CGlbGlobalConfig::mpr_intance = new CGlbGlobalConfig();

CGlbGlobalConfig::CGlbGlobalConfig(void)
{
}


CGlbGlobalConfig::~CGlbGlobalConfig(void)
{
}

CGlbGlobalConfig* const CGlbGlobalConfig::GetInstance()
{
	return CGlbGlobalConfig::mpr_intance.get();
}

glbBool CGlbGlobalConfig::IsMemCtrl()
{
	return g_isMemCtrl;
}
void CGlbGlobalConfig::SetMemCtrl(glbBool isMemCtrl)
{
	g_isMemCtrl = isMemCtrl;
}
glbDouble CGlbGlobalConfig::GetUnusedMem()
{
	return g_memThreshold/1024/1024;
}
void CGlbGlobalConfig::SetUnusedMem(glbDouble unusedMem)
{
	g_memThreshold = unusedMem*1024*1024;//M
}
glbBool CGlbGlobalConfig::IsDMemCtrl()
{
	return g_isDMemCtrl;
}
void CGlbGlobalConfig::SetDMemCtrl(glbBool isDMemCtrl)
{
	g_isDMemCtrl = isDMemCtrl;
}
glbDouble CGlbGlobalConfig::GetTotalDMem()
{
	return g_totalDMem/1024/1024;
}
void CGlbGlobalConfig::SetTotalDMem(glbDouble totalDMem)
{
	g_totalDMem = totalDMem*1024*1024;
}
