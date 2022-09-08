// MountView.h : interface of the CMountView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUNTVIEW_H__F7442C64_5485_11D3_96D8_00C04FCCB957__INCLUDED_)
#define AFX_MOUNTVIEW_H__F7442C64_5485_11D3_96D8_00C04FCCB957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "trace.h"
#include "Strcv.h"

class CMountView : public CScrollView
{
protected: // create from serialization only
	CMountView();
	DECLARE_DYNCREATE(CMountView)

// Attributes
public:
	CMountDoc* GetDocument();
	CTrace trace;
	CBrush brush[256];
	CMyPoint m_pointMidRight;
	CShotRecieve sr;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMountView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	double Calculate(CMyPoint pointSrc,CMyPoint pointTar);
	virtual ~CMountView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMountView)
	afx_msg void OnTestSurvey();
	afx_msg void OnTestFstbrk();
	afx_msg void OnTESTSvSys();
	afx_msg void OnTestSr();
	afx_msg void OnTESTShotPar();
	afx_msg void OnTestResolve();
	afx_msg void OnTestEquation();
	afx_msg void OnTestinitial();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTest4Project();
	afx_msg void OnTest4Initial();
	afx_msg void OnTest4FstBrk();
	afx_msg void OnTest4Resolve();
	afx_msg void OnTest4ShotPar();
	afx_msg void OnTest4ShotRecieve();
	afx_msg void OnTest4SvSys();
	afx_msg void OnTestCac();
	afx_msg void OnCalcStatic();
	afx_msg void On116Pro();
	afx_msg void On116Sys();
	afx_msg void On116Sr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MountView.cpp
inline CMountDoc* CMountView::GetDocument()
   { return (CMountDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOUNTVIEW_H__F7442C64_5485_11D3_96D8_00C04FCCB957__INCLUDED_)
