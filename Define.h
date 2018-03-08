// Define.h: interface for the Define class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFINE_H__315ABE2D_6AB8_4DBC_AE20_5EA359AC5B00__INCLUDED_)
#define AFX_DEFINE_H__315ABE2D_6AB8_4DBC_AE20_5EA359AC5B00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define NOCHESS 0
#define BLACK   1
#define RED     2
#define IsBlack(x) (x==1)
#define IsRed(x) (x==2)
#define IsSameSide(x,y) ((IsBlack(x)&&IsBlack(y))||(IsRed(x)&&IsRed(y)))

typedef struct _chessmanposition
{
	BYTE x;
	BYTE y;
	//BYTE arc;//不需要，可以由x、y的坐标从棋盘上判断是否挨着弧
}CHESSNAMPOS;

typedef struct _chessmove
{
	short Side;
	CHESSNAMPOS From;
	CHESSNAMPOS To;
	int Score;
	bool operator==(const _chessmove & move)
	{
		if(From.x==move.From.x && From.y==move.From.y && To.x==move.To.x && To.y==move.To.y)
			return true;
		else
			return false;

	}
}CHESSMOVE;


class Define  
{
public:
	Define();
	virtual ~Define();
	
};
typedef struct node{
	int i;//行
	int j;//列
	int arc;//逆时钟有无弧
	int color;//颜色
}Node;


#endif // !defined(AFX_DEFINE_H__315ABE2D_6AB8_4DBC_AE20_5EA359AC5B00__INCLUDED_)
