/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeMarker2DShapeSymbol.h
* @brief   2D��״����ͷ�ļ����ĵ�����CGlbGlobeMarker2DShapeSymbol��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-14 13:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
#include "GlbGlobeCustomShapeDrawable.h"

#include "StdAfx.h"
//AGG
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"
#include "agg_vcgen_markers_term.h"
#include "agg_renderer_scanline.h"
#include "agg_conv_contour.h"

#include "GlbLine.h"
#include "geos.h"

namespace GlbGlobe
{
	/**
	* @brief 2D��״������				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-14 13:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarker2DShapeSymbol :
		public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeMarker2DShapeSymbol(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeMarker2DShapeSymbol(void);
		/**
		* @brief ������Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] obj ��Ⱦ����
		* @param[in] geom Ҫ��Ⱦ�ļ�����
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		void DrawToImage(CGlbGlobeRObject *obj,glbByte *image/*RGBA*/,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext,
			IGlbGeometry* outline,IGlbGeometry* noRotOutline);

		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ������λ
		* @return  �����2D����
		*/
		IGlbGeometry *GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);

		IGlbGeometry *GetNoRotOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);
		//��ɹ��еģ���object��������extent
		IGlbGeometry *DealOutline(osg::Vec2d centerPosition,
			GlbMarker2DShapeSymbolInfo *markerInfo,CGlbFeature *feature,GlbGlobeTypeEnum globeType);
	private:
		void DealDraw(osg::Shape *shape,
			osg::ShapeDrawable *shapeFillDrawable,
			osg::Node *outlineNode,
			GlbMarker2DShapeSymbolInfo *marker2DInfo,
			CGlbFeature *feature);
		void DealAggRender(agg::rendering_buffer rbuf,renb_type &renb,CGlbGlobeRObject *obj,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext,IGlbGeometry* outline);

		void FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline);
		void DealAggTexture(agg::rasterizer_scanline_aa<> &ras,agg::rendering_buffer rbuf,
			IGlbGeometry *geom,GlbMarker2DShapeSymbolInfo *markerInfo,
			CGlbFeature *feature,glbBool &isRenderTex,IGlbGeometry *outline);

		IGlbGeometry *DealNoRotOutline(osg::Vec2d centerPosition,double rotation,
			GlbMarker2DShapeSymbolInfo *markerInfo,CGlbFeature *feature,GlbGlobeTypeEnum globeType);

		void DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,
			agg::rendering_buffer rbuf,IGlbGeometry *geom,GlbMarker2DShapeSymbolInfo *markerInfo,
			CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV,IGlbGeometry *outline);
		void DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,
			agg::rendering_buffer rbuf,GlbMarker2DShapeSymbolInfo *markerInfo,
			CGlbFeature *feature,glbBool &isRenderTex,IGlbGeometry *outline);

		osg::ref_ptr<osg::Vec2dArray> GetPointFromGeom(IGlbGeometry *geom); 
		IGlbGeometry *DealOutline3DVertexes(osg::Vec3d centerPosition,
			GlbMarker2DShapeSymbolInfo *markerInfo,CGlbFeature *feature,GlbGlobeTypeEnum globeType);

		geos::geom::Geometry* Interection(CGlbLine *outline, CGlbExtent &ext);
		void FillDrawVertexArray(agg::path_storage &ps,geos::geom::Polygon* poly);		

		void DealAggTexture(agg::rasterizer_scanline_aa<> &ras,agg::rendering_buffer rbuf,
			IGlbGeometry *geom,GlbMarker2DShapeSymbolInfo *markerInfo,
			CGlbFeature *feature,glbBool &isRenderTex,geos::geom::Polygon* poly,IGlbGeometry *outline);		
		void DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,
			agg::rendering_buffer rbuf,GlbMarker2DShapeSymbolInfo *markerInfo,
			CGlbFeature *feature,glbBool &isRenderTex,geos::geom::Polygon* poly,IGlbGeometry *outline);
		void DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,
			agg::rendering_buffer rbuf,IGlbGeometry *geom,GlbMarker2DShapeSymbolInfo *markerInfo,
			CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV,geos::geom::Polygon* poly,IGlbGeometry *outline);
	private:
		CGlbExtent tempExtent;
		glbInt32 tempImageW;
		glbInt32 tempImageH;
		glbref_ptr<CGlbExtent> cglbExtent;
		glbref_ptr<CGlbExtent> cglbOrginExtent;
		GlbGlobeTypeEnum mpr_globeType;
		osg::Vec3d mpr_centerPos;
		glbref_ptr<IGlbGeometry> mpr_noRotOutline;
	};
}