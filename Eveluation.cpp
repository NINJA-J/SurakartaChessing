// Eveluation.cpp: implementation of the CEveluation class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"
#include "Eveluation.h"
#include<cstdlib>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////6
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEveluation::CEveluation()
{

}

CEveluation::~CEveluation()
{

}
int CEveluation::Eveluate(BYTE position[6][6], BOOL IsBlackturn)
{

	//你需要在这个函数里添加有如下功能的代码：
	//1.可以知道棋盘上敌方和我方棋子的数量
	//2.可以得到双方所占位置的分值，每个位置对应的分值可参考相关论文
	//3.可以得到我方棋子可移动分值
	//4.尽量加一些其他估值方法

	 BlackValue=0,RedValue=0;//红黑方总的评估值
	 BProtectValue=0,RProtectValue=0; 
	 BAttactValue=0,RAttactValue=0;
	 RMoveValue=0,BMoveValue=0;//用来表示可移动分值
	 BPosValue=0,RPosValue=0;
	 BNum=0,RNum=0;//红方，黑方棋子数量
	 RArcValue=0, BArcValue=0;//占弧值
	 int i,j;
	
	 m_pMg->AnalysisAttackInfo(position, BNum, BAttactValue, BProtectValue, BMoveValue, RNum, RAttactValue, RProtectValue, RMoveValue);

	if (IsBlackturn)
	{
		BArcValue = GetArcValue(position, IsBlackturn);
		RArcValue = GetArcValue(position, 0);
	}
	else {
		RArcValue = GetArcValue(position, IsBlackturn);
		BArcValue = GetArcValue(position, 1);
	}
	BlackValue = BNum * 6 + BMoveValue * 1 + BAttactValue * 2 + BPosValue * 1 + BArcValue * 1;//abcd分别表示各个评分因素的权值
	RedValue = RNum * 6 + RMoveValue * 1 + RAttactValue * 2 + RPosValue * 1 + RArcValue * 1;



	if (IsBlackturn)//了解极大极小搜索原理，理解IsBlackturn 这个变量是干什么用的和评估值的意义，然后确定return的是哪个变量
		return BlackValue-RedValue;//返回的是一个随机数，只是为了让程序能运行
	else
		return RedValue-BlackValue;
}




void CEveluation::GetAttackInfo(BYTE position[6][6])
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

int CEveluation::GetArcValue(BYTE position[6][6], BOOL IsBlackturn)
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







