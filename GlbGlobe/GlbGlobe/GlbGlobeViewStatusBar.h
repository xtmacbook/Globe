/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeViewStatusBar.h
* @brief   ��ͼ���Ԫ��״̬��
*
* @version 1.0
* @author  ����
* @date    2014-5-26 16:00
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
	* @brief ��ͼ���Ԫ��״̬����
	* @version 1.0
	* @author  ����
	* @date    2014-5-26 16:01
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewStatusBar : public osg::Camera
	{
		/**
		* @brief ˽���࣬״̬���ص�
		* @version 1.0
		* @author  ����
		* @date    2014-5-27 10:01
		*/
		class CGlbGlobeViewStatusBarUpdateCallback : public osg::NodeCallback  
		{
		public:
			/**
			* @brief ���캯��
			*/
			CGlbGlobeViewStatusBarUpdateCallback()
			{
				mpr_p_glbView = NULL;
				mpr_p_globe = NULL;
			}
			/**
			* @brief ��������
			*/
			~CGlbGlobeViewStatusBarUpdateCallback(){}
			/**
			* @brief �ص�����
			*/
			void operator()(osg::Node* node, osg::NodeVisitor* nv);
			/**
			* @brief ������ͼ
			*/
			void Put_GlbGlobeView(CGlbGlobeView* pView);
			/**
			* @brief ���ó���
			*/
			void Put_GlbGlobe(CGlbGlobe* pGlobe);
		private:
			/*glbref_ptr<CGlbGlobeView>*/CGlbGlobeView*	mpr_p_glbView;	//��ͼ
			/*glbref_ptr<CGlbGlobe>*/	 CGlbGlobe*	    mpr_p_globe;		//����

			osg::Vec3d mpr_cameraPos;
			osg::Vec3d mpr_center;
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeViewStatusBar(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief ��������
		*/
		~CGlbGlobeViewStatusBar(void);
		/**
		* @brief ����״̬��λ��
		* @param [in] width : ��Ļ���
		*		  [in] height : ��Ļ�߶�
		*/
		void SetPosition(int left,int top,int width,int height);
	private:
		/**
		* @brief ����״̬���ڵ�
		*/
		osg::ref_ptr<osg::Geode> createViewStatusBar();
	};
};