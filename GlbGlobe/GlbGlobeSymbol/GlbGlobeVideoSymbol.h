/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeVideoSymbol.h
* @brief   ͶӰ����ͷ�ļ����ĵ�����CGlbGlobeVideoSymbol��
* @version 1.0
* @author  ����
* @date    2014-10-10 14:55
*********************************************************************/
#pragma once
#include "StdAfx.h"
#include "GlbPolygon.h"
#include "glbglobesymbol.h"
#include "GlbGlobeAutoTransform.h"

namespace GlbGlobe
{
	/**
	* @brief ͶӰ������				 
	* @version 1.0
	* @author  ����
	* @date    2014-10-10 14:55
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeVideoSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @ brief ���캯��
		*/
		CGlbGlobeVideoSymbol(void);
		/**
		* @ brief ��������
		*/
		~CGlbGlobeVideoSymbol(void);
		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] obj ��Ⱦ����
		* @param[in] geom ��Ⱦ������
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		void DrawToImage(CGlbGlobeRObject *obj,IGlbGeometry* geom,	glbByte *image/*RGBA*/,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ������λ
		* @return  �����2D����
		*/
		IGlbGeometry *GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	private:
		/**
		* @brief ��ƵͶӰ��ƽ��ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *DrawBillBoardVideo(CGlbGlobeRObject *obj,CGlbPolygon *polygon);
		/**
		* @brief ��ƵͶӰ������ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *DrawTerrainVideo(CGlbGlobeRObject *obj,CGlbPolygon *polygon);

		osg::Camera *initOverLayData(osg::Node *overLayNode,CGlbGlobe *globe);

		glbBool createShaders(osg::StateSet *ss,osg::Camera *camera,glbInt32 videoIndex);
	};
}

