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

#define USE_MULTI_PROCESS

struct ABParam {
	//AB剪枝参数
	BYTE position[6][6];
	bool isBlackTurn;
	bool isBlackPlay;
	int depth;
	BV_TYPE alpha;
	BV_TYPE beta;
	struct ABParam() {};
	struct ABParam(ABParam &param) {
		memcpy(position, param.position, sizeof(BYTE) * 36);
		isBlackTurn = param.isBlackTurn;
		isBlackPlay = param.isBlackPlay;
		depth = param.depth;
		alpha = param.alpha;
		beta = param.beta;
	}
	struct ABParam(ChessBoard board, bool _isBlackPlay, int _depth, BV_TYPE _alpha, BV_TYPE _beta) {
		board.getPosition(position);
		isBlackTurn = board.getTurn();
		isBlackPlay = _isBlackPlay;
		depth = _depth;
		alpha = _alpha;
		beta = _beta;
	}
}ABparam[96];

mutex paramMutex;
queue<ABParam> paramReady;
int  procGet = 0;
condition_variable procGetParam;

mutex scoreSaveMutex;
queue<BV_TYPE> scoreReturned;

mutex threadInitMutex;
bool threadInited = false;
thread threads[96];

mutex mtxReady;
mutex mtx;	//全局互斥锁
condition_variable cv;	//全局条件变量
bool ready = false;
int sk = 0;
int runNums = 0;
double smallTreeValue[20];
int waiterNum = 0;

bool CNegaScout::threadInited = false;
thread CNegaScout::threads[96];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////// /////////////////////////////////////////////////
//#define ABDebug

/*CNegaScout::CNegaScout(int k) {
	for (int i = 0; i < 96; i++)
	{
		threads[i] = thread(this->threadMain);
		//threads[i].join();
	}
}*/

CNegaScout::CNegaScout(){
	unique_lock<mutex> threadInitLock(threadInitMutex);
	if (!threadInited) {
		initThread();
		threadInited = true;
	}
}

CNegaScout::~CNegaScout() {
	unique_lock<mutex> threadInitLock(threadInitMutex);
	if (threadInited) {
		deleteThread();
		threadInited = false;
	}
}

CHESSMOVE CNegaScout::SearchAGoodMove(BYTE position[6][6], bool m_isPlayerBlack) {
#ifndef USE_MULTI_PROCESS
	BV_TYPE score;

	chessBoard.setChessPosition(position, m_isPlayerBlack);
	isBlackPlay = m_isPlayerBlack;
	m_nMaxDepth = m_nSearchDepth;

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
#else
	BV_TYPE score;

	chessBoard.setChessPosition(position, m_isPlayerBlack);
	isBlackPlay = m_isPlayerBlack;
	m_nMaxDepth = m_nSearchDepth;

	score = negaScoutMinWinProc(m_nMaxDepth, m_isPlayerBlack);
	//score = negaScoutMinWin(chessBoard, m_nMaxDepth, m_isPlayerBlack);

	chessBoard.move(bestMove);
	m_pEval.evaluate(chessBoard, m_isPlayerBlack);
	chessBoard.getPosition(position);
	return bestMove;
#endif
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
		BV_TYPE t = isMax ?
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
		if (isMax) {
			t = negaScoutMinWin(chessBoard, depth - 1, isBlackPlay, best, MAX_VALUE);
		} else {
			t = negaScoutMinWin(chessBoard, depth - 1, isBlackPlay, MIN_VALUE, best);
		}
	/*	if (depth == 3) {
			//	chessBoard.outputPosition();
			CString temp;
			temp.Format(
				"子树分数%lf,状态%lld,深度%d,alpha%lf，beta%lf", t, chessBoard.getIdNormal(), depth, alpha, beta);
			AfxMessageBox(temp);
		}*/
		chessBoard.unMove();

		if (isMax) {
			if (best < t) { //获得更大的估值
				alpha = best = t; //更新最值和alpha
				if (depth == m_nMaxDepth) {
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

double CNegaScout::negaScoutMinWinProc(int depth, bool isBlackPlay, BV_TYPE alpha, BV_TYPE beta) {
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

	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);
	for (int i = 0; i < count; i++) {
		//走子
		chessBoard.move(moveList[i]);
		//进入<分配任务>临界区加锁
		unique_lock<mutex> paramLock(paramMutex);
		//<分配任务>
		AfxMessageBox("分配任务");
		paramReady.push(
			ABParam(chessBoard, isBlackPlay, depth - 1,
				isMax ? best : MIN_VALUE,
				isMax ? MAX_VALUE : best
			)
		);
		//退出<分配任务>临界区去锁
		paramLock.unlock();
		//唤醒线程
		procGetParam.notify_one();
		//撤销走子
		chessBoard.unMove();
	}
	CString temp;
	temp.Format("分配结束：%d->%d", count, procGet);
	AfxMessageBox(temp);
	//等待返回值
	while (scoreReturned.size() != count);
	AfxMessageBox("所有线程结束");
	//处理返回值
	while (scoreReturned.size()) {
		BV_TYPE t = scoreReturned.front();
		scoreReturned.pop();
		if (isMax) {
			if (best < t) alpha = best = t; //更新最值和alpha
			if (best >= beta) break; //无法使上层变小，剪枝
		} else {
			if (best > t) beta = best = t;
			if (best <= alpha) break;
		}
	}
	AfxMessageBox("剪枝结束");
	//清空队列
	while (scoreReturned.size())scoreReturned.pop();

#ifdef USE_MAP
	m_pEval.addBoardValue(chessBoard.getId(), depth, best);
#endif
	return best;//返回最值
}

double CNegaScout::threadMain(int id) {
	CNegaScout negaScout;//搜索引擎的指针
	negaScout.SetSearchDepth(3);//设定搜索层数

	double otherScore;
	while (1) {
		unique_lock<mutex> paramLock(paramMutex);
		procGetParam.wait(paramLock);
		CString temp;
		temp.Format("%d线程接收任务", id);
		procGet++;
		AfxMessageBox(temp);
		if (!paramReady.size())break;
		ABParam param = paramReady.front();
		paramReady.pop();
		paramLock.unlock();

		ChessBoard chessBoard(param.position, param.isBlackTurn);
		otherScore = negaScout.negaScoutMinWin(
			chessBoard, 
			param.depth, 
			param.isBlackPlay, 
			param.alpha, 
			param.beta
		);
		unique_lock<mutex> scoreLock(scoreSaveMutex);
		scoreReturned.push(otherScore);
	}
	return 0.0;
}

void CNegaScout::initThread() {
	for (int i = 0; i < 96; i++) {
		threads[i] = thread(threadMain,i);
	};
}

void CNegaScout::deleteThread() {
	procGetParam.notify_all();//无任务时线程唤醒自动退出
	for (int i = 0; i < 96; i++) {
		threads[i].join();
	}
}
