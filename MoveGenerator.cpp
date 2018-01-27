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
	memset(position, 0, sizeof(BYTE[6][6]));
	for (int i = 0; i < 24; i++) {
		for (int arc = INNER; arc <= OUTER; arc++) {
			loop[arc][i] = &(this->position[arcLoop[arc][i][X]][arcLoop[arc][i][Y]]);
		}
	}
	checkStart[INNER] = checkStart[OUTER] = -1;
}

CMoveGenerator::~CMoveGenerator()
{

}
BOOL CMoveGenerator::IsValidMove(BYTE position[6][6], int nFromX, int nFromY, int nToX, int nToY) {
	/* һЩ�Ľ�
	�����������ԭ�����Ļ����Ͻ����˸Ľ������ж����̽����˼򻯲��������Ч��
	ԭ������˼·����
	��Ŀ��λ���������������ŷ����ж�Ŀ��λ���Ƿ��ڳ�ʼλ����Χ1������
	��Ŀ��λ���������ǳ����ŷ��������жϼ�����
	static const������Ҫ�Կռ任ʱ��������ɴ���
	���Ĵ�����forѭ����ҪΪ���ٴ�������չ����ʽ��ע��
	*/
	static const int X = 0;
	static const int Y = 1;

	static const int INNER = 0;//�����ڻ���������±��Ϊ0
	static const int OUTER = 1;//�����⻷��������±��Ϊ1

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
					position[arcLoop[arc][i][X]][arcLoop[arc][i][Y]];
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
			/* �ڻ���������������
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
						if (arcLoop[arc][edIndex][X] == nFromX &&
							arcLoop[arc][edIndex][Y] == nFromY)
							edIndex = colorLoop[edIndex][arc][dir];//95-98�����
						if (arcLoop[arc][edIndex][X] == nToX &&
							arcLoop[arc][edIndex][Y] == nToY) {
							if (stIndex / 6 != edIndex / 6) return true;
							if (dir == LAST && stIndex < edIndex) return true;//��76-80
							if (dir == NEXT && stIndex > edIndex) return true;//��76-80
						}
					}
				}
			}
		}
		/* 8�ֹ���������д��
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


int CMoveGenerator::AddMove(int nFromX,int nFromY,int nToX,int nToY,int nPly) {
 	m_nMoveList[nPly][m_nMoveCount].From.x=nFromX;
	m_nMoveList[nPly][m_nMoveCount].From.y=nFromY;
	m_nMoveList[nPly][m_nMoveCount].To.x=nToX;
	m_nMoveList[nPly][m_nMoveCount].To.y=nToY;

	m_nMoveCount++;
	return m_nMoveCount;
}


int CMoveGenerator::CreatePossibleMove(BYTE position[6][6],int nPly,int nSide) {//nside���������һ�����߷�	    
	m_nMoveCount = 0;
	memcpy(this->position, position, sizeof(BYTE[6][6]));
	for (int arc = INNER; arc <= OUTER; arc++) {
		for (int i = 0; i < 24; i++) {
			if (*loop[arc][i]) {
				checkStart[arc] = i;//sssssss
				break;
			}
		}
	}

	int s, e = -1, colorS, colorE;//����±꣬�յ��±꣬�����ɫ���յ���ɫ
	for (int arc = INNER; arc <= OUTER; arc++) {//���⻡��ѭ��һ��
		if (checkStart[arc] != -1) { //�жϵ�ǰ�����е�
			for (int dir = 1; dir <= 23; dir += 22) { // ˳ʱ��/��ʱ��
				s = checkStart[arc];
				e = ( s + dir ) % 24;
				colorS = *loop[arc][s];//���������ɫ������ÿգ��;ɺ���˼·һ��
				*loop[arc][s] = 0;
				for ( int i = 0; i < 24; i++, e = (e + dir) % 24) {
					colorE = *loop[arc][e];
					if (*loop[arc][e]) {
						if (colorS + nSide == 2 && colorE - nSide == 1 && s / 6 != e / 6) {
							AddMove(arcLoop[arc][s][X],
								arcLoop[arc][s][Y],
								arcLoop[arc][e][X],
								arcLoop[arc][e][Y],
								nPly);
						}
						*loop[arc][s] = colorS;//�ָ������ɫ����������
						s = e; //�յ���Ϊ�����
						colorS = *loop[arc][s];//���������ɫ������ÿգ��;ɺ���˼·һ��
						*loop[arc][s] = 0;
					}
				}
				if (!*loop[arc][s]) *loop[arc][s] = colorS;
			}
		}
	}

	//���������ŷ�
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

int CMoveGenerator::AnalysisAttackInfo(BYTE position[6][6],int &bNum, int & bAttack, int & bProtect, int & bMove, int &rNum, int & rAttack, int & rProtect, int & rMove)
{
	memcpy(this->position, position, sizeof(BYTE[6][6]));
	bAttack = bProtect = rAttack = rProtect = bMove = rMove = 0;
	for (int arc = INNER; arc <= OUTER; arc++) {
		for (int i = 0; i < 24; i++) {
			if (*loop[arc][i]) {
				checkStart[arc] = i;//sssssss
				break;
			}
		}
	}

	int s, e = -1, colorS, colorE;//����±꣬�յ��±꣬�����ɫ���յ���ɫ
	for (int arc = INNER; arc <= OUTER; arc++) {//���⻡��ѭ��һ��
		if (checkStart[arc] != -1) { //�жϵ�ǰ�����е�
			for (int dir = 1; dir <= 23; dir += 22) { // ˳ʱ��/��ʱ��
				s = checkStart[arc];
				e = (s + dir) % 24;

				colorS = *loop[arc][s];//���������ɫ������ÿգ��;ɺ���˼·һ��
				*loop[arc][s] = 0;

				for (int i = 0; i < 24; i++, e = (e + dir) % 24) {
					colorE = *loop[arc][e];
					if (*loop[arc][e]) {
						if (s / 6 != e / 6)
							colorS == colorE ?
							(colorS == BLACK ? bAttack++ : rAttack++) :
							(colorS == BLACK ? bProtect++ : rProtect++);

						*loop[arc][s] = colorS;//�ָ������ɫ����������
						s = e; //�յ���Ϊ�����
						colorS = *loop[arc][s];//���������ɫ������ÿգ��;ɺ���˼·һ��
						*loop[arc][s] = 0;
					}
				}
				if (!*loop[arc][s]) *loop[arc][s] = colorS;
			}
		}
	}

	//���������ŷ�
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			if (position[i][j] ) {
				position[i][j] == BLACK ? bNum++ : rNum++;
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (0 <= x + i && x + i < 6 &&
							0 <= y + j && y + j < 6 &&
							position[x + i][y + j] == 0) {
							position[x + i][y + j] == BLACK ? bMove++ : rMove++;
						}
					}
				}
			}
		}
	}
	return 0;
}
