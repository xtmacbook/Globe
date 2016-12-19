#include "StdAfx.h"
#include "GlbPolygon.h"
#include "IGlbGeometry.h"

CGlbPolygon::CGlbPolygon(glbByte coordDimension/* = 2*/,glbBool hasM/* = false*/)
{
	mpr_extRing        = NULL;
	mpr_inRings        = NULL;
	mpr_inCount        = 0;
	mpr_extent         = NULL;
	mpr_geometryclass  = NULL;
	mpr_coordDimension = coordDimension;
	mpr_hasM           = hasM;
}

CGlbPolygon::~CGlbPolygon( void )
{
	Empty();
	mpr_geometryclass  = NULL;
}

const CGlbExtent* CGlbPolygon::GetExtent()
{
	if(IsEmpty())
		return NULL;
	if (mpr_extent.get() == NULL)
	{		
		mpr_extent = new CGlbExtent();	
		UpdateExtent();
	}
	return mpr_extent.get();
}

GlbGeometryTypeEnum CGlbPolygon::GetType()
{
	return GLB_GEO_POLYGON;
}

glbBool CGlbPolygon::IsEmpty()
{
	return (mpr_extRing.get() == NULL
		    && mpr_inCount   == 0);
}

glbUInt32 CGlbPolygon::GetSize()
{
	glbUInt32 lsize = sizeof(glbref_ptr<CGlbLine>);
	lsize += sizeof(glbref_ptr<CGlbLine>*);
	lsize += sizeof(mpr_inCount);
	for(glbInt32 i=0;i<mpr_inCount;i++)
	{
		if(mpr_inRings[i].get())
			lsize += mpr_inRings[i].get()->GetSize();
	}
	lsize += sizeof(glbref_ptr<CGlbExtent>);
	if(mpr_extent.get())
		lsize += sizeof(CGlbExtent);
	lsize += sizeof(mpr_coordDimension);
	lsize += sizeof(glbBool);
	return lsize;
}

glbInt32 CGlbPolygon::GetCoordDimension()
{
	return mpr_coordDimension;
}

glbBool CGlbPolygon::HasM()
{
	return mpr_hasM;
}

void CGlbPolygon::Empty()
{
	mpr_extRing = NULL;
	for(glbInt32 i=0;i<mpr_inCount;i++)
		mpr_inRings[i] = NULL;
	if(mpr_inRings) delete[] mpr_inRings;
	mpr_inRings = NULL;
	mpr_inCount = 0;
	mpr_extent  = NULL;
}
glbBool CGlbPolygon::SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls)
{
	if(ctype != GLB_GEOCLASS_POINT)return false;
	mpr_geometryclass = gcls;

	if(mpr_extRing.get())
		mpr_extRing.get()->SetGeoClass(ctype,gcls);
	for(glbInt32 i=0;i<mpr_inCount;i++)
	{
		CGlbLine* ring = mpr_inRings[i].get();
		ring->SetGeoClass(ctype,gcls);
	}
	return true;
}
const CGlbGeometryClass* CGlbPolygon::GetGeoClass(GlbGeometryClassEnum ctype)
{
	if(ctype != GLB_GEOCLASS_POINT)return NULL;
	return mpr_geometryclass.get();
}
glbInt32 CGlbPolygon::GetInRingCount()
{	
	return mpr_inCount;
}

const CGlbLine* CGlbPolygon::GetExtRing()
{	
	return mpr_extRing.get();
}

glbBool CGlbPolygon::SetExtRing( CGlbLine* ring )
{
	if(ring == NULL)
	{
		GlbSetLastError(L"������NULL");
		return false;
	}
	if(ring->GetCoordDimension() != mpr_coordDimension)
	{
		GlbSetLastError(L"����ά����һ��");
		return false;
	}
	if(ring->HasM() != mpr_hasM)
	{
		GlbSetLastError(L"�Ƿ��ж���ֵ��һ��");
		return false;
	}
	if(mpr_extRing.get() != ring)
	{
		mpr_extRing = ring;
		mpr_extRing.get()->SetGeoClass(GLB_GEOCLASS_POINT,mpr_geometryclass.get());
	}
	mpr_extent = NULL;
	return true;
}

glbBool CGlbPolygon::AddInRing( CGlbLine* ring )
{
	return AddInRings(&ring,1);
}

glbBool CGlbPolygon::AddInRings(CGlbLine** rings,glbInt32 count)
{
	if(count <=0 || rings == NULL)
	{
		GlbSetLastError(L"������Ч");
		return false;
	}
	for(glbInt32 i =0;i<count;i++)
	{
		CGlbLine *ring = rings[i];
		if(ring->GetCoordDimension() != mpr_coordDimension)
		{
			GlbSetLastError(L"����ά����һ��");
			return false;
		}
		if(ring->HasM() != mpr_hasM)
		{
			GlbSetLastError(L"�Ƿ��ж���ֵ��һ��");
			return false;
		}
	}
	/*
		���ﲻ����malloc,��Ϊmalloc������ö���Ĺ��캯��
		���ԣ�����new,deleteƥ��.
	*/
	//glbref_ptr<CGlbLine>* newInRings = (glbref_ptr<CGlbLine>*)malloc((mpr_inCount + count)* sizeof(glbref_ptr<CGlbLine>));
	glbref_ptr<CGlbLine>* newInRings = new glbref_ptr<CGlbLine>[mpr_inCount + count];
	if (newInRings == NULL)
	{
		GlbSetLastError(L"�ڴ治��");
		return false;
	}
	for(glbInt32 i =0;i<count;i++)
	{
		CGlbLine *ring = rings[i];
		newInRings[mpr_inCount + i] = ring;
		ring->SetGeoClass(GLB_GEOCLASS_POINT,mpr_geometryclass.get());
	}
	if(mpr_inRings)
	{
		for(glbInt32 i=0;i<mpr_inCount;i++)
			newInRings[i] = mpr_inRings[i].release();		
		delete [] mpr_inRings;
	}
	mpr_inRings = newInRings;
		
	mpr_inCount += count;
	mpr_extent  =  NULL;
	return true;
}

const CGlbLine* CGlbPolygon::GetInRing( glbInt32 idx )
{
	if(mpr_inRings == NULL)
	{
		GlbSetLastError(L"û���ڻ�");
		return NULL;
	}
	if(idx<0||idx>=mpr_inCount)
	{
		GlbSetLastError(L"��������>=0С���ڻ���Ŀ");
		return NULL;
	}
	CGlbLine* line=mpr_inRings[idx].get();
	return line;
}

glbBool CGlbPolygon::DeleteInRing( glbInt32 idx )
{
	return DeleteInRings(idx,1);
}

glbBool CGlbPolygon::DeleteInRing(CGlbLine* ring)
{
	if(ring == NULL)return true;
	glbInt32 idx = -1;
	for(glbInt32 i=0;i<mpr_inCount;i++)
	{
		if(mpr_inRings[i].get() == ring)
		{
			idx = i;break;
		}
	}
	if(idx < 0)
	{
		GlbSetLastError(L"�ڻ�������");
		return false;
	}
	return DeleteInRings(idx,1);
}

glbBool CGlbPolygon::DeleteInRings(glbInt32 idx,glbInt32 count)
{
	if(count <=0)return true;
	if(mpr_inRings == NULL)
	{
		GlbSetLastError(L"û���ڻ�");
		return false;
	}
	if(idx<0||idx>=mpr_inCount)
	{
		GlbSetLastError(L"��������>=0С���ڻ���Ŀ");
		return false;
	}
	if(idx+count>= mpr_inCount)
		count = mpr_inCount - idx;
	if(mpr_inCount == count)
	{
		for(glbInt32 i=0;i<mpr_inCount;i++)
			mpr_inRings[i] = NULL;
		if(mpr_inRings) delete[] mpr_inRings;
		mpr_inRings = NULL;
		mpr_inCount = 0;
		mpr_extent = NULL;
		return true;
	}

	glbref_ptr<CGlbLine>* newInRings = NULL;
	newInRings = new glbref_ptr<CGlbLine>[mpr_inCount - count];
	if(!newInRings)
	{
		GlbSetLastError(L"�ڴ治��");
		return false;
	}
	// �����߶���
	memcpy(newInRings,
		   mpr_inRings,
		   idx*sizeof(glbref_ptr<CGlbLine>));
	memcpy(newInRings + idx,
		   mpr_inRings+(idx+count),
		   (mpr_inCount-idx-count)*sizeof(glbref_ptr<CGlbLine>));

	delete[] mpr_inRings;

	mpr_inRings = newInRings;

	mpr_inCount -= count;
	mpr_extent  =  NULL;
	return true;
}

void CGlbPolygon::UpdateExtent()
{	
	glbDouble x,y,z;
	if(IsEmpty())return;
	if(mpr_extent.get() == NULL)return;

	glbDouble minx=0.0,maxx=0.0,miny=0.0,maxy=0.0,minz=0.0,maxz=0.0;
	glbBool   isInit = false;
	const CGlbExtent* ext = NULL;
	if(mpr_extRing.get())
		ext = mpr_extRing.get()->GetExtent();
    if(ext)
	{
		ext->GetMin(&minx,&miny,&minz);
		ext->GetMax(&maxx,&maxy,&maxz);
		isInit = true;
	}
	for(glbInt32 i=0;i<mpr_inCount;i++)
	{
		const CGlbExtent* iext = mpr_inRings[i].get()->GetExtent();
		if(iext)
		{
			if(!isInit)
			{
				ext->GetMin(&minx,&miny,&minz);
				ext->GetMax(&maxx,&maxy,&maxz);
				isInit = true;
			}else{				
				ext->GetMin(&x,&y,&z);
				minx = min(x,minx);
				miny = min(y,miny);
				minz = min(z,minz);
				ext->GetMax(&x,&y,&z);
				maxx = max(x,maxx);
				maxy = max(y,maxy);
				maxz = max(z,maxz);
			}
		}
	}///for
	if(isInit)
	{
		mpr_extent->SetMin(minx,miny,minz);
		mpr_extent->SetMax(maxx,maxy,maxz);
	}
}
