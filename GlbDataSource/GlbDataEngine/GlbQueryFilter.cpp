#include "StdAfx.h"
#include "GlbQueryFilter.h"

CGlbQueryFilter::CGlbQueryFilter()
{
	mpr_strWhere         = L"";
	mpr_strPrefixClause  = L"";
	mpr_strPostfixClause = L"";
	mpr_strFieldsStr     = L"";
	mpr_geo              = NULL;
	mpr_spatialOp        = 0;
	mpr_startrow         = 0;
	mpr_endrow           = INT_MAX-1;

	mpr_isQueryGeo		 = true;
}

CGlbQueryFilter::~CGlbQueryFilter()
{

}
/*
		作者		:gwb
		创建日期	:2013.2.22
		功能业务说明:设置where子句
		输入参数    :
			-wherestr，where子句字符串
		输出参数	:
			-	无
	    返回值      : 1 设置成功，0 设置失败
*/
glbBool CGlbQueryFilter::SetWhereStr(const glbWChar* wherestr )
{
	if(wherestr == NULL)mpr_strWhere = L"";
	mpr_strWhere = wherestr;
	return true;
}

/*
	作者		:gwb
	创建日期	:2013.2.22
	功能业务说明:获取where子句
	输入参数    :
		-
	输出参数	:
		-	无
	返回值      : 返回where子句字符串
*/
const glbWChar* CGlbQueryFilter::GetWhereStr()
{
	if(mpr_strWhere == L"")return NULL;
	return mpr_strWhere.c_str();
}

/*
		作者		:gwb
		创建日期	:2013.2.22
		功能业务说明:设置sql后缀
		输入参数    :
			-clause，sql后缀
		输出参数	:
			-	无
	    返回值      : 1 设置成功，0 设置失败
*/
glbBool CGlbQueryFilter::SetPostfixClause(const glbWChar* clause)
{
	if(clause == NULL)mpr_strPostfixClause=L"";		
	mpr_strPostfixClause = clause;
	return 1;
}

/*
	作者		:gwb
	创建日期	:2013.2.22
	功能业务说明:获取sql后缀
	输入参数    :
		-
	输出参数	:
		-	无
	返回值      : 返回sql后缀
*/
const glbWChar* CGlbQueryFilter::GetPostfixClause()
{
	if (mpr_strPostfixClause == L"")return NULL;	
	return mpr_strPostfixClause.c_str();
}

/*
		作者		:gwb
		创建日期	:2013.2.22
		功能业务说明:设置sql前缀
		输入参数    :
			-clause，sql前缀
		输出参数	:
			-	无
	    返回值      : 1 设置成功，0 设置失败
*/
glbBool CGlbQueryFilter::SetPrefixClause(const glbWChar* clause)
{
	if(clause == NULL)mpr_strPrefixClause=L"";		
	mpr_strPrefixClause = clause;
	return 1;
}

/*
		作者		:gwb
		创建日期	:2013.2.22
		功能业务说明:获取sql前缀
		输入参数    :
			-
		输出参数	:
			-	无
	    返回值      : 返回sql前缀
*/
const glbWChar* CGlbQueryFilter::GetPrefixClause()
{
	if (mpr_strPrefixClause==L"")return NULL;
	return mpr_strPrefixClause.c_str();
}

/*
		作者		:gwb
		创建日期	:2013.2.22
		功能业务说明:设置sql字段集
		输入参数    :
			-fields，sql字段集
		输出参数	:
			-	无
	    返回值      : 1 设置成功，0 设置失败
*/
glbBool CGlbQueryFilter::SetFields(const glbWChar* fields)
{
	if(fields == NULL)mpr_strFieldsStr = L"";
	mpr_strFieldsStr = fields;
	return 1;
}

/*
		作者		:gwb
		创建日期	:2013.2.22
		功能业务说明:获取sql语句中的字段集
		输入参数    :
			-
		输出参数	:
			-	无
	    返回值      :  返回sql中字段集
*/
const glbWChar* CGlbQueryFilter::GetFields()
{
	if (mpr_strFieldsStr == L"")return NULL;
	return mpr_strFieldsStr.c_str();
}

glbBool   CGlbQueryFilter::PutSpatialFilter(IGlbGeometry *geo, glbInt32 spatialOp)
{
	mpr_geo       = geo;
	mpr_spatialOp = spatialOp;
	return true;
}

IGlbGeometry* CGlbQueryFilter::GetSpatialFilter(glbInt32* spatialOp)
{
	if(spatialOp!=NULL)*spatialOp = mpr_spatialOp;
	return mpr_geo.get();
}

glbBool CGlbQueryFilter::SetStartEndRow(glbInt32 startrow,glbInt32 endrow)
{
	if(startrow <0)
	{
		GlbSetLastError(L"起始行必须>=0");
		return false;
	}
	if(endrow<startrow)
	{
		GlbSetLastError(L"结束行>=起始行");
		return false;
	}
	mpr_startrow = startrow;
	mpr_endrow = endrow;
	return true;
}

glbBool CGlbQueryFilter::GetStartEndRow( glbInt32* startrow,glbInt32* endrow )
{
	if(startrow)
		*startrow = mpr_startrow;
	if(endrow)
		*endrow = mpr_endrow;
	return true;
}

glbWChar* CGlbQueryFilter::GetLastError()
{
	return GlbGetLastError();
}

glbBool CGlbQueryFilter::SetCameraDir(glbDouble xdir, glbDouble ydir, glbDouble zdir)
{
	mpr_cameraDir.clear();
	mpr_cameraDir.push_back(xdir);
	mpr_cameraDir.push_back(ydir);
	mpr_cameraDir.push_back(zdir);
	return true;
}

glbBool CGlbQueryFilter::GetCameraDir(glbDouble* xdir, glbDouble* ydir, glbDouble* zdir)
{
	if (mpr_cameraDir.size()<3) return false;
	if (*xdir==NULL || *ydir==NULL || *zdir==NULL) return false;

	*xdir = mpr_cameraDir.at(0);
	*ydir = mpr_cameraDir.at(1);
	*zdir = mpr_cameraDir.at(2);

	return true;
}

glbBool CGlbQueryFilter::SetIsQueryGeometry(glbBool isQGeo)
{
	mpr_isQueryGeo = isQGeo;
	return true;
}

glbBool CGlbQueryFilter::IsQueryGeometry()
{
	return mpr_isQueryGeo;
}