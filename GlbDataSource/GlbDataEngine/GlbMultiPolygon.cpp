#include "StdAfx.h"
#include "GlbPolygon.h"

CGlbMultiPolygon::CGlbMultiPolygon(glbByte coordDimension/* = 2*/,glbBool hasM/* = false*/)
{
	mpr_count    = 0;
	mpr_polygons = NULL;
	mpr_extent   = NULL;
	mpr_geocls   = NULL;
	mpr_coordDimension = coordDimension;
	mpr_hasM           = hasM;
}
CGlbMultiPolygon::~CGlbMultiPolygon()
{
	Empty();
	mpr_geocls = NULL;
}
GlbGeometryTypeEnum	CGlbMultiPolygon::GetType()
{
	return GLB_GEO_MULTIPOLYGON;
}
glbBool	CGlbMultiPolygon::IsEmpty()
{
	if(mpr_count == 0)return true;
	return false;
}
const CGlbExtent*	CGlbMultiPolygon::GetExtent()
{
	if(IsEmpty())return NULL;
	if(mpr_extent == NULL)
	{
		mpr_extent = new CGlbExtent();
		UpdateExtent();
	}
	return mpr_extent.get();
}
glbUInt32 CGlbMultiPolygon::GetSize()
{
	glbUInt32 lsize = sizeof(glbInt32);
	lsize += sizeof(glbref_ptr<CGlbPolygon>*);
	for(glbInt32 i=0;i<mpr_count;i++)
	{
		lsize += sizeof(glbref_ptr<CGlbPolygon>);
		lsize += mpr_polygons[i]->GetSize();
	}
	lsize += sizeof(glbref_ptr<CGlbExtent>);
	if(mpr_extent.get())
		lsize += sizeof(CGlbExtent);
	lsize += sizeof(glbref_ptr<CGlbGeometryClass>);
	lsize += sizeof(glbByte);
	lsize += sizeof(glbBool);
	return lsize;
}
glbInt32 CGlbMultiPolygon::GetCoordDimension()
{
	return mpr_coordDimension;
}
glbBool CGlbMultiPolygon::HasM()
{
	return mpr_hasM;
}
void  CGlbMultiPolygon::Empty()
{
	for(glbInt32 i=0;i<mpr_count;i++)
		mpr_polygons[i] = NULL;
	if(mpr_polygons)delete[] mpr_polygons;
	mpr_polygons = NULL;
	mpr_count    = 0; 
	mpr_extent   = NULL;
}
glbBool CGlbMultiPolygon::SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls)
{
	if(ctype != GLB_GEOCLASS_POINT)return false;
	for(glbInt32 i=0;i<mpr_count;i++)
	{
		mpr_polygons[i]->SetGeoClass(ctype,gcls);
	}
	mpr_geocls = gcls;
	return true;
}
const CGlbGeometryClass* CGlbMultiPolygon::GetGeoClass(GlbGeometryClassEnum ctype)
{
	if(ctype != GLB_GEOCLASS_POINT)return NULL;
	return mpr_geocls.get();
}
glbInt32 CGlbMultiPolygon::GetCount()
{
	return mpr_count;
}
glbBool	CGlbMultiPolygon::AddPolygon( CGlbPolygon*  polygon)
{
	return AddPolygons(&polygon,1);
}
glbBool CGlbMultiPolygon::AddPolygons(CGlbPolygon** polygons,glbInt32 cnt)
{
	if(polygons == NULL)
	{
		GlbSetLastError(L"参数无效");
		return false;
	}
	for(glbInt32 i=0;i<cnt; i++)
	{
		CGlbPolygon* polygon = polygons[i];
		if(polygon->GetCoordDimension() != mpr_coordDimension)
		{
			GlbSetLastError(L"坐标维度不匹配");
			return false;
		}
		if(polygon->HasM() != mpr_hasM)
		{
			GlbSetLastError(L"度量不匹配");
			return false;
		}
	}

	glbref_ptr<CGlbPolygon>* newPolygons = NULL;
	newPolygons = new glbref_ptr<CGlbPolygon>[mpr_count + cnt];
	if(newPolygons == NULL)
	{
		GlbSetLastError(L"内存不足");
		return false;
	}
	for(glbInt32 i=0;i<cnt; i++)
	{
		CGlbPolygon* polygon = polygons[i];
		polygon->SetGeoClass(GLB_GEOCLASS_POINT,mpr_geocls.get());
		newPolygons[i+mpr_count] = polygon;
	}
	if(mpr_polygons)
	{
		for(glbInt32 i=0;i<mpr_count;i++)
			newPolygons[i] = mpr_polygons[i].release();		
		delete[] mpr_polygons;
	}
	mpr_polygons = newPolygons;

	mpr_count  += cnt;
	mpr_extent = NULL;
	return true;
}
glbBool	CGlbMultiPolygon::DeletePolygon(glbInt32 idx)
{
	return DeletePolygons(idx,1);
}
glbBool	CGlbMultiPolygon::DeletePolygon(CGlbPolygon* polygon)
{
	glbInt32 idx = -1;
	for(glbInt32 i=0;i<mpr_count;i++)
	{
		if(mpr_polygons[i] == polygon)
		{
			idx = i;break;
		}
	}
	if(idx < 0)
	{
		GlbSetLastError(L"不存在");
		return false;
	}
	return DeletePolygons(idx,1);
}
glbBool CGlbMultiPolygon::DeletePolygons(glbInt32 idx,glbInt32 cnt)
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
	glbref_ptr<CGlbPolygon>* newPolygons = NULL;
	newPolygons = new glbref_ptr<CGlbPolygon>[mpr_count - cnt];
	if(newPolygons == NULL)
	{
		GlbSetLastError(L"内存不足");
		return false;
	}
	for(glbInt32 i=0;i<idx;i++)
		newPolygons[i] = mpr_polygons[i];
	for(glbInt32 i=idx + cnt;i<mpr_count;i++)
		newPolygons[i - cnt] = mpr_polygons[i];
	delete[] mpr_polygons;
	mpr_polygons = newPolygons;

	mpr_count  -= cnt;
	mpr_extent =  NULL;
	return true;
}
const CGlbPolygon* CGlbMultiPolygon::GetPolygon(glbInt32 idx)
{
	if(idx <0 || idx >= mpr_count)
	{
		GlbSetLastError(L"参数无效");
		return NULL;
	}
	return mpr_polygons[idx].get();
}

void CGlbMultiPolygon::UpdateExtent()
{
	if(IsEmpty()) 
		return;
	glbDouble minx = 0.0,miny = 0.0,minz = 0.0,maxx = 0.0,maxy = 0.0,maxz = 0.0;
	const CGlbExtent* extentGet = mpr_polygons[0]->GetExtent();
	extentGet->GetMin(&minx,&miny,&minz);
	extentGet->GetMax(&maxx,&maxy,&maxz);
	for(glbInt32 i=0;i<mpr_count;i++)
	{
		glbDouble minX = 0.0,minY = 0.0,minZ = 0.0,maxX = 0.0,maxY = 0.0,maxZ = 0.0;
		extentGet = mpr_polygons[i]->GetExtent();
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