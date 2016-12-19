#pragma once
#include <vector>
#include "IGlbDataEngine.h"
using namespace std;
/**
  * @brief ���Զ���
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbObject :public CGlbReference
{
public:
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	CGlbObject(IGlbObjectClass*  cls,CGlbFields* fields);
/**
  * @brief ��������
  *
  * û���κβ���������ֵ
  */
	~CGlbObject(void);
 /**
  * @brief ��ʼ������
  *
  * @param values ������ֶ�ֵ����
  * @return �ɹ�true��ʧ��false
  */
	glbBool Init(GLBVARIANT** values);
public:
 /**
  * @brief ��ȡ����id
  *
  * @return �ɹ�������id��ʧ�ܣ�-1
  */
	glbInt32			    GetOid();
 /**
  * @brief ���ö���id
  *
  * @param oid ����id
  * @return �ɹ�true��ʧ��false
  */
	glbBool				    SetOid(glbInt32 oid);
 /**
  * @brief ��ȡ������
  *
  * @return �ɹ�:���ض����࣬ʧ��: NULL
  */
	const IGlbObjectClass*	GetClass();
 /**
  * @brief ��ȡ�ֶ���Ϣ
  *
  * @return �ɹ�:�����ֶ���Ϣ��ʧ��: NULL
  */
	const CGlbFields*		GetFields();
 /**
  * @brief ��ȡ�ֶ�ֵ
  *
  * @param fidx    �ֶ�����
  * @param isdirty �Ƿ��޸�
  * @param isclean �Ƿ�����޸ı�־
  * @return        �ֶ�ֵ.ʧ�ܷ���NULL.
  */
	const GLBVARIANT*	    GetValue(glbInt32 fidx,glbBool *isdirty=NULL,glbBool isclean=false);
 /**
  * @brief ��ȡ�ֶ�ֵ
  *
  * @param fname   �ֶ���
  * @param value   ֵ
  * @param isdirty �Ƿ��޸�
  * @param isclean �Ƿ�����޸ı�־
  * @return �ɹ�true��ʧ��false
  */
	const GLBVARIANT*		GetValueByName(const glbWChar* fname,glbBool *isdirty=NULL,glbBool isclean=false);
 /**
  * @brief �����ֶ�ֵ
  *
  * @param fidx  �ֶ�����
  * @param value ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				    PutValue(glbInt32 fidx, GLBVARIANT& value);
 /**
  * @brief �����ֶ�ֵ
  *
  * @param fname �ֶ�����
  * @param value ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				    PutValueByName(const glbWChar*  fname,GLBVARIANT& value);
 /**
  * @brief ��ȡ�ϴ�ִ�еĴ�����Ϣ
  *
  * @return ������Ϣ
  */
	glbWChar*		        GetLastError();
/**
  * @brief ��ȡ��С
  *
  */
	glbUInt32               GetSize();
private:
	typedef struct _FieldValue
	{
		glbBool							isdirty;///<�Ƿ��޸�
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
	glbref_ptr<IGlbObjectClass>	 mpr_class;	 ///<Ҫ����	   
	glbref_ptr<CGlbFields>		 mpr_fields; ///<����Ϣ
	vector<FIELDVALUE>			 mpr_values; ///<�ֶ���Ϣ
};