// SurakartaView.cpp : implementation of the CSurakartaView class zsj
//
//

#include "stdafx.h"
#include "Surakarta.h"
#include "SurakartaDoc.h"
#include "SurakartaView.h"
#include "NewGame.h"
#include "Define.h"
#include "Eveluation.h"
#include "MoveGenerator.h"
#include "SearchEngine.h"
#include "NegaScout.h"
#include "StartDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSurakartaView

IMPLEMENT_DYNCREATE(CSurakartaView, CView)

BEGIN_MESSAGE_MAP(CSurakartaView, CView)
	//{{AFX_MSG_MAP(CSurakartaView)
	ON_COMMAND(ID_BEGIN, OnBegin)
	ON_COMMAND(ID_BACK, OnBack)
	ON_COMMAND(ID_REBEGIN, OnRebegin)
	ON_COMMAND(ID_SET, OnSet)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSurakartaView construction/destruction

////////black=1;red=2;无棋子则为0
BYTE color[6][6]={{1,1,1,1,1,1},{1,1,1,1,1,1},{0,0,0,0,0,0},{0,0,0,0,0,0},{2,2,2,2,2,2},{2,2,2,2,2,2}};
/////////////////////////////////////////////////////////////////////////////
BYTE BlackDown[6][6]={{2,2,2,2,2,2},{2,2,2,2,2,2},{0,0,0,0,0,0},{0,0,0,0,0,0},{1,1,1,1,1,1},{1,1,1,1,1,1}};
BYTE RedDown[6][6]={{1,1,1,1,1,1},{1,1,1,1,1,1},{0,0,0,0,0,0},{0,0,0,0,0,0},{2,2,2,2,2,2},{2,2,2,2,2,2}};
CFont font;  
CPen pen;

const BYTE InitChessBoard[6][6]=
{
	{BLACK,BLACK,BLACK,BLACK,BLACK,BLACK},
	{BLACK,BLACK,BLACK,BLACK,BLACK,BLACK},
	{NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS},
	{NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS,NOCHESS},
	{RED,RED,RED,RED,RED,RED},
	{RED,RED,RED,RED,RED,RED}
	
};
typedef struct node Node;
CSurakartaView::CSurakartaView()
{
	// TODO: add construction code here
	m_nMouseStep=1;
	m_begin=FALSE;
	m_nsecond=0;
	m_nminute=0;
	m_nhour=0;
	GameMode=0;
    m_isPlayerBlack=TRUE;
	m_isPlayerFirst=TRUE;
    m_isPlayerTurn=TRUE;
	
	memcpy(m_ChessBoard,InitChessBoard,36);//初始化棋盘
	CMoveGenerator *pMG;
	CEveluation *pEvel;
	m_pSE=new CNegaScout;//
	pMG=new CMoveGenerator;
	pEvel=new CEveluation;
	
	m_pSE->SetSearchDepth(5);//设定搜索层数
	m_pSE->SetMoveGenerator(pMG);//给搜索引擎设定走法产生器
	m_pSE->SetEveluator(pEvel);//给搜索引擎设定估值核心
	m_MoveChess.nChessID=NOCHESS;//将移动的棋子清空
}

CSurakartaView::~CSurakartaView()
{
}

BOOL CSurakartaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSurakartaView drawing

void CSurakartaView::OnDraw(CDC* pDC)
{
	CSurakartaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	int i;
	CPoint pt1;
	CPoint pt2;
	font.CreateFont( 22,                  //   nHeight     
            0,                            //   nWidth     
            1,                            //   nEscapement   
            0,                            //   nOrientation     
            FW_NORMAL,                    //   nWeight     
            FALSE,                        //   bItalic     
            FALSE,						  //   bUnderline     
            0,                            //   cStrikeOut     
            ANSI_CHARSET,                 //  nCharSet     
            OUT_DEFAULT_PRECIS,           //   nOutPrecision     
            CLIP_DEFAULT_PRECIS,          //   nClipPrecision     
            DEFAULT_QUALITY,              //   nQuality     
            DEFAULT_PITCH   |   FF_SWISS, //   nPitchAndFamily       
            _T("楷体"));  
	pen.CreatePen(PS_SOLID , 2 , RGB( 0 , 0 , 0 ));
	pDC->SelectObject(&pen);
	pDC->SelectObject(&font);  
	CBitmap bitmap;
	CDC dc;
	bitmap.LoadBitmap(IDB_BITMAP3);
	dc.CreateCompatibleDC(pDC);
	dc.SelectObject(&bitmap);
	pDC->BitBlt(0,0,2050,2050,&dc,0,0,SRCCOPY);
	dc.DeleteDC();
	bitmap.DeleteObject();


	pt1.x=100;
	pt1.y=100;
	pt2.x=350;
	pt2.y=100;
	for (i=0;i<6;i++)
	{
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
		pt1.y=pt1.y+50;
		pt2.y=pt2.y+50;
	}
	pt1.x=100;
	pt1.y=100;
	pt2.x=100;
	pt2.y=350;
	for (i=0;i<6;i++)
	{
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
		pt1.x=pt1.x+50;
		pt2.x=pt2.x+50;
	}
	


	pen.CreatePen(PS_SOLID , 3 , RGB( 0 , 0 , 0 ));
	pDC->SelectObject(&pen);
	pDC->Arc(0,0,200,200,200,100,100,200);
	pDC->Arc(50,50,150,150,150,100,100,150);
	pDC->Arc(0,250,200,450,100,250,200,350);
	pDC->Arc(50,300,150,400,100,300,150,350);
	pDC->Arc(250,0,450,200,350,200,250,100);
	pDC->Arc(300,50,400,150,350,150,300,100);
	pDC->Arc(250,250,450,450,250,350,350,250);
    pDC->Arc(300,300,400,400,300,350,350,300);
	
	DrawChess(color);
    str.Format("%d",m_nMouseStep-1);
	CString s,s1,s2;
	s.Format("%d",m_nhour);
	s1.Format("%d",m_nminute);
	s2.Format("%d",m_nsecond);

	pDC->SetBkMode(TRANSPARENT);
 
}

/////////////////////////////////////////////////////////////////////////////
// CSurakartaView printing

BOOL CSurakartaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSurakartaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSurakartaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSurakartaView diagnostics

#ifdef _DEBUG
void CSurakartaView::AssertValid() const
{
	CView::AssertValid();
}

void CSurakartaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSurakartaDoc* CSurakartaView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSurakartaDoc)));
	return (CSurakartaDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSurakartaView message handlers

void CSurakartaView::OnBegin() 
{
	// TODO: Add your command handler code here
	StartDlg dlg;
	UpdateData();
	CDC *pDC=GetDC();
	if(dlg.DoModal()==IDOK)
	{	
		if (dlg.m_red==0)
		{
			memcpy(color,RedDown,36);
            m_isPlayerBlack=FALSE;
		}
		else
		{
			//memcpy(color,BlackDown,36);
			m_isPlayerBlack=TRUE;
		}
		DrawChess(color);
		if (dlg.m_computer==0)
		{
			m_isPlayerFirst=FALSE;
			m_isPlayerTurn=FALSE;
			OnLButtonUp(1,CPoint(0,0));
		}
		else if(dlg.m_computer==1)
		{
			m_isPlayerFirst=TRUE;
			m_isPlayerTurn=TRUE;
		}
		else
			AfxMessageBox("请选择谁先下棋！");
	}
	m_begin=TRUE;	
	//pDC->TextOut(210,30,"电脑");
	SetTimer(1,1000,NULL);

}

void CSurakartaView::OnBack() 
{
	// TODO: Add your command handler code here
	CDC *pDC=GetDC();
    DrawChess(m_BackupChessBoard);
	m_nMouseStep=BackStep;
	memcpy(color,m_BackupChessBoard,36);
	str.Format("%d",m_nMouseStep/2);
	//pDC->TextOut(450,150,"步数："+str);
	//if(m_isPlayerBlack)
      //  pDC->TextOut(450,200,"It turns to  Black!");
	//else
	//	pDC->TextOut(450,200,"It turns to  Red!");
}


void CSurakartaView::OnRebegin() 
{
	// TODO: Add your command handler code here
	CDC *pDC=GetDC();
	Invalidate(); 
	UpdateWindow();
	m_nMouseStep=1;
	m_begin=FALSE;
	m_nsecond=0;
	m_nminute=0;
	m_nhour=0;
	int i,j;
	for(i=0;i<2;i++)
		for (j=0;j<6;j++)
			color[i][j]=1;
		for(i=2;i<4;i++)
			for (j=0;j<6;j++)
				color[i][j]=0;
			for(i=4;i<6;i++)
				for (j=0;j<6;j++)
					color[i][j]=2;
	Invalidate(); 
	UpdateWindow();
}


void CSurakartaView::OnSet() 
{
	// TODO: Add your command handler code here
	CMoveGenerator *pMG;
	CEveluation *pEvel;
	CNewGame newGameDlg;
	
	if (newGameDlg.DoModal()==IDOK)
	{
		if(m_pSE)
			delete m_pSE;//释放旧的搜索引擎
		switch(newGameDlg.GetSelectedEngine())
		{
		case 0:
			m_pSE=new CNegaScout;
			break;
		default:
			m_pSE=new CNegaScout;
			break;
		}
		m_pSE->SetSearchDepth(newGameDlg.GetSelectedPly());
		pEvel=new CEveluation;
		
	}
	else
		return;//维持现状
	memcpy(m_ChessBoard,InitChessBoard,36);
	m_MoveChess.nChessID=NOCHESS;//清除移动棋子
	pMG=new CMoveGenerator;
	m_pSE->SetMoveGenerator(pMG);
	m_pSE->SetEveluator(pEvel);
}


void CSurakartaView::DrawChess(BYTE color[6][6])
{
	int i,j;
	CDC *pDC=GetDC();
	CBitmap bitmap;
	CDC dc;
	int X,Y;
		

	for (i=0;i<6;i++)
	{
		for (j=0;j<6;j++)
		{
			if (color[i][j]==1)
			{
				X=j*50+84;
				Y=i*50+84;
				bitmap.LoadBitmap(IDB_BITMAP1);
				dc.CreateCompatibleDC(pDC);
				dc.SelectObject(&bitmap);
				pDC->BitBlt(X,Y,X+50,Y+50,&dc,0,0,SRCCOPY);
				dc.DeleteDC();
				bitmap.DeleteObject();
			}
			if (color[i][j]==2)
			{
				X=j*50+84;
				Y=i*50+84;
				bitmap.LoadBitmap(IDB_BITMAP2);
				dc.CreateCompatibleDC(pDC);
				dc.SelectObject(&bitmap);
				pDC->BitBlt(X,Y,X+50,Y+50,&dc,0,0,SRCCOPY);
				dc.DeleteDC();
				bitmap.DeleteObject();
			}
		}
	}
}


void CSurakartaView::Inverse(BYTE color[][6])
{
	int i,j;
	for(i=0;i<6;i++)
		for(j=0;j<6;j++)
			if(color[i][j]==1 || color[i][j]==2)
				color[i][j]=3-color[i][j];
}



void CSurakartaView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int i,j,flag=0;	
	if(m_begin==TRUE &&m_isPlayerTurn  ||GameMode){	
	CDC *pDC=GetDC();
	CDC dc;
	CBitmap bitmap;

	if(m_nMouseStep%2==1)
	{
		m_StarPoint=point;
		for ( i=75;i<375;i=i+50)
		{
			if (m_StarPoint.x>i&&m_StarPoint.x<i+50)
			{	
				Star_X=i+1;
				Star_j=(i-75)/50;//起点数组纵坐标
			}
		}
		for( i=75;i<375;i=i+50)
		{
			if (m_StarPoint.y>i&&m_StarPoint.y<i+50)
			{
				Star_Y=i+1;
				Star_i=(i-75)/50;//
			}
		}//保存起始位置
		
		{
		    if(color[Star_i][Star_j]==1 && m_isPlayerBlack)
			{
				bitmap.LoadBitmap(IDB_BITMAP5);
				dc.CreateCompatibleDC(pDC);
				dc.SelectObject(&bitmap);		
				pDC->BitBlt(Star_X,Star_Y,Star_X+50,Star_Y+50,&dc,0,0,SRCCOPY);
				dc.DeleteDC();
				bitmap.DeleteObject();	 
			
				memcpy(m_BackupChessBoard,color,36);
				BackStep=m_nMouseStep;
			    m_nMouseStep++;
			}
			if(color[Star_i][Star_j]==2 && !m_isPlayerBlack)
			{
				bitmap.LoadBitmap(IDB_BITMAP6);
				dc.CreateCompatibleDC(pDC);
				dc.SelectObject(&bitmap);		
				pDC->BitBlt(Star_X,Star_Y,Star_X+50,Star_Y+50,&dc,0,0,SRCCOPY);
				dc.DeleteDC();
				bitmap.DeleteObject();	 
		
				memcpy(m_BackupChessBoard,color,36);
				BackStep=m_nMouseStep;
			    m_nMouseStep++;
			}	    
		}
		
	}
	
	else //走动
	{
		///////贴棋盘的图
		//bitmap.LoadBitmap(IDB_BITMAP3);
		dc.CreateCompatibleDC(pDC);
		dc.SelectObject(&bitmap);
		CMoveGenerator *pMG;
		pMG=new CMoveGenerator;
		for ( i=75;i<375;i=i+50)
			{
				if (point.x>i&&point.x<i+50)
				{
				m_nX=i;
				m_nj=(i-75)/50;//点到的位置的坐标
				}
			}
		for( i=75;i<375;i=i+50)
			{
				if (point.y>i&&point.y<i+50)
				{
				m_nY=i;
				m_ni=(i-75)/50;
				}
			}
		if (pMG->IsValidMove(color,Star_i,Star_j,m_ni,m_nj))
		{
			m_isPlayerTurn=!m_isPlayerTurn;	
			if (color[m_ni][m_nj]==0)
			{
				pDC->BitBlt(Star_X,Star_Y,Star_X+50,Star_Y+50,&dc,0,0,SRCCOPY);
				dc.DeleteDC();
				bitmap.DeleteObject();
				if (color[Star_i][Star_j]==1)
				{
					pDC->TextOut(450,200,"It turns to  Red!");
				
					m_nMouseStep++;
					color[m_ni][m_nj]=1;
					color[Star_i][Star_j]=0;
					DrawChess(color);
				}
				else
				{
					pDC->TextOut(450,200,"It turns to  Black! ");			
			
					m_nMouseStep++;
					color[m_ni][m_nj]=2;
					color[Star_i][Star_j]=0;
					DrawChess(color);
				}
			}
			else if (color[m_ni][m_nj]==1)
			{		
				pDC->TextOut(450,200,"It turns to  Black!");	
				m_nMouseStep++;
				color[Star_i][Star_j]=0;
				color[m_ni][m_nj]=2;
				DrawChess(color);
				flag=0;
				for(i=0;i<6;i++)
					for(j=0;j<6;j++)
						if(color[i][j]==1)
						{
							flag=1;
							break;
						}	   
						if(flag==0)
						{
							AfxMessageBox("红棋获胜！");
							KillTimer(1);
						}		
			}
			else if (color[m_ni][m_nj]==2)
			{
				pDC->TextOut(450,200,"It turns to  Red!");							
				m_nMouseStep++;
				color[Star_i][Star_j]=0;
				color[m_ni][m_nj]=1;
				DrawChess(color);
				flag=0;
				for(i=0;i<6;i++)
					for(j=0;j<6;j++)
						if(color[i][j]==2)
						{
							flag=1;
							break;
						}	   
						if(flag==0)
						{ 
							AfxMessageBox("黑棋获胜！");
							KillTimer(1);
						}
			}
			//刷新棋盘
			Invalidate(); 
			//UpdateWindow();
		}
		else if(color[m_ni][m_nj]==color[Star_i][Star_j])
		{
			m_nMouseStep--;
			Invalidate(); 
		}
		else if(color[m_ni][m_nj]==0)
		{
			AfxMessageBox("无法到达！请重新落子。");
		}

}
		

str.Format("%d",m_nMouseStep/2);
pDC->TextOut(450,150,"步数："+str);
char msg[40];
      for ( i=75;i<375;i=i+50)
		if (point.x>i&&point.x<i+50)
			msg[0]=(i-75)/50;//点到的位置的坐标	  
      for( i=75;i<375;i=i+50)	 
		if (point.y>i&&point.y<i+50)
			msg[1]=(i-75)/50;					
CView::OnLButtonDown(nFlags, point);
}

}

void CSurakartaView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(((m_begin && !m_isPlayerTurn)  || nFlags) && !GameMode)
	{
		int j,i,flagR=0,flagB=0;
		CDC *pDC=GetDC();
		CDC dc;

//		if(!m_isPlayerBlack)
//			Inverse(color);
		m_BestMove=m_pSE->SearchAGoodMove(color,!m_isPlayerBlack);
//		if(!m_isPlayerBlack)
//			Inverse(color);			
		DrawChess(color);

		m_nMouseStep=m_nMouseStep+2;
		m_isPlayerTurn=!m_isPlayerTurn;
		for(i=0;i<6;i++)
			for(j=0;j<6;j++)
			{
				if(color[i][j]==1)
				{
					flagR=1;
					continue;
				}
				if(color[i][j]==2)
				{
					flagB=1;
					continue;
				}
			}
				if(flagR==0)
				{
					AfxMessageBox("Red Win!");
					KillTimer(1);
				}
				
				if(flagB==0)
				{
					AfxMessageBox("Black Win!");
					KillTimer(1);
				}
				
				str.Format("%d",m_nMouseStep/2);
				pDC->TextOut(450,150,"步数："+str);
				if(m_isPlayerBlack)
					pDC->TextOut(450,200,"It turns to  Black!");	
				else
					pDC->TextOut(450,200,"It turns to  Red!");
				//刷新棋盘
				Invalidate(); 
				//UpdateWindow();
	}
	CView::OnLButtonUp(nFlags, point);
}

void CSurakartaView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	m_nsecond++;
	if (m_nsecond>60)
	{
		m_nsecond=0;
		m_nminute++;
		if (m_nminute>60)
		{
			m_nminute=0;
			m_nhour++;
		}
		CView::OnTimer(nIDEvent);
	}
	CDC *pDC=GetDC();
	CString s,s1,s2;
	s.Format("%d",m_nhour);
	s1.Format("%d",m_nminute);
	s2.Format("%d",m_nsecond);
	pDC->TextOut(150,450,"已用时间："+ s +":"+ s1 +":"+ s2 +"  ");
	
}