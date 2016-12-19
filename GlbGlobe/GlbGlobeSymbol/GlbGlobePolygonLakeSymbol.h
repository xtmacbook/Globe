/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobePolygonLakeSymbol.h
* @brief   ����ˮ����ͷ�ļ����ĵ�����GlbGlobePolygonLakeSymbol��
* @version 1.0
* @author  ChenPeng
* @date    2016-8-15 15:20
*********************************************************************/
#pragma once

#include "GlbGlobeSymbolExport.h"
#include "GlbGlobeSymbol.h"
#include "GlbPolygon.h"
#include "StdAfx.h"

namespace GlbGlobe
{
	/**
	* @brief ����ˮ������
	* @version 1.0
	* @author  ChenPeng
	* @date    2016-8-15 15:20
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobePolygonLakeSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobePolygonLakeSymbol(void);
		~CGlbGlobePolygonLakeSymbol(void);

		virtual osg::Node *Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo);
		virtual IGlbGeometry * GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::Node *buildDrawable(CGlbGlobeRObject *obj,CGlbPolygon *geo);
		osg::Vec2dArray *buildTexCoords(osg::Vec3dArray *points);
	};
}