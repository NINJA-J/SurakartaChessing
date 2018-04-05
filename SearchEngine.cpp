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

BYTE CSearchEngine::MakeMove(CHESSMOVE *move)//由传入的走法改变棋盘
{
	BYTE Side;
	Side=CurPosition[move->To.x][move->To.y];
	CurPosition[move->To.x][move->To.y]=CurPosition[move->From.x][move->From.y];
	CurPosition[move->From.x][move->From.y]=NOCHESS;

	return Side;

}


void CSearchEngine::UnMakeMove(CHESSMOVE *move ,BYTE nChessID) {//恢复棋盘
	CurPosition[move->From.x][move->From.y]=CurPosition[move->To.x][move->To.y];
	CurPosition[move->To.x][move->To.y]=nChessID;
}

BV_TYPE CSearchEngine::isGameOver() {//未结束返回0，结束返回极大/极小值
	if (int type = chessBoard.isGameOver()) {
		int score = MAX_INT - chessBoard.getMoves() - 1;
		return ((type == B_WIN) ^ isBlackPlay) ? -score : score; //异或
	}
	else
		return 0;
}

BV_TYPE CSearchEngine::isGameOver(ChessBoard chessBoard) {//未结束返回0，结束返回极大/极小值
	if (int type = chessBoard.isGameOver()) {
		int score = MAX_VALUE - chessBoard.getMoves() - 1;
		return ((type == B_WIN) ^ isBlackPlay) ? -score : score; //异或
	}
	else
		return 0;
}
