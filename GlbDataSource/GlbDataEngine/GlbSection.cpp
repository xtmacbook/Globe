#include "stdafx.h"
#include "GlbSection.h"
#include "GlbWString.h"

CGlbSection::CGlbSection(IGlbFeatureClass* cls,CGlbFields* fields):
CGlbFeature(cls,fields)
{
}


CGlbSection::~CGlbSection(void)
{
}

const IGlbObjectCursor* CGlbSection::QueryCtrlPt(CGlbQueryFilter* queryFilter)
{
	IGlbFeatureClass*   fc             = (IGlbFeatureClass*)this->GetClass();
	IGlbSectionDataset* sectionDataset = NULL;
	if (fc && fc->QueryInterface(L"IGlbSectionDataset", (void**)(&sectionDataset)))
	{
		IGlbObjectClass* ctrlPtClass = (IGlbObjectClass*)sectionDataset->GetCtrlPtClass();
		if (ctrlPtClass)
		{
			const GLBVARIANT* val = NULL;
			val = this->GetValueByName(L"GLBSECID");
			CGlbWString whereStr;			
			if(queryFilter == NULL)
			{
				CGlbQueryFilter* filter = new CGlbQueryFilter();
				whereStr = L"GLBSECID = '";
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				filter->SetWhereStr(whereStr.c_str());
				IGlbObjectCursor* cursor = ctrlPtClass->Query(filter);				
				return cursor;
			}else{
				if(queryFilter->GetWhereStr() != NULL)
				{
					whereStr.append(queryFilter->GetWhereStr());
					whereStr.append(L" AND ");
				}
				whereStr.append(L"GLBSECID = '");
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				queryFilter->SetWhereStr(whereStr.c_str());				
			}
			return ctrlPtClass->Query(queryFilter);
		}
		else
		{
			GlbSetLastError(L"没有控制点数据集");
			return NULL;
		}
	}
	else
	{
		GlbSetLastError(L"没有初始化");
		return NULL;
	}
}

const IGlbFeatureCursor* CGlbSection::QueryNode(CGlbQueryFilter* queryFilter)
{
	IGlbFeatureClass*   fc             = (IGlbFeatureClass*)this->GetClass();
	IGlbSectionDataset* sectionDataset = NULL;
	if (fc && fc->QueryInterface(L"IGlbSectionDataset", (void**)(&sectionDataset)))
	{
		IGlbFeatureClass* nodeClass = (IGlbFeatureClass*)sectionDataset->GetNodeClass();
		if (nodeClass)
		{
			const GLBVARIANT* val = NULL;
			val = this->GetValueByName(L"GLBSECID");
			CGlbWString whereStr;			
			if(queryFilter == NULL)
			{
				CGlbQueryFilter* filter = new CGlbQueryFilter();
				whereStr = L"GLBSECID = '";
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				filter->SetWhereStr(whereStr.c_str());
				IGlbFeatureCursor* cursor = nodeClass->Query(filter);				
				return cursor;
			}else{
				if(queryFilter->GetWhereStr() != NULL)
				{
					whereStr.append(queryFilter->GetWhereStr());
					whereStr.append(L" AND ");
				}
				whereStr.append(L"GLBSECID = '");
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				queryFilter->SetWhereStr(whereStr.c_str());				
			}
			return nodeClass->Query(queryFilter);
		}
		else
		{
			GlbSetLastError(L"没有节点数据集");
			return NULL;
		}
	}
	else
	{
		GlbSetLastError(L"没有初始化");
		return NULL;
	}
}

const IGlbFeatureCursor* CGlbSection::QueryArc(CGlbQueryFilter* queryFilter)
{
	IGlbFeatureClass*   fc             = (IGlbFeatureClass*)this->GetClass();
	IGlbSectionDataset* sectionDataset = NULL;
	if (fc && fc->QueryInterface(L"IGlbSectionDataset", (void**)(&sectionDataset)))
	{
		IGlbFeatureClass* arcClass = (IGlbFeatureClass*)sectionDataset->GetArcClass();
		if (arcClass)
		{
			const GLBVARIANT* val = NULL;
			val = this->GetValueByName(L"GLBSECID");
			CGlbWString whereStr;			
			if(queryFilter == NULL)
			{
				CGlbQueryFilter* filter = new CGlbQueryFilter();
				whereStr = L"GLBSECID = '";
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				filter->SetWhereStr(whereStr.c_str());
				IGlbFeatureCursor* cursor = arcClass->Query(filter);				
				return cursor;
			}else{
				if(queryFilter->GetWhereStr() != NULL)
				{
					whereStr.append(queryFilter->GetWhereStr());
					whereStr.append(L" AND ");
				}
				whereStr.append(L"GLBSECID = '");
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				queryFilter->SetWhereStr(whereStr.c_str());				
			}
			return arcClass->Query(queryFilter);
		}
		else
		{
			GlbSetLastError(L"没有弧段数据集");
			return NULL;
		}
	}
	else
	{
		GlbSetLastError(L"没有初始化");
		return NULL;
	}
}

const IGlbFeatureCursor* CGlbSection::QueryPolygon(CGlbQueryFilter* queryFilter)
{
	IGlbFeatureClass* fc               = (IGlbFeatureClass*)this->GetClass();
	IGlbSectionDataset* sectionDataset = NULL;
	if (fc && fc->QueryInterface(L"IGlbSectionDataset", (void**)(&sectionDataset)))
	{
		IGlbFeatureClass* polygonClass = (IGlbFeatureClass*)sectionDataset->GetPolygonClass();
		if (polygonClass)
		{
			const GLBVARIANT* val = NULL;
			val = this->GetValueByName(L"GLBSECID");
			CGlbWString whereStr;			
			if(queryFilter == NULL)
			{
				CGlbQueryFilter* filter = new CGlbQueryFilter();
				whereStr = L"GLBSECID = '";
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				filter->SetWhereStr(whereStr.c_str());
				IGlbFeatureCursor* cursor = polygonClass->Query(filter);				
				return cursor;
			}else{
				if(queryFilter->GetWhereStr() != NULL)
				{
					whereStr.append(queryFilter->GetWhereStr());
					whereStr.append(L" AND ");
				}
				whereStr.append(L"GLBSECID = '");
				whereStr.append(val->strVal);
				whereStr.append(L"'");
				queryFilter->SetWhereStr(whereStr.c_str());				
			}
			return polygonClass->Query(queryFilter);
		}
		else
		{
			GlbSetLastError(L"没有多边形数据集");
			return NULL;
		}
	}
	else
	{
		GlbSetLastError(L"没有初始化");
		return NULL;
	}
}