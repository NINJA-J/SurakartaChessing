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
	static const int COLOR = 0;//循环链表末3项对应的含义（主要增加可读性）
	static const int LAST = 1;
	static const int NEXT = 2;

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

	int s, e = -1, colorS, colorE;//起点下标，终点下标，起点颜色，终点颜色
	for (int arc = INNER; arc <= OUTER; arc++) {//内外弧各循环一次
		if (loopStart[arc] != -1) { //判断当前弧上有点
			for (int dir = 1; dir <= 23; dir += 22) { // 顺时针/逆时针
				s = loopStart[arc];
				e = (s + dir) % 24;
				colorS = *loop[arc][s];//保留起点颜色，起点置空，和旧函数思路一样
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
						*loop[arc][s] = colorS;//恢复起点颜色，继续搜索
						s = e; //终点置为新起点
						colorS = *loop[arc][s];//保留起点颜色，起点置空，和旧函数思路一样
						*loop[arc][s] = 0;
					}
				}
				if (!*loop[arc][s]) *loop[arc][s] = colorS;
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

	int s, e = -1, colorS, colorE;//起点下标，终点下标，起点颜色，终点颜色
	for (int arc = INNER; arc <= OUTER; arc++) {//内外弧各循环一次
		if (loopStart[arc] != -1) { //判断当前弧上有点
			for (int dir = 1; dir <= 23; dir += 22) { // 顺时针/逆时针
				s = loopStart[arc];
				e = (s + dir) % 24;

				colorS = *loop[arc][s];//保留起点颜色，起点置空，和旧函数思路一样
				*loop[arc][s] = 0;

				for (int i = 0; i < 24; i++, e = (e + dir) % 24) {
					colorE = *loop[arc][e];
					if (*loop[arc][e]) {
						if (s / 6 != e / 6)
							colorS == colorE ?
							(colorS == BLACK ? bAValue++ : rAValue++) :
							(colorS == BLACK ? bPValue++ : rPValue++);

						*loop[arc][s] = colorS;//恢复起点颜色，继续搜索
						s = e; //终点置为新起点
						colorS = *loop[arc][s];//保留起点颜色，起点置空，和旧函数思路一样
						*loop[arc][s] = 0;
					}
				}
				if (!*loop[arc][s]) *loop[arc][s] = colorS;
			}
		}
	}

	//生成走子着法
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
	bValue = bNum * 6 + bMValue * 1 + bAValue * 2 + bPosValue * 1 + bArcValue * 1;//abcd分别表示各个评分因素的权值
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

	int s, e = -1, colorS, colorE;//起点下标，终点下标，起点颜色，终点颜色
	for (int arc = INNER; arc <= OUTER; arc++) {//内外弧各循环一次
		if (loopStart[arc] != -1) { //判断当前弧上有点
			for (int dir = 1; dir <= 23; dir += 22) { // 顺时针/逆时针
				s = loopStart[arc];
				e = (s + dir) % 24;
				colorS = *loop[arc][s];//保留起点颜色，起点置空，和旧函数思路一样
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

	//生成走子着法
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
