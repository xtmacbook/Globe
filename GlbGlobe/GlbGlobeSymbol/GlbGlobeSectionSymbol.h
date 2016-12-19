/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeSectionSymbol.h
* @brief   �������ͷ�ļ����ĵ�����CGlbGlobeSectionSymbol��
* @version 1.0
* @author  ����
* @date    2014-11-20 15:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
#include "GlbPolygon.h"
#include "GlbTin.h"
namespace GlbGlobe
{
	/**
	* @brief ���������				 
	* @version 1.0
	* @author  ����
	* @date    2014-11-20 15:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeSectionSymbol : public CGlbGlobeSymbol
	{
	public:
		enum GlbProjPlaneEnum
		{// ͶӰ��ö�ٱ���  x��(����) y��(γ��) z��(���θ߶�)
			GLB_PLANE_UNKNOWN	= 0,	// δ֪ͶӰƽ��
			GLB_PLANE_XY		= 1,	// XYƽ��
			GLB_PLANE_XZ		= 2,	// XZƽ��
			GLB_PLANE_YZ		= 3		// YZƽ��		
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeSectionSymbol(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeSectionSymbol(void);
		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	private:
		osg::Vec2 CaculateTexCoord(glbDouble ptx,glbDouble pty,glbDouble ptz, CGlbExtent* ext,
						GlbGlobeTexRepeatModeEnum texRMode,glbInt32 tilingU	,glbInt32 tilingV);

		// �������ݿռ������жϺ��ʵ�ͶӰƽ��
		GlbProjPlaneEnum CaculateProjPlane(CGlbExtent* ext);

		// �������εķ���
		osg::Vec3 CaculatePolyNormal(CGlbMultiPolygon *mulPoly, GlbGlobeTypeEnum type);

		// ����Section�е�polygon
		osg::Node* drawPolyGeom(CGlbMultiTin* geo, CGlbGlobeRObject *obj, CGlbFeature* polyFeature);
	private:
		// ����ͶӰ��  x��(����) y��(γ��) z��(���θ߶�)
		GlbProjPlaneEnum mpr_prjPlaneEnum;		
	};
}

