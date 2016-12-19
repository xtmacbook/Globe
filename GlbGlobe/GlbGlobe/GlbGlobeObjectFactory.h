/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeObjectFactory.h
* @brief   ���󹤳�ͷ�ļ����ĵ�����CGlbGlobeObject��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-5 14:20
*********************************************************************/

#pragma once
#include "GlbGlobeObject.h"
#include "GlbWString.h"

using namespace GlbGlobe;

class GLB_DLLCLASS_EXPORT IGlbGlobeObjectProvider : public CGlbReference
{
public:
	IGlbGlobeObjectProvider();
	virtual ~IGlbGlobeObjectProvider();
	virtual CGlbGlobeObject * Create()=0;
};

class GLB_DLLCLASS_EXPORT CGlbGlobeObjectFactory  : public CGlbReference
{
public:
	CGlbGlobeObjectFactory(void);
	~CGlbGlobeObjectFactory(void);
	
	static CGlbGlobeObjectFactory* GetInstance();

	void Register(IGlbGlobeObjectProvider* creator,glbWChar* objclassname);
	void UnRegister(glbWChar* objclassname);
	//IGlbGlobeObjectProvider* GetObjectCreator(int type);
	CGlbGlobeObject* GetObject(glbWChar* objclassname);
public:
	std::map<CGlbWString,IGlbGlobeObjectProvider*> mpr_providers;
};

//extern CGlbGlobeObjectFactory* g_objectFactory;
extern CGlbGlobeObjectFactory g_objectFactory;


