// SurakartaDoc.cpp : implementation of the CSurakartaDoc class
//

#include "stdafx.h"
#include "Surakarta.h"

#include "SurakartaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSurakartaDoc

IMPLEMENT_DYNCREATE(CSurakartaDoc, CDocument)

BEGIN_MESSAGE_MAP(CSurakartaDoc, CDocument)
	//{{AFX_MSG_MAP(CSurakartaDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSurakartaDoc construction/destruction

CSurakartaDoc::CSurakartaDoc()
{
	// TODO: add one-time construction code here

}

CSurakartaDoc::~CSurakartaDoc()
{
}

BOOL CSurakartaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSurakartaDoc serialization

void CSurakartaDoc::Serialize(CArchive& ar)
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
// CSurakartaDoc diagnostics

#ifdef _DEBUG
void CSurakartaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSurakartaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSurakartaDoc commands
