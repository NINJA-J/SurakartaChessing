#pragma once

#include "stdafx.h"
#include "Define.h"
#include <stack>

#define X 0
#define Y 1

#define INNER 0
#define OUTER 1

using namespace std;

class ChessBoard {
private:
	BYTE position[6][6];
	BYTE* loop[2][24];
	int checkStart[2];

	stack<CHESSMOVE> moves;

	static const int arcLoop[2][24][2];//64-75的定义添加到cpp文件里

	int m_nMoveCount;
	CHESSMOVE m_nMoveList[8][100];

	int AddMove(int nFromX, int nToX, int nFromY, int nToY, int nPly);

	int bValue ,rValue ;//总的评估值
	int bPValue , rPValue ;
	int bAValue , rAValue ;
	int bMValue , rMValue ;//用来表示可移动分值
	int bPosValue , RPosValue ;
	int bNum , rNum ;//红方，黑方棋子数量
	int bArcValue , rArcValue;//占弧值
public:
	ChessBoard();
	ChessBoard(BYTE position[6][6]);

	BOOL IsValidMove(int nFromX, int nFromY, int nToX, int nToY);
	//产生给定棋盘上的所有合法的走法
	int createPossibleMove(int nPly, int nSide);
	int analysis(int &bNum, int &bAttack, int &bProtect, int &bMove, int &rNum, int &rAttack, int &rProtect, int &rMove);
	int value();

	void move(int fX, int fY, int tX, int tY);
	void unMove();
	int isGameOver();

	int value(bool isBlack);
	int pValue(bool isBlack);
	int aValue(bool isBlack);
	int mValue(bool isBlack);
	int posValue(bool isBlack);
	int nums(bool isBlack);
	int arcValue(bool isBlack);

	BYTE* operator[](int x);
};
