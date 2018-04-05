// SearchEngine.cpp: implementation of the CSearchEngine class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"
#include "SearchEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSearchEngine::CSearchEngine():
	chessBoard(B_PLAYING){}

CSearchEngine::~CSearchEngine() {}

BYTE CSearchEngine::MakeMove(CHESSMOVE *move)//�ɴ�����߷��ı�����
{
	BYTE Side;
	Side=CurPosition[move->To.x][move->To.y];
	CurPosition[move->To.x][move->To.y]=CurPosition[move->From.x][move->From.y];
	CurPosition[move->From.x][move->From.y]=NOCHESS;

	return Side;

}


void CSearchEngine::UnMakeMove(CHESSMOVE *move ,BYTE nChessID) {//�ָ�����
	CurPosition[move->From.x][move->From.y]=CurPosition[move->To.x][move->To.y];
	CurPosition[move->To.x][move->To.y]=nChessID;
}

BV_TYPE CSearchEngine::isGameOver() {//δ��������0���������ؼ���/��Сֵ
	if (int type = chessBoard.isGameOver()) {
		int score = MAX_INT - chessBoard.getMoves() - 1;
		return ((type == B_WIN) ^ isBlackPlay) ? -score : score; //���
	}
	else
		return 0;
}

BV_TYPE CSearchEngine::isGameOver(ChessBoard chessBoard) {//δ��������0���������ؼ���/��Сֵ
	if (int type = chessBoard.isGameOver()) {
		int score = MAX_VALUE - chessBoard.getMoves() - 1;
		return ((type == B_WIN) ^ isBlackPlay) ? -score : score; //���
	}
	else
		return 0;
}
