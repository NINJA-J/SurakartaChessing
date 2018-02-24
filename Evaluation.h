// Evaluation.h: interface for the CEveluation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVELUATION_H__D5138E45_7853_40AB_80A8_5D7BB3E091F8__INCLUDED_)
#define AFX_EVELUATION_H__D5138E45_7853_40AB_80A8_5D7BB3E091F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MoveGenerator.h"
#include "ChessBoard.h"
#include <unordered_map>

typedef struct valueVector {
	int pValue;
	int aValue;
	int mValue;//用来表示可移动分值
	int posValue;
	int numValue;//红方，黑方棋子数量
	int arcValue;//占弧值

	valueVector(){
		pValue = aValue = mValue = posValue = numValue = arcValue = 0;
	}

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
	WeightVector() {};
	WeightVector(double p, double a, double m, double pos, double num, double arc) {
		pValue = p; aValue = a; mValue = m;
		posValue = pos; numValue = num; arcValue = arc;
		normalize();
	};
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
		if (total != 0) {
			pValue /= total;
			aValue /= total;
			mValue /= total;
			posValue /= total;
			numValue /= total;
			arcValue /= total;
		}
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

struct valUnion {
	int depth;
	int value;
	valUnion(int d, int v) :depth(d), value(v) {};
	valUnion() {};
};

class CEvaluation  
{
public:
	CEvaluation();
	CEvaluation(ChessBoard& board);
	virtual ~CEvaluation();
	void setChessBoard(ChessBoard &board);

	virtual int evaluate(BYTE position[6][6],BOOL bIsBlackTurn);//估值函数，对传入的棋盘打分，bIsBlackTurn代表轮到谁走棋
	virtual int evaluate();
	virtual ValueVector analysis(bool isBlackTurn);

	void GetAttackInfo(BYTE position[6][6]);
	int GetArcValue(BYTE position[6][6], BOOL bIsBlackTurn);//用于计算占弧价值

	inline bool getBoardValue(ID_TYPE id, int depth, int &value);
	inline bool addBoardValue(ID_TYPE id, int depth, int value);
	inline int getArcValue(bool isBlack);

	BYTE m_AttackPos[6][6];
	BYTE m_ProtectPos[6][6];
	BYTE m_MovePos[6][6];
	CMoveGenerator *m_pMg;
private:
	ChessBoard* chessBoard;
	ValueVector bValue, rValue;
	WeightVector weights[5];
	unordered_map<ID_TYPE,valUnion> boardValue;

	static const int posScore[3][6][6];

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

#endif // !defined(AFX_Evaluation_H__D5138E45_7853_40AB_80A8_5D7BB3E091F8__INCLUDED_)
