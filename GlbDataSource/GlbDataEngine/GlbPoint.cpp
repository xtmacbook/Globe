#include "StdAfx.h"
#include "GlbPoint.h"

CGlbPoint::CGlbPoint(glbDouble x,glbDouble y,glbBool hasM/*=false*/)
{
	mpr_x = x;
	mpr_y = y;
	mpr_z = 0.0;
	mpr_m = 0.0;
	mpr_coordDimension = 2;
	mpr_hasM = hasM;
}

CGlbPoint::CGlbPoint(glbDouble x,glbDouble y,glbDouble z,glbBool hasM/*=false*/)
{
	mpr_x = x;
	mpr_y = y;
	mpr_z = z;
	mpr_m = 0.0;
	mpr_coordDimension = 3;
	mpr_hasM = hasM;
}

CGlbPoint::~CGlbPoint(void)
{
}

glbBool CGlbPoint::GetX( glbDouble* x )
{
	if(!x)
	{
		GlbSetLastError(L"参数是NULL");
		return false;
	}
	*x=mpr_x;
	return true;
}

glbBool CGlbPoint::GetY( glbDouble* y )
{
	if(!y)
	{
		GlbSetLastError(L"参数是NULL");
		return false;
	}
	*y=mpr_y;
	return true;
}

glbBool CGlbPoint::GetZ( glbDouble* z )
{
	if(mpr_coordDimension == 2)
	{
		GlbSetLastError(L"二维点,不带Z");
		return false;
	}
	if(!z)
	{
		GlbSetLastError(L"参数是NULL");
		return false;
	}
	*z=mpr_z;
	return true;
}

glbBool CGlbPoint::GetM( glbDouble* m )
{
	if(mpr_hasM == false)
	{
		GlbSetLastError(L"不带度量值");
		return false;
	}
	if(!m)
	{
		GlbSetLastError(L"参数是NULL");
		return false;
	}
	*m=mpr_m;
	return true;
}

glbBool CGlbPoint::SetX( glbDouble x )
{
	mpr_x=x;
	return true;
}

glbBool CGlbPoint::SetY( glbDouble y )
{
	mpr_y=y;
	return true;
}

glbBool CGlbPoint::SetZ( glbDouble z )
{
	if(mpr_coordDimension == 2)
	{
		GlbSetLastError(L"二维点,不带Z");
		return false;
	}
	mpr_z=z;
	return true;
}

glbBool CGlbPoint::SetM( glbDouble m )
{
	if(mpr_hasM == false)
	{
		GlbSetLastError(L"不带度量值");
		return false;
	}
	mpr_m=m;
	return true;
}

glbBool CGlbPoint::GetXY( glbDouble* x,glbDouble* y )
{
	if(!x||!y)
	{
		GlbSetLastError(L"参数是NULL");
		return false;
	}
	*x=mpr_x;
	*y=mpr_y;
	return true;
}
glbBool CGlbPoint::GetXYZ( glbDouble* x,glbDouble* y,glbDouble* z )
{
	if(mpr_coordDimension == 2)
	{
		GlbSetLastError(L"二维点,不带Z");
		return false;
	}
	if(!x||!y||!z)
	{
		GlbSetLastError(L"参数是NULL");
		return false;
	}
	*x=mpr_x;
	*y=mpr_y;
	*z=mpr_z;
	return true;
}

glbBool CGlbPoint::SetXY( glbDouble x,glbDouble y )
{
	mpr_x=x;
	mpr_y=y;
	return true;
}

glbBool CGlbPoint::SetXYZ( glbDouble x,glbDouble y,glbDouble z )
{
	if(mpr_coordDimension == 2)
	{
		GlbSetLastError(L"二维点,不带Z");
		return false;
	}
	mpr_x=x;
	mpr_y=y;
	mpr_z=z;
	return true;
}

CGlbExtent* CGlbPoint::GetExtent()
{
	CGlbExtent* pex=new CGlbExtent();

	pex->SetMax(mpr_x,mpr_y,mpr_z);
	pex->SetMin(mpr_x,mpr_y,mpr_z);
	return pex;
}

GlbGeometryTypeEnum CGlbPoint::GetType()
{
	return GLB_GEO_POINT;
}

glbBool CGlbPoint::IsEmpty()
{
	return false;
}

glbUInt32 CGlbPoint::GetSize()
{
	return sizeof(glbDouble)*4
		  +sizeof(glbByte)
		  +sizeof(glbBool);
}

glbInt32 CGlbPoint::GetCoordDimension()
{
	return mpr_coordDimension;
}

glbBool CGlbPoint::HasM()
{
	return mpr_hasM;
}

void CGlbPoint::Empty()
{
}
glbBool CGlbPoint::SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls)
{
	return false;
}
const CGlbGeometryClass* CGlbPoint::GetGeoClass(GlbGeometryClassEnum ctype)
{
	return NULL;
}