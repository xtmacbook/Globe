/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeTaskManager.h
* @brief   ��������� ͷ�ļ�
*
* �����������CGlbGlobeTaskManager���class ,���������
*
* @version 1.0
* @author  ����
* @date    2014-5-12 14:18
*********************************************************************/
#include "glbref_ptr.h"
#include "IGlbGlobeTask.h"
#include <list>
#include <OpenThreads/Thread>
#include <OpenThreads/Block>
#include <osg/GraphicsContext>
#include <osgUtil/GLObjectsVisitor>
#include "GlbGlobeRObject.h"

#pragma once
namespace GlbGlobe
{
	class CGlbNodeCompileVisitor : public osg::NodeVisitor
	{
	public:

		CGlbNodeCompileVisitor();
		~CGlbNodeCompileVisitor();

		virtual void apply(osg::Node& node);
		virtual void apply(osg::Geode& node);

		virtual void apply(osg::Drawable& drawable);
		virtual void apply(osg::StateSet& stateset);
	public:
		osg::RenderInfo _renderInfo;
		osg::ref_ptr<osg::PixelBufferObject> _pbo;

	};

	class GLB_DLLCLASS_EXPORT CGlbGlobeTaskManager : public CGlbReference
	{
	private:
		/** 
		* @brief �����������߳���
		* @author ����
		* @date    2014-5-12 15:10
		* @note ��������ڲ���,����Ӷ��������б���ȡ����ִ��
		*/
		class CGlbGlobeTaskThread : public OpenThreads::Thread
		{
		public:
			/**
			* @brief �����������̹߳��캯��
			* @return ��
			*/
			CGlbGlobeTaskThread (CGlbGlobeTaskManager* taskManager);
			virtual ~CGlbGlobeTaskThread (void);
			/**
			* @brief �߳��������
			* @return ��
			*/
			virtual void run();
			/**
			* @brief �߳�ȡ�����
			* @return ��
			*/
			virtual int cancel();		
			/**
			* @brief �߳���ͣ���
			* @return ��
			*/
			void pause();
			/**
			* @brief �߳�ȡ����ͣ���
			* @return ��
			*/
			void resume();
		private:
			CGlbGlobeTaskManager* mpr_taskmanager;	
			glbBool mpr_bDone;
			glbBool mpr_pause;
			glbBool mpr_ispaused;
		};
		/** 
		* @brief ���ο��������߳���
		* @author ����
		* @date    2014-5-12 15:10
		* @note ��������ڲ���,����ӵ��ο������б���ȡ����ִ��
		*/
		class CGlbGlobeTerrainTaskThread : public OpenThreads::Thread
		{
		public:
			/**
			* @brief ���ο��������̹߳��캯��
			* @return ��
			*/
			CGlbGlobeTerrainTaskThread (CGlbGlobeTaskManager* taskManager);
			virtual ~CGlbGlobeTerrainTaskThread (void);
			/**
			* @brief �߳��������
			* @return ��
			*/
			virtual void run();
			/**
			* @brief �߳�ȡ�����
			* @return ��
			*/
			virtual int cancel();
			/**
			* @brief �߳���ͣ���
			* @return ��
			*/
			void pause();
			/**
			* @brief �߳�ȡ����ͣ���
			* @return ��
			*/
			void resume();
		private:
			CGlbGlobeTaskManager* mpr_taskmanager;	
			glbBool mpr_bDone;
			glbBool mpr_pause;
			glbBool mpr_ispaused;
		};
	public:
		/**
		* @brief globe����������๹�캯��	
		* @return ��
		*/
		CGlbGlobeTaskManager();
		/**
		* @brief globe�������������������
		* @return ��
		*/
		~CGlbGlobeTaskManager(void);

		/**
		* @brief globe������������ʼ������	
		* @param gc ������ʾ�б�ʹ�õ���Ⱦ������Ϣ
		* @return ��
		*/	  
		void init(osg::GraphicsContext* compileContext);

		/**
		* @brief ���һ������������
		* @return ��
		*/
		void  AddTask(IGlbGlobeTask* task);

		/**
		* @brief ���һ�����ο鴦������
		* @return ��
		*/	
		void  AddTerrainTask(IGlbGlobeTask* task);
		/**
		* @brief ��ȡһ������������
		* @return ���������δ����������
		-   û�������򷵻�NULL
		*/
		glbref_ptr<IGlbGlobeTask>  GetTask();
		/**
		* @brief ��ȡһ�����ο鴦������
		* @return ���������δ����������
		-   û�������򷵻�NULL
		*/	
		glbref_ptr<IGlbGlobeTask>  GetTerrainTask();
		/**
		* @brief ������ж���������
		* @return ��
		*/
		void  CleanObjectTasks();
		/**
		* @brief ��ȡ����������Ŀ
		* @return ������Ŀ
		*/
		glbInt32 GetObjectTaskNum();
		/**
		* @brief ɾ�����μ�������
		* @return  �ɹ�����true ��ʧ�ܷ���false
		*/
		glbBool	ReomveTerrainTask(IGlbGlobeTask* task);	
		/**
		* @brief �������е������߳�
		* @return  
		*/
		void DestoryAllTaskThreads();
		/**
		* @brief ��ͣ���ָ������������߳�
		* @return  
		*/
		void Pause(glbBool bPause);
	private:
		std::list<glbref_ptr<IGlbGlobeTask>> mpr_objtasks;					//��������������
		std::list<glbref_ptr<IGlbGlobeTask>> mpr_terraintasks;				//���ο�����������
		GlbCriticalSection		mpr_objtask_critical;						//���������ٽ���
		GlbCriticalSection		mpr_terraintask_critical;					//���ο������ٽ���

		CGlbGlobeTerrainTaskThread* mpr_tt_thread;							//���ο���������߳�
		//std::vector<CGlbGlobeTerrainTaskThread*> mpr_tt_thread;			//���ο���������߳�
		std::vector<CGlbGlobeTaskThread*> mpr_tasks_thread;					//ҵ���������߳�

		glbBool					mpr_isInit;									//��־�Ѿ���ʼ��
	public:
		//osg::ref_ptr<osgUtil::IncrementalCompileOperation> mpr_iCO;		//��������
		osg::ref_ptr<osg::GraphicsContext> mpr_compileContext;				//������context
		GlbCriticalSection		mpr_compile_critical;						//�����ٽ���

		OpenThreads::Block      mpr_tt_thread_block;				///<���ο�����߳���������
		OpenThreads::Block		mpr_tasks_thread_block;		///<��������߳���������
	};
}