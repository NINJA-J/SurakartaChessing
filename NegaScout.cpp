// NegaScout.cpp: implementation of the CNegaScout class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"
#include "NegaScout.h"
#include "Define.h"
#include <atomic>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>

using namespace std;
//多线程参数
mutex mtxReady;
mutex mtx;	//全局互斥锁
condition_variable cv;	//全局条件变量
bool ready = false;
int sk=0;
int runNums = 0;
double smallTreeValue[20];
int waiterNum = 0;
//double otherScore;
int kidTreeValue = -1;
struct ABParam {
	//AB剪枝参数
	ChessBoard chess;
	int depth;
	bool isBlackPlay;
	BV_TYPE alpha;
	BV_TYPE beta;
}ABparam[96];

int k = 0;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define USE_MAP

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////// /////////////////////////////////////////////////
//#define ABDebug
CNegaScout::CNegaScout(int k){
	for (int i = 0; i < 96; i++)
	{
		threads[i] = thread(this->threadMain);
		//threads[i].join();
	}

}
CNegaScout::CNegaScout() {

}

CNegaScout::~CNegaScout() {}

CHESSMOVE CNegaScout::SearchAGoodMove(BYTE position[6][6],bool m_isPlayerBlack) {
	BV_TYPE score;

	chessBoard.setChessPosition(position, m_isPlayerBlack);
	isBlackPlay = m_isPlayerBlack;
	m_nMaxDepth = m_nSearchDepth;
	
	
	//score = Main_NegaScout_ABTree(m_nMaxDepth, m_isPlayerBlack);
	
	//score = NegaScout_ABTree(m_nMaxDepth, m_isPlayerBlack);
	
	score = negaScoutMinWin(chessBoard, m_nMaxDepth, m_isPlayerBlack);
	//score = NegaScout_TT_HH(m_nMaxDepth, 0, m_isPlayerBlack);

	chessBoard.move(bestMove);
	m_pEval.evaluate(chessBoard, m_isPlayerBlack);
	chessBoard.getPosition(position);

	/*CString temp;
	temp.Format(
		"走法：(%d,%d)->(%d,%d)\n分数：%f", 
		bestMove.From.x, bestMove.From.y, bestMove.To.x, bestMove.To.y, score
	);
	AfxMessageBox(temp);*/
	return bestMove;
}

double CNegaScout::NegaScout_TT_HH(int depth,int num,bool isBlackPlay)
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
		if (depth == 3) {
			//	chessBoard.outputPosition();
			CString temp;
			temp.Format(
				"分数%lf，状态%lld,(%d,%d)->(%d,%d)", (double)t, chessBoard.getIdNormal(), mList[i].From.x, mList[i].From.y, mList[i].To.x, mList[i].To.y);
			AfxMessageBox(temp);
		}
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
			NegaScout_ABTree(depth - 1, isBlackPlay, best, MAX_INT) :
			NegaScout_ABTree(depth - 1, isBlackPlay, MIN_INT, best);
		if (depth == 3) {
			//	chessBoard.outputPosition();
			CString temp;
			temp.Format(
				"分数%lf，状态%lld,(%d,%d)->(%d,%d)", (double)t, chessBoard.getIdNormal(), moveList[i].From.x, moveList[i].From.y, moveList[i].To.x, moveList[i].To.y);
			AfxMessageBox(temp);
		}
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


double CNegaScout::negaScoutMinWin(ChessBoard chessBoard, int depth, bool isBlackPlay, BV_TYPE alpha, BV_TYPE beta) {
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
		BV_TYPE t;
		if (isMax)
		{
			t = negaScoutMinWin(chessBoard, depth - 1, isBlackPlay, best, MAX_VALUE);
		}
		else {
			t = negaScoutMinWin(chessBoard, depth - 1, isBlackPlay, MIN_VALUE, best);
		}
		// = isMax ?
		//	negaScoutMinWin(chessBoard, depth - 1, isBlackPlay, best, MAX_VALUE) :
		//	negaScoutMinWin(chessBoard, depth - 1, isBlackPlay, MIN_VALUE, best);
		if (depth == 3) {
			//	chessBoard.outputPosition();
			CString temp;
			temp.Format(
				"子树分数%lf,状态%lld,深度%d,alpha%lf，beta%lf", t, chessBoard.getIdNormal(), depth, alpha, beta);
			AfxMessageBox(temp);
		}
		chessBoard.unMove();
		
		if (isMax) {
			if (best < t) { //获得更大的估值
				alpha = best = t; //更新最值和alpha
				if (depth == m_nMaxDepth)
				{
					bestMove = moveList[i];
				}
			}
			if (best >= beta) return best; //无法使上层变小，剪枝
		}
		else {
			if (best > t) beta = best = t;
			if (best <= alpha) return best;
		}
	}
#ifdef USE_MAP
	m_pEval.addBoardValue(chessBoard.getId(), depth, best);
#endif
	return best;//返回最值
}

double CNegaScout::Main_NegaScout_ABTree(int depth, bool isBlackPlay, BV_TYPE alpha, BV_TYPE beta)
{
	kidTreeValue = -1;
	CHESSMOVE moveList[200];
	bool isMax = (m_nMaxDepth - depth) % 2 == 0;
	int side = (m_nMaxDepth - depth + isBlackPlay) % 2;//当前层谁走子

	BV_TYPE best = isMax ? MIN_VALUE : MAX_VALUE;//初始化搜索的估值的最值

	if (int i = isGameOver()) return i;//终局
#ifdef USE_MAP[]
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
	//double max = 0; 
	//CString temp;
	//temp.Format(
	//	"深度%d",
	//	depth
	//);
	//AfxMessageBox(temp);
	for (int i = 0; i < count; i++) {
		chessBoard.move(moveList[i]);
		
		/*if (depth == 3) {
			//chessBoard.outputPosition();
			CString temp;
			temp.Format(
				"走棋方%d，状态%lld,(%d,%d)->(%d,%d)", isBlackPlay, chessBoard.getIdNormal(), moveList[i].From.x, moveList[i].From.y, moveList[i].To.x, moveList[i].To.y);
			AfxMessageBox(temp);
		}*/
		
	
		//k++;
		BV_TYPE t;
		if (isMax) {
			ABparam[i].chess = chessBoard;
			ABparam[i].beta = MAX_VALUE;
			ABparam[i].isBlackPlay = isBlackPlay;
			ABparam[i].alpha =  best;
			ABparam[i].depth = depth - 1;
	
			cv.notify_one();

		}
		else {
			ABparam[i].chess = chessBoard;
			ABparam[i].beta = best;
			ABparam[i].isBlackPlay = isBlackPlay;
			ABparam[i].alpha = MIN_VALUE;
			ABparam[i].depth = depth - 1;
			cv.notify_one();
		
		}
	
		chessBoard.unMove();
		
	}
#ifdef USE_MAP
	m_pEval.addBoardValue(chessBoard.getId(), depth, best);
#endif
	return best;//返回最值
}


double CNegaScout::threadMain()
{
	double otherScore;
	mutex mtxProcess;
	unique_lock<mutex> lck(mtxProcess);
	ChessBoard chess;
	int depth;
	bool isBlackPlay;
	BV_TYPE alpha;
	BV_TYPE beta;
	CNegaScout *m_pSE;//搜索引擎的指针
	CMoveGenerator *pMG;
	CEvaluation *pEvel;
	m_pSE = new CNegaScout;
	pMG = new CMoveGenerator;
	pEvel = new CEvaluation;
	m_pSE->SetSearchDepth(3);//设定搜索层数
	m_pSE->SetMoveGenerator(pMG);//给搜索引擎设定走法产生器
	m_pSE->SetEveluator(pEvel);//给搜索引擎设定估值核心
	m_pSE->Setm_nMaxDepth(3);
	while (1) {
		cv.wait(lck);
		mtxReady.lock();
		kidTreeValue++;
		chess = ABparam[kidTreeValue].chess;
		depth = ABparam[kidTreeValue].depth;
		isBlackPlay = ABparam[kidTreeValue].isBlackPlay;
		alpha = ABparam[kidTreeValue].alpha;
		beta = ABparam[kidTreeValue].beta;
		mtxReady.unlock();
		//CString temp;
		//temp.Format("子树分数%lld,%d,%d,%d,%d",chess.getId(), ABparam.isBlackPlay, int(ABparam.alpha), int(ABparam.beta), ABparam.depth);
		//AfxMessageBox(temp);
		//Sleep(10000);
		//sk = 0;
		otherScore = m_pSE->negaScoutMinWin(chess,depth,isBlackPlay,alpha, beta);
		
		CString temp;
		temp.Format(
		"子树分数%lf,状态%lld,深度%d,alpha%lf，beta%lf",otherScore,chess.getIdNormal(),depth,alpha,beta);
		AfxMessageBox(temp);
		//k++;

	}
	return 0.0;
}


