#include "stdafx.h"
#include "GlbDrill.h"
#include "GlbWString.h"

CGlbDrill::CGlbDrill(IGlbFeatureClass* cls,CGlbFields* fields):
CGlbFeature(cls,fields)
{
}

CGlbDrill::~CGlbDrill(void)
{
}

const IGlbFeatureCursor* CGlbDrill::QueryStratum(CGlbQueryFilter* queryFilter)
{
	IGlbFeatureClass* fc           = (IGlbFeatureClass*)this->GetClass();
	IGlbDrillDataset* drillDataset = NULL;
	if (fc && fc->QueryInterface(L"IGlbDrillDataset", (void**)(&drillDataset)))
	{
		IGlbFeatureClass* stratumClass = (IGlbFeatureClass*)drillDataset->GetStratumClass();
		if (stratumClass)
		{
			const GLBVARIANT* val = NULL;
			val = this->GetValueByName(L"GLBPROJECTID");
			CGlbWString whereStr;			
			if(queryFilter == NULL)
			{
				CGlbQueryFilter* filter = new CGlbQueryFilter();
				whereStr = L"GLBPROJECTID = '";
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				filter->SetWhereStr(whereStr.c_str());
				IGlbFeatureCursor* cursor = stratumClass->Query(filter);				
				return cursor;
			}else{
				if(queryFilter->GetWhereStr() != NULL)
				{
					whereStr.append(queryFilter->GetWhereStr());
					whereStr.append(L" AND ");
				}
				whereStr.append(L"GLBPROJECTID = '");
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				queryFilter->SetWhereStr(whereStr.c_str());				
			}
			return stratumClass->Query(queryFilter);
		}
		else
		{
			GlbSetLastError(L"没有地层数据集");
			return NULL;
		}
	}
	else
	{
		GlbSetLastError(L"没有初始化");
		return NULL;
	}
}

const IGlbObjectCursor* CGlbDrill::QuerySkew(CGlbQueryFilter* queryFilter)
{
	IGlbFeatureClass* fc           = (IGlbFeatureClass*)this->GetClass();
	IGlbDrillDataset* drillDataset = NULL;
	if (fc && fc->QueryInterface(L"IGlbDrillDataset", (void**)(&drillDataset)))
	{
		IGlbObjectClass* skewClass = (IGlbObjectClass*)drillDataset->GetSkewClass();
		if (skewClass)
		{
			const GLBVARIANT* val = NULL;
			val = this->GetValueByName(L"GLBPROJECTID");
			CGlbWString whereStr;			
			if(queryFilter == NULL)
			{
				CGlbQueryFilter* filter = new CGlbQueryFilter();
				whereStr = L"GLBPROJECTID = '";
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				filter->SetWhereStr(whereStr.c_str());
				IGlbObjectCursor* cursor = skewClass->Query(filter);				
				return cursor;
			}else{
				if(queryFilter->GetWhereStr() != NULL)
				{
					whereStr.append(queryFilter->GetWhereStr());
					whereStr.append(L" AND ");
				}
				whereStr.append(L"GLBPROJECTID = '");
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				queryFilter->SetWhereStr(whereStr.c_str());				
			}
			return skewClass->Query(queryFilter);
		}
		else
		{
			GlbSetLastError(L"没有测斜数据集");
			return NULL;
		}
	}
	else
	{
		GlbSetLastError(L"没有初始化");
		return NULL;
	}
}

const IGlbObjectCursor* CGlbDrill::QuerySample(CGlbQueryFilter* queryFilter, const glbWChar* spDatasetName)
{
	IGlbFeatureClass* fc           = (IGlbFeatureClass*)this->GetClass();
	IGlbDrillDataset* drillDataset = NULL;
	if (fc && fc->QueryInterface(L"IGlbDrillDataset", (void**)(&drillDataset)))
	{
		IGlbObjectClass* sampleClass = (IGlbObjectClass*)drillDataset->GetSampleClassByName(spDatasetName);
		if (sampleClass)
		{
			const GLBVARIANT* val = NULL;
			val = this->GetValueByName(L"GLBPROJECTID");
			CGlbWString whereStr;			
			if(queryFilter == NULL)
			{
				CGlbQueryFilter* filter = new CGlbQueryFilter();
				whereStr = L"GLBPROJECTID = '";
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				filter->SetWhereStr(whereStr.c_str());
				IGlbObjectCursor* cursor = sampleClass->Query(filter);				
				return cursor;
			}else{
				if(queryFilter->GetWhereStr() != NULL)
				{
					whereStr.append(queryFilter->GetWhereStr());
					whereStr.append(L" AND ");
				}
				whereStr.append(L"GLBPROJECTID = '");
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				queryFilter->SetWhereStr(whereStr.c_str());				
			}
			return sampleClass->Query(queryFilter);
		}
		else
		{
			GlbSetLastError(L"没有样品数据集");
			return NULL;
		}
	}
	else
	{
		GlbSetLastError(L"没有初始化");
		return NULL;
	}
}