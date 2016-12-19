// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// CreatarMfcView.h : CCreatarMfcView ��Ľӿ�
//

#pragma once

#include "GlbGlobe.h"
#include "GlbGlobeView.h"
using namespace GlbGlobe;
#include "CreatarMfcDoc.h"

class CCreatarMfcView : public CView
{
protected: // �������л�����
	CCreatarMfcView();
	DECLARE_DYNCREATE(CCreatarMfcView)

// ����
public:
	CCreatarMfcDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CCreatarMfcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // CreatarMfcView.cpp �еĵ��԰汾
inline CCreatarMfcDoc* CCreatarMfcView::GetDocument() const
   { return reinterpret_cast<CCreatarMfcDoc*>(m_pDocument); }
#endif

