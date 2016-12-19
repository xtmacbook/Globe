/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    CGlbClipObject.h
* @brief   �ü����� ͷ�ļ�
*
* �����������CGlbClipObject���class
*
* @version 1.0
* @author  ChenPeng
* @date    2016-10-27 13:30
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "GlbGlobeObject.h"
#include "GlbGlobeLayer.h"
#include "GlbExtent.h"
#include <osg/ClipNode>

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CGlbClipObject : public CGlbGlobeObject
	{
	public:
		CGlbClipObject(void);
		~CGlbClipObject(void);
		glbBool AddClipPlane(glbDouble planeA,glbDouble planeB,glbDouble planeC,glbDouble planeD);
		glbBool RemoveClipPlane(glbUInt32 pos);
		void GetClipPlane(glbUInt32 pos,glbDouble &planeA, glbDouble &planeB, glbDouble &planeC, glbDouble &planeD);
		glbInt32 GetClipPlaneCount();
		void SetClipBox(CGlbExtent* ext);
		void SetOpposeSide(unsigned int pos);
		//�����յ������µĻ�,��Ҫ�ڵ���������³��߶ȣ���Ҫ��Ը߳�
		void ConvertLatLongHeightToXYZ(glbDouble lat,glbDouble lon,glbDouble ele,glbDouble &posX, glbDouble &posY, glbDouble &posZ);
		void ComputeClipPlane(osg::Vec3d v1, osg::Vec3d v2, osg::Vec3d v3, glbDouble &planeA, glbDouble &planeB, glbDouble &planeC, glbDouble &planeD);
		// �ü�����	
		void Clip(CGlbGlobeObject* obj, glbBool enableClip = true);		
		// �ü�Ҫ��ͼ��
		void Clip(CGlbGlobeLayer* layer, glbBool enableClip = true);
	private:
		osg::ref_ptr<osg::ClipNode> mpr_clipNode;
		osg::Vec3d                                 mpr_offsetPos;
		glbInt32                                      mpr_clipID;//������id,clipNode��ڵ�ֱ������clipPlane������
		bool                                             mpr_markOffsetPos;
	};
}