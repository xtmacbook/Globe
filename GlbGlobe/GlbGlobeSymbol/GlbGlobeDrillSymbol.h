/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeDrillSymbol.h
* @brief   ��׷���ͷ�ļ����ĵ�����CGlbGlobeDrillSymbol��
* @version 1.0
* @author  ����
* @date    2014-11-19 10:40
*********************************************************************/
#pragma once

#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeDrillSymbol��׷�����				 
	* @version 1.0
	* @author  ����
	* @date    2014-11-19 10:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeDrillSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeDrillSymbol(void);

		/**
		* @brief ��������
		*/
		~CGlbGlobeDrillSymbol(void);

		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	private:
		// ����Բ���εĵز�(�ز���)
		osg::Node* buildStratumCylinder(IGlbGeometry * _ln, bool isGlobe, double cylinderRadius,osg::Vec3d originPos);
		// �������ϵ������յ�pt0,pt1,�����γɵ�Բ����ͷ���
		void computeCylinderVertexs(osg::Vec3d pt0, osg::Vec3d pt1, double radius, osg::Vec3dArray* vts, osg::Vec3Array* nors);
	};
}

