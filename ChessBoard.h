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

	static const int arcLoop[2][24][2];//64-75�Ķ�����ӵ�cpp�ļ��� tt
	static const int posScore[3][6][6];

	int m_nMoveCount;
	CHESSMOVE m_nMoveList[8][100];

	int addMove(int nFromX, int nToX, int nFromY, int nToY, int nPly);

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
	bool initTurn(bool isBlackFirst);

	bool isValidMove(int nFromX, int nFromY, int nToX, int nToY);
	//�������������ϵ����кϷ����߷�
	int createPossibleMove(int nPly, int nSide);
	int analysis();
	int value();

	void move(int fX, int fY, int tX, int tY);
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
