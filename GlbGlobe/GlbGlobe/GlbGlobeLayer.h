/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeLayer.h
* @brief   Globeͼ�����ͷ�ļ�
*
* �����������CGlbGlobeLayer���class,������Domͼ�㣬Demͼ���Ҫ��ͼ��Ļ���
*
* @version 1.0
* @author  ����
* @date    2014-5-6 15:40
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "GlbReference.h"
#include "GlbWString.h"
#include "GlbGlobeTypes.h"
#include "IGlbDataEngine.h"
#include <osg/Switch>
#include "libxml/tree.h"
namespace GlbGlobe
{
	class CGlbGlobe;
	class GLB_DLLCLASS_EXPORT CGlbGlobeLayer : public CGlbReference
	{
	public:
		/**
		* @brief ͼ�㹹�캯��
		*/
		CGlbGlobeLayer(void);

		/**
		* @brief ͼ����������
		*/
		virtual ~CGlbGlobeLayer(void);

		/**
		* @brief ��ȡͼ������
		* @note ���麯��
		* @return ͼ������ 
		*/
		virtual const glbWChar* GetName()=0;

		/**
		* @brief ����ͼ������
		* @param name ͼ������
		* @note ���麯��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		virtual glbBool SetName(glbWChar* name)=0;

		/**
		* @brief ��ȡͼ������
		* 
		* @note ���麯��
		* @return  ͼ������
		-	 GLB_GLOBELAYER_DOM		domͼ��
		-	 GLB_GLOBELAYER_DEM		demͼ��
		-	 GLB_GLOBELAYER_FEATURE	Ҫ��ͼ��
		*/
		virtual GlbGlobeLayerTypeEnum GetType()=0;

		/**
		* @brief ��ȡͼ��id
		* @return  ͼ��id
		*/
		glbInt32 GetId();
		/**
		* @brief ����ͼ��id
		* @param [in] id ͼ��Ψһid
		* 
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetId(glbInt32 id);

		/**
		* @brief ��ȡͼ�����������id
		* @return  ����id
		*/
		glbInt32 GetGroupId();
		/**
		* @brief ����ͼ����������id
		* @param [in] id ͼ����������id
		* 
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetGroupId(glbInt32 id);
		/**
		* @brief ����ͼ�㼤���
		* @param [in] action �����
		*				
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetActiveAction(GlbGlobeActiveActionEnum action);
		/**
		* @brief ��ȡͼ�㼤���
		* @return  �����
		*/
		GlbGlobeActiveActionEnum GetActiveAction();
		/**
		* @brief ͼ�����л������أ�
		* @param [in] node xml��ȡ�ڵ�
		* @param [in] relativepath ���·��
		* @note ���麯��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		virtual glbBool   Load(xmlNodePtr node, glbWChar* relativepath )=0;

		virtual glbBool   Load2(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief ͼ�����л������棩
		* @param [in] node xmlд��ڵ�
		* @param [in] relativepath ���·��
		* @note ���麯��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		virtual glbBool   Save(xmlNodePtr node, glbWChar* relativepath )=0;

		/**
		* @brief ���ó���
		* @param [in] globe ����
		*
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		virtual glbBool SetGlobe(CGlbGlobe* globe);

		/**
		* @brief ��ȡ��������
		* @return ��������
		*/
		CGlbGlobe *GetGlobe();

		/**
		* @brief ����ͼ����ʾ������
		* @param [in] show ����  ��ʾΪtrue������Ϊfalse 
		* @param [in] isOnState true��ʾ�ڿɼ�����  false��ʾ���ڿɼ�����
		* @note ���麯��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		virtual glbBool Show(glbBool show,glbBool isOnState=false)=0;
		/**
		* @brief ��ȡͼ���Ƿ���ʾ
		* @return  ��ʾ ����true
		-  ���� ����false
		*/
		glbBool IsShow();
		/**
		* @brief ����ͼ�㲻͸����
		* @param [in] opacity ��͸����(0-100)
		-	0��ʾȫ͸��
		-	100��ʾ��͸��
		* @note �麯��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		virtual glbBool SetOpacity(glbInt32 opacity)=0;
		/**
		* @brief ��ȡͼ�㲻͸����
		* @return  ͼ�㲻͸����
		*/
		glbInt32 GetOpacity();
		/**
		* @brief ����ͼ������ϵͳ
		* @param new_srs ����ϵͳ	  
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetSpatialRS(glbWChar* new_srs);
		/**
		* @brief ��ȡͼ������ϵͳ
		* @return  ͼ������ϵ
		*/
		const glbWChar* GetSpatialRS();

		/**
		* @brief ����ͼ�����ݼ�
		* @param dataset ���ݼ�	  
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetDataset(IGlbDataset* dataset);
		/**
		* @brief ��ȡͼ�����ݼ�
		* @return  ͼ�����ݼ�
		*/
		IGlbDataset* GetDataset();
		/**
		* @brief ��ȡͼ�㷶Χ
		* @param [in] isWorld �Ƿ����������꣬������������꣬����Ϊ��λ;��������������꣬��ʹ�õ������꣬�Զ�Ϊ��λ
		* @note ���麯��					
		* @return  ͼ�㷶Χ
		*/
		virtual glbref_ptr<CGlbExtent> GetBound(glbBool isWorld)=0;
		/**
		* @brief ����ͼ����Ⱦ˳��
		* @param order ��Ⱦ˳��
		* @note order��ֵԽ��,Խ����Ⱦ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetRenderOrder(glbInt32 order);
		/**
		* @brief ��ȡͼ����Ⱦ˳��
		* @return  ��Ⱦ˳��
		*/
		glbInt32 GetRenderOrder();
		/**
		* @brief ����ͼ�����˳��
		* @param order ����˳��
		* @note order��ֵԽ��,Խ�ȼ���
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetLoadOrder(glbInt32 order);
		/**
		* @brief ��ȡͼ�����˳��
		* @return  ����˳��
		*/
		glbInt32 GetLoadOrder();
		/**
		* @brief ����ͼ����У���תʱ��ƫ����
		* @param yaw ƫ����(��λ����)	 
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetActionYaw(glbDouble yaw);
		/**
		* @brief ��ȡͼ����У���תʱ��ƫ����
		* @return ƫ����(��λ����)
		*/
		glbDouble  GetActionYaw();
		/**
		* @brief ����ͼ����У���תʱ����б��
		* @param pitch ��б��(��λ����)	 
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetActionPitch(glbDouble pitch);
		/**
		* @brief ��ȡͼ����У���תʱ����б��
		* @return ��б��(��λ����)
		*/
		glbDouble  GetActionPitch();
		/**
		* @brief ����ͼ����У���תʱ����ת��
		* @param roll ��ת��(��λ����)	 
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetActionRoll(glbDouble roll);
		/**
		* @brief ��ȡͼ����У���תʱ����ת��
		* @return ��ת��(��λ����)
		*/
		glbDouble GetActionRoll();
		/**
		* @brief ����ͼ����У���תʱ�Ķ�λ��ľ��Ȼ�Xֵ
		* @param lonOrX ����(��λ����)��Xֵ
		-  ��ģʽ��Ϊ ����, ƽ��ģʽ��Ϊ X
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetActionLonOrX(glbDouble lonOrX);

		/**
		* @brief ��ȡͼ����У���תʱ�Ķ�λ��ľ��Ȼ�Xֵ
		* @return ���Ȼ�Xֵ
		*/
		glbDouble GetActionLonOrX();
		/**
		* @brief ����ͼ����У���תʱ�Ķ�λ���γ�Ȼ�Yֵ
		* @param latOrY γ��(��λ����)��Yֵ
		-  ��ģʽ��Ϊ γ��, ƽ��ģʽ��Ϊ Y
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetActionLatOrY(glbDouble latOrY);
		/**
		* @brief ��ȡͼ����У���תʱ�Ķ�λ���γ�Ȼ�Yֵ
		* @return γ�Ȼ�Yֵ
		*/
		glbDouble GetActionLatOrY();
		/**
		* @brief ����ͼ����У���תʱ�Ķ�λ��ĸ߶Ȼ�Zֵ
		* @param altOrZ �߶Ȼ�Z
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetActionAltOrZ(glbDouble altOrZ);
		/**
		* @brief ��ȡͼ����У���תʱ�Ķ�λ��ĸ߶Ȼ�Z
		* @return �߶Ȼ�Z
		*/
		glbDouble GetActionAltOrZ();
		/**
		* @brief ����ͼ����ʾ��Ĭ�Ͼ���
		* @param distance ����
		* @note ���������ͼ�����<=Ĭ�Ͼ���ʱ����ʾͼ��; >Ĭ�Ͼ���ʱ������ͼ��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetDefaultVisibleDistance(glbDouble distance);
		/**
		* @brief ��ȡͼ����ʾ��Ĭ�Ͼ���
		* @return ͼ����ʾ��Ĭ�Ͼ���
		*/
		glbDouble GetDefaultVisibleDistance();
		/**
		* @brief ��ȡͼ���Ӧ��osg�ڵ�
		* @note ���麯��
		* @return osg�ڵ�
		*/
		virtual osg::Node* GetOsgNode();
		/**
		* @brief ����ͼ�㸸osg�ڵ�
		* @note ���麯��
		* @return �ɹ�����true�� ʧ�ܷ���false
		*/
		virtual glbBool SetParentNode(osg::Node* parentnode);
		/**
		* @brief ��ȡͼ��ڵ�ĸ�osg�ڵ�
		* @note ���麯��
		* @return osg�ڵ�
		*/
		virtual osg::Node* GetParentNode();
		/**
		* @brief ��ȡͼ���Ƿ�"��"��
		* @note ���麯��
		* @return true��ʾ"��"����Ҫˢ��,false���ʾ"����"����Ҫˢ��				
		*/
		virtual glbBool IsDirty();
		/**
		* @brief �������־
		* @param [in][out] pixelblock ���ݿ�ָ��				
		* @return   �ɹ�����true��ʧ�ܷ���false.
		*/
		virtual glbBool SetDiry(glbBool dirty);

		void SetDestroy();

	protected:
		glbInt32	mpr_layer_id;						//ͼ��id
		CGlbWString mpr_layer_name;						//ͼ������
		glbInt32	mpr_layergroup_id;					//ͼ�����id
		GlbGlobeActiveActionEnum mpr_activeaction;		//ͼ�㼤�������
		GlbGlobeLayerTypeEnum	mpr_layer_type;			//ͼ������
		CGlbGlobe*	mpr_globe;							//ͼ����������
		glbBool		mpr_show;							//ͼ������
		glbBool		mpr_stateShow;						//���ȿ��Ƶ���ʾ����
		glbBool		mpr_isOnState;						//ͼ���Ƿ��ڿɼ���Χ
		glbInt32	mpr_opacity;						//��͸���ȡ�0-100��
		CGlbWString	mpr_spatialRS;						//����ϵ
		glbref_ptr<IGlbDataset> mpr_dataset;			//ͼ�����ݼ�
		glbInt32	mpr_renderorder;					//��Ⱦ˳��
		glbInt32	mpr_loadorder;						//����˳��
		glbDouble	mpr_actionyaw;						//ͼ�������תʱ��ƫ���ǡ���y����ת��
		glbDouble	mpr_actionpitch;					//ͼ�������תʱ����б�ǡ���x����ת��
		glbDouble	mpr_actionroll;						//ͼ�������תʱ����ת�ǡ���z����ת��
		glbDouble	mpr_actionLonOrX;					//ͼ�������תʱ���ӵ㾭�Ȼ�X
		glbDouble	mpr_actionLatOrY;					//ͼ�������תʱ���ӵ�γ�Ȼ�Y			
		glbDouble	mpr_actionAltOrZ;					//ͼ�������תʱ���ӵ�߶Ȼ�Z
		glbDouble	mpr_defaultvisibledist;				//ͼ��Ĭ�Ͽɼ�����
		osg::ref_ptr<osg::Node>	mpr_layer_node;		    //ͼ�����ɵ�osg�ڵ�
		osg::ref_ptr<osg::Node> mpr_parent_node;		//ͼ�㸸osg�ڵ�
		glbBool		mpr_dirty;							//���ࡱ��־
		glbBool     mpr_isDestory;           			//ͼ�㷶Χ
	};

}


