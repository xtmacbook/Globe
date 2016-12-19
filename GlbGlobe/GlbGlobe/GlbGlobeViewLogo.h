/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeViewLogo.h
* @brief   视图相关元素LOGO
*
* @version 1.0
* @author  敖建
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
	* @brief 视图相关元素LOGO类
	* @version 1.0
	* @author  敖建
	* @date    2014-6-9 10:43
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewLogo : public osg::Camera
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeViewLogo(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeViewLogo(void);
		/**
		* @brief 设置LOGO位置
		* @param [in] width : 屏幕宽度
		*		  [in] height : 屏幕高度
		*/
		void SetPosition(int left,int top,int width,int height);
		/**
		* @brief 设置LOGO纹理
		* @param [in] imageData : 图片全路径
		*/
		void SetImageLogo(CGlbWString path);
	private:
		/**
		* @brief 创建LOGO节点
		*/
		osg::ref_ptr<osg::Geode> createViewLogo();
	private:
		osg::ref_ptr<osg::Geode> mpr_geode;
	};
}
