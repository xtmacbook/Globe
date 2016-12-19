/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeMarkerFireSymbol.h
* @brief   �����ͷ�ļ����ĵ�����CGlbGlobeMarkerFireSymbol��
* @version 1.0
* @author  malin
* @date    2015-12-17 13:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief �������				 
	* @version 1.0
	* @author  malin
	* @date   2015-12-17 13:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerFireSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeMarkerFireSymbol(void);
		~CGlbGlobeMarkerFireSymbol(void);

		/**
		* @brief ������Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);		
	};
}

