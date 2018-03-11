// NegaScout.cpp: implementation of the CNegaScout class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"
#include "NegaScout.h"
#include "Define.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////// /////////////////////////////////////////////////
//#define ABDebug
CNegaScout::CNegaScout(){
}

CNegaScout::~CNegaScout() {}

CHESSMOVE CNegaScout::SearchAGoodMove(BYTE position[6][6],bool m_isPlayerBlack) {
	BV_TYPE score;

	chessBoard.setChessPosition(position, m_isPlayerBlack);
	isBlackPlay = m_isPlayerBlack;
	m_nMaxDepth = m_nSearchDepth;

	//NegaScout_TT_HH(m_nMaxDepth,0,m_isPlayerBlack);
	//NegaScout_ABTree(m_nMaxDepth, m_isPlayerBlack);
	score = negaScoutMinWin(m_nMaxDepth, m_isPlayerBlack);

	chessBoard.move(bestMove);
	chessBoard.getPosition(position);

	CString temp;
	temp.Format(
		"走法：(%d,%d)->(%d,%d)\n分数：%f", 
		bestMove.From.x, bestMove.From.y, bestMove.To.x, bestMove.To.y, score
	);
	AfxMessageBox(temp);
	return bestMove;
}

double CNegaScout::NegaScout_TT_HH(int depth,int num,bool isBlackPlay)
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

double CNegaScout::NegaScout_ABTree(int depth, bool isBlackPlay, BV_TYPE alpha, BV_TYPE beta) {
	CHESSMOVE moveList[200];
	//0 floor is max floor
	//depth of 0 floor = m_nMaxDepth
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;//判断当前层是不是max层
												//cur floor is black playing -- side = 1
												//cur floor is red   playing -- side = 0
												//0   floor is black playing -- m_Type = 1
												//depth of 0   floor = m_nMaxDepth
												//depth of cur floor = m_nMaxDepth - depth
	int side = (m_nMaxDepth - depth + isBlackPlay) % 2;//当前层谁走子
	BV_TYPE best = isMax ? MIN_VALUE : MAX_VALUE;//初始化搜索的估值的最值

	if (int i = isGameOver()) return i;//终局
	if (depth <= 0) return m_pEval.evaluate(chessBoard, isBlackPlay);//叶结点

	int count = m_pMG.createPossibleMoves(chessBoard,moveList,200);

	for (int i = 0; i < count; i++) {
		chessBoard.move(moveList[i]);//
		int t = isMax ?
			NegaScout_ABTree(depth - 1, isBlackPlay, best, MAX_INT) :
			NegaScout_ABTree(depth - 1, isBlackPlay, MIN_INT, best);
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
	}
	return best;//返回最值
}

double CNegaScout::negaScoutMinWin(int depth, bool isBlackPlay, BV_TYPE alpha, BV_TYPE beta) {
	CHESSMOVE moveList[200];
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;
	int side = (m_nMaxDepth - depth + isBlackPlay) % 2;//当前层谁走子

	BV_TYPE best = isMax ? MIN_VALUE : MAX_VALUE;//初始化搜索的估值的最值

	if (int i = isGameOver()) return i;//终局
	if (depth <= 0) return m_pEval.evaluate(chessBoard, isBlackPlay);//叶结点
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, best)) return best;

	int count = m_pMG.createPossibleMoves(chessBoard,moveList, 200);
	for (int i = 0; i < count; i++) {
		chessBoard.move(moveList[i]);
		BV_TYPE t = isMax ?
			negaScoutMinWin(depth - 1, isBlackPlay, best, MAX_INT) :
			negaScoutMinWin(depth - 1, isBlackPlay, MIN_INT, best);
		chessBoard.unMove();

		if (isMax) {
			if (best < t) { //获得更大的估值
				alpha = best = t; //更新最值和alpha
				if (depth == m_nMaxDepth) bestMove = moveList[i];
			}
			if (best >= beta) return best; //无法使上层变小，剪枝
		} else {
			if (best > t) beta = best = t;
			if (best <= alpha) return best;
		}
	}
	m_pEval.addBoardValue(chessBoard.getId(), depth, best);
	return best;//返回最值
}
