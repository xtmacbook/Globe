/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeInstanceManager.h
* @brief   ʵ�������������ͷ�ļ�
*
* �����������CGlbGlobeInstanceManager���class
*
* @version 1.0
* @author  ����
* @date    2014-7-4 10:10
*********************************************************************/
#pragma once

#include "GlbReference.h"
#include "GlbGlobeObject.h"
#include "GlbWString.h"
#include <osg\Node>
namespace GlbGlobe
{
	/**
	* @brief ʵ����ʵ����
	*/
	class CGlbGlobeInstance : public CGlbReference
	{
	public:
		CGlbGlobeInstance();
		~CGlbGlobeInstance();

		glbBool addInstance(glbWChar* key,osg::Node *node);
		osg::Node *findInstance(glbWChar* key);	
		void clearInstance();
	private:
		std::map<CGlbWString,osg::ref_ptr<osg::Node>> mpr_instances;
		CRITICAL_SECTION mpr_critical;
	};

	class GLB_DLLCLASS_EXPORT CGlbGlobeInstanceManager
	{
	public:
		/**
		* @brief ʵ����������������캯��
		* @return ��
		*/
		CGlbGlobeInstanceManager(void);
		/**
		* @brief ʵ���������������������
		* @return ��
		*/
		~CGlbGlobeInstanceManager(void);

		/**
		* @brief ���һ��ʵ��
		* @param key ����ֵ
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		static glbBool AddInstance(glbWChar* key,osg::Node *node);

		/**
		* @brief ����ʵ����������ֵ����ʵ������
		* @param key ����ֵ
		* @return ʵ���������ɵĽڵ�osg::Node*
		*/
		static osg::Node *FindInstance(glbWChar* key);	

		/**
		* @brief ���� ʵ��������������е�ʵ��
		* @note �������������ڴ治��ʱ���á�
		* @return ��
		*/
		static void ClearInstance();
	};

}


