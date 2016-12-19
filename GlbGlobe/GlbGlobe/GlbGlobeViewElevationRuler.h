/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeViewElevationRuler.h
* @brief   ��ͼ���Ԫ�ص����̱��
*
* @version 1.0
* @author  ����
* @date    2014-6-6 16:40
*********************************************************************/
#pragma once
#include "GlbGlobeView.h"
#include <osg/Camera>
#include <osgViewer/View>
namespace GlbGlobe
{
	/** 
	*ö�� �������λ��ö������
	*/
	enum GlbGlobeElevationRulerPositionType
	{
		ELEVATIONRULER_NONE_BTN		= 0,		//����λ��
		ELEVATIONRULER_CURSOR_BTN	= 1,		//�α� 
		ELEVATIONRULER_UP_BTN		= 2,		//����
		ELEVATIONRULER_DOWN_BTN		= 3,		//����
		ELEVATIONRULER_EARTH_BTN	= 4,		//ȫ��
		ELEVATIONRULER_COUNTRY_BTN	= 5,		//����
		ELEVATIONRULER_PROVINCE_BTN	= 6,		//ʡ
		ELEVATIONRULER_CITY_BTN		= 7,		//����
		ELEVATIONRULER_STREET_BTN	= 8,		//�ֵ�
		ELEVATIONRULER_HOUSE_BTN	= 9,		//����
		ELEVATIONRULER_UG_BTN		= 10,		//����
		ELEVATIONRULER_OUT_BTN		= 11		//��������
	};
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief ��ͼ���Ԫ�ص��������
	* @version 1.0
	* @author  ����
	* @date    2014-6-6 16:46
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewElevationRuler : public osg::Camera
	{
		/**
		* @brief ˽���࣬��������¼��ص�
		* @version 1.0
		* @author  ����
		* @date    2014-6-9 11:17
		*/
		class CGlbGlobeViewElevationRulerEventHandler : public osgGA::GUIEventHandler
		{
		public:
			/**
			* @brief ���캯��
			*/
			CGlbGlobeViewElevationRulerEventHandler(CGlbGlobeView* pview,CGlbGlobe* pglobe,osg::Geode* pgeode)
			{
				mpr_p_view = pview;
				mpr_p_globe = pglobe;
				_ga_t0 = NULL;
				_ga_t1 = NULL;
				_geode = pgeode;
			}
			/**
			* @brief ��������
			*/
			~CGlbGlobeViewElevationRulerEventHandler(){}
			/**
			* @brief �¼��ص�
			*/
			glbBool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
		private:
			/**
			* @brief ��ȡ����ڵ������ϵ�ö��λ��
			* @param [in] ea ����¼�
			* @return GlbGlobeNavigatorPositionType ����ڵ������ϵ�λ�õ�ö������
			*/
			GlbGlobeElevationRulerPositionType getElevationRulerPositionType(const osgGA::GUIEventAdapter& ea);
			/**
			* @brief ��ȡ����ڵ������ϵ�ö��λ��
			* @param [in] ea ����¼�
			* @return GlbGlobeNavigatorPositionType ����ڵ������ϵ�λ�õ�ö������
			*/
			GlbGlobeElevationRulerPositionType getTouchElevationRulerPositionType(const osgGA::GUIEventAdapter& ea);
			/**
			* @brief ���е�distan_target�߶ȸ���
			*/
			glbBool FlyTo(glbDouble distan_target); 
			/**
			* @brief �����λ��ת��Ϊ�α���
			*/
			glbDouble DeltToDistance(glbDouble delt);
			/**
			* @brief ���α���ת��Ϊ���λ��
			*/
			glbDouble DistanceToDelt(glbDouble distan);
			/**
			* @brief �����¼�����
			* @return ����true ��view���ػ�
			*		-	����false �������ػ�
			*/
			glbBool handleTouch( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
		private:
			CGlbGlobeView* mpr_p_view;
			CGlbGlobe* mpr_p_globe;
			osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t1;	//��ǰ��ǰһ������¼�(��Ҫ��������갴�µ��Ǹ��¼�)
			osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t0;	//��ǰ����¼�
			osg::ref_ptr<osg::Geode> _geode;
		};

		/**
		* @brief ˽���࣬������߻ص�
		* @version 1.0
		* @author  ����
		* @date    2014-6-9 14:36
		*/
		class CGlbGlobeViewElevationRulerUpdateCallback : public osg::NodeCallback  
		{
		public:
			/**
			* @brief ���캯��
			*/
			CGlbGlobeViewElevationRulerUpdateCallback(CGlbGlobeView* pview,CGlbGlobe* pglobe)
			{
				mpr_p_glbView = pview;
				mpr_p_globe = pglobe;
			}
			/**
			* @brief ��������
			*/
			~CGlbGlobeViewElevationRulerUpdateCallback(){}
			/**
			* @brief �ص�����
			*/
			void operator()(osg::Node* node, osg::NodeVisitor* nv);
		private:
			/*glbref_ptr<CGlbGlobeView>*/CGlbGlobeView*	mpr_p_glbView;		//��ͼ
			/*glbref_ptr<CGlbGlobe>*/	 CGlbGlobe*	    mpr_p_globe;		//����
			/**
			* @brief ���α���ת��Ϊ���λ��
			*/
			glbDouble DistanceToDelt(glbDouble distan);
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeViewElevationRuler(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief ��������
		*/
		~CGlbGlobeViewElevationRuler(void);
		/**
		* @brief ���õ����̱��λ��
		* @param [in] width : ��Ļ���
		*		  [in] height : ��Ļ�߶�
		*/
		void SetPosition(int left,int top,int width,int height);
	private:
		/**
		* @brief ���������̱�߱�߲��ֽڵ�
		*/
		osg::ref_ptr<osg::Geode> createViewElevationRuler();
		/**
		* @brief ���������̱�����ֲ��ֽڵ�
		*/
		osg::ref_ptr<osg::Geode> createViewElevationRulerText();
	};
}

