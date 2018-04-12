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

//#define USE_MULTI_PROCESS

#define THREAD_N 96
#define SEARCH_DEPTH 6

#define USE_NEW_ABTREE

ThreadPool threadPool(THREAD_N);

struct Task {
	//AB剪枝参数
	BYTE position[6][6];
	bool isBlackTurn;
	bool isBlackPlay;
	int searchDepth;
	BV_TYPE alpha;
	BV_TYPE beta;
	ID_TYPE boardId;
	int useMethod;

	struct Task() {};
	struct Task(Task &param) {
		memcpy(position, param.position, sizeof(BYTE) * 36);
		isBlackTurn = param.isBlackTurn;
		isBlackPlay = param.isBlackPlay;
		searchDepth = param.searchDepth;
		alpha = param.alpha;
		beta = param.beta;
		boardId = param.boardId;
		useMethod = param.useMethod;
	}
	struct Task(ChessBoard board, bool _isBlackPlay, int _depth, BV_TYPE _alpha, BV_TYPE _beta,ID_TYPE _boardId,
		int _useMethod) {
		board.getPosition(position);
		isBlackTurn = board.getTurn();
		isBlackPlay = _isBlackPlay;
		searchDepth = _depth;
		alpha = _alpha;
		beta = _beta;
		boardId = _boardId;
		useMethod = _useMethod;
	}
}Task[96];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////// /////////////////////////////////////////////////

/* 在构造函数中加入输出语句后发现程序每一帧都会析构NegaScout和创建新的NegaScout，因此线程池的创建采用单次创建方式
 * 用静态的bool变量存储是否创建过线程池，析构时判断若创建过则进行析构操作
 * 线程的中止方式可见threadMain函数，采用无任务自动跳出循环的方式，即将任务清空（析构是一般都是这样）后notify_all
 */
CNegaScout::CNegaScout(){
}

CNegaScout::CNegaScout(BYTE position[6][6], bool isBlackTurn) {
	chessBoard.setChessPosition(position, isBlackTurn);
}

CNegaScout::CNegaScout(BYTE position[6][6], bool isBlackTurn, CHESSMOVE move) {
	chessBoard.setChessPosition(position, isBlackTurn);
	chessBoard.move(move);
}

CNegaScout::~CNegaScout() {
}
CHESSMOVE CNegaScout::SearchAGoodMove(BYTE position[6][6], bool m_isPlayerBlack) {

	BV_TYPE score;

	chessBoard.setChessPosition(position, m_isPlayerBlack);
	isBlackPlay = m_isPlayerBlack;
	m_nMaxDepth = SEARCH_DEPTH;

#ifdef USE_MULTI_PROCESS
	score = negaScoutMinWinProc(SEARCH_DEPTH, m_isPlayerBlack);
#else
	//score = negaScoutMinWin(SEARCH_DEPTH, m_isPlayerBlack);
	//score = NegaScout_ABTree(SEARCH_DEPTH, m_isPlayerBlack);
	score = negaScoutPVS(SEARCH_DEPTH);
#endif

	memcpy(CurPosition, position, sizeof(BYTE) * 36);
	MakeMove(&bestMove);
	memcpy(position, CurPosition, sizeof(BYTE) * 36);
//	chessBoard.move(bestMove);
//	chessBoard.getPosition(position);

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

bool CNegaScout::useMultiProcess(int depth) {
	return m_nSearchDepth - depth <= PROC_DEPTH;
}

BV_TYPE CNegaScout::NegaScout_TT_HH(int depth,int num,bool isBlackPlay)
{
	double t;
	int Count, i;
	int side;
	int best = -99999;
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

BV_TYPE CNegaScout::NegaScout_ABTree(int depth, BV_TYPE alpha, BV_TYPE beta) {
	CHESSMOVE moveList[200];
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;
	int side = (m_nMaxDepth - depth + isBlackPlay) % 2;//当前层谁走子
#ifdef USE_NEW_ABTREE
	BV_TYPE best = MIN_VALUE;
#else
	BV_TYPE best = isMax ? MIN_VALUE : MAX_VALUE;//初始化搜索的估值的最值
#endif

	if (int i = isGameOver()) return i;//终局
	if (depth <= 0) return m_pEval.evaluate(chessBoard, isBlackPlay);//叶结点

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
		BV_TYPE score = -NegaScout_ABTree(depth - 1, -beta, -alpha);
		chessBoard.unMove();

		if (score > best) {
			best = score;
			if (depth == m_nMaxDepth)
				bestMove = moveList[i];
		}

		if (best > alpha) alpha = best;
		if (alpha >= beta) return beta;
#else
		chessBoard.move(moveList[i]);//
		BV_TYPE t = isMax ?
			NegaScout_ABTree(depth - 1, best, MAX_INT) :
			NegaScout_ABTree(depth - 1, MIN_INT, best);
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

BV_TYPE CNegaScout::negaScoutMinWin(int depth, bool isBlackPlay, BV_TYPE alpha, BV_TYPE beta) {
	CHESSMOVE moveList[200];
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;
	int side = (m_nMaxDepth - depth + isBlackPlay) % 2;//当前层谁走子

	BV_TYPE best = isMax ? MIN_VALUE : MAX_VALUE;//初始化搜索的估值的最值

	if (int i = isGameOver(chessBoard)) return i;//终局
#ifdef USE_MAP
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, best)) {
		if (depth == m_nMaxDepth) {
			best;
		}
		return best;
	}
#endif
	if (depth <= 0) {
		BV_TYPE value = m_pEval.evaluate(chessBoard, isBlackPlay);//叶结点
#ifdef USE_MAP
		m_pEval.addBoardValue(chessBoard.getId(), 0, value);
#endif
		return value;
	}

	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);

	for (int i = 0; i < count; i++) {
		chessBoard.move(moveList[i]);
		BV_TYPE t = isMax ?
			negaScoutMinWin(depth - 1, isBlackPlay, best, MAX_VALUE) :
			negaScoutMinWin(depth - 1, isBlackPlay, MIN_VALUE, best);
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
#ifdef USE_MAP
	m_pEval.addBoardValue(chessBoard.getId(), depth, best);
#endif
	return best;//返回最值
}

BV_TYPE CNegaScout::negaScoutMinWinProc(int depth, bool isBlackPlay, BV_TYPE alpha, BV_TYPE beta) {
	CHESSMOVE moveList[200];
#ifdef USE_MAP
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, best)) {
		if (depth == m_nMaxDepth) {
			best;
		}
		return best;
	}
#endif
	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);

	vector<future<BV_TYPE>> results;
	BYTE position[6][6];
	bool isBlackTurn = isBlackPlay;
	chessBoard.getPosition(position);

	for (int i = 0; i < count; i++) {
		results.emplace_back(
			threadPool.enqueue([i](
				BYTE position[6][6],CHESSMOVE move, 
				bool isBlackTurn, bool isBlackPlay, 
				int depth, BV_TYPE alpha, BV_TYPE beta
				) {
			CNegaScout negaScout(position, isBlackTurn, move);
			negaScout.SetSearchDepth(SEARCH_DEPTH);
			return negaScout.negaScoutMinWin(depth, isBlackPlay, MIN_VALUE, MAX_VALUE);
		}, position, moveList[i], isBlackTurn, isBlackPlay, SEARCH_DEPTH - 1, alpha, beta));
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
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;
	int side = (m_nMaxDepth - depth + isBlackPlay) % 2;//当前层谁走子

	BV_TYPE value = isMax ? MIN_VALUE : MAX_VALUE;//初始化搜索的估值的最值

	if (int i = isGameOver(chessBoard)) return i;//终局
#ifdef USE_MAP
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, best)) {
		if (depth == m_nMaxDepth) {
			best;
		}
		return best;
	}
#endif
	if (depth <= 0) {
		BV_TYPE value = m_pEval.evaluate(chessBoard, isBlackPlay);//叶结点
#ifdef USE_MAP
		m_pEval.addBoardValue(chessBoard.getId(), 0, value);
#endif
		return value;
	}

	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);
	
	if (isMax) {
		for (int i = 0; i < count; i++) {
			chessBoard.move(moveList[i]);

			value = i ?
				negaScoutPVS(depth - 1, alpha, alpha + 1) :
				negaScoutPVS(depth - 1, alpha, beta);
			if (alpha <value && value < beta)
				value = negaScoutPVS(depth - 1, alpha, beta);

			chessBoard.unMove();
			if (value > alpha) {
				alpha = value;
				if (alpha >= beta)
					break;
				if (depth == m_nMaxDepth)
					bestMove = moveList[i];
			}
		}
		return alpha;
	} else {
		for (int i = 0; i < count; i++) {
			chessBoard.move(moveList[i]);

			value = i ?
				negaScoutPVS(depth - 1, beta - 1, beta) :
				negaScoutPVS(depth - 1, alpha, beta);
			if (alpha <value && value < beta)
				value = negaScoutPVS(depth - 1, alpha, beta);

			chessBoard.unMove();
			if (value < beta) beta = value;
			if (alpha >= beta)break;
		}
		return alpha;
	}
	return 0.0;
}
