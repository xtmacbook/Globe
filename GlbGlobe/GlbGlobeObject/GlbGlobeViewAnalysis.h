/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeVisualLineAnalysis.h
* @brief   ���߷���ͷ�ļ����ĵ�����CGlbGlobeVisualLineAnalysis��
* @version 1.0
* @author  ��ͥ��
* @date    2014-9-22 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeObjectExport.h"

#include "GlbGlobeREObject.h"
#include "IGlbGlobeTask.h"
#include "GlbGlobeCallBack.h"


#include "GlbLine.h"
#include "GlbPoint.h"

namespace GlbGlobe
{
	class CGlbViewGeometry;
	/**
	* @brief ���߷�����
	* @version 1.0
	* @author  ��ͥ��
	* @date    2014-9-22 15:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeViewAnalysis : public CGlbGlobeREObject
	{
		class CGlbGlobeViewAnalysisLoadDataTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeViewAnalysisLoadDataTask(CGlbGlobeViewAnalysis *obj,glbInt32 level);
			~CGlbGlobeViewAnalysisLoadDataTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject(){return mpr_obj.get();}
		private:
			glbref_ptr<CGlbGlobeViewAnalysis>          mpr_obj;
			glbInt32                                   mpr_level;
		};
	public:
		CGlbGlobeViewAnalysis(void);

		~CGlbGlobeViewAnalysis(void);

		virtual glbDouble GetDistance( osg::Vec3d &cameraPos,glbBool isCompute );

		virtual void LoadData( glbInt32 level );

		virtual void AddToScene();

		virtual glbInt32 RemoveFromScene( glbBool isClean );

		glbref_ptr<CGlbExtent> GetBound( glbBool isWorld = true );

		virtual IGlbGeometry * GetOutLine();

		virtual glbBool SetRenderInfo( GlbRenderInfo *renderInfo ,glbBool &isNeedDirectDraw);

		virtual GlbRenderInfo * GetRenderInfo();

		virtual glbBool SetShow( glbBool isShow,glbBool isOnState=false );

		virtual glbBool SetSelected( glbBool isSelected );

		virtual glbBool SetBlink( glbBool isBlink );

		virtual void DirectDraw( glbInt32 level );

		virtual void DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext );

		virtual glbInt32 GetOsgNodeSize();

		virtual glbBool Load( xmlNodePtr* node,glbWChar* prjPath );

		virtual glbBool Save( xmlNodePtr node,const glbWChar* prjPath );

		virtual GlbGlobeObjectTypeEnum GetType();

		
		/** �������� fov_indegree Ŀǰ���֧��50��*/
		glbBool SetView(CGlbPoint *watchPoint,CGlbPoint *targetPoint,glbDouble fov_indegree/*=50.0*/,glbBool &isNeedDirectDraw );
		/** �۲�� */
		CGlbPoint *GetWatchPoint()const{return mpr_watchPoint.get();}
		/** Ŀ��� */
		CGlbPoint *GetTargetPoint()const{return mpr_targetPoint.get();}
		/** ����fov����λ���� */
		glbDouble getViewFov()const{return mpr_fov_indegree;}
	private:
		void ReadData(glbInt32 level,glbBool isDirect=false);

		glbInt32 ComputeNodeSize(osg::Node *node);

		glbref_ptr<CGlbPoint>                           mpr_watchPoint;
		glbref_ptr<CGlbPoint>                           mpr_targetPoint;
		glbref_ptr<CGlbLine>							  mpr_visualLine;
		glbDouble                                         mpr_fov_indegree;

		glbDouble                                         mpr_distance;           ///<���������ľ���
		glbref_ptr<GlbRenderInfo>                         mpr_renderInfo;         ///<������Ⱦ��Ϣ
		glbInt32                                          mpr_objSize;			  ///<�ڵ��С
		GlbCriticalSection                                mpr_readData_critical;  ///<�����ڵ��ٽ���
		GlbCriticalSection                                mpr_addToScene_critical;///<�ڵ���ӳ����ٽ���
		glbBool											  mpr_needReReadData;	  ///<���¶�ȡ���ݱ�־
	};

}
