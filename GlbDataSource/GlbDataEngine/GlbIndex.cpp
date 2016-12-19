#include "StdAfx.h"
#include "GlbIndex.h"
#include "GlbError.h"

CGlbIndex::CGlbIndex()
{
 mpr_fields = new CGlbFields();
 mpr_isAscending = false;
 mpr_isUnique = false;
}
CGlbIndex::~CGlbIndex()
{
 if(mpr_fields)delete mpr_fields;
}

glbBool CGlbIndex::SetName(const glbWChar* iname)
{
 if(iname == NULL)
  return false;
 mpr_name = iname;
 return true;
}

const glbWChar* CGlbIndex::GetName()
{
 return mpr_name.c_str();
}

glbBool CGlbIndex::AddField(CGlbField* field)
{
 if(field == NULL)
  return false;
 return mpr_fields->AddField(field);
}

glbBool CGlbIndex::RemoveField(glbInt32 fidx)
{
 return mpr_fields->RemoveField(fidx);
}

glbBool CGlbIndex::RemoveFieldByName(const glbWChar* fname)
{
 return mpr_fields->RemoveFieldByName(fname);
}

glbInt32 CGlbIndex::GetFieldCount()
{
 return mpr_fields->GetFieldCount();
}

CGlbField* CGlbIndex::GetField(glbInt32 fidx)
{
 return mpr_fields->GetField(fidx);
}

CGlbField* CGlbIndex::GetFieldByName(const glbWChar* fname)
{
 return mpr_fields->GetFieldByName(fname);
}

glbBool CGlbIndex::SetAscending(glbBool ascend)
{
 mpr_isAscending = ascend;
 return true;
}

glbBool CGlbIndex::IsAscending()
{
 return mpr_isAscending;
}

glbBool CGlbIndex::SetUnique(glbBool unique)
{
 mpr_isUnique = unique;
 return true;
}

glbBool CGlbIndex::IsUniqure()
{
 return mpr_isUnique;
}

const glbWChar* CGlbIndex::GetLastError()
{
	return GlbGetLastError();
}
