// NewGame.cpp : implementation file
//

#include "stdafx.h"
#include "Surakarta.h"
#include "NewGame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewGame dialog


CNewGame::CNewGame(CWnd* pParent /*=NULL*/)
	: CDialog(CNewGame::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewGame)
	m_nEdit = 0;
	//}}AFX_DATA_INIT
}


void CNewGame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewGame)
	DDX_Control(pDX, IDC_LISTENGINE, m_SearchEngineList);
	DDX_Control(pDX, IDC_PLY, m_SetPly);
	DDX_Text(pDX, IDC_EDIT1, m_nEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewGame, CDialog)
	//{{AFX_MSG_MAP(CNewGame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewGame message handlers

BOOL CNewGame::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_SearchEngineList.AddString("NegaScout search Engine");
	m_SearchEngineList.AddString("Alpha Beta search Engine");
	m_SearchEngineList.SetCurSel(0);
	m_SetPly.SetRange(1,7);
	m_SetPly.SetPos(6);
	m_nEdit = m_SetPly.GetPos();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewGame::OnOK() 
{
	// TODO: Add extra validation here
	
	m_nSelectedEngine=m_SearchEngineList.GetCurSel();
	m_nSelectedPly=m_SetPly.GetPos();
	CDialog::OnOK();
}
