#include "StdAfx.h"
#include "GlbIndexes.h"
#include "GlbError.h"

CGlbIndexes::CGlbIndexes()
{
}

CGlbIndexes::~CGlbIndexes()
{
}

glbInt32 CGlbIndexes::GetIndexesCount()
{
	return mpr_indexes.size();
}

CGlbIndex* CGlbIndexes::GetIndex(glbInt32 fidx)
{
	if(fidx < 0)
	{
		GlbSetLastError(L"����С��0");
		return false;
	}
	if(fidx >= (glbInt32)mpr_indexes.size())
	{
		GlbSetLastError(L"��������������Ŀ");
		return false;
	}
	return mpr_indexes[fidx].get();
}

CGlbIndex* CGlbIndexes::GetIndexByName(const glbWChar* iname)
{
	if(iname == NULL)
	{
		GlbSetLastError(L"�����ǿ�ֵ");
		return NULL;
	}
	for(glbInt32 i = 0; i < (glbInt32)mpr_indexes.size();i++)
	{
		if(wcscmp(iname,mpr_indexes[i]->GetName())==0)
			return mpr_indexes[i].get();
	}
	GlbSetLastError(L"����������");
	return NULL;
}

glbBool CGlbIndexes::AddIndex(CGlbIndex* index)
{
	if(index == NULL)
	{
		GlbSetLastError(L"�����ǿ�ֵ");
		return false;
	}
	mpr_indexes.push_back(index);
	return true;
}

glbBool CGlbIndexes::RemoveIndex(glbInt32 fidx)
{
	if(fidx < 0)
	{
		GlbSetLastError(L"����С��0");
		return false;
	}
	if(fidx >= (glbInt32)mpr_indexes.size())
	{
		GlbSetLastError(L"��������������Ŀ");
		return false;
	}
	mpr_indexes.erase(mpr_indexes.begin()+fidx);
	return true;
}

glbBool CGlbIndexes::RemoveIndexByName( const glbWChar* name )
{
	if (NULL == name)
	{
		GlbSetLastError(L"�����ǿ�ֵ");
		return false;
	}
	for (glbInt32 i =0; i < (glbInt32)mpr_indexes.size(); i++)
	{
		if ( wcscmp(mpr_indexes[i]->GetName(),name) == 0)
		{
			mpr_indexes.erase(mpr_indexes.begin()+i);
			return true;
		}
	}
	return false;
}

const glbWChar* CGlbIndexes::GetLastError()
{
	return GlbGetLastError();
}