/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeAutoTransform.h
* @brief   ������������ʵ��AutoTransform
* @version 1.0
* @author  ChenPeng
* @date    2014-8-21 11:55
*********************************************************************/
#pragma once
#include "GlbGlobeSymbolExport.h"
#include <osg/AutoTransform>
#include "CGlbGlobe.h"
using namespace osg;
namespace GlbGlobe
{
	class GLB_SYMBOLDLL_CLASSEXPORT GlbGlobeAutoTransform : public osg::AutoTransform
	{
	public:
		GlbGlobeAutoTransform(CGlbGlobe *globe = NULL);
		~GlbGlobeAutoTransform(void);

		virtual void accept(NodeVisitor& nv);

	private:
		CGlbGlobe *mpr_globe;
	};
}