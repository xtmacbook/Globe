/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobePolygonSymbol.h
* @brief   ����η���ͷ�ļ����ĵ�����CGlbGlobePolygonSymbol��
* @version 1.0
* @author  ��ͥ��
* @date    2014-6-18 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeSymbolExport.h"

#include "GlbGlobeSymbol.h"

#include "GlbPolygon.h"

#include "StdAfx.h"

#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"
#include "agg_vcgen_markers_term.h"
#include "agg_renderer_scanline.h"
#include "agg_conv_contour.h"

#include "geos.h"

namespace osg{class Geometry;}

namespace GlbGlobe
{
	/**
	* @brief ����η�����
	* @version 1.0
	* @author  ��ͥ��
	* @date    2014-6-18 15:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobePolygonSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobePolygonSymbol(void);
		~CGlbGlobePolygonSymbol(void);

		virtual osg::Node *Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo);

		virtual void DrawToImage(CGlbGlobeRObject *obj, IGlbGeometry* geom, glbByte *image/*RGBA*/,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext);

		virtual IGlbGeometry * GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		void setPolygon2DSymbolInfo(osg::StateSet *sset,GlbPolygon2DSymbolInfo *polygon2DSmblInfo);
		osg::Node *buildDrawable(CGlbGlobeRObject *obj,CGlbPolygon *geo );
		osg::Node *buildDrawable(CGlbGlobeRObject *obj,CGlbMultiPolygon *geo );
		// ʹ�þ�γ�����������������
		osg::Vec2Array *buildTexCoords(osg::Vec3Array *points,double &sizeX,double &sizeY,double &centX,double &centY)const;
		osg::Vec2Array *buildTexCoordsEx(osg::Vec3Array *points,float rotAnge_rad,double &sizeX,double &sizeY,double &centX,double &centY)const;
		osg::Vec2dArray *buildTexCoordsEx(osg::Vec3dArray *points,float rotAnge_rad,double &sizeX,double &sizeY,double &centX,double &centY)const;

		//////////////////////////////////////////////////////////////////////////
		void FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline);
		void DealAggRender( renb_type &renb,CGlbGlobeRObject *obj,IGlbGeometry *geom,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);

		void DealAggTexture(agg::rasterizer_scanline_aa<> &ras,
			renb_type &renb, IGlbGeometry *geom, GlbPolygon2DSymbolInfo *polyInfo,
			CGlbFeature *feature,glbBool &isRenderTex);

		void DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,
			renb_type &renb, IGlbGeometry *geom, GlbPolygon2DSymbolInfo *polyInfo,
			CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV);
		void DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,
			renb_type &renb,IGlbGeometry *geom,GlbPolygon2DSymbolInfo *polyInfo,
			CGlbFeature *feature,glbBool &isRenderTex);

		// �������rect����
		geos::geom::Geometry* Interection(CGlbPolygon *geom, CGlbExtent &ext);
		void FillDrawVertexArray(agg::path_storage &ps,geos::geom::Polygon* poly=NULL);
		
		void DealAggTexture(agg::rasterizer_scanline_aa<> &ras,
			renb_type &renb, CGlbPolygon *geom, geos::geom::Polygon* poly, GlbPolygon2DSymbolInfo *polyInfo,
			CGlbFeature *feature,glbBool &isRenderTex);
		void DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,
			renb_type &renb,IGlbGeometry *geom, geos::geom::Polygon* poly, GlbPolygon2DSymbolInfo *polyInfo,
			CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV);
		void DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,
			renb_type &renb,IGlbGeometry *geom,geos::geom::Polygon* poly,GlbPolygon2DSymbolInfo *polyInfo,
			CGlbFeature *feature,glbBool &isRenderTex);
	private:
		CGlbExtent tempExtent;
		glbInt32 tempImageW;
		glbInt32 tempImageH;
		glbref_ptr<CGlbExtent> cglbExtent;
	};

}
