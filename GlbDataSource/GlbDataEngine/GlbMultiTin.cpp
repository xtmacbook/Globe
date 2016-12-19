#include "StdAfx.h"
#include "GlbTin.h"
CGlbMultiTin::CGlbMultiTin(glbBool hasM/* = false*/)
{
	mpr_count          = 0;
	mpr_tins           = NULL;
	mpr_extent         = NULL;
	mpr_vertexcls      = NULL;
	mpr_tricls         = NULL;
	mpr_coordDimension = 3;
	mpr_hasM           = hasM;
}
CGlbMultiTin::~CGlbMultiTin()
{
	Empty();
}
GlbGeometryTypeEnum CGlbMultiTin::GetType()
{
	return GLB_GEO_MULTITIN;
}
glbBool CGlbMultiTin::IsEmpty()
{
	return mpr_count==0?true:false;
}
const CGlbExtent* CGlbMultiTin::GetExtent()
{
	if(IsEmpty())return NULL;
	if(mpr_extent == NULL)
	{
		mpr_extent = new CGlbExtent();
		UpdateExtent();
	}
	return mpr_extent.get();
}
glbUInt32 CGlbMultiTin::GetSize()
{
	glbUInt32 lsize = sizeof(glbInt32);
	lsize += sizeof(glbref_ptr<CGlbTin>*);
	for(glbInt32 i=0;i<mpr_count;i++)
	{
		lsize += sizeof(glbref_ptr<CGlbTin>);
		lsize += mpr_tins[i]->GetSize();
	}
	lsize += sizeof(glbref_ptr<CGlbExtent>);
	if(mpr_extent.get())
		lsize += sizeof(CGlbExtent);
	lsize += sizeof(glbref_ptr<CGlbGeometryClass>);
	lsize += sizeof(glbref_ptr<CGlbGeometryClass>);
	lsize += sizeof(glbByte);
	lsize += sizeof(glbBool);
	return lsize;
}
glbInt32 CGlbMultiTin::GetCoordDimension()
{
	return 3;
}
glbBool CGlbMultiTin::HasM()
{
	return mpr_hasM;
}
void CGlbMultiTin::Empty()
{
	for(glbInt32 i=0;i<mpr_count;i++)
		mpr_tins[i] = NULL;
	if(mpr_tins)delete[] mpr_tins;
	mpr_tins     = NULL;
	mpr_count    = 0; 
	mpr_extent   = NULL;
}
glbBool CGlbMultiTin::SetGeoClass
		(GlbGeometryClassEnum ctype,
		 CGlbGeometryClass*   gcls)
{
	for(glbInt32 i=0;i<mpr_count;i++)
	{
		mpr_tins[i]->SetGeoClass(ctype,gcls);
	}
	if(ctype == GLB_GEOCLASS_POINT)
		mpr_vertexcls = gcls;
	else if(ctype == GLB_GEOCLASS_TRIANGLE)
		mpr_tricls = gcls;
	return true;
}
const CGlbGeometryClass* CGlbMultiTin::GetGeoClass
	(GlbGeometryClassEnum ctype)
{
	if(ctype == GLB_GEOCLASS_POINT)
		return mpr_vertexcls.get();
	else if(ctype == GLB_GEOCLASS_TRIANGLE)
		return mpr_tricls.get();
	return NULL;
}
glbInt32 CGlbMultiTin::GetCount()
{
	return mpr_count;
}
glbBool	CGlbMultiTin::AddTin( CGlbTin* tin)
{
	return AddTins(&tin,1);
}
glbBool CGlbMultiTin::AddTins(CGlbTin** tins,glbInt32 cnt)
{
	if(tins == NULL)
	{
		GlbSetLastError(L"参数无效");
		return false;
	}
	for(glbInt32 i=0;i<cnt; i++)
	{
		CGlbTin* tin = tins[i];
		if(tin->GetCoordDimension() != mpr_coordDimension)
		{
			GlbSetLastError(L"坐标维度不匹配");
			return false;
		}
		if(tin->HasM() != mpr_hasM)
		{
			GlbSetLastError(L"度量不匹配");
			return false;
		}
	}

	glbref_ptr<CGlbTin>* newTins = NULL;
	newTins = new glbref_ptr<CGlbTin>[mpr_count + cnt];
	if(newTins == NULL)
	{
		GlbSetLastError(L"内存不足");
		return false;
	}
	for(glbInt32 i=0;i<cnt; i++)
	{
		CGlbTin* tin = tins[i];
		tin->SetGeoClass(GLB_GEOCLASS_POINT,mpr_vertexcls.get());
		tin->SetGeoClass(GLB_GEOCLASS_TRIANGLE,mpr_tricls.get());
		newTins[i+mpr_count] = tin;
	}
	if(mpr_tins)
	{
		for(glbInt32 i=0;i<mpr_count;i++)
			newTins[i] = mpr_tins[i].release();		
		delete[] mpr_tins;
	}
	mpr_tins = newTins;

	mpr_count  += cnt;
	mpr_extent = NULL;
	return true;
}
glbBool	CGlbMultiTin::DeleteTin(glbInt32 idx)
{
	return DeleteTins(idx,1);
}
glbBool CGlbMultiTin::DeleteTins(glbInt32 idx,glbInt32 cnt)
{
	if(cnt <=0) return true;
	if(idx <0 || idx >= mpr_count)
	{
		GlbSetLastError(L"参数无效");
		return NULL;
	}
	if((idx+cnt) >= mpr_count)
		cnt = mpr_count - idx;
	if(cnt == mpr_count)
	{
		Empty();
		return true;
	}
	glbref_ptr<CGlbTin>* newTins = NULL;
	newTins = new glbref_ptr<CGlbTin>[mpr_count - cnt];
	if(newTins == NULL)
	{
		GlbSetLastError(L"内存不足");
		return false;
	}
	for(glbInt32 i=0;i<idx;i++)
		newTins[i] = mpr_tins[i];
	for(glbInt32 i=idx + cnt;i<mpr_count;i++)
		newTins[i - cnt] = mpr_tins[i];
	delete[] mpr_tins;
	mpr_tins = newTins;

	mpr_count  -= cnt;
	mpr_extent =  NULL;
	return true;
}
const CGlbTin* CGlbMultiTin::GetTin(glbInt32 idx)
{
	if(idx <0 || idx >= mpr_count)
	{
		GlbSetLastError(L"参数无效");
		return NULL;
	}
	return mpr_tins[idx].get();
}
void CGlbMultiTin::UpdateExtent()
{
	if(IsEmpty()) 
		return;
	glbDouble minx = 0.0,miny = 0.0,minz = 0.0,maxx = 0.0,maxy = 0.0,maxz = 0.0;
	const CGlbExtent* extentGet = mpr_tins[0]->GetExtent();
	extentGet->GetMin(&minx,&miny,&minz);
	extentGet->GetMax(&maxx,&maxy,&maxz);
	for(glbInt32 i=0;i<mpr_count;i++)
	{
		glbDouble minX = 0.0,minY = 0.0,minZ = 0.0,maxX = 0.0,maxY = 0.0,maxZ = 0.0;
		extentGet = mpr_tins[i]->GetExtent();
		extentGet->GetMin(&minX,&minY,&minZ);
		extentGet->GetMax(&maxX,&maxY,&maxZ);

		if(minX<minx) minx=minX;
		if(maxX>maxx) maxx=maxX;
		if(minY<miny) miny=minY;
		if(maxY>maxy) maxy=maxY;
		if(minZ<minz) minz=minZ;
		if(maxZ>maxz) maxz=maxZ;
	}
	mpr_extent->SetMin(minx,miny,minz);
	mpr_extent->SetMax(maxx,maxy,maxz);
}