/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobePolygonBuildingSymbol.h
* @brief   ����η���ͷ�ļ����ĵ�����CGlbGlobePolygonBuildingSymbol��
* @version 1.0
* @author  ��ͥ��
* @date    2014-6-18 15:30
*********************************************************************/
#pragma once


#include "GlbGlobeSymbolExport.h"

#include "GlbGlobeSymbol.h"


namespace GlbGlobe
{
	/**
	* @brief ����ν���������
	* @version 1.0
	* @author  ��ͥ��
	* @date    2014-6-18 15:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobePolygonBuildingSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobePolygonBuildingSymbol(void);
		~CGlbGlobePolygonBuildingSymbol(void);

		virtual osg::Node * Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo );

		virtual void DrawToImage( CGlbGlobeRObject *obj, IGlbGeometry* geom,glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext );

		virtual IGlbGeometry * GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	};

}
