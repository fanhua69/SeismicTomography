// NewMount.h : main header file for the NEWMOUNT application
//

#if !defined(AFX_NEWMOUNT_H__F7442C5A_5485_11D3_96D8_00C04FCCB957__INCLUDED_)
#define AFX_NEWMOUNT_H__F7442C5A_5485_11D3_96D8_00C04FCCB957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMountApp:
// See NewMount.cpp for the implementation of this class
//

#ifndef MOUNT_ERROR
#define MOUNT_ERROR 99999
#endif

class CMountApp : public CWinApp
{
public:
	CMountApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMountApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMountApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWMOUNT_H__F7442C5A_5485_11D3_96D8_00C04FCCB957__INCLUDED_)
