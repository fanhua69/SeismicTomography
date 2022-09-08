// MountDoc.h : interface of the CMountDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUNTDOC_H__F7442C62_5485_11D3_96D8_00C04FCCB957__INCLUDED_)
#define AFX_MOUNTDOC_H__F7442C62_5485_11D3_96D8_00C04FCCB957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMountDoc : public CDocument
{
protected: // create from serialization only
	CMountDoc();
	DECLARE_DYNCREATE(CMountDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMountDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMountDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMountDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOUNTDOC_H__F7442C62_5485_11D3_96D8_00C04FCCB957__INCLUDED_)
