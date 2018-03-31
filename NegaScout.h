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
#include <thread>
using namespace std;
//#define ABDebug
class CNegaScout : 
	public CSearchEngine 
{
public:
	CNegaScout();
	CNegaScout(int k);
	virtual ~CNegaScout();
	virtual CHESSMOVE SearchAGoodMove(BYTE position[6][6],bool m_isPlayerBlack);
protected:
	double NegaScout_TT_HH(int depth,int num,bool isBlackPlay);
	double NegaScout_ABTree(int depth, bool isBlackPlay, BV_TYPE alpha = MIN_VALUE, BV_TYPE beta = MAX_VALUE);
	double negaScoutMinWin(ChessBoard chessBoard, int depth, bool isBlackPlay, BV_TYPE alpha = MIN_VALUE, BV_TYPE beta = MAX_VALUE);
	static void negaScoutMinWin(int kidTreeValue, ChessBoard chessBoard, int depth, bool isBlackPlay, BV_TYPE alpha = MIN_VALUE, BV_TYPE beta = MAX_VALUE);

	static double threadMain();
	double Main_NegaScout_ABTree(int depth, bool isBlackPlay, BV_TYPE alpha = MIN_VALUE, BV_TYPE beta = MAX_VALUE);
    CHESSMOVE m_bestMove;
	thread threads[96];
};

#endif // !defined(AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_)
