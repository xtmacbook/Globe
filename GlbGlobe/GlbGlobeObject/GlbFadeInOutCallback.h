/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    CGlbFadeInOutCallback.h
* @brief   ���󽥽�����ͷ�ļ����ĵ�����CGlbFadeInOutCallback��
* @version 1.0
* @author  ChenPeng
* @date    2016-3-4 9:00
*********************************************************************/
#pragma once

#include <osg/Geode>
#include <osg/Geometry>

namespace GlbGlobe
{
	class CGlbFadeInOutCallback : public osg::NodeCallback
	{
	public:
		CGlbFadeInOutCallback(glbFloat startAlpha,glbFloat endAlpha,
			CGlbGlobeRObject *rObject,glbDouble durationTime,osg::Vec4 fadeColor);
		~CGlbFadeInOutCallback(void);

		virtual void operator()(osg::Node *node,osg::NodeVisitor *nv);
		private:
			void changeMaterialAlpha(osg::StateSet* ss);
			//�ж϶����Ƿ�����ά����Ч��
			glbBool isThreeDimensionalObject();
	private: 
		glbFloat mpr_startAlpha;
		glbFloat mpr_endAlpha;
		glbFloat mpr_currentAlpha;
		double mpr_simulationTime;//��¼�ϴ��޸�alpha��ʱ��
		double mpr_durationTime;//�޸�alpha��ʱ������
		glbBool mpr_isFadeIn;//��ʶ�ǽ������ǽ���
		glbref_ptr<CGlbGlobeRObject> mpr_rObject;
		osg::Vec4 mpr_fadeColor;
		glbBool mpr_isThreeDimensional;
	};
}