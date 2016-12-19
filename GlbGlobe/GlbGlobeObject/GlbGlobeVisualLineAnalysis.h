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
	/**
	* @brief ���߷�����
	* @version 1.0
	* @author  ��ͥ��
	* @date    2014-9-22 15:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeVisualLineAnalysis : public CGlbGlobeREObject
	{
		class CGlbGlobeVisualLineAnalysisLoadDataTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeVisualLineAnalysisLoadDataTask(CGlbGlobeVisualLineAnalysis *obj,glbInt32 level);
			~CGlbGlobeVisualLineAnalysisLoadDataTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject(){return mpr_obj.get();}
		private:
			glbref_ptr<CGlbGlobeVisualLineAnalysis>    mpr_obj;
			glbInt32                                   mpr_level;
		};
	public:
		CGlbGlobeVisualLineAnalysis(void);
		~CGlbGlobeVisualLineAnalysis(void);
		
		virtual glbBool Load( xmlNodePtr* node,glbWChar* prjPath );

		virtual glbBool Save( xmlNodePtr node,const glbWChar* prjPath );

		virtual GlbGlobeObjectTypeEnum GetType();

		virtual glbDouble GetDistance( osg::Vec3d &cameraPos,glbBool isCompute );

		virtual void LoadData( glbInt32 level );

		virtual void AddToScene();

		virtual glbInt32 RemoveFromScene( glbBool isClean );

		virtual glbref_ptr<CGlbExtent> GetBound( glbBool isWorld = true );

		virtual IGlbGeometry * GetOutLine();

		virtual glbBool SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw);

		virtual GlbRenderInfo * GetRenderInfo();

		virtual glbBool SetShow( glbBool isShow,glbBool isOnState=false );

		virtual void DirectDraw( glbInt32 level );		

		virtual glbInt32 GetOsgNodeSize();		
		//////////////////////////////////////////////////////////////////////////	

		/** �������� */
		glbBool SetSightLine(CGlbPoint *watchPoint,CGlbPoint *targetPoint,glbBool &isNeedDirectDraw );
		/** �۲�� */
		CGlbPoint *GetWatchPoint()const{return mpr_watchPoint.get();}
		/** Ŀ��� */
		CGlbPoint *GetTargetPoint()const{return mpr_targetPoint.get();}
	private:
		void ReadData(glbInt32 level,glbBool isDirect=false);

		glbInt32 ComputeNodeSize( osg::Node *node );

		glbref_ptr<CGlbPoint>                           mpr_watchPoint;		  ///<���λ��(��γ��)
		glbref_ptr<CGlbPoint>                           mpr_targetPoint;		  ///<����λ��
		glbref_ptr<CGlbLine>							  mpr_visualLine;		
		
		glbDouble                                         mpr_distance;           ///<���������ľ���
		glbref_ptr<GlbRenderInfo>                         mpr_renderInfo;         ///<������Ⱦ��Ϣ
		glbInt32                                          mpr_objSize;			  ///<�ڵ��С
		GlbCriticalSection                                mpr_readData_critical;  ///<�����ڵ��ٽ���
		GlbCriticalSection                                mpr_addToScene_critical;///<�ڵ���ӳ����ٽ���
		glbBool											  mpr_needReReadData;	  ///<���¶�ȡ���ݱ�־
	};

}
