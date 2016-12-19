/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeObject.h
* @brief   ����ģ��ͷ�ļ����ĵ�����CGlbGlobeObject��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-5 14:20
*********************************************************************/
#pragma once
#include "GlbReference.h"
#include "GlbGlobeExport.h"
#include "GlbGlobeTypes.h"
#include "GlbWString.h"
#include "libxml/tree.h"
namespace GlbGlobe
{
	class CGlbGlobe;
	/**
	* @brief ����ģ����				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-5 14:20
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeObject : public CGlbReference
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeObject();
		/**
		* @brief ��������
		*/
		~CGlbGlobeObject();
		/**
		* @brief ��ȡ��������
		* @return ��������
		*/
		const glbWChar *GetName();
		/**
		* @brief ���ö�������
		* @param[in]  name ��������
		*/
		void SetName(const glbWChar *name);
		/**
		* @brief ��ȡ��������
		* @return ����ö������
		*/
		virtual GlbGlobeObjectTypeEnum GetType();
		/**
		* @brief ��ȡ����ID
		* @return ����ID
		*/
		glbInt32 GetId();
		/**
		* @brief ���ö���ID
		* @param[in]  id ����ID
		*/
		void SetId(glbInt32 id);
		/**
		* @brief ��ȡ�����������ID
		* @return �����������ID
		*/
		glbInt32 GetGroupId();
		/**
		* @brief ���ö����������ID
		* @param[in]  id �����������ID
		*/
		void SetGroupId(glbInt32 id);
		/**
		* @brief ��ȡ��������
		* @return ��������
		*/
		CGlbGlobe *GetGlobe();
		/**
		* @brief ���ó�������
		* @param[in]  globe ��������
		*/
		virtual void SetGlobe(CGlbGlobe *globe);
		/**
		* @brief ��ȡ���󼤻ʽ
		* @return ���󼤻ʽö��
		*/
		GlbGlobeActiveActionEnum GetActiveAction();
		/**
		* @brief ���ö��󼤻ʽ
		* @param[in]  action ���ʽö��
		*/
		void SetActiveAction(GlbGlobeActiveActionEnum action);	
		/**
		* @brief �����л����󣬽���xml��ȡ��������ֵ
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return �����л��ɹ�����true����֮����false
		*/
		virtual glbBool Load(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief �����л����󣬽���xml��ȡ��������ֵ
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return �����л��ɹ�����true����֮����false
		*/
		virtual glbBool Load2(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief ���л��������л��������Ե�xml�����ļ�
		* @param[in]  os �����ļ������
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return ���л��ɹ�����true����֮����false
		*/
		virtual glbBool Save(xmlNodePtr node,const glbWChar* prjPath);
		/**
		* @brief ��ȡ������÷����Ĵ�����Ϣ
		* @return ������Ϣ
		* @note ���÷��������ٷ��ص�ָ��
		*/
		glbWChar *GetLastError();
	protected:
		glbInt32 mpt_id;///<����Id
		glbInt32 mpt_grpId;///����������Id
		CGlbWString mpt_name;///<��������
		CGlbGlobe *mpt_globe;///<��������
		GlbGlobeActiveActionEnum mpt_action;///<�����
	};
}