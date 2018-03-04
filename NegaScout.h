// NegaScout.h: interface for the CNegaScout class.
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
class CNegaScout : 
	public CSearchEngine 
{
public:
	CNegaScout();
	virtual ~CNegaScout();
	virtual CHESSMOVE SearchAGoodMove(BYTE position[6][6],bool m_isPlayerBlack);
protected:
	double NegaScout_TT_HH(int depth,int num,bool isBlackPlay);
	double NegaScout_ABTree(int depth, bool isBlackPlay, int alpha = 0, int beta = 0);
	double negaScoutMinWin(int depth, bool isBlackPlay, int alpha = MIN_INT, int beta = MAX_INT);
    CHESSMOVE m_bestMove;
};

#endif // !defined(AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_)
