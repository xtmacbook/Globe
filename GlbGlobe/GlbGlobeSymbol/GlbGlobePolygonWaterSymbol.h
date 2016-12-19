/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobePolygonWaterSymbol.h
* @brief   �����ˮ�����ͷ�ļ����ĵ�����CGlbGlobePolygonWaterSymbol��
* @version 1.0
* @author  ��ͥ��
* @date    2014-6-18 15:30
*********************************************************************/
#pragma once

//  2016/10/11 ��ע�� ˮЧ���� CGlbGlobePolygonLakeSymbol��ʵ�� ,�������   ����

#include "GlbGlobeSymbolExport.h"

#include "GlbGlobeSymbol.h"

namespace GlbGlobe
{
	class IWaterScene
	{
	public:
		virtual void enableReflections( bool enable )=0;
		virtual void enableRefractions( bool enable )=0;
	};

	/**
	* @brief �����ˮ�������
	* @version 1.0
	* @author  ��ͥ��
	* @date    2014-6-18 15:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobePolygonWaterSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobePolygonWaterSymbol(void);
		~CGlbGlobePolygonWaterSymbol(void);

		virtual osg::Node * Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo );

		virtual void DrawToImage( CGlbGlobeRObject *obj, IGlbGeometry* geom,glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext );

		virtual IGlbGeometry * GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::Node *buildPolygonWaterSurfaceDrawable(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	};

}
