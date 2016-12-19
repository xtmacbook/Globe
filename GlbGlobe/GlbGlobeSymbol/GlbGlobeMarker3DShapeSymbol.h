/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeMarker3DShapesSymbol.h
* @brief   3D��״����ͷ�ļ����ĵ�����CGlbGlobeMarker3DShapeSymbol��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-20 11:15
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
#include "osg/ShapeDrawable"

namespace GlbGlobe
{
	/**
	* @brief 3D��״������				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-20 11:15
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarker3DShapeSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeMarker3DShapeSymbol(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeMarker3DShapeSymbol(void);
		/**
		* @brief ������Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		void DealDraw(osg::Shape *shape,
			osg::ShapeDrawable *shapeFillDrawable,
			osg::ShapeDrawable *shapeOutDrawable,
			GlbMarker3DShapeSymbolInfo *marker3DInfo,
			CGlbFeature *feature);
	};
}