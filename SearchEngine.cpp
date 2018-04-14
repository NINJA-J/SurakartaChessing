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

CSearchEngine::CSearchEngine(){}

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
