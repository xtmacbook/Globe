#include "StdAfx.h"
#include "GlbLine.h"

CGlbMultiLine::CGlbMultiLine(glbByte coordDimension/* = 2*/,glbBool hasM/* = false*/)
{
	mpr_count          = 0;
	mpr_curves         = NULL;
	mpr_extent         = NULL;
	mpr_geocls         = NULL;
	mpr_coordDimension = coordDimension;
	mpr_hasM           = hasM;
}
CGlbMultiLine::~CGlbMultiLine()
{
	Empty();
	mpr_geocls = NULL;
}
GlbGeometryTypeEnum	CGlbMultiLine::GetType()
{
	return GLB_GEO_MULTILINE;
}
glbBool	CGlbMultiLine::IsEmpty()
{
	if(mpr_count == 0)return true;
	return false;
}

const CGlbExtent* CGlbMultiLine::GetExtent()
{
	if(IsEmpty())return NULL;
	if(mpr_extent == NULL)
	{
		mpr_extent = new CGlbExtent();
		UpdateExtent();
	}
	return mpr_extent.get();
}
glbUInt32 CGlbMultiLine::GetSize()
{
	glbUInt32 lsize = sizeof(glbInt32);
	lsize += sizeof(glbref_ptr<CGlbLine>*);
	for(glbInt32 i=0; i< mpr_count; i++)
	{
		lsize += sizeof(glbref_ptr<CGlbLine>);
		lsize += mpr_curves[i]->GetSize();
	}
	lsize += sizeof(glbref_ptr<CGlbExtent>);
	if(mpr_extent.get())
		lsize += sizeof(CGlbExtent);
	lsize += sizeof(glbref_ptr<CGlbGeometryClass>);
	lsize += sizeof(glbByte);
	lsize += sizeof(glbBool);
	return lsize;
}
glbInt32 CGlbMultiLine::GetCoordDimension()
{
	return mpr_coordDimension;
}
glbBool CGlbMultiLine::HasM()
{
	return mpr_hasM;
}
void CGlbMultiLine::Empty()
{
	for(glbInt32 i=0;i<mpr_count;i++)
		mpr_curves[i] = NULL;
	if(mpr_curves) delete[] mpr_curves;
	mpr_curves = NULL;
	mpr_extent = NULL;
}
glbBool CGlbMultiLine::SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls)
{
	if(ctype != GLB_GEOCLASS_POINT)return false;
	for(glbInt32 i=0;i<mpr_count;i++)
	{
		mpr_curves[i]->SetGeoClass(ctype,gcls);
	}
	mpr_geocls = gcls;
	return true;
}
const CGlbGeometryClass* CGlbMultiLine::GetGeoClass(GlbGeometryClassEnum ctype)
{
	if(ctype != GLB_GEOCLASS_POINT)return NULL;
	return mpr_geocls.get();
}
glbInt32 CGlbMultiLine::GetCount()
{
	return mpr_count;
}
const CGlbLine* CGlbMultiLine::GetLine(glbInt32 idx)
{
	if(idx <0 || idx >= mpr_count)
	{
		GlbSetLastError(L"参数无效");
		return NULL;
	}
	return mpr_curves[idx].get();
}
glbBool	CGlbMultiLine::AddLine(CGlbLine* curve)
{
	return AddLines(&curve,1);
}
glbBool CGlbMultiLine::AddLines(CGlbLine** curves,glbInt32 cnt)
{
	if(curves == NULL)return true;
	for(glbInt32 i=0;i<cnt; i++)
	{
		CGlbLine* line = curves[i];
		if(line->GetCoordDimension() != mpr_coordDimension)
		{
			GlbSetLastError(L"坐标维度不匹配");
			return false;
		}
		if(line->HasM() != mpr_hasM)
		{
			GlbSetLastError(L"度量不匹配");
			return false;
		}
	}

	glbref_ptr<CGlbLine>* newCurves = NULL;
	newCurves = new glbref_ptr<CGlbLine>[mpr_count + cnt];
	if(newCurves == NULL)
	{
		GlbSetLastError(L"内存不足");
		return false;
	}
	for(glbInt32 i=0;i<cnt; i++)
	{
		CGlbLine* line = curves[i];
		line->SetGeoClass(GLB_GEOCLASS_POINT,mpr_geocls.get());
		newCurves[i+mpr_count] = line;
	}
	if(mpr_curves)
	{
		for(glbInt32 i=0;i<mpr_count;i++)
			newCurves[i] = mpr_curves[i].release();		
		delete[] mpr_curves;
	}
	mpr_curves = newCurves;

	mpr_count  += cnt;
	mpr_extent = NULL;
	return true;
}
glbBool	CGlbMultiLine::DeleteLine(glbInt32 idx)
{
	return DeleteLines(idx,1);
}
glbBool CGlbMultiLine::DeleteLine(CGlbLine* line)
{
	if(line == NULL)return true;
	glbInt32 idx = -1;
	for(glbInt32 i=0;i<mpr_count;i++)
	{
		if(mpr_curves[i].get() == line)
		{
			idx = i;break;
		}
	}
	if(idx < 0)
	{
		GlbSetLastError(L"内环不存在");
		return false;
	}
	return DeleteLines(idx,1);
}
glbBool CGlbMultiLine::DeleteLines(glbInt32 idx,glbInt32 cnt)
{
	if(cnt <=0 ) return true;
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
	glbref_ptr<CGlbLine>* newCurves = NULL;
	newCurves = new glbref_ptr<CGlbLine>[mpr_count - cnt];
	if(newCurves == NULL)
	{
		GlbSetLastError(L"内存不足");
		return false;
	}
	for(glbInt32 i=0;i<idx;i++)
		newCurves[i] = mpr_curves[i];
	for(glbInt32 i=idx + cnt;i<mpr_count;i++)
		newCurves[i - cnt] = mpr_curves[i];
	delete[] mpr_curves;
	mpr_curves = newCurves;

	mpr_count -= cnt;
	mpr_extent = NULL;
	return true;
}
void CGlbMultiLine::UpdateExtent()
{
	if(IsEmpty()) 
		return;
	glbDouble minx = 0.0,miny = 0.0,minz = 0.0,maxx = 0.0,maxy = 0.0,maxz = 0.0;
	const CGlbExtent* extentGet = mpr_curves[0]->GetExtent();
	extentGet->GetMin(&minx,&miny,&minz);
	extentGet->GetMax(&maxx,&maxy,&maxz);
	for(glbInt32 i=0;i<mpr_count;i++)
	{
		glbDouble minX = 0.0,minY = 0.0,minZ = 0.0,maxX = 0.0,maxY = 0.0,maxZ = 0.0;
		extentGet = mpr_curves[i]->GetExtent();
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