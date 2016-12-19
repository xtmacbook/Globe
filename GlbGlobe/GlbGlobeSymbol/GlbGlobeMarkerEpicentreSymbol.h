/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeMarkerEpicentreSymbol.h
* @brief   ��Դ�����ͷ�ļ����ĵ�����CGlbGlobeMarkerEpicentreSymbol��
* @version 1.0
* @author  malin
* @date    2015-12-30 9:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief ��Դ�������				 
	* @version 1.0
	* @author  malin
	* @date   2015-12-30 9:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerEpicentreSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeMarkerEpicentreSymbol(void);
		~CGlbGlobeMarkerEpicentreSymbol(void);

		/**
		* @brief ������Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);	
	private:
		osg::Node* createEpicentreNode(glbDouble radius, glbInt32 clr);
	};
}


