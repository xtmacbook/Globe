/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    CGlbGlobeMarkerPixelSymbol.h
* @brief   ���ص����ͷ�ļ����ĵ�����CGlbGlobeMarkerPixelSymbol��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-8 13:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief ���ص������				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-8 13:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerPixelSymbol :public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeMarkerPixelSymbol(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeMarkerPixelSymbol(void);
		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	};
}