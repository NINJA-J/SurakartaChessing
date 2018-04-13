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
#include "Define.h"

typedef struct valueVector {
	int pValue;
	int aValue;
	int mValue;//用来表示可移动分值
	int posValue;
	int numValue;//红方，黑方棋子数量
	int arcValue;//占弧值

	valueVector() {
		pValue = aValue = mValue = posValue = numValue = arcValue = 0;
	}

	valueVector operator-(const valueVector & b)const {
		valueVector temp;
		temp.pValue = pValue - b.pValue; temp.aValue = aValue - b.aValue; temp.mValue = mValue - b.mValue;
		temp.posValue = posValue - b.posValue;
		temp.numValue = numValue - b.numValue;
		temp.arcValue = arcValue - b.arcValue;
		return temp;
	}
	valueVector operator-=(const valueVector & b) {
		valueVector temp;
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

	BV_TYPE delta;

	weightVector() {};

	weightVector(double p, double a, double m, double pos, double num, double arc) {
		pValue = p; aValue = a; mValue = m;
		posValue = pos; numValue = num; arcValue = arc;
		normalize();
	};

	weightVector operator-(const weightVector & b)const {
		WeightVector temp;
		temp.pValue = pValue - b.pValue; temp.aValue = aValue - b.aValue; temp.mValue = mValue - b.mValue;
		temp.posValue = posValue - b.posValue;
		temp.numValue = numValue - b.numValue;
		temp.arcValue = arcValue - b.arcValue;
		return temp.normalize();
	}

	weightVector operator-=(const weightVector & b) {
		WeightVector temp;
		pValue -= b.pValue; aValue -= b.aValue; mValue -= b.mValue;
		posValue -= b.posValue; numValue -= b.numValue; arcValue -= b.arcValue;
		return this->normalize();
	}

	inline weightVector normalize() {
		double total = pValue + aValue + mValue + posValue + numValue + arcValue;
		if (total != 0) {
			pValue /= total;
			aValue /= total;
			mValue /= total;
			posValue /= total;
			numValue /= total;
			arcValue /= total;
			delta = 1.0 / total;
		}
		return *this;
	}
}WeightVector;

BV_TYPE operator*(ValueVector value, WeightVector weight);

BV_TYPE operator*(WeightVector weight, ValueVector value);

class valUnion {
public:
	int depth;
	BV_TYPE value;
	valUnion(int d, BV_TYPE v) :depth(d), value(v) {};
	valUnion() {};
};

class CEvaluation {
public:
	CEvaluation();
	virtual ~CEvaluation();

	virtual BV_TYPE evaluate(ChessBoard &board, bool isBlackTurn);//估值函数，对传入的棋盘打分，bIsBlackTurn代表轮到谁走棋
	virtual ValueVector analysis(ChessBoard &board, bool isBlackTurn);
	virtual ValueVector analysisOld(ChessBoard &board, bool isBlackTurn);

	//void GetAttackInfo(BYTE position[6][6]);
	int GetArcValue(BYTE position[6][6], BOOL bIsBlackTurn);//用于计算占弧价值

	bool getBoardValue(ID_TYPE id, int depth, BV_TYPE &value);
	BV_TYPE addBoardValue(ID_TYPE id, int depth, BV_TYPE value);
	int getArcValue(ChessBoard &board, bool isBlack);

	void setWeightVector(WeightVector &wv);
	BV_TYPE getValueDelta();

private:
	ValueVector bValue, rValue;
	WeightVector weight = WeightVector(
		1, //pWeight	- 保护权值
		2, //aWeight	- 攻击权值
		1, //mWeight	- 走子权值
		1, //posWeight	- 位置权值
		6, //numWeight	- 子力权值
		5  //arcWeight	- 占弧权值
	);
	static unordered_map<ID_TYPE, valUnion> boardValue;

	static const int posScore[3][6][6];
};

#endif // !defined(AFX_Evaluation_H__D5138E45_7853_40AB_80A8_5D7BB3E091F8__INCLUDED_)
