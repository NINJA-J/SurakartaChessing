#if !defined(AFX_STARTDLG_H__024A2914_C7F8_42D5_BBAD_306BF63D8B59__INCLUDED_)
#define AFX_STARTDLG_H__024A2914_C7F8_42D5_BBAD_306BF63D8B59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StartDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// StartDlg dialog

class StartDlg : public CDialog
{
// Construction
public:
	StartDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(StartDlg)
	enum { IDD = IDD_DIALOG1 };
	int		m_computer;
	int		m_red;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StartDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(StartDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STARTDLG_H__024A2914_C7F8_42D5_BBAD_306BF63D8B59__INCLUDED_)
