/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbCalculateBoundBoxVisitor.h
* @brief   �����ͷ�ļ����ĵ�����CGlbCalculateBoundBoxVisitor��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-14 15:00
*********************************************************************/
#pragma once
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/BoundingBox>
#include <osg/BoundingSphere>
#include <osg/MatrixTransform>

namespace GlbGlobe
{
	/**
	* @brief  ����node�ڵ㼰�������нڵ�Ŀռ䷶Χboundbox��
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-14 15:00
	*/
	class CGlbCalculateBoundBoxVisitor : public osg::NodeVisitor
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbCalculateBoundBoxVisitor(void);
		/**
		* @brief ��������
		*/
		~CGlbCalculateBoundBoxVisitor(void);

		/**
		* @brief ���ʽڵ����
		*/
		virtual void apply(osg::Geode &node)
		{
			osg::BoundingBox bb;
			for (unsigned int i = 0; i < node.getNumDrawables(); ++i)
			{			
				bb.expandBy(node.getDrawable(i)->getBound());
			}

			if (bb.valid())
			{
				// transform corners by current matrix
				osg::BoundingBoxd xbb;
				for (unsigned int i = 0; i < 8; ++i)
				{
					osg::Vec3d xv = bb.corner(i) * mpr_TransformMatrix;
					xbb.expandBy(xv);
				}
				mpr_BoundingBox.expandBy(xbb);
			}

			traverse(node);
		}

		/**
		* @brief ���ʽڵ����
		*/
		virtual void apply(osg::Transform& node)
		{
			osg::Matrix m;
			node.computeLocalToWorldMatrix(m,NULL);
			osg::Matrix currentMatrix = mpr_TransformMatrix;
			mpr_TransformMatrix.preMult(m);
			traverse(node);
			//�ָ�����
			mpr_TransformMatrix = currentMatrix;

		}

		/**
		* @brief ��ȡ����ڵ��Χ��
		* @return �ڵ��Χ��
		*/
		osg::BoundingBoxd &getBoundBox(){  return mpr_BoundingBox; }

	private:
		osg::BoundingBoxd mpr_BoundingBox;	// bound box
		osg::Matrix mpr_TransformMatrix;	// current transform matrix
	};
}