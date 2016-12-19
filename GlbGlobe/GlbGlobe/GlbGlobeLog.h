#pragma once

//log�ļ�·��  
#define LOG_FILE_NAME "log.txt"  

//���ÿ���  
#define LOG_ENABLE  

#include <fstream>    
#include <string>    
#include <ctime>    
#include "GlbTime.h"

#include <comutil.h>
#pragma comment(lib, "comsuppw.lib") 

using namespace std;  

class GLB_DLLCLASS_EXPORT CGlbGlobeLog
{
public:
	CGlbGlobeLog(void);
	~CGlbGlobeLog(void);

public:    
	static void GetLogFilePath(CHAR* szPath)  
	{  
		GetModuleFileNameA( NULL, szPath, MAX_PATH ) ;  
		ZeroMemory(strrchr(szPath,'\\'), strlen(strrchr(szPath,'\\' ) )*sizeof(CHAR)) ;  
		strcat(szPath,"\\");  
		strcat(szPath,LOG_FILE_NAME);  
	}  
	//���һ�����ݣ��������ַ���(ascii)����������������������ö��  
	//��ʽΪ��[2011-11-11 11:11:11] aaaaaaa������  
	template <class T>  
	static void WriteLog(T x)  
	{  
		CHAR szPath[MAX_PATH] = {0};  
		GetLogFilePath(szPath);  

		ofstream fout(szPath,ios::app);  
		fout.seekp(ios::end);  
		fout << GetSystemTime() << x <<endl;  
		fout.close();  
	}  

	//���2�����ݣ��ԵȺ����ӡ�һ������ǰ����һ�������������ַ�������������������ֵ  
	template<class T1,class T2>   
	static void WriteLog2(T1 x1,T2 x2)  
	{  
		CHAR szPath[MAX_PATH] = {0};  
		GetLogFilePath(szPath);  
		ofstream fout(szPath,ios::app);  
		fout.seekp(ios::end);  
		fout << GetSystemTime() << x1 <<" = "<<x2<<endl;  
		fout.close();  
	}  

	//���һ�е�ǰ������ʼ�ı�־,�괫��__FUNCTION__  
	template <class T>  
	static void WriteFuncBegin(T x)  
	{  
		CHAR szPath[MAX_PATH] = {0};  
		GetLogFilePath(szPath);  
		ofstream fout(szPath,ios::app);  
		fout.seekp(ios::end);  
		fout << GetSystemTime() << "    --------------------"<<x<<"  Begin--------------------" <<endl;  
		fout.close();  
	}  

	//���һ�е�ǰ���������ı�־���괫��__FUNCTION__  
	template <class T>  
	static void WriteFuncEnd(T x)  
	{  
		CHAR szPath[MAX_PATH] = {0};  
		GetLogFilePath(szPath);  
		ofstream fout(szPath,ios::app);  
		fout.seekp(ios::end);  
		fout << GetSystemTime() << "--------------------"<<x<<"  End  --------------------" <<endl;  
		fout.close();  
	}  
	
	// * @param format  ��ʽ�ַ���
	static bool OutputLogString(WCHAR* format,...)
	{
		std::string _strTime = GetSystemTime();
		//����ATL��װ_bstr_t�Ĺ��� ʵ��char* -> wchar*
		_bstr_t t = _strTime.c_str();
		wchar_t* pwchar = (wchar_t*)t;
		std::wstring _wstrTime = pwchar;

		glbInt32 nlen = GLBSTRBUFFERSIZE*sizeof(WCHAR);
		WCHAR* info = (WCHAR*)malloc(nlen);
		if (NULL == info)
		{
			return false;
		}
		va_list args;
		va_start(args, format);
		glbInt32 nretval = vswprintf(info, nlen, format, args);
		va_end(args);
		//��������ַ������ڴ���������ظ�ֵ
		if (nretval >= 0)
		{
			_wstrTime += info;
		}
		free(info);

		OutputDebugString(_wstrTime.c_str());

		return true;
	}

	//// * @param format  ��ʽ�ַ���
	//static bool OutputLogString(glbWChar* format,...)
	//{
	//	CGlbString _strTime = GetSystemTime();
	//	CGlbWString wstr = _strTime.ToWString();	

	//	glbInt32 nlen = GLBSTRBUFFERSIZE*sizeof(glbWChar);
	//	glbWChar* info = (glbWChar*)malloc(nlen);
	//	if (NULL == info)
	//	{
	//		return false;
	//	}
	//	va_list args;
	//	va_start(args, format);
	//	glbInt32 nretval = vswprintf(info, nlen, format, args);
	//	va_end(args);
	//	//��������ַ������ڴ���������ظ�ֵ
	//	if (nretval >= 0)
	//	{
	//		wstr += info;
	//	}
	//	free(info);

	//	OutputDebugString(wstr.c_str());

	//	return true;
	//}

private:  
	//��ȡ����ʱ�䣬��ʽ��"[2011-11-11 11:11:11] ";   
	static string GetSystemTime()    
	{    
		time_t tNowTime;    
		time(&tNowTime);    
		tm* tLocalTime = localtime(&tNowTime);    
		char szTime[30] = {'\0'};    
		strftime(szTime, 30, "[%Y-%m-%d %H:%M:%S] ", tLocalTime);    
		string strTime = szTime;    
		return strTime;    
	}    
};

