/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeLinePixelSymbol.h
* @brief   �����߷���ͷ�ļ����ĵ�����CGlbGlobeLinePixelSymbol��
* @version 1.0
* @author  ChenPeng
* @date    2014-6-4 10:30
*********************************************************************/
#pragma once
#include "StdAfx.h"
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

#include "geos.h"
namespace GlbGlobe
{
	/**
	* @brief �����߷�����				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-6-4 10:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeLinePixelSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeLinePixelSymbol(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeLinePixelSymbol(void);
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

		static osg::Node *DrawBorderline(CGlbGlobeRObject *obj,IGlbGeometry *geo,GlbRenderInfo *renderInfo);

		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ������λ
		* @return  �����2D����
		*/
		IGlbGeometry *GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	private:
		void DealAggRender(renb_type &renb,CGlbGlobeRObject *obj,IGlbGeometry* geom, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		void FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline);
		bool ReCalPoint(agg::path_storage &ps,osg::Vec2d& point, osg::Vec2d& nextpoint, CGlbExtent& ext);

		bool Intersect(osg::Vec2d v0,osg::Vec2d v1,osg::Vec2d v2,osg::Vec2d v3,osg::Vec2d& inter);
		double Cross(osg::Vec2d v0,osg::Vec2d v1);

		double mult(osg::Vec2d a, osg::Vec2d b, osg::Vec2d c);
		bool intersect(osg::Vec2d aa, osg::Vec2d bb, osg::Vec2d cc, osg::Vec2d dd);  

		geos::geom::Geometry* Interection(IGlbGeometry *geom, CGlbExtent &ext);
		void FillDrawVertexArray(agg::path_storage &ps,geos::geom::Geometry* outline);
	private:
		CGlbExtent tempExtent;
		glbInt32 tempImageW;
		glbInt32 tempImageH;
	};
}