/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeObjectExport.h
* @version 1.0
* @author  ��ͥ��
* @date    2014-6-18 15:30
*********************************************************************/
#pragma once


#ifdef GLBGLOBE_OBJECT_EXPORTS
#define GLB_OBJECTDLL_CLASSEXPORT __declspec(dllexport) 
#else 
#define GLB_OBJECTDLL_CLASSEXPORT __declspec(dllimport) 
#endif
