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

	//�����һ���߷��Ƿ�Ϸ�
	static BOOL IsValidMove(BYTE position[6][6],int nFromX,int nFromY,int nToX,int nToY);

	//�������������ϵ����кϷ����߷�
	bool initOutputList(CHESSMOVE *list, int maxSize);
	int createPossibleMoves(ChessBoard &board, CHESSMOVE* list, int maxSize );
	int createPossibleMoveOld(BYTE position[6][6], int nSide, CHESSMOVE *list, int maxSize);
	inline void addMove(CHESSMOVE move);
	bool cmpMoveLists(CHESSMOVE* list1, CHESSMOVE* list2, int size1, int size2);
//	int AnalysisAttackInfo(BYTE position[6][6],int &bNum,int &bPos, int &bAttack, int &bProtect, int &bMove,int &rNum,int &rPos, int &rAttack, int &rProtect, int &rMove);
private: 
	static const int arcLoop[2][24][2];//64-75�Ķ�����ӵ�cpp�ļ���

	CHESSMOVE* listOutput;
	int listMaxSpace;
	int moveCount;
};

#endif // !defined(AFX_MOVEGENERATOR_H__70DFE443_B8BC_48BB_871B_68E756817EAE__INCLUDED_)
