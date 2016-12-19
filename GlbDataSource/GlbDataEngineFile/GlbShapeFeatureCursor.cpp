#pragma warning(disable:4251)
#include "StdAfx.h"
#include "GlbShapeFeatureCursor.h"
#include "GlbString.h"
#include "ogr_gensql.h"
CGlbShapeFeatureCursor::CGlbShapeFeatureCursor(void)
{
	mpr_featureclass = NULL;
	mpr_fields       = NULL;
	mpr_extent       = NULL;
	mpr_ogrlayer     = NULL;
	mpr_ogrfeature   = NULL;
	mpr_startindex   = mpr_endindex = -1;
	mpr_index        = -1;
	mpr_count        = 0;
}

CGlbShapeFeatureCursor::~CGlbShapeFeatureCursor(void)
{
	mpr_featureclass = NULL;
	mpr_fields       = NULL;
	mpr_extent       = NULL;
	if(mpr_ogrfeature)delete mpr_ogrfeature;
	if(mpr_ogrlayer)  delete mpr_ogrlayer;
}

glbBool CGlbShapeFeatureCursor::Initialize( CGlbShapeFeatureClass* featureCls,OGRLayer* ogrLayer,glbInt32 startrow,glbInt32 endrow)
{
	if (featureCls == NULL || ogrLayer == NULL)
	{
		GlbSetLastError(L"参数有NULL");
		return false;
	}
	
	mpr_startindex   = startrow;
	mpr_endindex     = endrow;
	if(mpr_endindex == INT_MAX)mpr_endindex--;
	mpr_ogrlayer     = ogrLayer;
	mpr_featureclass = featureCls;
	mpr_index        = startrow;
	mpr_count        = mpr_ogrlayer->GetFeatureCount();
	if(mpr_count > mpr_endindex - mpr_startindex +1)
		mpr_count = mpr_endindex - mpr_startindex +1;

	mpr_ogrlayer->SetNextByIndex(mpr_index);		
	mpr_ogrfeature       = mpr_ogrlayer->GetNextFeature();
	CGlbWString critName = featureCls->GetName();
	critName.append(L"_cursor");
	mpr_critical.SetName(critName.c_str());
	return true;
}

glbInt32 CGlbShapeFeatureCursor::GetCount()
{
	return mpr_count;
}

CGlbFields* CGlbShapeFeatureCursor::GetFields()
{
	if(mpr_ogrlayer == NULL)
		return NULL;
	if (mpr_fields.get() == NULL)
	{		
		/*
			代码线程不安全!!!，所以加锁!!!.
		*/
		GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		mpr_fields = GetFieldsFromOrgLayer(mpr_ogrlayer);
	}
	return mpr_fields.get();
}

const IGlbFeatureClass* CGlbShapeFeatureCursor::GetClass()
{
	return mpr_featureclass.get();
}

//const CGlbExtent* CGlbShapeFeatureCursor::GetExtent()
//{
//	if(mpr_extent.get() == NULL && mpr_ogrlayer)
//	{
//		OGREnvelope* ps = new OGREnvelope();
//		OGRErr      err = mpr_ogrlayer->GetExtent(ps);		
//		if (!err)
//		{
//			mpr_extent = new CGlbExtent(ps->MinX,ps->MaxX,ps->MinY,ps->MaxY);
//			delete ps;
//		}
//	}
//	return mpr_extent.get();
//}

glbBool CGlbShapeFeatureCursor::Requery()
{
	if (mpr_ogrlayer == NULL)
	{
		GlbSetLastError(L"没有初始化");
		return false;
	}
	//mpr_ogrlayer的代码不是线程安全的!!!!.
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	mpr_ogrlayer->ResetReading();
	mpr_count = mpr_ogrlayer->GetFeatureCount();	
	mpr_index = mpr_startindex;
	mpr_ogrlayer->SetNextByIndex(mpr_index);
	
	if(mpr_count > mpr_endindex - mpr_startindex +1)
		mpr_count = mpr_endindex - mpr_startindex +1;
	
	if(mpr_ogrfeature)delete mpr_ogrfeature;
	mpr_ogrfeature = mpr_ogrlayer->GetNextFeature();
	return true;
}

glbBool CGlbShapeFeatureCursor::MoveNext()
{
	if (mpr_ogrlayer == NULL)
	{
		GlbSetLastError(L"没有初始化");
		return false;
	}
	if(mpr_index  > mpr_endindex
		|| (mpr_index - mpr_startindex)>mpr_count)
	{	
		if(mpr_ogrfeature)
		{
			delete mpr_ogrfeature;
			mpr_ogrfeature = NULL;
		}
		GlbSetLastError(L"游标已经到末尾");
		return false;
	}
	mpr_index++;
	
	//mpr_ogrlayer的代码线程不安全!!!!.
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	OGRFeature* fea = mpr_ogrlayer->GetNextFeature();
	if(mpr_ogrfeature)
	{
		delete mpr_ogrfeature;
		mpr_ogrfeature = NULL;
	}
	mpr_ogrfeature = fea;	
	if(mpr_ogrfeature == NULL)
	{
		GlbSetLastError(L"游标已经到末尾");
		return false;
	}
	return true;
}

CGlbFeature* CGlbShapeFeatureCursor::GetFeature()
{
	if (mpr_ogrlayer == NULL)
	{
		GlbSetLastError(L"没有初始化");
		return NULL;
	}
	if (mpr_ogrfeature == NULL)
		return NULL;
	GetFields();
	GLBVARIANT** values = ReadFromOrgFeature(mpr_ogrlayer,mpr_ogrfeature,mpr_fields.get());
	if(values)
	{
		CGlbFeature* feature = new CGlbFeature((IGlbFeatureClass*)this->GetClass(),(CGlbFields*)this->GetFields());
		if(feature->Init(values,NULL))
		{
			OGRGeometry* ogrGeo = mpr_ogrfeature->GetGeometryRef();
			IGlbGeometry* geo   = GetGeometryFromOGR(ogrGeo);
			feature->PutGeometry(geo,0,false);
			delete[] values;
			return feature;
		}
		else
		{
			for(glbInt32 i=0; i<mpr_fields->GetFieldCount(); i++)
			{
				glbVariantClean(*values[i]);
			}
			delete[] values;
			delete feature;
		}
	}
	return NULL;
}

