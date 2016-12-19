/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeFeatureLayer.h
* @brief   Ҫ��ͼ��ͷ�ļ�
*
* �����������CGlbGlobeDomLayer���class
*
* @version 1.0
* @author  ����
* @date    2014-5-8 17:40
*********************************************************************/

#pragma once
#include "GlbGlobeExport.h"
#include "GlbGlobeLayer.h"
#include "GlbQueryFilter.h"
#include "GlbGlobeRenderer.h"
#include "GlbFeature.h"
#include "GlbTin.h"
#include <osg\Polytope>
#include <osg\BoundingBox>
#include "libxml/tree.h"
#include "GlbClipObject.h"

namespace GlbGlobe
{
	/**
	* @brief Ҫ��ͼ����
	* @note ��������Ⱦ�������ݼ����磺�� ���ߣ���......��
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeFeatureLayer : public CGlbGlobeLayer
	{ 
	public:
		/**
		* @brief Ҫ��ͼ�㹹�캯��
		* @return ��
		*/
		CGlbGlobeFeatureLayer(void);
		/**
		* @brief Ҫ��ͼ����������
		* @return ��
		*/
		~CGlbGlobeFeatureLayer(void);

		/* ����ӿ� */
		/**
		* @brief ��ȡͼ������
		* @return ͼ������ 
		*/
		const glbWChar* GetName();
		/**
		* @brief ����ͼ������
		* @param name ͼ������
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetName(glbWChar* name);
		/**
		* @brief ��ȡͼ������
		* @return  ͼ������
		*/
		GlbGlobeLayerTypeEnum GetType();
		/**
		* @brief ͼ�����л������أ�
		* @param [in] node xml��ȡ�ڵ�
		* @param [in] relativepath ���·��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool   Load(xmlNodePtr node, glbWChar* relativepath );
		glbBool   Load2(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief ͼ�����л������棩
		* @param [in] node xmlд��ڵ�
		* @param [in] relativepath ���·��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool   Save(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief ����ͼ����ʾ������
		* @param [in] show ����  ��ʾΪtrue������Ϊfalse
		* @param [in] isOnState true��ʾ�ڿɼ�����  false��ʾ���ڿɼ�����
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool Show(glbBool show,glbBool isOnState=false);
		/**
		* @brief ����ͼ����˸
		* @param [in] isBlink �Ƿ���˸,true��˸
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool Blink(glbBool isBlink);
		glbBool IsBlink();
		void SetClipObject(CGlbClipObject *clipObject);
		glbBool Clip(glbBool isClip);
		glbBool IsClip();
		glbBool SetGlobe(CGlbGlobe* globe);
		/**
		* @brief ����ͼ�㲻͸����
		* @param [in] opacity ��͸����(0-100)
		-	0��ʾȫ͸��
		-	100��ʾ��͸��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetOpacity(glbInt32 opacity);
		/**
		* @brief ��ȡͼ�㷶Χ
		* @param [in] isWorld �Ƿ����������꣬������������꣬����Ϊ��λ;��������������꣬��ʹ�õ������꣬�Զ�Ϊ��λ				
		* @return  ͼ�㷶Χ
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld);

		/* Ҫ��ͼ��ӿ� */
		/**
		* @brief ͼ�����
		*				 
		*/
		void Update();
		/*
		* @brief ����ͼ���ж����Ϊ�����϶��������¶���
		* @param [in] isground �����϶��������¶���
		-	true��ʾΪ�����϶���
		-	false��ʾΪ�����¶���
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetGround(glbBool isground);
		/**
		* @brief ��ȡͼ���ж����ǵ����϶����ǵ����¶���
		* 				
		* @return  �����϶��������¶���
		-	true��ʾΪ�����϶���
		-	false��ʾΪ�����¶���
		*/
		glbBool IsGround();
		/**
		* @brief ����ͼ������ݹ�����
		* @param [in] filter ������
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetFilter(CGlbQueryFilter* filter);
		/**
		* @brief ��ȡͼ������ݹ�����
		* 				
		* @return  ���ݹ�����
		*/
		CGlbQueryFilter* GetFilter();
		/**
		* @brief ���õ�ǰ����Ŀɼ�����
		* @param [in] extent1 �ɼ�����1
		* @param [in] extent2 �ɼ�����2
		* @note �ɼ����������Ϊ����������Сͼ�����ݼ��Ĳ�ѯ���
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetVisibleTerrainExtent(CGlbExtent* extent1, CGlbExtent* extent2);
		/**
		* @brief ֪ͨͼ�����µ�Ҫ�ض��������
		* @param [in] feature �µ�Ҫ�ض���				
		* @return  ��
		*/
		void NotifyFeatureAdd(CGlbFeature* feature); 
		/**
		* @brief ֪ͨͼ����Ҫ�ض���ɾ����
		* @param [in] feature_oid ɾ����Ҫ�ض���id				
		* @return  ��
		*/
		void NotifyFeatureDelete(glbInt32 feature_oid);
		/**
		* @brief ֪ͨͼ����Ҫ�ض����޸���
		* @param [in] feature_oid �޸ĵ�Ҫ�ض���id				
		* @return  ��
		*/
		void NotifyFeatureModity(glbInt32 feature_oid);
		/**
		* @brief ���һ��ͼ����Ⱦ��
		* @param [in] renderer ��Ⱦ��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool AddRenderer(CGlbGlobeRenderer* renderer); 
		/**
		* @brief ɾ��һ��ͼ����Ⱦ��
		* @param [in] idx ��Ⱦ�����
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool RemoveRenderer(glbInt32 idx); 
		/**
		* @brief ɾ������ͼ����Ⱦ��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool RemoveAllRenderers();
		/**
		* @brief ��ȡͼ����Ⱦ����
		* 			
		* @return  ��Ⱦ������
		*/
		glbInt32 GetRendererCount(); 
		/**
		* @brief ��ȡһ��ͼ����Ⱦ��
		* @param [in] idx ��Ⱦ�����
		* @return  �ɹ�������Ⱦ��ָ��
		-	 ʧ�ܷ���NULL
		*/
		CGlbGlobeRenderer* GetRenderer(glbInt32 idx); 
		/**
		* @brief ����ͼ������Ƿ�����ж��
		* @param [in] isAllowUnload �Ƿ�����ж�� true-����ж��  false-����ж��
		* @return ��
		*/
		void SetIsAllowUnLoad(glbBool isAllowUnload=false);
		/**
		* @brief ��ȡͼ������Ƿ�����ж��		
		* @return ��������ж�� true-����ж��  false-����ж��
		*/
		glbBool IsAllowUnLoad();
		/**
		* @brief  ����Ҫ��ѡ��״̬		
		* @param  [in] ftOID Ҫ�ض����ID
		* @param  [in] isSelected ѡ��״̬,true ѡ��,false ��ѡ��
		* @return void
		*/
		void    SetFeatureSelected(glbInt32 ftOID,glbBool isSelected);
		glbBool IsFeatureSelected(glbInt32 ftOID);
		/**
		* @brief  ����Ҫ����˸״̬		
		* @param  [in] ftOID Ҫ�ض����ID
		* @param  [in] isBlink ��˸״̬,true ��˸,false ����˸
		* @return void
		*/
		void    SetFeatureBlink(glbInt32 ftOID,glbBool isBlink);
		glbBool IsFeatureBlink(glbInt32 ftOID);
		/**
		* @brief  ����Ҫ������״̬		
		* @param  [in] ftOID  Ҫ�ض����ID
		* @param  [in] isShow ����״̬,true ��ʾ,false ����ʾ
		* @return void
		*/
		void    SetFeatureShow(glbInt32 ftOID,glbBool isShow);
		glbBool IsFeatureShow(glbInt32 ftOID);

		/**
		* @brief �����Ƿ�ʹ��������θ���Ϊ���������ľ���	 -- �Ƶ�Renderer��
		* @param [in] isCameraAltitudeAsDistance true-ʹ�������������Ϊ
		* @return void
		*/
		//void SetCameraAltitudeAsDistance(glbBool isCameraAltitudeAsDistance);
		//glbBool IsCameraAltitudeAsDistance();

		/**
		* @brief  ����ͼ���Ƿ���Ҫ��ײ���		
		* @param  [in] isCollisionDetect  true��ʾ��Ҫ������ײ��⣬false��ʾ����Ҫ��ײ���		
		* @return void
		*/
		void	SetCollisionDetect(glbBool isCollisionDetect);
		glbBool IsCollisionDectect();
	private:
		glbBool IsEqual(CGlbExtent* extent, CGlbExtent* extent_other);
		glbInt32 ComputeFitLoopdepth(GlbGlobeTypeEnum globe_type, osg::Camera* p_osgcamera, CGlbExtent* p_layer_extent);
		osg::BoundingBoxd ComputeVisibleExtent(osg::Polytope cv, CGlbExtent* extent, GlbGlobeTypeEnum globe_type, glbInt32 loopdepth);
		CGlbTin* ExtentToTin(CGlbExtent* p_extent);

		bool	BuildQueryfilterPostfixClause(CGlbQueryFilter* queryFilter);
	private:
		glbref_ptr<IGlbFeatureCursor> mpr_p_cursor;							//Ҫ���α�
		std::map<glbInt32/*feature oid*/,glbInt32/*feature oid*/> mpr_features;		//װ�ص�Ҫ��
		//std::map<glbInt32/*feature oid*/,glbInt32/*feature oid*/> mpr_tempfoids;	//�ɼ���Ҫ��oid����
		std::map<glbInt32/*feature oid*/,glbByte/*state*/> mpr_ftstate;//Ҫ��״̬
		glbBool	mpr_isground;										//�Ƿ�������					
		glbref_ptr<CGlbQueryFilter> mpr_query_filter;				//���ݹ�����
		glbref_ptr<CGlbExtent> mpr_visible_extent;					//
		glbref_ptr<CGlbExtent> mpr_p_visible_extent_onterrain;		//������ɼ����򡾵������꡿
		std::vector<glbref_ptr<CGlbGlobeRenderer>> mpr_renderers;	//��Ⱦ������
		glbref_ptr<CGlbQueryFilter> mpr_temp_query_filter;

		GlbCriticalSection	mpr_critical;							//�޸Ĳ��� ���� �ٽ���
		GlbCriticalSection	mpr_queryfilter_critical;				//���ݹ����� �ٽ���
		glbBool				mpr_isBlink;							//ͼ����˸
		glbBool				mpr_isClip;							//ͼ��ü�
		glbBool				mpr_isAllowUnLoad;						//ͼ��Ҫ��װ�غ��Ƿ��ж��
		glbBool				mpr_isCameraAltitudeAsDistance;			//�Ƿ�ʹ��������θ߶���Ϊ���������ľ���

		glbBool				mpr_isCollisionDetected;				//�Ƿ���Ҫ��ײ���
		glbInt32			mpr_featureCnt;							//���ݼ��еĶ�������
		glbBool				mpr_isEOF;								//��־�Ƿ����������ݼ��е���������
		glbref_ptr<CGlbClipObject>   mpr_clipObject;//����ü����󣬷���������ü�����

	};
}


