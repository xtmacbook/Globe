/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeObjectExport.h
* @version 1.0
* @author  龙庭友
* @date    2014-6-18 15:30
*********************************************************************/
#pragma once


#ifdef GLBGLOBE_OBJECT_EXPORTS
#define GLB_OBJECTDLL_CLASSEXPORT __declspec(dllexport) 
#else 
#define GLB_OBJECTDLL_CLASSEXPORT __declspec(dllimport) 
#endif
