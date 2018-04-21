// SurakartaView.h : interface of the CSurakartaView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURAKARTAVIEW_H__C2CAF01A_D372_4611_B604_CD959E906362__INCLUDED_)
#define AFX_SURAKARTAVIEW_H__C2CAF01A_D372_4611_B604_CD959E906362__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SurakartaDoc.h"
#include "Define.h"
#include "NegaScout.h"
typedef struct _movechess
{
	BYTE nChessID;
	POINT ptMovePoint;
	
}MOVECHESS;

class CSurakartaView : public CView
{
protected: // create from serialization only
	CSurakartaView();
	DECLARE_DYNCREATE(CSurakartaView)

// Attributes
public:
	CSurakartaDoc* GetDocument();
	CPoint m_Black;
	CPoint m_Red;
	CPoint m_StarPoint;
	int m_nMouseStep;
	int m_nX;
	int Star_X;
	int Star_Y;
	int m_nY;
	int Star_i;
	int Star_j;
	int m_ni;
	int m_nj;
	BOOL m_begin;
	int m_nsecond,m_nminute,m_nhour;
	CString str;
    BOOL m_isPlayerBlack;
	BOOL m_isPlayerFirst;
	BOOL m_isPlayerTurn;
	int GameMode;
	int BackStep;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSurakartaView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void isBlackTurn(UINT nFlags);
	void isRedTurn(UINT nFlags);

	virtual ~CSurakartaView();
public:
	void Inverse(BYTE color[6][6]);
	void DrawChess(BYTE color[6][6]);
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSurakartaView)
	afx_msg void OnBegin();
	afx_msg void OnBack();
	afx_msg void OnRebegin();
	afx_msg void OnSet();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BYTE m_ChessBoard[6][6];//棋盘数组
	BYTE m_BackupChessBoard[6][6];//用于错误恢复
	MOVECHESS m_MoveChess;//保存当前被拖拽的棋子的结构
	POINT m_ptMoveChess;//用于保存当前被拖拽的棋子的位置
	CSearchEngine *m_pSE;//搜索引擎的指针
	BOOL RedEat;	
	CHESSMOVE m_BestMove;
};

#ifndef _DEBUG  // debug version in SurakartaView.cpp
inline CSurakartaDoc* CSurakartaView::GetDocument()
   { return (CSurakartaDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SURAKARTAVIEW_H__C2CAF01A_D372_4611_B604_CD959E906362__INCLUDED_)
