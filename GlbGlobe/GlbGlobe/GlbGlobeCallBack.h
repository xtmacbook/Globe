/********************************************************************
  * Copyright (c) 2014 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    CGlbGlobeCallBack.h
  * @brief   Globe�����ڵ�ص���ͷ�ļ�
  *
  * �����������CGlbGlobeCallBack���class,�����޸ĳ����ڵ����еĽڵ�
  *
  * @version 1.0
  * @author  ����
  * @date    2014-5-12 16:40
*********************************************************************/
#pragma once
#include "GlbGlobeRObject.h"
#include "GlbGlobeLayer.h"
#include <osg\Node>
#include <osg\NodeCallback>
#include <osg\Geode>

namespace GlbGlobe
{

class GLB_DLLCLASS_EXPORT CGlbGlobeCallBack : public osg::NodeCallback
{
public:
	/**
	 * @brief Globe�����ڵ��޸�������
	 * @author ����
	 * @date    2014-5-12 17:16
	 * @note Globe�����ڵ��޸�����ڲ���
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeCallBackTask : public CGlbReference
	{
		friend class CGlbGlobeCallBack;
	public:	
		 /**
		  * @brief �ڵ��޸������캯��
		  * @param [in] robj �ɻ��ƶ���
		  * @param [in] node ����osg�ڵ�
		  * @param [in] size ����ռ�õ��ڴ��С
		  * @param [in] compileTime node����Ϊ��ʾ�б���Ҫ��ʱ�����ֵ
		  * @return ��
		  */
		CGlbGlobeCallBackTask(CGlbGlobeRObject* robj, osg::Node* node=NULL, glbInt32 size=0, glbDouble compileTime=0.0);
		 /**
		  * @brief �ڵ��滻�����캯��
		  * @param [in] robj �ɻ��ƶ���
		  * @param [in] origChild ԭ�ڵ�
		  * @param [in] newChild  ��osg�ڵ�		
		   @param [in] size ����ռ�õ��ڴ��С��
		  * @param [in] compileTime node����Ϊ��ʾ�б���Ҫ��ʱ�����ֵ
		  * @return ��
		  */
		CGlbGlobeCallBackTask(CGlbGlobeRObject* robj,osg::Node* origChild, osg::Node* newChild, glbInt32 size=0, glbDouble compileTime=0.0);
		 /**
		  * @brief �ڵ��޸������캯��
		  * @param [in] lyr ����ͼ��FeatureLayer
		  * @param [in] node ����osg�ڵ�
		  * @param [in] size ����ռ�õ��ڴ��С
		  * @param [in] compileTime node����Ϊ��ʾ�б���Ҫ��ʱ�����ֵ
		  * @return ��
		  */
		CGlbGlobeCallBackTask(osg::Node* parent_node, osg::Node* node=NULL, glbInt32 size=0, glbDouble compileTime=0.0);	
		 /**
		  * @brief �ڵ��޸�������������
		  * @return ��
		  */
		~CGlbGlobeCallBackTask();

		glbref_ptr<CGlbGlobeRObject> mpr_obj;		
		osg::ref_ptr<osg::Node>      mpr_NotObjParentNode;   // �����ڵ��origChild
		osg::ref_ptr<osg::Node>      mpr_node;				 // ������Ҫ�¼ӵĽڵ��newChild
		glbInt32                     mpr_size;
		glbDouble					 mpr_compileTime;
	private:
		glbBool                      mpr_isRemoveTask;		
		glbBool						 mpr_isReplaceTask;
	};

	class GLB_DLLCLASS_EXPORT CGlbGlobeFadeCallBackTask : public CGlbReference
	{
	public:
		CGlbGlobeFadeCallBackTask(osg::Node *node,osg::NodeCallback *callBack = NULL);
		~CGlbGlobeFadeCallBackTask();
		osg::ref_ptr<osg::Node> mpr_node;
		osg::ref_ptr<osg::NodeCallback> mpr_callBack;
	};
	
public:
	/**
	* @brief ���캯��
	* @return ��
	*/
	CGlbGlobeCallBack(void);
	/**
	* @brief ��������
	* @return ��
	*/
	~CGlbGlobeCallBack(void);

	/**
	* @brief ���һ���ҽڵ�����
	* @param [in] task �ҽڵ�����
	* @return ��
	*/
	 void AddHangTask(CGlbGlobeCallBackTask *task);
	/**
	* @brief ���һ��ժ�ڵ�����
	* @param [in] task ժ�ڵ�����
	* @return ��
	*/
	 void AddRemoveTask(CGlbGlobeCallBackTask *task);
	/**
	* @brief ���һ���滻�ڵ�����
	* @param [in] task �滻�ڵ�����
	* @return ��
	*/
	 void AddReplaceTask(CGlbGlobeCallBackTask *task);
	/**
	* @brief �������������У������Ҷ��к�ժ���У�
	* @return ��
	*/
	 void CleanQueue();
	/**
	* @brief ��ȡ��ǰ��ʹ�õ��Դ�����
	* @return ��ʹ�õ��Դ�����
	*/
	 glbInt32 GetUsedDislayMemory(){return mpr_useddisplaymemory;}
    /**
	 * @brief ��ȡ��֡����.
	*/
	 glbInt32 GetHangSize(){return mpr_hangSize;}
	 /**
	* @brief globe�ڵ�ص�ʵ��
	* @param [in] node ������osg�ڵ�
	* @param [in] nv osg�ڵ������	  
	* @return  ��
	*/
	  virtual void operator() (osg::Node* node, osg::NodeVisitor* nv);

	  void AddFadeTask(CGlbGlobeFadeCallBackTask *task);
	  
private:
	/*
	 *    ��������
	 */
	 void SwapQueue();

	 // ceshi
	 // ���㵱ǰ��������Ⱦ�Ķ��� ռ�õ��ڴ�����
	 glbInt64 ComputeTotalUsedMemory(std::map<int, glbref_ptr<CGlbGlobeRObject>> &objs);
private:
	glbInt32                                     mpr_useddisplaymemory;		///<�Ѿ�ʹ�õ��Դ�����
	glbInt32                                     mpr_hangSize;              ///<��֡����.
	std::list<glbref_ptr<CGlbGlobeCallBackTask>> mpr_prepareHangtasks;		///<�������	
	std::list<glbref_ptr<CGlbGlobeCallBackTask>> mpr_hangtasks;		        ///<����
	std::list<glbref_ptr<CGlbGlobeFadeCallBackTask>> mpr_prepareFadetasks;			///<���仺�����
	std::list<glbref_ptr<CGlbGlobeFadeCallBackTask>> mpr_fadetasks;			///<�������
	GlbCriticalSection                           mpr_critical;	            ///<�ٽ���

	// ceshi
	glbDouble									 mpr_maximumTimeAvailableForGLCompile;	///<�����ʱ�����롾Ĭ��5ms��

	//double										 mpr_previous;
	std::map<int, glbref_ptr<CGlbGlobeRObject>>  mpr_loadedObjs; ///��¼��ǰ�ڴ��еĶ���
};
}


