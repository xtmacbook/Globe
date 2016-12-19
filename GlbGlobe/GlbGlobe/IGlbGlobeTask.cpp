#include "StdAfx.h"
#include "IGlbGlobeTask.h"

using namespace GlbGlobe;

IGlbGlobeTask::IGlbGlobeTask(void)
{
	mpr_isDoing = false;
	mpr_isFinished = false;
}


IGlbGlobeTask::~IGlbGlobeTask(void)
{
}

glbBool IGlbGlobeTask::isDoing()
{
	return mpr_isDoing;
}

glbBool IGlbGlobeTask::isFinished()
{
	return mpr_isFinished;
}