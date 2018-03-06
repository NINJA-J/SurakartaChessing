#if !defined(AFX_NEWGAME_H__AA62815B_FACF_4F51_BB58_6FA207C2D137__INCLUDED_)
#define AFX_NEWGAME_H__AA62815B_FACF_4F51_BB58_6FA207C2D137__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewGame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewGame dialog

class CNewGame : public CDialog
{
// Construction
public:
	CNewGame(CWnd* pParent = NULL);   // standard constructor

	int GetSelectedEngine() {return m_nSelectedEngine;}
	int GetSelectedPly() {return m_nSelectedPly;}
protected:
	int m_nSelectedEngine;//搜索引擎选择
	int m_nSelectedPly;//深度选择
// Dialog Data
	//{{AFX_DATA(CNewGame)
	enum { IDD = IDD_DIALOG2 };
	CListBox	m_SearchEngineList;
	CSpinButtonCtrl	m_SetPly;
	int		m_nEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewGame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewGame)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWGAME_H__AA62815B_FACF_4F51_BB58_6FA207C2D137__INCLUDED_)
