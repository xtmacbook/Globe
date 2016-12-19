/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeLineArrowSymbol.h
* @brief   ��ͷ�߷���ͷ�ļ����ĵ�����CGlbGlobeLineArrowSymbol��
* @version 1.0
* @author  ����
* @date    2014-8-7 11:13
*********************************************************************/

#pragma once
#include "StdAfx.h"
#include "glbglobesymbol.h"
#include "GlbGlobeCustomShapeDrawable.h"
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
	* @brief ��ͷ�߷�����				 
	* @version 1.0
	* @author  ����
	* @date    2014-8-7 11:13
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeLineArrowSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @ brief ���캯��
		*/
		CGlbGlobeLineArrowSymbol(void);
		/**
		* @ brief ��������
		*/
		virtual ~CGlbGlobeLineArrowSymbol(void);
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
		IGlbGeometry *DealOutline(osg::Vec3d centerPosition,
			GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,GlbGlobeTypeEnum globeType);
		void DealDraw(osg::Shape *shape,osg::ShapeDrawable *shapeFillDrawable,osg::ShapeDrawable *shapeOutDrawable,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature);
		void DealAggRender(renb_type &renb,CGlbGlobeRObject *obj,IGlbGeometry* geom,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		void FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline);
		void DealAggTexture(agg::rasterizer_scanline_aa<> &ras,	renb_type &renb,IGlbGeometry* geom,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,glbBool &isRenderTex);

		IGlbGeometry *DealNoRotOutline(osg::Vec3d centerPosition,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,GlbGlobeTypeEnum globeType);

		void DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,renb_type &renb,IGlbGeometry* geom,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV);
		void DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,renb_type &renb,IGlbGeometry* geom,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,glbBool &isRenderTex);

	private:
		CGlbExtent tempExtent;
		glbInt32 tempImageW;
		glbInt32 tempImageH;
	};
}