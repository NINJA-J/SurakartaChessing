// SearchEngine.h: interface for the CSearchEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEARCHENGINE_H__43AA9763_1A7D_4ED0_B4E8_7B9196170C54__INCLUDED_)
#define AFX_SEARCHENGINE_H__43AA9763_1A7D_4ED0_B4E8_7B9196170C54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Define.h"
#include "Evaluation.h"
#include "MoveGenerator.h"
#include "ChessBoard.h"
class CSearchEngine  
{
public:
	CSearchEngine();
	virtual ~CSearchEngine();
	//Ϊ��ǰ�����߳���һ�����ӿ�
	virtual CHESSMOVE SearchAGoodMove(BYTE position[6][6], bool m_isPlayerBlack)=0;
	
	void SetSearchDepth(int nDepth) {m_nSearchDepth=nDepth;};//�趨����������
	void SetEveluator(CEvaluation *pEval) { /*m_pEval = pEval;*/ };//�趨��ֵ����		
	void SetMoveGenerator(CMoveGenerator *pMG) { /*m_pMG = pMG;*/ };//�趨�߷�������
protected:
	BYTE MakeMove(CHESSMOVE *move);//��ĳһ�߷���������֮�������
	void UnMakeMove(CHESSMOVE *move ,BYTE nChessID);//�ָ�һ��
	BV_TYPE isGameOver();//�ж��Ƿ���ʤ��
	BV_TYPE isGameOver(ChessBoard chessBoard);
	BYTE CurPosition[6][6];//��ǰ�ڵ����̵�״̬
	CHESSMOVE bestMove;//��¼��ѵ��߷�
	CMoveGenerator m_pMG;//�߷�������ָ��
	CEvaluation m_pEval;//��ֵ����ָ��
	int m_nSearchDepth;//����������
	int m_nMaxDepth;//��ǰ������������

	ChessBoard chessBoard;
	bool isBlackPlay;
};

#endif // !defined(AFX_SEARCHENGINE_H__43AA9763_1A7D_4ED0_B4E8_7B9196170C54__INCLUDED_)
