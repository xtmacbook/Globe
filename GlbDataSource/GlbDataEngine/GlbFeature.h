#pragma once
#include <vector>
#include <map>
#include "IGlbDataEngine.h"
using namespace std;
/**
  * @brief Ҫ�ض���
  *
  * һ��Ҫ�ض��󣬰���������ͨ�ֶκͼ����ֶ�
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbFeature :public CGlbReference
{
public:
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	CGlbFeature( IGlbFeatureClass* cls,CGlbFields* fields);
/**
  * @brief ��������
  *
  * û���κβ���������ֵ
  */
	~CGlbFeature(void);
 /**
  * @brief ��ʼ��Ҫ�ض���
  *
  * @param values �ֶ�ֵ
  * @return �ɹ�true��ʧ��false
  */
	virtual glbBool     Init(GLBVARIANT** values,CGlbExtent* ext);
public:
/**
  * @brief ��ȡ����id
  *
  * @return �ɹ�������id��ʧ�ܣ�-1
  */
	glbInt32                  GetOid();
/**
  * @brief ���ö����oid
  *
  * @param oid ���õ�oidֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				      SetOid(glbInt32 oid);
/**
  * @brief ��ȡ�����FeatureClass
  *
  * @return �ɹ�:���ض���FeatureClass��ʧ��: NULL
  */
	const IGlbFeatureClass*   GetClass();
/**
  * @brief ��ȡ����������ֶ���Ϣ
  *
  * @return �ɹ�:�����ֶ���Ϣ��ʧ��: NULL
  */
	const CGlbFields*         GetFields();
/**
  * @brief ��ȡ����Ŀռ�������Ϣ
  *
  * @return �ɹ�:���ؿռ�������Ϣ��ʧ��: NULL
  */
	const glbWChar*           GetSRS();
/**
  * @brief ��ȡ���������ε����
  *
  * @return �ɹ�:���������ʧ��: NULL
  */
	const CGlbExtent*         GetExtent();
/**
  * @brief ��ȡ�����ֶ�ֵ
  *
  * @param fidx    �ֶ�λ������
  * @param value   ��ȡ��ֵ
  * @param isdirty ֵ�Ƿ��޸Ĺ�
  * @param isclean �Ƿ�����޸ı�־
  * @return �ɹ�true��ʧ��false
  */
	const GLBVARIANT*         GetValue(glbInt32 fidx,glbBool *isdirty=NULL,glbBool isclean=false);
/**
  * @brief ��ȡ�����ֶ�ֵ
  *
  * @param fname �ֶ���
  * @param value ��ȡ��ֵ
  * @param isdirty ֵ�Ƿ��޸Ĺ�
  * @param isclean �Ƿ�����޸ı�־
  * @return �ɹ�true��ʧ��false
  */
	const GLBVARIANT*         GetValueByName( const glbWChar* fname,glbBool *isdirty=NULL,glbBool isclean=false);
/**
  * @brief ���ö����ֶ�ֵ
  *
  * @param fidx  �ֶ�λ������
  * @param value ���õ�ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool                   PutValue(glbInt32 fidx, GLBVARIANT& value);
/**
  * @brief ���ö����ֶ�ֵ
  *
  * @param fname �ֶ�����
  * @param value ���õ�ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool                   PutValueByName(const glbWChar* fname, GLBVARIANT& value);
/**
  * @brief ��ȡ�ϴ�ִ�еĴ�����Ϣ
  *
  * @return ������Ϣ
  */
	glbWChar*                 GetLastError();
/**
  * @brief ��ȡһ���ļ��ζ���
  *
  * @param geo   ���ζ���
  * @param level ����
  * @return �ɹ�true��ʧ��false
  */
	glbBool	                  GetGeometry(IGlbGeometry** geo, glbUInt32 level=0,glbBool *isdirty=NULL,glbBool isclean=false,glbBool needquery=true);
/**
  * @brief ���ü�������
  *
  * @param geo   Ҫ���õļ�������
  * @param level Ҫ���õ����ݵļ���
  * @return �ɹ�true��ʧ��false
  */
	glbBool                   PutGeometry(IGlbGeometry* geo, glbUInt32 level,glbBool setdirty=true);
private:
	typedef struct _FieldValue
	{
		glbBool							isdirty;///<ֵ�Ƿ��޸�
		GLBVARIANT*						value;	///<���α���
		_FieldValue()
		{
			isdirty = false;
			value   = NULL;
		}
		~_FieldValue()
		{
			if(value)
			{
				glbVariantClean(*value);
				delete value;
			}
		}
	}FIELDVALUE;
	typedef struct _GeometryValue
	{
		glbBool                         isdirty;///<ֵ�Ƿ��޸�
		glbref_ptr<IGlbGeometry>        geo;	///<����
		_GeometryValue()
		{
			isdirty = false;
			geo     = NULL;
		}
	}GEOMETRYVALUE;

	glbref_ptr<IGlbFeatureClass>		 mpr_class;	
	glbref_ptr<CGlbFields>               mpr_fields;
	vector<FIELDVALUE>					 mpr_values;
protected://�̳��߿��Է���.	
	glbref_ptr<CGlbExtent>				  mpr_extent;
	map<glbUInt32/*����*/, GEOMETRYVALUE> mpr_geos;	 
};