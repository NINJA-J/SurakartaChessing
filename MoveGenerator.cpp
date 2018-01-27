// MoveGenerator.cpp: implementation of the CMoveGenerator class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"

#include "MoveGenerator.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int CMoveGenerator::arcLoop[2][24][2] = {
	{   //内弧
		{ 1,0 },{ 1,1 },{ 1,2 },{ 1,3 },{ 1,4 },{ 1,5 },
		{ 0,4 },{ 1,4 },{ 2,4 },{ 3,4 },{ 4,4 },{ 5,4 },
		{ 4,5 },{ 4,4 },{ 4,3 },{ 4,2 },{ 4,1 },{ 4,0 },
		{ 5,1 },{ 4,1 },{ 3,1 },{ 2,1 },{ 1,1 },{ 0,1 }
	},{ //外弧
		{ 2,0 },{ 2,1 },{ 2,2 },{ 2,3 },{ 2,4 },{ 2,5 },
		{ 0,3 },{ 1,3 },{ 2,3 },{ 3,3 },{ 4,3 },{ 5,3 },
		{ 3,5 },{ 3,4 },{ 3,3 },{ 3,2 },{ 3,1 },{ 3,0 },
		{ 5,2 },{ 4,2 },{ 3,2 },{ 2,2 },{ 1,2 },{ 0,2 }
	} };
CMoveGenerator::CMoveGenerator() {
	memset(position, 0, sizeof(BYTE[6][6]));
	for (int i = 0; i < 24; i++) {
		for (int arc = INNER; arc <= OUTER; arc++) {
			loop[arc][i] = &(this->position[arcLoop[arc][i][X]][arcLoop[arc][i][Y]]);
		}
	}
	checkStart[0] = checkStart[1] = -1;
}

CMoveGenerator::~CMoveGenerator()
{

}
BOOL CMoveGenerator::IsValidMove(BYTE position[6][6], int nFromX, int nFromY, int nToX, int nToY) {
	/* 一些改进
	本函数在理解原函数的基础上进行了改进，对判断流程进行了简化并尝试提高效率
	原函数的思路在于
	若目标位置无字则考虑走子着法，判断目标位置是否在初始位置周围1格以内
	若目标位置有子则考虑吃子着法，具体判断见下文
	static const类型主要以空间换时间减少生成次数
	后文大量的for循环主要为减少代码量，展开形式见注释
	*/
	static const int X = 0;
	static const int Y = 1;

	static const int INNER = 0;//所有内环相关数组下标均为0
	static const int OUTER = 1;//所有外环相关数组下标均为1

	static const int COLOR = 0;//循环链表末3项对应的含义（主要增加可读性）
	static const int LAST = 1;
	static const int NEXT = 2;

	static const int arcLoop[2][24][2] = { //arcLoop[2][][]={{inner},{outer}}，一个完整的弧上的24个点的坐标组成的链
		{   //内弧
			{ 1,0 },{ 1,1 },{ 1,2 },{ 1,3 },{ 1,4 },{ 1,5 },
			{ 0,4 },{ 1,4 },{ 2,4 },{ 3,4 },{ 4,4 },{ 5,4 },
			{ 4,5 },{ 4,4 },{ 4,3 },{ 4,2 },{ 4,1 },{ 4,0 },
			{ 5,1 },{ 4,1 },{ 3,1 },{ 2,1 },{ 1,1 },{ 0,1 }
		},{ //外弧  
			{ 2,0 },{ 2,1 },{ 2,2 },{ 2,3 },{ 2,4 },{ 2,5 },
			{ 0,3 },{ 1,3 },{ 2,3 },{ 3,3 },{ 4,3 },{ 5,3 },
			{ 3,5 },{ 3,4 },{ 3,3 },{ 3,2 },{ 3,1 },{ 3,0 },
			{ 5,2 },{ 4,2 },{ 3,2 },{ 2,2 },{ 1,2 },{ 0,2 }
		} };

	int colorLoop[24][2][3];//数组模拟内外弧循环链表
							/* 一些说明
							loop[][][3]={color,lastIndex,nextIndex}
							//loop[][2][]={{inner},{outer}}

							//
							棋盘上每个点对应的弧链中的点的下标，重叠的点取其中一个下标 oneArcIndex = arcIndex[x][y]
							另一个下标的计算方法：根据棋盘对称性和上图中弧链坐标对称性可知
							anotherArcIndex = 23 - arcIndex[y][x]，
							对于非交点上的点(内弧上非1/4/13/16或外弧上非2/3/14/15)，前后两种算法得到的下标一样 */
	static const int arcIndex[2][6][6] = {//坐标下标映射表
		{ //innerArc，内弧
			{ -1,23,-1,-1, 6,-1 },
			{ 0, 1, 2, 3, 4, 5 },
			{ -1,21,-1,-1, 8,-1 },
			{ -1,20,-1,-1, 9,-1 },
			{ 17,16,15,14,13,12 },
			{ -1,18,-1,-1,11,-1 }
		},
		{ //outerArc，外弧
			{ -1,-1,23, 6,-1,-1 },
			{ -1,-1,22, 7,-1,-1 },
			{ 0, 1, 2, 3, 4, 5 },
			{ 17,16,15,14,13,12 },
			{ -1,-1,19,10,-1,-1 },
			{ -1,-1,18,11,-1,-1 }
		}
	};

	if (nFromX == nToX && nFromY == nToY) //起点终点重合
		return false;
	if (!position[nFromX][nFromY]) //起点无子
		return false;
	if (position[nFromX][nFromY] == position[nToX][nToY]) //起点终点颜色相同
		return false;

	if (!position[nToX][nToY]) { //终点无子，考虑走子
		return (nFromX - 1 <= nToX && nToX <= nFromX + 1 &&
			nFromY - 1 <= nToY && nToY <= nFromY + 1);
	}
	else { //终点有子，考虑吃子，由前面的判断可知起点终点颜色不同
		   /* 过弧可达性判断
		   判断分为两部分，可达性和是否过弧；内外弧上点的关系用双向链表表示，在这里用数组模拟，每个点的last和next表示弧上前一个和后一个点的下标
		   链表建立后可以通过判断起点的last和next对应的节点是否为终点判断可达
		   过弧则通过判断下标除六后是否相同,其中对于除6相同的情况,如下图
		   0   1   2   3   4   5   6   7   8   9   10  11  12  13  14 ....
		   -1  -1  -1  -1  -1  -1  -1  s   -1  -1  -1  e   -1  -1  -1 ....
		   对于这种情况，如果s是通过LAST（<==）方向搜索到的e，则可以根据s<e判断s经过了4个弧（s>e时0个）
		   反之，如果s是通过NEXT（==>）方向搜索到的e，则在s>e的情况下可判断s经过了4个弧（s<e时0个）
		   0   1   2   3   4   5   6   7   8   9   10  11  12  13  14 ....
		   -1  -1  -1  s   -1  -1  -1  -1  s   -1  -1  e   -1  1   -1 ....
		   对于另外一种情况，如上图，s处在交点位置上，使得s3可以通过弧再经过s8到达e，反向则不行
		   这种情况下链表的初始判断是e为s3的NEXT即s8，通过判断s3和e表示同一个点后可令e=e的NEXT即s8的NEXT，e11，从而得到e的正确位置
		   由于在进行过弧检测前会首先检测两个点是否在同一弧上，则不可能出现s8的NEXT为s3的情况（即整个弧上只有s一个点，矛盾）
		   故这种方法可以解决吃子路径问题，在用正确的s和e作过弧检测即可完成判断
		   个人思考：可以利用论文中的着法预生成思路，利用空间换时间，在一个特定的棋局情况下，直接生成对应的循环链表，这样可以直接生成所有可行的吃子着法，以降低链表生成次数来提高效率（时间多一点说不定可以多搜索几个局面）
		   */
		int head[2] = { -1,-1 };

		for (int i = 0; i < 24; i++) {
			for (int arc = INNER; arc <= OUTER; arc++) {  //对内外两个弧分别生成链表
				colorLoop[i][arc][COLOR] =
					position[arcLoop[arc][i][X]][arcLoop[arc][i][Y]];
				colorLoop[i][arc][LAST] = colorLoop[i][arc][NEXT] = -1; //链表节点初始化
																		/*colorLoop[i][arc][...] ==> 循环链表中arc弧上第i个节点的 颜色/前驱下标/后继下标
																		//arcLoop[arc][i][..] ==> 弧位置数组中arc弧上的第i个节点的 X坐标/Y坐标
																		//head[arc] ==> arc弧的循环链表的头结点的下标
																		//下面的代码主要构建循环链表，模拟链表的插入动作*/
				if (colorLoop[i][arc][COLOR]) {
					if (head[arc] == -1) {
						head[arc] =
							colorLoop[i][arc][LAST] =
							colorLoop[i][arc][NEXT] = i;
					}
					else {    //链接
						colorLoop[i][arc][LAST] = colorLoop[head[arc]][arc][LAST];
						colorLoop[i][arc][NEXT] = head[arc];
						colorLoop[colorLoop[head[arc]][arc][LAST]][arc][NEXT] = i;
						colorLoop[head[arc]][arc][LAST] = i;
					}
				}
			}
			/* 内弧链表单独构建代码
			colorLoop[i][OUTER][COLOR] =
			//                position[ arcLoop[OUTER][i][X] ][ arcLoop[OUTER][i][Y] ];
			//            if( colorLoop[i][OUTER][COLOR] ){
			//                if( OUTER_HEAD == -1 ){
			//                    OUTER_HEAD = colorLoop[i][OUTER][LAST] = colorLoop[i][OUTER][NEXT] = i;
			//                } else {
			//                    colorLoop[ i ][OUTER][LAST] = colorLoop[OUTER_HEAD][OUTER][LAST];
			//                    colorLoop[ i ][OUTER][NEXT] = OUTER_HEAD;
			//                    colorLoop[ colorLoop[OUTER_HEAD][OUTER][LAST] ][OUTER][NEXT] = i;
			//                    colorLoop[ OUTER_HEAD ][OUTER][LAST] = i;
			//                }
			//            }*/
		}

		int stIndex;//起始节点在弧中的下标
		int edIndex;//终点节点在弧中的下标
					//3层循环，每层两种，共8次判断，展开情况见下方注释
		for (int arc = INNER; arc <= OUTER; arc++) {   //内外弧分别检测
													   //arcIndex[arc][x][y] ==> arc弧中坐标为x,y的点在弧中的下标，不在弧中为-1
			if (arcIndex[arc][nFromX][nFromY] != -1 && arcIndex[arc][nToX][nToY] != -1) {//若两节点在弧的坐标下标映射表中均存在对应下标，则两点在同一弧上，有可能可达
				for (int idxSty = 0; idxSty <= 1; idxSty++) {
					stIndex = arcIndex[arc][nFromX][nFromY];//默认下标
					if (idxSty == 1) {
						if (!(nFromX == nFromY || nFromX + nFromY == 5))
							continue;//不在交点（对称轴上），下标唯一，不再判断
						else//在交点上，计算第二个下标
							stIndex = 23 - arcIndex[arc][nFromY][nFromX];
					}

					for (int dir = LAST; dir <= NEXT; dir++) {  //双向检查
						edIndex = colorLoop[stIndex][arc][dir];
						if (arcLoop[arc][edIndex][X] == nFromX &&
							arcLoop[arc][edIndex][Y] == nFromY)
							edIndex = colorLoop[edIndex][arc][dir];//95-98行情况
						if (arcLoop[arc][edIndex][X] == nToX &&
							arcLoop[arc][edIndex][Y] == nToY) {
							if (stIndex / 6 != edIndex / 6) return true;
							if (dir == LAST && stIndex < edIndex) return true;//见76-80
							if (dir == NEXT && stIndex > edIndex) return true;//见76-80
						}
					}
				}
			}
		}
		/* 8种过弧检测分立写法
		if( arcIndex[INNER][nFromX][nFromY] != -1 &&
		//            arcIndex[INNER][nToX  ][nToY  ] != -1 ){
		//            stIndex = arcIndex[INNER][nFromX][nFromY];
		//            edIndex = colorLoop[stIndex][INNER][LAST];
		//            if( arcLoop[INNER][edIndex][X] == nToX &&
		//                arcLoop[INNER][edIndex][Y] == nToY &&
		//                ( stIndex/6 != edIndex/6 || stIndex < edIndex ) ) return true;
		//            edIndex = colorLoop[stIndex][INNER][NEXT];
		//            if( arcLoop[INNER][edIndex][X] == nToX &&
		//                arcLoop[INNER][edIndex][Y] == nToY &&
		//                ( stIndex/6 != edIndex/6 || stIndex > edIndex ) ) return true;
		//
		//            stIndex = 23 - arcIndex[INNER][nFromY][nFromX];
		//            edIndex = colorLoop[stIndex][INNER][LAST];
		//            if( arcLoop[INNER][edIndex][X] == nToX &&
		//                arcLoop[INNER][edIndex][Y] == nToY &&
		//                ( stIndex/6 != edIndex/6 || stIndex < edIndex ) ) return true;
		//            edIndex = colorLoop[stIndex][INNER][NEXT];
		//            if( arcLoop[INNER][edIndex][X] == nToX &&
		//                arcLoop[INNER][edIndex][Y] == nToY &&
		//                ( stIndex/6 != edIndex/6 || stIndex > edIndex ) ) return true;
		//        }
		//        if( arcIndex[OUTER][nFromX][nFromY] != -1 &&
		//            arcIndex[OUTER][nToX  ][nToY  ] != -1 ){
		//            stIndex = arcIndex[OUTER][nFromX][nFromY];
		//            edIndex = colorLoop[stIndex][OUTER][LAST];
		//            if( arcLoop[OUTER][edIndex][X] == nToX &&
		//                arcLoop[OUTER][edIndex][Y] == nToY &&
		//                ( stIndex/6 != edIndex/6 || stIndex < edIndex ) ) return true;
		//            edIndex = colorLoop[stIndex][OUTER][NEXT];
		//            if( arcLoop[OUTER][edIndex][X] == nToX &&
		//                arcLoop[OUTER][edIndex][Y] == nToY &&
		//                ( stIndex/6 != edIndex/6 || stIndex > edIndex ) ) return true;
		//
		//            stIndex = 23 - arcIndex[OUTER][nFromY][nFromX];
		//            edIndex = colorLoop[stIndex][OUTER][LAST];
		//            if( arcLoop[OUTER][edIndex][X] == nToX &&
		//                arcLoop[OUTER][edIndex][Y] == nToY &&
		//                ( stIndex/6 != edIndex/6 || stIndex < edIndex ) ) return true;
		//            edIndex = colorLoop[stIndex][OUTER][NEXT];
		//            if( arcLoop[OUTER][edIndex][X] == nToX &&
		//                arcLoop[OUTER][edIndex][Y] == nToY &&
		//                ( stIndex/6 != edIndex/6 || stIndex > edIndex ) ) return true;
		//        }*/
		return false;
	}
}


int CMoveGenerator::AddMove(int nFromX,int nFromY,int nToX,int nToY,int nPly)
{

 		m_nMoveList[nPly][m_nMoveCount].From.x=nFromX;
		m_nMoveList[nPly][m_nMoveCount].From.y=nFromY;
		m_nMoveList[nPly][m_nMoveCount].To.x=nToX;
		m_nMoveList[nPly][m_nMoveCount].To.y=nToY;

	m_nMoveCount++;
	return m_nMoveCount;
}


int CMoveGenerator::CreatePossibleMove(BYTE position[6][6],int nPly,int nSide)//nside代表产生哪一方的走法
{	    
	m_nMoveCount = 0;
	static const int INNER = 0;//所有内环相关数组下标均为0
	static const int OUTER = 1;//所有外环相关数组下标均为1

	static const int arcLoop[2][24][2] = {
		{   //内弧
			{ 1,0 },{ 1,1 },{ 1,2 },{ 1,3 },{ 1,4 },{ 1,5 },
			{ 0,4 },{ 1,4 },{ 2,4 },{ 3,4 },{ 4,4 },{ 5,4 },
			{ 4,5 },{ 4,4 },{ 4,3 },{ 4,2 },{ 4,1 },{ 4,0 },
			{ 5,1 },{ 4,1 },{ 3,1 },{ 2,1 },{ 1,1 },{ 0,1 }
		},{ //外弧
			{ 2,0 },{ 2,1 },{ 2,2 },{ 2,3 },{ 2,4 },{ 2,5 },
			{ 0,3 },{ 1,3 },{ 2,3 },{ 3,3 },{ 4,3 },{ 5,3 },
			{ 3,5 },{ 3,4 },{ 3,3 },{ 3,2 },{ 3,1 },{ 3,0 },
			{ 5,2 },{ 4,2 },{ 3,2 },{ 2,2 },{ 1,2 },{ 0,1 }
		} };

	int s, e = -1, colorS, colorE;//起点下标，终点下标，起点颜色，终点颜色
	for (int arc = INNER; arc <= OUTER; arc++) {//内外弧各循环一次
		if (checkStart[arc] != -1) { //判断当前弧上有点
			for (int i = 1; i <= 23; i += 22) { // 顺时针/逆时针
				s = checkStart[arc];
				do {
					colorS = *loop[arc][s]; //保留起点颜色，起点置空，和旧函数思路一样
					*loop[arc][s] = 0;

					e = (s + i) % 24;
					while (!*loop[arc][e]) { e = (e + i) % 24; } //搜索到第一个有子的位置
					colorE = *loop[arc][e];//记录终点颜色

					if (colorS + nSide == 2 && colorE - nSide == 1 && s / 6 != e / 6) {
						AddMove(arcLoop[arc][s][X],
							arcLoop[arc][s][Y],
							arcLoop[arc][e][X],
							arcLoop[arc][e][Y],
							nPly);
					}
					*loop[arc][s] = colorS;//恢复起点颜色，继续搜索
					s = e; //终点置为新起点
				} while (e != checkStart[arc]);
			}
		}
	}

	//生成走子着法
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			if (position[i][j] == 2 - nSide) {
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (0 <= x + i && x + i < 6 &&
							0 <= y + j && y + j < 6 &&
							position[x + i][y + j] == 0) {
							AddMove(i, j, x + i, y + j, nPly);
						}
					}
				}
			}
		}
	}

	return m_nMoveCount;
}
