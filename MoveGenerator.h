// MoveGenerator.h: interface for the CMoveGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVEGENERATOR_H__70DFE443_B8BC_48BB_871B_68E756817EAE__INCLUDED_)
#define AFX_MOVEGENERATOR_H__70DFE443_B8BC_48BB_871B_68E756817EAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Define.h"
#include <stack>
#include "ChessBoard.h"

class CMoveGenerator  
{
public:
	CMoveGenerator(void);
	CMoveGenerator(ChessBoard &board);
	virtual ~CMoveGenerator();

	void setChessBoard(ChessBoard &board);

	//检查下一个走法是否合法
	static BOOL IsValidMove(BYTE position[6][6],int nFromX,int nFromY,int nToX,int nToY);

	//产生给定棋盘上的所有合法的走法
	int createPossibleMoves( CHESSMOVE* list, int maxSize );
	inline void move(CHESSMOVE move);
	inline void unMove();
	inline void addMove(CHESSMOVE move);
//	int AnalysisAttackInfo(BYTE position[6][6],int &bNum,int &bPos, int &bAttack, int &bProtect, int &bMove,int &rNum,int &rPos, int &rAttack, int &rProtect, int &rMove);
private: 
	static const int arcLoop[2][24][2];//64-75的定义添加到cpp文件里

	ChessBoard* board;
	CHESSMOVE* listOutput;
	int listMaxSpace;
	int moveCount;
};

#endif // !defined(AFX_MOVEGENERATOR_H__70DFE443_B8BC_48BB_871B_68E756817EAE__INCLUDED_)
