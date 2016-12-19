/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeDigHoleSymbol.h
* @brief   �ڶ�����ͷ�ļ����ĵ�����CGlbGlobeDigHoleSymbol��
* @version 1.0
* @author  ����
* @date    2014-8-29 16:55
*********************************************************************/
#pragma once
#include "stdafx.h"
#include "glbglobesymbol.h"
//AGG
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"
#include "agg_vcgen_markers_term.h"
#include "agg_renderer_scanline.h"
#include "agg_conv_contour.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeDigHoleSymbol�ڶ�������				 
	* @version 1.0
	* @author  ����
	* @date    2014-8-29 16:55
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeDigHoleSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @ brief ���캯��
		*/
		CGlbGlobeDigHoleSymbol(void);
		/**
		* @ brief ��������
		*/
		virtual ~CGlbGlobeDigHoleSymbol(void);
		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo,glbDouble depth,glbBool isShowWall);
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
		void DealAggRender(renb_type &renb,CGlbGlobeRObject *obj,IGlbGeometry* geom,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		void FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline);
	private:
		//�жϵ㣨px,py���Ƿ��ڶ����polygon�� ��polygon_pts ����ε㼯�� ptCnt ����ε����� Ŀǰֻ֧�ֵ������Ķ����
		bool PtInPolygon(double px, double py, double* polygon_pts, int ptCnt);
	private:
		CGlbExtent tempExtent;
		glbInt32 tempImageW;
		glbInt32 tempImageH;
		agg::renderer_base<agg::pixfmt_rgba32> dd;
	};
}