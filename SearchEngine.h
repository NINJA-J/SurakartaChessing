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
	//为当前局面走出下一步。接口
	virtual CHESSMOVE SearchAGoodMove(BYTE position[6][6], bool m_isPlayerBlack)=0;
	
	void SetSearchDepth(int nDepth) {m_nSearchDepth=nDepth;};//设定最大搜索深度
	void SetEveluator(CEvaluation *pEval) { /*m_pEval = pEval;*/ };//设定估值引擎		
	void SetMoveGenerator(CMoveGenerator *pMG) { /*m_pMG = pMG;*/ };//设定走法产生器
protected:
	BYTE MakeMove(CHESSMOVE *move);//由某一走法产生走了之后的棋盘
	void UnMakeMove(CHESSMOVE *move ,BYTE nChessID);//恢复一步
	BV_TYPE isGameOver();//判断是否有胜负
	BV_TYPE isGameOver(ChessBoard chessBoard);
	BYTE CurPosition[6][6];//当前节点棋盘的状态
	CHESSMOVE bestMove;//记录最佳的走法
	CMoveGenerator m_pMG;//走法产生器指针
	CEvaluation m_pEval;//估值核心指针
	int m_nSearchDepth;//最大搜索深度
	int m_nMaxDepth;//当前搜索的最大深度

	ChessBoard chessBoard;
	bool isBlackPlay;
};

#endif // !defined(AFX_SEARCHENGINE_H__43AA9763_1A7D_4ED0_B4E8_7B9196170C54__INCLUDED_)
