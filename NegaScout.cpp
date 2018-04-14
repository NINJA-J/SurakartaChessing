// NegaScout.cpp: implementation of the CNegaScout class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"
#include "NegaScout.h"
#include "Define.h"

#include <queue>

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

/* 在构造函数中加入输出语句后发现程序每一帧都会析构NegaScout和创建新的NegaScout，因此线程池的创建采用单次创建方式
 * 用静态的bool变量存储是否创建过线程池，析构时判断若创建过则进行析构操作
 * 线程的中止方式可见threadMain函数，采用无任务自动跳出循环的方式，即将任务清空（析构是一般都是这样）后notify_all
 */
CNegaScout::CNegaScout(){
}

CNegaScout::CNegaScout(Task & t):chessBoard(t) {
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
	m_nMaxDepth = m_nSearchDepth = t.searchDepth;
	isBlackPlay = t.isBlackPlay;
}

CNegaScout::CNegaScout(bool isBlackTurn):chessBoard(isBlackTurn) {
}

CNegaScout::CNegaScout(BYTE position[6][6], bool isBlackTurn):chessBoard(position,isBlackTurn) {
}

CNegaScout::CNegaScout(BYTE position[6][6], bool isBlackTurn, CHESSMOVE move):chessBoard(position, isBlackTurn) {
	chessBoard.move(move);
}

CNegaScout::~CNegaScout() {
}

CHESSMOVE CNegaScout::SearchAGoodMove(BYTE position[6][6], bool m_isPlayerBlack) {

	int score;

	chessBoard.setChessPosition(position, m_isPlayerBlack);
	isBlackPlay = m_isPlayerBlack;
	m_nMaxDepth = SEARCH_DEPTH;

/*	目前MinWin的多线程可运行，PVS单线程可运行，并且加入了MAP功能
 *	若要打开多线程功能，在Define.h中取消USE_MULTI_PROCESS的注释
 *	后期想尝试在多线程中加入更新alpha和beta的功能，即先运行5个节点，返回后根据结果生成新任务的alpha和beta

 *	主要参数设置都在Define.h中
 */
//	score = negaScoutMinWin(SEARCH_DEPTH);
//	score = negaScoutAlphaBeta(SEARCH_DEPTH);
	score = negaScoutPVS(SEARCH_DEPTH);

	memcpy(CurPosition, position, sizeof(BYTE) * 36);
	MakeMove(&bestMove);
	memcpy(position, CurPosition, sizeof(BYTE) * 36);

	CString temp;
	temp.Format(
		"走法：(%d,%d)->(%d,%d)\n分数：%d",
		bestMove.From.x, bestMove.From.y, bestMove.To.x, bestMove.To.y, score
	);
	AfxMessageBox(temp);
	return bestMove;
}

void CNegaScout::setPosition(BYTE position[6][6], bool isBlackTurn) {
	chessBoard.setChessPosition(position, isBlackTurn);
}

void CNegaScout::setSearchMethod(int sMethod, bool useMProcess) {
	searchMethod = sMethod;
}

void CNegaScout::move(CHESSMOVE move) {
	chessBoard.move(move);
}

Task CNegaScout::createTask() {
	Task t;
/*		棋盘信息
 *	BYTE position[6][6];
 *	bool isBlackTurn;
 *	ID_TYPE boardId;
 *		控制变量
 *	int searchDepth;
 *	int useMethod;
 *		搜索信息
 *	int depth;
 *	bool isBlackPlay;
 *	int alpha;
 *	int beta;
 */
	chessBoard.getPosition(t.position);
	t.isBlackTurn = chessBoard.getTurn();
	t.isBlackPlay = isBlackPlay;
	t.boardId = chessBoard.getIdRaw();

	return t;
}

bool CNegaScout::useMultiProcess(int depth) {
#ifdef USE_MULTI_PROCESS
	return SEARCH_DEPTH - depth <= PROC_DEPTH;
#else
	return false;
#endif
}

inline int CNegaScout::negaScoutSearch(Task &t) {
	switch (t.useMethod) {
	case PVS:		return negaScoutPVS(t.depth, t.alpha, t.beta);
	case MIN_WIN:	return negaScoutMinWin(t.depth, t.alpha, t.beta);
	case AB_TREE:	return negaScoutAlphaBeta(t.depth, t.alpha, t.beta);
	case MTD_F:		return 0;
	default:		return MIN_VALUE;
	}
	return MIN_VALUE;
}

inline bool CNegaScout::negaScoutPrevTest(int depth, int &value) {
	if (int i = isGameOver()) { value = i; return true; }
	//判断已有更好或等同的评估值
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, value)) { return true; }
	//叶节点直接评估
	if (depth <= 0) {
		value = m_pEval.addBoardValue(
			chessBoard.getId(), 0, m_pEval.evaluate(chessBoard, isBlackPlay)// * (SEARCH_DEPTH % 2 ? -1 : 1)
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

	if (negaScoutPrevTest(depth, best))return best;
	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);

	if (useMultiProcess(depth))
		std::runtime_error("Alpha Beta Multi-Process Undefined!!!");
	/*
	for (int i = 0; i < nextMoveCount; i++) {
        MakeMove(&m_pMG->m_MoveList[depth][i], whoTurn);
        score = -alphabeta(depth - 1, -beta, -alpha);
        UnMakeMove(&m_pMG->m_MoveList[depth][i]);
        if (score>alpha) {
            alpha = score;
            if (depth == m_nMaxDepth){
                m_cmBestMove = m_pMG->m_MoveList[depth][i];
            }
        }
        if (alpha >= beta)break;
    }*/
	for (int i = 0; i < count; i++) {
		chessBoard.move(moveList[i]);
		value = -negaScoutAlphaBeta(depth - 1, -beta, -alpha);
		chessBoard.unMove();

		best = max(best, value);
		if (value > alpha) {
			alpha = value;
			if (depth == m_nMaxDepth)
				bestMove = moveList[i];
			if (alpha >= beta) return beta;
		}
	}
	return best;
}

int CNegaScout::negaScoutMinWin(int depth, int alpha, int beta) {
	CHESSMOVE moveList[200];
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;
	int side = (m_nMaxDepth - depth + isBlackPlay) % 2;//当前层谁走子

	int best = isMax ? MIN_VALUE : MAX_VALUE;//初始化搜索的估值的最值

	if (int i = isGameOver(chessBoard)) return i;//终局
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, best)) return best;
	if (depth <= 0) 
		return m_pEval.addBoardValue(
			chessBoard.getId(), 0, m_pEval.evaluate(chessBoard, isBlackPlay) * (SEARCH_DEPTH % 2 ? -1 : 1)
		);

	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);

	if (useMultiProcess(depth)) {
		vector<future<int>> results;

		for (int i = 0; i < count; i++) {
			chessBoard.move(moveList[i]);

			Task t = createTask();
			t.setCtrlParam(SEARCH_DEPTH, MIN_WIN);
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
			int t = isMax ?
				negaScoutMinWin(depth - 1, best, MAX_VALUE) :
				negaScoutMinWin(depth - 1, MIN_VALUE, best);
			chessBoard.unMove();

			if (isMax) {
				if (best < t) { //获得更大的估值
					alpha = best = t; //更新最值和alpha
					if (depth == m_nMaxDepth) {
						bestMove = moveList[i];
					}
				}
				if (t > beta) return best; //无法使上层变小，剪枝
			}
			else {
				if (best > t) beta = best = t;
				if (t < alpha) return best;
			}
		}
		return m_pEval.addBoardValue(chessBoard.getId(), depth, best);//返回最值
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
	if (negaScoutPrevTest(depth, value)) return value;

	CHESSMOVE moveList[200];
	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);

	if (useMultiProcess(depth)) {
		std::runtime_error("PVS MultiProcess Undefined!!!");
		vector<future<int>> results;

		for (int i = 1; i < count; i++) {
			chessBoard.move(moveList[i]);
			Task t = createTask();
			t.setCtrlParam(SEARCH_DEPTH, PVS);
			t.setSearchParam(depth - 1, isBlackPlay, alpha, beta);
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
			best = max(best, value);
			if (value > alpha) {
				alpha = value;
				if (depth == m_nMaxDepth) bestMove = moveList[i];
				if (alpha >= beta) break;
			}
		}
	}
	
	return m_pEval.addBoardValue(chessBoard.getId(), 0, best);
}

int CNegaScout::isGameOver() {//未结束返回0，结束返回极大/极小值
	if (int type = chessBoard.isGameOver()) {
		int score = MAX_INT - chessBoard.getMoves() - 1;
		return ((type == B_WIN) ^ isBlackPlay) ? -score : score; //异或
	}
	else
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
