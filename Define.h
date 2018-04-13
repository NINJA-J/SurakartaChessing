// Define.h: interface for the Define class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFINE_H__315ABE2D_6AB8_4DBC_AE20_5EA359AC5B00__INCLUDED_)
#define AFX_DEFINE_H__315ABE2D_6AB8_4DBC_AE20_5EA359AC5B00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/********** 棋盘参数 **********/
#define NOCHESS 0
#define BLACK   1
#define RED     2

#define B_PLAYING true
#define R_PLAYING false

#define B_WIN 1
#define R_WIN 2
#define IsBlack(x) (x==1)
#define IsRed(x) (x==2)
#define IsSameSide(x,y) ((IsBlack(x)&&IsBlack(y))||(IsRed(x)&&IsRed(y)))

/********** Evaluation参数 **********/
#define USE_MAP
#define USE_INT_AS_BOARD_VALUE

#define MAX_INT 0x7fffff00
#define MIN_INT (-MAX_INT)

#define MAX_DOUBLE 100000.0
#define MIN_DOUBLE (-MAX_DOUBLE)

#ifdef USE_INT_AS_BOARD_VALUE
typedef	int BV_TYPE;
#define MAX_VALUE (MAX_INT)
#define MIN_VALUE (MIN_INT)
#else
typedef double BV_TYPE;
#define MAX_VALUE (MAX_DOUBLE)
#define MIN_VALUE (MIN_DOUBLE)
#endif

typedef long long int ID_TYPE;

/********** NegaScout参数 **********/
//#define USE_MULTI_PROCESS
#define USE_NEW_ABTREE

#define PROC_DEPTH 1
#define SEARCH_DEPTH 8

#define AB_TREE 0
#define PVS 1
#define MTD_F 2
#define MAX_MIN 3
#define MIN_WIN 4

#define THREAD_N 96

/********** 其他参数 **********/

const int arcLoop[2][24][2] = {
	{   //内弧
		{ 1,0 },{ 1,1 },{ 1,2 },{ 1,3 },{ 1,4 },{ 1,5 },
		{ 0,4 },{ 1,4 },{ 2,4 },{ 3,4 },{ 4,4 },{ 5,4 },
		{ 4,5 },{ 4,4 },{ 4,3 },{ 4,2 },{ 4,1 },{ 4,0 },
		{ 5,1 },{ 4,1 },{ 3,1 },{ 2,1 },{ 1,1 },{ 0,1 }
	},{ //外弧
		{ 2,0 },{ 2,1 },{ 2,2 },{ 2,3 },{ 2,4 },{ 2,5 },
		{ 0,3 },{ 1,3 },{ 2,3 },{ 3,3 },{ 4,3 },{ 5,3 },
		{ 3,5 },{ 3,4 },{ 3,3 },{ 3,2 },{ 3,1 },{ 3,0 },
		{ 5,2 },{ 4,2 },{ 3,2 },{ 2,2 },{ 1,2 },{ 0,2 }
	}
};

typedef struct _chessmanposition
{
	BYTE x;
	BYTE y;
	struct _chessmanposition() {};
	struct _chessmanposition(BYTE _x, BYTE _y) :x(_x), y(_y) {};
	//BYTE arc;//不需要，可以由x、y的坐标从棋盘上判断是否挨着弧
}CHESSNAMPOS;

typedef struct _chessmove
{
	BYTE Side;
	CHESSNAMPOS From;
	CHESSNAMPOS To;
	int Score;
	bool isMove;//是不是走子着法
	bool operator==(const _chessmove & move) {
		return (From.x == move.From.x && From.y == move.From.y && To.x == move.To.x && To.y == move.To.y);
	}

	struct _chessmove() {};
	struct _chessmove(BYTE xf, BYTE yf, BYTE xt, BYTE yt, BYTE side): From(xf,yf),To(xt,yt),Side(side){
		Score = 0;
		isMove = true;
	};
	struct _chessmove(BYTE xf, BYTE yf, BYTE xt, BYTE yt, BYTE side,bool _isMove) :
		From(xf, yf), To(xt, yt), Side(side),isMove(_isMove) {
		Score = 0;
	};
}CHESSMOVE;


class Define  
{
public:
	Define();
	virtual ~Define();
	
};
typedef struct node{
	int i;//行
	int j;//列
	int arc;//逆时钟有无弧
	int color;//颜色
}Node;

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

	/*	struct Task() {};
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
	struct Task(ChessBoard board, bool _isBlackPlay, int _depth, int _alpha, int _beta,ID_TYPE _boardId,
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
	*/

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

#endif // !defined(AFX_DEFINE_H__315ABE2D_6AB8_4DBC_AE20_5EA359AC5B00__INCLUDED_)
