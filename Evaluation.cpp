// Evaluation.cpp: implementation of the CEvaluation class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"
#include "Evaluation.h"
#include <cstdlib>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define USE_NEW_ANALYSIS

//////////////////////////////////////////////////////////////////////6
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

unordered_map<ID_TYPE, valUnion> CEvaluation::boardValue;

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

CEvaluation::CEvaluation() {}

CEvaluation::~CEvaluation() {}

BV_TYPE CEvaluation::evaluate(ChessBoard &board, bool isBlackTurn) {

	//你需要在这个函数里添加有如下功能的代码：
	//1.可以知道棋盘上敌方和我方棋子的数量
	//2.可以得到双方所占位置的分值，每个位置对应的分值可参考相关论文
	//3.可以得到我方棋子可移动分值
	//4.尽量加一些其他估值方法

	ValueVector values;

	return  weight * analysis(board, isBlackTurn);
}

ValueVector CEvaluation::analysis(ChessBoard &board, bool isBlackTurn)
{
	ValueVector rVal, bVal;

#ifdef USE_NEW_ANALYSIS
	int s, e;//起点下标，终点下标，起点颜色，终点颜色
	for (int arc = INNER; arc <= OUTER; arc++) {//内外弧各循环一次
		int sIndex = board.getLoopStart(arc);
		s = e = -1;
		if (sIndex != -1) { //判断当前弧上有点
			for (int dir = 1; dir <= 23; dir += 22) { // 顺时针/逆时针
				s = sIndex;
				do {
					int colorTemp = board(arc, s);
					board(arc, s) = 0;

					e = (s + dir) % 24;
					while (e != sIndex && !board(arc, e)) e = (e + dir) % 24;

					board(arc, s) = colorTemp;

					if (s / 6 != e / 6)
						board(arc, s) != board(arc, e) ?
						(board(arc, s) == BLACK ? bVal.aValue++ : rVal.aValue++) :
						(board(arc, s) == BLACK ? bVal.pValue++ : rVal.pValue++);
					if (e == sIndex)
						break;
					s = e;
				} while (s != sIndex);
			}
		}
	}
#else
	int s, e = -1, colorS, colorE;//起点下标，终点下标，起点颜色，终点颜色
	for (int arc = INNER; arc <= OUTER; arc++) {//内外弧各循环一次
		if (board.getLoopStart(arc) != -1) { //判断当前弧上有点
			for (int dir = 1; dir <= 23; dir += 22) { // 顺时针/逆时针
				s = board.getLoopStart(arc);
				e = (s + dir) % 24;

				colorS = board(arc, s);//保留起点颜色，起点置空，和旧函数思路一样
				board(arc, s) = 0;

				for (int i = 0; i < 24; i++, e = (e + dir) % 24) {
					colorE = board(arc, e);
					if (board(arc, e)) {
						if (s / 6 != e / 6)
							colorS != colorE ?
							(colorS == BLACK ? bVal.aValue++ : rVal.aValue++) :
							(colorS == BLACK ? bVal.pValue++ : rVal.pValue++);

						board(arc, s) = colorS;//恢复起点颜色，继续搜索
						s = e; //终点置为新起点
						colorS = board(arc, s);//保留起点颜色，起点置空，和旧函数思路一样
						board(arc, s) = 0;
					}
				}
				if (!board(arc, s)) board(arc, s) = colorS;
			}
		}
	}
#endif
	//生成走子着法
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

	bVal.arcValue = getArcValue(board, B_PLAYING);
	rVal.arcValue = getArcValue(board, R_PLAYING);

	bVal.numValue = board.getNums(B_PLAYING);
	rVal.numValue = board.getNums(R_PLAYING);

	return isBlackTurn ? bVal - rVal : rVal - bVal;
}

ValueVector CEvaluation::analysisOld(ChessBoard & board, bool isBlackTurn) {
	BYTE position[6][6];
	CMoveGenerator mg;
	board.getPosition(position);
	ValueVector val;
	int playColor = isBlackTurn ? BLACK : RED;
	int flag = 1, i, j, m, n;
	for (i = 0; i<6; i++) {
		for (j = 0; j<6; j++) {
			if (position[i][j]) {
				for (m = 0; m<6; m++) {
					for (n = 0; n<6; n++) {
						if (position[m][n]) {
							if (i == m && j == n) continue;

							if (position[m][n] != position[i][j]) {
								if (mg.IsValidMove(position, m, n, i, j))
									(position[m][n] == playColor ? val.aValue++ : val.aValue--);
							}
							else {
								position[i][j] = 3 - position[m][n];
								if (mg.IsValidMove(position, m, n, i, j))
									(position[m][n] == playColor ? val.pValue++ : val.pValue--);
								position[i][j] = position[m][n];
							}
						}
					}
				}
			}
		}
	}
	return val;
}

int CEvaluation::GetArcValue(BYTE position[6][6], BOOL IsBlackturn)
{
	int BArcNum = 0, RArcNum = 0, NoArcNum = 0;
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
#ifdef USE_MAP
	unordered_map<ID_TYPE, valUnion>::const_iterator iter = boardValue.find(id);
	if (iter == boardValue.end()) return false;
	if (iter->second.depth < depth)return false;
	value = iter->second.value;
	return true;
#else
	return false;
#endif
}

BV_TYPE CEvaluation::addBoardValue(ID_TYPE id, int depth, BV_TYPE value) {
#ifdef USE_MAP
	unordered_map<ID_TYPE, valUnion>::iterator iter = boardValue.find(id);
	if (iter == boardValue.end())
		boardValue.insert({ id,valUnion(depth,value) });
	else {
		iter->second.depth = depth;
		iter->second.value = value;
	}
#endif
	return value;
}

int CEvaluation::getArcValue(ChessBoard &board, bool isBlack) {
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

void CEvaluation::setWeightVector(WeightVector & wv) {
}

BV_TYPE CEvaluation::getValueDelta() {
	return weight.delta;
}









