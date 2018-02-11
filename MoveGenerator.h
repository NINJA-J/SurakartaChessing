// MoveGenerator.h: interface for the CMoveGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVEGENERATOR_H__70DFE443_B8BC_48BB_871B_68E756817EAE__INCLUDED_)
#define AFX_MOVEGENERATOR_H__70DFE443_B8BC_48BB_871B_68E756817EAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Define.h"

class CMoveGenerator  
{
public:
	CMoveGenerator();
	virtual ~CMoveGenerator();
	//检查下一个走法是否合法
	static BOOL IsValidMove(BYTE position[6][6],int nFromX,int nFromY,int nToX,int nToY);
	//产生给定棋盘上的所有合法的走法
	int CreatePossibleMove(BYTE position[6][6], int nPly, int nSide);
	int AnalysisAttackInfo(BYTE position[6][6],int &bNum,int &bPos, int &bAttack, int &bProtect, int &bMove,int &rNum,int &rPos, int &rAttack, int &rProtect, int &rMove);
    
	//存放走法队列
	CHESSMOVE m_nMoveList[8][100];
	//插入一个走法
	int AddMove(int nFromX,int nToX,int nFromY,int nToY,int nPly);
private: 
	
	static const int X = 0;
	static const int Y = 1;

	static const int INNER = 0;//所有内环相关数组下标均为0
	static const int OUTER = 1;//所有外环相关数组下标均为1

	static const int arcLoop[2][24][2];//64-75的定义添加到cpp文件里

	BYTE position[6][6];
	BYTE* loop[2][24];
	int checkStart[2];
	//记录m_nMoveList中走法的数量
	int m_nMoveCount;

	ChessBoard* board;
};

#endif // !defined(AFX_MOVEGENERATOR_H__70DFE443_B8BC_48BB_871B_68E756817EAE__INCLUDED_)
