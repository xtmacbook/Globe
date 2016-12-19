// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "libxml/tree.h"

// TODO: reference additional headers your program requires here

//全局变量
#include <osg/CoordinateSystemNode>
extern osg::ref_ptr<osg::EllipsoidModel> g_ellipsmodel;//全局托球体

//对象工厂
#include "GlbGlobeObjectFactory.h"
//extern CGlbGlobeObjectFactory* g_objectFactory;
extern CGlbGlobeObjectFactory g_objectFactory;//modified by longtingyou

// agg将overlayimg混合到src_img上[RGBA模式]
glbBool BlendImage(glbByte* src_img, glbByte* overlay_img, glbInt32 wid, glbInt32 hei,glbInt32 opacity=100, glbBool rgba_type=true);

//代码转换:从一种编码转为另一种编码  
int code_convert(char* from_charset, char* to_charset, char* inbuf, int inlen, char* outbuf, int outlen);
//UNICODE码转为GB2312码  
//成功则返回一个动态分配的char*变量，需要在使用完毕后手动free，失败返回NULL
char* u2g(char *inbuf);
//GB2312码转为UNICODE码  
//成功则返回一个动态分配的char*变量，需要在使用完毕后手动free，失败返回NULL
char* g2u(char *inbuf);
//获取系统剩余内存
glbInt64 GetUnusedMemory();
//输出日志
#include "GlbLog.h"
/****************全局配置信息*********************/
extern glbBool   g_isMemCtrl;
extern glbInt64  g_memThreshold;//M
extern glbBool   g_isDMemCtrl;
extern glbInt64  g_totalDMem;//M
/****************全局配置信息*********************/
//#ifndef _X64
#include "GlbLic.h"
#include "GlbWString.h"
#include "GlbString.h"
extern GlbLic      g_srvLic;
extern CGlbWString g_srvIP;
extern glbInt32    g_srvPort;
GlbLicResult*  srvlicVery();
//#endif

#if 1 // 开发模式时最大任务数量为1
#define GLOBE_DEVELOPER_MODE 1  // 定义开发者模式
#define MAX_TASKTHREAD_NUM  1  ///<定义对象任务处理线程最大数量
#else// 发布模式最大任务数量为3
#define MAX_TASKTHREAD_NUM  3 ///<定义对象任务处理线程最大数量
#endif

//我的输出日志
#include "GlbGlobeLog.h"
#ifdef LOG_ENABLE  

//用下面这些宏来使用本文件  
#define LOG(x)          CGlbGlobeLog::WriteLog(x);          //括号内可以是字符串(ascii)、整数、浮点数、bool等  
#define LOG2(x1,x2)     CGlbGlobeLog::WriteLog2(x1,x2);  
#define LOG_FUNC        LOG(__FUNCTION__)               //输出当前所在函数名  
#define LOG_LINE        LOG(__LINE__)                       //输出当前行号  
#define LOG_FUNC_BEGIN  CGlbGlobeLog::WriteFuncBegin(__FUNCTION__);     //形式如：[时间]"------------FuncName  Begin------------"  
#define LOG_FUNC_END    CGlbGlobeLog::WriteFuncEnd(__FUNCTION__);      //形式如：[时间]"------------FuncName  End------------" 
#else  

#define LOG(x)                
#define LOG2(x1,x2)       
#define LOG_FUNC          
#define LOG_LINE          
#define LOG_FUNC_BEGIN    
#define LOG_FUNC_END      

#endif  

