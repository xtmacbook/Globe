#include "StdAfx.h"
#include "GlbGlobeView.h"
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osgdb/ReaderWriter>
#include "GRKSLib.h"
#define PAGELODSIZE 500000000
using namespace GlbGlobe;

CGlbGlobeViewRegistry CGlbGlobeViewRegistry::mpr_self;

void CGlbGlobeView::myosgviewer::frame(double simulationTime/*=USE_REFERENCE_TIME*/)
{
	osgViewer::Viewer::frame(simulationTime);
	
	//if(mpr_eventhandle)mpr_eventhandle->OnFrame();
}

glbBool CGlbGlobeView::CGlbGlobeMousePickHandle::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	if (ea.getTouchData())
	{
		if (ea.getTouchData()->getNumTouchPoints())
		{
			if (_ga_t0 == NULL)
				_ga_t0 = &ea;
			glbDouble tim = ea.getTime();
			glbBool isOutMouse = false;
			if ( tim - _ga_t0->getTime() > 0.1)
			{
				isOutMouse = true;
				_ga_t0 = &ea;
			}
			if (isOutMouse)
			{
				switch (ea.getTouchData()->get(0).phase)
				{
				case osgGA::GUIEventAdapter::TOUCH_BEGAN:	//push
				case osgGA::GUIEventAdapter::TOUCH_MOVED:	//push
					{
						osg::Vec3d _pushPos(0.0,0.0,0.0);
						//记录选中点经纬高
						glbDouble dx = ea.getTouchData()->get(0).x;
						glbDouble dy = ea.getTouchData()->get(0).y;
						//窗口坐标系(鼠标位置) 转换为 屏幕坐标
						mpr_globe->GetView()->WindowToScene(dx,dy);
						std::vector<glbref_ptr<CGlbGlobeObject>> results;
						std::vector<osg::Vec3d> InterPoints;
						osg::Vec3d pt11_w;
						glbDouble ptx1_s = dx,pty1_s = dy;
						mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
						glbBool isObjfocus = mpr_globe->GetView()->Pick(ptx1_s,pty1_s,results,InterPoints);
						if (isObjfocus && InterPoints.size()!=0)
						{
							pt11_w = InterPoints[0];
						}
						glbBool ischoose = mpr_globe->ScreenToTerrainCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());
						if (!ischoose)
						{
							if (mpr_globe->IsUnderGroundMode())	  //开启地下模式 
							{
								glbBool isugchoose = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());
								if (!isugchoose && InterPoints.size()==0)
									return false;
							}else if (InterPoints.size()==0)			//未开地下模式
								return false;
						}			
						if (isObjfocus && InterPoints.size()!=0)
						{
							osg::Vec3d oldcameraPos,oldcameraPos_w;
							mpr_globe->GetView()->GetCameraPos(oldcameraPos);
							g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(oldcameraPos.y()),osg::DegreesToRadians(oldcameraPos.x()),oldcameraPos.z(),oldcameraPos_w.x(),oldcameraPos_w.y(),oldcameraPos_w.z());
							osg::Vec3d pt1_w;
							osg::Vec3d pttemp = _pushPos;
							g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_pushPos.y()),osg::DegreesToRadians(_pushPos.x()),_pushPos.z(),pt1_w.x(),pt1_w.y(),pt1_w.z());
							glbDouble dis1 = (oldcameraPos_w - pt1_w).length();
							glbDouble dis2 = (oldcameraPos_w - pt11_w).length();
							if (dis2 < dis1)
							{
								g_ellipsmodel->convertXYZToLatLongHeight(pt11_w.x(),pt11_w.y(),pt11_w.z(),_pushPos.y(),_pushPos.x(),_pushPos.z());
								_pushPos.x() = osg::RadiansToDegrees(_pushPos.x());
								_pushPos.y() = osg::RadiansToDegrees(_pushPos.y());
							}
						}
						GlbLogWOutput(GLB_LOGTYPE_INFO,L"经度:%.5f 纬度:%.5f 海拔高度:%.5f \r\n",_pushPos.x(),_pushPos.y(),_pushPos.z());
					}
					break;
				case osgGA::GUIEventAdapter::TOUCH_ENDED:	//push
					{
						_ga_t0 = NULL;
					}
					break;
				}
			}
			return false;
		}
	}
	switch (ea.getEventType())
	{
	case (osgGA::GUIEventAdapter::PUSH):	//鼠标按下
		{
			osg::Vec3d _pushPos(0.0,0.0,0.0);
			//记录选中点经纬高
			glbDouble dx = ea.getX();
			glbDouble dy = ea.getY();
			//窗口坐标系(鼠标位置) 转换为 屏幕坐标
			//mpr_globe->GetView()->WindowToScreen(dx,dy);
			std::vector<glbref_ptr<CGlbGlobeObject>> results;
			std::vector<osg::Vec3d> InterPoints;
			osg::Vec3d pt11_w;
			glbDouble ptx1_s = dx,pty1_s = dy;
			mpr_globe->GetView()->ScreenToWindow(ptx1_s,pty1_s);
			glbBool isObjfocus = mpr_globe->GetView()->Pick(ptx1_s,pty1_s,results,InterPoints);
			if (isObjfocus && InterPoints.size()!=0)
			{
				pt11_w = InterPoints[0];
			}
			glbBool ischoose = mpr_globe->ScreenToTerrainCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());
			if (!ischoose)
			{
				if (mpr_globe->IsUnderGroundMode())	  //开启地下模式 
				{
					glbBool isugchoose = mpr_globe->ScreenToUGPlaneCoordinate(dx,dy,_pushPos.x(),_pushPos.y(),_pushPos.z());
					if (!isugchoose && InterPoints.size()==0)
						return false;
				}else if (InterPoints.size()==0)			//未开地下模式
					return false;
			}			
			if (isObjfocus && InterPoints.size()!=0)
			{
				osg::Vec3d oldcameraPos,oldcameraPos_w;
				mpr_globe->GetView()->GetCameraPos(oldcameraPos);
				if(mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(oldcameraPos.y()),osg::DegreesToRadians(oldcameraPos.x()),oldcameraPos.z(),oldcameraPos_w.x(),oldcameraPos_w.y(),oldcameraPos_w.z());
					osg::Vec3d pt1_w;
					osg::Vec3d pttemp = _pushPos;
					g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(_pushPos.y()),osg::DegreesToRadians(_pushPos.x()),_pushPos.z(),pt1_w.x(),pt1_w.y(),pt1_w.z());
					glbDouble dis1 = (oldcameraPos_w - pt1_w).length();
					glbDouble dis2 = (oldcameraPos_w - pt11_w).length();
					if (dis2 < dis1)
					{
						g_ellipsmodel->convertXYZToLatLongHeight(pt11_w.x(),pt11_w.y(),pt11_w.z(),_pushPos.y(),_pushPos.x(),_pushPos.z());
						_pushPos.x() = osg::RadiansToDegrees(_pushPos.x());
						_pushPos.y() = osg::RadiansToDegrees(_pushPos.y());
					}
				}
				else
				{
					oldcameraPos_w = oldcameraPos;
					osg::Vec3d pt1_w;
					osg::Vec3d pttemp = _pushPos;
					pt1_w = _pushPos;
					glbDouble dis1 = (oldcameraPos_w - pt1_w).length();
					glbDouble dis2 = (oldcameraPos_w - pt11_w).length();
					if (dis2 < dis1)
					{
						_pushPos = pt11_w;
					}
				}
			}
			GlbLogWOutput(GLB_LOGTYPE_INFO,L"经度:%.8f 纬度:%.8f 海拔高度:%.5f \r\n",_pushPos.x(),_pushPos.y(),_pushPos.z());
		}
		break;
	default:
		break;
	}
	return false;
}

void CGlbGlobeView::CGlbGlobeRenderThread::run()
{
	if (mpr_done)return;

	mpr_done = true;
	OutputDebugString(L"GLBGlobe WARING : RenderThread(viewer::frame) start.\n");
	// 一秒钟循环60次
	double _runMaxFrameRate = 60.0; // 60帧/秒 Max 6 updates per seconds  150ms
	double minFrameTime = _runMaxFrameRate>0.0 ? 1.0/_runMaxFrameRate : 0.0;
	if(mpr_myosgviewer)mpr_myosgviewer->setReleaseContextAtEndOfFrameHint(false);

	while (mpr_done)
	{
		if (mpr_pause)
		{// 暂停 0.1秒  微秒 （1秒==1百万微秒）
			mpr_ispaused = true;
			OpenThreads::Thread::microSleep(100);
			continue;
		}

		osg::Timer_t startFrameTick = osg::Timer::instance()->tick();
		try
		{
			if (mpr_myosgviewer)			
				mpr_myosgviewer->frame();				
		}catch (std::exception& e)
		{
			OutputDebugString(L"Error: RenderThread(viewer::frame) exception!");
			LOG(e.what());
		}
		catch(...)
		{
			OutputDebugString(L"Error: RenderThread(viewer::frame) exception!");
			LOG("Error, RenderThread(viewer::frame) exception!");
		}
		// work out if we need to force a sleep to hold back the frame rate
		osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
		//WCHAR wBuff[128];
		//swprintf(wBuff,L"frameTime: %.5f .\n",frameTime);
		//OutputDebugString(wBuff);		

		if (frameTime < minFrameTime) 
			OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0*(minFrameTime-frameTime)));		
	}

	OutputDebugString(L"GLBGlobe WARING : RenderThread(viewer::frame) exit.\n");
}

void CGlbGlobeView::CGlbGlobeRenderThread::pause()
{
	mpr_pause = true;
	while(!mpr_ispaused)
	{
		OpenThreads::Thread::microSleep(100);
	}
}
void CGlbGlobeView::CGlbGlobeRenderThread::resume()
{
	mpr_pause    = false;
	mpr_ispaused = false;
}
int CGlbGlobeView::CGlbGlobeRenderThread::cancel()
{
	if(mpr_done)
	{
		mpr_done = false;
		OpenThreads::Thread::setCancelModeAsynchronous();//cancelMode==1	

		Sleep(100);
		OpenThreads::Thread::cancel();		
		return 1;
	}
	return 0;
}

void CGlbGlobeView::CGlbGlobeUpdateThread::run()
{
	if (mpr_done)return;
	mpr_done = true;
	OutputDebugString(L"GLBGlobe WARING : CGlbGlobeUpdateThread start!\n");
	// 一秒钟循环8次
	double _runMaxFrameRate = 67.0; // 15帧/秒 Max 6 updates per seconds  150ms
	double minFrameTime = _runMaxFrameRate>0.0 ? 1.0/_runMaxFrameRate : 0.0;

	osg::Timer_t vertifyStartTick = osg::Timer::instance()->tick();

	while (mpr_done)
	{
#ifndef GLOBE_DEVELOPER_MODE
		osg::Timer_t vertifyEndTick = osg::Timer::instance()->tick();
		double vertifyTime = osg::Timer::instance()->delta_s(vertifyStartTick, vertifyEndTick);
		if (vertifyTime > 600) //600
		{// 验证加密狗是否存在--10分钟检查一次
			vertifyStartTick = vertifyEndTick;
			//bool isok = false;
			//long rt = gverify("glbglobe","2.0");
			//if(rt == ERROR_OK)isok=true;
			//if(!isok)
			//{
			//	std::string msg="";			
			//	msg += "本地没有注册或已过有效期";
			//	::MessageBoxA(NULL,msg.c_str(),"请注册",MB_OK);	
			//	// 进程退出			
			//	ExitProcess(0);
			//}	

			//bool isok = true;
			//GlbLicResult* rts = srvlicVery();
			//if(rts == NULL)isok=false;
			//if(rts!=NULL && rts->code=="err")isok=false;
			//if(!isok)
			//{
			//	long rt = gverify("glbglobe","2.0");
			//	if(rt == ERROR_OK)isok=true;
			//}
			//if(!isok)
			//{
			//	std::string msg="";
			//	if(rts)
			//	{
			//		msg="许可服务:";
			//		msg+=rts->message;
			//		msg+=" 或 ";
			//	}
			//	msg += "本地没有注册或已过有效期";
			//	::MessageBoxA(NULL,msg.c_str(),"请注册",MB_OK);			 
			//}
			//if(rts)delete rts;
			//if(!isok)					
			//	ExitProcess(0);
		}
#endif

		if (mpr_pause)
		{// 暂停 0.1秒  微秒 （1秒==1百万微秒）
			mpr_ispaused = true;
			OpenThreads::Thread::microSleep(100);
			continue;
		}	

		osg::Timer_t startFrameTick = osg::Timer::instance()->tick();
		try
		{
			if (mpr_globe)
			{
				mpr_globe->Update();
			}
		}
		catch (std::exception& e)
		{
			OutputDebugString(L"Error: updateThread exception!\n");
			LOG(e.what());
		}
		catch (...)
		{
			OutputDebugString(L"Error: updateThread exception!\n");
			LOG("Error: updateThread exception!");			
		}
		// work out if we need to force a sleep to hold back the frame rate
		osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);

		//#ifdef _DEBUG
		//		float els = frameTime*1000.0f;
		//		_swprintf(wBuff,L"Update use time: %.3f milisecond.\n", els);
		//		OutputDebugString(wBuff);
		//#endif
		if (frameTime < minFrameTime) 
			OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0*(minFrameTime-frameTime)));
	}

	OutputDebugString(L"GLBGlobe WARING : CGlbGlobeUpdateThread exit.\n");
}

void CGlbGlobeView::CGlbGlobeUpdateThread::pause()
{
	mpr_pause = true;
	while(!mpr_ispaused)
	{
		OpenThreads::Thread::microSleep(100);
	}
}
void CGlbGlobeView::CGlbGlobeUpdateThread::resume()
{
	mpr_pause    = false;
	mpr_ispaused = false;
}
int CGlbGlobeView::CGlbGlobeUpdateThread::cancel()
{
	if(mpr_done)
	{
		mpr_done = false;
		OpenThreads::Thread::setCancelModeAsynchronous();//cancelMode==1	

		Sleep(100);
		OpenThreads::Thread::cancel();
		return 1;
	}
	return 0;
}

void CGlbGlobeView::CGlbGlobeComputeElevationThread::run()
{
	if (mpr_done)return;
	mpr_done = true;
	OutputDebugString(L"GLBGlobe WARING : ComputeElevationThread start!\n");
	// 一秒钟循环40次
	double _runMaxFrameRate = 40.0; // 40帧/秒 Max 40 updates per seconds  25ms
	double minFrameTime = _runMaxFrameRate>0.0 ? 1.0/_runMaxFrameRate : 0.0;
	while (mpr_done)
	{
		if (mpr_pause)
		{// 暂停 0.1秒  微秒 （1秒==1百万微秒）
			mpr_ispaused = true;
			OpenThreads::Thread::microSleep(100);
			continue;
		}
		osg::Timer_t startFrameTick = osg::Timer::instance()->tick();
		try
		{
			if (mpr_globe)
			{
				mpr_globe->ComputeElevation();
			}
		}
		catch (std::exception& e)
		{
			OutputDebugString(L"Error : ComputeElevationThread exception.\n");
			LOG(e.what());
		}	
		catch (...)
		{
			OutputDebugString(L"Error : ComputeElevationThread exception.\n");
			LOG("Error : ComputeElevationThread exception.")
		}
		// work out if we need to force a sleep to hold back the frame rate
		osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
		if (frameTime < minFrameTime) 
			OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0*(minFrameTime-frameTime)));
	}
	OutputDebugString(L"GLBGlobe WARING : ComputeElevationThread exit.\n");
}

void CGlbGlobeView::CGlbGlobeComputeElevationThread::pause()
{
	mpr_pause = true;
	while(!mpr_ispaused)
	{
		OpenThreads::Thread::microSleep(100);
	}
}
void CGlbGlobeView::CGlbGlobeComputeElevationThread::resume()
{
	mpr_pause    = false;
	mpr_ispaused = false;
}
int CGlbGlobeView::CGlbGlobeComputeElevationThread::cancel()
{
	if(mpr_done)
	{
		mpr_done = false;
		OpenThreads::Thread::setCancelModeAsynchronous();//cancelMode==1	

		Sleep(100);
		OpenThreads::Thread::cancel();		
		return 1;
	}
	return 0;
}

void CGlbGlobeView::CGlbGlobeDispatcherThread::run()
{
	if (mpr_done)return;

	mpr_done = true;
	OutputDebugString(L"GLBGlobe WARING : DispatcherThread start!\n");
	// 一秒钟循环20次
	double _runMaxFrameRate = 20.0; // 20帧/秒 Max 6 updates per seconds  150ms
	double minFrameTime = _runMaxFrameRate>0.0 ? 1.0/_runMaxFrameRate : 0.0;
	while (mpr_done)
	{
		if (mpr_pause)
		{// 暂停 0.1秒  微秒 （1秒==1百万微秒）
			mpr_ispaused = true;
			OpenThreads::Thread::microSleep(100);
			continue;
		}

		osg::Timer_t startFrameTick = osg::Timer::instance()->tick();
		try{
			if (mpr_globe)
			{				
				//osg::Timer_t startFrameTick2 = osg::Timer::instance()->tick();
				mpr_globe->Dispatch();
				//if (true)
				//{// 测试时间
				//	osg::Timer_t endFrameTick2 = osg::Timer::instance()->tick();
				//	double frameTime2 = osg::Timer::instance()->delta_s(startFrameTick2, endFrameTick2);
				//	LOG2("dispatcher use time :", frameTime2);
				//}
			}
		}catch (std::exception& e)
		{
			OutputDebugString(L"Error : DispatcherThread exception.\n");
			LOG(e.what());
		}
		catch (...)
		{
			OutputDebugString(L"Error : DispatcherThread exception.\n");
			LOG("Error : DispatcherThread exception!");
		}

		// work out if we need to force a sleep to hold back the frame rate
		osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
		if (frameTime < minFrameTime) 
			OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0*(minFrameTime-frameTime)));
	}
	OutputDebugString(L"GLBGlobe WARING : DispatcherThread exit.\n");
}

void CGlbGlobeView::CGlbGlobeDispatcherThread::pause()
{
	mpr_pause = true;
	while(!mpr_ispaused)
	{
		OpenThreads::Thread::microSleep(100);
	}
}
void CGlbGlobeView::CGlbGlobeDispatcherThread::resume()
{
	mpr_pause    = false;
	mpr_ispaused = false;
}
int CGlbGlobeView::CGlbGlobeDispatcherThread::cancel()
{
	if(mpr_done)
	{
		mpr_done = false;
		OpenThreads::Thread::setCancelModeAsynchronous();//cancelMode==1	

		Sleep(100);
		OpenThreads::Thread::cancel();		
		return 1;
	}
	return 0;
}


CGlbGlobeView::CGlbGlobeView(void)
{
	mpr_hwnd        = 0;
	mpr_parent_hwnd = 0;
	mpr_globe       = NULL;
	mpr_osgviewer   = NULL;
	mpr_p_root		= NULL;
	mpr_eventhandle = NULL;
	mpr_renderthread = NULL;
	mpr_updatethread = NULL;
	mpr_computeElvthread = NULL;
	mpr_manipulatormanager = NULL;
	mpr_dispatcherthread = NULL;
	mpr_cliphandler = NULL;
	mpr_isrun = false;
	//视图相关元素变量
 	mpr_statusbar = NULL;	//状态栏
 	mpr_ruler = NULL;		//导航标尺
 	mpr_navigator = NULL ;	//导航盘
 	mpr_logo = NULL;		//LOGO
 	mpr_scale = NULL;		//比例尺
	mpr_centermark=NULL;	//中心标记
	mpr_sky = NULL; //天空要素	
	mpr_mousepick = NULL;

	mpr_UndergroundDepth = -2000.0;
	mpr_cameraElevation = 0.0;
	mpr_focusElevation = 0.0;
	mpr_updateCameraMode = false;

	mpr_shareContext = NULL;
	//mpr_bInter = false;
	//mpr_interPos = osg::Vec3d(DBL_MAX,DBL_MAX,DBL_MAX);

	mpr_bShake = false;
	mpr_shakeTime = 10.0;

	//mpr_lightsManager.clear();
	mpr_backColor = osg::Vec4f(0,0,0,1);
	mpr_undergroundBackColor = osg::Vec4f(0,0.5,1,1); //0, 128, 255 天蓝色

	mpr_nextXpos = -1;
	mpr_nextYpos = -1;

	//使用手动重置为无信号状态，初始化时无信号状态										
	mpr_hEvent_SaveImage = CreateEvent(NULL,TRUE,FALSE,NULL);
}


CGlbGlobeView::~CGlbGlobeView(void)
{
	{//UnregisterTouchWindow 必须是Win7或以上.
		typedef	BOOL (__stdcall *PFNUNREGISTERTOUCHWINDOW)(HWND);
		HMODULE handle = ::LoadLibraryW(L"user32.dll");
		if(handle != NULL)
		{
			static PFNUNREGISTERTOUCHWINDOW pfUnregister = (PFNUNREGISTERTOUCHWINDOW)GetProcAddress(handle, "UnregisterTouchWindow");
			if(pfUnregister!=NULL)(*pfUnregister)(mpr_hwnd);
		}
	}
	CGlbGlobeViewRegistry * viewregsitry = CGlbGlobeViewRegistry::GetViewRegistry();
	viewregsitry->RemoveView(mpr_hwnd);	
	Pause();
	// 2015.6.24 马林 不能调用Stop,线程退出机制似乎有问题导致无法正常退出引发系统报错！改为使用Pause,让系统自己销毁线程。
	//Stop();
	if(mpr_eventhandle)
		delete mpr_eventhandle;
	if (mpr_cliphandler.valid())
	{
		mpr_p_root->removeUpdateCallback((osg::NodeCallback*)mpr_cliphandler);
		mpr_cliphandler.release();
		mpr_cliphandler = NULL;
	}
	mpr_globe=NULL;
	mpr_p_root=NULL;	
}

static LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CGlbGlobeViewRegistry * viewregsitry = CGlbGlobeViewRegistry::GetViewRegistry();
	CGlbGlobeView * pview = (CGlbGlobeView*)viewregsitry->FindView(hwnd);
	bool _continuesysproc = false;
	if(pview)
	{
		_continuesysproc = pview->HandEvents(uMsg,wParam,lParam);
	}
	if(!_continuesysproc && pview->_windowProcedure)
		return ::CallWindowProc(pview->_windowProcedure,hwnd,uMsg,wParam,lParam);
	return 1;
}

static LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

 HWND CGlbGlobeView::Create(HWND hWndParent)
 {
  	registerWindowClasses();
  
  	unsigned int merror=0;
  
  	DWORD dwExStyle =   0;
  	DWORD dwStyle   =   WS_VISIBLE;
  	if(0){//如果要装饰
  		dwStyle |= WS_CAPTION   |
  			WS_SYSMENU     |
  			WS_MINIMIZEBOX |
  			WS_MAXIMIZEBOX |
  			WS_SIZEBOX;
  		dwExStyle =   WS_EX_APPWINDOW           |
  			WS_EX_OVERLAPPEDWINDOW    |
  			WS_EX_ACCEPTFILES         |
  			WS_EX_LTRREADING;
  	}
  	//计算窗口大小
  	int top=0,left=0;
  	unsigned int width=0,height=0;
  	if(hWndParent==0)
  	{
  		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
  		if (wsi) 
  			wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
  		else
  		{
  			width = 1024;height = 800;
  		}
  		dwStyle |= WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  	}
  	else
  	{
  		dwStyle  |= WS_CHILD;// | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  		//dwExStyle = WS_EX_CLIENTEDGE;
  		RECT rect;
  		::GetClientRect(hWndParent,&rect);		
  		height = rect.bottom - rect.top;
  		width  = rect.right  - rect.left;
  	}
  	std::wstring window_name = L"CreateGlobe View";
  	HWND hwnd = ::CreateWindowEx(
  		dwExStyle,
  		mpr_classname.c_str(),
  		window_name.c_str(),
  		dwStyle,
  		left,
  		top,
  		width,
  		height,
  		hWndParent,
  		NULL,								
  		::GetModuleHandle(NULL),
  		NULL
  		);
  	if(hwnd == 0)
  	{  		
		DWORD err = ::GetLastError();
		//报告错误信息
  		return hwnd;
  	}	
  	CGlbGlobeViewRegistry * viewregsitry = CGlbGlobeViewRegistry::GetViewRegistry();
  	viewregsitry->RegistryView(hwnd,this);
  	mpr_hwnd = hwnd;	
  	mpr_parent_hwnd = hWndParent;
  	{//初始化光标
  		mpr_cursor = ::LoadCursor(NULL,IDC_ARROW);
  	}
	{//RegisterTouchWindow 必须是Win7或以上.
		typedef	BOOL (__stdcall *PFNREGISTERTOUCHWINDOW)(HWND, ULONG);
		HMODULE handle = ::LoadLibraryW(L"user32.dll");
		if(handle != NULL)
		{
			static PFNREGISTERTOUCHWINDOW pfRegister = (PFNREGISTERTOUCHWINDOW)GetProcAddress(handle, "RegisterTouchWindow");
			if(pfRegister!=NULL)(*pfRegister)(mpr_hwnd,0);
		}
	}
  	//一下是osg::Viewer的初始化
  	{
		HMODULE _hmd = NULL;
		{//ocx 不能装载osgdb_png.dll			
			HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
			CGlbWString mdir = CGlbPath::GetModuleDir(hmd);

			CGlbWString dpath = mdir + L"\\libpng.dll"; 
			_hmd= LoadLibrary(dpath.c_str());
			dpath = mdir + L"\\libtiff.dll";
			_hmd= LoadLibrary(dpath.c_str());	
			dpath = mdir + L"\\osgSim.dll";
			_hmd= LoadLibrary(dpath.c_str());
			dpath = mdir + L"\\osgFX.dll";
			_hmd= LoadLibrary(dpath.c_str());
			dpath = mdir + L"\\osgTerrain.dll";
			_hmd= LoadLibrary(dpath.c_str());
			dpath = mdir + L"\\osgVolume.dll";
			_hmd= LoadLibrary(dpath.c_str());
			dpath = mdir + L"\\osgdb_ive.dll";
			_hmd= LoadLibrary(dpath.c_str()); 
			dpath = mdir + L"\\osgdb_3ds.dll";
			_hmd= LoadLibrary(dpath.c_str()); 
			//dpath = mdir + L"\\osgdb_obj.dll";
			//_hmd= LoadLibrary(dpath.c_str()); 
		}
  		if(mpr_osgviewer == NULL)
  		{
  			RECT rect;
  			::GetClientRect(hwnd,&rect);
  			width = rect.right - rect.left;
  			height= rect.bottom- rect.top;
  			mpr_osgviewer = new myosgviewer();	//new osgViewer::Viewer();
  			osg::ref_ptr<osgViewer::StatsHandler> stats = new osgViewer::StatsHandler();	
  			stats->setKeyEventTogglesOnScreenStats('x');//  替代原来的s键
  			mpr_osgviewer->addEventHandler(stats);	//添加显示帧率回调	
  			if(mpr_parent_hwnd != 0)
  				mpr_osgviewer->setKeyEventSetsDone(0);//屏蔽ESC退出程序			

			//设置图形设备上下文
  			osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
  			osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(hwnd);
  			traits->x = 0;
  			traits->y = 0;
  			traits->width = width;
  			traits->height = height;
  			traits->windowDecoration = false;
  			traits->doubleBuffer = true;  			
  			traits->setInheritedWindowPixelFormat = true;
  			traits->stencil = true;			
  			traits->inheritedWindowData = windata;
			traits->sharedContext = 0;	
			traits->pbuffer = false;
			//traits->depth = 32;
  			osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());
			//if (gc)
			//{
			//	osgViewer::GraphicsHandleWin32* graphicsHandleWin32 = dynamic_cast<osgViewer::GraphicsHandleWin32*>(gc);
			//	if (graphicsHandleWin32)
			//	{
			//		HGLRC _hglrc = graphicsHandleWin32->getWGLContext();
			//		HDC _hdc = GetDC(hwnd);
			//		HGLRC _sharehglrc=wglCreateContext(_hdc);
			//		if ( !wglShareLists(_hglrc, _sharehglrc) )
			//		{						
			//			OSG_NOTICE << "wglShareLists failed ." << std::endl;		
			//		}
			//		else
			//		{
			//			const osg::GraphicsContext::Traits* src_traits = gc->getTraits();
			//			osg::GraphicsContext::Traits* traits = new osg::GraphicsContext::Traits;
			//			traits->screenNum = src_traits->screenNum;
			//			traits->displayNum = src_traits->displayNum;
			//			traits->hostName = src_traits->hostName;
			//			traits->width = 100;
			//			traits->height = 100;
			//			traits->red = src_traits->red;
			//			traits->green = src_traits->green;
			//			traits->blue = src_traits->blue;
			//			traits->alpha = src_traits->alpha;
			//			traits->depth = src_traits->depth;
			//			traits->sharedContext = 0;
			//			traits->pbuffer = false;

			//			osg::ref_ptr<osg::GraphicsContext> shareContext = osg::GraphicsContext::createGraphicsContext(traits);
			//			shareContext->setName("shareContext");
			//			osgViewer::GraphicsHandleWin32* graphicsHandleWin32 = dynamic_cast<osgViewer::GraphicsHandleWin32*>(shareContext.get());
			//			if (graphicsHandleWin32)
			//			{
			//				unsigned int contextID = gc->getState()->getContextID(); 
			//				unsigned int shareID = shareContext->getState()->getContextID();
			//				// 将ContextID设为与渲染ID一致以保证使用的是同一套显示列表地址
			//				//shareContext->setState(gc->getState());
			//				shareContext->getState()->setContextID(gc->getState()->getContextID());

			//				graphicsHandleWin32->setHDC(_hdc);
			//				graphicsHandleWin32->setHWND(hwnd);
			//				graphicsHandleWin32->setWGLContext(_sharehglrc);

			//				mpr_shareContext = shareContext;
			//			}	
			//		}
			//	}				
			//}
  			osg::ref_ptr<osg::Camera> camera = new osg::Camera;
  			camera->setGraphicsContext(gc);
  			camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));
  			//camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			//glEnable(GL_STENCIL_TEST);//longtingyou
			camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			osg::DisplaySettings::instance()->setMinimumNumStencilBits(8);
			//camera->getDisplaySettings()->setMinimumNumStencilBits(8);//longtingyou

  			camera->setClearColor(osg::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));  //修改背景为黑色
			camera->setViewMatrixAsLookAt(osg::Vec3d(0.0,-1.0,0.0),osg::Vec3d(0.0,0.0,0.0),osg::Vec3d(0.0,0.0,1.0));
  			camera->setProjectionMatrixAsPerspective(
  				30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0, 1000.0);
			
			//camera->setCullingMode(osg::Camera::NO_CULLING);
			// 注释:裁剪方式必须去掉osg::CullSettings::SMALL_FEATURE_CULLING否则像素点和像素线将绘制不出来 2015.6.17 马林
			//camera->setCullingMode(osg::Camera::DEFAULT_CULLING);
			camera->setCullingMode(camera->getCullingMode() &
				~osg::CullSettings::SMALL_FEATURE_CULLING);
			//camera->setCullingMode(osg::Camera::VIEW_FRUSTUM_SIDES_CULLING 				
			//	| osg::Camera::SHADOW_OCCLUSION_CULLING
			//	| osg::Camera::CLUSTER_CULLING);

			{//输出场景深度
				osg::Texture2D *tex=new osg::Texture2D();
				tex->setTextureSize(traits->width, traits->height);
				tex->setShadowTextureMode(osg::Texture2D::LUMINANCE);
				tex->setInternalFormat( GL_DEPTH_COMPONENT24 );
				tex->setSourceFormat( GL_DEPTH_COMPONENT );
				tex->setSourceType(GL_FLOAT); 
				tex->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
				tex->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST );
				tex->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
				tex->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);

				camera->attach( osg::Camera::DEPTH_BUFFER, tex ); 
				mpr_depthTexture=tex; 

				tex=new osg::Texture2D();
				tex->setTextureSize(traits->width, traits->height);
				tex->setShadowTextureMode(osg::Texture2D::LUMINANCE);
				tex->setInternalFormat( GL_RGBA32F_ARB );
				tex->setSourceFormat( GL_RGBA );
				tex->setSourceType(GL_FLOAT); 
				tex->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
				tex->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST );
				tex->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
				tex->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);

				camera->attach( osg::Camera::COLOR_BUFFER, tex ); 
				mpr_sceneTexture=tex;

				//camera->setCullMask( ~64 ); 
			}  
			
  			mpr_osgviewer->setCamera(camera.get());
			//设置足够大的osg调度上限，避免osg参与调度。osg内此默认值为300
  			//mpr_osgviewer->getDatabasePager()->setTargetMaximumNumberOfPageLOD(300/*PAGELODSIZE*/);
			mpr_osgviewer->getDatabasePager()->setDoPreCompile(true);

  			//mpr_osgviewer->addEventHandler(new CGlbGlobeViewElementPickHandler(this));		
  		}
  		_windowProcedure = NULL;
  
  		if(mpr_osgviewer)
  		{
  			::SetLastError(0);
  			_windowProcedure = (WNDPROC)::SetWindowLongPtr(hwnd, GWLP_WNDPROC, LONG_PTR(WindowProc2));
  			merror = ::GetLastError();
  			mpr_osgviewer->mpr_eventhandle = mpr_eventhandle;
			mpr_p_rootEx = new CGlbGlobeSceneManager();  		
			mpr_p_root=mpr_p_rootEx;
  			mpr_osgviewer->setSceneData(mpr_p_root.get());

			// 添加光源根节点
			mpr_p_LightsRoot = new osg::Group;
			mpr_p_LightsRoot->setName("LightRoot");
			mpr_p_root->addChild(mpr_p_LightsRoot);
  		}
  	}
 	return hwnd;	
 }

void CGlbGlobeView::registerWindowClasses()
{
	mpr_classname = L"CreatarGlobe View for Win32";

	WNDCLASSEX wc;

	HINSTANCE hinst = ::GetModuleHandle(NULL);

	wc.cbSize        = sizeof(wc);
	wc.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hinst;
	wc.hIcon         = NULL;
	wc.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = 0;
	wc.lpszClassName = mpr_classname.c_str();
	wc.hIconSm       = NULL;

	if (::RegisterClassEx(&wc)==0)
	{
		unsigned int lastError = ::GetLastError();
		if (lastError!=ERROR_CLASS_ALREADY_EXISTS)
		{
			//报告错误
			return;
		}
	}
	return;
}

void CGlbGlobeView::SetWinPosition(glbInt32 left,glbInt32 top,glbInt32 width,glbInt32 height)
{
	if(mpr_hwnd)
	{
		::SetWindowPos(mpr_hwnd,NULL,left,top,width,height,SWP_NOACTIVATE|SWP_NOOWNERZORDER);
		if(mpr_osgviewer)
		{
			mpr_osgviewer->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width)/static_cast<double>(height), 1.0, 1000.0);
			mpr_osgviewer->getCamera()->setViewport(left,top,width,height);
			if(mpr_statusbar.valid())
				mpr_statusbar->SetPosition(0,0,width,height);
				//osg::ref_ptr<osg::Viewport> vp = mpr_statusbar->getViewport();
				//if(vp->y() != 0)
				//	mpr_statusbar->setViewport(new osg::Viewport(0,height - 25,width,25));
				//else
				//	mpr_statusbar->setViewport(new osg::Viewport(0,0,width,25));
			if (mpr_centermark.valid())
				mpr_centermark->SetPosition(0,0,width,height);
			if(mpr_scale.valid())
				mpr_scale->SetPosition(0,0,width,height);
			if(mpr_logo.valid())
				mpr_logo->SetPosition(0,0,width,height);
			if(mpr_navigator.valid())
				mpr_navigator->SetPosition(0,0,width,height);
			if(mpr_ruler.valid())
				mpr_ruler->SetPosition(0,0,width,height);
			//if(mpr_nokey)
			//	mpr_nokey->SetPosition(width,height);
			//if (mpr_globe)
			//	mpr_globe->NotifyViewportChanged(left,top,width,height);
		}
	}
}

void CGlbGlobeView::SetFocus(void)
{
	::SetFocus(mpr_hwnd);
}

HCURSOR CGlbGlobeView::SetCursor(HCURSOR hcursor)
{
	HCURSOR oldcursor = mpr_cursor;
	mpr_cursor = hcursor;	
	return oldcursor;
}

void CGlbGlobeView::SetBackColor(glbFloat r,glbFloat g,glbFloat b,glbFloat a)
{
	mpr_backColor.set(r,g,b,a);
	if(mpr_osgviewer)
	{
		osg::Vec4f clearClr = mpr_osgviewer->getCamera()->getClearColor();
		if (clearClr != mpr_backColor)
			mpr_osgviewer->getCamera()->setClearColor(mpr_backColor);

		if(mpr_p_root->getNumChildren() > 2)
		{
			osg::Node *baseGlobe = mpr_p_root->getChild(1);
			if(baseGlobe->getName() == "slaveCamera")
			{
				if(mpr_p_root->getNumChildren() > 3)
					baseGlobe = mpr_p_root->getChild(2);
			}

			if(baseGlobe->getName() != "baseGlobe")
				return;
			osg::ref_ptr<osg::StateSet> stateset = baseGlobe->getOrCreateStateSet();	
			osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
			if(material == NULL)
				material = new osg::Material;

			material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(r,g,b,a));
			material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(r,g,b,a));
			material->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(r,g,b,a));
			material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(r,g,b,a));
			stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}
	}	
}

void CGlbGlobeView::GetBackColor(glbFloat &r,glbFloat &g,glbFloat &b,glbFloat &a)
{
	r = mpr_backColor.r();
	g = mpr_backColor.g();
	b = mpr_backColor.b();
	a = mpr_backColor.a();
}

void CGlbGlobeView::SetUnderGroundBackColor(glbFloat r,glbFloat g,glbFloat b,glbFloat a)
{
	mpr_undergroundBackColor.set(r,g,b,a);
	if(mpr_osgviewer)
	{
		osg::Vec4f clearClr = mpr_osgviewer->getCamera()->getClearColor();
		if (clearClr != mpr_undergroundBackColor)
			mpr_osgviewer->getCamera()->setClearColor(mpr_undergroundBackColor);
	}	
}

void CGlbGlobeView::GetUnderGroundBackColor(glbFloat &r,glbFloat &g,glbFloat &b,glbFloat &a)
{
	r = mpr_undergroundBackColor.r();
	g = mpr_undergroundBackColor.g();
	b = mpr_undergroundBackColor.b();
	a = mpr_undergroundBackColor.a();
}

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif
#ifndef GET_KEYSTATE_WPARAM
#define GET_KEYSTATE_WPARAM                     (LOWORD(wParam))
#endif
#ifndef GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM                  ((short)HIWORD(wParam))
#endif

glbBool CGlbGlobeView::HandEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		{			
			::SetFocus(mpr_hwnd);			
			if(mpr_eventhandle)
			{
				long xpos = GET_X_LPARAM(lParam); 
				long ypos = GET_Y_LPARAM(lParam);
				GlbGlobeMouseButtonEnum button = GLB_LEFT_MOUSEBUTTON;
				if(uMsg == WM_LBUTTONDOWN)
					button=GLB_LEFT_MOUSEBUTTON;
				else if(uMsg == WM_RBUTTONDOWN)
					button=GLB_RIGHT_MOUSEBUTTON;
				else
					button=GLB_MIDDLE_MOUSEBUTTON;
				long nflags = GET_KEYSTATE_WPARAM(wParam);
				return mpr_eventhandle->OnButtonDown(button,nflags,xpos,ypos);
			}
		}
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		{
			::SetFocus(mpr_hwnd);			
			if(mpr_eventhandle)
			{
				long xpos = GET_X_LPARAM(lParam); 
				long ypos = GET_Y_LPARAM(lParam); 
				GlbGlobeMouseButtonEnum button = GLB_LEFT_MOUSEBUTTON;
				if(uMsg == WM_LBUTTONUP)
					button=GLB_LEFT_MOUSEBUTTON;
				else if(uMsg == WM_RBUTTONUP)
					button=GLB_RIGHT_MOUSEBUTTON;
				else
					button=GLB_MIDDLE_MOUSEBUTTON;
				long nflags = GET_KEYSTATE_WPARAM(wParam);
				return mpr_eventhandle->OnButtonUp(button,nflags,xpos,ypos);
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
		{
			::SetFocus(mpr_hwnd);						
			if(mpr_eventhandle)
			{
				long xpos = GET_X_LPARAM(lParam); 
				long ypos = GET_Y_LPARAM(lParam); 
				GlbGlobeMouseButtonEnum button = GLB_LEFT_MOUSEBUTTON;
				if(uMsg == WM_LBUTTONDBLCLK)
					button=GLB_LEFT_MOUSEBUTTON;
				else if(uMsg == WM_RBUTTONDBLCLK)
					button=GLB_RIGHT_MOUSEBUTTON;
				else
					button=GLB_MIDDLE_MOUSEBUTTON;
				long nflags = GET_KEYSTATE_WPARAM(wParam);
				return mpr_eventhandle->OnButtonDblClk(button,nflags,xpos,ypos);
			}
		}	
		break;
	case WM_MOUSEWHEEL:
		{			
			if(mpr_eventhandle)
			{
				long nflags = GET_KEYSTATE_WPARAM(wParam);
				long zdelta = GET_WHEEL_DELTA_WPARAM(wParam);
				long xpos = GET_X_LPARAM(lParam); 
				long ypos = GET_Y_LPARAM(lParam); 	
				return mpr_eventhandle->OnMouseWheel(nflags,zdelta,xpos,ypos);
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			if(mpr_eventhandle)
			{
				long nflags = GET_KEYSTATE_WPARAM(wParam);
				long xpos = GET_X_LPARAM(lParam); 
				long ypos = GET_Y_LPARAM(lParam); 
				if(xpos == mpr_nextXpos && ypos == mpr_nextYpos)
					return false;
				mpr_nextXpos = xpos;
				mpr_nextYpos = ypos;
				return mpr_eventhandle->OnMouseMove(nflags,xpos,ypos);
			}
		}
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			//
			if(mpr_eventhandle)
				return mpr_eventhandle->OnKeyDown(wParam,lParam);
		}
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		{			
			if(wParam == VK_ESCAPE && mpr_parent_hwnd)
			{//通知父窗口，可以退出全屏
				::SendMessage(mpr_parent_hwnd,WM_KEYUP,wParam,lParam);
			}
			if(wParam == 'F' || wParam == 'f')
			{
				glbBool bWire = mpr_globe->IsTerrainWireMode();
				mpr_globe->EnableTerrainWireMode(!bWire);
			}
			if(wParam == 'V' || wParam == 'v')
			{
				glbBool bEnableUpdate = mpr_globe->IsTerrainUpdateEnable();
				mpr_globe->EnableTerrainUpdate(!bEnableUpdate);
			}
			if (wParam == 'K' || wParam == 'k')
			{	
				//{// 测试坐标轴对象 2016.7.11 malin
				//	TestCreateCoordinateAxisObject();
				//}
				//{// 测试ComputeCameraPitchAndYaw 2016.6.12 malin
				//	osg::Vec3d cameraPos,focusPos;
				//	GetCameraPos(cameraPos);
				//	GetFocusPos(focusPos);
				//	glbDouble pitch,yaw;
				//	ComputeCameraPitchAndYaw(cameraPos.x(),cameraPos.y(),cameraPos.z(),
				//		focusPos.x(),focusPos.y(),focusPos.z(),
				//		pitch,yaw);
				//	glbDouble _yaw = GetCameraYaw();
				//	glbDouble _pitch = GetCameraPitch();

				//	if (_yaw != yaw || _pitch != pitch)
				//		int _err = 1;
				//}
				//Pause(true);
				//Shake(!mpr_bShake);
				//if (mpr_bShake)
				//{
				//	mpr_smokeNode = CreateSmoke();
				//	mpr_p_root->addChild(mpr_smokeNode);
				//	// 启用雾效
				//	//mpr_fog = CreateFog(true);

				//	//mpr_globe->GetTerrain()->GetNode()->getOrCreateStateSet()->setAssociatedModes(mpr_fog.get(),osg::StateAttribute::ON );
				//	//mpr_globe->GetTerrain()->GetNode()->getOrCreateStateSet()->setMode(GL_FOG,
				//	//							osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
				//	//mpr_globe->GetTerrain()->GetNode()->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OFF);
				//	//mpr_globe->GetTerrain()->GetNode()->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
				//	
				//}
				//else
				//{
				//	mpr_p_root->removeChild(mpr_smokeNode);
				//	//mpr_globe->GetTerrain()->GetNode()->getOrCreateStateSet()->removeAssociatedModes(mpr_fog.get());
			
				//}
				//Pause(false);
				//glbBool res = SaveImage(L"f:\\Large_Screenshot.tif",4564,3283);//*/
				//glbBool res = SaveImage(L"f:\\Large_Screenshot.jpg");
				//CGlbGlobeDataExchange::tinToOsgFile(L"F:\\测试\\zjkjsgd_js0007.tins",L"F:\\测试\\zjkjsgd_js0007.osg",RGB(0,192,192),false);
			}
			//if (wParam == 'O' || wParam == 'o')
			//{
			//	bool isStero = osg::DisplaySettings::instance()->getStereo();
			//	Pause(true);
			//	isStero = !isStero;
			//	osg::DisplaySettings::instance()->setStereo(isStero);
			//	if (isStero || osg::DisplaySettings::instance()->getStereoMode()!=osg::DisplaySettings::QUAD_BUFFER)
			//		osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::QUAD_BUFFER);
			//	Pause(false);
			//}			
			//if (wParam == 'P' || wParam == 'p')
			//{
			//	bool isStero = osg::DisplaySettings::instance()->getStereo();
			//	Pause(true);
			//	isStero = !isStero;
			//	osg::DisplaySettings::instance()->setStereo(isStero);
			//	if (isStero || osg::DisplaySettings::instance()->getStereoMode()!=osg::DisplaySettings::HORIZONTAL_SPLIT)
			//		osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
			//	Pause(false);
			//}
			if(mpr_eventhandle)
				return mpr_eventhandle->OnKeyUp(wParam,lParam);
		}
		break;	
	case WM_SETCURSOR:
		{
			::SetCursor(mpr_cursor);
			return true;
		}
		break;
	}
	return false;
}

IGlbGlobeViewEventHandle*  CGlbGlobeView::SetEventHandle(IGlbGlobeViewEventHandle *event_handle)
{
	IGlbGlobeViewEventHandle *old_eventhandle = mpr_eventhandle;
	mpr_eventhandle = event_handle;
	if(mpr_osgviewer)
		mpr_osgviewer->mpr_eventhandle = event_handle;
	return old_eventhandle;
}

IGlbGlobeViewEventHandle* CGlbGlobeView::GetEventHandle()
{
	return mpr_eventhandle;
}

#include <osgGA/TrackballManipulator>
glbBool CGlbGlobeView::SetGlobe(CGlbGlobe* p_globe)
{
	OutputDebugString(L"	SetGlobe ...\n");
	if(mpr_osgviewer == NULL)
	{
		return false;//需要先创建调用Create创建视图窗口
	}
	if (p_globe == NULL)
	{
		return false;
	}
	if (p_globe == mpr_globe.get())
	{
		return true;
	}
	OutputDebugString(L"	SetGlobe begin...\n");
	//停止 所有线程
	Pause(true);

	OutputDebugString(L"	Paused ...\n");
	//Stop();
	if(mpr_globe.get() && mpr_globe->mpr_root.get() && mpr_p_root.get())
	{
		mpr_p_root->removeChild(mpr_globe->mpr_root.get());
		if(mpr_p_LightsRoot)
		{
			mpr_p_LightsRoot->removeChild(mpr_globe->mpr_root.get());
			mpr_p_root->removeChild(mpr_p_LightsRoot.get());
			mpr_p_LightsRoot=NULL;
		}
	}
	mpr_globe = p_globe;
	mpr_globe->mpr_globeview = this;
	OutputDebugString(L"	mpr_globe Initialize...\n");
	mpr_globe->Initialize();	
	
	if (mpr_p_LightsRoot==NULL)	
	{
		mpr_p_LightsRoot = new osg::Group();
		mpr_p_LightsRoot->setName("LightRoot");
		mpr_p_root->addChild(mpr_p_LightsRoot);
	}
	mpr_p_LightsRoot->addChild(mpr_globe->mpr_root);
	
	// 添加一个太阳光源	和一个反向太阳光源 【都是平行光】	点亮整个场景
	{	
		//// 设置全局环境光为白色光 2016.11.22 - 无用
		//osg::LightModel* lightModel = new osg::LightModel;
		//lightModel->setAmbientIntensity(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
		//mpr_p_LightsRoot->getOrCreateStateSet()->setAttribute(lightModel);		

		osg::Node* sunLight = createSunLight();
		//mpr_p_LightsRoot->addChild(sunLight);
		osg::LightSource* sunls = dynamic_cast<osg::LightSource*>(sunLight);
		osg::Light* sun = sunls->getLight();
		GlbGlobeLight glbLight;
		if (osgLight2glbLight(sun,glbLight))
		{// 添加此光源到光源管理器中
			AddLight(glbLight);
		}	//*/

		////添加对称太阳光源 
		//osg::Node* symmetricalSunLight = createSymmetricalSunLight();
		//osg::LightSource* ssunls = dynamic_cast<osg::LightSource*>(symmetricalSunLight);
		//osg::Light* ssun = ssunls->getLight();
		//if (osgLight2glbLight(ssun,glbLight))
		//{// 添加此光源到光源管理器中
		//	AddLight(glbLight);
		//}	//*/		
	}
	
	//mpr_p_root->addChild(mpr_globe->mpr_root); 	

	//测试代码
#ifdef _DEBUG
	//mpr_p_root->addChild(createEarth());
	//mpr_globe->mpr_root->addChild(createPlane());
#endif
	OutputDebugString(L"	initElements...\n");
	//初始化视图元素
	initElements();
	
	OutputDebugString(L"	initElements end...\n");
	if (mpr_mousepick.valid())
	{
		mpr_mousepick.release();
		mpr_mousepick = NULL;
	}
	mpr_mousepick = new CGlbGlobeMousePickHandle(p_globe);
	mpr_p_root->setEventCallback(mpr_mousepick.get());	
	
	OutputDebugString(L"	setEventCallback end...\n");
	if (mpr_manipulatormanager.valid())
	{
		mpr_manipulatormanager.release();
		mpr_manipulatormanager = NULL;
	}
	mpr_manipulatormanager = new CGlbGlobeManipulatorManager(p_globe);
	mpr_osgviewer->setCameraManipulator(mpr_manipulatormanager.get());	

	OutputDebugString(L"	setCameraManipulator end...\n");
	osg::Vec3d eye,center,up;
	mpr_osgviewer->getCameraManipulator()->getHomePosition(eye,center,up);
	//mpr_osgviewer->setLightingMode(osg::View::LightingMode::HEADLIGHT);
	if (mpr_cliphandler.valid())
	{
		mpr_p_root->removeUpdateCallback((osg::NodeCallback*)mpr_cliphandler);
		mpr_cliphandler.release();
		mpr_cliphandler = NULL;
	}
	mpr_cliphandler = new CGlbGlobeClipHandler(p_globe);
	mpr_p_root->setUpdateCallback((osg::NodeCallback*)mpr_cliphandler);

	OutputDebugString(L"	setUpdateCallback end...\n");

	// 设置创建预编译线程来编译显示列表避免帧冲击[osg本身在实现此接口功能是有问题的，所以暂时不能用2015.4.16]
	osg::DisplaySettings::instance()->setCompileContextsHint(true);

	// 由于有多个context所以系统默认会选择DrawThreadPerContext模式 
	mpr_osgviewer->setThreadingModel(osgViewer::Viewer::AutomaticSelection); // SingleThreaded   //AutomaticSelection //CullPerCameraDrawPerContext
	mpr_osgviewer->realize();

	// attach an IncrementaCompileOperation to allow the master loading 
	// to be handled with an incremental compile to avoid frame drops when large objects are added.
	// 附加增量编译以避免帧冲击 -- 由于已经启用了预编译选项osg::DisplaySettings::instance()->setCompileContextsHint(true)所以理论上增量编译是在另一个线程来进行的不会影响渲染线程的时间。
	//osgUtil::IncrementalCompileOperation* _ico = new osgUtil::IncrementalCompileOperation();
	//mpr_osgviewer->setIncrementalCompileOperation(_ico);
	// 初始化场景任务处理管理器
	if (mpr_globe->mpr_taskmanager==NULL)
		mpr_globe->mpr_taskmanager = new CGlbGlobeTaskManager();
	
	//osg::ref_ptr<osg::GraphicsContext> gc = mpr_osgviewer->getCamera()->getGraphicsContext();	
	mpr_shareContext = osg::GraphicsContext::getCompileContext(0);
	mpr_globe->mpr_taskmanager->init(mpr_shareContext.get());

	OutputDebugString(L"	mpr_taskmanager::init end...\n");
	
	// enable the image cache so we don't need to keep loading the particle files
	osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options;
	options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL);
	osgDB::Registry::instance()->setOptions(options);

	// 启动读取osg文件时构建kdtree加速选择
	osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::Options::BUILD_KDTREES);

	Pause(false);

	OutputDebugString(L"	SetGlobe end...\n");
	return true;
}

void CGlbGlobeView::Run()
{
#ifndef GLOBE_DEVELOPER_MODE
	{
		bool isok = true;
		GlbLicResult* rts = srvlicVery();
		if(rts == NULL)isok=false;
		if(rts!=NULL && rts->code=="err")isok=false;
		if(!isok)
		{
			long rt = gverify("glbglobe","2.0");
			if(rt == ERROR_OK)isok=true;
		}
		if(!isok)
		{
			std::string msg="";
			if(rts)
			{
				msg="许可服务:";
				msg+=rts->message;
				msg+=" 或 ";
			}
			msg += "本地没有注册或已过有效期";
			::MessageBoxA(NULL,msg.c_str(),"请注册",MB_OK);			 
		}
		if(rts)delete rts;
		if(!isok)return;
		
		//long rt = gverify("glbglobe","2.0");
		//if(rt == ERROR_OK)isok=true;		
		//if(!isok)
		//{
		//	std::string msg="";			
		//	msg += "本地没有注册或已过有效期";
		//	::MessageBoxA(NULL,msg.c_str(),"请注册",MB_OK);			
		//	return;
		//}
	}
#endif

	if(mpr_isrun)return;
	//OutputDebugString(L"GlbGlobeView :: run start.\n");
	if(mpr_osgviewer)
	{		
		if (mpr_updatethread)
		{// 启动更新线程
			mpr_updatethread->cancel();
			delete mpr_updatethread;
			mpr_updatethread = NULL;
		}
		mpr_updatethread = new CGlbGlobeUpdateThread(GetGlobe());
		mpr_updatethread->startThread();		
		if (mpr_dispatcherthread)
		{// 启动调度线程
			mpr_dispatcherthread->cancel();
			delete mpr_dispatcherthread;
			mpr_dispatcherthread = NULL;
		}
		mpr_dispatcherthread = new CGlbGlobeDispatcherThread(GetGlobe());
		mpr_dispatcherthread->startThread();

		if (mpr_computeElvthread)
		{// 启动高程计算线程
			mpr_computeElvthread->cancel();
			delete mpr_computeElvthread;
			mpr_computeElvthread =NULL;
		}

		mpr_computeElvthread = new CGlbGlobeComputeElevationThread(GetGlobe());
		mpr_computeElvthread->startThread();		
		if(mpr_parent_hwnd == 0)
		{// 渲染
			mpr_osgviewer->run();
			mpr_isrun=true;
			return;
		}

		// 启动渲染渲染
		if (mpr_renderthread)
		{// 如果线程存在，先取消它
			mpr_renderthread->cancel();
			delete mpr_renderthread;
			mpr_renderthread = NULL;
		}
		mpr_renderthread = new CGlbGlobeRenderThread(mpr_osgviewer.get());
		mpr_renderthread->startThread();
		mpr_isrun = true;	
	}
	return;
}

void CGlbGlobeView::Pause(glbBool bPause)
{
	if(!mpr_isrun)return;
	if(mpr_osgviewer == NULL)return;
	//OutputDebugString(L"WARING : GlbGlobeView Pause is start.\n");
	if(bPause)		
	{
		if (mpr_updatethread)
			mpr_updatethread->pause();			
		if (mpr_computeElvthread)
			mpr_computeElvthread->pause();				
		if (mpr_dispatcherthread)		
			mpr_dispatcherthread->pause();	
		if (mpr_renderthread)
			mpr_renderthread->pause();						
		if (mpr_parent_hwnd==0)
			mpr_osgviewer->stopThreading();
		//OutputDebugString(L"WARING : GlbGlobeView Pause(true) end.\n");
	}else{
		if (mpr_updatethread)
			mpr_updatethread->resume();			
		if (mpr_computeElvthread)
			mpr_computeElvthread->resume();				
		if (mpr_dispatcherthread)		
			mpr_dispatcherthread->resume();	
		if (mpr_renderthread)
			mpr_renderthread->resume();						
		if (mpr_parent_hwnd==0)
			mpr_osgviewer->startThreading();

		//OutputDebugString(L"WARING : GlbGlobeView Pause(false) end.\n");
	}

	//if (mpr_globe && mpr_globe->mpr_taskmanager)
	//	mpr_globe->mpr_taskmanager->Pause(bPause);
}

void CGlbGlobeView::PauseRenderThread(glbBool bPause)
{
	if(!mpr_isrun)return;
	if(mpr_osgviewer == NULL)return;

	if(bPause)		
	{		
		if (mpr_renderthread)
			mpr_renderthread->pause();						
		if (mpr_parent_hwnd==0)
			mpr_osgviewer->stopThreading();
	}else{		
		if (mpr_renderthread)
			mpr_renderthread->resume();						
		if (mpr_parent_hwnd==0)
			mpr_osgviewer->startThreading();
	}
}

void CGlbGlobeView::Stop()
{
	if(!mpr_isrun)return;

	Pause(true);
	if(mpr_osgviewer)
	{		
		if (mpr_updatethread)
		{// 清除更新线程
			mpr_updatethread->cancel();
			delete mpr_updatethread;
			mpr_updatethread = NULL;
		}		
		if (mpr_computeElvthread)
		{// 清除高程计算线程
			mpr_computeElvthread->cancel();
			delete mpr_computeElvthread;
			mpr_computeElvthread =NULL;
		}		
		if (mpr_dispatcherthread)
		{// 清除调度线程
			mpr_dispatcherthread->cancel();
			delete mpr_dispatcherthread;
			mpr_dispatcherthread = NULL;
		}

		// 消除渲染线程
		if (mpr_renderthread)
		{
			mpr_renderthread->cancel();
			delete mpr_renderthread;
			mpr_renderthread = NULL;
		}

		if (mpr_parent_hwnd==0)
		{
			mpr_osgviewer->stopThreading();
			mpr_isrun=false;
			return;
		}
		mpr_isrun=false;
	}
	return;
}

GlbGlobeManipulatorTypeEnum CGlbGlobeView::GetManipulatorType()
{
	if(mpr_osgviewer)
	{
		osg::ref_ptr<osgGA::CameraManipulator> cm= mpr_osgviewer->getCameraManipulator();
		CGlbGlobeManipulatorManager* mg = dynamic_cast<CGlbGlobeManipulatorManager*>(cm.get());
		if(mg)
			return mg->GetType();
		return GLB_MANIPULATOR_UNKNOWN;
	}
	return GLB_MANIPULATOR_UNKNOWN;
}

glbBool CGlbGlobeView::SetDragMode()
{
	if (mpr_manipulatormanager.valid())
	{		
		Pause(true);
		mpr_manipulatormanager->SetDragMode();
		Pause(false);
		return true;
	}
	return false;
}

glbBool CGlbGlobeView::SetLockMode(GlbGlobeLockModeEnum glbType)
{
	if (mpr_manipulatormanager.valid())
	{
		GlbGlobeManipulatorTypeEnum mtype = mpr_manipulatormanager->GetType();
		if(mtype == GLB_MANIPULATOR_FREE)	//自由操控器
		{
			mpr_manipulatormanager->SetLockMode(glbType);
		}
	}
	return false;
}

glbBool CGlbGlobeView::SetDriveMode()
{	
	if (mpr_manipulatormanager.valid())
	{		
		Pause(true); //暂停
		mpr_manipulatormanager->SetDriveMode();
		Pause(false);//Resume
		return true;
	}
	return false;
}

glbBool CGlbGlobeView::SetPathMode(CGlbGlobeDynamic *obj)
{
	if (mpr_manipulatormanager.valid())
	{				
		Pause(true);		
		mpr_manipulatormanager->SetPathMode(obj);
		Pause(false);		
		return true;
	}
	return false;
}

void CGlbGlobeView::WGS84ToWorld(glbDouble* xorlon,glbDouble* yorlat,glbDouble* zoralt)
{
	double x,y,z;
	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(*yorlat),osg::DegreesToRadians(*xorlon),*zoralt,x,y,z);
	*xorlon = x;
	*yorlat = y;
	*zoralt = z;
}

void CGlbGlobeView::WorldToWGS84(glbDouble* xorlon,glbDouble* yorlat,glbDouble* zoralt)
{
	double lon,lat,alt;
	g_ellipsmodel->convertXYZToLatLongHeight(*xorlon,*yorlat,*zoralt,lat,lon,alt);
	*xorlon = osg::RadiansToDegrees(lon);
	*yorlat = osg::RadiansToDegrees(lat);
	*zoralt = alt;
}

void CGlbGlobeView::WindowToScreen(glbDouble &sx,glbDouble &sy)
{
	sy = mpr_osgviewer->getCamera()->getViewport()->height() - sy;
}

void CGlbGlobeView::ScreenToWindow(glbDouble &sx,glbDouble &sy)
{
	sy = mpr_osgviewer->getCamera()->getViewport()->height() - sy;
}

void CGlbGlobeView::WindowToScene(glbDouble &sx,glbDouble &sy)
{
	if (mpr_hwnd)
	{
		//GlbLogWOutput(GLB_LOGTYPE_INFO,L"wx %lf wy %lf ---->",sx,sy);
		RECT rect;
		::GetWindowRect(mpr_hwnd,&rect);
		sx -= rect.left;
		sy = rect.bottom - sy;	
		//GlbLogWOutput(GLB_LOGTYPE_INFO,L"sx %lf sy %lf \n",sx,sy);
	}
}

void CGlbGlobeView::SceneToWindow(glbDouble &sx,glbDouble &sy)
{
	if (mpr_hwnd)
	{
		RECT rect;
		::GetWindowRect(mpr_hwnd,&rect);
		sx += rect.left;
		sy = rect.bottom - sy;	
	}
}

void CGlbGlobeView::getScreenFocusPt(glbDouble& x,glbDouble& y)
{
	x = mpr_osgviewer->getCamera()->getViewport()->width();
	x = x/2;
	y = mpr_osgviewer->getCamera()->getViewport()->height();
	y = y/2;
}

osg::Camera* CGlbGlobeView::GetOsgCamera()
{
	if(mpr_osgviewer)
		return mpr_osgviewer->getCamera();
	else
		return NULL;
}

glbDouble CGlbGlobeView::GetYaw()
{
	if(!mpr_manipulatormanager.valid())
		return 0;
	return mpr_manipulatormanager->GetYaw();
}

glbBool CGlbGlobeView::SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance)
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->SetYaw(yaw,old_focus,olddistance);
}

glbDouble CGlbGlobeView::GetPitch()
{
	if(!mpr_manipulatormanager.valid())
		return 0;
	return mpr_manipulatormanager->GetPitch();
}

glbBool CGlbGlobeView::SetPitch(glbDouble pitch,osg::Vec3d old_focus, glbDouble olddistance)
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->SetPitch(pitch,old_focus,olddistance);
}

glbBool CGlbGlobeView::GetCameraPos(osg::Vec3d& cameraPos)
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->GetCameraPos(cameraPos);
}

glbDouble CGlbGlobeView::GetCameraYaw()
{
	if(mpr_manipulatormanager.valid())		
		return mpr_manipulatormanager->GetCameraYaw();
	return 0.0;
}

glbDouble CGlbGlobeView::GetCameraPitch()
{
	if(mpr_manipulatormanager.valid())		
		return mpr_manipulatormanager->GetCameraPitch();
	return 0.0;
}

glbBool CGlbGlobeView::GetFocusPos(osg::Vec3d& focusPos)
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->GetFocusPos(focusPos);
}

glbDouble CGlbGlobeView::GetDistance()
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->GetDistance();
}

void CGlbGlobeView::SetCameraParam(glbDouble yaw, glbDouble pitch, osg::Vec3d focusPos, glbDouble distance)
{
	if(mpr_manipulatormanager.valid())
		mpr_manipulatormanager->SetCameraParam(yaw,pitch,focusPos,distance);
}

void CGlbGlobeView::Drag(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2)
{
	if(mpr_manipulatormanager.valid())
		mpr_manipulatormanager->Drag(ptx1,pty1,ptx2,pty2);
}
void CGlbGlobeView::Rotate(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2)
{
	if(mpr_manipulatormanager.valid())
		mpr_manipulatormanager->Rotate(ptx1,pty1,ptx2,pty2);
}
void CGlbGlobeView::Zoom(glbBool isScrollUp)
{
	if(mpr_manipulatormanager.valid())
		mpr_manipulatormanager->Zoom(isScrollUp);
}

glbBool CGlbGlobeView::JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt,glbDouble distance,glbDouble yaw, glbDouble pitch)
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->JumpTo(xOrlon,yOrlat,zOrAlt,distance,yaw,pitch);
}

glbBool CGlbGlobeView::FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt,glbDouble distance, glbDouble yaw,glbDouble pitch,glbDouble seconds)
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->FlyTo(xOrlon,yOrlat,zOrAlt,distance,yaw,pitch,seconds);
}
glbBool CGlbGlobeView::FlyAround(glbDouble lonORx,glbDouble latORy,glbDouble zOrAlt,glbDouble seconds,glbBool repeat)
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->FlyAround(lonORx,latORy,zOrAlt,seconds,repeat);
}
glbBool CGlbGlobeView::IsFly()
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->IsFlying();
}
glbBool CGlbGlobeView::StopFly()
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->StopFlying();
}

glbBool CGlbGlobeView::Move(glbDouble anglex,glbDouble angley)
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->Move(anglex,angley);
}

glbBool CGlbGlobeView::TranslateCamera(glbDouble x,glbDouble y,glbDouble z)
{
	if(!mpr_manipulatormanager.valid())
		return false;
	return mpr_manipulatormanager->TranslateCamera(x,y,z);
}

void CGlbGlobeView::UpdateTerrainElevationOfCameraPos(glbDouble elevation)
{
	mpr_cameraElevation = elevation;
	mpr_manipulatormanager->UpdateCameraElevate(elevation);
}

void CGlbGlobeView::UpdateTerrainElevationOfFoucsPos(glbDouble elevation)
{
	mpr_focusElevation = elevation;
	mpr_manipulatormanager->UpdateFocusElevate(elevation);
}

glbDouble CGlbGlobeView::GetTerrainElevationOfCameraPos()
{
	return mpr_cameraElevation;
}

glbDouble CGlbGlobeView::GetTerrainElevationOfFoucsPos()
{
	return mpr_focusElevation;
}

void CGlbGlobeView::UpdateFocusElevate(/*glbBool bInter, osg::Vec3d interPos*/)
{
	//mpr_bInter = bInter;
	//mpr_interPos = interPos;
	mpr_manipulatormanager->UpdateFocusElevate(0);
}

// glbBool CGlbGlobeView::getSightLineInterTerrain(osg::Vec3d& interPos)
// {
//		interPos = mpr_interPos;
//		return mpr_bInter;
// }

void CGlbGlobeView::ShowNavigator(bool isShow)
{
	if(mpr_navigator.valid())
	{
		osg::Node::NodeMask masknum = mpr_navigator->getNodeMask();
		if(masknum == 0x02 && !isShow)
			mpr_navigator->setNodeMask(0x0);
		else if(masknum == 0x0 && isShow)
			mpr_navigator->setNodeMask(0x02);
	}
}
void CGlbGlobeView::ShowElevationRuler(bool isShow)
{
 	if(mpr_ruler.valid())
 	{
 		osg::Node::NodeMask masknum = mpr_ruler->getNodeMask();
 		if(masknum == 0x02 && !isShow)
 			mpr_ruler->setNodeMask(0x0);
 		else if(masknum == 0x0 && isShow)
 			mpr_ruler->setNodeMask(0x02);
 	}
}
void CGlbGlobeView::ShowStatusBar(bool isShow)
{
	if(mpr_statusbar.valid())
	{
		osg::Node::NodeMask masknum = mpr_statusbar->getNodeMask();
		if(masknum == 0x02 && !isShow)
			mpr_statusbar->setNodeMask(0x0);
		else if(masknum == 0x0 && isShow)
			mpr_statusbar->setNodeMask(0x02);
	}
}
void CGlbGlobeView::ShowLogo(bool isShow)
{
	if(mpr_logo.valid())
	{
		osg::Node::NodeMask masknum = mpr_logo->getNodeMask();
		if(masknum == 0x02 && !isShow)
			mpr_logo->setNodeMask(0x0);
		else if(masknum == 0x0 && isShow)
			mpr_logo->setNodeMask(0x02);
	}
}
void CGlbGlobeView::ShowScaleRuler(bool isShow)
{
	if(mpr_scale.valid())
	{
		osg::Node::NodeMask masknum = mpr_scale->getNodeMask();
		if(masknum == 0x02 && !isShow)
			mpr_scale->setNodeMask(0x0);
		else if(masknum == 0x0 && isShow)
			mpr_scale->setNodeMask(0x02);
	}
}

void CGlbGlobeView::ShowCentermark(bool isShow)
{
	if(mpr_centermark.valid())
	{
		osg::Node::NodeMask masknum = mpr_centermark->getNodeMask();
		if(masknum == 0x02 && !isShow)
			mpr_centermark->setNodeMask(0x0);
		else if(masknum == 0x0 && isShow)
			mpr_centermark->setNodeMask(0x02);
	}
}
void CGlbGlobeView::initElements()
{
	//slaveCamera
	{
		osg::ref_ptr<osg::Node> node = mpr_p_root->getChild(0);
		if(node.valid() && node->getName() == "slaveCamera")
		{
			mpr_p_root->removeChild(node.get());
			node = NULL;
		}
	}

	//BaseGlobe
	{
		osg::ref_ptr<osg::Node> node = mpr_p_root->getChild(0);
		if(node.valid() && node->getName() == "baseGlobe")
		{
			mpr_p_root->removeChild(node.get());
			node = NULL;
		}
	}

	//天空
	if(mpr_sky)
	{
		mpr_p_root->removeChild(mpr_sky->GetOsgNode());
	}
	mpr_sky=NULL;
	CGlbGlobeElementFactory* factory = CGlbGlobeElementFactory::GetInstance();
	mpr_sky = factory->CreateElement(L"glbsky");
	if(mpr_sky!=NULL)
	{
		mpr_sky->Initialize(this);
		mpr_p_root->addChild(mpr_sky->GetOsgNode());
		mpr_sky->Show(true);
	}	

	OutputDebugString(L"	sky Element end.\n");

	//状态栏
	if(mpr_statusbar)
	{
		mpr_p_root->removeChild(mpr_statusbar.get());
	}
	mpr_statusbar = NULL;	
	RECT rect;
	::GetClientRect(this->GetHWnd(),&rect);		
 	int height = rect.bottom - rect.top;
	int width  = rect.right  - rect.left;
	mpr_statusbar = new CGlbGlobeViewStatusBar(this,width,height,mpr_globe.get());
	ShowStatusBar(true);mpr_p_root->addChild(mpr_statusbar.get());
	
	OutputDebugString(L"	statusbar Element end.\n");

	//导航盘
	if(mpr_navigator)
	{
		mpr_p_root->removeChild(mpr_navigator.get());
	}
	mpr_navigator = NULL;
	mpr_navigator = new CGlbGlobeViewNavigator(this,width,height,mpr_globe.get());
	ShowNavigator(true);mpr_p_root->addChild(mpr_navigator.get());
	
	OutputDebugString(L"	navigator Element end.\n");

	//导航标尺
	if(mpr_ruler)
	{
		mpr_p_root->removeChild(mpr_ruler.get());
	}
	mpr_ruler = NULL;
	mpr_ruler = new CGlbGlobeViewElevationRuler(this,width,height,mpr_globe.get());
	ShowElevationRuler(true);mpr_p_root->addChild(mpr_ruler.get());	

	OutputDebugString(L"	ruler Element end.\n");

	//中心标记
	/*if(mpr_centermark)
	{
		mpr_p_root->removeChild(mpr_centermark.get());
	}
	mpr_centermark=NULL;
	mpr_centermark = new CGlbGlobeViewCentermark(this,width,height,mpr_globe.get());
	ShowCentermark(true);mpr_p_root->addChild(mpr_centermark.get());*/	
	////Logo
	//if(mpr_logo)
	//{
	//	mpr_p_root->removeChild(mpr_logo.get());
	//}
	//mpr_logo=NULL;
	//mpr_logo = new CGlbGlobeViewLogo(this,width,height,mpr_globe);
	//ShowLogo(true);mpr_p_root->addChild(mpr_logo.get());
	//Scale
	if(mpr_scale)
	{
		mpr_p_root->removeChild(mpr_scale.get());
	}
	mpr_scale=NULL;
	mpr_scale = new CGlbGlobeViewScale(this,width,height,mpr_globe.get());
	ShowScaleRuler(true);
	//ShowLogo(true);
	mpr_p_root->addChild(mpr_scale.get());

	OutputDebugString(L"	scaleRuler Element end.\n");
}
//显示星空
void CGlbGlobeView::ShowStars(bool isShow)
{
	if(mpr_sky!=NULL)
	{
		mpr_sky->Show(isShow,L"glbstars");
	}
	return;	
}
//显示太阳
void CGlbGlobeView::ShowSun(bool isShow)
{
	if(mpr_sky!=NULL)
	{
		mpr_sky->Show(isShow,L"glbsun");
	}

	//if (mpr_p_root)
	//{
	//	osg::Node* sunLight = NULL;
	//	unsigned int num = mpr_p_root->getNumChildren();
	//	for(unsigned int i = 0; i < num; i++)
	//	{
	//		if (mpr_p_root->getChild(i)->getName()=="sunLight")
	//		{
	//			sunLight = mpr_p_root->getChild(i);
	//			break;
	//		}
	//	}
	//	if (isShow)
	//	{
	//		if (!sunLight)
	//			sunLight= createSunLight();
	//		mpr_p_root->addChild(sunLight);
	//	}
	//	else
	//	{
	//		if (sunLight)
	//			mpr_p_root->removeChild(sunLight);
	//	}
	//}
	return;	
}
//显示月亮
void CGlbGlobeView::ShowMoon(bool isShow)
{
	if(mpr_sky!=NULL)
	{
		mpr_sky->Show(isShow,L"glbmoon");
	}
	return;	
}
//显示大气
void CGlbGlobeView::ShowAtmosphere(bool isShow)
{
	if(mpr_sky!=NULL)
	{
		mpr_sky->Show(isShow,L"glbatmosphere");
	}
	return;	
}
//显示天穹
void CGlbGlobeView::ShowSkydom(bool isShow)
{	
	//return;
	if(mpr_sky!=NULL)
	{
		mpr_sky->Show(isShow,L"glbskybox");
	}
	return;	
}
//设置许可服务
bool CGlbGlobeView::SetLicSrv(CGlbWString ip,glbInt32 port)
{
	if(ip.length()==0 || port<0)
	{
		GlbSetLastError(L"无效参数");
		return false;
	}
	g_srvLic.close();
	g_srvIP = ip;
	g_srvPort=port;
	return true;
}


osg::ref_ptr<osg::Geode> CGlbGlobeView::createPlane()
{
	//test code: build a plane
	osg::ref_ptr<osg::Vec3Array> vc  = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec2Array> coord = new osg::Vec2Array();
	glbInt32 wid = 18;
	glbInt32 hei = 9;
	for (glbInt32 j = -hei;j <= hei;j++)
	{
		for (glbInt32 i = -wid;i <= wid;i++)
		{
			vc->push_back(osg::Vec3((glbFloat)i*10000.0,(glbFloat)j*10000,0.0));
			coord->push_back(osg::Vec2((glbFloat)i/(glbFloat)(wid*2)+0.5f,(glbFloat)j/(glbFloat)(hei*2)+0.5f));
		}
	}
	osg::ref_ptr<osg::DrawElementsUInt> element= new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS,0);

	for (glbInt32 j = 0;j < hei*2;j++)
	{
		for (glbInt32 i = 0;i < wid*2;i++)
		{
			glbInt32 index = i+j*(wid*2+1);
			element->push_back(index);
			element->push_back(index+1);
			element->push_back(index+1+wid*2+1);
			element->push_back(index+wid*2+1);
		}
	}
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	geom->setVertexArray(vc.get());
	geom->setTexCoordArray(0,coord.get());
	geom->addPrimitiveSet(element.get());
	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
	color->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	geom->setColorArray(color.get());
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array();
	nc->push_back(osg::Vec3(0.0f,0.0f,1.0f));
	geom->setNormalArray(nc.get());
	geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	CGlbWString dpath = CGlbPath::GetExecDir();
	dpath += L"\\land_shallow_topo_2048.jpg";
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(dpath.ToString().c_str());
	osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;  
	tex->setImage(image.get());
	tex->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT); 
	tex->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setTextureAttributeAndModes(0,tex.get(),osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_BLEND,osg::StateAttribute::ON);
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::ON ); 
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	geom->setStateSet(stateset.get());
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom.get());
	return geode;
}

osg::ref_ptr<osg::Geode> CGlbGlobeView::createEarth()
{// test code : build a earth
	osg::ref_ptr<osg::Vec3dArray> vc = new osg::Vec3dArray();
	osg::ref_ptr<osg::Vec2dArray> vt = new osg::Vec2dArray();
	int upperBound = 63;
	double scaleFactor = (double)1/63;
	double maxLat = 90.0;
	double minLon = -180.0;
	double latrange = 180.0;
	double lonrange = 360.0;
	osg::ref_ptr<osg::EllipsoidModel> _ellips = new osg::EllipsoidModel(osg::WGS_84_RADIUS_EQUATOR,osg::WGS_84_RADIUS_POLAR);
	for(int i = 0; i < 64; i++)
	{
		for(int j = 0; j < 64; j++)
		{	
			osg::Vec3d geocentric;
			_ellips->convertLatLongHeightToXYZ(
				osg::DegreesToRadians( maxLat - scaleFactor*latrange*i ),
				osg::DegreesToRadians( minLon + scaleFactor*lonrange*j ),
				0.0,
				geocentric.x(), geocentric.y(), geocentric.z() );
			vc->push_back(geocentric);
			vt->push_back(osg::Vec2d( j * scaleFactor,1.0 - i * scaleFactor));
		}
	}
	int meshPointCount = 64;
	osg::ref_ptr<osg::DrawElementsUInt> index = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
	for(int i = 0; i < upperBound; i++)
	{
		for(int j = 0; j < upperBound; j++)
		{
			index->push_back(i*meshPointCount + j);
			index->push_back((i+1)*meshPointCount + j);
			index->push_back(i*meshPointCount + j+1);

			index->push_back(i*meshPointCount + j+1);
			index->push_back((i+1)*meshPointCount + j);
			index->push_back((i+1)*meshPointCount + j+1);
		}
	}
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	geom->setVertexArray(vc.get());
	geom->addPrimitiveSet(index.get());
	geom->setTexCoordArray(0,vt.get());
	osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
	color->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	geom->setColorArray(color.get());
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array();
	nc->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
	geom->setNormalArray(nc.get());
	geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom.get());

	CGlbWString dpath = CGlbPath::GetExecDir();
	dpath += L"\\land_shallow_topo_2048.jpg";
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(dpath.ToString().c_str());
	osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;  
	tex->setImage(image.get());
	tex->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE); 
	tex->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setTextureAttributeAndModes(0,tex.get(),osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_BLEND,osg::StateAttribute::ON);
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::ON ); 
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	geom->setStateSet(stateset.get());

	//CGlbOpacityVisitor nodeVisiter(50);
	//geode->accept(nodeVisiter);
	//stateset->setMode(GL_BLEND, osg::StateAttribute::ON);	
	//stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	//osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);	
	//stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	return geode;
}

void CGlbGlobeView::WorldToScreen(glbDouble &sx,glbDouble &sy,glbDouble &sz)
{
	osg::Camera* p_camera = mpr_osgviewer->getCamera();
	osg::Matrixd VPW = p_camera->getViewMatrix() *
		p_camera->getProjectionMatrix() *
		p_camera->getViewport()->computeWindowMatrix();
	osg::Vec3d window = osg::Vec3d(sx,sy,sz) * VPW;
	sx = window.x();
	//y = p_camera->getViewport()->height() - window.y();
	sy = window.y();
	sz = window.z();
}
void CGlbGlobeView::ScreenToWorld(glbDouble &sx,glbDouble &sy,glbDouble &sz)
{// 屏幕-〉世界 
	osg::Camera* p_camera = mpr_osgviewer->getCamera();
	osg::Matrixd VPW = p_camera->getViewMatrix() *
		p_camera->getProjectionMatrix() *
		p_camera->getViewport()->computeWindowMatrix();
	osg::Matrixd inverseVPW;
	inverseVPW.invert(VPW);
	// y值重定位
	//y = p_camera->getViewport()->height() - y;
	osg::Vec3d world = osg::Vec3d(sx,sy,sz) * inverseVPW;
	sx = world.x();
	sy = world.y();
	sz = world.z();
}

glbBool CGlbGlobeView::contains(const osg::BoundingBox& bb)
{
	osg::Camera* p_osgcamera = mpr_osgviewer->getCamera();
	osg::Matrixd _modelView = p_osgcamera->getViewMatrix();
	osg::Matrixd _projection = p_osgcamera->getProjectionMatrix();
	osg::Polytope cv;
	cv.setToUnitFrustum();
	cv.transformProvidingInverse((_modelView)*(_projection));
	// 视锥体包含至少部分boundbox
	if (cv.contains(bb))
		return true;

	//osg::Polytope::PlaneList& _planeList = cv.getPlaneList();
	//for(osg::Polytope::PlaneList::const_iterator itr=_planeList.begin();
	//	itr!=_planeList.end();
	//	++itr)
	//{		
	//	int res=itr->intersect(bb);
	//	if (res==0) 
	//		return true;		
	//}
	return false;	
}


glbBool CGlbGlobeView::Pick( glbInt32 mousex,glbInt32 mousey, std::vector<glbref_ptr<CGlbGlobeObject>>&result,std::vector<osg::Vec3d>&InterPoints)
{
	if (!mpr_globe)//没有场景直接返回false
		return false;

	glbDouble _x = mousex;
	glbDouble _y = mousey;

	WindowToScreen(_x,_y);

	osg::Vec3d vStart(_x,_y,0.0);
	osg::Vec3d vEnd(_x,_y,1.0);	
	//ScreenToWorld(vStart.x(),vStart.y(),vStart.z());
	//ScreenToWorld(vEnd.x(),vEnd.y(),vEnd.z());

	osg::Camera* p_camera = mpr_osgviewer->getCamera();
	osg::Matrixd VPW = p_camera->getViewMatrix() *
		p_camera->getProjectionMatrix() *
		p_camera->getViewport()->computeWindowMatrix();
	osg::Matrixd inverseVPW;
	inverseVPW.invert(VPW);
	vStart = vStart * inverseVPW;
	vEnd = vEnd * inverseVPW;

	// 1.选取非贴地形绘制对象
	//std::vector<osg::Vec3d> pointResult;
	bool bSucc = mpr_globe->mpr_sceneobjIdxManager->Query(vStart,vEnd,result,InterPoints);
	//if (bSucc==false)
	//{// 测试
	//	osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, mousex, mousey);//(vStart, vEnd);
	//	//osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, vStart, vEnd);//(vStart, vEnd);
	//	osgUtil::IntersectionVisitor iv(picker.get());
	//	mpr_osgviewer->getCamera()->accept(iv);
	//	//GetGlobe()->mpr_p_objects_groundnode->accept(iv);		
	//	if (picker->containsIntersections())
	//	{
	//		osgUtil::LineSegmentIntersector::Intersections& intersections = picker->getIntersections();
	//		osgUtil::LineSegmentIntersector::Intersections::iterator itr = intersections.begin();
	//		while(itr != intersections.end())
	//		{
	//			//const osgUtil::LineSegmentIntersector::Intersection& hit = *itr;
	//			//osg::Vec3d hitPt = hit.getWorldIntersectPoint();
	//			osg::NodePath nodepath = itr->nodePath;
	//			//double dis = (vStart-hitPt).length();
	//			itr++;
	//		}			
	//	}
	//}

	// 2. 选取 贴地形绘制对象
	glbDouble lonOrX,latOrY,atlOrZ;
	mpr_globe->ScreenToTerrainCoordinate(_x,_y,lonOrX,latOrY,atlOrZ);
	osg::Vec3d terrainPos(lonOrX,latOrY,atlOrZ);	
	if (mpr_globe->GetType()==GLB_GLOBETYPE_GLOBE)
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(latOrY),osg::DegreesToRadians(lonOrX),atlOrZ,terrainPos.x(),terrainPos.y(),terrainPos.z());
	/* 根据terrainPos计算距离阀值 4个像素
	*/
	double _pixels = 4;
	osg::Vec3d screenPos=terrainPos;
	WorldToScreen(screenPos.x(),screenPos.y(),screenPos.z());
	osg::Vec3d topPos(screenPos.x(),screenPos.y()+_pixels,screenPos.z());
	ScreenToWorld(topPos.x(),topPos.y(),topPos.z());
	osg::Vec3d rightPos(screenPos.x()+_pixels,screenPos.y(),screenPos.z());
	ScreenToWorld(rightPos.x(),rightPos.y(),rightPos.z());
	double dis1 = (terrainPos-topPos).length();
	double dis2 = (terrainPos-rightPos).length();
	double nearest_dis = max(dis1,dis2);
	// 查询
	osg::Vec3d tPoint(lonOrX,latOrY,atlOrZ);	
	std::vector<glbref_ptr<CGlbGlobeObject>> result2;
	bool bSucc2 = mpr_globe->mpr_sceneobjIdxManager->QueryOnTerrainObjects(tPoint,result2,nearest_dis);
	for(size_t k = 0; k < result2.size(); k++)
	{
		result.push_back(result2.at(k));		
		InterPoints.push_back(terrainPos);
	}

	// 3. 选取 像素 绘制的点、线
	//if (!bSucc && !bSucc2)
	{// 用棱锥体求交的方法来获取 点状地物 2013.12.31
		double xMin = _x - 3;
		double yMin = _y - 3;
		double xMax = _x + 3;
		double yMax = _y + 3;
		double zNear = 0.0;//window coord
		osg::Polytope _polytope;
		_polytope.add(osg::Plane(1.0, 0.0, 0.0, -xMin));
		_polytope.add(osg::Plane(-1.0,0.0 ,0.0, xMax));
		_polytope.add(osg::Plane(0.0, 1.0, 0.0,-yMin));
		_polytope.add(osg::Plane(0.0,-1.0,0.0, yMax));
		_polytope.add(osg::Plane(0.0,0.0,1.0, -zNear));

		osg::Polytope transformedPolytope;
		osg::Camera* p_camera = mpr_osgviewer->getCamera();
		osg::Matrix VPW = p_camera->getViewMatrix() *
			p_camera->getProjectionMatrix() *
			p_camera->getViewport()->computeWindowMatrix();	

		transformedPolytope.setAndTransformProvidingInverse(_polytope, VPW);
		// end 生成polytope	

		std::vector<glbref_ptr<CGlbGlobeObject>> _rr;
		bSucc = mpr_globe->mpr_sceneobjIdxManager->Query(transformedPolytope,_rr,false);
		if (_rr.size()>0)
		{		
			//double x,y,z;
			std::vector<glbref_ptr<CGlbGlobeObject>>::iterator itr = _rr.begin();
			while(itr!= _rr.end())
			{							
				CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>((*itr).get());
				if (robj)
				{
					bool bSelected = _isObjectHaveSelected(robj,result);
					if (!bSelected)
					{
						result.push_back(*itr);
						//robj->GetBound()->GetCenter(&x,&y,&z);
						//InterPoints.push_back(osg::Vec3d(x,y,z));
					}
				}										
				itr++;
			}
		}
	}// end 棱锥求交

	if (result.size()>0)
		return true;
	return false;
}

glbBool CGlbGlobeView::PickNearestObject( glbInt32 mousex,glbInt32 mousey, osg::Vec3d& InterPoint)
{
	if (!mpr_globe)//没有场景直接返回false
		return false;
#ifdef _DEBUG
	osg::Timer_t startFrameTick = osg::Timer::instance()->tick();	
#endif

	glbBool rt = false;

	glbDouble _x = mousex;
	glbDouble _y = mousey;

	WindowToScreen(_x,_y);

	osg::Vec3d vStart(_x,_y,0.0);
	osg::Vec3d vEnd(_x,_y,1.0);	

	osg::Camera* p_camera = mpr_osgviewer->getCamera();
	osg::Matrixd VPW = p_camera->getViewMatrix() *
		p_camera->getProjectionMatrix() *
		p_camera->getViewport()->computeWindowMatrix();
	osg::Matrixd inverseVPW;
	inverseVPW.invert(VPW);
	vStart = vStart * inverseVPW;
	vEnd = vEnd * inverseVPW;

	// 1.选取非贴地形绘制对象
	std::vector<glbref_ptr<CGlbGlobeObject>> result;
	std::vector<osg::Vec3d> pointResult;
	bool bSucc = mpr_globe->mpr_sceneobjIdxManager->Query(vStart,vEnd,result,pointResult);

	if (pointResult.size()>0)
	{
		InterPoint = pointResult.at(0);
		rt = true;
	}
	else
	{// 找不到
		//// 选取 像素 绘制的点、线		
		//{// 用棱锥体求交的方法来获取 点状地物 2013.12.31
		//	double xMin = _x - 3;
		//	double yMin = _y - 3;
		//	double xMax = _x + 3;
		//	double yMax = _y + 3;
		//	double zNear = 0.0;//window coord
		//	osg::Polytope _polytope;
		//	_polytope.add(osg::Plane(1.0, 0.0, 0.0, -xMin));
		//	_polytope.add(osg::Plane(-1.0,0.0 ,0.0, xMax));
		//	_polytope.add(osg::Plane(0.0, 1.0, 0.0,-yMin));
		//	_polytope.add(osg::Plane(0.0,-1.0,0.0, yMax));
		//	_polytope.add(osg::Plane(0.0,0.0,1.0, -zNear));

		//	osg::Polytope transformedPolytope;
		//	osg::Camera* p_camera = mpr_osgviewer->getCamera();
		//	osg::Matrix VPW = p_camera->getViewMatrix() *
		//		p_camera->getProjectionMatrix() *
		//		p_camera->getViewport()->computeWindowMatrix();	

		//	transformedPolytope.setAndTransformProvidingInverse(_polytope, VPW);
		//	// end 生成polytope	

		//	std::vector<glbref_ptr<CGlbGlobeObject>> _rr;
		//	bSucc = mpr_globe->mpr_sceneobjIdxManager->Query(transformedPolytope,_rr,false);
		//	if (_rr.size()>0)
		//	{		
		//		double x,y,z;
		//		std::vector<glbref_ptr<CGlbGlobeObject>>::iterator itr = _rr.begin();
		//		while(itr!= _rr.end())
		//		{							
		//			CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>((*itr).get());
		//			if (robj)
		//			{
		//				bool bSelected = _isObjectHaveSelected(robj,result);
		//				if (!bSelected)
		//				{
		//					result.push_back(*itr);
		//					// 交点不准,会导致操控器鼠标按下后画面漂移 2016.9.18
		//					robj->GetBound()->GetCenter(&x,&y,&z);
		//					pointResult.push_back(osg::Vec3d(x,y,z));
		//					break;//找到一个就可以结束了
		//				}
		//			}										
		//			itr++;
		//		}
		//	}
		//}// end 棱锥求交

		//if (pointResult.size()>0)
		//{
		//	InterPoint = pointResult.at(0);
		//	rt = true;
		//}
	}
#ifdef _DEBUG
	osg::Timer_t currTime = osg::Timer::instance()->tick();
	double pickTime = osg::Timer::instance()->delta_m(startFrameTick,currTime);
	if (pickTime > 5.0)
	{
		int isLineinter = pointResult.size()>0 ? 1 : 0;
		wchar_t buff[128];
		swprintf(buff,L"PickNearestObject use time %lf ms. object count %d. \n ", pickTime, pointResult.size());
		OutputDebugString(buff);
	}
#endif

	return rt;
}


#include "GlbGlobeMath.h"
#include "GlbPoint.h"
glbBool CGlbGlobeView::Pick( CGlbPolygon *region,std::vector<glbref_ptr<CGlbGlobeObject>>&result,glbDouble minAltitude, glbDouble maxAltitude)
{
	if (!mpr_globe)
		return false;
	GlbGlobeTypeEnum globeType = mpr_globe->GetType();
	//1.选取非贴地形绘制对象
	CGlbExtent extBox;
	CGlbExtent onTerrainExtent;
	CGlbLine* ringLn = (CGlbLine*) region->GetExtRing();
	glbInt32 coordDimension = region->GetCoordDimension();
	glbInt32 ptCnt = ringLn->GetCount();
	glbDouble* pts = (glbDouble*)ringLn->GetPoints();
	glbDouble ptx,pty;
	osg::Vec3d pos;
	for (glbInt32 i = 0; i < ptCnt; i++)
	{
		ringLn->GetPoint(i,&ptx,&pty);
		
		// 非贴地外包
		if (globeType==GLB_GLOBETYPE_GLOBE)
		{
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),minAltitude,pos.x(),pos.y(),pos.z());
			extBox.Merge(pos.x(),pos.y(),pos.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),maxAltitude,pos.x(),pos.y(),pos.z());
			extBox.Merge(pos.x(),pos.y(),pos.z());
		}
		else
		{
			extBox.Merge(ptx,pty,minAltitude);
			extBox.Merge(ptx,pty,maxAltitude);
		}

		// 贴地外包
		onTerrainExtent.Merge(ptx,pty,0);
	}

	glbDouble west,east,south,north;
	onTerrainExtent.Get(&west,&east,&south,&north);
	
	// 1. 粗交
	std::vector<glbref_ptr<CGlbGlobeObject>> temp_result;
	// 1.1 .box粗交求选中-非贴地形绘制对象
	bool bSucc = mpr_globe->mpr_sceneobjIdxManager->QueryObjects(&extBox,temp_result);	
	// 1.2 首先用外框过滤一遍 - 贴地形绘制对象
	bool bSucc2 = mpr_globe->mpr_sceneobjIdxManager->QueryOnTerrainObjects(&onTerrainExtent,temp_result);

	//2 精细求选中
	if (temp_result.size()>0){
		for (size_t k = 0; k < temp_result.size(); k++)
		{
			CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*> (temp_result.at(k).get());
			if (robj)
			{
				glbref_ptr<IGlbGeometry> glbGeom = robj->GetOutLine();
				if (!glbGeom) continue;
				CGlbExtent* extgeo = const_cast<CGlbExtent*>(glbGeom->GetExtent());
				if (!extgeo) continue;				
				{// 边界rect必须相交,才有可能选中
					glbDouble minX,maxX,minY,maxY;
					extgeo->Get(&minX,&maxX,&minY,&maxY);
					minX = max(minX,west);
					minY = max(minY,south);
					maxX = min(maxX,east);
					maxY = min(maxY,north);

					// 没有交集，直接转入下一个obj
					if ( (minX > maxX) || (minY > maxY) )
						continue;
				}

				glbDouble ptx,pty;
				CGlbPoint* pt3d = dynamic_cast<CGlbPoint*> (glbGeom.get());
				CGlbMultiPoint* p_pts = dynamic_cast<CGlbMultiPoint*> (glbGeom.get());
				CGlbLine* p_ln = dynamic_cast<CGlbLine*> (glbGeom.get());
				CGlbMultiLine* p_lns =  dynamic_cast<CGlbMultiLine*> (glbGeom.get());
				CGlbMultiPolygon* p_polys = dynamic_cast<CGlbMultiPolygon*> (glbGeom.get());
				if (pt3d)
				{// 单点对象
					pt3d->GetXY(&ptx,&pty);					
					bool bInPoly = PtInPolygon(ptx,pty,pts,ptCnt,coordDimension);
					//bool bInPoly = PtInPolygonExt(ptx,pty,pts,ptCnt,coordDimension);
					if (bInPoly)
					{// 点在选择区域内
						result.push_back(robj);						
					}
				}
				else if (p_pts)
				{// 点状对象 [点在选择区域内]					
					glbInt32 ptCnts = p_pts->GetCount();
					for(glbInt32 i = 0; i < ptCnts; i++)
					{
						p_pts->GetPoint(i,&ptx,&pty);
						bool bInPoly = PtInPolygon(ptx,pty,pts,ptCnt,coordDimension);
						if (bInPoly)
						{// 点在选择区域内
							result.push_back(robj);
							break;
						}
					}					
				}				
				else if (p_ln)
				{// 单线对象
					glbBool bInter = Intersect(p_ln,region);
					if (bInter)
					{// 线与region相交
						result.push_back(robj);						
					}
				}
				else if (p_lns)
				{// 线状对象 
					glbInt32 lnCnt = p_lns->GetCount();
					for(glbInt32 i =0;i < lnCnt; i++)
					{
						CGlbLine* p_ln = const_cast<CGlbLine*>(p_lns->GetLine(i));
						glbBool bInter = Intersect(p_ln,region);
						if (bInter)
						{// 线与region相交
							result.push_back(robj);
							break;
						}
					}				
				}
				else if (p_polys)
				{// 面状地物 [两个面有交集]
					glbInt32 polyCnt = p_polys->GetCount();
					for(glbInt32 i = 0; i < polyCnt; i++)
					{
						CGlbPolygon* p_poly = const_cast<CGlbPolygon*>(p_polys->GetPolygon(i));
						glbBool bInter = Intersect(p_poly,region);
						if (bInter)
						{// 线与region相交
							result.push_back(robj);
						}	
					}								
				}
			}
		}
	}
	
	if (result.size()>0)
		return true;
	return false;
}


bool CGlbGlobeView::_isObjectHaveSelected(CGlbGlobeObject* obj, std::vector<glbref_ptr<CGlbGlobeObject>>& result)
{
	if (obj==NULL)	return true;

	if (result.size()<=0) return false;

	std::vector<glbref_ptr<CGlbGlobeObject>>::iterator itr = result.begin();
	while(itr != result.end())
	{
		CGlbGlobeObject* ro = itr->get();
		if (!ro) continue;
		if (obj->GetId() == ro->GetId() && obj == ro)
			return true;
		itr++;
	}
	return false;
}

glbDouble CGlbGlobeView::GetCurrentPixelWidth()
{
	if(mpr_scale == NULL)
		return -DBL_MAX;
	glbInt32 wid = ::GetSystemMetrics(SM_CXSCREEN);
	return mpr_scale->GetCurrentScale() * 40 / wid;
}

#include <osg/LightModel>
osg::Node* CGlbGlobeView::createSunLight()
{
	osg::LightSource* sunLightSource = new osg::LightSource;
	sunLightSource->setName("sunLight");

	osg::Light* sunLight = new osg::Light;
	sunLight->setLightNum(1);
	sunLightSource->setLight(sunLight);
	sunLight->setPosition( osg::Vec4( 0.1f, 0.1f, 0.1f, 0.0f ) ); //方向(0.1,0.1,0.1)	
	sunLight->setAmbient( osg::Vec4( 0.2f, 0.2f, 0.2f, 1.0f ) );	

	sunLightSource->setLocalStateSetModes( osg::StateAttribute::ON );
	sunLightSource->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	//sunLightSource->getOrCreateStateSet()->setMode(GL_LIGHT0,osg::StateAttribute::ON);

	// 设置全局环境光为淡白色光 2016.11.22
	//osg::LightModel* lightModel = new osg::LightModel;
	//lightModel->setAmbientIntensity(osg::Vec4(0.8f,0.8f,0.8f,1.0f));
	//sunLightSource->getOrCreateStateSet()->setAttribute(lightModel);	

	// 开启光照
	osg::ref_ptr<osg::StateSet> ss = mpr_p_LightsRoot->getOrCreateStateSet();
	ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	ss->setMode(GL_LIGHT1, osg::StateAttribute::ON);

	return sunLightSource;
}// end CGlbGlobe::createSunLight

// 对称的太阳光源
osg::Node* CGlbGlobeView::createSymmetricalSunLight()
{
	osg::LightSource* sunLightSource = new osg::LightSource;
	sunLightSource->setName("sunLight");

	osg::Light* sunLight = new osg::Light;
	sunLight->setLightNum(2);
	sunLightSource->setLight(sunLight);
	sunLight->setPosition( osg::Vec4( -0.1f, -0.1f, -0.1f, 0.0f ) ); //方向(0.1,0.1,0.1)	
	sunLight->setAmbient( osg::Vec4( 0.2f, 0.2f, 0.2f, 1.0f ) );	

	sunLightSource->setLocalStateSetModes( osg::StateAttribute::ON );
	sunLightSource->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);

	// 开启光照
	osg::ref_ptr<osg::StateSet> ss = mpr_p_LightsRoot->getOrCreateStateSet();
	ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	ss->setMode(GL_LIGHT2, osg::StateAttribute::ON);

	return sunLightSource;
}

GlbGlobeLightingModeEnum CGlbGlobeView::GetLightingMode()
{
	GlbGlobeLightingModeEnum _lm = GLB_NO_LIGHT;
	if (mpr_osgviewer)
	{
		osg::View::LightingMode _mode = mpr_osgviewer->getLightingMode();
		switch(_mode)
		{
		case osg::View::LightingMode::HEADLIGHT:
			_lm = GLB_HEAD_LIGHT;
			break;
		case osg::View::LightingMode::SKY_LIGHT:
			_lm = GLB_SKY_LIGHT;
			break;
		case osg::View::LightingMode::NO_LIGHT:
			_lm = GLB_NO_LIGHT;
			break;
		}
	}	
	return _lm;
}

bool CGlbGlobeView::SetLightingMode(GlbGlobeLightingModeEnum mode)
{
	if (mpr_osgviewer)
	{
		osg::View::LightingMode _m;
		switch(mode)
		{
		case GLB_HEAD_LIGHT:
			_m = osg::View::LightingMode::HEADLIGHT;
			break;
		case GLB_SKY_LIGHT:
			_m = osg::View::LightingMode::SKY_LIGHT;
			break;
		case GLB_NO_LIGHT:
			_m = osg::View::LightingMode::NO_LIGHT;
			break;
		}
		mpr_osgviewer->setLightingMode(_m);
		return true;
	}
	return false;
}

 glbBool CGlbGlobeView::osgLight2glbLight(osg::Light* lgt, GlbGlobeLight& glbLight)
{
	if (lgt==NULL)
		return false;
	glbLight.Id = lgt->getLightNum();
	osg::Vec4 pos = lgt->getPosition();
	glbLight.Position[0] = pos.x();
	glbLight.Position[1] = pos.y();
	glbLight.Position[2] = pos.z();
	if (fabs(pos.w()-0)<0.0000001)
		glbLight.bParallel = true;
	else
		glbLight.bParallel = false;
	osg::Vec4 ambient = lgt->getAmbient();
	glbLight.Ambient[0] = ambient.x();
	glbLight.Ambient[1] = ambient.y();
	glbLight.Ambient[2] = ambient.z();
	osg::Vec4 diffuse = lgt->getDiffuse();
	glbLight.Diffuse[0] = diffuse.x();
	glbLight.Diffuse[1] = diffuse.y();
	glbLight.Diffuse[2] = diffuse.z();
	osg::Vec4 specular = lgt->getSpecular();
	glbLight.Specular[0] = specular.x();
	glbLight.Specular[1] = specular.y();
	glbLight.Specular[2] = specular.z();
	osg::Vec3 direction = lgt->getDirection();
	glbLight.SpotDir[0] = direction[0];
	glbLight.SpotDir[1] = direction[1];
	glbLight.SpotDir[2] = direction[2];
	glbLight.SpotCutoff = lgt->getSpotCutoff();
	glbLight.SpotExponent = lgt->getSpotExponent();
	glbLight.ConstantAttenuation = lgt->getConstantAttenuation();
	glbLight.LinearAttenuation = lgt->getLinearAttenuation();
	glbLight.QuadRaticAttenuation = lgt->getQuadraticAttenuation();

	return true;
}

 glbBool CGlbGlobeView::glbLight2osgLight(GlbGlobeLight& glbLight, osg::Light* lgt)
 {
	 if (lgt==NULL) 
		 lgt = new osg::Light;

	 lgt->setLightNum(glbLight.Id);
	 lgt->setPosition(osg::Vec4(glbLight.Position[0],glbLight.Position[1],glbLight.Position[2],glbLight.bParallel ? 0 : 1.0));
	 lgt->setAmbient(osg::Vec4(glbLight.Ambient[0],glbLight.Ambient[1],glbLight.Ambient[2],1.0));
	 lgt->setDiffuse(osg::Vec4(glbLight.Diffuse[0],glbLight.Diffuse[1],glbLight.Diffuse[2],1.0));
	 lgt->setSpecular(osg::Vec4(glbLight.Specular[0],glbLight.Specular[1],glbLight.Specular[2],1.0));
	 lgt->setDirection(osg::Vec3(glbLight.SpotDir[0],glbLight.SpotDir[1],glbLight.SpotDir[2]));
	 lgt->setSpotCutoff(glbLight.SpotCutoff);
	 lgt->setSpotExponent(glbLight.SpotExponent);
	 lgt->setConstantAttenuation(glbLight.ConstantAttenuation);
	 lgt->setLinearAttenuation(glbLight.LinearAttenuation);
	 lgt->setQuadraticAttenuation(glbLight.QuadRaticAttenuation);
	 return true;	  
 }

glbInt32 CGlbGlobeView::GetLightsCount()
{
	return mpr_lightsManager.size();
}

glbBool CGlbGlobeView::GetLightByIndex(glbInt32 idx, GlbGlobeLight& lgt)
{
	if (idx < 0 || idx > mpr_lightsId.size()-1)
		return false;
	
	glbInt32 lgtId = mpr_lightsId[idx];	
	return GetLightById(lgtId,lgt);
}

glbBool CGlbGlobeView::GetLightById(glbInt32 id, GlbGlobeLight& lgt)
{
	std::map<glbInt32,GlbGlobeLight>::iterator itr = mpr_lightsManager.find(id);
	if (itr != mpr_lightsManager.end())
	{
		lgt = itr->second;
		return true;
	}
	return false;
}

glbBool CGlbGlobeView::AddLight(GlbGlobeLight light)
{// 最多8盏灯（opengl）--id对应的 0-7	
	if (light.Id > 7 || light.Id < 1)
		return false;
	//std::map<int, int>  ddd;
	//std::map<int, int>::iterator iii = ddd.find(0);
	//if (iii != ddd.end())
	//{
	//	int kk = 1;
	//}

	std::map<glbInt32,GlbGlobeLight>::iterator itr = mpr_lightsManager.find(light.Id);
	if (itr == mpr_lightsManager.end())
	{// 没有找到已有ID
		mpr_lightsId.push_back(light.Id);
	}		
	
	mpr_lightsManager[light.Id] = light;
	//mpr_lightsManager.insert(make_pair(light.Id, light));	

	if (mpr_p_LightsRoot==NULL)
	{// 灯光		
		mpr_p_LightsRoot = new osg::Group;	
		mpr_p_LightsRoot->setName("LightRoot");
		mpr_p_root->addChild(mpr_p_LightsRoot);
	}
	osg::ref_ptr<osg::Light> lgt = new osg::Light;
	glbLight2osgLight(light,lgt);

	osg::LightSource* lightSource = new osg::LightSource;
	lightSource->setLight(lgt);

	lightSource->setLocalStateSetModes( osg::StateAttribute::ON );
	lightSource->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);

	// 添加光源节点
	mpr_p_LightsRoot->addChild(lightSource);

	// 开启光照
	osg::ref_ptr<osg::StateSet> ss = mpr_p_LightsRoot->getOrCreateStateSet();
	ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	if (light.onOrOFF)
		ss->setMode(GL_LIGHT0+light.Id, osg::StateAttribute::ON);
	else
		ss->setMode(GL_LIGHT0+light.Id, osg::StateAttribute::OFF);
	return true;
}

glbBool CGlbGlobeView::RemoveLight(glbInt32 id)
{
	std::map<glbInt32,GlbGlobeLight>::iterator itr = mpr_lightsManager.find(id);
	if (itr != mpr_lightsManager.end())
	{
		mpr_lightsManager.erase(itr);
		int num = mpr_p_LightsRoot->getNumChildren();
		for (int i = 0; i < num; i++)
		{
			osg::LightSource* ltSource = dynamic_cast<osg::LightSource*>(mpr_p_LightsRoot->getChild(i));
			if (ltSource)
			{
				osg::Light* lt = ltSource->getLight();
				if (lt->getLightNum()==id)
				{
					mpr_p_LightsRoot->removeChild(ltSource);
					// 关闭光照
					osg::ref_ptr<osg::StateSet> ss = mpr_p_LightsRoot->getOrCreateStateSet();					
					ss->setMode(GL_LIGHT0+id, osg::StateAttribute::OFF);

					return true;
				}
			}
		}		
	}

	std::vector<glbInt32>::iterator _itrId = mpr_lightsId.begin();
	while (_itrId != mpr_lightsId.end())
	{
		if (*_itrId == id)
		{
			_itrId = mpr_lightsId.erase(_itrId);
			continue;
		}
		_itrId++;
	}
	
	return false;
}

glbBool CGlbGlobeView::UpdateLight(GlbGlobeLight& light)
{
	std::map<glbInt32,GlbGlobeLight>::iterator itr = mpr_lightsManager.find(light.Id);
	if (itr != mpr_lightsManager.end())
	{
		mpr_lightsManager[light.Id] = light;
		// 默认光源0，是osg来管理的头灯/SKYLIGHT，直接修改其属性就可
		if (light.Id==0)
		{
			osg::Light* lgt = mpr_osgviewer->getLight();			
			lgt->setPosition(osg::Vec4(light.Position[0],light.Position[1],light.Position[2],light.bParallel ? 0 : 1.0));
			lgt->setAmbient(osg::Vec4(light.Ambient[0],light.Ambient[1],light.Ambient[2],1.0));
			lgt->setDiffuse(osg::Vec4(light.Diffuse[0],light.Diffuse[1],light.Diffuse[2],1.0));
			lgt->setSpecular(osg::Vec4(light.Specular[0],light.Specular[1],light.Specular[2],1.0));
			lgt->setDirection(osg::Vec3(light.SpotDir[0],light.SpotDir[1],light.SpotDir[2]));
			lgt->setSpotCutoff(light.SpotCutoff);
			lgt->setSpotExponent(light.SpotExponent);
			lgt->setConstantAttenuation(light.ConstantAttenuation);
			lgt->setLinearAttenuation(light.LinearAttenuation);
			lgt->setQuadraticAttenuation(light.QuadRaticAttenuation);
			return true;
		}
			 
		int num = mpr_p_LightsRoot->getNumChildren();
		for (int i = 0; i < num; i++)
		{
			osg::LightSource* ltSource = dynamic_cast<osg::LightSource*>(mpr_p_LightsRoot->getChild(i));
			if (ltSource)
			{
				osg::Light* lt = ltSource->getLight();
				if (lt->getLightNum()==light.Id)
				{
					osg::Light* osglgt = NULL;
					if (glbLight2osgLight(light,osglgt))
					{
						ltSource->setLight(osglgt);
						osg::ref_ptr<osg::StateSet> ss = mpr_p_LightsRoot->getOrCreateStateSet();
						if (light.onOrOFF)
							ss->setMode(GL_LIGHT0+light.Id, osg::StateAttribute::ON);
						else
							ss->setMode(GL_LIGHT0+light.Id, osg::StateAttribute::OFF);
						return true;
					}
					break;
				}
			}
		}	
	}
	return false;
}

glbBool CGlbGlobeView::SaveImage(CGlbWString strFileName)
{	
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	osg::Camera::DrawCallback* callback = mpr_osgviewer->getCamera()->getPostDrawCallback();
	while(callback)
	{
		callback = mpr_osgviewer->getCamera()->getPostDrawCallback();
		osg::OperationThread::microSleep(100);
	}
	CGlbString str = strFileName.ToString();
	mpr_osgviewer->getCamera()->setPostDrawCallback(new CGlbGlobeCaptureDrawCallback(str,mpr_hEvent_SaveImage));

	// 设置信号量为不发信号状态
	ResetEvent(mpr_hEvent_SaveImage);
	DWORD dwRes = WaitForSingleObject(mpr_hEvent_SaveImage,1000); //1000毫秒等待时间 千分之一秒=1毫秒  

	if (dwRes==WAIT_TIMEOUT || dwRes != WAIT_OBJECT_0)
	{
		int _error = 1;
		return false;
	}

	return true;
}

glbBool CGlbGlobeView::SaveImage(CGlbWString strFileName, glbInt32 imgWidth, glbInt32 imgHeight)
{
	CGlbString str = strFileName.ToString();

// 使用fbo输出大尺寸图片--思路 ： 离线渲染
GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	//int texWidth = 2048, texHeight = 1520;
	//osg::ref_ptr<osg::FrameBufferObject> fb = new osg::FrameBufferObject;
	//osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;
	//tex->setTextureSize(texWidth, texHeight);
	//tex->setInternalFormat(GL_RGBA);
	////tex->setSourceFormat(GL_RGBA);
	////tex->setSourceType(GL_FLOAT);
	//tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	//tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	//tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	//tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	//osg::ref_ptr<osg::Texture2D> depthTex = new osg::Texture2D;
	//depthTex->setTextureSize(texWidth, texHeight);
	//depthTex->setSourceFormat(GL_DEPTH_COMPONENT);
	//depthTex->setSourceType(GL_FLOAT);
	//depthTex->setInternalFormat(GL_DEPTH_COMPONENT24);
	//depthTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
	//depthTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
	//depthTex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	//depthTex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	//osg::RenderBuffer* colorRB = 0;
	//osg::RenderBuffer* depthRB = 0;

	//fb->setAttachment(osg::Camera::COLOR_BUFFER,
	//	osg::FrameBufferAttachment(tex.get()));
	//fb->setAttachment(osg::Camera::DEPTH_BUFFER,
	//	osg::FrameBufferAttachment(depthTex.get()));

	int width = imgWidth;
	int height = imgHeight;
	long l,b,w,h;	
	osg::Camera* pMainCamera = mpr_osgviewer->getCamera();
	osg::Viewport* pViewPort = pMainCamera->getViewport();
	l = pViewPort->x();
	b = pViewPort->y();
	w = pViewPort->width();
	h = pViewPort->height();

	// 输出图像的放缩比例			
	int magWidth  = int(float(width) / w + 0.5);
	int magHeight = int(float(height) / h + 0.5);
	magWidth = max(1,magWidth);
	magHeight = max(1,magHeight);

	bool bUserDefine = ((width - w*magWidth) == 0  && (height - h*magHeight)==0) ? false : true;

	int outputWidth,outputHeight;
	
	outputWidth = magWidth*w;
	outputHeight = magHeight*h;	

	double left,right,bottom,top,zNear,zFar;
	osg::Vec3d eye,center,up;

	pMainCamera->getProjectionMatrixAsFrustum(left,right,bottom,top,zNear,zFar);
	pMainCamera->getViewMatrixAsLookAt(eye,center,up);
	
	// 采用渲染到纹理(osg::Image)来替代渲染到帧缓存中，直接操作纹理中数据	
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;

	// set up projection.
	camera->setProjectionMatrixAsFrustum(left,right,bottom,top,zNear,zFar);		

	// set view
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrixAsLookAt(eye,center,up);

	// set viewport
	camera->setViewport(new osg::Viewport(l, b, w, h));

	// set the camera to render before the main camera.
	camera->setRenderOrder(osg::Camera::PRE_RENDER);

	// tell the camera to use OpenGL frame buffer object where supported.
	osg::Camera::RenderTargetImplementation renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;
	camera->setRenderTargetImplementation(renderImplementation);

	unsigned int samples = 0;
	unsigned int colorSamples = 0;
	osg::Image* image = new osg::Image;
	image->allocateImage(w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE);

	osg::ref_ptr<osg::Image> outputImage = new osg::Image;
	try
	{
		outputImage->allocateImage(outputWidth, outputHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE);	//建立一个大的数据buffer用于写入
	}
	catch (...)
	{
		//pView->Refresh();						
		MessageBox(NULL,L"系统内存不足,创建失败!",L"Error", MB_OK);			
		return false;
	}

	if (bUserDefine)
	{
		outputWidth = width;
		outputHeight = height;
	}

	// attach the image so its copied on each frame.
	camera->attach(osg::Camera::COLOR_BUFFER, image,samples, colorSamples);

	camera->addChild(mpr_globe->mpr_root);

	// 先暂停线程
	Pause(true);

	pMainCamera->addChild(camera);	 	

	// 新算法
	double half_wx = (right-left)/magWidth*0.5;
	double half_wy = (top-bottom)/magHeight*0.5;

	for( int m = 0; m < magHeight; m++)
		for ( int n = 0; n < magWidth; n++)
		{
			double centerX = left + half_wx + n*half_wx*2;
			double centerY = bottom + half_wy + m*half_wy*2;
			camera->setProjectionMatrixAsFrustum(centerX-half_wx,centerX+half_wx,centerY-half_wy,centerY+half_wy,zNear,zFar);

			mpr_osgviewer->frame();

			//OpenThreads::Thread::microSleep(1000);

			int start_col = n*image->s();
			int start_row = m*image->t();
			for (int j = 0; j < image->t(); j++)// 高度
			{
				unsigned char* sourcedata = image->data(0, j);
				unsigned char* destdata = outputImage->data(start_col, start_row++);
				memcpy(destdata,sourcedata,image->s()*4);				 
			}
		}
	
	if ( (outputImage->s() != outputWidth) || (outputImage->t() != outputHeight) )
	{
		outputImage->scaleImage(outputWidth,outputHeight,1);
	}
	osgDB::writeImageFile(*(outputImage.get()), str.c_str());

	// 删除添加的camera节点
	osg::Group* pGroup = camera->getParent(0);
	pGroup->removeChild(camera);	

	Pause(false);

	return true;
}

void CGlbGlobeView::Shake(glbBool isShake)
{
	if (mpr_bShake == isShake)
		return;
	mpr_bShake = isShake;
	if(mpr_manipulatormanager.valid())		
		mpr_manipulatormanager->Shake(isShake);

}
glbBool CGlbGlobeView::IsShake()
{
	return mpr_bShake;
}
void CGlbGlobeView::SetShakeSope(glbDouble horizontalDegreeOrMeter, glbDouble verticalDegreeOrMeter)
{
	mpr_shakeHorDegree = horizontalDegreeOrMeter;
	mpr_shakeVerDegree = verticalDegreeOrMeter;
	if(mpr_manipulatormanager.valid())		
		mpr_manipulatormanager->SetShakeSope(horizontalDegreeOrMeter,verticalDegreeOrMeter);
}
glbBool CGlbGlobeView::GetShakeSope(glbDouble &horizontalDegreeOrMeter, glbDouble &verticalDegreeOrMeter)
{
	horizontalDegreeOrMeter = mpr_shakeHorDegree;
	verticalDegreeOrMeter = mpr_shakeVerDegree;
	return true;
}
void CGlbGlobeView::SetShakeTime(glbDouble seconds)
{
	mpr_shakeTime = seconds;
	if(mpr_manipulatormanager.valid())		
		mpr_manipulatormanager->SetShakeTime(seconds);
}

glbDouble CGlbGlobeView::GetShakeTime()
{
	return mpr_shakeTime;
}

void CGlbGlobeView::EnalbeVirtualReferencePlane(glbBool bEnable)
{
	if(mpr_manipulatormanager.valid())		
		mpr_manipulatormanager->EnalbeVirtualReferencePlane(bEnable);
}

void CGlbGlobeView::SetVirtualReferencePlane(glbDouble zOrAltitude)
{
	if(mpr_manipulatormanager.valid())		
		mpr_manipulatormanager->SetVirtualReferencePlane(zOrAltitude);
}

glbDouble CGlbGlobeView::GetVirtualReferencePlane()
{
	if(mpr_manipulatormanager.valid())		
		mpr_manipulatormanager->GetVirtualReferencePlane();
	return -10.0;
}
#include <osgParticle/SmokeEffect>
#include <osgParticle/ExplosionDebrisEffect>
#include <osgParticle/ExplosionEffect>
#include <osgParticle/FireEffect>
osg::ref_ptr<osg::Node> CGlbGlobeView::CreateSmoke()
{
	osg::ref_ptr<osg::Switch> smokeNode = new osg::Switch();
	
	// 风向
	osg::Vec3 wind(1.0f,0.0f,0.0f);

	// 位置
	osg::Vec3d position;
	osg::Vec3d focusPos;
	glbDouble _x,_y;
	getScreenFocusPt(_x,_y);
	glbDouble lonOrX,latOrY,atlOrZ;
	mpr_globe->ScreenToTerrainCoordinate(_x,_y,lonOrX,latOrY,atlOrZ);

	focusPos.set(lonOrX,latOrY,atlOrZ);
	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),
						position.x(),position.y(),position.z());

	wind = position;
	wind.normalize();
	wind.set(wind.x(),wind.y(),wind.z()+0.1);
	wind.normalize();

	float scale = 2.5;
	float intensity = 20.0f;
	//// 爆炸模拟， 10.0f为缩放比，默认为1.0f，不缩放
	//osg::ref_ptr<osgParticle::ExplosionEffect> explosion = new osgParticle::ExplosionEffect(position,scale,intensity);
	//// 碎片模拟
	//osg::ref_ptr<osgParticle::ExplosionDebrisEffect> explosionDebris = new osgParticle::ExplosionDebrisEffect(position,scale,intensity);
	//// 烟模拟
	//osg::ref_ptr<osgParticle::SmokeEffect> smoke = new osgParticle::SmokeEffect(position,scale,intensity);
	//// 火焰模拟
	//osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(position,scale,intensity);

	// 脚本 : 在屏幕范围内随进放 10 处起烟点和8处起火点
	int wx = mpr_osgviewer->getCamera()->getViewport()->width() * 0.8;	
	int wy = mpr_osgviewer->getCamera()->getViewport()->height() * 0.8;
	osg::Vec3d pos;
	for (int k = 0; k < 8; k++)
	{
		scale = 1 + rand()/double(RAND_MAX) * 3;

		glbDouble sx = _x + (rand()/double(RAND_MAX) - 0.5) * wx;
		glbDouble sy = _y + (rand()/double(RAND_MAX) - 0.5) * wy;
		mpr_globe->ScreenToTerrainCoordinate(sx,sy,lonOrX,latOrY,atlOrZ);
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(latOrY),osg::DegreesToRadians(lonOrX),atlOrZ,
			pos.x(),pos.y(),pos.z());

#if 0
		// 爆炸模拟， 10.0f为缩放比，默认为1.0f，不缩放
		osg::ref_ptr<osgParticle::ExplosionEffect> explosion = new osgParticle::ExplosionEffect(pos,scale,intensity);

		// 碎片模拟
		//osg::ref_ptr<osgParticle::ExplosionDebrisEffect> explosionDebris = new osgParticle::ExplosionDebrisEffect(pos,scale,intensity);

		// 烟模拟
		osg::ref_ptr<osgParticle::SmokeEffect> smoke = new osgParticle::SmokeEffect(pos,scale,intensity);

		// 设置风向
		explosion->setWind(wind);
		//explosionDebris->setWind(wind);
		smoke->setWind(wind);

		// 添加子节点
		smokeNode->addChild(explosion.get());
		//smokeNode->addChild(explosionDebris.get());
		smokeNode->addChild(smoke.get());
#else
		// 爆炸模拟， 10.0f为缩放比，默认为1.0f，不缩放
		osg::ref_ptr<osgParticle::ExplosionEffect> explosion = new osgParticle::ExplosionEffect(pos,scale,intensity);

		// 自定义烟雾对象
		osg::ref_ptr<osg::Group> smoke = CreateMySmokeParticle(pos);

		explosion->setWind(wind);

		smokeNode->addChild(explosion.get());
		smokeNode->addChild(smoke.get());
#endif
	}

	scale = 2;
	intensity = 1;
	for (int k = 0; k < 8; k++)
	{
		scale = 1 + rand()/double(RAND_MAX) * 3;

		glbDouble sx = _x + (rand()/double(RAND_MAX) - 0.5) * wx;
		glbDouble sy = _y + (rand()/double(RAND_MAX) - 0.5) * wy;
		mpr_globe->ScreenToTerrainCoordinate(sx,sy,lonOrX,latOrY,atlOrZ);
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(latOrY),osg::DegreesToRadians(lonOrX),atlOrZ,
			pos.x(),pos.y(),pos.z());

		wind = pos;
		wind.normalize();

		// 爆炸模拟， 10.0f为缩放比，默认为1.0f，不缩放
		osg::ref_ptr<osgParticle::ExplosionEffect> explosion = new osgParticle::ExplosionEffect(pos,scale,intensity);		

		// 碎片模拟
		osg::ref_ptr<osgParticle::ExplosionDebrisEffect> explosionDebris = new osgParticle::ExplosionDebrisEffect(pos,1.0);

		// 火焰模拟
		osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(pos,scale,intensity);

		// 设置风向		
		fire->setTextureFileName("res/smoke.rgb");

		fire->setEmitterDuration(10000000);

		fire->setWind(wind);

		// 添加子节点
		smokeNode->addChild(fire.get());		
		smokeNode->addChild(explosion.get());
		smokeNode->addChild(explosionDebris.get());
	}

	return smokeNode.get();
}

#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/BoxPlacer>

// 创建自定义烟雾粒子系统
osg::ref_ptr<osg::Group> CGlbGlobeView::CreateMySmokeParticle(osg::Vec3 pos)
{
	osg::ref_ptr<osg::Group> smokeNode = new osg::Group();

	// 创建粒子系统模板
	osgParticle::Particle ptemplate;
	// 设置生命周期
	ptemplate.setLifeTime(1.5);
	// 设置粒子大小变化范围
	ptemplate.setSizeRange(osgParticle::rangef(0.75f,3.0f)); // 0.75-3.0
	// 设置粒子alpha变化范围
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
	float _rclr = 0.4f;//0.2f + rand()/double(RAND_MAX);
	// 设置粒子颜色变化范围
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.1f,0.1f,0.1f,1.0f), osg::Vec4(_rclr,_rclr,_rclr,0.0f)));
	// 设置半径
	ptemplate.setRadius(0.05f); //0.05
	// 设置重量
	ptemplate.setMass(0.05f); //0.05

	// 创建粒子系统
	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
	// 设置材质， 是否放射粒子，是否添加光照
	ps->setDefaultAttributes("res/smoke.rgb",false,false);

	// 加入模板
	ps->setDefaultParticleTemplate(ptemplate);

	// 创建粒子发射器(包括计数器，放置器和发射器）
	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
	// 关联粒子系统
	emitter->setParticleSystem(ps.get());

	// 创建发射器和计数器， 调整每一帧增加的粒子数目
	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
	// 设置每秒添加的粒子的个数
	counter->setRateRange(100.0f,100.0f);
	// 关联计数器
	emitter->setCounter(counter.get());
	// 设置一个点放置器
	osg::ref_ptr<osgParticle::PointPlacer> placer = new osgParticle::PointPlacer();
	// 设置位置
	placer->setCenter(pos);
	// 关联点放置器
	emitter->setPlacer(placer.get());

	// box放置器
	//osg::ref_ptr<osgParticle::BoxPlacer> placer = new osgParticle::BoxPlacer();
	//placer->setXRange(-100,100);
	//placer->setYRange(-100,100);
	//placer->setZRange(-100,100);
	//emitter->setPlacer(placer.get());

	// 创建弧度发射器
	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
	// 设置发射速度变化范围
	shooter->setInitialSpeedRange(0,30); // 100-0
	// 关联发射器
	emitter->setShooter(shooter.get());

	// 加入到场景中
	smokeNode->addChild(emitter.get());

	// 创建标准编程器对象，控制粒子在生命周期中的更新
	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
	// 关联粒子系统
	program->setParticleSystem(ps.get());

	// 创建重力模拟对象
	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
	// 设置重力加速的，末日为9.80665f
	ap->setToGravity(-1.0f);
	// 关联重力
	program->addOperator(ap.get());

	// 创建空气阻力模拟
	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
	// 设置空气属性
	// FluidViscosity 为1.8e-5f,FluidDensity 为1.2929f
	ffo->setFluidToAir();
	// 关联空气阻力
	program->addOperator(ffo.get());

	// 添加到场景
	smokeNode->addChild(program.get());

	// 添加更新器，实现每帧粒子的管理
	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
	// 关联粒子系统
	psu->addParticleSystem(ps.get());
	
	// 加入场景
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(ps.get());

	smokeNode->addChild(geode.get());
	smokeNode->addChild(psu.get());
	return smokeNode.get();
}

// 创建雾效 放到 地面节点  看效果
osg::ref_ptr<osg::Fog> CGlbGlobeView::CreateFog(bool m_Linear)
{
	osg::ref_ptr<osg::Fog> fog = new osg::Fog();
	// 设置颜色
	fog->setColor(osg::Vec4f(1.0f,1.0f,1.0f,1.0f));

	// 设置浓度
	fog->setDensity(0.01f);
	// 设置雾效模式为线性雾
	if (m_Linear)
	{
		fog->setMode(osg::Fog::LINEAR);
	}
	else // 全局雾
	{
		fog->setMode(osg::Fog::EXP);
	}
	// 设置雾效近点浓度
	fog->setStart(1.0f);
	// 设置雾效远点浓度
	fog->setEnd(2000.0f);

	return fog.get();
}

glbBool CGlbGlobeView::AddSlaveCamera(int x,int y,int width,int height)
{
	osg::ref_ptr<CGlbGlobeViewSlave> vs = new CGlbGlobeViewSlave(this,x,y,width,height);
	glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpr_p_root,vs);
	mpr_globe->mpr_p_callback->AddHangTask(task.get());
	return true;
}

glbBool CGlbGlobeView::RemoveSlaveCamera()
{
	if(mpr_globe->mpr_ugplane.valid())
		mpr_globe->mpr_ugplane->SetShowFilter(false);
	if(mpr_p_root->getNumChildren() < 2)
		return false;
	osg::Node *node = mpr_p_root->getChild(1);
	if(!node || node->getName() != "slaveCamera")
		return false;
	glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpr_p_root,node);
	mpr_globe->mpr_p_callback->AddRemoveTask(task.get());
	return true;
}

void CGlbGlobeView::OpenGlobeUGPlane( glbBool isOpen )
{
	mpr_globe->mpr_ugplane->SetShowFilter(isOpen);
}

#include <osg/Math>
void CGlbGlobeView::ComputeCameraPitchAndYaw(glbDouble cameraLonOrX, glbDouble cameraLatOrY, glbDouble cameraAltOrZ,
												glbDouble focusLonOrX, glbDouble focusLatOrY, glbDouble focusAltOrZ ,
												glbDouble &pitch, glbDouble &yaw)
{
	if (mpr_globe == NULL) return;
	osg::Vec3d cameraPos(cameraLonOrX,cameraLatOrY,cameraAltOrZ);
	osg::Vec3d focusPos(focusLonOrX,focusLatOrY,focusAltOrZ);
	
	if (GLB_GLOBETYPE_GLOBE == mpr_globe->GetType())
	{
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraLatOrY),osg::DegreesToRadians(cameraLonOrX),cameraAltOrZ,
															cameraPos.x(),cameraPos.y(),cameraPos.z());
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(focusLatOrY),osg::DegreesToRadians(focusLonOrX),focusAltOrZ,
															focusPos.x(),focusPos.y(),focusPos.z());
	}

	osg::Vec3d lookvec = focusPos - cameraPos;
	double distance = lookvec.length();
	osg::Vec3d up(0,0,1);
	if (GLB_GLOBETYPE_GLOBE == mpr_globe->GetType())
		up = g_ellipsmodel->computeLocalUpVector(cameraPos.x(),cameraPos.y(),cameraPos.z());
	//osg::Vec3d s(lookvec ^ up);
	//up = lookvec ^ s;
	//up.normalize();

	osg::Matrixd mat_camera;
	mat_camera.makeLookAt(cameraPos,focusPos,up);//相机观察矩阵	: 世界坐标系下顶点向相机坐标系转换的变换矩阵
	mat_camera = osg::Matrixd::inverse(mat_camera);//相机位置姿态矩阵 ： 相机坐标系下顶点向世界坐标系转换的变换矩阵
	osg::Matrixd mat = mat_camera;

	if (GLB_GLOBETYPE_GLOBE == mpr_globe->GetType())
	{
		// 计算pitch
		osg::Vec3d cameraPos_w = cameraPos;
		osg::Matrixd mat_cameraToworld;
		g_ellipsmodel->computeLocalToWorldTransformFromXYZ(cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z(),mat_cameraToworld);
		mat = mat_camera * osg::Matrixd::inverse(mat_cameraToworld);	

		if (mat(2,2) > 1.0 || mat(2,2) < -1.0)	//防止mat(2,2)溢出(-1.0,1.0)范围,asin无法计算
		{
			glbDouble dm = mat(2,0)*mat(2,0)+mat(2,1)*mat(2,1)+mat(2,2)*mat(2,2);
			dm = sqrt(dm);
			pitch = osg::RadiansToDegrees(asin(-mat(2,2)/dm));
		}else{	  
			pitch = osg::RadiansToDegrees(asin(-mat(2,2)));
		}

		// 计算yaw
		yaw = osg::RadiansToDegrees(atan2(-mat(0,1),mat(0,0)));
	}
	else if (GLB_GLOBETYPE_FLAT == mpr_globe->GetType())
	{
		 //mat = mat_camera;
		 if (lookvec.z()/distance > 1.0)
			 pitch = 90.0;
		 else if (lookvec.z()/distance < -1.0)
			 pitch = -90.0;
		 else
			 pitch = osg::RadiansToDegrees(asin(lookvec.z()/distance));
		 //如果pitch角为正负90时，mpr_yaw则不变  
		 yaw = osg::RadiansToDegrees(atan2(lookvec.x(),lookvec.y()));
	}	
}

void CGlbGlobeView::SetDriveMoveSpeed(glbDouble speed)
{
	mpr_manipulatormanager->SetDriveMoveSpeed(speed);
}

glbBool CGlbGlobeView::AddCollisionObject(CGlbGlobeRObject *obj)
{
	return mpr_manipulatormanager->AddCollisionObject(obj);
}

glbBool CGlbGlobeView::AddCollisionLaysObject(glbInt32 layId,CGlbGlobeRObject* obj)
{
	return mpr_manipulatormanager->AddCollisionLaysObject(layId,obj);
}

#include "GlbGlobeFeatureLayer.h"
glbBool CGlbGlobeView::AddCollisionLayer(CGlbGlobeLayer *layer)
{
	CGlbGlobeFeatureLayer* featurelayer = dynamic_cast<CGlbGlobeFeatureLayer*>(layer); 
	if (featurelayer)
		featurelayer->SetCollisionDetect(true);
	return true;
}

glbBool CGlbGlobeView::RemoveCollisionObjectOrLayerById(int id,glbBool isLyr)
{
	if (isLyr)
	{// 如果是图层，需要通知图层关闭碰撞检测
		CGlbGlobeFeatureLayer* featurelayer = dynamic_cast<CGlbGlobeFeatureLayer*>(GetGlobe()->GetLayerById(id));
		if (featurelayer)
			featurelayer->SetCollisionDetect(false);
	}
	return mpr_manipulatormanager->RemoveCollisionObjectOrLayerById(id,isLyr);
}

CGlbGlobeRObject* CGlbGlobeView::getCollisionObjectByObjId(int idx)
{
	return mpr_manipulatormanager->getCollisionObjectByObjId(idx);
}

void CGlbGlobeView::GetHaveAddObjsAndLays(std::vector<glbInt32>& ids,std::vector<glbInt32>&lays)
{
	return mpr_manipulatormanager->GetHaveAddObjsAndLays(ids,lays);
}

glbInt32 CGlbGlobeView::GetCollisionObjectCount()
{
	return mpr_manipulatormanager->GetCollisionObjectCount();
}

glbBool CGlbGlobeView::RemoveAllCollsionObject()
{
	return mpr_manipulatormanager->RemoveAllCollsionObject();
}

glbDouble CGlbGlobeView::GetDriveMoveSpeed()
{
	return mpr_manipulatormanager->GetDriveMoveSpeed();
}



//#include "GlbGlobeCoordinateAxisObject.h"
//#include "GlbGlobeREObject.h"
//void CGlbGlobeView::TestCreateCoordinateAxisObject()
//{// 测试 坐标轴对象
//	osg::Vec3 origin, axisLen, axisStep;
//	CGlbGlobeREObject* obj = dynamic_cast<CGlbGlobeREObject*>(g_objectFactory.GetObject(L"GlbGlobeCoordinateAxis"));	
//
//	//CGlbGlobeCoordinateAxisObject* obj = new CGlbGlobeCoordinateAxisObject();
//	glbref_ptr<GlbCoordinateAxisSymbolInfo> axisInfo = new GlbCoordinateAxisSymbolInfo();
//	double cx = GetOsgCamera()->getViewport()->width() * 0.5;
//	double cy = GetOsgCamera()->getViewport()->height() * 0.5;
//
//	double xx,yy,zz;
//	mpr_globe->ScreenToTerrainCoordinate(cx,cy,xx,yy,zz);
//	axisInfo->originX = xx;
//	axisInfo->originY = yy;
//	axisInfo->originZ = zz;
//
//	axisInfo->axisLengthX = 1000;
//	axisInfo->axisLengthY = 1000;
//	axisInfo->axisLengthZ = 300;
//
//	axisInfo->axisStepX = 200;
//	axisInfo->axisStepY = 200;
//	axisInfo->axisStepZ = 100;
//
//	axisInfo->bInvertZ = true;
//
//	glbBool isNeedDirectDraw;
//	obj->SetRenderInfo(axisInfo.get(),isNeedDirectDraw);
//
//
//	obj->SetMinVisibleDistance(0);
//	obj->SetMaxVisibleDistance(100000000000000);
//
//	// 添加坐标轴对象
//	mpr_globe->AddObject(obj);
//}

glbBool GlbGlobe::CGlbGlobeView::CheckCollision() const
{
	return mpr_globe->CheckCullisionModel();
}

osg::Node * GlbGlobe::CGlbGlobeView::GetSkydom()
{
	if(mpr_sky == NULL)
		return NULL;
	return mpr_sky->GetNode(L"glbskybox");
}

osg::Node * GlbGlobe::CGlbGlobeView::GetStars()
{
	if(mpr_sky == NULL)
		return NULL;
	return mpr_sky->GetNode(L"glbstars");
}

osg::Node * GlbGlobe::CGlbGlobeView::GetGlobeElementNode()
{
	if(mpr_sky == NULL)
		return NULL;
	return mpr_sky->GetOsgNode();
}
