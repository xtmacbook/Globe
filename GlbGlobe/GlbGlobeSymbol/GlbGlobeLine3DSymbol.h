/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeLine3DSymbol.h
* @brief   3d�߷���ͷ�ļ����ĵ�����CGlbGlobeLine3DSymbol��
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
	* @brief 3d�߷�����				 
	* @version 1.0
	* @author  ����
	* @date    2014-8-7 11:13
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeLine3DSymbol
	{
	public:
		/**
		* @ brief ���캯��
		*/
		CGlbGlobeLine3DSymbol(void);
		/**
		* @ brief ��������
		*/
		~CGlbGlobeLine3DSymbol(void);
		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	};
}
