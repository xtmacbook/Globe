#pragma once
#pragma warning(disable:4091)
#include "GlbIndex.h"
#include "GlbDataEngineExport.h"
/**
  * @brief ������
  * 
  * @version 1.0
  * @date    2014-03-10 11:21
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbIndexes:public CGlbReference
{
public:
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
CGlbIndexes();
/**
  * @brief ��������
  *
  * û���κβ���������ֵ
  */
~CGlbIndexes();
public:
/**
  * @brief ��ȡ�������ܸ���
  *
  * @return �ɹ�true��ʧ��false
  */
 glbInt32           GetIndexesCount();
/**
  * @brief ��ȡĳ������
  *
  * @param fidx ������λ��
  * @return ȡ�õ�����
  */
 CGlbIndex*			GetIndex(glbInt32 fidx);
/**
  * @brief ������������ȡĳ������
  *
  * @param iname ����������
  * @return ȡ�õ�����
  */
 CGlbIndex*			GetIndexByName(const glbWChar* iname);
/**
  * @brief �����������һ������
  *
  * @param index ��ӵ�������λ��
  * @return �ɹ�true��ʧ��false
  */
 glbBool            AddIndex(CGlbIndex* index);
/**
  * @brief �Ӽ�����ɾ��һ������
  *
  * @param fidx ɾ��������λ��
  * @return �ɹ�true��ʧ��false
  */
 glbBool			RemoveIndex(glbInt32 fidx);
/**
  * @brief �������ִӼ�����ɾ��һ������
  *
  * @param name ɾ������������
  * @return �ɹ�true��ʧ��false
  */
 glbBool			RemoveIndexByName(const glbWChar* name);
/**
  * @brief ��ȡ�ϴ�ִ�еĴ�����Ϣ
  *
  * @return ������Ϣ
  */
 const glbWChar*	GetLastError();

private:
 vector<glbref_ptr<CGlbIndex>> mpr_indexes; ///< ��������
};