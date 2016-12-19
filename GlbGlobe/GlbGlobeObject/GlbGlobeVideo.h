/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeVideo.h
* @brief   ͶӰ��Ƶ����ͷ�ļ����ĵ�����CGlbGlobeVideo��
* @version 1.0
* @author  ����
* @date    2014-10-8 10:20
*********************************************************************/
#pragma once
#include "GlbGlobeObjectExport.h"
#include "GlbGlobeReobject.h"
#include "CGlbGlobe.h"
#include "GlbPolygon.h"
#include <osg/ImageStream>

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeVideoͶӰ������				 
	* @version 1.0
	* @author  ����
	* @date    2014-10-8 10:20
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeVideo : public CGlbGlobeREObject
	{
	public:
		class CGlbGlobeVideoTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeVideoTask(CGlbGlobeVideo *obj,glbInt32 level);
			~CGlbGlobeVideoTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobeVideo> mpr_obj;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeVideo(void);
		/**
		* @brief ��������
		**/
		~CGlbGlobeVideo(void);
		/**
		* @brief ��ȡ��������
		* @return ����ö������
		*/
		GlbGlobeObjectTypeEnum GetType();
		/**
		* @brief �����л����󣬽���xml��ȡ��������ֵ
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return �����л��ɹ�����true����֮����false
		*/
		glbBool Load(xmlNodePtr *node,const glbWChar* prjPath);
		glbBool Load2(xmlNodePtr *node,const glbWChar* prjPath);
		/**
		* @brief ���л��������л��������Ե�xml�����ļ�
		* @param[in]  os �����ļ������
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return ���л��ɹ�����true����֮����false
		*/
		glbBool Save(xmlNodePtr node,const glbWChar* prjPath);
		/**
		* @brief ��ȡ��������ľ���
		* @param[in]  cameraPos ����ڳ����е�λ��
		* @param[in]  isCompute �����Ƿ���Ҫ���¼��㣬���¼�������true����֮����false
		* @return ��������ľ���
		*/
		glbDouble GetDistance(osg::Vec3d &cameraPos,glbBool isCompute);
		/**
		* @brief װ�����ݣ���������Ƶ���ĵ��ø÷���
		* @param[in]  level ��Ҫװ�����ݵĵȼ�
		*/
		void LoadData(glbInt32 level);
		/**
		* @brief ��osg�ڵ���߽���osg�ڵ㵽������
		*/
		void AddToScene();
		/**
		* @brief �ӳ�����ժ��osg�ڵ�
		* @param[in]  isClean true��ʾͬʱɾ������osg�ڵ��Խ�ʡ�ڴ�
		* @return ���ؽ�ʡ���ڴ��С
		*/
		glbInt32 RemoveFromScene(glbBool isClean);
		/**
		* @brief ��ȡ����ڵ��Χ��
		* @param[in]  isWorld true��ʾ��ȡ���������Χ��(��λ����)
		*  -                                    false��ʾ��ȡ���������Χ�У���λ���ȣ�
		* @return �ڵ��Χ��
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief ��Ե��λ���ʱ�����¶����Ӧ��ĵ��θ߶�
		*/
		void UpdateElevate();
		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ��������λ
		* @return  �����2D����
		*/
		IGlbGeometry *GetOutLine();
		/**
		* @brief ���ö������Ⱦ��Ϣ
		* @param[in]  renderInfo �������Ⱦ��Ϣ
		* @return ���óɹ�����true����֮����false
		*/
		glbBool SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw);
		/**
		* @brief ��ȡ�������Ⱦ��Ϣ
		* @return �������Ⱦ��Ϣ
		*/
		GlbRenderInfo *GetRenderInfo();
		/**
		* @brief ���ö�������
		* @param[in]  isShow ������ʾ����Ϊtrue����������Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		glbBool SetShow(glbBool isShow,glbBool isOnState = false);
		/**
		* @brief ���ö����ѡ��״̬
		* @param[in]  isSelected ����ѡ������Ϊtrue����֮����Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		glbBool SetSelected(glbBool isSelected);
		/**
		* @brief ���ö����Ƿ���˸
		* @param[in]  isBlink ��˸����Ϊtrue������˸����Ϊfalse
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		glbBool SetBlink(glbBool isBlink);
		/**
		* @brief ����������Ⱦ
		* @param[in]  level ������Ⱦ�ȼ�
		*/
		void DirectDraw(glbInt32 level);
		/**
		* @brief ���ö��󺣰�ģʽ
		* @param[in]  mode ����ģʽö��
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode);
		/**
		* @brief ���ö�����Ⱦ���ȼ�
		* @param[in]  order ������Ⱦ���ȼ�
		*/
		void SetRenderOrder(glbInt32 order);

		//////////////////////////////////////////////////////////////////////////
		glbBool SetGeo(CGlbPolygon *polygon,glbBool &isNeedDirectDraw);
		CGlbPolygon *GetGeo() const{return mpr_polygonGeo.get();}
		/**
		* @brief ���ö��󺣰�ģʽ
		* @param[in]  mode ����ģʽö��
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);

		glbBool SetEdit(glbBool isEdit);

		virtual glbInt32 GetOsgNodeSize();
	private:
		/**
		* @brief ��ȡ����
		*/
		void ReadData(glbInt32 level,glbBool isDirect = false);

		void ComputePosByAltitudeAndGloleType(osg::Vec3d &position);
		void DealModePosByChangeAltitudeOrChangePos();
		glbBool GetFeaturePoint( glbInt32 idx,glbDouble *ptx,glbDouble *pty,glbDouble *ptz,glbDouble *ptm );
		glbInt32 ComputeNodeSize(osg::Node *node);
		void DirtyOnTerrainObject();
		void DealSelected();
		void DealBlink();

		CGlbPolygon *RemoveRepeatePoints(CGlbPolygon *polygon);

		glbBool DealBillboardSymbolChange(GlbVideoSymbolInfo *videoInfo,glbBool &isNeedDirectDraw);
		glbBool DealTerrainSymbolChange(GlbVideoSymbolInfo *videoInfo,glbBool &isNeedDirectDraw);
	private:
		glbDouble					mpr_distance;		///<���������ľ���
		glbref_ptr<GlbRenderInfo>	mpr_renderInfo;		///<������Ⱦ��Ϣ
		glbref_ptr<IGlbGeometry>	mpr_outline;		///<�����2D����
		glbref_ptr<CGlbPolygon>    mpr_polygonGeo;   ///<����Ķ��������
		glbDouble					mpr_elevation;		///<��ǰ���θ�
		glbBool						mpr_isAltitudeChanged;
		glbBool						mpr_isUpdataFrame;	///<֡�����Ƿ�仯

		GlbCriticalSection			mpr_readData_critical;		///<�����ڵ��ٽ���
		GlbCriticalSection			mpr_addToScene_critical;	///<�ڵ����ӳ����ٽ���
		//CRITICAL_SECTION			mpr_createNode_critical;	///<ReadData�����ڵ��ٽ���
		glbBool						mpr_needReReadData;			///<���¶�ȡ���ݱ�־
		glbInt32					mpr_objSize;				///<obj��size
		glbBool						mpr_isCanDrawImage;
		osg::ref_ptr<osg::ImageStream> mpr_videoPlayer;

		glbBool mpr_isNeedRecoveState;
	};
}