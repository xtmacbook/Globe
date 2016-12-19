/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeViewSymbol.h
* @brief   ���������������ͷ�ļ����ĵ�����CGlbGlobeViewSymbol��
* @version 1.0
* @author  ��ͥ��
* @date    2014-9-22 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeSymbolExport.h"

#include "GlbGlobeSymbol.h"

#include "GlbPoint.h"

namespace GlbGlobe
{
	/**
	* @brief �����������������
	* @version 1.0
	* @author  ��ͥ��
	* @date    2014-9-22 15:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeViewSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeViewSymbol(void);
		~CGlbGlobeViewSymbol(void);

		virtual osg::Node * Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo );

		virtual void DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext );

		virtual IGlbGeometry * GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo );

	};

}
