/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeRenderer.h
* @brief   Ҫ��ͼ����Ⱦ��ͷ�ļ�
*
* �����������CGlbGlobeRenderer���class
*
* @version 1.0
* @author  ����
* @date    2014-5-9 10:10
*********************************************************************/
#pragma once

#include "GlbReference.h"
#include "GlbGlobeObject.h"
#include "GlbWString.h"
#include "GlbGlobeObjectRenderInfos.h"
#include <osg\Node>

namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeFeatureLayer;
	/**
	* @brief Ҫ��ͼ����Ⱦ����				 
	* @version 1.0
	* @author  ����
	* @date    2014-5-9 10:10
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeRenderer : public CGlbReference
	{
		friend class CGlbGlobeFeatureLayer;
	public:
		glbUInt32 UnRef()
		{
			return CGlbReference::UnRef();
		}
		/**
		* @brief ��Ⱦ�����캯��
		* @return ��
		*/
		CGlbGlobeRenderer(void);
		/**
		* @brief ��Ⱦ����������
		* @return ��
		*/
		~CGlbGlobeRenderer(void);
		/**
		* @brief ��ȡ��Ⱦ������
		* @return  ��Ⱦ������
		*/
		const glbWChar* GetName();
		/**
		* @brief ������Ⱦ������
		* @param [in] name ��Ⱦ������
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetName(glbWChar* name);
		/**
		* @brief ���ü����ֶ�
		* @param [in] field �����ֶ���
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetGeoField(glbWChar* field);
		/**
		* @brief ��ȡ�����ֶ�����
		* @return �����ֶ�����
		*/
		const glbWChar* GetGeoField();
		/**
		* @brief ���ñ�ע�ֶ�
		* @param [in] field ��ע�ֶ���
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetLabelField(glbWChar* field);
		/**
		* @brief ��ȡ��ע�ֶ�����
		* @return ��ע�ֶ�����
		*/
		const glbWChar* GetLabelField();
		/**
		* @brief ������Ⱦ������Ϣ
		* @param [in] rinfo ��Ⱦ������Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetRenderInfo(GlbRenderInfo* rinfo);
		/**
		* @brief ��ȡ��Ⱦ������Ϣ
		* @return ��Ⱦ������Ϣ
		*/
		GlbRenderInfo* GetRenderInfo();
		/**
		* @brief ��ȡ��С�ɼ�������Ϣ
		* @return ��С�ɼ�������Ϣ
		*/
		GlbRenderDouble* GetMinDistance();
		/**
		* @brief ������С�ɼ�������Ϣ
		* @param [in] dis ��С�ɼ�������Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetMinDistance(GlbRenderDouble* dis);
		/**
		* @brief ��ȡ���ɼ�������Ϣ
		* @return ���ɼ�������Ϣ
		*/	
		GlbRenderDouble* GetMaxDistance();
		/**
		* @brief �������ɼ�������Ϣ
		* @param [in] dis ���ɼ�������Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetMaxDistance(GlbRenderDouble* dis);
		/**
		* @brief ��ȡ��Ⱦ˳��
		* @return ��Ⱦ˳��
		*/	
		GlbRenderInt32* GetRenderOrder();
		/**
		* @brief ������Ⱦ˳��
		* @param [in] order ��Ⱦ˳��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetRenderOrder(GlbRenderInt32* order);
		/**
		* @brief ��ȡ����˳��
		* @return ����˳��
		*/
		GlbRenderInt32* GetLoadOrder();
		/**
		* @brief ���ü���˳��
		* @param [in] order ����˳��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetLoadOrder(GlbRenderInt32* order);
		/**
		* @brief ��ȡ������Ϣ
		* @return ������Ϣ
		*/
		GlbRenderString* GetRMessage();
		/**
		* @brief ���õ�����Ϣ
		* @param [in] msg ������Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetRMessage(GlbRenderString* msg);
		/**
		* @brief ��ȡ��ʾ��Ϣ
		* @return ��ʾ��Ϣ
		*/
		GlbRenderString* GetToolTip();
		/**
		* @brief ������ʾ��Ϣ
		* @param [in] tips ��ʾ��Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetToolTip(GlbRenderString* tips);
		/**
		* @brief ���ø߳�ģʽ
		* @param [in] mode �߳�ģʽ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode);
		/**
		* @brief ��ȡ�߳�ģʽ
		* @return �߳�ģʽ
		*/
		GlbAltitudeModeEnum GetAltitudeMode();
		/**
		* @brief �����Ƿ��ǵ������
		* @param [in] isground �������Ϊtrue�����¶���Ϊfalse
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetGround(glbBool isground);
		/**
		* @brief ��ȡ�Ƿ��ǵ������
		* @return ������󷵻�true�����¶��󷵻�false
		*/
		glbBool IsGround();
		/**
		* @brief ������Ⱦ���ж�������
		* @param [in] isShow true��ʾ��ʾ��false��ʾ����
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetShow(glbBool isShow);
		/**
		* @brief ���л�(����)
		* @param [in] node xml������
		* @param [in] relativepath ���·��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool Load(xmlNodePtr node,glbWChar* relativepath );
		glbBool Load2(xmlNodePtr node,glbWChar* relativepath );
		/**
		* @brief ���л�(����)
		* @param [in] node xml������
		* @param [in] relativepath ���·��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool Save(xmlNodePtr node, glbWChar* relativepath);
		/**
		* @brief ���ݶ���id��ȡ����
		* @param [in] objId ����id	
		* @return  �ɹ����ض���ָ��
		-	 ʧ�ܷ���NULL
		*/
		CGlbGlobeObject* GetObjectById(glbInt32 objId);
		/**
		* @brief ��Ӷ���
		* @param [in] obj ����ָ��	
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool AddObject(CGlbGlobeObject* obj);
		/**
		* @brief ���ݶ���idɾ������
		* @param [in] objId ����id	
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool RemoveObject(glbInt32 objId);
		/**
		* @brief ɾ�����ж���		
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool RemoveAllObjects();

		/**
		* @brief ��ȡ��Ⱦ���ж�������	  
		* @return  ��������
		*/
		glbInt32 GetObjectCount();
		/**
		* @brief ����Ҫ��feature��������
		* @param [in] feature Ҫ��	
		* @return  �ɹ������½��Ķ���ָ��
		-	 ʧ�ܷ���NULL
		*/
		CGlbGlobeObject* CreateObject(CGlbFeature* feature);
		/**
		* @brief ��ȡ��Ⱦ����osg�ڵ� 
		* @return osg�ڵ�
		*/
		osg::Node* GetNode();
		/**
		* @brief ����Renderer�еĶ����Ƿ�����ж��
		* @param [in] isAllowUnload �Ƿ�����ж�� true-����ж��  false-����ж��
		* @return ��
		*/
		void SetIsAllowUnLoad(glbBool isAllowUnload);
		/**
		* @brief �����Ƿ�ʹ��������θ���Ϊ���������ľ���	
		* @param [in] isCameraAltitudeAsDistance true-ʹ�������������Ϊ
		* @return �ɹ�����true,ʧ�ܷ���false
		*/
		void SetCameraAltitudeAsDistance(glbBool isCameraAltitudeAsDistance);
		glbBool IsCameraAltitudeAsDistance();
		/**
		* @brief ���ö����Ƿ�������������ʶ
		* @param[in]  isFaded ����������������Ϊtrue����֮����Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/		
		void SetIsFade(glbBool isFaded);
		/**
		* @brief ��ȡ������Ƿ�������������ʶ
		* @return ������������true����֮����false
		*/
		glbBool IsFaded();
		/**
		* @brief ���ö��󽥽���������͸���Ȳ���
		* @param[in]  startAlpha ����������ʼ͸����
		* @param[in]  endAlpha ������������͸����
		*/
		void SetFadeParameter(glbFloat startAlpha,glbFloat endAlpha);

		/**
		* @brief ��ȡ���󽥽���������͸���Ȳ���
		* @param[out]  startAlpha ����������ʼ͸����
		* @param[out]  endAlpha ������������͸����
		*/
		void GetFadeParameter(glbFloat *startAlpha,glbFloat *endAlpha);

		/**
		* @brief ���ö��󽥽������Ĺ�������ʱ��
		* @param[in]  durationTime ����������������ʱ��
		*/
		void SetFadeDurationTime(glbDouble durationTime);

		/**
		* @brief ��ȡ���󽥽������Ĺ�������ʱ��
		* @return ���������Ĺ�������ʱ��
		*/
		glbDouble GetFadeDurationTime();

		/**
		* @brief  ����ͼ���Ƿ���Ҫ��ײ���		
		* @param  [in] isCollisionDetect  true��ʾ��Ҫ������ײ��⣬false��ʾ����Ҫ��ײ���		
		*		  [in] layerId ͼ��ID
		* @return void
		*/
		void	SetCollisionDetect(glbBool isCollisionDetect, glbInt32 layerId);
		/**
		* @brief ���ö����Ƿ���ҪԤ������ʾ�б�
		* @param isNeedToPreCompile true��ʾ��ҪԤ���� false ��ʾ����ҪԤ����
		*/
		void SetIsNeedToPreCompile(glbBool isNeedToPreCompile);
		/**
		* @brief ��ȡ�����Ƿ���ҪԤ������ʾ�б�
		* @return ��Ҫ����true,����Ҫ����false
		*/
		glbBool IsNeedToPreCompile();
	private:
		// ����Renderer�е�������Ե�obj��,��renderorder,loadorder,minDistance.....
		void UpdateAttribute(CGlbGlobeObject* obj);		
	private:
		CGlbWString               mpr_name;							//��Ⱦ������
		CGlbWString               mpr_geofield;						//�����ֶ�
		CGlbWString               mpr_labelfield;					//ע���ֶ�
		glbBool		              mpr_isground;	   				    //�Ƿ�������
		glbref_ptr<GlbRenderInfo> mpr_renderinfo;					//��Ⱦ������Ϣ
		GlbRenderDouble*          mpr_mindistance;					//��С�ɼ�����
		GlbRenderDouble*          mpr_maxdistance;					//���ɼ�����
		GlbRenderInt32*           mpr_loadorder;					//����˳��
		GlbRenderInt32*           mpr_renderorder;					//��Ⱦ˳��
		GlbRenderString*          mpr_message;						//������Ϣ
		GlbRenderString*          mpr_tooltips;						//��ʾ��Ϣ
		GlbAltitudeModeEnum       mpr_altitudemode;					//�߳�ģʽ
		CGlbGlobe*                mpr_globe;
		osg::ref_ptr<osg::Node>   mpr_node;							//osg�ڵ�
		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>> mpr_objects;			//��������
		GlbCriticalSection	                           mpr_critical;		//�޸Ĳ��� ���� �ٽ���
		glbBool					  mpr_isShow;						//��Ⱦ����������
		glbBool					  mpr_isCameraAltitudeAsDistance;	//�Ƿ�����߶���Ϊ����

		glbBool					mpr_isFaded;						//ͼ���ж����Ƿ񽥱���ʾ
		glbFloat				mpr_fadeStartAlpha;					//���俪ʼalphaֵ
		glbFloat				mpr_fadeEndAlpha;					//�������alphaֵ
		glbDouble				mpr_fadeDurationTime;				//��������ʱ��

		glbBool					mpr_isNeedToPreCompile;		///<�Ƿ���ҪԤ������ʾ�б�
	};

}

