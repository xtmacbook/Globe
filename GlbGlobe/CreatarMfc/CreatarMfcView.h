// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// CreatarMfcView.h : CCreatarMfcView 类的接口
//

#pragma once

#include "GlbGlobe.h"
#include "GlbGlobeView.h"
using namespace GlbGlobe;
#include "CreatarMfcDoc.h"

class CCreatarMfcView : public CView
{
protected: // 仅从序列化创建
	CCreatarMfcView();
	DECLARE_DYNCREATE(CCreatarMfcView)

// 特性
public:
	CCreatarMfcDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CCreatarMfcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void LoadModel();
private:
	glbref_ptr<CGlbGlobe>     mpr_globe;
	glbref_ptr<CGlbGlobeView> mpr_globeview;
	HWND                      mpr_globeviewHwnd;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCreateDynamic();
	afx_msg void OnFeaturelayer();
	afx_msg void OnButton8();
	afx_msg void OnCreateDomLayer();
	afx_msg void OnCreateDemLayer();
	afx_msg void OnTerrBuilder();
};

#ifndef _DEBUG  // CreatarMfcView.cpp 中的调试版本
inline CCreatarMfcDoc* CCreatarMfcView::GetDocument() const
   { return reinterpret_cast<CCreatarMfcDoc*>(m_pDocument); }
#endif

