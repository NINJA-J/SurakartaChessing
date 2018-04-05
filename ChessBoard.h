#pragma once

#include "stdafx.h"
#include "Define.h"
#include <stack>

#define PX 0
#define PY 1

#define INNER 0
#define OUTER 1

#define SIG_BLACK (((ID_TYPE)1)<<63)
#define SIG_RED ((ID_TYPE)0)

using namespace std;

class ChessBoard {
private:
	BYTE position[6][6];
	BYTE* loop[2][24];
	int loopStart[2][3];
	bool isBlackTurn;

	ID_TYPE rawId;
	ID_TYPE idList[6][6][3];
	stack<CHESSMOVE> moves;

	static const BYTE defaultStartBoard[6][6];
	void initIdList();

	int bNum, rNum;//红方，黑方棋子数量
public:
	ChessBoard(bool isBlackFirst = true);
	ChessBoard(BYTE position[6][6], bool isBlackFirst = true);
	ChessBoard::ChessBoard(ChessBoard & copy);
	bool setChessPosition(const BYTE position[6][6], bool isBlackFirst = true);
	void printBoard(char* board = NULL, char noChess = '-', char black = '*', char red = 'O');

	bool setTurn(bool isBlackTurn);
	bool getTurn();

	void move(int fX, int fY, int tX, int tY);
	void move(CHESSMOVE move);
	void unMove();
	int getMoves();
	int isGameOver();//在搜索中用到，表示某一方的搜索树的终局

	int getLoopStart(int arc, int color = NOCHESS);
	inline void chkLoopStart();
	void attachLoopList();
	ID_TYPE getId();
	ID_TYPE getIdRaw();
	ID_TYPE getIdNormal();
	int getNums(bool isBlack);
	int getSearchMoves();
	void getPosition(BYTE pos[][6]);
	void outputPosition();
	BYTE* operator[](int x);
	BYTE& operator[](CHESSNAMPOS pos);
	BYTE& operator()(int arc, int index);
	inline BYTE& pInner(int index);
	inline BYTE& pOuter(int index);
	BYTE& pArc(int arc, int index);
};
