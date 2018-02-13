#include "ChessBoard.h"

const int ChessBoard::posScore[3][6][6] = {
	{
		{5,20,20,20,20,5},
		{20,30,50,50,30,20},
		{20,50,40,40,50,50},
		{20,50,40,40,50,20},
		{20,30,50,50,30,20},
		{5,20,20,20,20,5 }
	},{
		{ 5,20,20,20,20,5 },
		{ 20,30,50,50,30,20 },
		{ 20,50,40,40,50,50 },
		{ 20,50,40,40,50,20 },
		{ 20,30,50,50,30,20 },
		{ 5,20,20,20,20,5 }
	},{
		{ 5,20,20,20,20,5 },
		{ 20,30,50,50,30,20 },
		{ 20,50,40,40,50,50 },
		{ 20,50,40,40,50,20 },
		{ 20,30,50,50,30,20 },
		{ 5,20,20,20,20,5 }
	}
};

const BYTE ChessBoard::defaultStartBoard[6][6] = {
	{1,1,1,1,1,1},
	{1,1,1,1,1,1},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{2,2,2,2,2,2},
	{2,2,2,2,2,2}
};

int ChessBoard::addMove(int nFromX, int nToX, int nFromY, int nToY) {
	if (moveList == NULL) return -1;

	(*moveList)[moveCount].From.x = nFromX;
	(*moveList)[moveCount].From.y = nFromY;
	(*moveList)[moveCount].To.x = nToX;
	(*moveList)[moveCount].To.y = nToY;

	moveCount++;
	return moveCount;
}

void ChessBoard::inidIdList() {
	long long temp = 1;
	rawId = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			idList[i][j][NOCHESS] = 0;
			idList[i][j][BLACK] = temp;
			idList[i][j][RED] = -temp;
			rawId += temp + idList[i][j][position[i][j]];
			temp *= 3;
		}
	}
}

ChessBoard::ChessBoard(bool isBlackFirst) {
	setChessPosition(defaultStartBoard);
	setChessTurn(isBlackFirst);

	for (int i = 0; i < 24; i++) {
		for (int arc = INNER; arc <= OUTER; arc++) {
			loop[arc][i] = &(this->position[arcLoop[arc][i][X]][arcLoop[arc][i][Y]]);
		}
	}
	loopStart[INNER] = loopStart[OUTER] = -1;
	moveList = NULL;
}

ChessBoard::ChessBoard(BYTE position[6][6], bool isBlackFirst) {
	setChessPosition(position);
	setChessTurn(isBlackFirst);

	for (int i = 0; i < 24; i++) {
		for (int arc = INNER; arc <= OUTER; arc++) {
			loop[arc][i] = &(this->position[arcLoop[arc][i][X]][arcLoop[arc][i][Y]]);
		}
	}
	loopStart[INNER] = loopStart[OUTER] = -1;
	moveList = NULL;
}

bool ChessBoard::setChessPosition(const BYTE position[6][6]) {
	bNum = rNum = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			this->position[i][j] = position[i][j];
			if (position[i][j] == BLACK) bNum++;
			if (position[i][j] == RED) rNum++;
		}
	}
	return true;
}

bool ChessBoard::setChessTurn(bool isBlackTurn) {
	if (moves.size() > 0)return false;
	this->isBlackTurn = isBlackTurn;
	return true;
}

int ChessBoard::getChessTurn() {
	return isBlackTurn;
}

bool ChessBoard::isValidMove(int nFromX, int nFromY, int nToX, int nToY)
{
	static const int COLOR = 0;//ѭ������ĩ3���Ӧ�ĺ��壨��Ҫ���ӿɶ��ԣ�
	static const int LAST = 1;
	static const int NEXT = 2;

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
		return false;
	}
}

int ChessBoard::createPossibleMove(CHESSMOVE* moveList, int nSide) {
	moveCount = 0;
	*(this->moveList) = moveList;
	memcpy(this->position, position, sizeof(BYTE[6][6]));
	for (int arc = INNER; arc <= OUTER; arc++) {
		for (int i = 0; i < 24; i++) {
			if (*loop[arc][i]) {
				loopStart[arc] = i;//sssssss
				break;
			}
		}
	}

	int s, e = -1, colorS, colorE;//����±꣬�յ��±꣬�����ɫ���յ���ɫ
	for (int arc = INNER; arc <= OUTER; arc++) {//���⻡��ѭ��һ��
		if (loopStart[arc] != -1) { //�жϵ�ǰ�����е�
			for (int dir = 1; dir <= 23; dir += 22) { // ˳ʱ��/��ʱ��
				s = loopStart[arc];
				e = (s + dir) % 24;
				colorS = *loop[arc][s];//���������ɫ������ÿգ��;ɺ���˼·һ��
				*loop[arc][s] = 0;
				for (int i = 0; i < 24; i++, e = (e + dir) % 24) {
					colorE = *loop[arc][e];
					if (*loop[arc][e]) {
						if (colorS + nSide == 2 && colorE - nSide == 1 && s / 6 != e / 6) {
							addMove(arcLoop[arc][s][X],
								arcLoop[arc][s][Y],
								arcLoop[arc][e][X],
								arcLoop[arc][e][Y]);
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
							addMove(i, j, x + i, y + j);

						}
					}
				}
			}
		}
	}
	moveList = NULL;
	return moveCount;
}

int ChessBoard::analysis() {
	bAValue = bPValue = bMValue = bNum = bPosValue = bValue = bArcValue = 0;
	rAValue = rPValue = rMValue = rNum = rPosValue = rValue = rArcValue = 0;
	//1        1         1        1                             1
	for (int arc = INNER; arc <= OUTER; arc++) {
		for (int i = 0; i < 24; i++) {
			if (*loop[arc][i]) {
				loopStart[arc] = i;//sssssss
				break;
			}
		}
	}

	int s, e = -1, colorS, colorE;//����±꣬�յ��±꣬�����ɫ���յ���ɫ
	for (int arc = INNER; arc <= OUTER; arc++) {//���⻡��ѭ��һ��
		if (loopStart[arc] != -1) { //�жϵ�ǰ�����е�
			for (int dir = 1; dir <= 23; dir += 22) { // ˳ʱ��/��ʱ��
				s = loopStart[arc];
				e = (s + dir) % 24;

				colorS = *loop[arc][s];//���������ɫ������ÿգ��;ɺ���˼·һ��
				*loop[arc][s] = 0;

				for (int i = 0; i < 24; i++, e = (e + dir) % 24) {
					colorE = *loop[arc][e];
					if (*loop[arc][e]) {
						if (s / 6 != e / 6)
							colorS == colorE ?
							(colorS == BLACK ? bAValue++ : rAValue++) :
							(colorS == BLACK ? bPValue++ : rPValue++);

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
			if (position[i][j]) {
				position[i][j] == BLACK ? (bPosValue += posScore[0][i][j]) : (rPosValue += posScore[0][i][j]);
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (0 <= x + i && x + i < 6 &&
							0 <= y + j && y + j < 6 &&
							position[x + i][y + j] == 0) {
							position[x + i][y + j] == BLACK ? bMValue++ : rMValue++;
						}
					}
				}
			}
		}
	}

	bArcValue = getArcValue(isBlackTurn);
	rArcValue = getArcValue(!isBlackTurn);

	return 0;
}

int ChessBoard::value() {
	bValue = bNum * 6 + bMValue * 1 + bAValue * 2 + bPosValue * 1 + bArcValue * 1;//abcd�ֱ��ʾ�����������ص�Ȩֵ
	rValue = rNum * 6 + rMValue * 1 + rAValue * 2 + rPosValue * 1 + rArcValue * 1;

	return isBlackTurn ? bValue - rValue : rValue - bValue;
}

void ChessBoard::boardIteration(void(*pointProc)(int, int), void(*arcPointProc)(bool,int), void(*stepProc)(int, int, int, int), void(*arcStepProc)(bool,int, int)) {
	for (int arc = INNER; arc <= OUTER; arc++) {
		for (int i = 0; i < 24; i++) {
			arcPointProc(arc == OUTER, i);
			if (*loop[arc][i] && loopStart[arc] == -1) {
				loopStart[arc] = i;
			}
		}
	}

	int s, e = -1, colorS, colorE;//����±꣬�յ��±꣬�����ɫ���յ���ɫ
	for (int arc = INNER; arc <= OUTER; arc++) {//���⻡��ѭ��һ��
		if (loopStart[arc] != -1) { //�жϵ�ǰ�����е�
			for (int dir = 1; dir <= 23; dir += 22) { // ˳ʱ��/��ʱ��
				s = loopStart[arc];
				e = (s + dir) % 24;
				colorS = *loop[arc][s];//���������ɫ������ÿգ��;ɺ���˼·һ��
				*loop[arc][s] = 0;
				for (int i = 0; i < 24; i++, e = (e + dir) % 24) {
					colorE = *loop[arc][e];
					if (*loop[arc][e]) {
						arcStepProc(arc == OUTER, s, e); //
					}
				}
				if (!*loop[arc][s]) *loop[arc][s] = colorS;
			}
		}
	}

	//���������ŷ�
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			pointProc(i, j);
			if (position[i][j]) {
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						if (0 <= x + i && x + i < 6 &&
							0 <= y + j && y + j < 6 &&
							position[x + i][y + j] == 0) {
							stepProc(i, j, x + i, y + j);

						}
					}
				}
			}
		}
	}
}

void ChessBoard::move(int fX, int fY, int tX, int tY) {
	CHESSMOVE move(
		fX, fY, tX, tY,
		isBlackTurn ? BLACK : RED,
		!position[tX][tY]);
	this->move(move);
	isBlackTurn = !isBlackTurn; 
}

void ChessBoard::move(CHESSMOVE move) {
	BYTE sColor = position[move.From.x][move.From.y];
	BYTE eColor = position[move.To.x][move.To.y];
	if (eColor)
		eColor == BLACK ? bNum-- : rNum--;

	rawId -= idList[move.From.x][move.From.y][sColor];
	rawId -= idList[move.To.x][move.To.y][eColor];
	rawId += idList[move.To.x][move.To.y][sColor];

	position[move.To.x][move.To.y] = position[move.From.x][move.From.y];
	position[move.From.x][move.From.y] = NOCHESS;

	isBlackTurn = !isBlackTurn;
}

void ChessBoard::unMove() {
	CHESSMOVE move = moves.top();
	moves.pop();

	BYTE sColor = move.Side ? BLACK : RED;
	BYTE eColor = move.isMove ? 0 : 3-sColor;

	rawId -= idList[move.To.x][move.To.y][sColor];
	rawId += idList[move.From.x][move.From.y][sColor];
	if (eColor) {
		eColor == BLACK ? bNum++ : rNum++;
		rawId += idList[move.To.x][move.To.y][eColor];
	}

	position[move.To.x][move.To.y] = eColor;
	position[move.From.x][move.From.y] = sColor;

	isBlackTurn = !isBlackTurn;
}

ID_TYPE ChessBoard::getId() {
	return isBlackTurn ? getIdRaw()|SIG_BLACK : getIdRaw();
}

ID_TYPE ChessBoard::getIdRaw() {
	return rawId;
}

int ChessBoard::finishedMoves() {
	return moves.size();
}

int ChessBoard::isGameOver() {
	if (bNum&&rNum) 
		return false;
	return bNum ? BLACK : RED;
}

int ChessBoard::getPValue(bool isBlack) {
	return isBlack ? bPValue : rPValue;
}

int ChessBoard::getAValue(bool isBlack) {
	return isBlack ? bAValue : rAValue;
}

int ChessBoard::getMValue(bool isBlack) {
	return isBlack ? bMValue : rMValue;
}

int ChessBoard::getPosValue(bool isBlack) {
	return isBlack ? bPosValue : rPosValue;
}

int ChessBoard::getNums(bool isBlack) {
	return isBlack ? bNum : rNum;
}

int ChessBoard::getArcValue(bool isBlack)
{
	int BArcNum = 0, RArcNum = 0, NoArcNum = 0;
	int i;
	for (i = 1; i < 5; i++) {
		if (position[i][0] == 1) {
			BArcNum++;
		} else if (position[i][0] == 2) {
			RArcNum++;
		} else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (position[0][i] == 1) {
			BArcNum++;
		} else if (position[0][i] == 2) {
			RArcNum++;
		} else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (position[i][5] == 1) {
			BArcNum++;
		} else if (position[i][5] == 2) {
			RArcNum++;
		} else {
			NoArcNum++;
		}
	}
	for (i = 1; i < 5; i++) {
		if (position[5][i] == 1) {
			BArcNum++;
		} else if (position[5][i] == 2) {
			RArcNum++;
		} else {
			NoArcNum++;
		}
	}
	if (isBlackTurn) {
		return BArcNum * 5 + NoArcNum * 5;
	} else {
		return RArcNum * 5 + NoArcNum * 5;
	}
}

BYTE * ChessBoard::operator[](int x) {
	return position[x];
}

BYTE & ChessBoard::operator[](CHESSNAMPOS pos)
{
	return position[pos.x][pos.y];
}

inline BYTE& ChessBoard::pInner(int index){
	return *loop[INNER][index];
}

inline BYTE& ChessBoard::pOuter(int index)
{
	return *loop[OUTER][index];
}

inline BYTE& ChessBoard::pArc(int arc, int index) {
	return *loop[arc][index];
}
