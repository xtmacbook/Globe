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
		GlbSetLastError(L"����С��0");
		return NULL;
	}
	if(fidx >= (glbInt32)mpr_fields.size())
	{
		GlbSetLastError(L"���������ֶ���Ŀ");
		return NULL;
	}
	return mpr_fields[fidx].get();
}

CGlbField* CGlbFields::GetFieldByName(const glbWChar* iname)
{
	if(iname == NULL)
	{
		GlbSetLastError(L"�����ǿ�ֵ");
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
	GlbSetLastError(L"�ֶβ�����");
	return NULL;
}

glbInt32 CGlbFields::FindFieldIndex(const glbWChar* iname)
{
	if(iname == NULL)
	{
		GlbSetLastError(L"�����ǿ�ֵ");
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
	GlbSetLastError(L"�ֶβ�����");
	return -1;
}

glbBool CGlbFields::AddField(CGlbField* field)
{
	if(field == NULL || field->GetName() == NULL)
	{
		GlbSetLastError(L"�����ǿ�ֵ,���ֶ���������");
		return false;
	}
	if (GetFieldByName(field->GetName()))
	{
		GlbSetLastError(L"���ֶ��Ѵ���");
		return false;
	}
	mpr_fields.push_back(field);
	return true;
}

glbBool CGlbFields::RemoveField(glbInt32 fidx)
{
	if(fidx < 0)
	{
		GlbSetLastError(L"����С��0");
		return false;
	}
	if(fidx >= (glbInt32)mpr_fields.size())
	{
		GlbSetLastError(L"���������ֶ���Ŀ");
		return false;
	}
	mpr_fields.erase(mpr_fields.begin()+fidx);
	return true;
}

glbBool CGlbFields::RemoveFieldByName(const glbWChar* iname)
{
	if(iname == NULL)
	{
		GlbSetLastError(L"�����ǿ�ֵ");
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
	GlbSetLastError(L"�ֶβ�����");
	return false;
}
glbWChar* CGlbFields::GetLastError()
{
	return GlbGetLastError();
}