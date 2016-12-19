// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"
#include "afxwin.h"
#include "afxcmn.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

#include <afxwin.h>
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#include <comsvcs.h>

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <afxdlgs.h>

using namespace ATL;
#import "cwCore.tlb" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#import "cwSystemUI.tlb"   raw_interfaces_only, raw_native_types, no_namespace, named_guids//, auto_search
#import "cwGeometrys.tlb"  raw_interfaces_only, raw_native_types, no_namespace, named_guids//, auto_search
#import "cwObjects.tlb"    raw_interfaces_only, raw_native_types, no_namespace, named_guids//, auto_search
#import "cwGrids.tlb"      raw_interfaces_only, raw_native_types, no_namespace, named_guids//, auto_search
#import "cwWkSpace.tlb"    raw_interfaces_only, raw_native_types, no_namespace, named_guids//, auto_search
#import "cwScene3D.tlb"	   raw_interfaces_only, raw_native_types, no_namespace, named_guids//, auto_search
#import "SceneTools.tlb"   raw_interfaces_only, raw_native_types, no_namespace, named_guids//, auto_search
#import "D:\XModeling\trunck\bin\release\cwSystemUI.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search
//#import "D:\XModeling\trunck\bin\release\cwSystemUI.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search
//#import "D:\XModeling\trunck\bin\release\cwSystemUI.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search
