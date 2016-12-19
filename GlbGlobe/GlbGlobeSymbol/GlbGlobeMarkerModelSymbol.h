/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeMarkerModelSymbol.h
* @brief   ģ�ͷ���ͷ�ļ����ĵ�����CGlbGlobeMarkerModelSymbol��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-8 13:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief ģ�ͷ�����				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-8 13:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerModelSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeMarkerModelSymbol(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeMarkerModelSymbol(void);
		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo,glbInt32 level=0);
	};
}