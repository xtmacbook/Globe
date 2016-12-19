/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeViewCentermark.h
* @brief   ��ͼ���Ԫ�����ı��
*
* @version 1.0
* @author  ����
* @date    2014-7-16 11:43
*********************************************************************/

#pragma once
#include "GlbGlobeView.h"
#include <osg/Camera>
#include <osgViewer/View>
namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief ��ͼ���Ԫ�����ı����
	* @version 1.0
	* @author  ����
	* @date    2014-7-16 11:43
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewCentermark : public osg::Camera
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeViewCentermark(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief ��������
		*/
		~CGlbGlobeViewCentermark(void);
		/**
		* @brief �������ı��λ��
		* @param [in] width : ��Ļ���
		*		  [in] height : ��Ļ�߶�
		*/
		void SetPosition(int left,int top,int width,int height);
	};
}

