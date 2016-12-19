/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeDigHole.h
* @brief   �ڶ�����ͷ�ļ����ĵ�����CGlbGlobeDigHole��
* @version 1.0
* @author  ����
* @date    2014-8-22 15:34
*********************************************************************/
#pragma once
#include "GlbGlobeObjectExport.h"
#include "GlbGlobeReobject.h"
#include "CGlbGlobe.h"
#include "GlbPolygon.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeDigHole�߶�����				 
	* @version 1.0
	* @author  ����
	* @date    2014-8-22 15:34
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeDigHole : public CGlbGlobeREObject 
	{
	public:
		class CGlbGlobeDigHoleTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeDigHoleTask(CGlbGlobeDigHole *obj,glbInt32 level);
			~CGlbGlobeDigHoleTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobeDigHole> mpr_obj;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeDigHole(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeDigHole(void);
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
		**/
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
		**/
		//void UpdateElevate();
		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ������λ
		* @return  �����2D����
		*/
		IGlbGeometry *GetOutLine();
		/**
		* @brief ���ö������Ⱦ��Ϣ
		* @param[in]  renderInfo �������Ⱦ��Ϣ
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true����֮����false
		*/
		glbBool SetRenderInfo(GlbRenderInfo *renderInfo ,glbBool &isNeedDirectDraw);
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
		/*
		* @brief ��ȡ��ǰLOD�ڵ�Ĵ�С����λ���ֽڡ�
		* @return ��ǰLOD����osg�ڵ��С
		*/
		glbInt32 GetOsgNodeSize();
		/**
		* @brief �����ڶ��������μ�����
		* @param[in] region ����μ�����
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		*/
		void SetRegion(CGlbPolygon *region ,glbBool &isNeedDirectDraw);

		/**
		* @brief ��ȡ�ڶ��������μ�����
		* @return �ڶ��������μ�����
		*/
		CGlbPolygon* GetRegion();
		/**
		* @brief �����ڶ��������
		* @param [in] depth �����溣�θ߶�
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		*/
		void SetDepth(double depth ,glbBool &isNeedDirectDraw);

		/**
		* @brief ��ȡ�ڶ��������
		* @return �ڶ��������
		*/
		glbDouble GetDepth();
		/**
		* @brief �����Ƿ���ʾ���ı���
		*/
		void ShowWall(glbBool isshow);
		/**
		* @brief ��ȡ�Ƿ���ʾ���ı���
		*/
		glbBool IsShowWall();
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		void DrawToImage(glbByte *image,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);

		/**
		* @brief ��ȡ�����ڶ��Ķ�����Ŀ
		*/
		glbInt32 GetDiggedCount();

		/**
		* @brief ��ȡ�����ڶ��Ķ���
		*/
		CGlbGlobeObject* GetDiggedObject(glbInt32 objId);

		/**
		* @brief ��Ӷ��󣬲����ڶ�
		*/
		glbBool AddDiggedObject(CGlbGlobeRObject* obj);

		/**
		* @brief ʹ���󲻲����ڶ�
		*/
		glbBool RemoveDiggedObject(glbInt32 objId);

		void testDigHoleObj();
	private:
		/**
		* @brief ��ȡ����
		*/
		void ReadData(glbInt32 level ,glbBool isDirect=false);
		/**
		* @brief �����ڶ�����
		*/
		osg::Node* DrawDigHole(CGlbPolygon* polygon3d,glbDouble depth);
		osg::Node* DrawDigHole2(CGlbPolygon* polygon3d,glbDouble depth);//������Ӱ���Ƶķ��������ڶ�����
		osg::Node* DrawDigHole3(CGlbPolygon* polygon3d,glbDouble depth);
		// ��������ζ����м����� - ����CGlbMultiPolygon����
		IGlbGeometry* AnalysisPolyGeomertry(CGlbFeature* feature, CGlbPolygon* polyGeo);
		glbInt32 ComputeNodeSize(osg::Node *node);
		void DealSelected();
		void DealBlink();

		void DirtyOnTerrainObject();
	private:
		glbDouble mpr_distance;///<���������ľ���
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;///<������Ⱦ��Ϣ
		glbref_ptr<CGlbPolygon> mpr_holeRegion;	//�ڶ���Χ
		glbDouble mpr_holeDepth;	//�ڶ����
		glbBool mpr_isShowWall;		//�Ƿ���ʾ���ı���
		glbref_ptr<IGlbGeometry> mpr_outline;///<�����2D����
		glbDouble mpr_holeUpHei;	//�ɰ�����������߶�
		glbDouble mpr_holeDownHei;	//�ɰ�����������߶�
		std::vector<CGlbGlobeRObject*> mpr_diggedobjs;

		glbBool                   mpr_isCanDrawImage;		///<���������������	
		GlbCriticalSection        mpr_addToScene_critical;	///<�ڵ���ӳ����ٽ���
		GlbCriticalSection        mpr_readData_critical;	///<ReadData�����ڵ��ٽ���	
		glbBool                   mpr_needReReadData;		///<���¶�ȡ���ݱ�־
		glbInt32                  mpr_objSize;				///<obj��size
	};
}


