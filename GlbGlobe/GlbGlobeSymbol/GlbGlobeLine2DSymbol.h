/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeLine2DSymbol.h
* @brief   2d�߷���ͷ�ļ����ĵ�����CGlbGlobeLine2DSymbol��
* @version 1.0
* @author  ����
* @date    2014-8-12 11:13
*********************************************************************/
#pragma once
#include "StdAfx.h"
#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief 2d�߷�����				 
	* @version 1.0
	* @author  ����
	* @date    2014-8-12 11:13
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeLine2DSymbol
	{
	public:
		/**
		* @ brief ���캯��
		*/
		CGlbGlobeLine2DSymbol(void);
		/**
		* @ brief ��������
		*/
		~CGlbGlobeLine2DSymbol(void);
		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	};
}

