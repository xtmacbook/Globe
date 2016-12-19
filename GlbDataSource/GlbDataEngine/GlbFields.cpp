#include "StdAfx.h"
#include "GlbFields.h"
#include "GlbError.h"

CGlbFields::CGlbFields()
{
}

CGlbFields::~CGlbFields()
{
}

glbInt32 CGlbFields::GetFieldCount()
{
	return mpr_fields.size();
}

CGlbField* CGlbFields::GetField(glbInt32 fidx)
{
	if(fidx < 0)
	{
		GlbSetLastError(L"参数小于0");
		return NULL;
	}
	if(fidx >= (glbInt32)mpr_fields.size())
	{
		GlbSetLastError(L"参数大于字段数目");
		return NULL;
	}
	return mpr_fields[fidx].get();
}

CGlbField* CGlbFields::GetFieldByName(const glbWChar* iname)
{
	if(iname == NULL)
	{
		GlbSetLastError(L"参数是空值");
		return NULL;
	}
	CGlbWString str(iname);
	str.ToUpper();
	for(glbInt32 i = 0; i < (glbInt32)mpr_fields.size(); i++)
	{
		if(wcscmp(str.c_str(),mpr_fields[i]->GetName())==0)
		{
			return mpr_fields[i].get();
		}
	}
	GlbSetLastError(L"字段不存在");
	return NULL;
}

glbInt32 CGlbFields::FindFieldIndex(const glbWChar* iname)
{
	if(iname == NULL)
	{
		GlbSetLastError(L"参数是空值");
		return -1;
	}
	CGlbWString str(iname);
	str.ToUpper();
	for(glbInt32 i = 0; i < (glbInt32)mpr_fields.size(); i++)
	{
		if (wcscmp(str.c_str(),mpr_fields[i]->GetName()) == 0)
		{
			return i;
		}
	}
	GlbSetLastError(L"字段不存在");
	return -1;
}

glbBool CGlbFields::AddField(CGlbField* field)
{
	if(field == NULL || field->GetName() == NULL)
	{
		GlbSetLastError(L"参数是空值,或字段名不存在");
		return false;
	}
	if (GetFieldByName(field->GetName()))
	{
		GlbSetLastError(L"该字段已存在");
		return false;
	}
	mpr_fields.push_back(field);
	return true;
}

glbBool CGlbFields::RemoveField(glbInt32 fidx)
{
	if(fidx < 0)
	{
		GlbSetLastError(L"参数小于0");
		return false;
	}
	if(fidx >= (glbInt32)mpr_fields.size())
	{
		GlbSetLastError(L"参数大于字段数目");
		return false;
	}
	mpr_fields.erase(mpr_fields.begin()+fidx);
	return true;
}

glbBool CGlbFields::RemoveFieldByName(const glbWChar* iname)
{
	if(iname == NULL)
	{
		GlbSetLastError(L"参数是空值");
		return NULL;
	}
	CGlbWString str(iname);
	str.ToUpper();
	for(glbInt32 i = 0; i < (glbInt32)mpr_fields.size(); i++)
	{
		if(wcscmp(str.c_str(),mpr_fields[i]->GetName())==0)
		{
			return RemoveField(i);
		}
	}
	GlbSetLastError(L"字段不存在");
	return false;
}
glbWChar* CGlbFields::GetLastError()
{
	return GlbGetLastError();
}