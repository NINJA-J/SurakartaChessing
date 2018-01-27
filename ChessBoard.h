#pragma once

#include "stdafx.h"
#include "Define.h"
#include <stack>

#define X 0
#define Y 1

#define INNER 0
#define OUTER 1

#define MAX_INT 0x7fffffff
#define MIN_INT 0x80000000

using namespace std;

class ChessBoard {
private:
	BYTE position[6][6];
	BYTE* loop[2][24];
	int checkStart[2];
	bool isBlackTurn;

	stack<CHESSMOVE> moves;

	static const int arcLoop[2][24][2];//64-75的定义添加到cpp文件里 tt
	static const int posScore[3][6][6];

	int m_nMoveCount;
	CHESSMOVE m_nMoveList[8][100];

	int addMove(int nFromX, int nToX, int nFromY, int nToY, int nPly);

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
	bool initTurn(bool isBlackFirst);

	bool isValidMove(int nFromX, int nFromY, int nToX, int nToY);
	//产生给定棋盘上的所有合法的走法
	int createPossibleMove(int nPly, int nSide);
	int analysis();
	int value();

	void move(int fX, int fY, int tX, int tY);
	void unMove();
	int isGameOver(bool isBlackPlaying);//在搜索中用到，表示某一方的搜索树的终局

	int getValue(bool isBlack);
	int getPValue(bool isBlack);
	int getAValue(bool isBlack);
	int getMValue(bool isBlack);
	int getPosValue(bool isBlack);
	int getNums(bool isBlack);
	int getArcValue(bool isBlack);

	BYTE* operator[](int x);
};
