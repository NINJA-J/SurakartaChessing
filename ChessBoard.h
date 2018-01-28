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
	int loopStart[2];
	bool isBlackTurn;

	int moveCount;
	CHESSMOVE** moveList;
	stack<CHESSMOVE> moves;

	static const int arcLoop[2][24][2];//64-75�Ķ�����ӵ�cpp�ļ��� tt
	static const int posScore[3][6][6];
	static const BYTE defaultStartBoard[6][6];

	int addMove(int nFromX, int nToX, int nFromY, int nToY);

	int bValue ,rValue ;//�ܵ�����ֵ
	int bPValue , rPValue ;
	int bAValue , rAValue ;
	int bMValue , rMValue ;//������ʾ���ƶ���ֵ
	int bPosValue , rPosValue ;
	int bNum , rNum ;//�췽���ڷ���������
	int bArcValue , rArcValue;//ռ��ֵ
public:
	ChessBoard(bool isBlackFirst = true);
	ChessBoard(BYTE position[6][6], bool isBlackFirst = true);
	bool setChessPosition(const BYTE position[6][6]);
	bool setChessTurn(bool isBlackTurn);

	bool isValidMove(int nFromX, int nFromY, int nToX, int nToY);
	//�������������ϵ����кϷ����߷�
	int createPossibleMove(CHESSMOVE* moveList, int nSide);
	int analysis();
	int value();

	void move(int fX, int fY, int tX, int tY);
	void move(CHESSMOVE move);
	void unMove();
	int isGameOver(bool isBlackPlaying);//���������õ�����ʾĳһ�������������վ�

	int getValue(bool isBlack);
	int getPValue(bool isBlack);
	int getAValue(bool isBlack);
	int getMValue(bool isBlack);
	int getPosValue(bool isBlack);
	int getNums(bool isBlack);
	int getArcValue(bool isBlack);

	BYTE* operator[](int x);
};
