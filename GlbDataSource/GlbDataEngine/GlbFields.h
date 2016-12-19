#pragma once
#include <vector>
#include "GlbField.h"
#include "GlbDataEngineExport.h"
#include "glbref_ptr.h"
using namespace std;
/**
  * @brief �ֶμ�����
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbFields:public CGlbReference
{
public:
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	CGlbFields();
/**
  * @brief ��������
  *
  * û���κβ���������ֵ
  */
	~CGlbFields();
public:
/**
  * @brief ��ȡ�ֶθ���ֵ
  *
  * @return �ֶ��ܸ���
  */
	glbInt32 GetFieldCount();
/**
  * @brief ��ȡĳ���ֶ�
  *
  * @param fidx �ֶε�λ��
  * @return �ֶ�
  */
	CGlbField* GetField(glbInt32 fidx);
/**
  * @brief �����ֶ����ֻ�ȡĳ���ֶ�
  *
  * @param iname �ֶ���
  * @return �ֶ�
  */
	CGlbField* GetFieldByName(const glbWChar* iname);
/**
  * @brief �����ֶ����ֻ�ȡĳ���ֶ��±�
  *
  * @param fname �ֶ���
  * @return ���ֶ��±�
  */
	glbInt32 FindFieldIndex(const glbWChar* fname);
/**
  * @brief ���һ���ֶ�
  *
  * @param field Ҫ��ӵ��ֶ�
  * @return �ɹ�true��ʧ��false
  */
	glbBool AddField(CGlbField* field);
/**
  * @brief ɾ��һ���ֶ�
  *
  * @param fidx Ҫɾ�����ֶε��±�
  * @return �ɹ�true��ʧ��false
  */
	glbBool RemoveField(glbInt32 fidx);
/**
  * @brief ��������ɾ��һ���ֶ�
  *
  * @param iname Ҫɾ�����ֶε�����
  * @return �ɹ�true��ʧ��false
  */
	glbBool RemoveFieldByName( const glbWChar* iname);
/**
  * @brief ��ȡ�ϴ�ִ�еĴ�����Ϣ
  *
  * @return ������Ϣ
  */
	glbWChar* GetLastError();

private:
	vector<glbref_ptr<CGlbField>> mpr_fields; ///<�ֶμ�������
};