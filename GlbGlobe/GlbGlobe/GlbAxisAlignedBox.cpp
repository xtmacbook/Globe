#include "StdAfx.h"
#include "GlbAxisAlignedBox.h"

using namespace GlbGlobe;

CGlbAxisAlignedBox::CGlbAxisAlignedBox(void)
{
	SetMinimum( -0.5, -0.5, -0.5 );
	SetMaximum( 0.5, 0.5, 0.5 );
	mpr_extent = EXTENT_NULL;
	mpr_p_corners = NULL;
}


CGlbAxisAlignedBox::~CGlbAxisAlignedBox(void)
{
	if (mpr_p_corners)
	{
		delete[] mpr_p_corners;
		mpr_p_corners = NULL;
	}
}
