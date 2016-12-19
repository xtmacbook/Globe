#include "StdAfx.h"
#include "GlbObject.h"

CGlbObject::CGlbObject(IGlbObjectClass* cls,CGlbFields* fields)
{
	mpr_class  = cls;
	mpr_fields = fields;
	FIELDVALUE fv;
	mpr_values.resize(mpr_fields->GetFieldCount(),fv);
}

CGlbObject::~CGlbObject(void)
{
	mpr_class = NULL;
	mpr_fields= NULL;
}

glbBool CGlbObject::Init( GLBVARIANT** values)
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
	glbInt32 cnt = mpr_fields->GetFieldCount();
	for(glbInt32 i=0; i<cnt; i++)
	{
		mpr_values[i].isdirty = false;
		mpr_values[i].value   = values[i];
	}
	return true;
}

glbInt32 CGlbObject::GetOid()
{
	if (mpr_fields == NULL)
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

glbBool CGlbObject::SetOid(glbInt32 oid)
{
	if (oid < 0)
	{
		GlbSetLastError(L"参数无效");
		return false;
	}
	if (mpr_fields == NULL)
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
		mpr_values[index].value = new GLBVARIANT();
	mpr_values[index].value->isnull = false;
	mpr_values[index].value->isarray= false;
	mpr_values[index].value->vt     = GLB_DATATYPE_INT32;
	mpr_values[index].value->lVal   = oid;
	return true;
}

const IGlbObjectClass* CGlbObject::GetClass()
{
	return mpr_class.get();
}

const CGlbFields* CGlbObject::GetFields()
{
	return mpr_fields.get();
}

const GLBVARIANT* CGlbObject::GetValue
	    ( glbInt32    fidx,
		  glbBool     *isdirty/*=NULL*/,
		  glbBool     isclean/*=false*/)
{
	if (mpr_fields == NULL)
	{
		GlbSetLastError(L"没有字段");
		return NULL;
	}
	if(fidx<0 || fidx >= mpr_fields->GetFieldCount())
	{
		GlbSetLastError(L"fidx必须>=0且小于字段数目");
		return NULL;
	}
	if(fidx >= (glbInt32)mpr_values.size())
	{
		GlbSetLastError(L"新增字段");
		return NULL;
	}
	CGlbField* fd = (CGlbField*)mpr_fields->GetField(fidx);
	if(mpr_values[fidx].value)
	{		
		if(fd->GetType() != mpr_values[fidx].value->vt)
		{
			GlbSetLastError(L"字段类型修改后与值类型不一致");
			return NULL;
		}
	}else{
		mpr_values[fidx].isdirty = false;
		mpr_values[fidx].value   = defautToVariant(fd);
	}
	if (isdirty != NULL)
		*isdirty =  mpr_values[fidx].isdirty;
	if (isclean)
		mpr_values[fidx].isdirty = false;
	return mpr_values[fidx].value;
}

const GLBVARIANT* CGlbObject::GetValueByName
	            ( const glbWChar* fname,
				  glbBool     *isdirty/*=NULL*/,
		          glbBool     isclean/*=false*/ )
{
	if (mpr_fields == NULL)
	{
		GlbSetLastError(L"没有字段");
		return NULL;
	}
	if (fname == NULL)
	{
		GlbSetLastError(L"fname参数无效");
		return NULL;
	}
	glbInt32 fidx = mpr_fields->FindFieldIndex(fname);
	if(fidx <0)return NULL;
	
	return GetValue(fidx,isdirty,isclean);
}

glbBool CGlbObject::PutValue( glbInt32 fidx, GLBVARIANT& value )
{
	if (mpr_fields == NULL)
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	CGlbField* field = mpr_fields->GetField(fidx);
	if (field == NULL)return false;
	if (field->GetType() != value.vt)
	{
		GlbSetLastError(L"值类型与字段类型不一致");
		return false;
	}
	if(fidx > (glbInt32)mpr_values.size())
	{
		GlbSetLastError(L"新增字段");
		return false;
	}
	mpr_values[fidx].isdirty = true;
	if(mpr_values[fidx].value == NULL)
		mpr_values[fidx].value   = new GLBVARIANT;
	
	glbVariantClean(*mpr_values[fidx].value);
	glbVariantCopy(*mpr_values[fidx].value, value);
	return true;
}

glbBool CGlbObject::PutValueByName( const glbWChar* fname,GLBVARIANT& value )
{
	if (mpr_fields == NULL)
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if (fname == NULL)
	{
		GlbSetLastError(L"fname参数无效");
		return false;
	}
	glbInt32 fidx = mpr_fields->FindFieldIndex(fname);
	if (fidx == -1)return false;
	
	return PutValue(fidx, value);
}

glbWChar* CGlbObject::GetLastError()
{
	return GlbGetLastError();
}

glbUInt32 CGlbObject::GetSize()
{
	glbUInt32 lsize = sizeof(glbref_ptr<IGlbObjectClass>);
	lsize += sizeof(glbref_ptr<IGlbObjectCursor>);
	lsize += sizeof(glbref_ptr<CGlbFields>);
	lsize += sizeof(vector<FIELDVALUE>);
	for(size_t i=0;i<mpr_values.size();i++)
	{
		lsize += sizeof(FIELDVALUE);
		if(mpr_values[i].value != NULL)
		{
			lsize += glbVariantSize(*mpr_values[i].value);
		}
	}
	return lsize;
}
