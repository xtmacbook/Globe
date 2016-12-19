/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeMarkerSmokeSymbol.h
* @brief   �������ͷ�ļ����ĵ�����CGlbGlobeMarkerSmokeSymbol��
* @version 1.0
* @author  malin
* @date    2015-12-17 13:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief ���������				 
	* @version 1.0
	* @author  malin
	* @date   2015-12-17 13:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerSmokeSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeMarkerSmokeSymbol(void);
		~CGlbGlobeMarkerSmokeSymbol(void);
		/**
		* @brief ������Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);	
	private:
		osg::ref_ptr<osg::Group> CreateMySmokeParticle(osg::Vec3 pos,osg::Vec3 startColor, osg::Vec3 endColor, float scale, float intensity);
	};
}


