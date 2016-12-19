/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeLine.h
* @brief   �߶���ͷ�ļ����ĵ�����CGlbGlobeLine��
* @version 1.0
* @author  ����
* @date    2014-6-3 10:00
*********************************************************************/
#pragma once
#include "GlbGlobeObjectExport.h"
#include "GlbGlobeReobject.h"
#include "CGlbGlobe.h"
#include "GlbLine.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeLine�߶�����				 
	* @version 1.0
	* @author  ����
	* @date    2014-6-3 10:00
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeLine : public CGlbGlobeREObject
	{
	public:
		class CGlbGlobeLineTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeLineTask(CGlbGlobeLine *obj,glbInt32 level);
			~CGlbGlobeLineTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobeLine> mpr_obj;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeLine(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeLine(void);
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
		/*
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
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);	
		/**
		* @brief ���ö�����Ⱦ���ȼ�
		* @param[in]  order ������Ⱦ���ȼ�
		*/
		void SetRenderOrder(glbInt32 order);	
		/**
		* @brief ��ȡ�߶�����ά�߼�����
		* @return ��ά�߼�����
		*/
		CGlbLine *GetGeo();
		/**
		* @brief �����߶�����ά�߼�����
		* @param[in] line ��ά�߼�����
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		glbBool SetGeo(CGlbLine *line,glbBool &isNeedDirectDraw );	
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		void DrawToImage(glbByte *image,glbInt32 imageW,
			glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief ��ȡ��ǰLOD�ڵ�Ĵ�С����λ���ֽڡ�
		* @return ��ǰLOD����osg�ڵ��С
		*/
		glbInt32 GetOsgNodeSize();

		/**
		* @brief ���ö��������˳�"�༭"״̬
		* @param[in]  isEdit ����༭״̬Ϊtrue���˳�Ϊfalse
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		glbBool SetEdit(glbBool isEdit);

		CGlbLine *RemoveRepeatePoints(CGlbLine *line);
	private:
		//װ������
		void ReadData(glbInt32 level, glbBool isDirect=false);
		void ComputePosByAltitudeAndGloleType(osg::Vec3d &position);
		glbInt32 ComputeNodeSize(osg::Node *node);
		void DealSelected();
		void DealBlink();
		void DealEdit();
		void DealOpacity(osg::Node *node,GlbRenderInfo *renderInfo);		

		glbBool DealPixelSymbol(GlbLinePixelSymbolInfo *linePixelInfo,glbBool &isNeedDirectDraw);
		glbBool DealArrowSymbol(GlbLineArrowSymbolInfo *lineArrowInfo,glbBool &isNeedDirectDraw);
		glbBool DealDynamicArrowSymbol(GlbLineDynamicArrowSymbolInfo *dynamicArrowInfo,glbBool &isNeedDirectDraw);
		void DealModePosByChangeAltitudeOrChangePos();

		osg::MatrixTransform* createline(CGlbLine* line3d);

		// �����߶����м����� - ����CGlbMultiLine����
		IGlbGeometry* AnalysisLineGeomertry(CGlbFeature* feature, CGlbLine* lineGeo);
		// ˢ�µ��Ρ��ࡱ����
		void DirtyOnTerrainObject();

		//�ӳ������Ƴ��ڵ㣬���ڷ�������ģʽת��Ϊ������ģʽʱ���������еķ������ζ���ڵ��Ƴ�
		void RemoveNodeFromScene(glbBool isClean=true);

		glbBool GetFeaturePoint( glbInt32 idx,glbDouble *ptx,glbDouble *pty,glbDouble *ptz,glbDouble *ptm);

		//�����������ɫ��������ʱ����Ҫ
		void ParseObjectFadeColor();
	private:
		glbDouble                 mpr_distance;		    	///<���������ľ���
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;	        ///<������Ⱦ��Ϣ
		glbref_ptr<IGlbGeometry>  mpr_outline;		        ///<�����2D����
		glbref_ptr<CGlbLine>    mpr_lineGeo;			    ///<�߶���ļ�������		
		glbDouble                 mpr_elevation;			///<��ǰ���θ�
		glbBool                   mpr_isCanDrawImage;		///<���������������
		glbInt32                  mpr_objSize;				///<obj��size
		GlbCriticalSection        mpr_addToScene_critical;	///<�ڵ���ӳ����ٽ���
		GlbCriticalSection        mpr_readData_critical;	///<ReadData�����ڵ��ٽ���	
		glbBool                   mpr_needReReadData;		///<���¶�ȡ���ݱ�־
		GlbCriticalSection		  mpr_outline_critical;		///<outline��д�ٽ���		
	};
}