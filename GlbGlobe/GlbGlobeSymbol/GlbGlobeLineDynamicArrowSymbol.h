/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeLineDynamicArrowSymbol.h
* @brief   ��̬��ͷ�߷���ͷ�ļ����ĵ�����CGlbGlobeLineDynamicArrowSymbol��
* @version 1.0
* @author  ChenPeng
* @date    2016-6-20 11:40
*********************************************************************/

#pragma once
#include "GlbGlobeSymbol.h"
#include <osg/AnimationPath>
namespace GlbGlobe
{
	/**
	* @brief ��̬��ͷ�߷�����				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2016-6-20 11:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeLineDynamicArrowSymbol : public CGlbGlobeSymbol
	{
	private:
		struct PathMark
		{
			int lineIndedx;
			osg::Vec3d startPoint;
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeLineDynamicArrowSymbol(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeLineDynamicArrowSymbol(void);
		/**
		* @brief ��̬��ͷ�߷�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] obj ��Ⱦ����
		* @param[in] geom Ҫ��Ⱦ�ļ�����
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		IGlbGeometry *GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::AnimationPathCallback *CreateAnimationPathCallback(osg::Vec3dArray *points, glbDouble startTime,glbDouble endTime,glbBool isModel = false);
		osg::Geode *Draw3DArrow(glbDouble radius,glbDouble length,osg::Vec4f color);
	};
}