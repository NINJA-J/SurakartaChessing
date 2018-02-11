#pragma once

#include "stdafx.h"
#include "Define.h"
#include "Eveluation.h"
#include <stack>

typedef unsigned long long int ID_TYPE;

#define X 0
#define Y 1

#define INNER 0
#define OUTER 1

#define MAX_INT 0x7fffffff
#define MIN_INT 0x80000000

#define SIG_BLACK (((ID_TYPE)1)<<63)
#define SIG_RED ((ID_TYPE)0)

using namespace std;

class ChessBoard {
private:
	BYTE position[6][6];
	BYTE* loop[2][24];
	int loopStart[2];
	bool isBlackTurn;

	ID_TYPE rawId;
	ID_TYPE idList[6][6][3];

	int moveCount;
	CHESSMOVE** moveList;
	stack<CHESSMOVE> moves;

	static const int posScore[3][6][6];
	static const BYTE defaultStartBoard[6][6];

	int addMove(int nFromX, int nToX, int nFromY, int nToY);
	void inidIdList();

	int bValue ,rValue ;//总的评估值
	int bPValue , rPValue ;
	int bAValue , rAValue ;
	int bMValue , rMValue ;//用来表示可移动分值
	int bPosValue , rPosValue ;
	int bNum , rNum ;//红方，黑方棋子数量
	int bArcValue , rArcValue;//占弧值
public:
	ChessBoard(bool isBlackFirst = true);
	ChessBoard(BYTE position[6][6], bool isBlackFirst = true);
	bool setChessPosition(const BYTE position[6][6]);
	bool setChessTurn(bool isBlackTurn);

	bool isValidMove(int nFromX, int nFromY, int nToX, int nToY);
	//产生给定棋盘上的所有合法的走法
	int createPossibleMove(CHESSMOVE* moveList, int nSide);
	int analysis();
	int value();
	void boardIteration(
		void(*pointProc)(int,int),
		void(*arcPointProc)(bool,int), 
		void(*stepProc)(int,int,int,int), 
		void(*eatProc)(bool,int,int)
	);

	void move(int fX, int fY, int tX, int tY);
	void move(CHESSMOVE move);
	void unMove();
	inline ID_TYPE getId();
	inline ID_TYPE getIdRaw();
	int finishedMoves();
	int isGameOver();//在搜索中用到，表示某一方的搜索树的终局

	int getPValue(bool isBlack);
	int getAValue(bool isBlack);
	int getMValue(bool isBlack);
	int getPosValue(bool isBlack);
	int getNums(bool isBlack);
	int getArcValue(bool isBlack);

	BYTE* operator[](int x);
	BYTE& operator[](CHESSNAMPOS pos);
};
