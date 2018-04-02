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
	int mValue;//������ʾ���ƶ���ֵ
	int posValue;
	int numValue;//�췽���ڷ���������
	int arcValue;//ռ��ֵ

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
	double mValue;//������ʾ���ƶ���ֵ
	double posValue;
	double numValue;//�췽���ڷ���������
	double arcValue;//ռ��ֵ
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

	virtual BV_TYPE evaluate(ChessBoard &board, bool isBlackTurn);//��ֵ�������Դ�������̴�֣�bIsBlackTurn�����ֵ�˭����
	virtual ValueVector analysis(ChessBoard &board, bool isBlackTurn);
	virtual ValueVector analysisOld(ChessBoard &board, bool isBlackTurn);

	//void GetAttackInfo(BYTE position[6][6]);
	int GetArcValue(BYTE position[6][6], BOOL bIsBlackTurn);//���ڼ���ռ����ֵ

	bool getBoardValue(ID_TYPE id, int depth, BV_TYPE &value);
	bool addBoardValue(ID_TYPE id, int depth, BV_TYPE value);
	int getArcValue(ChessBoard &board, bool isBlack);

private:
	ValueVector bValue, rValue;
	WeightVector weights[5];
	unordered_map<ID_TYPE, valUnion> boardValue;

	static const int posScore[3][6][6];
};

#endif // !defined(AFX_Evaluation_H__D5138E45_7853_40AB_80A8_5D7BB3E091F8__INCLUDED_)
