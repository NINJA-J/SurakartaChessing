// Evaluation.cpp: implementation of the CEvaluation class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"
#include "Evaluation.h"
#include <cstdlib>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////6
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BV_TYPE operator*(WeightVector weight, ValueVector value) {
	double sum = 0;
	sum += value.pValue * weight.pValue;
	sum += value.aValue * weight.aValue;
	sum += value.mValue * weight.mValue;
	sum += value.posValue * weight.posValue;
	sum += value.numValue * weight.numValue;
	sum += value.arcValue * weight.arcValue;
	return sum;
}

BV_TYPE operator*(ValueVector value, WeightVector weight) {
	double sum = 0;
	sum += value.pValue * weight.pValue;
	sum += value.aValue * weight.aValue;
	sum += value.mValue * weight.mValue;
	sum += value.posValue * weight.posValue;
	sum += value.numValue * weight.numValue;
	sum += value.arcValue * weight.arcValue;
	return sum;
}

const int CEvaluation::posScore[3][6][6] = {
	{
		{ 5,20,20,20,20,5 },
		{ 20,30,50,50,30,20 },
		{ 20,50,40,40,50,50 },
		{ 20,50,40,40,50,20 },
		{ 20,30,50,50,30,20 },
		{ 5,20,20,20,20,5 }
	},{
		{ 5,20,20,20,20,5 },
		{ 20,30,50,50,30,20 },
		{ 20,50,40,40,50,50 },
		{ 20,50,40,40,50,20 },
		{ 20,30,50,50,30,20 },
		{ 5,20,20,20,20,5 }
	},{
		{ 5,20,20,20,20,5 },
		{ 20,30,50,50,30,20 },
		{ 20,50,40,40,50,50 },
		{ 20,50,40,40,50,20 },
		{ 20,30,50,50,30,20 },
		{ 5,20,20,20,20,5 }
	}
};

CEvaluation::CEvaluation(){}

CEvaluation::~CEvaluation() {}

BV_TYPE CEvaluation::evaluate(ChessBoard &board, bool isBlackTurn) {

	//����Ҫ�������������������¹��ܵĴ��룺
	//1.����֪�������ϵз����ҷ����ӵ�����
	//2.���Եõ�˫����ռλ�õķ�ֵ��ÿ��λ�ö�Ӧ�ķ�ֵ�ɲο��������
	//3.���Եõ��ҷ����ӿ��ƶ���ֵ
	//4.������һЩ������ֵ����

	ValueVector values;
	WeightVector weights(
		1, //pWeight	- ����Ȩֵ
		2, //aWeight	- ����Ȩֵ
		1, //mWeight	- ����Ȩֵ
		1, //posWeight	- λ��Ȩֵ
		6, //numWeight	- ����Ȩֵ
		5  //arcWeight	- ռ��Ȩֵ
	);
	
	 return  weights * analysis(board,isBlackTurn);
}

ValueVector CEvaluation::analysis(ChessBoard &board,bool isBlackTurn)
{
	ValueVector rVal,bVal;

	int s, e = -1, colorS, colorE;//����±꣬�յ��±꣬�����ɫ���յ���ɫ
	for (int arc = INNER; arc <= OUTER; arc++) {//���⻡��ѭ��һ��
		if (board.getLoopStart(arc) != -1) { //�жϵ�ǰ�����е�
			for (int dir = 1; dir <= 23; dir += 22) { // ˳ʱ��/��ʱ��
				s = board.getLoopStart(arc);
				e = (s + dir) % 24;

				colorS = board(arc,s);//���������ɫ������ÿգ��;ɺ���˼·һ��
				board(arc,s) = 0;

				for (int i = 0; i < 24; i++, e = (e + dir) % 24) {
					colorE = board(arc,e);
					if (board(arc, e)) {
						if (s / 6 != e / 6)
							colorS != colorE ?
							(colorS == BLACK ? bVal.aValue++ : rVal.aValue++) :
							(colorS == BLACK ? bVal.pValue++ : rVal.pValue++);

						board(arc, s) = colorS;//�ָ������ɫ����������
						s = e; //�յ���Ϊ�����
						colorS = board(arc, s);//���������ɫ������ÿգ��;ɺ���˼·һ��
						board(arc, s) = 0;
					}
				}
				if (!board(arc, s)) board(arc, s) = colorS;
			}
		}
	}

	//���������ŷ�
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			if (board[i][j]) {
				board[i][j] == BLACK ? 
					(bVal.posValue += posScore[0][i][j]) : 
					(rVal.posValue += posScore[0][i][j]);
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (0 <= x + i && x + i < 6 &&
							0 <= y + j && y + j < 6 &&
							board[x + i][y + j] == 0) {
							board[i][j] == BLACK ? bVal.mValue++ : rVal.mValue++;
						}
					}
				}
			}
		}
	}

	bVal.arcValue = getArcValue(board,B_PLAYING);
	rVal.arcValue = getArcValue(board,R_PLAYING);

	bVal.numValue = board.getNums(B_PLAYING);
	rVal.numValue = board.getNums(R_PLAYING);

	return isBlackTurn ? bVal - rVal : rVal - bVal;
}

int CEvaluation::GetArcValue(BYTE position[6][6], BOOL IsBlackturn)
{
	int BArcNum=0, RArcNum=0, NoArcNum=0;
	int i;
	for (i = 1; i < 5; i++) {
		if (position[i][0] == 1)
		{
			BArcNum++;
		}
		else if (position[i][0] == 2)
		{
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (position[0][i] == 1)
		{
			BArcNum++;
		}
		else if (position[0][i] == 2)
		{
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (position[i][5] == 1)
		{
			BArcNum++;
		}
		else if (position[i][5] == 2)
		{
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (position[5][i] == 1)
		{
			BArcNum++;
		}
		else if (position[5][i] == 2)
		{
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	if (IsBlackturn) {
		return BArcNum * 5 + NoArcNum * 5;
	}
	else {
		return RArcNum * 5 + NoArcNum * 5;
	}
}

bool CEvaluation::getBoardValue(ID_TYPE id, int depth, BV_TYPE & value) {
	unordered_map<ID_TYPE, valUnion>::const_iterator iter = boardValue.find(id);
	if (iter == boardValue.end()) return false;
	if (iter->second.depth < depth)return false;
	value = iter->second.value;
	return true;
}

bool CEvaluation::addBoardValue(ID_TYPE id, int depth, BV_TYPE value) {
	unordered_map<ID_TYPE, valUnion>::iterator iter = boardValue.find(id);
	if (iter == boardValue.end())
		boardValue.insert({ id,valUnion(depth,value) });
	else {
		iter->second.depth = depth;
		iter->second.value = value;
	}
	return true;
}

int CEvaluation::getArcValue(ChessBoard &board,bool isBlack) {
	int BArcNum = 0, RArcNum = 0, NoArcNum = 0;
	int i;
	for (i = 1; i < 5; i++) {
		if (board[i][0] == 1) {
			BArcNum++;
		}
		else if (board[i][0] == 2) {
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (board[0][i] == 1) {
			BArcNum++;
		}
		else if (board[0][i] == 2) {
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (board[i][5] == 1) {
			BArcNum++;
		}
		else if (board[i][5] == 2) {
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (board[5][i] == 1) {
			BArcNum++;
		}
		else if (board[5][i] == 2) {
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	if (isBlack) {
		return BArcNum + NoArcNum;
	}
	else {
		return RArcNum + NoArcNum;
	}
}









