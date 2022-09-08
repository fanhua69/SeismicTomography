// MountDoc.cpp : implementation of the CMountDoc class
//

#include "stdafx.h"
#include "NewMount.h"

#include "MountDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMountDoc

IMPLEMENT_DYNCREATE(CMountDoc, CDocument)

BEGIN_MESSAGE_MAP(CMountDoc, CDocument)
	//{{AFX_MSG_MAP(CMountDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMountDoc construction/destruction

CMountDoc::CMountDoc()
{
	// TODO: add one-time construction code here

}

CMountDoc::~CMountDoc()
{
}

BOOL CMountDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMountDoc serialization

void CMountDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMountDoc diagnostics

#ifdef _DEBUG
void CMountDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMountDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMountDoc commands
