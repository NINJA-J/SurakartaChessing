// Eveluation.h: interface for the CEveluation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVELUATION_H__D5138E45_7853_40AB_80A8_5D7BB3E091F8__INCLUDED_)
#define AFX_EVELUATION_H__D5138E45_7853_40AB_80A8_5D7BB3E091F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MoveGenerator.h"
#include "ChessBoard.h"

typedef struct valueVector {
	int pValue;
	int aValue;
	int mValue;//用来表示可移动分值
	int posValue;
	int numValue;//红方，黑方棋子数量
	int arcValue;//占弧值
	ValueVector operator-(const ValueVector & b)const {
		ValueVector temp;
		temp.pValue = pValue - b.pValue; temp.aValue = aValue - b.aValue; temp.mValue = mValue - b.mValue;
		temp.posValue = posValue - b.posValue;
		temp.numValue = numValue - b.numValue;
		temp.arcValue = arcValue - b.arcValue;
		return temp;
	}
	ValueVector operator-=(const ValueVector & b) {
		ValueVector temp;
		pValue -= b.pValue; aValue -= b.aValue; mValue -= b.mValue;
		posValue -= b.posValue; numValue -= b.numValue; arcValue -= b.arcValue;
		return *this;
	}
}ValueVector;

typedef struct weightVector {
	double pValue;
	double aValue;
	double mValue;//用来表示可移动分值
	double posValue;
	double numValue;//红方，黑方棋子数量
	double arcValue;//占弧值
	WeightVector operator-(const WeightVector & b)const {
		WeightVector temp;
		temp.pValue = pValue - b.pValue; temp.aValue = aValue - b.aValue; temp.mValue = mValue - b.mValue;
		temp.posValue = posValue - b.posValue;
		temp.numValue = numValue - b.numValue;
		temp.arcValue = arcValue - b.arcValue;
		return temp.normalize();
	}
	WeightVector operator-=(const WeightVector & b) {
		WeightVector temp;
		pValue -= b.pValue; aValue -= b.aValue; mValue -= b.mValue;
		posValue -= b.posValue; numValue -= b.numValue; arcValue -= b.arcValue;
		return this->normalize();
	}
	inline WeightVector normalize() {
		double total = pValue + aValue + mValue + posValue + numValue + arcValue;
		pValue /= total;
		aValue /= total;
		mValue /= total;
		posValue /= total;
		numValue /= total;
		arcValue /= total;
		return *this;
	}
}WeightVector;

double operator*(ValueVector value, WeightVector weight) {
	double sum = 0;
	sum += value.pValue * weight.pValue;
	sum += value.aValue * weight.aValue;
	sum += value.mValue * weight.mValue;
	sum += value.posValue * weight.posValue;
	sum += value.numValue * weight.numValue;
	sum += value.arcValue * weight.arcValue;
	return sum;
}

double operator*(WeightVector weight, ValueVector value) {
	double sum = 0;
	sum += value.pValue * weight.pValue;
	sum += value.aValue * weight.aValue;
	sum += value.mValue * weight.mValue;
	sum += value.posValue * weight.posValue;
	sum += value.numValue * weight.numValue;
	sum += value.arcValue * weight.arcValue;
	return sum;
}


class CEveluation  
{
public:
	CEveluation();
	virtual ~CEveluation();
	virtual int evaluate(BYTE position[6][6],BOOL bIsBlackTurn);//估值函数，对传入的棋盘打分，bIsBlackTurn代表轮到谁走棋
	void GetAttackInfo(BYTE position[6][6]);
	int GetArcValue(BYTE position[6][6], BOOL bIsBlackTurn);//用于计算占弧价值
	BYTE m_AttackPos[6][6];
	BYTE m_ProtectPos[6][6];
	BYTE m_MovePos[6][6];
	CMoveGenerator *m_pMg;
private:
	ChessBoard* chessBoard;
	ValueVector bValue, rValue;
	WeightVector weights[5];

	void pointProc(int x, int y);
	void arcPointProc(bool isOuter, int index);
	void stepProc(int fx, int fy, int tx, int ty);
	void arcStepProc(bool isOuter, int fIndex, int tIndex);
	
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
