/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeRObject.h
* @brief   �ɻ��ƶ���ͷ�ļ����ĵ�����CGlbGlobeRObject��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-6 10:50
*********************************************************************/
#pragma once
#include "GlbGlobeObject.h"
#include "osg/Node"
#include "GlbWString.h"
#include "GlbExtent.h"
#include "IGlbGeometry.h"
#include "GlbFeature.h"
#include "GlbGlobeLayer.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbClipObject.h"

namespace GlbGlobe
{
	/**
	* @brief �ɻ��ƶ�����				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-6 10:50
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeRObject : public CGlbGlobeObject
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeRObject();
		/**
		* @brief ��������
		*/
		~CGlbGlobeRObject();
		/**
		* @brief �޸Ķ���״̬���Ƿ��ڿɼ�����
		* @param[in]  isInViewPort �ڿɼ���������Ϊtrue����֮����Ϊfalse
		*/
		void SetInViewPort(glbBool isInViewPort);
		/**
		* @brief ��ȡ�����Ƿ��ڿɼ�����
		* @return �ڿɼ����򷵻�true����֮����false
		*/
		glbBool IsInViewPort();
		/**
		* @brief ����ӳ������Ƴ�ʱ���������ٱ�ʶ
		* @param[in]  isInViewPort �ڿɼ���������Ϊtrue����֮����Ϊfalse
		*/
		void SetDestroy();
		/**
		* @brief ��ȡ�������ٱ�ʶ
		* @return ���ٷ���true����֮����false
		*/
		glbBool IsDestroy();
		/**
		* @brief ��ȡ��������ľ���
		* @param[in]  cameraPos ����ڳ����е�λ��
		* @param[in]  isCompute �����Ƿ���Ҫ���¼��㣬���¼�������true����֮����false
		* @return ��������ľ���
		*/
		virtual glbDouble GetDistance(osg::Vec3d &cameraPos,glbBool isCompute);
		/**
		* @brief �����Ƿ�ʹ��������θ���Ϊ���������ľ���	
		* @param [in] isCameraAltitudeAsDistance true-ʹ�������������Ϊ
		* @return ��
		*/
		void SetCameraAltitudeAsDistance(glbBool isCameraAltitudeAsDistance);
		glbBool IsCameraAltitudeAsDistance();
		/**
		* @brief װ�����ݣ���������Ƶ���ĵ��ø÷���
		* @param[in]  level ��Ҫװ�����ݵĵȼ�
		*/
		virtual void LoadData(glbInt32 level);
		/**
		* @brief ��osg�ڵ���߽���osg�ڵ㵽������
		* @return ���عҵ���������osg�ڵ��С
		*/
		virtual void AddToScene();
		/**
		* @brief �ӳ�����ժ��osg�ڵ�
		* @param[in]  isClean true��ʾͬʱɾ������osg�ڵ��Խ�ʡ�ڴ�
		* @return ���ؽ�ʡ���ڴ��С
		*/
		virtual glbInt32 RemoveFromScene(glbBool isClean);
		/**
		* @brief ��ȡ����ǰLOD����
		* @return ����ǰLOD����
		*/
		glbInt32 GetCurrLevel();
		/**
		* @brief �Ƿ�ʹ��ʵ����
		* @param[in]  useInstance ʹ��ʵ��������Ϊtrue����֮����Ϊfalse
		* @note ֻ�Ե������ã�ʵ�������󣬹�ͬʹ��osg�ڵ㣬���Խ�ʡ�ڴ桢�Դ�
		*/
		virtual void SetUseInstance(glbBool useInstance);
		/**
		* @brief ��ȡ�����Ƿ�ʵ������ʶ
		* @return ʵ��������true����֮����false
		*/
		glbBool IsUseInstance();
		/**
		* @brief ��ȡ���󸸽ڵ�
		* @return ���ڵ�ָ��
		*/
		osg::Node *GetParentNode();
		/**
		* @brief ���ö��󸸽ڵ�
		* @param[in] ���ڵ�ָ��
		*/
		virtual void SetParentNode(osg::Node *parent);
		/**
		* @brief �ж϶����Ƿ����û����ͼװ�ع�����
		* @return ����û�з���true����֮����false
		*/
		glbBool GetNeverLoadState();
		/**
		* @brief ��������װ��״̬��ʹ���ٴ�LoadDataʱ���Բ�������װ������
		*/
		void ResetLoadData();
		/**
		* @brief ��ȡ�����Ƿ�����װ������.
		*/
		glbBool IsLoadData();		
		/**
		* @brief ��ȡ�������ɼ�����
		* @return �������ɼ�����
		*/
		glbDouble GetMaxVisibleDistance();
		/**
		* @brief ��ȡ������С�ɼ�����
		* @return ������С�ɼ�����
		*/
		glbDouble GetMinVisibleDistance();
		/**
		* @brief ��ȡ������Ⱦ���ȼ�
		* @return ������Ⱦ���ȼ�
		*/
		glbInt32 GetRenderOrder();
		/**
		* @brief ��ȡ����װ�����ȼ�
		* @return ����װ�����ȼ�
		*/
		glbInt32 GetLoadOrder();
		/**
		* @brief ����Ԫ��
		* @param[in]  feature �����Ӧ��Ԫ��
		* @param[in]  geoField �����ֶ�
		*/
		void SetFeature(CGlbFeature *feature,const glbWChar* geoField=L"");
		/**
		* @brief ��ȡԪ��
		* @return �����Ӧ��Ԫ��
		*/
		CGlbFeature *GetFeature();
		/**
		* @brief ����Ԫ��ͼ��
		* @param[in]  featureLayer Ԫ�����ڵ�ͼ��
		*/
		void SetFeatureLayer(CGlbGlobeLayer *featureLayer);
		/**
		* @brief ��ȡ����ڵ��Χ��
		* @param[in]  isWorld true��ʾ��ȡ���������Χ��(��λ����)
		*  -                                    false��ʾ��ȡ���������Χ�У���λ���ȣ�
		* @return �ڵ��Χ��
		*/
		virtual glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief ���ö������ڰ˲����ڵ� 
		* @param[in]  octree �������ڰ˲����ڵ�
		*/
		void SetOctree(void *octree);		
		/**
		* @brief ��ȡ�������ڰ˲����ڵ�
		* @return �������ڰ˲����ڵ�
		*/
		void *GetOctree();		
		/**
		* @brief ���ö��������Ĳ����ڵ� 
		* @param[in]  qtree ���������Ĳ����ڵ�
		*/
		void SetQtree(void *qtree);

		/**
		* @brief ��ȡ���������Ĳ����ڵ�
		* @return  ���������Ĳ����ڵ�
		*/
		void *GetQtree();

		/**
		* @brief ��ȡ����ĺ���ģʽ
		* @return ����ģʽö��
		*/
		GlbAltitudeModeEnum GetAltitudeMode();
		/**
		* @brief ��Ե��λ���ʱ�����¶����Ӧ��ĵ��θ߶�
		*/
		virtual void UpdateElevate();
		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ������λ
		* @return  �����2D����
		*/
		virtual IGlbGeometry *GetOutLine();
		/**
		* @brief ���ö������Ⱦ��Ϣ
		* @param[in]  renderInfo �������Ⱦ��Ϣ
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true����֮����false
		*/
		virtual glbBool SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw);
		/**
		* @brief ��ȡ�������Ⱦ��Ϣ
		* @return �������Ⱦ��Ϣ
		*/
		virtual GlbRenderInfo *GetRenderInfo();
		/**
		* @brief ���ö�������
		* @param[in]  isShow ������ʾ����Ϊtrue����������Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		virtual glbBool SetShow(glbBool isShow,glbBool isOnState=false);
		/**
		* @brief ��ȡ�����������ʶ
		* @return ��ʾ����true�����ط���false
		*/
		glbBool IsShow();	

		/**
		* @brief ���ö����Ƿ�������������ʶ
		* @param[in]  isFaded ����������������Ϊtrue����֮����Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		glbBool EnableFade(glbBool isFaded);

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
		* @brief ���ö����ѡ��״̬
		* @param[in]  isSelected ����ѡ������Ϊtrue����֮����Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		virtual glbBool SetSelected(glbBool isSelected);
		/**
		* @brief ��ȡ�����ѡ��״̬
		* @return ѡ�з���true����֮����false
		*/
		glbBool IsSelected();
		/**
		* @brief ���ö���Ĳü�״̬
		* @param[in]  isSelected ���ü�����Ϊtrue����֮����Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		virtual glbBool SetCliped(glbBool isCliped);
		/**
		* @brief ��ȡ����Ĳü�״̬
		* @return ѡ�з���true����֮����false
		*/
		glbBool IsCliped();
		void SetClipObject(CGlbClipObject *clipObject);
		/**
		* @brief ���ö����Ƿ���˸
		* @param[in]  isBlink ��˸����Ϊtrue������˸����Ϊfalse
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		virtual glbBool SetBlink(glbBool isBlink);
		/**
		* @brief ��ȡ�������˸��ʶ
		* @return ��˸����true������˸����false
		*/
		glbBool IsBlink();
		/**
		* @brief ����������Ⱦ	
		* @param[in]  level ������Ⱦ�ȼ�
		*/
		virtual void DirectDraw(glbInt32 level);
		/**
		* @brief ��ȡ������ʾ��Ϣ
		* @return ������ʾ��ʾ
		* @note �����ŵ�������ʱ����ʾ����
		*/		
		const glbWChar* GetTooltip();
		/**
		* @brief ��ȡ������Ϣ
		* @return ������Ϣ
		* @note ���������ʱ����ʾ����
		*/		
		const glbWChar* GetRMessage();
		/**
		* @brief ���ö����Ƿ��ǵ������
		* @param[in]  ground �ǵ����������Ϊtrue����֮����Ϊfalse
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		glbBool SetGround(glbBool ground);
		/**
		* @brief ��ȡ�����Ƿ��ǵ������
		* @return �Ƿ���true����֮����false
		*/
		glbBool IsGround();
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		virtual void DrawToImage(glbByte *image/*RGBA*/,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief ��ȡ��ǰLOD����osg�ڵ�
		* @return ��ǰLOD����osg�ڵ�
		*/
		osg::Node *GetOsgNode();

		/**
		* @brief ��ȡ��ǰLOD�ڵ�Ĵ�С����λ���ֽڡ�
		* @return ��ǰLOD����osg�ڵ��С
		*/
		virtual glbInt32 GetOsgNodeSize();
		/**
		* @brief �����ڹҶ�����
		* @return  
		*/
		void DecreaseHangTaskNum(void);

		/**
		* @brief �����л����󣬽���xml��ȡ��������ֵ
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return �����л��ɹ�����true����֮����false
		*/
		virtual glbBool Load(xmlNodePtr* node,const glbWChar* prjPath);
		virtual glbBool Load2(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief ���л��������л��������Ե�xml�����ļ�
		* @param[in]  os �����ļ������
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return ���л��ɹ�����true����֮����false
		*/
		virtual glbBool Save(xmlNodePtr node,const glbWChar* prjPath);

		/**
		* @brief ���ö���ĸ�����
		* @param[in]  parentObj ������
		*/
		void SetParentObject(CGlbGlobeObject *parentObj);

		/**
		* @brief ��ȡ����ĸ�����
		*/
		CGlbGlobeObject *GetParentObject()const;
		/**
		*  @brief ��ȡ�Ӷ�����Ŀ.
		*/
		virtual glbInt32 GetCount();
		/**
		*  @brief ��ȡ�Ӷ���.
		*/
		virtual CGlbGlobeRObject *GetRObject(glbInt32 idx);
		/**
		* @brief ���ö������༭״̬
		* @return �ɹ�����true,ʧ�ܷ���false
		*/
		virtual glbBool SetEdit(glbBool isEdit);
		/**
		* @brief ��ȡ�����Ƿ��ڱ༭״̬
		* @return �ڱ༭״̬����true,���򷵻�false
		*/
		glbBool IsEdit();
		/**
		* @brief ���ö���༭ģʽ
		* @return �ɹ�����true,ʧ�ܷ���false
		*/
		glbBool SetEditMode(GlbGlobeRObEditModeEnum editMode);
		/**
		* @brief ��ȡ����༭ģʽ
		* @return ����ǰ�༭ģʽ
		*/
		GlbGlobeRObEditModeEnum GetEditMode();
		/**
		* @brief ��ȡ����༭�ڵ�
		* @return ����ǰ�༭�ڵ�
		*/
		osg::Group *GetEditNode();
		/**
		* @brief ���ö���ͨ������
		* @param[in] data ͨ�����ݣ�����symbol��object֮������ݴ���
		*/
		void SetCustomData(osg::Object *data);
		/**
		* @brief ��ȡ����ͨ������
		* @return ����ͨ������
		*/
		osg::Object *GetCustomData();
		/**
		* @brief ��ȡ��ǰ��Ƶ���������
		* @return ��ǰ��Ƶ���������
		*/
		glbInt32 GetVideoIndex();
		/**
		* @brief ���ö���ͨ������
		* @param[in] str ����ͨ������
		*/
		void SetCustomDouble(glbDouble customDouble);
		/**
		* @brief ��ȡ����ͨ������
		* @return ����ͨ������
		*/
		glbDouble GetCustomDouble();
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
		/**
		* @brief ���ö���Ԥ������ʾ�б��Ƿ����
		* @param isPreCompiled true��ʾ��ʾ�б��Ѿ�Ԥ������� false ��ʾ��ʾ�б���뻹û�����
		*/
		void SetIsPreCompiled(glbBool isPreCompiled);
		/**
		* @brief ��ȡ����Ԥ������ʾ�б��Ƿ����
		* @return ��ɷ���true,���򷵻�false
		*/
		glbBool IsPreCompiled();

		// ��Ա����
		typedef std::pair<glbDouble,glbDouble>  MinMaxPair;
		typedef std::vector<MinMaxPair> RangeList;

		/**
		* @brief ������ر�lod
		* @param enable trueΪ������false Ϊ�ر�
		* @return ��
		*/
		void EnableLOD(glbBool enable);

		/**
		* @brief lod�Ƿ���
		* @return trueΪ������false Ϊ�ر�
		*/
		glbBool IsLODEnable();

		/**
		* @brief ���������distance����ʱ����ʾ�ȼ�
		* @param distance �����object�ľ���
		* @return level�ȼ�
		*/
		virtual glbInt32 ComputeRefLevel(glbDouble distance);

		/**
		* @brief���lod��Ϣ
		* @param level  �ȼ�
		* @param min �������
		* @param max ��Զ����
		* @return ��
		*/
		void AddLODRange(glbInt32 level, glbDouble min, glbDouble max);

		/**
		* @brief ��ȡ����rangeList�б�		
		* @return rangeList�б�
		*/
		inline const RangeList& getLODRangeList() const { return mpt_rangeList; }

		/**
		* @brief ���ö���rangeList�б�		
		* @param [in] rangeList lod�б�
		* @return ��
		*/
		inline void setRangeList(const RangeList& rangeList) { mpt_rangeList=rangeList; }
		/**
		* @brief ���ö����Ƿ�����ж��
		* @param [in] isAllowUnload �Ƿ�����ж�� true-����ж��  false-����ж��
		* @return ��
		*/
		void SetIsAllowUnLoad(glbBool isAllowUnload=false);
		/**
		* @brief ��ȡ�����Ƿ�����ж��		
		* @return ��������ж�� true-����ж��  false-����ж��
		*/
		glbBool IsAllowUnLoad();

		/**
		* @brief ���ö����Ƿ�����ѡ��
		* @param [in] isEnableSelect �Ƿ�����ѡ�� true-����ѡ��  false-���ɱ�ѡ��
		* @return ��
		*/
		void EnableSelect(glbBool isEnableSelect);
		/**
		* @brief ��ȡ�����Ƿ�����ѡ��	
		* @return ��������ѡ�� true-����ѡ��  false-����ѡ��
		*/
		glbBool IsSelectEnable();
		/**
		* @brief �������Ľ���
		* @param vtStart  ���
		* @param vtEnd  �յ�
		* @param interPoint ����
		* @return 
		*/
		virtual glbBool interset(osg::Vec3d vtStart, osg::Vec3d vtEnd, osg::Vec3d& interPoint);
	protected:
		osg::ref_ptr<osg::Node> mpt_node;///<OSG�ڵ�
		glbBool                 mpt_isUseInstance;///<�Ƿ�ʵ��������
		glbInt32                mpt_currLevel;///<��ǰLOD����
		glbInt32                mpt_preLevel;///<��һ��LOD����
		osg::ref_ptr<osg::Node> mpt_parent;///<���ڵ�

		osg::ref_ptr<osg::Node> mpt_preNode;///<��һ��OSG�ڵ�
		glbBool                 mpt_loadState;///<�Ƿ�����װ������		
		glbInt32                mpt_HangTaskNum;  ///<���������		
		glbDouble               mpt_maxVisibleDistance;///<���ɼ�����
		glbDouble               mpt_minVisibleDistance;///<��С�ɼ�����
		glbInt32                mpt_renderOrder;///<��Ⱦ���ȼ� ע:��ֵС���Ȼ��ƣ���ֵ��ĺ���� 
		glbInt32                mpt_loadOrder;///<װ�����ȼ�
		GlbAltitudeModeEnum     mpt_altitudeMode;///<�߳�ģʽ
		glbBool                 mpt_isSelected;///<�Ƿ�ѡ��
		glbBool                 mpt_isCliped;//�Ƿ�ü�
		glbBool                 mpt_isShow;///<�û������Ƿ���ʾ
		glbBool                 mpt_isDispShow;///<���ȿ����Ƿ���ʾ
		glbBool                 mpt_isBlink;///<�Ƿ���˸
		glbBool                 mpt_isGround;///<�Ƿ��ǵ������
		glbref_ptr<CGlbFeature> mpt_feature;///<����Ԫ��
		CGlbWString             mpt_geoField;///<�����ֶ�
		CGlbWString             mpt_tooltip;///<��ʾ��Ϣ
		CGlbWString             mpt_message;///<��Ϣ
		glbBool                      mpt_isFaded;//�Ƿ�����������
		glbFloat                     mpt_fadeStartAlpha;//���俪ʼalphaֵ
		glbFloat                     mpt_fadeEndAlpha;//�������alphaֵ
		glbDouble                 mpt_fadeDurationTime;//��������ʱ��
		osg::Vec4					mpt_fadeColor;//������ɫ��һ�㶼�Ƕ����FillColor
		/*
		��������������ͼ��.
		�������,ɾ��ͼ��ʱ,ͼ������ü�����>0,ͼ�㲻������.
		*/
		/*glbref_ptr<CGlbGlobeLayer>*/ CGlbGlobeLayer* mpt_featureLayer;///<Ҫ��ͼ��

		CGlbGlobeObject *mpt_parentObj;///<������[��϶�����]

		glbBool mpt_isEdit;				///<�Ƿ��ڱ༭״̬
		GlbGlobeRObEditModeEnum mpt_editMode;///<�༭ģʽ
		osg::ref_ptr<osg::Group>  mpr_editNode;				///<�༭����.

		glbBool					mpt_isNeedToPreCompile;		///<�Ƿ���ҪԤ������ʾ�б�
		glbBool					mpt_isPreCompiled;			///<Ԥ�����Ƿ����

		glbBool					mpt_isLODEnable;			///<�����Ƿ�ʹ��LOD
		RangeList               mpt_rangeList;				///<����lod�б�
		glbBool					mpt_isAllowUnLoad;			///<�����Ƿ�ж��
		glbBool					mpt_isCameraAltitudeAsDistance; ///<�Ƿ�ʹ��������θ���Ϊ���������ľ���
		glbBool					mpt_isEnableSelect;			///<�Ƿ�����ѡ��
		glbInt32                mpt_videoIndex;//��¼��ǰ��Ƶ������
	private:
		glbBool mpr_isInViewPort;///<�Ƿ��ڿɼ�����
		glbBool mpr_isDestroy;///<�Ƿ�����
		void *mpr_octree;///<�˲����ڵ�
		void *mpr_qtree;///<�Ĳ����ڵ�
		osg::ref_ptr<osg::Object> mpr_customData;
		glbDouble                             mpr_customDouble;
		glbref_ptr<CGlbClipObject> mpr_clipObject;
	};
}