// MoveGenerator.cpp: implementation of the CMoveGenerator class zsj.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Surakarta.h"

#include "MoveGenerator.h"
#include "math.h"
#include <thread>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int CMoveGenerator::arcLoop[2][24][2] = {
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
	} };

CMoveGenerator::CMoveGenerator() {
	listOutput = NULL;
	listMaxSpace = 0;
}

CMoveGenerator::CMoveGenerator(ChessBoard & _board) {
	listOutput = NULL;
	listMaxSpace = 0;
}

CMoveGenerator::~CMoveGenerator() { }

BOOL CMoveGenerator::IsValidMove(BYTE position[6][6], int nFromX, int nFromY, int nToX, int nToY) {
	/* һЩ�Ľ�
	�����������ԭ�����Ļ����Ͻ����˸Ľ������ж����̽����˼򻯲��������Ч��
	ԭ������˼·����
	��Ŀ��λ���������������ŷ����ж�Ŀ��λ���Ƿ��ڳ�ʼλ����Χ1������
	��Ŀ��λ���������ǳ����ŷ��������жϼ�����
	static const������Ҫ�Կռ任ʱ��������ɴ���
	���Ĵ�����forѭ����ҪΪ���ٴ�������չ����ʽ��ע��
	*/
	static const int COLOR = 0;//ѭ������ĩ3���Ӧ�ĺ��壨��Ҫ���ӿɶ��ԣ�
	static const int LAST = 1;
	static const int NEXT = 2;

	static const int arcLoop[2][24][2] = { //arcLoop[2][][]={{inner},{outer}}��һ�������Ļ��ϵ�24�����������ɵ���
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
		} };

	int colorLoop[24][2][3];//����ģ�����⻡ѭ������
							/* һЩ˵��
							loop[][][3]={color,lastIndex,nextIndex}
							//loop[][2][]={{inner},{outer}}

							//
							������ÿ�����Ӧ�Ļ����еĵ���±꣬�ص��ĵ�ȡ����һ���±� oneArcIndex = arcIndex[x][y]
							��һ���±�ļ��㷽�����������̶Գ��Ժ���ͼ�л�������Գ��Կ�֪
							anotherArcIndex = 23 - arcIndex[y][x]��
							���ڷǽ����ϵĵ�(�ڻ��Ϸ�1/4/13/16���⻡�Ϸ�2/3/14/15)��ǰ�������㷨�õ����±�һ�� */
	static const int arcIndex[2][6][6] = {//�����±�ӳ���
		{ //innerArc���ڻ�
			{ -1,23,-1,-1, 6,-1 },
			{ 0, 1, 2, 3, 4, 5 },
			{ -1,21,-1,-1, 8,-1 },
			{ -1,20,-1,-1, 9,-1 },
			{ 17,16,15,14,13,12 },
			{ -1,18,-1,-1,11,-1 }
		},
		{ //outerArc���⻡
			{ -1,-1,23, 6,-1,-1 },
			{ -1,-1,22, 7,-1,-1 },
			{ 0, 1, 2, 3, 4, 5 },
			{ 17,16,15,14,13,12 },
			{ -1,-1,19,10,-1,-1 },
			{ -1,-1,18,11,-1,-1 }
		}
	};

	if (nFromX == nToX && nFromY == nToY) //����յ��غ�
		return false;
	if (!position[nFromX][nFromY]) //�������
		return false;
	if (position[nFromX][nFromY] == position[nToX][nToY]) //����յ���ɫ��ͬ
		return false;

	if (!position[nToX][nToY]) { //�յ����ӣ���������
		return (nFromX - 1 <= nToX && nToX <= nFromX + 1 &&
			nFromY - 1 <= nToY && nToY <= nFromY + 1);
	}
	else { //�յ����ӣ����ǳ��ӣ���ǰ����жϿ�֪����յ���ɫ��ͬ
		   /* �����ɴ����ж�
		   �жϷ�Ϊ�����֣��ɴ��Ժ��Ƿ���������⻡�ϵ�Ĺ�ϵ��˫�������ʾ��������������ģ�⣬ÿ�����last��next��ʾ����ǰһ���ͺ�һ������±�
		   �����������ͨ���ж�����last��next��Ӧ�Ľڵ��Ƿ�Ϊ�յ��жϿɴ�
		   ������ͨ���ж��±�������Ƿ���ͬ,���ж��ڳ�6��ͬ�����,����ͼ
		   0   1   2   3   4   5   6   7   8   9   10  11  12  13  14 ....
		   -1  -1  -1  -1  -1  -1  -1  s   -1  -1  -1  e   -1  -1  -1 ....
		   ����������������s��ͨ��LAST��<==��������������e������Ը���s<e�ж�s������4������s>eʱ0����
		   ��֮�����s��ͨ��NEXT��==>��������������e������s>e������¿��ж�s������4������s<eʱ0����
		   0   1   2   3   4   5   6   7   8   9   10  11  12  13  14 ....
		   -1  -1  -1  s   -1  -1  -1  -1  s   -1  -1  e   -1  1   -1 ....
		   ��������һ�����������ͼ��s���ڽ���λ���ϣ�ʹ��s3����ͨ�����پ���s8����e����������
		   �������������ĳ�ʼ�ж���eΪs3��NEXT��s8��ͨ���ж�s3��e��ʾͬһ��������e=e��NEXT��s8��NEXT��e11���Ӷ��õ�e����ȷλ��
		   �����ڽ��й������ǰ�����ȼ���������Ƿ���ͬһ���ϣ��򲻿��ܳ���s8��NEXTΪs3�����������������ֻ��sһ���㣬ì�ܣ�
		   �����ַ������Խ������·�����⣬������ȷ��s��e��������⼴������ж�
		   ����˼�����������������е��ŷ�Ԥ����˼·�����ÿռ任ʱ�䣬��һ���ض����������£�ֱ�����ɶ�Ӧ��ѭ��������������ֱ���������п��еĳ����ŷ����Խ����������ɴ��������Ч�ʣ�ʱ���һ��˵�������Զ������������棩
		   */
		int head[2] = { -1,-1 };

		for (int i = 0; i < 24; i++) {
			for (int arc = INNER; arc <= OUTER; arc++) {  //�������������ֱ���������
				colorLoop[i][arc][COLOR] =
					position[arcLoop[arc][i][PX]][arcLoop[arc][i][PY]];
				colorLoop[i][arc][LAST] = colorLoop[i][arc][NEXT] = -1; //����ڵ��ʼ��
																		/*colorLoop[i][arc][...] ==> ѭ��������arc���ϵ�i���ڵ�� ��ɫ/ǰ���±�/����±�
																		//arcLoop[arc][i][..] ==> ��λ��������arc���ϵĵ�i���ڵ�� X����/Y����
																		//head[arc] ==> arc����ѭ�������ͷ�����±�
																		//����Ĵ�����Ҫ����ѭ������ģ������Ĳ��붯��*/
				if (colorLoop[i][arc][COLOR]) {
					if (head[arc] == -1) {
						head[arc] =
							colorLoop[i][arc][LAST] =
							colorLoop[i][arc][NEXT] = i;
					}
					else {    //����
						colorLoop[i][arc][LAST] = colorLoop[head[arc]][arc][LAST];
						colorLoop[i][arc][NEXT] = head[arc];
						colorLoop[colorLoop[head[arc]][arc][LAST]][arc][NEXT] = i;
						colorLoop[head[arc]][arc][LAST] = i;
					}
				}
			}
		}

		int stIndex;//��ʼ�ڵ��ڻ��е��±�
		int edIndex;//�յ�ڵ��ڻ��е��±�
					//3��ѭ����ÿ�����֣���8���жϣ�չ��������·�ע��
		for (int arc = INNER; arc <= OUTER; arc++) {   //���⻡�ֱ���
													   //arcIndex[arc][x][y] ==> arc��������Ϊx,y�ĵ��ڻ��е��±꣬���ڻ���Ϊ-1
			if (arcIndex[arc][nFromX][nFromY] != -1 && arcIndex[arc][nToX][nToY] != -1) {//�����ڵ��ڻ��������±�ӳ����о����ڶ�Ӧ�±꣬��������ͬһ���ϣ��п��ܿɴ�
				for (int idxSty = 0; idxSty <= 1; idxSty++) {
					stIndex = arcIndex[arc][nFromX][nFromY];//Ĭ���±�
					if (idxSty == 1) {
						if (!(nFromX == nFromY || nFromX + nFromY == 5))
							continue;//���ڽ��㣨�Գ����ϣ����±�Ψһ�������ж�
						else//�ڽ����ϣ�����ڶ����±�
							stIndex = 23 - arcIndex[arc][nFromY][nFromX];
					}

					for (int dir = LAST; dir <= NEXT; dir++) {  //˫����
						edIndex = colorLoop[stIndex][arc][dir];
						if (arcLoop[arc][edIndex][PX] == nFromX &&
							arcLoop[arc][edIndex][PY] == nFromY)
							edIndex = colorLoop[edIndex][arc][dir];//95-98�����
						if (arcLoop[arc][edIndex][PX] == nToX &&
							arcLoop[arc][edIndex][PY] == nToY) {
							if (stIndex / 6 != edIndex / 6) return true;
							if (dir == LAST && stIndex < edIndex) return true;//��76-80
							if (dir == NEXT && stIndex > edIndex) return true;//��76-80
						}
					}
				}
			}
		}
		return false;
	}
}

bool CMoveGenerator::initOutputList(CHESSMOVE * list, int maxSize) {
	if (list == NULL || maxSize == 0) return false;
	listOutput = list;
	listMaxSpace = maxSize;
	moveCount = 0;
	return true;
}

int CMoveGenerator::createPossibleMoves(ChessBoard &board,CHESSMOVE * list, int maxSize) {
	if (!initOutputList(list, maxSize)) return 0;
	bool playSide = board.getTurn();
	BYTE playColor = board.getTurn() == B_PLAYING ? BLACK : RED;

	int s, e;//����±꣬�յ��±꣬�����ɫ���յ���ɫ
	for (int arc = INNER; arc <= OUTER; arc++) {//���⻡��ѭ��һ��
		int sIndex = board.getLoopStart(arc, playColor);
		s = e = -1;
		if (sIndex != -1) { //�жϵ�ǰ�����е�
			for (int dir = 1; dir <= 23; dir += 22) { // ˳ʱ��/��ʱ��

				s = sIndex;
				do {
					int colorTemp = board(arc, s);
					board(arc, s) = 0;

					e = (s + dir) % 24;
					while (e != sIndex && !board(arc, e)) e = (e + dir) % 24;

					board(arc, s) = colorTemp;

					if (e != sIndex) {
						if (board(arc, s) == playColor&&board(arc, e) != playColor&&s / 6 != e / 6) {
							addMove(CHESSMOVE(
								arcLoop[arc][s][PX], arcLoop[arc][s][PY],
								arcLoop[arc][e][PX], arcLoop[arc][e][PY],
								playColor, false));
						}
					} else
						break;
					s = e;

				} while (s != sIndex);
			}
		}
	}

	//���������ŷ�
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			if (board[i][j] == playColor) {
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (0 <= x + i && x + i < 6 &&
							0 <= y + j && y + j < 6 &&
							(x || y) &&
							board[x + i][y + j] == 0) {
							addMove(CHESSMOVE(i, j, x + i, y + j, playColor, true));
						}
					}
				}
			}
		}
	}
	return moveCount;
}

int CMoveGenerator::createPossibleMoveOld(BYTE position[6][6], int nSide, CHESSMOVE * list, int maxSize) {
	if (!initOutputList(list, maxSize))return 0;
	for (int x = 0; x < 6; x++) {
		for (int y = 0; y < 6; y++) {
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 6; j++) {
					if (position[x][y] == 2 - nSide&&IsValidMove(position, x, y, i, j)) {
						addMove(CHESSMOVE(x, y, i, j, nSide, position[i][j] == 0));
					}
				}
			}
		}
	}
	return moveCount;
}

inline void CMoveGenerator::addMove(CHESSMOVE move) {
	if (listOutput&&moveCount < listMaxSpace) {
		listOutput[moveCount++] = move;
	}
}

bool CMoveGenerator::cmpMoveLists(CHESSMOVE * list1, CHESSMOVE * list2, int size1, int size2) {
	if (size1 != size2)return false;
	for (int i = 0; i < size1; i++) {
		bool found = false;
		for (int j = 0; j < size2; j++) {
			if (list1[i] == list2[j]) {
				found = true;
				break;
			}
		}
		if (!found)
			return false;
	}
	return true;
}
