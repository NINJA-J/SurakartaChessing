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

CEvaluation::CEvaluation(ChessBoard &board) {
	chessBoard = &board;
}

CEvaluation::~CEvaluation() {}

void CEvaluation::setChessBoard(ChessBoard &board) {
	chessBoard = &board;
}

int CEvaluation::evaluate(BYTE position[6][6], BOOL IsBlackturn) {

	//你需要在这个函数里添加有如下功能的代码：
	//1.可以知道棋盘上敌方和我方棋子的数量
	//2.可以得到双方所占位置的分值，每个位置对应的分值可参考相关论文
	//3.可以得到我方棋子可移动分值
	//4.尽量加一些其他估值方法

	ValueVector values;
	WeightVector weights(
		1, //pWeight	- 保护权值
		2, //aWeight	- 攻击权值
		1, //mWeight	- 走子权值
		1, //posWeight	- 位置权值
		6, //numWeight	- 子力权值
		1  //arcWeight	- 占弧权值
	);
	
	 return  weights * analysis(IsBlackturn);
}

int CEvaluation::evaluate()
{
	BYTE pos[6][6];
	chessBoard->getPosition(pos);
	return evaluate(pos,chessBoard->getTurn());
}

ValueVector CEvaluation::analysis( bool isBlackTurn)
{
	ValueVector rVal,bVal;
	BYTE loopStart[2] = { -1,-1 };
	
	for (int arc = INNER; arc <= OUTER; arc++) {
		for (int i = 0; i < 24; i++) {
			if (chessBoard->pArc(arc,i)) {
				loopStart[arc] = i;//sssssss
				break;
			}
		}
	}

	int s, e = -1, colorS, colorE;//起点下标，终点下标，起点颜色，终点颜色
	for (int arc = INNER; arc <= OUTER; arc++) {//内外弧各循环一次
		if (loopStart[arc] != -1) { //判断当前弧上有点
			for (int dir = 1; dir <= 23; dir += 22) { // 顺时针/逆时针
				s = loopStart[arc];
				e = (s + dir) % 24;

				colorS = chessBoard->pArc(arc,s);//保留起点颜色，起点置空，和旧函数思路一样
				chessBoard->pArc(arc,s) = 0;

				for (int i = 0; i < 24; i++, e = (e + dir) % 24) {
					colorE = chessBoard->pArc(arc,e);
					if (chessBoard->pArc(arc, e)) {
						if (s / 6 != e / 6)
							colorS == colorE ?
							(colorS == BLACK ? bVal.aValue++ : rVal.aValue++) :
							(colorS == BLACK ? bVal.pValue++ : rVal.pValue++);

						chessBoard->pArc(arc, s) = colorS;//恢复起点颜色，继续搜索
						s = e; //终点置为新起点
						colorS = chessBoard->pArc(arc, s);//保留起点颜色，起点置空，和旧函数思路一样
						chessBoard->pArc(arc, s) = 0;
					}
				}
				if (!chessBoard->pArc(arc, s)) chessBoard->pArc(arc, s) = colorS;
			}
		}
	}

	//生成走子着法
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			if ((*chessBoard)[i][j]) {
				(*chessBoard)[i][j] == BLACK ? 
					(bVal.posValue += posScore[0][i][j]) : 
					(rVal.posValue += posScore[0][i][j]);
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (0 <= x + i && x + i < 6 &&
							0 <= y + j && y + j < 6 &&
							(*chessBoard)[x + i][y + j] == 0) {
							(*chessBoard)[x + i][y + j] == BLACK ? bVal.mValue++ : rVal.mValue++;
						}
					}
				}
			}
		}
	}

	bVal.arcValue = getArcValue(B_PLAYING);
	rVal.arcValue = getArcValue(R_PLAYING);

	bVal.numValue = chessBoard->getNums(B_PLAYING);
	rVal.numValue = chessBoard->getNums(R_PLAYING);

	return isBlackTurn ? bVal - rVal : rVal - bVal;
}
/*
void CEvaluation::GetAttackInfo(BYTE position[6][6])
{
	BYTE CurPosition[6][6];
	memset(m_AttackPos,0,36);
	memset(m_ProtectPos,0,36);
	memset(m_MovePos,0,36);
    memcpy(CurPosition,position,36);
	int flag=1,i,j,m,n;
	for (i=0;i<6;i++) {
		for (j=0;j<6;j++) {
			if(CurPosition[i][j]) {
				for (m=0;m<6;m++) {
					for(n=0;n<6;n++) {
						if(i==m && j==n) {
							continue;
						}
						if( CurPosition[m][n] != CurPosition[i][j] ) { //
							if(  CurPosition[m][n] && m_pMg->IsValidMove(CurPosition,m,n,i,j)) {
								m_AttackPos[i][j]++;
							}
						} else {
							if(CurPosition[m][n]) {
								CurPosition[m][n] = 3 - CurPosition[i][j];
								if(m_pMg->IsValidMove(CurPosition,m,n,i,j)) {
									m_ProtectPos[i][j]++;
								}
								CurPosition[m][n]=CurPosition[i][j];
							}
						}
					}
				}
			}
		}
	}
	memcpy(position,CurPosition,36);
}
*/
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

bool CEvaluation::getBoardValue(ID_TYPE id, int depth, int & value) {
	unordered_map<ID_TYPE, valUnion>::const_iterator iter = boardValue.find(id);
	if (iter == boardValue.end()) return false;
	if (iter->second.depth < depth)return false;
	value = iter->second.value;
	return true;
}

bool CEvaluation::addBoardValue(ID_TYPE id, int depth, int value) {
	unordered_map<ID_TYPE, valUnion>::iterator iter = boardValue.find(id);
	if (iter == boardValue.end())
		boardValue.insert({ id,valUnion(depth,value) });
	else {
		iter->second.depth = depth;
		iter->second.value = value;
	}
	return true;
}

int CEvaluation::getArcValue(bool isBlack) {
	int BArcNum = 0, RArcNum = 0, NoArcNum = 0;
	int i;
	for (i = 1; i < 5; i++) {
		if ((*chessBoard)[i][0] == 1) {
			BArcNum++;
		}
		else if ((*chessBoard)[i][0] == 2) {
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if ((*chessBoard)[0][i] == 1) {
			BArcNum++;
		}
		else if ((*chessBoard)[0][i] == 2) {
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if ((*chessBoard)[i][5] == 1) {
			BArcNum++;
		}
		else if ((*chessBoard)[i][5] == 2) {
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if ((*chessBoard)[5][i] == 1) {
			BArcNum++;
		}
		else if ((*chessBoard)[5][i] == 2) {
			RArcNum++;
		}
		else {
			NoArcNum++;
		}
	}
	if (isBlack) {
		return BArcNum * 5 + NoArcNum * 5;
	}
	else {
		return RArcNum * 5 + NoArcNum * 5;
	}
}









