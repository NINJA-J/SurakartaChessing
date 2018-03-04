#pragma once

#include "stdafx.h"
#include "Define.h"
#include <stack>

typedef unsigned long long int ID_TYPE;

#define PX 0
#define PY 1

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
	stack<CHESSMOVE> moves,cMoves;

	static const BYTE defaultStartBoard[6][6];

	int addMove(int nFromX, int nToX, int nFromY, int nToY);
	void initIdList();

	int bNum , rNum ;//红方，黑方棋子数量
public: 
	ChessBoard(bool isBlackFirst = true);
	ChessBoard(BYTE position[6][6], bool isBlackFirst = true);
	bool setChessPosition(const BYTE position[6][6], bool isBlackFirst = true);

	bool setChessTurn(bool isBlackTurn);
	int getChessTurn();

	void move(int fX, int fY, int tX, int tY);
	void move(CHESSMOVE move);
	void cMove(CHESSMOVE move);
	void unMove();
	int finishedMoves();
	int isGameOver();//在搜索中用到，表示某一方的搜索树的终局

	int getLoopStart(int arc);
	ID_TYPE getId();
	inline ID_TYPE getIdRaw();
	int getNums(bool isBlack);
	int getSearchMoves();
	bool getTurn();
	void getPosition(BYTE pos[][6]);

	BYTE* operator[](int x);
	BYTE& operator[](CHESSNAMPOS pos);
	inline BYTE& pInner(int index);
	inline BYTE& pOuter(int index);
	BYTE& pArc(int arc, int index);
};
