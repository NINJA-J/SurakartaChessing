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

const int arcLoop[2][24][2] = {
	{   //�ڻ�
		{ 1,0 },{ 1,1 },{ 1,2 },{ 1,3 },{ 1,4 },{ 1,5 },
		{ 0,4 },{ 1,4 },{ 2,4 },{ 3,4 },{ 4,4 },{ 5,4 },
		{ 4,5 },{ 4,4 },{ 4,3 },{ 4,2 },{ 4,1 },{ 4,0 },
		{ 5,1 },{ 4,1 },{ 3,1 },{ 2,1 },{ 1,1 },{ 0,1 }
	},{ //�⻡
		{ 2,0 },{ 2,1 },{ 2,2 },{ 2,3 },{ 2,4 },{ 2,5 },
		{ 0,3 },{ 1,3 },{ 2,3 },{ 3,3 },{ 4,3 },{ 5,3 },
		{ 3,5 },{ 3,4 },{ 3,3 },{ 3,2 },{ 3,1 },{ 3,0 },
		{ 5,2 },{ 4,2 },{ 3,2 },{ 2,2 },{ 1,2 },{ 0,2 }
	}
};

typedef struct _chessmanposition
{
	BYTE x;
	BYTE y;
	struct _chessmanposition() {};
	struct _chessmanposition(BYTE _x, BYTE _y) :x(_x), y(_y) {};
	//BYTE arc;//����Ҫ��������x��y��������������ж��Ƿ��Ż�
}CHESSNAMPOS;

typedef struct _chessmove
{
	short Side;
	CHESSNAMPOS From;
	CHESSNAMPOS To;
	int Score;
	bool isMove;//�ǲ��������ŷ�
	bool operator==(const _chessmove & move) {
		return (From.x == move.From.x && From.y == move.From.y && To.x == move.To.x && To.y == move.To.y);
	}

	struct _chessmove() {};
	struct _chessmove(BYTE xf, BYTE yf, BYTE xt, BYTE yt, short side): From(xf,yf),To(xt,yt),Side(side){
		Score = 0;
		isMove = true;
	};
	struct _chessmove(BYTE xf, BYTE yf, BYTE xt, BYTE yt, short side,bool _isMove) :
		From(xf, yf), To(xt, yt), Side(side),isMove(_isMove) {
		Score = 0;
	};
}CHESSMOVE;


class Define  
{
public:
	Define();
	virtual ~Define();
	
};
typedef struct node{
	int i;//��
	int j;//��
	int arc;//��ʱ�����޻�
	int color;//��ɫ
}Node;


#endif // !defined(AFX_DEFINE_H__315ABE2D_6AB8_4DBC_AE20_5EA359AC5B00__INCLUDED_)
