#include "StdAfx.h"
#include "GlbGlobeCallBack.h"
#include <osg/Group>
#include "GlbGlobeMemCtrl.h"


std::string GetSystemTimeExt()    
{    
	time_t tNowTime;    
	time(&tNowTime);    
	tm* tLocalTime = localtime(&tNowTime);    
	char szTime[30] = {'\0'};    
	strftime(szTime, 30, "[%Y-%m-%d %H:%M:%S] ", tLocalTime);    
	std::string strTime = szTime;    
	return strTime;    
}    

//*   �����Կ���֡�����������   */
#define FRAME_DEAL_SIZE 1000000

using namespace GlbGlobe;
CGlbGlobeCallBack::CGlbGlobeCallBackTask::CGlbGlobeCallBackTask(CGlbGlobeRObject* robj, osg::Node* parentNode, glbInt32 size,glbDouble compileTime):
															mpr_obj(robj),
															mpr_node(parentNode),
															mpr_size(size),
															mpr_compileTime(compileTime)
{
	mpr_obj = robj;
	mpr_node = parentNode;
	mpr_size = size;
	mpr_compileTime = compileTime;
	mpr_NotObjParentNode = NULL;
	mpr_isRemoveTask = false;
	mpr_isReplaceTask = false;
}	

CGlbGlobeCallBack::CGlbGlobeCallBackTask::CGlbGlobeCallBackTask(CGlbGlobeRObject* robj,osg::Node* origChild, osg::Node* newChild, glbInt32 size, glbDouble compileTime)
{
	mpr_obj = robj;
	mpr_NotObjParentNode = origChild;
	mpr_node = newChild;
	mpr_compileTime = compileTime;
	mpr_size = size;
	mpr_isRemoveTask = false;
	mpr_isReplaceTask = false;
}

CGlbGlobeCallBack::CGlbGlobeCallBackTask::CGlbGlobeCallBackTask(osg::Node* parent_node, osg::Node* node, glbInt32 size,glbDouble compileTime)
{
	mpr_NotObjParentNode = parent_node;
	mpr_node = node;
	mpr_size = size;
	mpr_compileTime = compileTime;
	mpr_obj = NULL;
	mpr_isRemoveTask = false;
	mpr_isReplaceTask = false;
}

CGlbGlobeCallBack::CGlbGlobeCallBackTask::~CGlbGlobeCallBackTask()
{
	mpr_obj = NULL;
	mpr_node = NULL;
	mpr_NotObjParentNode = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////
CGlbGlobeCallBack::CGlbGlobeCallBack(void)
{
	mpr_useddisplaymemory = 0;		
	mpr_maximumTimeAvailableForGLCompile = 0.005; //Ԥ��5�������ʱ��
	mpr_critical.SetName(L"globe_callback");
}


CGlbGlobeCallBack::~CGlbGlobeCallBack(void)
{
	
}

void CGlbGlobeCallBack::operator() (osg::Node* node, osg::NodeVisitor* nv)
{
	glbDouble tm = CGlbGlobeMemCtrl::GetInstance()->GetTimeToCompileCost();
	if (tm >= mpr_maximumTimeAvailableForGLCompile)
	{// ������δ���ʱ���Ѿ�ռ���˲�����mpr_maximumTimeAvailableForGLCompileʱ�����ȴ���һ֡�ٴ������
		traverse(node,nv);	

		CGlbGlobeMemCtrl::GetInstance()->ClearTimeToCompileCost();
		return;
	}
	
	if ( (mpr_hangtasks.size()<=0 && mpr_prepareHangtasks.size()>0) ||
		(mpr_fadetasks.size()<=0 && mpr_prepareFadetasks.size()>0)	)
	{// ȫ�����������ܽ����µ�
		SwapQueue();
	}

	//double st = nv->getFrameStamp()->getSimulationTime();
	//double dt = st - mpr_previous;
	//mpr_previous = st;
	//if (mpr_hangtasks.size()>0)
	//{
	//	char buff[128];
	//	sprintf(buff,"******fram %ld begin",nv->getFrameStamp()->getFrameNumber());
	//	LOG(buff);
	//}
	while(mpr_hangtasks.size()>0)
	{		
		glbref_ptr<CGlbGlobeCallBackTask> task = mpr_hangtasks.front();
		mpr_hangtasks.pop_front();
		if (task && task->mpr_obj)
		{
			osg::Group* parentNode = dynamic_cast<osg::Group*>(task->mpr_obj->GetParentNode());				
			if (parentNode)
			{
				if (task->mpr_isReplaceTask == true)
				{// �滻�ڵ�����
					parentNode->removeChild(task->mpr_NotObjParentNode.get());
					parentNode->addChild(task->mpr_node.get());
					task->mpr_obj->DecreaseHangTaskNum();
					
					//if (true)
					//{// ����			
					//	WCHAR wBuff[128];
					//	//osg::Node* node = task->mpr_node->asGroup()->getChild(0)->asGroup()->getChild(0);
					//	CGlbString nodeName = task->mpr_node->getName();
					//	CGlbString _time = GetSystemTimeExt();
					//	swprintf(wBuff,L"%s %s CallBack replace node +-.\n",_time.ToWString().c_str(),nodeName.ToWString().c_str());
					//	OutputDebugString(wBuff);
					//}

					//mpr_loadedObjs[task->mpr_obj->GetId()] = task->mpr_obj;
				}
				else
				{
					if(task->mpr_isRemoveTask == false)
					{// ��	
						if (task->mpr_node->getNumParents()<=0)
						{// ����û�и��ڵ㣬���������ȥ
							parentNode->addChild(task->mpr_node.get());		
							task->mpr_obj->DecreaseHangTaskNum();	

							tm+=task->mpr_compileTime;

							//mpr_loadedObjs[task->mpr_obj->GetId()] = task->mpr_obj;

							//{
							//	osg::Node* nd = task->mpr_node->asGroup()->getChild(0)->asGroup()->getChild(0);
							//	LOG(nd->getName().c_str());
							//	LOG(task->mpr_compileTime);						
							//}
							if (tm >= mpr_maximumTimeAvailableForGLCompile)
							{// ������δ���ʱ���Ѿ�ռ���˲�����mpr_maximumTimeAvailableForGLCompileʱ�����ȴ���һ֡�ٴ������
								traverse(node,nv);	
								CGlbGlobeMemCtrl::GetInstance()->ClearTimeToCompileCost();
								return;
							}						
							else
							{
								CGlbGlobeMemCtrl::GetInstance()->IncrementTimeToCompileCost(task->mpr_compileTime);
							}
						}
						else
						{
							mpr_useddisplaymemory -= task->mpr_size;
							task->mpr_obj->DecreaseHangTaskNum();
						}
					}
					else
					{// ժ
						if (parentNode->removeChild(task->mpr_node.get()) == false)
						{
							mpr_useddisplaymemory += task->mpr_size;		

							//std::map<int, glbref_ptr<CGlbGlobeRObject>>::iterator itrFind = mpr_loadedObjs.find(task->mpr_obj->GetId());
							//if (itrFind != mpr_loadedObjs.end())
							//{// �ҵ�
							//	mpr_loadedObjs.erase(itrFind);
							//}
						}
						//else
						//{
						//	task->mpr_node = NULL;
						//	task->mpr_obj->mpt_node = NULL;
						//}
					}
				}			
			}							
		}
		else if (task && task->mpr_NotObjParentNode)
		{// ��������
			if(task->mpr_isRemoveTask == false)
			{
				if(task->mpr_node->getName() == "slaveCamera")
				{
					osg::Group *group = task->mpr_NotObjParentNode->asGroup();
					if(group->getNumChildren() > 2)
						group->insertChild(1,task->mpr_node.get());
					else
						group->addChild(task->mpr_node.get());
				}
				else
					task->mpr_NotObjParentNode->asGroup()->addChild(task->mpr_node.get());						
			}else{
				if(task->mpr_NotObjParentNode->asGroup()->removeChild(task->mpr_node.get()) == false)
				{
					mpr_useddisplaymemory  += task->mpr_size;					
				}
			}
		}							
	}		
	if (tm>0)// frame������������ʱ��������
		CGlbGlobeMemCtrl::GetInstance()->ClearTimeToCompileCost();
	while(mpr_fadetasks.size() > 0)
	{
		glbref_ptr<CGlbGlobeFadeCallBackTask> task = mpr_fadetasks.front();
		mpr_fadetasks.pop_front();
		if(task && task->mpr_node.valid())
		{
			if(task->mpr_callBack.valid())
				task->mpr_node->removeUpdateCallback(task->mpr_callBack.get());
			else
			{
				osg::ref_ptr<osg::NodeCallback> nb = task->mpr_node->getUpdateCallback();
				if(nb.valid())
					task->mpr_node->removeUpdateCallback(nb.get());
			}
		}
	}
	//// �����ڴ�ռ���Ƿ�׼ȷ -- ���׼ȷ ok 2016.3.30 malin
	//glbInt64 hasUsedMemory = CGlbGlobeMemCtrl::GetInstance()->GetUsedMem();
	//if(hasUsedMemory > g_memThreshold)
	//{
	//	if (0)
	//	{
	//		glbInt64 usedMem = ComputeTotalUsedMemory(mpr_loadedObjs);
	//		if (hasUsedMemory > usedMem)
	//		{
	//			int error = 1;
	//		}
	//	}
	//}

	traverse(node,nv);		
}
/*
*  ��Ӷ��������
*
*  Dispatcher�߳�--->�����AddToScene--->AddHangTask()
*  Globe��Update�߳�-->FeatuerLayer::Update()--->AddHangTask()
*  ����/�������߳�-->RObject::DirectDraw()---->RObject::AddToScene--->AddHangTask()
*  
*  ���� ��frame�߳� �����⡿
*/
void CGlbGlobeCallBack::AddHangTask(CGlbGlobeCallBackTask *task)
{
	if(task == NULL)return;
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	task->mpr_isRemoveTask = false;
	mpr_hangSize += task->mpr_size;
	mpr_prepareHangtasks.push_back(task);
}
void CGlbGlobeCallBack::AddRemoveTask(CGlbGlobeCallBackTask *task)
{
	if(task == NULL)return;
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	task->mpr_isRemoveTask = true;
	mpr_prepareHangtasks.push_back(task);	
}
 void CGlbGlobeCallBack::AddReplaceTask(CGlbGlobeCallBackTask *task)
 {
	 if(task == NULL)return;
	 GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	 task->mpr_isReplaceTask = true;
	 task->mpr_isRemoveTask = false;
	 mpr_prepareHangtasks.push_back(task);	
 }
void CGlbGlobeCallBack::SwapQueue()
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::list<glbref_ptr<CGlbGlobeCallBackTask>>::iterator itr = mpr_prepareHangtasks.begin();	
	for(itr;itr != mpr_prepareHangtasks.end();itr++)
	{
		mpr_hangtasks.push_back((*itr).get());
		if((*itr)->mpr_isRemoveTask)
		{
			mpr_useddisplaymemory -= (*itr)->mpr_size;
		}
		else
		{
			mpr_useddisplaymemory += (*itr)->mpr_size;
		}
	}	
	mpr_prepareHangtasks.clear();
	mpr_hangSize = 0;

	// ����fadecallbackTask
	std::list<glbref_ptr<CGlbGlobeFadeCallBackTask>>::iterator itrFade = mpr_prepareFadetasks.begin();
	for(itrFade;itrFade != mpr_prepareFadetasks.end();itrFade++)
	{
		mpr_fadetasks.push_back((*itrFade).get());		
	}	
	mpr_prepareFadetasks.clear();
}
/*
*    ��ն���
*    Dispatcher::Update()--->CleanQueue();
*/
void CGlbGlobeCallBack::CleanQueue()
{
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	std::list<glbref_ptr<CGlbGlobeCallBackTask>> tempList;
	std::list<glbref_ptr<CGlbGlobeCallBackTask>>::iterator itr = mpr_prepareHangtasks.begin();
	while(itr != mpr_prepareHangtasks.end())
	{
		if ((*itr)->mpr_obj)
		{
			if ((*itr)->mpr_isReplaceTask == true)// �滻
				(*itr)->mpr_obj->DecreaseHangTaskNum();
			else
			{
				if ((*itr)->mpr_isRemoveTask == false)
				{				
					(*itr)->mpr_obj->DecreaseHangTaskNum();
				}
				else
					tempList.push_back((*itr).get());
			}
		}
		else
		{
			//����ͼ����ɾ��dispatch����֮�������
			tempList.push_back((*itr).get());
		}
		itr++;		
	}
	mpr_prepareHangtasks.clear();
	mpr_prepareHangtasks = tempList;
	mpr_hangSize = 0;
	/*itr = mpr_hangtasks.begin();
	while(itr != mpr_hangtasks.end())
	{
	if ((*itr)->mpr_obj)
	{
	if ((*itr)->mpr_isRemoveTask == false)
	(*itr)->mpr_obj->DecreaseHangTaskNum();			
	else
	tempList.push_back((*itr).get());
	}		
	itr++;	
	}
	mpr_hangtasks.clear();
	mpr_hangtasks = tempList;
	mpr_hangSize  = 0;*/
    /*
	*   ժ���в����
	*   �� RObject->RemoveFromScene(true)�������ڴ桿����£�
	*           mpt_node = NULL��ͬʱ�����ժ����
	*           ���ǡ�������ժ����,��mpt_node ����Զ���ڳ������ϡ�
	*/

}



CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask::CGlbGlobeFadeCallBackTask( osg::Node *node,osg::NodeCallback *callBack )
{
	mpr_node = node;
	mpr_callBack = callBack;
}

CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask::~CGlbGlobeFadeCallBackTask()
{

}

void CGlbGlobeCallBack::AddFadeTask( CGlbGlobeFadeCallBackTask *task )
{
	if(task == NULL)return;
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	mpr_prepareFadetasks.push_back(task);
}

glbInt64 CGlbGlobeCallBack::ComputeTotalUsedMemory(std::map<int, glbref_ptr<CGlbGlobeRObject>> &objs)
{
	glbInt64 _memoryUsed = 0;
	std::map<int, glbref_ptr<CGlbGlobeRObject>>::iterator itr = objs.begin();
	for(itr; itr!=objs.end(); itr++)
	{
		_memoryUsed += itr->second->GetOsgNodeSize();
	}

	return _memoryUsed;
}