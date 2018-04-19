// NegaScout.cpp: implementation of the CNegaScout class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"
#include "NegaScout.h"
#include "Define.h"

#include <queue>
#include <ctime>

#include <atomic>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ThreadPool threadPool(THREAD_N);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////// /////////////////////////////////////////////////

CNegaScout::CNegaScout(){
}

CNegaScout::CNegaScout(Task & t):chessBoard(t.position,t.isBlackTurn,t.boardId) {
	/*		棋盘信息 -- 这个部分在ChessBoard中初始化了
	*	BYTE position[6][6];	used
	*	bool isBlackTurn;		used
	*	ID_TYPE boardId;		used
	*		控制变量
	*	int searchDepth;		used
	*	int useMethod;			use in negaScoutSearch
	*		搜索信息
	*	int depth;				use in negaScout***
	*	bool isBlackPlay;		used
	*	int alpha;			use in negaScout***
	*	int beta;			use in negaScout***
	*/
	searchDepth = m_nMaxDepth = m_nSearchDepth = t.searchDepth;
	isBlackPlay = t.isBlackPlay;
}

CNegaScout::CNegaScout(bool isBlackTurn):chessBoard(isBlackTurn) { }

CNegaScout::CNegaScout(BYTE position[6][6], bool isBlackTurn):chessBoard(position,isBlackTurn) { }

CNegaScout::CNegaScout(BYTE position[6][6], bool isBlackTurn, CHESSMOVE move):chessBoard(position, isBlackTurn) {
	chessBoard.move(move);
}

CNegaScout::~CNegaScout() { }

CHESSMOVE CNegaScout::SearchAGoodMove(BYTE position[6][6], bool m_isPlayerBlack) {
	int score;

	chessBoard.setChessPosition(position, m_isPlayerBlack);
	isBlackPlay = m_isPlayerBlack;
	searchDepth = m_nMaxDepth = SEARCH_DEPTH;

/*	目前MinWin的多线程可运行，PVS单线程可运行，并且加入了MAP功能
 *	若要打开多线程功能，在Define.h中取消USE_MULTI_PROCESS的注释
 *	后期想尝试在多线程中加入更新alpha和beta的功能，即先运行5个节点，返回后根据结果生成新任务的alpha和beta

 *	主要参数设置都在Define.h中
 */
	clock_t startTime = clock();
//	score = negaScoutMinWin(SEARCH_DEPTH);
//	score = negaScoutAlphaBeta(SEARCH_DEPTH);
	score = negaScoutPVS(searchDepth);
	clock_t runTime = clock() - startTime;

	memcpy(CurPosition, position, sizeof(BYTE) * 36);
	MakeMove(&bestMove);
	memcpy(position, CurPosition, sizeof(BYTE) * 36);

	CString temp;
	temp.Format(
		"运行时间：%.2f\n走法：(%d,%d)->(%d,%d)\n分数：%d", (double)runTime / CLOCKS_PER_SEC,
		bestMove.From.x, bestMove.From.y, bestMove.To.x, bestMove.To.y, score
	);
	AfxMessageBox(temp);
	return bestMove;
}

void CNegaScout::setPosition(BYTE position[6][6], bool isBlackTurn) {
	chessBoard.setChessPosition(position, isBlackTurn);
}

Task CNegaScout::createTask() {
	Task t;
	chessBoard.getPosition(t.position);
	t.isBlackTurn = chessBoard.getTurn();
	t.isBlackPlay = isBlackPlay;
	t.boardId = chessBoard.getIdRaw();

	return t;
}

bool CNegaScout::useMultiProcess(int depth) {
#ifdef USE_MULTI_PROCESS
	return searchDepth - depth < PROC_DEPTH;
#else
	return false;
#endif
}

inline int CNegaScout::negaScoutSearch(Task &t) {
	switch (t.useMethod) {
	case PVS:		return -negaScoutPVS(t.depth, -t.beta, -t.alpha);
	case MIN_WIN:	return -negaScoutMinWin(t.depth, -t.beta, -t.alpha);
	case AB_TREE:	return -negaScoutAlphaBeta(t.depth, -t.beta, -t.alpha);
	case MTD_F:		return 0;
	case ZERO_WIN:	return -negaScoutAlphaBeta(t.depth, -t.alpha-1, -t.alpha);
	default:		return MIN_VALUE;
	}
	return MIN_VALUE;
}

inline bool CNegaScout::negaScoutPrevTest(int depth,int &alpha,int &beta, int &value) {
	if (value = isGameOver())
		return true;
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, alpha, beta, value))
		return true;
	if (depth <= 0) {
		value = m_pEval.addBoardValue(
			chessBoard.getId(), 0, alpha, beta, m_pEval.evaluate(chessBoard, isBlackPlay)
		);
		return true;
	}
	return false;
}

int CNegaScout::negaScoutMinMax(int depth,int num,bool isBlackPlay) {
	int t;
	int Count, i;
	int side;
	int best = -99999;
	CHESSMOVE mList[200];
	i = isGameOver();//判断胜负
	if (i != 0) return i;
	side = (m_nMaxDepth - depth + isBlackPlay) % 2;//判断预测是己方该走还是敌方该走

	
	if (depth <= 0) return m_pEval.evaluate(chessBoard, isBlackPlay);

	Count = m_pMG.createPossibleMoves(chessBoard,mList,200);//返回下一步多少种走法

	for (i = 0; i < Count; i++) {
		chessBoard.move(mList[i]);

		t = -negaScoutMinMax(depth - 1, i, isBlackPlay);//递归调用
		if (t > best) {
			best = t;
			if(depth == m_nMaxDepth) bestMove = mList[i];//存储最优走法
		}
		chessBoard.unMove();//还原棋盘
	}
	return best;
}

int CNegaScout::negaScoutAlphaBeta(int depth, int alpha, int beta) {
	CHESSMOVE moveList[200];
	int value, best = MIN_VALUE;

	if (negaScoutPrevTest(depth, alpha, beta, value))return value;

	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);
	int aOrigin = alpha, bOrigin = beta;

	for (int i = 0; i < count; i++) {
		chessBoard.move(moveList[i]);
		value = -negaScoutAlphaBeta(depth - 1, -beta, -alpha);
		chessBoard.unMove();

		best = max(best, value);
		if (value > alpha) {
			alpha = value;
			if (depth == m_nMaxDepth) bestMove = moveList[i];
			if (alpha >= beta) break;
		}
	}
	return m_pEval.addBoardValue(chessBoard.getId(), depth, aOrigin, bOrigin, best);
}

int CNegaScout::negaScoutMinWin(int depth, int alpha, int beta) {
	CHESSMOVE moveList[200];
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;
	int side = (m_nMaxDepth - depth + isBlackPlay) % 2;//当前层谁走子

	int best = isMax ? MIN_VALUE : MAX_VALUE;//初始化搜索的估值的最值

	if (int i = isGameOver(chessBoard)) return i;//终局
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, alpha, beta, best)) return best;
	if (depth <= 0)
		return m_pEval.addBoardValue(
			chessBoard.getId(), alpha, beta, 0, m_pEval.evaluate(chessBoard, isBlackPlay)
		);

	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);

	if (useMultiProcess(depth)) {
		vector<future<int>> results;

		for (int i = 0; i < count; i++) {
			chessBoard.move(moveList[i]);

			Task t = createTask();
			t.setCtrlParam(searchDepth, MIN_WIN);
			t.setSearchParam(depth - 1, isBlackPlay, MIN_VALUE, MAX_VALUE);

			results.emplace_back(
				threadPool.enqueue([i](Task t) {
				CNegaScout negaScout(t);
				return negaScout.negaScoutSearch(t);
			}, t));

			chessBoard.unMove();
		}

		int max = MIN_VALUE;
		for (int i = 0; i < count; i++) {
			int t = results[i].get();
			if (t > max) {
				max = t;
				bestMove = moveList[i];
			}
		}
		return max;
	} else {
		for (int i = 0; i < count; i++) {
			chessBoard.move(moveList[i]);
			int t = negaScoutMinWin(depth - 1, alpha, beta);
			chessBoard.unMove();

			if (isMax) {
				if (best < t) { //获得更大的估值
					best = t;
					alpha = max(alpha, best);
					if (depth == m_nMaxDepth) bestMove = moveList[i];
					if (alpha > beta) break; //无法使上层变小，剪枝
				}
			} else {
				if (best > t) {
					best = t;
					beta = min(beta, best);
					if (t < alpha) break;
				}
				
			}
		}
		return m_pEval.addBoardValue(chessBoard.getId(), depth, alpha, beta, best);//返回最值
	}
}

int CNegaScout::negaScoutPVS(int depth, int alpha, int beta) {
	/*
	pvs(node, depth, alpha, beta, player)
		if (depth = 0)
			return valuation(player)
		else
			if (player = maxplayer)
				for each child of node
					if child is first child
						value := pvs(child, depth-1, alpha, beta, minplayer)
					else
						value := pvs(child, depth-1, alpha, alpha+1, minplayer)
					if alpha <value< beta
						value := pvs(child, depth-1, value, beta, minplayer)
					if (value >alpha)alpha:=value
					if (alpha >=beta)break
				return alpha
			else
				for each child of node
					if child is first child
						value := pvs(child, depth-1, alpha, beta, maxplayer)
					else
						value := pvs(child, depth-1, beta-1, beta, maxplayer)
					if alpha < value < beta
						value := pvs(child,depth-1,alpha,value,maxplayer)
					if(value<beta)beta:=value
					if (alpha >= beta)break
				return alpha
	*/
	int value, best = MIN_VALUE;
	if (negaScoutPrevTest(depth, alpha, beta, value))return value;

	int aOrigin = alpha, bOrigin = beta;
	CHESSMOVE moveList[200];
	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);

	if (useMultiProcess(depth)) {
		ThreadStatus status[96];
		Task tasks[96];
		future<int> results[96];

		int maxThreads = 5,i;
		int runningThreads = 0;
		int lastThreads = count - 1;
		bool hasSpace = false,pruned = false;

		chessBoard.move(moveList[0]);
		int value = -negaScoutPVS(depth - 1, -beta, -alpha);
		chessBoard.unMove();

		best = max(best, value);
		if (value > alpha) {
			alpha = value;
			if (depth == m_nMaxDepth) bestMove = moveList[0];
			if (alpha >= beta) pruned = true;
		}

		for (i = 1; i < count; i++) {
			chessBoard.move(moveList[i]);
			tasks[i] = createTask();
			chessBoard.unMove();
			status[i] = ready;
		}

		i = 1;
		while(lastThreads&&!pruned){
			hasSpace = false;
			if (status[i] == zeroWin || status[i] == running) {//当前线程正在计算
				if (results[i].wait_for(chrono::milliseconds(5)) == future_status::ready) {//判断当前线程是否计算结束
					int val = results[i].get();
					runningThreads--;//运行线程数量减一

					best = max(val, best);
					if (val > alpha) {
						alpha = val;
						if (depth == searchDepth) bestMove = moveList[i];
						if (alpha >= beta) pruned = true;
					}

					if (val < alpha||status[i]==running) {
						status[i] = finished;
						lastThreads--;
					}
					else
						hasSpace = true;
				}
			}
			else if (status[i] == ready&&runningThreads < maxThreads)//当前线程未开始计算，且有空间进行计算
				hasSpace = true;

			if (hasSpace&&!pruned) {
				runningThreads++;
				tasks[i].setSearchParam(depth - 1, isBlackPlay, alpha, beta);
				if (status[i] == ready) {
					tasks[i].setCtrlParam(searchDepth, ZERO_WIN);
					results[i] = threadPool.enqueue([i](Task t) {
						CNegaScout negaScout(t);
						return negaScout.negaScoutSearch(t);
					}, tasks[i]);
					status[i] = zeroWin;
				}
				else if (status[i] == zeroWin) {
					tasks[i].setCtrlParam(searchDepth, PVS);
					results[i] = threadPool.enqueue([i](Task t) {
						CNegaScout negaScout(t);
						return negaScout.negaScoutSearch(t);
					}, tasks[i]);
					status[i] = running;
				}
			}
			if (++i == count) i = 1;
		}
	} else {
		//后面的节点在之前的基础上搜索
		for (int i = 0; i < count; i++) {
			chessBoard.move(moveList[i]);
			if (i == 0) {
				value = -negaScoutPVS(depth - 1, -beta, -alpha);
			} else {
				value = -negaScoutPVS(depth - 1, -alpha - 1, -alpha);//零窗口搜索
				if (alpha < value && value < beta)
					value = -negaScoutPVS(depth - 1, -beta, -value);
			}
			chessBoard.unMove();

			//更新alpha和beta并判断剪枝
			if (value > best) {
				if (depth == searchDepth)bestMove = moveList[i];
				best = value;
			}
			if (value > alpha) {
				alpha = value;
				if (alpha >= beta) break;
			}
		}
	}
	
	return m_pEval.addBoardValue(chessBoard.getId(), depth, aOrigin, bOrigin, best);
}

int CNegaScout::negaScoutBT(int depth, int alpha, int beta) {
	return 0;
}

int CNegaScout::negaScoutMTD(int depth, int alpha, int beta) {
	return 0;
}

int CNegaScout::isGameOver() {//未结束返回0，结束返回极大/极小值
	if (BYTE winner = chessBoard.isGameOver()) {
		int score = MAX_INT - chessBoard.getMoves() - 1;
		return /*((winner == BLACK) ^ chessBoard.getTurn()) ? */-score /*: score*/; //异或
	} else
		return 0;
}

int CNegaScout::isGameOver(ChessBoard chessBoard) {//未结束返回0，结束返回极大/极小值
	if (int type = chessBoard.isGameOver()) {
		int score = MAX_VALUE - chessBoard.getMoves() - 1;
		return ((type == B_WIN) ^ isBlackPlay) ? -score : score; //异或
	}
	else
		return 0;
}
