#pragma once
#include "GlbFields.h"
#include "GlbDataEngineExport.h"
/**
  * @brief ������
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbIndex:public CGlbReference
{
public:
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	CGlbIndex();
/**
  * @brief ��������
  *
  * û���κβ���������ֵ
  */
	~CGlbIndex();
/**
  * @brief ��������������
  *
  * @param iname ������
  * @return �ɹ�true��ʧ��false
  */
	glbBool SetName(const glbWChar* iname);
/**
  * @brief ��ȡ������
  *
  * @return ������
  */
	const glbWChar* GetName();
/**
  * @brief ��������ֶ�
  *
  * @param field ��ӵ������ֶ�
  * @return �ɹ�true��ʧ��false
  */
	glbBool AddField(CGlbField* field);
/**
  * @brief ɾ��������ĳ���ֶ�
  *
  * @param fidx ɾ�����ֶ�λ��
  * @return �ɹ�true��ʧ��false
  */
	glbBool RemoveField(glbInt32 fidx);
/**
  * @brief ͨ������ɾ��������ĳ���ֶ�
  *
  * @param fname ɾ�����ֶ�����
  * @return �ɹ�true��ʧ��false
  */
	glbBool RemoveFieldByName(const glbWChar* fname);
/**
  * @brief ��ȡ�������ֶ�����
  *
  * @return �ֶ�����
  */
	glbInt32 GetFieldCount();
/**
  * @brief ��ȡ������ĳ���ֶ�
  *
  * @param fidx �ֶ��������е�λ��
  * @return �ֶ�
  */
	CGlbField* GetField(glbInt32 fidx);
/**
  * @brief �������ֻ�ȡ������ĳ���ֶ�
  *
  * @param fname �ֶε�����
  * @return �ֶ�
  */
	CGlbField* GetFieldByName(const glbWChar* fname);
/**
  * @brief ���������Ƿ�Ϊ����
  *
  * @param ascend ����true������false
  * @return �ɹ�true��ʧ��false
  */
	glbBool SetAscending(glbBool ascend);
/**
  * @brief �ж��Ƿ�Ϊ����
  *
  * @return ����true������false
  */
	glbBool IsAscending();
/**
  * @brief ���������Ƿ���Ψһ����
  *
  * @param unique ��true������false
  * @return �ɹ�true��ʧ��false
  */
	glbBool SetUnique(glbBool unique);
/**
  * @brief �ж��Ƿ���Ψһ����
  *
  * @return ��true������false
  */
	glbBool IsUniqure();
/**
  * @brief ��ȡ�ϴ�ִ�еĴ�����Ϣ
  *
  * @return ������Ϣ
  */
 const glbWChar* GetLastError();
private:
	CGlbWString		mpr_name;			///< ������
	CGlbFields*		mpr_fields;			///< �ֶμ���
	glbBool			mpr_isAscending;	///< �Ƿ�������
	glbBool			mpr_isUnique;		///< �Ƿ���Ψһ����
};