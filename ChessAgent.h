// NegaScout.h: interface for the ChessAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_)
#define AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SearchEngine.h"
#include "ChessBoard.h"
//#define ABDebug
class ChessAgent :
	public CSearchEngine
{
public:
	ChessAgent();
	virtual ~ChessAgent();
	virtual CHESSMOVE SearchAGoodMove(BYTE position[6][6], int m_isPlayerBlack);
protected:
	int negaScout_TT_HH(int depth, int num, int m_Type);
	int negaScout_ABTree(int depth, int m_Type, int alpha = 0, int beta = 0);
	int negaScoutMinWin(int depth, int m_Type, int alpha = MIN_INT, int beta = MAX_INT);
	CHESSMOVE m_bestMove;
};

#endif // !defined(AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_)