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
	

	ID_TYPE rawId;
	const ID_TYPE idList[6][6][3] = {
		{
			{ 0,(ID_TYPE)1,(ID_TYPE)-1 },{ 0,(ID_TYPE)3,(ID_TYPE)-3 },{ 0,(ID_TYPE)9,(ID_TYPE)-9 },
			{ 0,(ID_TYPE)27,(ID_TYPE)-27 },{ 0,(ID_TYPE)81,(ID_TYPE)-81 },{ 0,(ID_TYPE)243,(ID_TYPE)-243 }
		},{
			{ 0,(ID_TYPE)729,(ID_TYPE)-729 },{ 0,(ID_TYPE)2187,(ID_TYPE)-2187 },
			{ 0,(ID_TYPE)6561,(ID_TYPE)-6561 },{ 0,(ID_TYPE)19683,(ID_TYPE)-19683 },
			{ 0,(ID_TYPE)59049,(ID_TYPE)-59049 },{ 0,(ID_TYPE)177147,(ID_TYPE)-177147 }
		},{
			{ 0,(ID_TYPE)531441,(ID_TYPE)-531441 },{ 0,(ID_TYPE)1594323,(ID_TYPE)-1594323 },
			{ 0,(ID_TYPE)4782969,(ID_TYPE)-4782969 },{ 0,(ID_TYPE)14348907,(ID_TYPE)-14348907 },
			{ 0,(ID_TYPE)43046721,(ID_TYPE)-43046721 },{ 0,(ID_TYPE)129140163,(ID_TYPE)-129140163 }
		},{
			{ 0,(ID_TYPE)387420489,(ID_TYPE)-387420489 },{ 0,(ID_TYPE)1162261467,(ID_TYPE)-1162261467 },
			{ 0,(ID_TYPE)3486784401,(ID_TYPE)-3486784401 },{ 0,(ID_TYPE)10460353203,(ID_TYPE)-10460353203 },
			{ 0,(ID_TYPE)31381059609,(ID_TYPE)-31381059609 },{ 0,(ID_TYPE)94143178827,(ID_TYPE)-94143178827 }
		},{
			{ 0,(ID_TYPE)282429536481,(ID_TYPE)-282429536481 },{ 0,(ID_TYPE)847288609443,(ID_TYPE)-847288609443 },
			{ 0,(ID_TYPE)2541865828329,(ID_TYPE)-2541865828329 },{ 0,(ID_TYPE)7625597484987,(ID_TYPE)-7625597484987 },
			{ 0,(ID_TYPE)22876792454961,(ID_TYPE)-22876792454961 },{ 0,(ID_TYPE)68630377364883,(ID_TYPE)-68630377364883 }
		},{
			{ 0,(ID_TYPE)205891132094649,(ID_TYPE)-205891132094649 },{ 0,(ID_TYPE)617673396283947,(ID_TYPE)-617673396283947 },
			{ 0,(ID_TYPE)1853020188851841,(ID_TYPE)-1853020188851841 },{ 0,(ID_TYPE)5559060566555523,(ID_TYPE)-5559060566555523 },
			{ 0,(ID_TYPE)16677181699666569,(ID_TYPE)-16677181699666569 },{ 0,(ID_TYPE)50031545098999707,(ID_TYPE)-50031545098999707 }
		} };
	stack<CHESSMOVE> moves;

	const BYTE defaultStartBoard[6][6] = {
		{ 1,1,1,1,1,1 },
		{ 1,1,1,1,1,1 },
		{ 0,0,0,0,0,0 },
		{ 0,0,0,0,0,0 },
		{ 2,2,2,2,2,2 },
		{ 2,2,2,2,2,2 }
	};
	void initId();

	int bNum, rNum;//红方，黑方棋子数量
public:

	bool isBlackTurn;

	ChessBoard(Task &t);
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
