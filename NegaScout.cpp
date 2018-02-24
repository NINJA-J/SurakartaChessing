// NegaScout.cpp: implementation of the CNegaScout class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"
#include "NegaScout.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////// /////////////////////////////////////////////////
//#define ABDebug
CNegaScout::CNegaScout() {
//	top=-1;
	flag1=0;
	flag2=0;
	m_pEval = new CEvaluation(chessBoard);
	m_pMG = new CMoveGenerator(chessBoard);
}
int x;
CNegaScout::~CNegaScout()
{

}
CHESSMOVE CNegaScout::SearchAGoodMove(BYTE position[6][6],int m_isPlayerBlack) {
	chessBoard.setChessPosition(position, m_isPlayerBlack);
	isBlackPlay = m_isPlayerBlack;
	m_nMaxDepth = m_nSearchDepth;

	//NegaScout_TT_HH(m_nMaxDepth,0,m_isPlayerBlack);
	//NegaScout_ABTree(m_nMaxDepth, m_isPlayerBlack);
	negaScoutMinWin(m_nMaxDepth, m_isPlayerBlack);
	chessBoard.cMove(bestMove);
	int score;
	m_pEval->getBoardValue(chessBoard.getId(), 0, score);
	chessBoard.getPosition(position);

	//CString temp;
	//temp.Format("走法：%d%d%d%d",m_cmBestMove.From.x,m_cmBestMove.From.y,  m_cmBestMove.To.x, m_cmBestMove.To.y);
	//AfxMessageBox(temp);
	CString temp;
	temp.Format("分数：%d",score);
	AfxMessageBox(temp);
	return bestMove;
}

int CNegaScout::NegaScout_TT_HH(int depth,int num,int m_Type)
{
	int t;
	int Count, i;
	BYTE type;
	int side;
	int score;
	int best = -99999;
	CHESSMOVE mList[200];
	i = isGameOver();//判断胜负
	if (i != 0)
	{
		return i;
	}
	side = (m_nMaxDepth - depth + m_Type) % 2;//判断预测是己方该走还是敌方该走

	
	if (depth <= 0){	//叶子节点取估值
		return m_pEval->evaluate();
	}

	Count = m_pMG->createPossibleMoves(mList,200);//返回下一步多少种走法

	for (i = 0; i < Count; i++)
	{

		chessBoard.move(mList[i]);

		t = -NegaScout_TT_HH(depth - 1, i, m_Type);//递归调用
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

int CNegaScout::NegaScout_ABTree(int depth, int m_Type, int alpha, int beta) {
#ifdef ABDebug
	static const int SIZE = 100;//随机数范围0-100
	static const int MAX_INT = SIZE + 1;//最大值
	static const int MIN_INT = -1;//最小值
	m_nMaxDepth = 3;

	if (depth == m_nMaxDepth) {
		alpha = MIN_INT;
		beta = MAX_INT;
	}
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;//判断当前层是不是max层
	int best = isMax ? MIN_INT : MAX_INT;//初始化搜索的估值的最值

	if (rand() % 100 <= 10) return rand() % SIZE;//模拟游戏结束，概率0.1
	if (depth <= 0) return rand() % SIZE;//到达叶节点，随机返回估值

	int count = 3;//设定每层3个分支

	for (int i = 0; i < count; i++) {
		int t;//µ›πÈµ˜”√
		if (isMax) {
			t = NegaScout_ABTree(depth - 1, m_Type, best, MAX_INT);
			if (best < t) alpha = best = t; //更新最值和alpha
			cout << depth << "\t" << t << "\t" << alpha << "\t" << beta << '\n';

			if (best >= beta) return best; //无法使上层变小，剪枝
		}
		else {
			t = NegaScout_ABTree(depth - 1, m_Type, MIN_INT, best);
			if (best > t) beta = best = t;
			cout << depth << "\t" << t << "\t" << alpha << "\t" << beta << '\n';

			if (best <= alpha) return best;
		}
	}
	return best;//返回最值
#else
	CHESSMOVE moveList[200];

	if (depth == m_nMaxDepth) { //在第一层时自动设定alpha和beta，因为这一层肯定是maxint和minint，减少searchagoodmove里的代码量
		alpha = MIN_INT;
		beta = MAX_INT;
	}
	//0 floor is max floor
	//depth of 0 floor = m_nMaxDepth
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;//判断当前层是不是max层
												//cur floor is black playing -- side = 1
												//cur floor is red   playing -- side = 0
												//0   floor is black playing -- m_Type = 1
												//depth of 0   floor = m_nMaxDepth
												//depth of cur floor = m_nMaxDepth - depth
	int side = (m_nMaxDepth - depth + m_Type) % 2;//当前层谁走子
	int best = isMax ? MIN_INT : MAX_INT;//初始化搜索的估值的最值

	if (int i = isGameOver()) return i;//终局
	if (depth <= 0)
	{
		//CString temp;
		//temp.Format("x:%d", x);
		//AfxMessageBox(temp);
		
		return m_pEval->evaluate();//叶结点
	}

	int count = m_pMG->createPossibleMoves(moveList,200);

	for (int i = 0; i < count; i++) {
		x++;
		chessBoard.move(moveList[i]);//
		int t = isMax ?
			NegaScout_ABTree(depth - 1, m_Type, best, MAX_INT) :
			NegaScout_ABTree(depth - 1, m_Type, MIN_INT, best);
		chessBoard.unMove();

		if (isMax) {
			if (best < t) { //获得更大的估值
				alpha = best = t; //更新最值和alpha
				
				if (depth == m_nMaxDepth){//只有在第一层更新
					bestMove = moveList[i];
				}
					
			}
			if (best >= beta) return best; //无法使上层变小，剪枝
		}
		else {
			if (best > t) {
				beta = best = t;
				if (depth == m_nMaxDepth)
					bestMove = moveList[i];
			}
			if (best <= alpha) return best;
		}
	}
	return best;//返回最值
#endif
}

int CNegaScout::negaScoutMinWin(int depth, int m_Type, int alpha, int beta)
{
	CHESSMOVE moveList[200];
	//0 floor is max floor
	//depth of 0 floor = m_nMaxDepth
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;//判断当前层是不是max层
												//cur floor is black playing -- side = 1
												//cur floor is red   playing -- side = 0
												//0   floor is black playing -- m_Type = 1
												//depth of 0   floor = m_nMaxDepth
												//depth of cur floor = m_nMaxDepth - depth
	int side = (m_nMaxDepth - depth + m_Type) % 2;//当前层谁走子
	int best = isMax ? MIN_INT : MAX_INT;//初始化搜索的估值的最值

	if (int i = isGameOver()) return i;//终局
	if (depth <= 0) return m_pEval->evaluate();//叶结点
	if (m_pEval->getBoardValue(chessBoard.getId(), depth, best)) return best;

	int count = m_pMG->createPossibleMoves(moveList, 200);

	for (int i = 0; i < count; i++) {
		chessBoard.move(moveList[x]);
		int t = isMax ?
			negaScoutMinWin(depth - 1, m_Type, best, MAX_INT) :
			negaScoutMinWin(depth - 1, m_Type, MIN_INT, best);
		chessBoard.unMove();

		if (isMax) {
			if (best < t) { //获得更大的估值
				alpha = best = t; //更新最值和alpha
				if (depth == m_nMaxDepth) bestMove = moveList[x];
			}
			if (best >= beta) return best; //无法使上层变小，剪枝
		}
		else {
			if (best > t) {
				beta = best = t;
				if (depth == m_nMaxDepth) bestMove = moveList[x];
			}
			if (best <= alpha) return best;
		}
	}
	m_pEval->addBoardValue(chessBoard.getId(), depth, best);
	return best;//返回最值
}
