/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeTinObject.h
* @brief   ���������ͷ�ļ����ĵ�����CGlbGlobeTinObject��
* @version 1.0
* @author  ��ͥ��
* @date    2014-11-13 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeObjectExport.h"

#include "GlbGlobeREObject.h"
#include "IGlbGlobeTask.h"
#include "GlbGlobeCallBack.h"

#include "GlbTin.h"
#include "GlbRobjectMeshBox.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeTinObject������Tin������
	* @version 1.0
	* @author  ��ͥ��
	* @date    2014-11-13 15:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeTinObject : public CGlbGlobeREObject
	{
		class CGlbGlobeTinLoadDataTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeTinLoadDataTask(CGlbGlobeTinObject *obj,glbInt32 level);
			~CGlbGlobeTinLoadDataTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobeTinObject>    mpr_obj;
			glbInt32                        mpr_level;
		};
	public:
		CGlbGlobeTinObject(void);
		~CGlbGlobeTinObject(void);

		virtual glbBool SetAltitudeMode( GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw );

		virtual void SetRenderOrder( glbInt32 order );

		virtual glbDouble GetDistance( osg::Vec3d &cameraPos,glbBool isCompute );

		virtual void LoadData( glbInt32 level );

		virtual void AddToScene();

		virtual glbInt32 RemoveFromScene( glbBool isClean );

		virtual glbref_ptr<CGlbExtent> GetBound( glbBool isWorld = true );

		virtual void UpdateElevate();

		virtual IGlbGeometry * GetOutLine();

		virtual glbBool SetRenderInfo( GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw );

		virtual GlbRenderInfo * GetRenderInfo();

		virtual glbBool SetShow( glbBool isShow,glbBool isOnState=false );

		virtual glbBool SetSelected( glbBool isSelected );

		void SetPartSelected(glbInt32 partId,glbBool isSelected);

		virtual glbBool SetBlink( glbBool isBlink );

		virtual void DirectDraw( glbInt32 level );

		virtual void DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext );

		virtual glbInt32 GetOsgNodeSize();

		virtual glbBool Load( xmlNodePtr* node,const glbWChar* prjPath );

		virtual glbBool Load2( xmlNodePtr* node,const glbWChar* prjPath );

		virtual glbBool Save( xmlNodePtr node,const glbWChar* prjPath );

		virtual glbBool SetEdit( glbBool isEdit );

		virtual GlbGlobeObjectTypeEnum GetType();

		glbBool SetGeo(IGlbGeometry *tinGeo,glbBool &isNeedDirectDraw);
		IGlbGeometry *GetGeo()const{return mpr_tinGeo.get();}

		/**
		* @brief �߶������Ľ���
		* @param vtStart  ���
		* @param vtEnd  �յ�
		* @param interPoint ����
		* @return 0:û�н��� 1�н���
		*/
		virtual glbBool interset(osg::Vec3d vtStart, osg::Vec3d vtEnd, osg::Vec3d& interPoint);
	
	 
	private:
		void ReadData(glbInt32 level ,glbBool isDirect=false);
		glbBool DealTinModelSymbol(osg::MatrixTransform *tinContainer,glbInt32 level,GlbTinSymbolInfo *tinInfo,glbBool &isNeedDirectDraw);
		void DealSelected();
		void DealBlink();
		void ComputePosByAltitudeAndGloleType(osg::Vec3d &position);
		void DealModePosByChangeAltitudeOrChangePos();
		glbInt32 ComputeNodeSize( osg::Node *node );
		void DirtyOnTerrainObject();
		osg::Node* FindPartNodeByID(glbInt32 partId);

		bool judgeTinWaterNeedDirectDraw( GlbTinWaterSymbolInfo *tinWaterSmblInfo, bool &isNeedDirectDraw );
		//�ָ���
		void MeshTinToGrids(glbInt32 level);

		//�����������ɫ��������ʱ����Ҫ
		void ParseObjectFadeColor();
		/**
		* @brief ���¸���
		* @ parentMatrix ����tinContainer�ľ���
		* @globeMode �Ƿ�������ģʽ
		*/
		void updateMeshBox(const osg::Matrix parentMatrix,const GlbTinSymbolInfo * tinSmblInfo, 
			glbref_ptr<CGlbFeature>  feature,GLboolean globeMode);

		glbDouble                                         mpr_distance;        ///<���������ľ���
		glbref_ptr<GlbRenderInfo>                         mpr_renderInfo;      ///<������Ⱦ��Ϣ
		glbref_ptr<IGlbGeometry>                          mpr_outline;         ///<�����2D����
		glbref_ptr<IGlbGeometry>                          mpr_tinGeo;      ///<����Ķ��������

		glbDouble                                         mpr_elevation;		///<��ǰ���θ�

		glbBool                   mpr_isCanDrawImage;		///<���������������
		glbInt32                  mpr_objSize;				///<obj��size

		GlbCriticalSection        mpr_addToScene_critical;	///<�ڵ���ӳ����ٽ���
		GlbCriticalSection		  mpr_readData_critical;	///<ReadData�����ڵ��ٽ���	
		glbBool                   mpr_needReReadData;		///<���¶�ȡ���ݱ�־

		std::map<glbInt32,glbBool> mpr_partSelect;   ///<������id���Ƿ�ѡ�б�־����

		glbFloat				 mpr_TexRotation;

		ClbRobjectMeshBox		mpr_MeshBox;
	};

}
