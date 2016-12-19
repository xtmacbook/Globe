/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeViewLogo.h
* @brief   ��ͼ���Ԫ��LOGO
*
* @version 1.0
* @author  ����
* @date    2014-6-9 10:29
*********************************************************************/
#pragma once
#include "GlbGlobeView.h"
#include "GlbWString.h"
#include <osg/Camera>
#include <osgViewer/View>
namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief ��ͼ���Ԫ��LOGO��
	* @version 1.0
	* @author  ����
	* @date    2014-6-9 10:43
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewLogo : public osg::Camera
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeViewLogo(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief ��������
		*/
		~CGlbGlobeViewLogo(void);
		/**
		* @brief ����LOGOλ��
		* @param [in] width : ��Ļ���
		*		  [in] height : ��Ļ�߶�
		*/
		void SetPosition(int left,int top,int width,int height);
		/**
		* @brief ����LOGO����
		* @param [in] imageData : ͼƬȫ·��
		*/
		void SetImageLogo(CGlbWString path);
	private:
		/**
		* @brief ����LOGO�ڵ�
		*/
		osg::ref_ptr<osg::Geode> createViewLogo();
	private:
		osg::ref_ptr<osg::Geode> mpr_geode;
	};
}
