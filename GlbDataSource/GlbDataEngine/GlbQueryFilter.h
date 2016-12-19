#pragma once
#include "IGlbGeometry.h"
/**
* @brief ��ѯ����
* 
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API CGlbQueryFilter: public CGlbReference
{
public:
	/**
	* @brief ���캯��
	*
	* û���κβ���������ֵ
	*/
	CGlbQueryFilter();
	/**
	* @brief ��������
	*
	* û���κβ���������ֵ
	*/
	~CGlbQueryFilter();
	/**
	* @brief ����where�Ӿ�
	*
	* @param wherestr Ҫ���õ�where�Ӿ�
	* @return �ɹ�:true��ʧ��:false
	*/
	glbBool SetWhereStr(const glbWChar* wherestr );
	/**
	* @brief ��ȡwhere�Ӿ�
	*
	* @return �ɹ�:where�Ӿ䣬ʧ��:NULL
	*/
	const glbWChar* GetWhereStr();
	/**
	* @brief ���ò�ѯ����׺�ؼ���
	*
	* @param clause Ҫ���õĺ�׺�ؼ���
	* @return �ɹ�:true��ʧ��:false
	*/
	glbBool SetPostfixClause(const glbWChar* clause);
	/**
	* @brief ��ȡ��ѯ����׺�ؼ���
	*
	* @return �ɹ�:���ز�ѯ����׺�ؼ��֣�ʧ��:NULL
	*/
	const glbWChar* GetPostfixClause();
	/**
	* @brief ���ò�ѯ���ǰ׺�ؼ���
	*
	* @param clause Ҫ���õ�ǰ׺�ؼ���
	* @return �ɹ�:true��ʧ��:false
	*/
	glbBool SetPrefixClause(const glbWChar* clause);
	/**
	* @brief ��ȡ��ѯ���ǰ׺�ؼ���
	*
	* @return �ɹ�:���ز�ѯ���ǰ׺�ؼ��֣�ʧ��:NULL
	*/
	const glbWChar* GetPrefixClause();
	/**
	* @brief ���ò�ѯ���ֶ�
	*
	* @param fields Ҫ���õĲ�ѯ�ֶ�
	* @return �ɹ�:true��ʧ��:false
	*/
	glbBool SetFields(const glbWChar* fields);
	/**
	* @brief ��ȡ��ѯ���ֶ�
	*
	* @return �ɹ�:���ز�ѯ���ֶΣ�ʧ��:NULL
	*/
	const  glbWChar* GetFields();
	/**
	* @brief ���ÿռ��ѯ��
	*
	* @param geo �������ռ��ѯ�ļ��ζ���
	* @param spatialOp ��ѯ�Ĳ������ͣ����������ѯ���ռ��ཻ�Ȳ�ѯ
	* @return �ɹ�:true��ʧ��:false
	*/
	glbBool   PutSpatialFilter(IGlbGeometry *geo, glbInt32 spatialOp);
	/**
	* @brief ��ȡ�����ռ��ѯ�ļ��ζ���
	*
	* @param spatialOp ��������
	* @return �ɹ�:���ؼ��ζ���ʧ��:NULL
	*/
	IGlbGeometry* GetSpatialFilter(glbInt32* spatialOp);
	/**
	* @brief ��ѯ����ٴι��ˣ����ò鿴���������ʼ��
	*
	* @param startrow �鿴����ʼ��,Ĭ������Ϊ-1
	* @param endrow �鿴��ֹ��,Ĭ������Ϊ-1
	* @return �ɹ�:true��ʧ��:false
	*/
	glbBool SetStartEndRow(glbInt32 startrow = -1,glbInt32 endrow = -1);
	/**
	* @brief ��ȡ�鿴���������ʼ��
	*
	* @param startrow ���ز鿴����ʼ��
	* @param endrow ���ز鿴����ֹ��
	* @return �ɹ�:true��ʧ��:false
	*/
	glbBool GetStartEndRow(glbInt32* startrow,glbInt32* endrow);
	/**
	* @brief ��ȡ���һ��ִ�еĴ�����Ϣ
	*
	* @return ������Ϣ
	*/
	glbWChar* GetLastError();
	/**
	* @brief �����������  ������ - ���λ�ã�
	*
	* @param xdir,ydir,zdir �������
	* @return �ɹ�:true��ʧ��:false
	*/	
	glbBool SetCameraDir(glbDouble xdir, glbDouble ydir, glbDouble zdir);
	/**
	* @brief ��ȡ�������  ������ - ���λ�ã�
	*
	* @param xdir,ydir,zdir �������
	* @return �ɹ�:true��ʧ��:false
	*/
	glbBool GetCameraDir(glbDouble* xdir, glbDouble* ydir, glbDouble* zdir);
	/**
	* @brief �����Ƿ���Ҫ��ѯGeometry�ֶ�
	*
	* @param isQGeo - true(Ĭ��) �� ��ѯgeometry�ֶΣ� false : ����ѯgeomtry�ֶ�
	* @return �ɹ�:true��ʧ��:false
	* @note ��geometry�ֶ��������ܴ���tin������ʵ�����������Ҫʹ��geometryʱ���ô˽ӿ�
	*/	
	glbBool SetIsQueryGeometry(glbBool isQGeo);
	/**
	* @brief ��ȡ�Ƿ���Ҫ��ѯGeometry�ֶ�
	*
	* @param true�� ��ѯgeometry�ֶΣ� false : ����ѯgeomtry�ֶ�
	* @return �ɹ�:true��ʧ��:false
	*/
	glbBool IsQueryGeometry();
private:
	CGlbWString						mpr_strWhere;			///< ��ѯ��where�Ӿ�
	CGlbWString						mpr_strPostfixClause;	///< ��ѯ�ĺ�׺���
	CGlbWString						mpr_strPrefixClause;	///< ��ѯ��ǰ׺���
	CGlbWString						mpr_strFieldsStr;		///< ��ѯ���ֶμ�
	glbref_ptr<IGlbGeometry>		mpr_geo;				///< �ռ��ѯ�ļ��ζ���
	glbInt32						mpr_spatialOp;			///< �ռ��ѯ�Ĳ������ͣ������ѯ���ཻ��ѯ��
	glbInt32						mpr_startrow;			///< �鿴����ʼ��
	glbInt32						mpr_endrow;				///< �鿴����ֹ��
	std::vector<glbDouble>			mpr_cameraDir;			///< �������
	glbBool							mpr_isQueryGeo;			///< �Ƿ��ѯgeo�ֶ�
};