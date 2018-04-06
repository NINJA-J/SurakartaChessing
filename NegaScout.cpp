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
	int moveId;
	struct ABParam() {};
	struct ABParam(ABParam &param) {
		memcpy(position, param.position, sizeof(BYTE) * 36);
		isBlackTurn = param.isBlackTurn;
		isBlackPlay = param.isBlackPlay;
		depth = param.depth;
		alpha = param.alpha;
		beta = param.beta;
		moveId = param.moveId;
	}
	struct ABParam(ChessBoard board, bool _isBlackPlay, int _depth, BV_TYPE _alpha, BV_TYPE _beta,int _moveId) {
		board.getPosition(position);
		isBlackTurn = board.getTurn();
		isBlackPlay = _isBlackPlay;
		depth = _depth;
		alpha = _alpha;
		beta = _beta;
		moveId = _moveId;
	}
}ABparam[96];

mutex paramMutex;
queue<ABParam> paramReady;
int  procGet = 0;
condition_variable procGetParam;

mutex scoreMutex;
condition_variable scoreSaveCV;
condition_variable scoreReadCV;
queue<BV_TYPE> scoreReturned;
BV_TYPE score;
int moveId;

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

/* 在构造函数中加入输出语句后发现程序每一帧都会析构NegaScout和创建新的NegaScout，因此线程池的创建采用单次创建方式
 * 用静态的bool变量存储是否创建过线程池，析构时判断若创建过则进行析构操作
 * 线程的中止方式可见threadMain函数，采用无任务自动跳出循环的方式，即将任务清空（析构是一般都是这样）后notify_all
 */
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
/* 加入了USE_MULTI_PROCESS（使用多线程）宏定义，保留原来的非多线程方法

 */
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
		chessBoard.unMove();

		if (isMax) {
			if (t > beta) return best; //无法使上层变小，剪枝
			if (best < t) { //获得更大的估值
				alpha = best = t; //更新最值和alpha
				if (depth == m_nMaxDepth) {
					bestMove = moveList[i];
				}
			}
		}
		else {
			if (t < alpha) return best;
			if (best > t) beta = best = t;
		}
	}
#ifdef USE_MAP
	m_pEval.addBoardValue(chessBoard.getId(), depth, best);
#endif
	return best;//返回最值
}

double CNegaScout::negaScoutMinWinProc(int depth, bool isBlackPlay, BV_TYPE alpha, BV_TYPE beta) {
	CHESSMOVE moveList[200];
#ifdef USE_MAP[]
	if (m_pEval.getBoardValue(chessBoard.getId(), depth, best)) {
		if (depth == m_nMaxDepth) {
			best;
		}
		return best;
	}
#endif

	int count = m_pMG.createPossibleMoves(chessBoard, moveList, 200);
	//获取变量操作锁
	unique_lock<mutex> paramLock(paramMutex);
	
	for (int i = 0; i < count; i++) {
		//走子
		chessBoard.move(moveList[i]);
		//分配任务
		paramReady.push(
			ABParam(
				chessBoard, isBlackPlay, depth - 1,
				MIN_VALUE, MAX_VALUE, i
			)
		);
		//撤子
		chessBoard.unMove();
	}
	//唤醒一个线程
	paramLock.unlock();
	procGetParam.notify_one();
	//开始等待线程返回结果
	unique_lock<mutex> scoreLock(scoreMutex);
	//初始化结果返回数量0
	int scoreRtn = 0;
	//初始化最佳值
	BV_TYPE best = MIN_VALUE;
	//若所有线程未全部返回值，继续循环
	while (scoreRtn < count) {
		//等待赋值操作锁
		/* 问题：这里有可能发生死锁问题，猜想原因是分配任务是丢失任务或者子线程死循环导致返回值数量不正确
		 * 暂时还未想出测试方案，而且使用多线程后虽然速度方面没有大碍，但是落子后有明显卡顿，还不知道原因
		 */
		scoreReadCV.wait(scoreLock);
		//返回数量加一
		scoreRtn++;
		//获取最佳走法
		if (score > best) {
			best = score;
			bestMove = moveList[moveId];
		}
	}

#ifdef USE_MAP
	m_pEval.addBoardValue(chessBoard.getId(), depth, best);
#endif
	return best;//返回最值
}

double CNegaScout::threadMain(int id) {
	CNegaScout negaScout;//搜索引擎的指针
	/* 可参考SearchEngine.h文件，我将evaluation和movegenerator
	 * 都设置为私有变量而不是指针，故不再需要后面的两个函数
	 */
	negaScout.SetSearchDepth(6);//设定搜索层数

	unique_lock<mutex> paramLock(paramMutex);

	BV_TYPE otherScore;
	while (1) {
		//获取锁，判断有任务则执行，无任务则退出线程
		procGetParam.wait(paramLock);
		if (!paramReady.size()) break;
		//获取任务
		ABParam param = paramReady.front();
		paramReady.pop();
		//若还有任务，唤醒下一个线程
		if (paramReady.size()) 
			procGetParam.notify_one();
		//初始化chessBoard
		ChessBoard chessBoard(param.position, param.isBlackTurn);
		//运行剪枝
		otherScore = negaScout.negaScoutMinWin(
			chessBoard, 
			param.depth, 
			param.isBlackPlay, 
			param.alpha, 
			param.beta
		);
		//准备返回评估值，采用一次返回一个值的方式，可修改，我采用这个方法
		unique_lock<mutex> scoreLock(scoreMutex);
		//存储值，两种思路，
		//用优先队列存储多个值，问题是不好判断值对应的走棋，可尝试返回结构体数据
		//用两个全局变量，存储评估值和走棋id（对应moveList的下标）
		scoreReturned.push(otherScore);
		score = otherScore;
		moveId = param.moveId;
		//唤醒主线程读取结果
		scoreReadCV.notify_one();
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
