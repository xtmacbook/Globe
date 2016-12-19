#include "StdAfx.h"
#include "GlbGlobeTaskManager.h"
#include <osgUtil/IncrementalCompileOperation>
#include <osgViewer/api/Win32/GraphicsHandleWin32>
#include <osg/GLObjects>
#include <osgdb/ReadFile>
#include <osgUtil/GLObjectsVisitor>
#include "GlbNodeIsCompiledVisitor.h"

#include "GlbGlobeMemCtrl.h"
//#define MAX_TASKTHREAD_NUM  1 //3 ///<定义对象任务处理线程最大数量

using namespace GlbGlobe;

CGlbNodeCompileVisitor::CGlbNodeCompileVisitor():
osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)	
{

}

CGlbNodeCompileVisitor::~CGlbNodeCompileVisitor()
{

}
void CGlbNodeCompileVisitor::apply(osg::Node& node)
{
	if (node.getStateSet())
	{
		apply(*(node.getStateSet()));
	}

	traverse(node);
}

void CGlbNodeCompileVisitor::apply(osg::Geode& node)
{
	if (node.getStateSet())
	{
		apply(*(node.getStateSet()));
	}

	for(unsigned int i=0;i<node.getNumDrawables();++i)
	{
		osg::Drawable* drawable = node.getDrawable(i);
		if (drawable)
		{
			apply(*drawable);
			if (drawable->getStateSet())
			{
				apply(*(drawable->getStateSet()));
			}
		}
	}
}

void CGlbNodeCompileVisitor::apply(osg::Drawable& drawable)
{
	if (_renderInfo.getState() &&
		(drawable.getUseDisplayList() || drawable.getUseVertexBufferObjects()))
	{
		drawable.compileGLObjects(_renderInfo);
	}	
}

void CGlbNodeCompileVisitor::apply(osg::StateSet& stateset)
{		
	if (_renderInfo.getState())
	{
		stateset.compileGLObjects(*_renderInfo.getState());	
	}

	//const osg::StateSet::TextureAttributeList& tal = stateset.getTextureAttributeList();
	//for(osg::StateSet::TextureAttributeList::const_iterator itr = tal.begin();
	//	itr != tal.end();
	//	++itr)
	//{
	//	const osg::StateSet::AttributeList& al = *itr;
	//	osg::StateAttribute::TypeMemberPair tmp(osg::StateAttribute::TEXTURE,0);
	//	osg::StateSet::AttributeList::const_iterator texItr = al.find(tmp);
	//	if (texItr != al.end())
	//	{
	//		osg::Texture* texture = dynamic_cast<osg::Texture*>(texItr->second.first.get());
	//		if (texture)
	//		{
	//			texture->compileGLObjects(*_renderInfo.getState());				
	//		}
	//	}
	//}
}

//////////////////////////////////////////////////////////////////////////

CGlbGlobeTaskManager::CGlbGlobeTaskThread::CGlbGlobeTaskThread (CGlbGlobeTaskManager* taskManager)
{
	mpr_taskmanager = taskManager;
	mpr_bDone = true;
	mpr_pause    = false;
	mpr_ispaused = false;
}
CGlbGlobeTaskManager::CGlbGlobeTaskThread::~CGlbGlobeTaskThread ()
{

}
void CGlbGlobeTaskManager::CGlbGlobeTaskThread::run()
{
	//bool res = mpr_taskmanager->mpr_compileContext->realize();
	while(mpr_bDone)
	{
		if (mpr_pause)
		{// 暂停 0.1秒  微秒 （1秒==1百万微秒）
			mpr_ispaused = true;
			OpenThreads::Thread::microSleep(100);
			continue;
		}

		//mpr_taskmanager->mpr_tasks_thread_block.block();

		glbref_ptr<IGlbGlobeTask> task = mpr_taskmanager->GetTask();
		if(task)
		{
			CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*> (task->getObject());
			bool bToCompiling = false;
			if (robj)
				bToCompiling = robj->IsNeedToPreCompile();
			if(g_isMemCtrl)
			{
				glbInt64 hasUsedMem = CGlbGlobeMemCtrl::GetInstance()->GetUsedMem();
				if(hasUsedMem >= g_memThreshold)
				{
					/*
					*    尽量避免 震荡
					*/					
					if (robj)
						robj->ResetLoadData();
				}else{
					task->doRequest();					
				}
			}else{
				task->doRequest();				
			}			

			if (bToCompiling && mpr_taskmanager && mpr_taskmanager->mpr_compileContext.valid())
			{// 创建osg::Operation添加到mpr_iCO中进行显示列表预编译				
				robj->SetIsPreCompiled(false);
				osg::Node* objNode = robj->GetOsgNode();
				if (objNode)
				{	
					GlbScopedLock<GlbCriticalSection> lock(mpr_taskmanager->mpr_compile_critical);	
					// 必须用osg的makeCurrent,用opengl的wglMakeCurrent会有问题的！！！！2016.12.5
					mpr_taskmanager->mpr_compileContext->makeCurrent();

					//osgViewer::GraphicsHandleWin32* graphicsHandleWin32 = dynamic_cast<osgViewer::GraphicsHandleWin32*>(mpr_taskmanager->mpr_compileContext.get());
					//if (graphicsHandleWin32)
					//{
					//	HDC _hdc = graphicsHandleWin32->getHDC();
					//	HGLRC _hglrc = graphicsHandleWin32->getWGLContext();

					//	//LOG("***wglMakeCurrent(_hdc, _hglrc)");
					//	if (!::wglMakeCurrent(_hdc, _hglrc))
					//	{
					//		//LOG("error : ***wglMakeCurrent(_hdc, _hglrc). ");
					//		GlbLogWOutput(GlbLogTypeEnum::GLB_LOGTYPE_ERR,L"***wglMakeCurrent(_hdc, _hglrc) failed! \n");
					//		continue;
					//	}							

						//osg::RenderInfo renderInfo;
						//renderInfo.setState(mpr_taskmanager->mpr_compileContext->getState());
						//CGlbNodeCompileVisitor nc;
						//nc._renderInfo = renderInfo;
						//objNode->accept(nc);
							
						osgUtil::GLObjectsVisitor glov;
						glov.setState(mpr_taskmanager->mpr_compileContext->getState());
						objNode->accept(glov);

						//{// 验证是否显示列表生成完毕+纹理obj生成了
							//CGlbNodeIsCompiledVisitor nic;
							//objNode->accept(nic);
							//glbBool isCompiled = nic.IsCompiled();
							//if (isCompiled==false)
							//{
							//	int errooroo = 1;
							//}

						//}

						mpr_taskmanager->mpr_compileContext->releaseContext();


						//LOG("wglMakeCurrent(_hdc, NULL)");						
						//if (!::wglMakeCurrent(NULL, NULL))												
						//{
						//	//LOG("error : wglMakeCurrent(NULL, NULL). ");
						//	GlbLogWOutput(GlbLogTypeEnum::GLB_LOGTYPE_ERR,L"***wglMakeCurrent(NULL, NULL) failed! \n");
						//	continue;
						//}
					//}					
				}
				// 设置编译完成标志
				robj->SetIsPreCompiled(true);

				if (robj)
				{// 预先计算bound，减少updatetravel中的计算时间
					osg::Node* objNode = robj->GetOsgNode();
					if (objNode)
						objNode->getBound();
				}
			}			

			task=NULL;
		}
		else
		{
			//mpr_taskmanager->mpr_tasks_thread_block.reset();

			OpenThreads::Thread::microSleep(5);  // 单位： 毫秒（千分之一秒）			
		}
	}

	OutputDebugString(L"	WARING : CGlbGlobeTaskManager::TaskThread exit. \n");
}
int CGlbGlobeTaskManager::CGlbGlobeTaskThread::cancel()
{
	mpr_bDone = false;	
	int res = OpenThreads::Thread::setCancelModeAsynchronous();//cancelMode==1	
	Sleep(100);
	//OpenThreads::Thread::microSleep(100);
	res = OpenThreads::Thread::cancel();			
	return 1;
}

void CGlbGlobeTaskManager::CGlbGlobeTaskThread::pause()
{
	mpr_pause = true;
	while(!mpr_ispaused)
	{
		OpenThreads::Thread::microSleep(5);
	}
}

void CGlbGlobeTaskManager::CGlbGlobeTaskThread::resume()
{
	mpr_pause    = false;
	mpr_ispaused = false;
}

CGlbGlobeTaskManager::CGlbGlobeTerrainTaskThread::CGlbGlobeTerrainTaskThread (CGlbGlobeTaskManager* taskManager)
{
	mpr_taskmanager = taskManager;
	mpr_bDone = true;
	mpr_pause    = false;
	mpr_ispaused = false;
}
CGlbGlobeTaskManager::CGlbGlobeTerrainTaskThread::~CGlbGlobeTerrainTaskThread ()
{

}
void CGlbGlobeTaskManager::CGlbGlobeTerrainTaskThread::run()
{
	while(mpr_bDone)
	{
		if (mpr_pause)
		{// 暂停 0.1秒  微秒 （1秒==1百万微秒）
			mpr_ispaused = true;
			OpenThreads::Thread::microSleep(100);
			continue;
		}

		bool isSucc = false;
		isSucc = mpr_taskmanager->mpr_tt_thread_block.block();

		glbref_ptr<IGlbGlobeTask> task = mpr_taskmanager->GetTerrainTask();
		if(task)
		{
			task->doRequest();					
			task=NULL;
		}
		else
		{
			mpr_taskmanager->mpr_tt_thread_block.reset();
			OpenThreads::Thread::microSleep(5);
		}
	}
	OutputDebugString(L"	WARING: CGlbGlobeTaskManager::CGlbGlobeTerrainTaskThread exit. \n");
}
int CGlbGlobeTaskManager::CGlbGlobeTerrainTaskThread::cancel()
{
	mpr_bDone = false;
	int res = OpenThreads::Thread::setCancelModeAsynchronous();//cancelMode==1		
	Sleep(100);
	//OpenThreads::Thread::microSleep(100);
	OpenThreads::Thread::cancel();			
	return 1;
}
void CGlbGlobeTaskManager::CGlbGlobeTerrainTaskThread::pause()
{
	mpr_pause = true;
	while(!mpr_ispaused)
	{
		OpenThreads::Thread::microSleep(5);
	}
}

void CGlbGlobeTaskManager::CGlbGlobeTerrainTaskThread::resume()
{
	mpr_pause    = false;
	mpr_ispaused = false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

CGlbGlobeTaskManager::CGlbGlobeTaskManager()
{
	mpr_objtask_critical.SetName(L"task_objtask");
	mpr_terraintask_critical.SetName(L"task_terraintask");
	mpr_compile_critical.SetName(L"task_compile");
	mpr_isInit = false;
}

CGlbGlobeTaskManager::~CGlbGlobeTaskManager(void)
{
	OutputDebugString(L"	DestoryAllTaskThreads begin.\n");
	// 清除任务队列
	//CleanObjectTasks();
	// 销毁所有任务线程
	DestoryAllTaskThreads();
	OutputDebugString(L"	DestoryAllTaskThreads end.\n");
}

void CGlbGlobeTaskManager::init(osg::GraphicsContext* compileContext)
{
	if (mpr_isInit)
	// 如果已经初始化了，则直接返回
		return;

	mpr_compileContext = compileContext;
	
	mpr_tt_thread = new CGlbGlobeTerrainTaskThread(this);
	mpr_tt_thread->startThread();		
	for(int i=0;i<MAX_TASKTHREAD_NUM;i++)
	{
		CGlbGlobeTaskThread* tt = new CGlbGlobeTaskThread(this); 		
		tt->startThread();
		mpr_tasks_thread.push_back(tt);
	}
	mpr_isInit = true;
}

void CGlbGlobeTaskManager::AddTask(IGlbGlobeTask* task)
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_objtask_critical);
	mpr_objtasks.push_back(task);

	// 取消任务线程的阻塞		
	//mpr_tasks_thread_block.release();		
}
void CGlbGlobeTaskManager::AddTerrainTask(IGlbGlobeTask* task)
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_terraintask_critical);
	mpr_terraintasks.push_back(task);
	// 取消阻塞
	mpr_tt_thread_block.release();
}
glbref_ptr<IGlbGlobeTask> CGlbGlobeTaskManager::GetTask()
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_objtask_critical);
	// 没有任务，返回空
	if (mpr_objtasks.size()<=0)return NULL;

	glbref_ptr<IGlbGlobeTask> p_task = mpr_objtasks.front();
	mpr_objtasks.pop_front();	
	return p_task;
}
glbref_ptr<IGlbGlobeTask> CGlbGlobeTaskManager::GetTerrainTask()
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_terraintask_critical);

	// 没有任务，返回空
	if (mpr_terraintasks.size()<=0)return NULL;

	glbref_ptr<IGlbGlobeTask> p_task = mpr_terraintasks.front();
	mpr_terraintasks.pop_front();
	return p_task;
}
void CGlbGlobeTaskManager::CleanObjectTasks()
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_objtask_critical);
	//std::list<glbref_ptr<IGlbGlobeTask>>::iterator itr;
	//for(itr = mpr_objtasks.begin(); itr != mpr_objtasks.end(); ++itr )
	//{
	//	glbref_ptr<IGlbGlobeTask> p_task = *itr;
	//	if (p_task)
	//	{
	//		CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*> (p_task->getObject());
	//		if (robj)
	//			robj->ResetLoadData();
	//	}
	//	p_task = NULL;
	//}
	//mpr_objtasks.clear();

	while(mpr_objtasks.size()>0)
	{		
		glbref_ptr<IGlbGlobeTask> task = mpr_objtasks.front();
		mpr_objtasks.pop_front();
		if (task)
		{
			CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*> (task->getObject());
			if (robj)
				robj->ResetLoadData();
		}
	}
}

glbInt32 CGlbGlobeTaskManager::GetObjectTaskNum()
{
	return mpr_objtasks.size();
}

void CGlbGlobeTaskManager::Pause(glbBool bPause)
{
	if (bPause)
	{
		OutputDebugString(L"	terraintask thread paused begin. \n");
		mpr_tt_thread_block.release();

		mpr_tt_thread->pause();
		size_t cn = mpr_tasks_thread.size();
		for(size_t i=0;i<cn;i++)
		{
			mpr_tasks_thread[i]->pause();		
		}

		OutputDebugString(L"	terraintask thread paused. \n");
	}
	else
	{
		OutputDebugString(L"	terraintask thread resume begin. \n");
		mpr_tt_thread->resume();
		size_t cn = mpr_tasks_thread.size();
		for(size_t i=0;i<cn;i++)
		{
			mpr_tasks_thread[i]->resume();		
		}

		OutputDebugString(L"	terraintask thread resume. \n");
	}
}

glbBool	CGlbGlobeTaskManager::ReomveTerrainTask(IGlbGlobeTask* task)
{
	glbBool bfind = false;
	GlbScopedLock<GlbCriticalSection> lock(mpr_terraintask_critical);

	std::list<glbref_ptr<IGlbGlobeTask>> newtasks;

	std::list<glbref_ptr<IGlbGlobeTask>>::iterator itr = mpr_terraintasks.begin();
	while (itr!= mpr_terraintasks.end())
	{
		if ((*itr).get()==task)
		{// 找到
			bfind = true;
		}
		else
			newtasks.push_back(*itr);
		++itr;
	}

	if (bfind)
	{
		mpr_terraintasks.clear();
		mpr_terraintasks = newtasks;
	}
	return bfind;
}
void CGlbGlobeTaskManager::DestoryAllTaskThreads()
{
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_objtask_critical);
		mpr_objtasks.clear();
	}

	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_terraintask_critical);
		mpr_terraintasks.clear();
	}

	// 停止线程
	if (mpr_tt_thread)
	{
		mpr_tt_thread_block.release();

		mpr_tt_thread->pause();
		mpr_tt_thread->cancel();
		delete mpr_tt_thread;
		mpr_tt_thread=NULL;

		OutputDebugString(L"	terraintask thread cancel. \n");
	}

	size_t cn = mpr_tasks_thread.size();
	for(size_t i=0;i<cn;i++)
	{
		//mpr_tasks_thread_block.release();

		mpr_tasks_thread[i]->pause();		
		mpr_tasks_thread[i]->cancel();
		delete mpr_tasks_thread[i];
		mpr_tasks_thread[i]=NULL;

		OutputDebugString(L"	task thread cancel. \n");
	}
	mpr_tasks_thread.clear();
}