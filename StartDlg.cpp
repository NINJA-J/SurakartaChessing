// StartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Surakarta.h"
#include "StartDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// StartDlg dialog


StartDlg::StartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(StartDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(StartDlg)
	m_computer = -1;
	m_red = -1;
	//}}AFX_DATA_INIT
}


void StartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StartDlg)
	DDX_Radio(pDX, IDC_COMPUTER, m_computer);
	DDX_Radio(pDX, IDC_RED, m_red);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StartDlg, CDialog)
	//{{AFX_MSG_MAP(StartDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StartDlg message handlers

void StartDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
