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

typedef enum threadStatus {
	ready,
	zeroWin,
	running,
	finished,
	killed
}ThreadStatus;

typedef struct _task {
	//棋盘信息
	BYTE position[6][6];
	bool isBlackTurn;
	ID_TYPE boardId;
	//控制变量
	int searchDepth = SEARCH_DEPTH;
	int useMethod = PVS;
	//搜索信息
	bool isBlackPlay = true;//一般都是我方走红
	int depth = SEARCH_DEPTH - 1;
	int alpha = MIN_VALUE;
	int beta = MAX_VALUE;

	void setCtrlParam(int sDepth, int method) {
		searchDepth = sDepth;
		useMethod = method;
	}
	void setSearchParam(int d, bool play, int a, int b) {
		depth = d;
		isBlackPlay = play;
		alpha = a;
		beta = b;
	}

} Task;

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
	~CNegaScout();
	CHESSMOVE searchAGoodMove();
	CHESSMOVE SearchAGoodMove(BYTE position[6][6], bool m_isPlayerBlack);

	void setPosition(BYTE position[6][6], bool isBlackTurn);
	void setPlayerSide(bool isBlackPlay) { this->isBlackPlay = isBlackPlay; }
	void setMethod(int sMethod) { searchMethod = sMethod; }
	void setWeightVector(WeightVector &wv) { m_pEval.setWeightVector(wv); }
	void setUseMultiProcess(bool use = true) { uMultiProcess = use; }
	void setUseMapVersion(MapVersion v = current) { m_pEval.setMapVersion(v); }
	void setSearchDepth(int depth) { searchDepth = depth; }

	double getLastSearchTime() { return lastSearchTime; }
	CHESSMOVE getBestMove() { return bestMove; }
	int getLastScore() { return lastScore; }

	Task createTask();
	bool useMultiProcess(int depth);

	int isGameOver();//判断是否有胜负
	int isGameOver(ChessBoard chessBoard);

	inline int negaScoutSearch(Task &t);
	inline bool negaScoutPrevTest(int depth, int &alpha, int &beta, int &value);
	int negaScoutMinMax(int depth,int num,bool isBlackPlay);
	int negaScoutAlphaBeta(int depth = SEARCH_DEPTH, int alpha = MIN_VALUE, int beta = MAX_VALUE);
	int negaScoutMinWin(int depth = SEARCH_DEPTH, int alpha = MIN_VALUE, int beta = MAX_VALUE);
	int negaScoutPVS(int depth = SEARCH_DEPTH, int alpha = MIN_VALUE, int beta = MAX_VALUE);
	int negaScoutBT(int depth = SEARCH_DEPTH, int alpha = MIN_VALUE, int beta = MAX_VALUE);
	int negaScoutMTD(int depth = SEARCH_DEPTH, int alpha = MIN_VALUE, int beta = MAX_VALUE);
private:
    CHESSMOVE m_bestMove;
	ChessBoard chessBoard;
	bool isBlackPlay;

	int searchMethod = PVS;
	int searchDepth = SEARCH_DEPTH;
	double lastSearchTime;
	int lastScore;

	bool uMultiProcess = false;
};

#endif // !defined(AFX_NEGASCOUT_H__F7857B67_4C28_429B_8DE7_2F6F7CBE3572__INCLUDED_)
