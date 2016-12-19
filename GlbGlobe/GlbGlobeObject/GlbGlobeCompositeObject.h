/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeCompositeObject.h
* @brief   ���϶���ͷ�ļ����ĵ�����CGlbGlobeCompositeObject��
* @version 1.0
* @author  ��ͥ��
* @date    2014-9-4 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeObjectExport.h"

#include "GlbGlobeREObject.h"
#include "CGlbGlobe.h"


#include <list>

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeCompositeObject���϶�����
	* @version 1.0
	* @author  ��ͥ��
	* @date    2014-9-4 15:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeCompositeObject : public CGlbGlobeREObject
	{
	public:
		CGlbGlobeCompositeObject(void);
		~CGlbGlobeCompositeObject(void);

		glbBool   SetAltitudeMode( GlbAltitudeModeEnum mode ,glbBool &isNeedDirectDraw);
		void      SetRenderOrder( glbInt32 order );
		void      SetLoadOrder(glbInt32 order);
		void      SetUseInstance(glbBool useInstance);
		glbDouble GetDistance( osg::Vec3d &cameraPos,glbBool isCompute );
		void      LoadData( glbInt32 level );
		void      AddToScene();
		glbInt32  RemoveFromScene( glbBool isClean );
		void      SetParentNode( osg::Node *parent );
		glbref_ptr<CGlbExtent> GetBound( glbBool isWorld = true );
		void            UpdateElevate();
		IGlbGeometry *  GetOutLine();
		glbBool         SetRenderInfo( GlbRenderInfo *renderInfo ,glbBool &isNeedDirectDraw);
		GlbRenderInfo * GetRenderInfo();
		glbBool SetShow( glbBool isShow,glbBool isOnState=false );
		glbBool SetSelected( glbBool isSelected );
		glbBool SetBlink( glbBool isBlink );
		void    DirectDraw( glbInt32 level );
		void    DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext );
		glbInt32 GetOsgNodeSize();
		glbBool  Load( xmlNodePtr* node,glbWChar* prjPath );
		glbBool  Save( xmlNodePtr node,const glbWChar* prjPath );
		GlbGlobeObjectTypeEnum GetType();
		void SetGlobe( CGlbGlobe *globe );

		//////////////////////////////////////////////////////////////////////////
		glbInt32 GetCount();
		/* ��Ӹ����Ӷ���*/
		glbBool AddObject(CGlbGlobeRObject *obj);
		/* �Ƴ������Ӷ��� �±��0��ʼ*/
		glbBool RemoveObject(glbInt32 idx);
		/* ��ȡ�����Ӷ��� �±��0��ʼ*/
		CGlbGlobeRObject *GetRObject(glbInt32 idx);
	private:
		typedef std::vector<glbref_ptr<CGlbGlobeRObject> >    CGlbGlobeRObjectList;
		CGlbGlobeRObjectList                                 mpr_subogjs;

		glbInt32                                             mpr_objSize;		
	};

}
