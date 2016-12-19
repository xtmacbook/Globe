
// GlobeView.h : interface of the CGlobeView class
//


#pragma once

#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
using namespace GlbGlobe;
// CGlobeView window

class CGlobeView : public CWnd
{
// Construction
public:
	CGlobeView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CGlobeView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
	glbref_ptr<CGlbGlobe>     mpr_globe;
	glbref_ptr<CGlbGlobeView> mpr_globeview;
	HWND                      mpr_globeviewHwnd;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCgglobeLoadmodel();
	afx_msg void OnCgglobeSavescene();
	afx_msg void OnCgglobeOpenscene();
	afx_msg void OnCgglobeCreated();
	afx_msg void OnFileds();
};

