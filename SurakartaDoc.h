// SurakartaDoc.h : interface of the CSurakartaDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURAKARTADOC_H__D1DEA378_FDAF_4843_948A_5DAD45878928__INCLUDED_)
#define AFX_SURAKARTADOC_H__D1DEA378_FDAF_4843_948A_5DAD45878928__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSurakartaDoc : public CDocument
{
protected: // create from serialization only
	CSurakartaDoc();
	DECLARE_DYNCREATE(CSurakartaDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSurakartaDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSurakartaDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSurakartaDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SURAKARTADOC_H__D1DEA378_FDAF_4843_948A_5DAD45878928__INCLUDED_)
