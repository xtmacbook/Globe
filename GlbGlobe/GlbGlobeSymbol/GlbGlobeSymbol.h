/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeSymbol.h
* @brief   ���Ż���ͷ�ļ����ĵ�����CGlbGlobeSymbol��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-8 11:30
*********************************************************************/
#pragma once
#include "osg/Node"
#include "GlbGlobeSymbolExport.h"
#include "GlbGlobeRObject.h"

namespace GlbGlobe
{
	/**
	* @brief ���Ż���				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-8 11:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeSymbol : public CGlbReference
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeSymbol(void);
		/**
		* @brief ��������
		*/
		virtual ~CGlbGlobeSymbol(void);
		/**
		* @brief ������Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] obj ��Ⱦ����
		* @param[in] geom Ҫ��Ⱦ�ļ�����
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		virtual void DrawToImage(CGlbGlobeRObject *obj,IGlbGeometry* geom,
			glbByte *image/*RGBA*/,glbInt32 imageW,
			glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ������λ
		* @return  �����2D����
		*/
		virtual IGlbGeometry *GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	protected:
		glbref_ptr<IGlbGeometry> mpt_outline;///<�����λ��ƶ���Ķ�ά������ ��λ������λ
	};
}