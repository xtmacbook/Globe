#include "StdAfx.h"
#include "GlbField.h"
#include "GlbError.h"

CGlbField::CGlbField()
{
	mpr_name.clear();
	mpr_alias.clear();	
	mpr_type     = GLB_DATATYPE_UNKNOWN;
	mpr_geotype  = GLB_GEO_UNKNOWN;
	mpr_length   =0;
	mpr_unit.clear();
	mpr_userid.clear();
	mpr_password.clear();
	mpr_isrequired = false;
	mpr_iscannull = true;
	mpr_default.clear();
	mpr_id = -1;
}
CGlbField::~CGlbField()
{}
const glbInt32 CGlbField::GetID()
{
	return mpr_id;
}
glbBool  CGlbField::SetID(glbInt32 id)
{
	mpr_id = id;
	return true;
}
const glbWChar* CGlbField::GetName()
{
	return mpr_name.c_str();
}
glbBool CGlbField::SetName( const glbWChar* name )
{
	if(NULL == name||lstrlen(name)==0)
	{
		GlbSetLastError(L"名称不能空");
		return false;
	}
	mpr_name = name;
	mpr_name.ToUpper();
	return true;
}
const glbWChar* CGlbField::GetAlias()
{
	return mpr_alias.c_str();
}
glbBool CGlbField::SetAlias( const glbWChar* aliasname )
{
	if(NULL == aliasname)
		mpr_alias = L"";
	else
	{
		mpr_alias = aliasname;
		mpr_alias.ToUpper();
	}
	return true;
}
GlbDataTypeEnum CGlbField::GetType()
{
	return mpr_type;
}
glbBool CGlbField::SetType( GlbDataTypeEnum datatype )
{
	mpr_type=datatype;
	return true;
}
glbInt32 CGlbField::GetLength()
{
	return mpr_length;
}
glbBool CGlbField::SetLength( const glbInt32 length )
{
	mpr_length=length;
	return true;
}
GlbGeometryTypeEnum CGlbField::GetGeoType()
{
	return mpr_geotype;
}
glbBool CGlbField::SetGeoType( GlbGeometryTypeEnum geoType )
{
	mpr_geotype=geoType;
	if(mpr_type == GLB_DATATYPE_GEOMETRY
		|| mpr_type == GLB_DATATYPE_BLOB)
		mpr_iscannull = true;
	return true;
}
const glbWChar* CGlbField::GetUnit()
{
	return mpr_unit.c_str();
}
glbBool CGlbField::SetUnit( const glbWChar* unit )
{
	if(NULL == unit)
		mpr_unit = L"";
	else
		mpr_unit=unit;
	return true;
}

const glbWChar* CGlbField::GetUserID()
{
	return mpr_userid.c_str();
}
glbBool CGlbField::SetUserID( const glbWChar* userid )
{
	if(NULL == userid)
		mpr_userid = L"";
	else
		mpr_userid=userid;
	return true;
}
const glbWChar* CGlbField::GetPassWord()
{
	return mpr_password.c_str();
}
glbBool CGlbField::SetPassWord( const glbWChar* pw )
{
	if(NULL == pw)
		mpr_password = L"";
	else
		mpr_password=pw;
	return true;
}
glbBool CGlbField::IsRequired()
{
	return mpr_isrequired;
}
glbBool CGlbField::SetRequired( glbBool bisrequired )
{
	mpr_isrequired=bisrequired;
	return true;
}
glbBool CGlbField::IsCanNull()
{
	return mpr_iscannull;
}
glbBool CGlbField::SetCanNull( glbBool bisnull )
{
	if(mpr_type == GLB_DATATYPE_GEOMETRY
		|| mpr_type == GLB_DATATYPE_BLOB)
	{
		mpr_iscannull = true;
	}else{
		mpr_iscannull=bisnull;
	}	
	return true;
}
const glbWChar* CGlbField::GetDefault()
{
	if(mpr_default.length()==0)return NULL;
	return mpr_default.c_str();
}

glbBool CGlbField::SetDefault(const glbWChar* df )
{
	if(mpr_type == GLB_DATATYPE_VECTOR
		||mpr_type==GLB_DATATYPE_GEOMETRY
		||mpr_type==GLB_DATATYPE_BLOB)
	{
		GlbSetLastError(L"向量,几何,BLOB不支持设置默认值");
		return false;
	}
	if(NULL == df)
		mpr_default = L"";
	else
		mpr_default=df;
	return true;
}

 glbWChar* CGlbField::GetLastError()
{
	return GlbGetLastError();
}
