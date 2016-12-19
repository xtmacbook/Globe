/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    CGlbClipObject.h
* @brief   裁剪对象 头文件
*
* 这个档案定义CGlbClipObject这个class
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
		//如果封闭的是上下的话,需要节点上下面的下沉高度，需要相对高程
		void ConvertLatLongHeightToXYZ(glbDouble lat,glbDouble lon,glbDouble ele,glbDouble &posX, glbDouble &posY, glbDouble &posZ);
		void ComputeClipPlane(osg::Vec3d v1, osg::Vec3d v2, osg::Vec3d v3, glbDouble &planeA, glbDouble &planeB, glbDouble &planeC, glbDouble &planeD);
		// 裁剪对象	
		void Clip(CGlbGlobeObject* obj, glbBool enableClip = true);		
		// 裁剪要素图层
		void Clip(CGlbGlobeLayer* layer, glbBool enableClip = true);
	private:
		osg::ref_ptr<osg::ClipNode> mpr_clipNode;
		osg::Vec3d                                 mpr_offsetPos;
		glbInt32                                      mpr_clipID;//不设置id,clipNode与节点直接设置clipPlane有区别
		bool                                             mpr_markOffsetPos;
	};
}