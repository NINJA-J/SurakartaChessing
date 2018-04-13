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
	*	BV_TYPE alpha;			use in negaScout***
	*	BV_TYPE beta;			use in negaScout***
	*/
	m_nMaxDepth = m_nSearchDepth = t.searchDepth;
	isBlackPlay = t.isBlackPlay;
}

CNegaScout::CNegaScout(BYTE position[6][6], bool isBlackTurn):chessBoard(position,isBlackTurn) {
	chessBoard.setChessPosition(position, isBlackTurn);
}

CNegaScout::CNegaScout(BYTE position[6][6], bool isBlackTurn, CHESSMOVE move):chessBoard(position, isBlackTurn) {
	chessBoard.move(move);
}

CNegaScout::~CNegaScout() {
}

CHESSMOVE CNegaScout::SearchAGoodMove(BYTE position[6][6], bool m_isPlayerBlack) {

	BV_TYPE score;

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
		"走法：(%d,%d)->(%d,%d)\n分数：%f",
		bestMove.From.x, bestMove.From.y, bestMove.To.x, bestMove.To.y, score
	);
	AfxMessageBox(temp);
	return bestMove;
}

void CNegaScout::setPosition(BYTE position[6][6], bool isBlackTurn) {
	chessBoard.setChessPosition(position, isBlackTurn);
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
 *	BV_TYPE alpha;
 *	BV_TYPE beta;
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

inline BV_TYPE CNegaScout::negaScoutSearch(Task &t) {
	switch (t.useMethod) {
	case PVS:		return negaScoutPVS(t.depth, t.alpha, t.beta);
	case MIN_WIN:	return negaScoutMinWin(t.depth, t.alpha, t.beta);
	case AB_TREE:	return negaScoutAlphaBeta(t.depth, t.alpha, t.beta);
	case MTD_F:		return 0;
	default:		return MIN_VALUE;
	}
	return MIN_VALUE;
}

inline bool CNegaScout::negaScoutPrevTest(int depth, BV_TYPE &value) {
	if (int i = isGameOver()) { value = i; return true; }
	//判断已有更好或等同的评估值
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, value)) { return true; }
	//叶节点直接评估
	if (depth <= 0) {
		value = m_pEval.addBoardValue(
			chessBoard.getId(), 0, m_pEval.evaluate(chessBoard, isBlackPlay) * (SEARCH_DEPTH % 2 ? -1.0 : 1.0)
		);
		return true;
	}
	return false;
}

BV_TYPE CNegaScout::NegaScout_TT_HH(int depth,int num,bool isBlackPlay)
{
	double t;
	int Count, i;
	int side;
	BV_TYPE best = -99999;
	CHESSMOVE mList[200];
	i = isGameOver();//判断胜负
	if (i != 0)
	{
		return i;
	}
	side = (m_nMaxDepth - depth + isBlackPlay) % 2;//判断预测是己方该走还是敌方该走

	
	if (depth <= 0){	//叶子节点取估值
		return m_pEval.evaluate(chessBoard, isBlackPlay);
	}

	Count = m_pMG.createPossibleMoves(chessBoard,mList,200);//返回下一步多少种走法

	for (i = 0; i < Count; i++)
	{

		chessBoard.move(mList[i]);

		t = -NegaScout_TT_HH(depth - 1, i, isBlackPlay);//递归调用
		if (t > best)
		{
			best = t;
			if(depth == m_nMaxDepth){
				bestMove = mList[i];//存储最优走法
			}
			

		}
		chessBoard.unMove();//还原棋盘
	}
	
	return best;
}

BV_TYPE CNegaScout::negaScoutAlphaBeta(int depth, BV_TYPE alpha, BV_TYPE beta) {
	CHESSMOVE moveList[200];
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;
	int side = (m_nMaxDepth - depth + isBlackPlay) % 2;//当前层谁走子
#ifdef USE_NEW_ABTREE
	BV_TYPE best = MIN_VALUE;
#else
	BV_TYPE best = isMax ? MIN_VALUE : MAX_VALUE;//初始化搜索的估值的最值
#endif

	if (int i = isGameOver()) return i;//终局
	if (depth <= 0)
#ifdef USE_NEW_ABTREE
		return -m_pEval.evaluate(chessBoard, isBlackPlay);
#else
		return m_pEval.evaluate(chessBoard, isBlackPlay);//叶结点
#endif
	if (useMultiProcess(depth))
		std::runtime_error("Alpha Beta Multi-Process Undefined!!!");

	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);
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
#ifdef USE_NEW_ABTREE
		chessBoard.move(moveList[i]);
		BV_TYPE score = -negaScoutAlphaBeta(depth - 1, -beta, -alpha);
		chessBoard.unMove();

		if (score > alpha) {
			alpha = score;
			if (depth == m_nMaxDepth)
				bestMove = moveList[i];
		}
		if (alpha >= beta) return beta;
#else
		chessBoard.move(moveList[i]);//
		BV_TYPE t = isMax ?
			negaScoutAlphaBeta(depth - 1, best, MAX_INT) :
			negaScoutAlphaBeta(depth - 1, MIN_INT, best);
		chessBoard.unMove();

		if (isMax) {
			if (best < t) { //获得更大的估值
				alpha = best = t; //更新最值和alpha
				if (depth == m_nMaxDepth) bestMove = moveList[i];
			}
			if (best >= beta) return best; //无法使上层变小，剪枝
		}
		else {
			if (best > t) beta = best = t;
			if (best <= alpha) return best;
		}
#endif
	}
#ifdef USE_NEW_ABTREE
	return alpha;
#else
	return best;//返回最值
#endif
}

BV_TYPE CNegaScout::negaScoutMinWin(int depth, BV_TYPE alpha, BV_TYPE beta) {
	CHESSMOVE moveList[200];
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;
	int side = (m_nMaxDepth - depth + isBlackPlay) % 2;//当前层谁走子

	BV_TYPE best = isMax ? MIN_VALUE : MAX_VALUE;//初始化搜索的估值的最值

	if (int i = isGameOver(chessBoard)) return i;//终局
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, best)) return best;
	if (depth <= 0) 
		return m_pEval.addBoardValue(
			chessBoard.getId(), 0, m_pEval.evaluate(chessBoard, isBlackPlay) * (SEARCH_DEPTH % 2 ? -1.0 : 1.0)
		);

	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);

	if (useMultiProcess(depth)) {
		vector<future<BV_TYPE>> results;

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

		BV_TYPE max = MIN_VALUE;
		for (int i = 0; i < count; i++) {
			BV_TYPE t = results[i].get();
			if (t > max) {
				max = t;
				bestMove = moveList[i];
			}
		}
		return max;
	} else {
		for (int i = 0; i < count; i++) {
			chessBoard.move(moveList[i]);
			BV_TYPE t = isMax ?
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
		//#ifdef USE_MAP
		//		m_pEval.addBoardValue(chessBoard.getId(), depth, best);
		//#endif
		return best;//返回最值
	}
}

BV_TYPE CNegaScout::negaScoutPVS(int depth, BV_TYPE alpha, BV_TYPE beta) {
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
	CHESSMOVE moveList[200];
	BV_TYPE value, delta = m_pEval.getValueDelta();

	if (negaScoutPrevTest(depth, value)) return value;

	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);

	chessBoard.move(moveList[0]);
	value = -negaScoutPVS(depth - 1, -beta, -alpha);
	chessBoard.unMove();

	if (value > alpha) {
		alpha = value;
		if (depth == m_nMaxDepth) bestMove = moveList[0];
	}
	if (alpha >= beta) return beta;

	if (useMultiProcess(depth)) {
		std::runtime_error("PVS MultiProcess Undefined!!!");
		vector<future<BV_TYPE>> results;

		for (int i = 1; i < count; i++) {
			chessBoard.move(moveList[i]);
			Task t = createTask();
			t.setCtrlParam(SEARCH_DEPTH, PVS);
			t.setSearchParam(depth - 1, isBlackPlay, alpha, beta);
		}
	} else {
		//后面的节点在之前的基础上搜索
		for (int i = 1; i < count; i++) {
			chessBoard.move(moveList[i]);
			//这个负号是用的网上alpha beta剪枝的写法，好像大部分代码也这么写
			value = -negaScoutPVS(depth - 1, -alpha - delta, -alpha);
			if (alpha < value && value < beta)
				value = -negaScoutPVS(depth - 1, -beta, -value);

			chessBoard.unMove();
			//更新alpha和beta并判断剪枝
			if (value > alpha) {
				alpha = value;
				if (depth == m_nMaxDepth) bestMove = moveList[i];
				if (alpha >= beta) return beta;
			}

		}
	}
	
	return m_pEval.addBoardValue(chessBoard.getId(), 0, alpha);
}

BV_TYPE CNegaScout::isGameOver() {//未结束返回0，结束返回极大/极小值
	if (int type = chessBoard.isGameOver()) {
		int score = MAX_INT - chessBoard.getMoves() - 1;
		return ((type == B_WIN) ^ isBlackPlay) ? -score : score; //异或
	}
	else
		return 0;
}

BV_TYPE CNegaScout::isGameOver(ChessBoard chessBoard) {//未结束返回0，结束返回极大/极小值
	if (int type = chessBoard.isGameOver()) {
		int score = MAX_VALUE - chessBoard.getMoves() - 1;
		return ((type == B_WIN) ^ isBlackPlay) ? -score : score; //异或
	}
	else
		return 0;
}
