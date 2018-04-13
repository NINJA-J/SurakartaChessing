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
#include "ThreadPool.h"
#include <atomic>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>

//#define ABDebug
class CNegaScout : 
	public CSearchEngine 
{
public:
	CNegaScout();
	CNegaScout(Task &t);
	CNegaScout(bool isBlackTurn);
	CNegaScout(BYTE position[6][6], bool isBlackTurn);
	CNegaScout(BYTE position[6][6], bool isBlackTurn, CHESSMOVE move);
	//CNegaScout(int k);
	virtual ~CNegaScout();
	virtual CHESSMOVE SearchAGoodMove(BYTE position[6][6],bool m_isPlayerBlack);

	void setPosition(BYTE position[6][6], bool isBlackTurn);
	void setPlayerSide(bool isBlackPlay) { this->isBlackPlay = isBlackPlay; };
	void move(CHESSMOVE move);

	Task createTask();
	bool useMultiProcess(int depth);

	int isGameOver();//≈–∂œ «∑Ò”– §∏∫
	int isGameOver(ChessBoard chessBoard);

	inline int negaScoutSearch(Task &t);
	inline bool negaScoutPrevTest(int depth, int &value);
	int NegaScout_TT_HH(int depth,int num,bool isBlackPlay);
	int negaScoutAlphaBeta(int depth = SEARCH_DEPTH, int alpha = MIN_VALUE, int beta = MAX_VALUE);
	int negaScoutMinWin(int depth = SEARCH_DEPTH, int alpha = MIN_VALUE, int beta = MAX_VALUE);
	int negaScoutPVS(int depth = SEARCH_DEPTH, int alpha = MIN_VALUE, int beta = MAX_VALUE);

	//	static void negaScoutMinWin(int kidTreeValue, ChessBoard chessBoard, int depth, bool isBlackPlay, int alpha = MIN_VALUE, int beta = MAX_VALUE);
private:
    CHESSMOVE m_bestMove;
	ChessBoard chessBoard;
	bool isBlackPlay;
	int searchMethod = PVS;
};

#endif // !defined(AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_)
