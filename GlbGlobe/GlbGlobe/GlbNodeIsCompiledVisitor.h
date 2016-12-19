/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbNodeIsCompiledVisitor.h
* @brief   �жϽڵ���Geometry��Texture�Ƿ��Ѿ�������ͷ�ļ����ĵ�����CGlbNodeIsCompiledVisitor��
* @version 1.0
* @author  Malin
* @date    2015-3-19 15:00
*********************************************************************/
#pragma once

#include <osg/NodeVisitor>
#include <osg/Geode>

namespace GlbGlobe
{
	/**
	* @brief  �жϽڵ���Geometry��Texture�Ƿ��Ѿ�������
	* @version 1.0
	* @author  Malin
	* @date    2015-3-19 15:00
	*/
	class GLB_DLLCLASS_EXPORT CGlbNodeIsCompiledVisitor : public osg::NodeVisitor
	{
	public:
		CGlbNodeIsCompiledVisitor(void);
		~CGlbNodeIsCompiledVisitor(void);
		/**
		* @brief ʵ����ӿ�
		* @return  ��
		*/
		virtual void apply(osg::Geode &node);		
		/**
		* @brief �����Ƿ��Ѿ�����	
		* @return  true��ʾ�Ѿ����룬
		false��ʾδ����
		*/
		glbBool IsCompiled();
	private:
		glbBool mpr_isCompiled;
	};
}