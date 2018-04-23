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

	BV_TYPE isGameOver();//�ж��Ƿ���ʤ��
	BV_TYPE isGameOver(ChessBoard chessBoard);

	inline BV_TYPE negaScoutSearch(Task &t);
	inline bool negaScoutPrevTest(int depth, BV_TYPE &value);
	BV_TYPE NegaScout_TT_HH(int depth,int num,bool isBlackPlay);
	BV_TYPE negaScoutAlphaBeta(int depth = SEARCH_DEPTH, BV_TYPE alpha = MIN_VALUE, BV_TYPE beta = MAX_VALUE);
	BV_TYPE negaScoutMinWin(int depth = SEARCH_DEPTH, BV_TYPE alpha = MIN_VALUE, BV_TYPE beta = MAX_VALUE);
	BV_TYPE negaScoutPVS(int depth = SEARCH_DEPTH, BV_TYPE alpha = MIN_VALUE, BV_TYPE beta = MAX_VALUE);
	BV_TYPE negaScoutPVSProc(int depth = SEARCH_DEPTH, BV_TYPE alpha = MIN_VALUE, BV_TYPE beta = MAX_VALUE);
	BV_TYPE negaScoutMinWinProc(int depth, bool isBlackPlay, BV_TYPE alpha, BV_TYPE beta);


	//	static void negaScoutMinWin(int kidTreeValue, ChessBoard chessBoard, int depth, bool isBlackPlay, BV_TYPE alpha = MIN_VALUE, BV_TYPE beta = MAX_VALUE);
private:
    CHESSMOVE m_bestMove;
	ChessBoard chessBoard;
	bool isBlackPlay;
	int searchMethod = PVS;
};

#endif // !defined(AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_)
