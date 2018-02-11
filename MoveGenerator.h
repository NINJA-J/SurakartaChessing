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
	//�����һ���߷��Ƿ�Ϸ�
	static BOOL IsValidMove(BYTE position[6][6],int nFromX,int nFromY,int nToX,int nToY);
	//�������������ϵ����кϷ����߷�
	int CreatePossibleMove(BYTE position[6][6], int nPly, int nSide);
	int AnalysisAttackInfo(BYTE position[6][6],int &bNum,int &bPos, int &bAttack, int &bProtect, int &bMove,int &rNum,int &rPos, int &rAttack, int &rProtect, int &rMove);
    
	//����߷�����
	CHESSMOVE m_nMoveList[8][100];
	//����һ���߷�
	int AddMove(int nFromX,int nToX,int nFromY,int nToY,int nPly);
private: 
	
	static const int X = 0;
	static const int Y = 1;

	static const int INNER = 0;//�����ڻ���������±��Ϊ0
	static const int OUTER = 1;//�����⻷��������±��Ϊ1

	static const int arcLoop[2][24][2];//64-75�Ķ�����ӵ�cpp�ļ���

	BYTE position[6][6];
	BYTE* loop[2][24];
	int checkStart[2];
	//��¼m_nMoveList���߷�������
	int m_nMoveCount;

	ChessBoard* board;
};

#endif // !defined(AFX_MOVEGENERATOR_H__70DFE443_B8BC_48BB_871B_68E756817EAE__INCLUDED_)
