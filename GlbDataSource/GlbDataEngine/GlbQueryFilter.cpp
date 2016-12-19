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
		����		:gwb
		��������	:2013.2.22
		����ҵ��˵��:����where�Ӿ�
		�������    :
			-wherestr��where�Ӿ��ַ���
		�������	:
			-	��
	    ����ֵ      : 1 ���óɹ���0 ����ʧ��
*/
glbBool CGlbQueryFilter::SetWhereStr(const glbWChar* wherestr )
{
	if(wherestr == NULL)mpr_strWhere = L"";
	mpr_strWhere = wherestr;
	return true;
}

/*
	����		:gwb
	��������	:2013.2.22
	����ҵ��˵��:��ȡwhere�Ӿ�
	�������    :
		-
	�������	:
		-	��
	����ֵ      : ����where�Ӿ��ַ���
*/
const glbWChar* CGlbQueryFilter::GetWhereStr()
{
	if(mpr_strWhere == L"")return NULL;
	return mpr_strWhere.c_str();
}

/*
		����		:gwb
		��������	:2013.2.22
		����ҵ��˵��:����sql��׺
		�������    :
			-clause��sql��׺
		�������	:
			-	��
	    ����ֵ      : 1 ���óɹ���0 ����ʧ��
*/
glbBool CGlbQueryFilter::SetPostfixClause(const glbWChar* clause)
{
	if(clause == NULL)mpr_strPostfixClause=L"";		
	mpr_strPostfixClause = clause;
	return 1;
}

/*
	����		:gwb
	��������	:2013.2.22
	����ҵ��˵��:��ȡsql��׺
	�������    :
		-
	�������	:
		-	��
	����ֵ      : ����sql��׺
*/
const glbWChar* CGlbQueryFilter::GetPostfixClause()
{
	if (mpr_strPostfixClause == L"")return NULL;	
	return mpr_strPostfixClause.c_str();
}

/*
		����		:gwb
		��������	:2013.2.22
		����ҵ��˵��:����sqlǰ׺
		�������    :
			-clause��sqlǰ׺
		�������	:
			-	��
	    ����ֵ      : 1 ���óɹ���0 ����ʧ��
*/
glbBool CGlbQueryFilter::SetPrefixClause(const glbWChar* clause)
{
	if(clause == NULL)mpr_strPrefixClause=L"";		
	mpr_strPrefixClause = clause;
	return 1;
}

/*
		����		:gwb
		��������	:2013.2.22
		����ҵ��˵��:��ȡsqlǰ׺
		�������    :
			-
		�������	:
			-	��
	    ����ֵ      : ����sqlǰ׺
*/
const glbWChar* CGlbQueryFilter::GetPrefixClause()
{
	if (mpr_strPrefixClause==L"")return NULL;
	return mpr_strPrefixClause.c_str();
}

/*
		����		:gwb
		��������	:2013.2.22
		����ҵ��˵��:����sql�ֶμ�
		�������    :
			-fields��sql�ֶμ�
		�������	:
			-	��
	    ����ֵ      : 1 ���óɹ���0 ����ʧ��
*/
glbBool CGlbQueryFilter::SetFields(const glbWChar* fields)
{
	if(fields == NULL)mpr_strFieldsStr = L"";
	mpr_strFieldsStr = fields;
	return 1;
}

/*
		����		:gwb
		��������	:2013.2.22
		����ҵ��˵��:��ȡsql����е��ֶμ�
		�������    :
			-
		�������	:
			-	��
	    ����ֵ      :  ����sql���ֶμ�
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
		GlbSetLastError(L"��ʼ�б���>=0");
		return false;
	}
	if(endrow<startrow)
	{
		GlbSetLastError(L"������>=��ʼ��");
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