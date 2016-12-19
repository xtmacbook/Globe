#include "StdAfx.h"
#include "GlbGlobeREObject.h"
using namespace GlbGlobe;

glbBool CGlbGlobeREObject::SetAltitudeMode( GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw )
{
	mpt_altitudeMode = mode;
	return true;
}

glbBool CGlbGlobeREObject::SetMaxVisibleDistance( glbDouble distance )
{
	mpt_maxVisibleDistance = distance;
	return true;
}

glbBool CGlbGlobeREObject::SetMinVisibleDistance( glbDouble distance )
{
	mpt_minVisibleDistance = distance;
	return true;
}

void CGlbGlobeREObject::SetRenderOrder( glbInt32 order )
{
	mpt_renderOrder = order;
}

void CGlbGlobeREObject::SetLoadOrder( glbInt32 order )
{
	mpt_loadOrder = order;
}

glbBool CGlbGlobeREObject::SetToolTip( glbWChar* tooltip )
{
	mpt_tooltip = tooltip;
	return true;
}

glbBool CGlbGlobeREObject::SetRMessage( glbWChar* msg )
{
	mpt_message = msg;
	return true;
}
