/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeViewNavigator.h
* @brief   ��ͼ���Ԫ�ص�����
*
* @version 1.0
* @author  ����
* @date    2014-5-30 14:20
*********************************************************************/
#pragma once
#include "GlbGlobeView.h"
#include <osg/Camera>
#include <osgViewer/View>
namespace GlbGlobe
{
	/** 
	*ö�� ������λ��ö������
	*/
	enum GlbGlobeNavigatorPositionType
	{
		NAVIGATOR_NONE_BTN = 0,		//����λ��
		NAVIGATOR_LEFT_BTN = 1,		//����Move��
		NAVIGATOR_RIGHT_BTN = 2,	//����Move��
		NAVIGATOR_UP_BTN = 3,		//����Move��
		NAVIGATOR_DOWN_BTN = 4,		//����Move��
		NAVIGATOR_CENTER_BTN = 5,	//����������
		NAVIGATOR_YAW_BTN = 6,		//YawȦ
		NAVIGATOR_PITCH_BTN = 7,	//PitchȦ
		NAVIGATOR_OUT_BTN = 8		//��������
	};
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief ��ͼ���Ԫ�ص�������
	* @version 1.0
	* @author  ����
	* @date    2014-5-30 14:46
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewNavigator : public osg::Camera
	{
		/**
		* @brief ˽���࣬�������¼��ص�
		* @version 1.0
		* @author  ����
		* @date    2014-6-3 10:10
		*/
		class CGlbGlobeViewNavigatorEventHandler : public osgGA::GUIEventHandler
		{
		public:
			/**
			* @brief ���캯��
			*/
			CGlbGlobeViewNavigatorEventHandler(CGlbGlobeView* pview,osg::Geode* pgeode)
			{
				mpr_p_view = pview;
				_ga_t0 = NULL;
				_ga_t1 = NULL;
				moveflag = 0;				
				push_pitch = -90;
				_geode = pgeode;
			}
			/**
			* @brief ��������
			*/
			~CGlbGlobeViewNavigatorEventHandler(){}
			/**
			* @brief �¼��ص�
			*/
			glbBool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
			/**
			* @brief �����¼�����
			* @return ����true ��view���ػ�
			*		-	����false �������ػ�
			*/
			glbBool handleTouch( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
		private:
			/**
			* @brief ��ȡ����ڵ������ϵ�ö��λ��
			* @param [in] ea ����¼�
			* @return GlbGlobeNavigatorPositionType ����ڵ������ϵ�λ�õ�ö������
			*/
			GlbGlobeNavigatorPositionType getNavigatorPositionType(const osgGA::GUIEventAdapter& ea);
			/**
			* @brief ��ȡ����ڵ������ϵ�ö��λ��
			* @param [in] ea ����¼�
			* @return GlbGlobeNavigatorPositionType ����ڵ������ϵ�λ�õ�ö������
			*/
			GlbGlobeNavigatorPositionType getTouchNavigatorPositionType(const osgGA::GUIEventAdapter& ea);
		private:
			CGlbGlobeView* mpr_p_view;
			osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t1;	//��ǰ��ǰһ������¼�(��Ҫ��������갴�µ��Ǹ��¼�)
			osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t0;	//��ǰ����¼�
			glbInt8 moveflag;	//���״̬
			glbDouble push_pitch;
			osg::ref_ptr<osg::Geode> _geode;
		};
		/**
		* @brief ˽���࣬�����̻ص�
		* @version 1.0
		* @author  ����
		* @date    2014-5-30 14:21
		*/
		class CGlbGlobeViewNavigatorUpdateCallback : public osg::NodeCallback  
		{
		public:
			/**
			* @brief ���캯��
			*/
			CGlbGlobeViewNavigatorUpdateCallback(CGlbGlobeView* pview,CGlbGlobe* pglobe)
			{
				mpr_p_glbView = pview;
				mpr_p_globe = pglobe;
			}
			/**
			* @brief ��������
			*/
			~CGlbGlobeViewNavigatorUpdateCallback(){}
			/**
			* @brief �ص�����
			*/
			void operator()(osg::Node* node, osg::NodeVisitor* nv);
		private:
			/*glbref_ptr<CGlbGlobeView>*/CGlbGlobeView*	mpr_p_glbView;	//��ͼ
			/*glbref_ptr<CGlbGlobe>*/    CGlbGlobe*		mpr_p_globe;		//����
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeViewNavigator(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief ��������
		*/
		~CGlbGlobeViewNavigator(void);
		/**
		* @brief ���õ�����λ��
		* @param [in] width : ��Ļ���
		*		  [in] height : ��Ļ�߶�
		*/
		void SetPosition(int left,int top,int width,int height);
	private:
		/**
		* @brief ���������̽ڵ�
		*/
		osg::ref_ptr<osg::Geode> createViewNavigator();
	};
}