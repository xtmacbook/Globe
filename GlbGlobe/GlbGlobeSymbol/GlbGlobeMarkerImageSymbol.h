/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeMarkerImageSymbol.h
* @brief   ͼ�����ͷ�ļ����ĵ�����GlbGlobeMarkerImageSymbol��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-20 11:15
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
#include "osg/Geometry"
#include "GlbGlobeAutoTransform.h"

namespace GlbGlobe
{
	/**
	* @brief ͼ�������				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-20 11:15
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerImageSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeMarkerImageSymbol(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeMarkerImageSymbol(void);
		/**
		* @brief ������Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::Geometry *CreateOutlineQuad(osg::Vec3 cornerVec,osg::Vec3 widthVec,osg::Vec3 heightVec);
	};
}