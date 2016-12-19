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

//ȫ�ֱ���
#include <osg/CoordinateSystemNode>
extern osg::ref_ptr<osg::EllipsoidModel> g_ellipsmodel;//ȫ��������

//���󹤳�
#include "GlbGlobeObjectFactory.h"
//extern CGlbGlobeObjectFactory* g_objectFactory;
extern CGlbGlobeObjectFactory g_objectFactory;//modified by longtingyou

// agg��overlayimg��ϵ�src_img��[RGBAģʽ]
glbBool BlendImage(glbByte* src_img, glbByte* overlay_img, glbInt32 wid, glbInt32 hei,glbInt32 opacity=100, glbBool rgba_type=true);

//����ת��:��һ�ֱ���תΪ��һ�ֱ���  
int code_convert(char* from_charset, char* to_charset, char* inbuf, int inlen, char* outbuf, int outlen);
//UNICODE��תΪGB2312��  
//�ɹ��򷵻�һ����̬�����char*��������Ҫ��ʹ����Ϻ��ֶ�free��ʧ�ܷ���NULL
char* u2g(char *inbuf);
//GB2312��תΪUNICODE��  
//�ɹ��򷵻�һ����̬�����char*��������Ҫ��ʹ����Ϻ��ֶ�free��ʧ�ܷ���NULL
char* g2u(char *inbuf);
//��ȡϵͳʣ���ڴ�
glbInt64 GetUnusedMemory();
//�����־
#include "GlbLog.h"
/****************ȫ��������Ϣ*********************/
extern glbBool   g_isMemCtrl;
extern glbInt64  g_memThreshold;//M
extern glbBool   g_isDMemCtrl;
extern glbInt64  g_totalDMem;//M
/****************ȫ��������Ϣ*********************/
//#ifndef _X64
#include "GlbLic.h"
#include "GlbWString.h"
#include "GlbString.h"
extern GlbLic      g_srvLic;
extern CGlbWString g_srvIP;
extern glbInt32    g_srvPort;
GlbLicResult*  srvlicVery();
//#endif

#if 1 // ����ģʽʱ�����������Ϊ1
#define GLOBE_DEVELOPER_MODE 1  // ���忪����ģʽ
#define MAX_TASKTHREAD_NUM  1  ///<��������������߳��������
#else// ����ģʽ�����������Ϊ3
#define MAX_TASKTHREAD_NUM  3 ///<��������������߳��������
#endif

//�ҵ������־
#include "GlbGlobeLog.h"
#ifdef LOG_ENABLE  

//��������Щ����ʹ�ñ��ļ�  
#define LOG(x)          CGlbGlobeLog::WriteLog(x);          //�����ڿ������ַ���(ascii)����������������bool��  
#define LOG2(x1,x2)     CGlbGlobeLog::WriteLog2(x1,x2);  
#define LOG_FUNC        LOG(__FUNCTION__)               //�����ǰ���ں�����  
#define LOG_LINE        LOG(__LINE__)                       //�����ǰ�к�  
#define LOG_FUNC_BEGIN  CGlbGlobeLog::WriteFuncBegin(__FUNCTION__);     //��ʽ�磺[ʱ��]"------------FuncName  Begin------------"  
#define LOG_FUNC_END    CGlbGlobeLog::WriteFuncEnd(__FUNCTION__);      //��ʽ�磺[ʱ��]"------------FuncName  End------------" 
#else  

#define LOG(x)                
#define LOG2(x1,x2)       
#define LOG_FUNC          
#define LOG_LINE          
#define LOG_FUNC_BEGIN    
#define LOG_FUNC_END      

#endif  

