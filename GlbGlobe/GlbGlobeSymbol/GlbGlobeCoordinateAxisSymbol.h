/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeCoordinateAxisSymbol.h
* @brief   ���������ͷ�ļ����ĵ�����CGlbGlobeCoordinateAxisSymbol��
* @version 1.0
* @author  ����
* @date    2016-7-6 15:40
*********************************************************************/
#pragma once

#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeCoordinateAxisSymbol �����������				 
	* @version 1.0
	* @author  ����
	* @date    2016-7-6 15:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeCoordinateAxisSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeCoordinateAxisSymbol(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeCoordinateAxisSymbol(void);

		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::Node* buildCoordinateAxis(osg::Vec3d axislen, osg::Vec3d axistep, osg::Vec3d axiscale, bool isInvertZ,double labelSize=40.0);
		osg::Node* buildCoordinateAxisGrid(osg::Vec3d axislen, osg::Vec3d axistep, osg::Vec3d axiscale, bool isInvertZ);
		osg::Node* buildCoordinateAxisLabel(osg::Vec3d orig,osg::Vec3d axislen, osg::Vec3d axistep, osg::Vec3d axiscale, bool isInvertZ, double labelSize=40.0);

	private:
		GlbGlobeTypeEnum mpr_globeType;
	};

}