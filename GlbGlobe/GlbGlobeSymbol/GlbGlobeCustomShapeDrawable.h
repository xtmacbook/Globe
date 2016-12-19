/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeCustomShapeDrawable.h
* @brief   �Զ��弸��������ļ����ĵ������Զ��弸�������
* @version 1.0
* @author  ChenPeng
* @date    2014-5-14 15:00
*********************************************************************/
#pragma once
#include <osg/ShapeDrawable>
using namespace osg;

namespace GlbGlobe
{
	class GlobeShapeDrawable : public ShapeDrawable
	{
	public:
		GlobeShapeDrawable(Shape* shape, TessellationHints* hints=0);
		~GlobeShapeDrawable();
		virtual void drawImplementation(RenderInfo& renderInfo) const;
		virtual void accept(PrimitiveFunctor& pf) const;
		virtual osg::BoundingBox computeBound() const;
	};
}