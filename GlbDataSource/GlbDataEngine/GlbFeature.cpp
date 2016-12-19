#include "StdAfx.h"
#include "GlbFeature.h"
#include "GlbPoint.h"
#include "GlbLine.h"
#include "GlbPolygon.h"
#include "GlbTin.h"


CGlbFeature::CGlbFeature( IGlbFeatureClass* cls,CGlbFields* fields)
{
	mpr_class    = cls;
	mpr_fields   = fields;
	FIELDVALUE   fv;
	mpr_values.resize(mpr_fields->GetFieldCount(), fv);
	mpr_extent   = NULL;
}

CGlbFeature::~CGlbFeature(void)
{
	mpr_class  = NULL;
	mpr_fields = NULL;
	mpr_extent = NULL;
}

glbBool CGlbFeature::Init(GLBVARIANT** values,CGlbExtent* ext)
{
	if(mpr_fields == NULL)
	{
		GlbSetLastError(L"没有初始化");
		return false;
	}
	if (values == NULL)
	{
		GlbSetLastError(L"参数无效");
		return false;
	}
	mpr_extent = ext;
	glbInt32 cnt = mpr_fields->GetFieldCount();
	for(glbInt32 i=0; i<cnt; i++)
	{
		mpr_values[i].isdirty = false;
		mpr_values[i].value   = values[i];
	}
	return true;
}

glbInt32 CGlbFeature::GetOid()
{	
	if(NULL == mpr_fields)
	{
		GlbSetLastError(L"没有字段");
		return -1;
	}
	glbInt32 index = mpr_fields->FindFieldIndex(L"GLBOID");
	if (index == -1)
	{
		GlbSetLastError(L"没有查询GLBOID字段");
		return -1;
	}
	if (mpr_values[index].value == NULL)
		return -1;
	return mpr_values[index].value->lVal;
}

glbBool CGlbFeature::SetOid( glbInt32 oid )
{
	if (oid < 0)
	{
		GlbSetLastError(L"参数无效");
		return false;
	}
	
	if(NULL == mpr_fields)
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	glbInt32 index = mpr_fields->FindFieldIndex(L"GLBOID");
	if (index == -1)
	{
		GlbSetLastError(L"没有查询GLBOID字段");
		return false;
	}
	mpr_values[index].isdirty       = true;
	if(mpr_values[index].value == NULL)
		mpr_values[index].value = new GLBVARIANT;
	mpr_values[index].value->isnull = false;
	mpr_values[index].value->isarray= false;
	mpr_values[index].value->vt     = GLB_DATATYPE_INT32;
	mpr_values[index].value->lVal   = oid;
	return true;
}

const IGlbFeatureClass* CGlbFeature::GetClass()
{
	return mpr_class.get();	
}

const CGlbFields* CGlbFeature::GetFields()
{	
	return mpr_fields.get();
}

const glbWChar* CGlbFeature::GetSRS()
{
	if(mpr_class)
		return mpr_class->GetSRS();
	return NULL;
}

const CGlbExtent* CGlbFeature::GetExtent()
{
	if(mpr_extent == NULL && mpr_geos.size()>0)
	{
		map<glbUInt32/*级别*/, GEOMETRYVALUE>::iterator itr;
		map<glbUInt32/*级别*/, GEOMETRYVALUE>::iterator itrEnd;
		itr = mpr_geos.begin();
		itrEnd = mpr_geos.end();
		for(itr; itr!= itrEnd;itr++)
		{
			if(itr->second.geo)
			{
				mpr_extent = (CGlbExtent*)itr->second.geo->GetExtent();
				break;
			}
		}
	}
	return mpr_extent.get();
}

const GLBVARIANT*  CGlbFeature::GetValue
	  ( glbInt32 fidx,
	    glbBool *isdirty/*=NULL*/,
		glbBool isclean/*=false*/)
{
	if (mpr_fields == NULL)
	{
		GlbSetLastError(L"没有字段");
		return NULL;
	}
	
	CGlbField* field  = mpr_fields->GetField(fidx);
	if(  field == NULL)return NULL;
	
	if(fidx >= (glbInt32)mpr_values.size())
	{
		GlbSetLastError(L"新增字段");
		return NULL;
	}
	CGlbWString fname = field->GetName();
	if (fname == L"GLBGEO")
	{
		GlbSetLastError(L"系统缺省几何字段值需用GetGeometry方法获取");
		return NULL;
	}
	if(mpr_values[fidx].value)
	{
		if(mpr_values[fidx].value->vt != field->GetType())
		{
			GlbSetLastError(L"字段类型变化");
			return NULL;
		}
	}else{
		mpr_values[fidx].isdirty = false;
		mpr_values[fidx].value   = defautToVariant(field);
	}
	
	if (isdirty != NULL)
		*isdirty =  mpr_values[fidx].isdirty;
	if (isclean)
		mpr_values[fidx].isdirty = false;
	return mpr_values[fidx].value;
}

const GLBVARIANT* CGlbFeature::GetValueByName
	  ( const glbWChar* fname,
	    glbBool       * isdirty,
		glbBool         isclean)
{
	if (fname == NULL)
	{
		GlbSetLastError(L"fname参数无效");
		return NULL;
	}
	if (mpr_fields == NULL)
	{
		GlbSetLastError(L"没有字段");
		return NULL;
	}
	glbInt32 fidx = mpr_fields->FindFieldIndex(fname);
	if(fidx <0) return NULL;
	return GetValue(fidx,isdirty,isclean);
}

glbBool CGlbFeature::PutValue( glbInt32 fidx, GLBVARIANT& value )
{
	if (mpr_fields == NULL)
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	CGlbField* field = mpr_fields->GetField(fidx);
	if (field == NULL)return false;
	
	if(fidx >= (glbInt32)mpr_values.size())
	{
		GlbSetLastError(L"新增字段");
		return false;
	}	
	CGlbWString fname = field->GetName();	
	if (fname == L"GLBGEO")
	{
		GlbSetLastError(L"系统缺省几何类型字段值需用PutGeometry设置");
		return false;
	}
	if(value.isnull)
	{
		if(field->IsCanNull() == false)
		{
			GlbSetLastError(L"字段不容许空值");
			return false;
		}
		if(mpr_values[fidx].value)
		{
			glbVariantClean(*mpr_values[fidx].value);
			delete mpr_values[fidx].value;
			mpr_values[fidx].value = NULL;
		}
		return true;
	}
	if (field->GetType() != value.vt)
	{
		GlbSetLastError(L"值类型与字段类型不一致");
		return false;
	}
	
	mpr_values[fidx].isdirty = true;
	if(mpr_values[fidx].value == NULL)
		mpr_values[fidx].value   = new GLBVARIANT;
	glbVariantClean(*mpr_values[fidx].value);
	glbVariantCopy(*mpr_values[fidx].value, value);
	return true;
}

glbBool CGlbFeature::PutValueByName( const glbWChar* fname, GLBVARIANT& value )
{
	if (fname == NULL)
	{
		GlbSetLastError(L"fname参数无效");
		return false;
	}
	if (mpr_fields == NULL)
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	glbInt32 fidx = mpr_fields->FindFieldIndex(fname);
	if (fidx == -1)return false;
	
	return PutValue(fidx, value);
}

glbWChar* CGlbFeature::GetLastError()
{
	return GlbGetLastError();
}

glbBool CGlbFeature::GetGeometry( IGlbGeometry** geo, glbUInt32 level/*=0*/
	                              ,glbBool *isdirty/*=NULL*/,glbBool isclean/*=false*/
								  ,glbBool needquery/*=true*/)
{
	if (mpr_fields == NULL)
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if(geo == NULL)
	{
		GlbSetLastError(L"参数geo是NULL");
		return false;
	}
	CGlbField* gf = mpr_fields->GetFieldByName(L"GLBGEO");
	if(gf == NULL)
	{
		GlbSetLastError(L"没有检索系统缺省几何字段");
		return false;
	}
	map<glbUInt32/*级别*/, GEOMETRYVALUE>::iterator itr = mpr_geos.find(level);
	if(itr != mpr_geos.end())
	{
		*geo = itr->second.geo.get();
		if(isdirty)
			*isdirty = itr->second.isdirty;
		if(isclean)
			itr->second.isdirty = false;
		return true;
	}
	*geo = NULL;
	glbBool rs = true;
	if(mpr_class && needquery)
	{	
		rs = mpr_class->GetGeometry(GetOid(),geo,level);
		if(rs)
		{
			GEOMETRYVALUE gv;
			gv.geo          = *geo;
			mpr_geos[level] = gv;
		}
	}
	return  rs;
}

glbBool CGlbFeature::PutGeometry( IGlbGeometry* geo, glbUInt32 level,glbBool setdirty/*=true*/)
{
	if (mpr_fields == NULL)
	{
		GlbSetLastError(L"没有初始化");
		return false;
	}
	
	CGlbField* gf = mpr_fields->GetFieldByName(L"GLBGEO");
	if(gf == NULL)
	{
		GlbSetLastError(L"没有检索系统缺省几何字段");
		return false;
	}
	if (geo != NULL && gf->GetGeoType() != geo->GetType())
	{
		GlbSetLastError(L"值几何类型与字段几何类型不一致");
		return false;
	}
	map<glbUInt32, GEOMETRYVALUE>::iterator itr = mpr_geos.find(level);
	
	if(itr == mpr_geos.end())
	{
		GEOMETRYVALUE gv;
		if(setdirty)
			gv.isdirty = true;
		gv.geo          = geo;
		mpr_geos[level] = gv;
	}else{
		itr->second.isdirty = false;
		if(setdirty)
			itr->second.isdirty = true;
		itr->second.geo     = geo;
	}
	if(geo && mpr_class)
	{
		GlbGeometryTypeEnum geoType = geo->GetType();
		CGlbGeometryClass* pcls = (CGlbGeometryClass*)mpr_class->GetGeoClass(GLB_GEOCLASS_POINT);
		if(pcls)geo->SetGeoClass(GLB_GEOCLASS_POINT,pcls);

		CGlbGeometryClass* tcls = (CGlbGeometryClass*)mpr_class->GetGeoClass(GLB_GEOCLASS_TRIANGLE);
		if(tcls)geo->SetGeoClass(GLB_GEOCLASS_TRIANGLE,tcls);
	}
	mpr_extent = NULL;
	return true;
}
