/********************************************************************
 * Copyright (c) 2013 ��ά����
 * All rights reserved.
 *
 * @file    
 * @brief   
 * @version 1.0
 * @author  GWB
 * @date    2014-03-10 11:21
 ********************************************************************
 */
#pragma once
#include "GlbWString.h"
#include "GlbDataEngineExport.h"
#include "GlbDataEngineType.h"
#include "GlbReference.h"
#pragma warning(disable:4251)
/**
  * @brief �ֶ���
  *
  * �����ֶε���Ϣ���ɻ�ȡ�������޸�
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbField :public CGlbReference
{
public:
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
 CGlbField();
/**
  * @brief ��������
  *
  * û���κβ���������ֵ
  */
 ~CGlbField();
private:
 CGlbWString             mpr_name;					///<�ֶ���
 CGlbWString             mpr_alias; 				///<�ֶα���
 GlbDataTypeEnum		 mpr_type;					///<�ֶ�����
 GlbGeometryTypeEnum	 mpr_geotype;				///<�ֶμ�������
 glbInt32		         mpr_length;				///<�ֶγ���
 CGlbWString             mpr_unit;  				///<��λ
 CGlbWString             mpr_userid;				///<�û���
 CGlbWString             mpr_password;				///<����
 glbBool                 mpr_isrequired;			///<�Ƿ����
 glbBool                 mpr_iscannull;				///<�Ƿ���Ϊ��
 CGlbWString			 mpr_default;        		///<Ĭ��ֵ
 glbInt32                mpr_id;					///<idֵ
public:

/**
  * @brief ��ȡ�ֶ�idֵ
  *
  * @return �ֶ�id
  */
 const glbInt32          GetID();
/**
  * @brief �����ֶ�id
  *
  * �ַ��������ַ���
  *
  * @param id ���õ�idֵ
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetID(glbInt32 id);
/**
  * @brief ��ȡ�ֶ���,�����ִ�Сд
  *
  * @return �ֶ���
  */
 const glbWChar*         GetName();
/**
  * @brief �����ֶ���,�����ִ�Сд
  *
  * @param name ���������ֶ���
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetName(const glbWChar* name);
/**
  * @brief ��ȡ�ֶα���,�����ִ�Сд
  *
  * @return �ֶα���
  */
 const glbWChar*         GetAlias();
/**
  * @brief �����ֶα���
  *
  * @param aliasname �������õ��ֶα���
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetAlias(const glbWChar* aliasname);
/**
  * @brief ��ȡ�ֶ�����
  *
  * @return �ֶ�����
  */
 GlbDataTypeEnum         GetType();
/**
  * @brief �����ֶε���������
  *
  * @param datatype �ֶ�����
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetType(GlbDataTypeEnum datatype);
/**
  * @brief ��ȡGLB_DATATYPE_STRING�ַ����ֶεĳ���
  *
  * @return �ֶεĳ���
  */
 glbInt32                GetLength();
/**
  * @brief ����GLB_DATATYPE_STRING�ַ����ֶεĳ���
  *
  * @param length    ����
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetLength(const glbInt32 length);
/**
  * @brief ��ȡGLB_DATATYPE_GEOMETRY�����ֶεļ�������
  *
  * @return ��������
  */
 GlbGeometryTypeEnum     GetGeoType();
/**
  * @brief ����GLB_DATATYPE_GEOMETRY�����ֶεļ�������
  *
  * @param geoType ��������
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetGeoType(GlbGeometryTypeEnum geoType);
/**
  * @brief ��ȡ�ֶεĵ�λ
  *
  * @return ��λ
  */
 const glbWChar*         GetUnit();
/**
  * @brief �����ֶεĵ�λ
  *
  * @param unit ��λ
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetUnit(const glbWChar* unit);
/**
  * @brief ��ȡ�û�id
  *
  * @return �û�id
  */
 const glbWChar*         GetUserID();
/**
  * @brief �����û�id
  *
  * @param userid �û�id
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetUserID(const glbWChar* userid);
/**
  * @brief ��ȡ�û�����
  *
  * @return �û�����
  */
 const glbWChar*         GetPassWord();
/**
  * @brief �����û�����
  *
  * @param pw �û�����
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetPassWord(const glbWChar* pw);
/**
  * @brief �жϸ��ֶ��Ƿ���ϵͳ�ֶ�,ϵͳ�ֶβ���ɾ��.
  *
  * @return ��true������false
  */
 glbBool                 IsRequired();
/**
  * @brief �����ֶ��Ƿ���ϵͳ�ֶ�
  *
  * @param bisrequired ����ֵ��true��false
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetRequired(glbBool bisrequired);
/**
  * @brief �жϸ��ֶε�ֵ�Ƿ������NULL.
  *
  * @return ��true������false
  */
 glbBool                 IsCanNull();
/**
  * @brief �����ֶ��Ƿ����Ϊ��
  * @brief GLB_DATATYPE_GEOMETRY,GLB_DATATYPE_BLOB �ֶα�������NULL.
  *
  * @param bisnull ����ֵ��true��false
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetCanNull(glbBool bisnull);
/**
  * @brief ����Ĭ��ֵ
  *        Geometry��BlobĬ��ֵΪNULL,����Ҫ����
           GLB_DATATYPE_BYTE
		   GLB_DATATYPE_CHAR		     
		   GLB_DATATYPE_INT16    
           GLB_DATATYPE_INT32    
           GLB_DATATYPE_INT64    
           GLB_DATATYPE_FLOAT    
           GLB_DATATYPE_DOUBLE
		       ��������,Ĭ��ֵת��Ϊ�ַ����洢
		   GLB_DATATYPE_BOOLEAN
		       true ת��Ϊ "1",false ת��Ϊ "0"
           GLB_DATATYPE_STRING
		   GLB_DATATYPE_DATE
		       ����(ʱ��)ת��ΪDouble��ת��Ϊ�ַ����洢
		   GLB_DATATYPE_VECTOR
		       ת��Ϊ"x,y,z"��ʽ���ַ����洢
  * @param df Ĭ��ֵ
  * @return �ɹ�true��ʧ��false
  */
 glbBool                 SetDefault(const glbWChar* df);
/**
  * @brief ��ȡ�ֶ�Ĭ��ֵ
  *
  *        Geometry��BlobĬ��ֵΪNULL,����Ҫ����
           GLB_DATATYPE_BYTE
		   GLB_DATATYPE_CHAR		     
		   GLB_DATATYPE_INT16    
           GLB_DATATYPE_INT32    
           GLB_DATATYPE_INT64    
           GLB_DATATYPE_FLOAT    
           GLB_DATATYPE_DOUBLE
		       ��������,Ĭ��ֵת��Ϊ�ַ����洢
		   GLB_DATATYPE_BOOLEAN
		       true ת��Ϊ "1",false ת��Ϊ "0"
           GLB_DATATYPE_STRING
		   GLB_DATATYPE_DATE
		       ����(ʱ��)ת��ΪDouble��ת��Ϊ�ַ����洢
		   GLB_DATATYPE_VECTOR
		       ת��Ϊ"x,y,z"��ʽ���ַ����洢
  * @return �ֶ�Ĭ��ֵ
  */
 const glbWChar*         GetDefault();
/**
  * @brief ��ȡ�ϴ�ִ�еĴ�����Ϣ
  *
  * @return ������Ϣ
  */
 glbWChar*               GetLastError();
};