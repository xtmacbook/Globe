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
//����ת��:��һ�ֱ���תΪ��һ�ֱ���  
int code_convert_obj(char* from_charset, char* to_charset, char* inbuf, int inlen, char* outbuf, int outlen);
//UNICODE��תΪGB2312��  
//�ɹ��򷵻�һ����̬�����char*��������Ҫ��ʹ����Ϻ��ֶ�free��ʧ�ܷ���NULL
char* u2g_obj(char *inbuf);
//��־
//#include "GlbLog.h"

std::string GetSystemTime();    

//��ȡʣ���ڴ�
extern glbInt64 GetUnusedMemory();
/*
	��������꣬��Ϊ��ʹ��ffmpeg������!!!!!.
*/
#define __STDC_CONSTANT_MACROS