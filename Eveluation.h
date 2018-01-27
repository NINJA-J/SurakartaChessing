// Eveluation.h: interface for the CEveluation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVELUATION_H__D5138E45_7853_40AB_80A8_5D7BB3E091F8__INCLUDED_)
#define AFX_EVELUATION_H__D5138E45_7853_40AB_80A8_5D7BB3E091F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MoveGenerator.h"
class CEveluation  
{
public:
	CEveluation();
	virtual ~CEveluation();
	virtual int Eveluate(BYTE position[6][6],BOOL bIsBlackTurn);//估值函数，对传入的棋盘打分，bIsBlackTurn代表轮到谁走棋
	void GetAttackInfo(BYTE position[6][6]);
	int GetArcValue(BYTE position[6][6], BOOL bIsBlackTurn);//用于计算占弧价值
	BYTE m_AttackPos[6][6];
	BYTE m_ProtectPos[6][6];
	BYTE m_MovePos[6][6];
	CMoveGenerator *m_pMg;
private:
	int BlackValue,RedValue;
	int BNum,RNum;
	int BPosValue, RPosValue;
	int BProtectValue,BAttactValue;
	int RProtectValue,RAttactValue;
	int RMoveValue,BMoveValue;
	int RArcValue, BArcValue;
	int PosScore[6][6] = { 5,20,20,20,20,5,
						20,30,50,50,30,20,
						20,50,40,40,50,50,
						20,50,40,40,50,20,
						20,30,50,50,30,20,
						5,20,20,20,20,5 };
};

#endif // !defined(AFX_EVELUATION_H__D5138E45_7853_40AB_80A8_5D7BB3E091F8__INCLUDED_)
