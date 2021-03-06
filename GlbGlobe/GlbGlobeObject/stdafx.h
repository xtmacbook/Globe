// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "osg/Vec4"
#include "GlbCommTypes.h"
#define GetAValue(rgba)      (LOBYTE((rgba)>>24))
extern osg::Vec4 GetColor(glbInt32 color);

#define GetRValueInRGBA(rgba)      (LOBYTE(rgba))
#define GetGValueInRGBA(rgba)      (LOBYTE(((WORD)(rgba)) >> 8))
#define GetBValueInRGBA(rgba)      (LOBYTE((rgba)>>16))
#define GetRValueInBGRA(bgra)      (LOBYTE((bgra)>>16))
#define GetGValueInBGRA(bgra)      (LOBYTE(((bgra & 0xFFFF)) >> 8))//(LOBYTE(((WORD)(bgra)) >> 8))
#define GetBValueInBGRA(bgra)      (LOBYTE(bgra))

#include "osg/CoordinateSystemNode"
extern osg::ref_ptr<osg::EllipsoidModel> g_ellipsoidModel;

// TODO: reference additional headers your program requires here

#include "GlbObjectRegisterCreator.h"
#include "GlbGlobeREObject.h"
//extern CGlbObjectRegisterCreator* g_creator;
extern CGlbObjectRegisterCreator g_creator;//modified by longtingyou
//代码转换:从一种编码转为另一种编码  
int code_convert_obj(char* from_charset, char* to_charset, char* inbuf, int inlen, char* outbuf, int outlen);
//UNICODE码转为GB2312码  
//成功则返回一个动态分配的char*变量，需要在使用完毕后手动free，失败返回NULL
char* u2g_obj(char *inbuf);
//日志
//#include "GlbLog.h"

std::string GetSystemTime();    

//获取剩余内存
extern glbInt64 GetUnusedMemory();
/*
	下面这个宏，是为了使用ffmpeg而定义!!!!!.
*/
#define __STDC_CONSTANT_MACROS