/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeNetworkSymbol.h
* @brief   �������ͷ�ļ����ĵ�����CGlbGlobeNetworkSymbol��
* @version 1.0
* @author  ����
* @date    2016-3-1 17:40
*********************************************************************/
#pragma once

#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeNetworkSymbol���������				 
	* @version 1.0
	* @author  ����
	* @date    2016-3-1 17:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeNetworkSymbol  : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeNetworkSymbol(void);
		~CGlbGlobeNetworkSymbol(void);

		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	};
}

