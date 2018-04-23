#include "stdafx.h"
#include "ChessBoard.h"
#include <string>
#include <iostream>

using namespace std;

//初始化棋盘的id，独一无二
void ChessBoard::initId() {
	rawId = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			rawId += idList[i][j][BLACK] + idList[i][j][position[i][j]];
		}
	}
}

//采用任务结构体对棋盘进行初始化
ChessBoard::ChessBoard(Task & t) {
	setChessPosition(t.position, t.isBlackTurn);
	rawId = t.boardId;

}

//采用默认情况初始化棋盘
ChessBoard::ChessBoard(bool isBlackFirst) {
	setChessPosition(defaultStartBoard,isBlackFirst);
	initId();
}

//采用正常传入棋盘初始化
ChessBoard::ChessBoard(BYTE position[6][6], bool isBlackFirst) {
	setChessPosition(position,isBlackFirst);
	initId();
}

//默认调用的拷贝构造函数
ChessBoard::ChessBoard(ChessBoard & copy) {
	setChessPosition(copy.position, copy.isBlackTurn);
	rawId = copy.rawId;
}
//正常初始化棋盘
bool ChessBoard::setChessPosition(const BYTE position[6][6], bool isBlackFirst) {
	bNum = rNum = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			this->position[i][j] = position[i][j];
			if (position[i][j] == BLACK) bNum++;
			if (position[i][j] == RED) rNum++;
		}
	}
	isBlackTurn = isBlackFirst;
	while (moves.size()) moves.pop();

	attachLoopList();
	chkLoopStart();

	return true;
}
//输出棋盘
void ChessBoard::printBoard(char* board, char noChess, char black, char red) {
	string str;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			switch (position[i][j]) {
			case NOCHESS:	str += noChess + " "; break;
			case BLACK:		str += black + " "; break;
			case RED:		str += red + " "; break;
			default:		str += ". ";
			}
		}
		str += "\n";
	}
	str += "\0";
	if (board) strcpy(board, str.c_str());
}
//控制谁先走
bool ChessBoard::setTurn(bool isBlackTurn) {
	if (moves.size() > 0)return false;
	this->isBlackTurn = isBlackTurn;
	return true;
}
//得到谁先走
bool ChessBoard::getTurn() {
	return isBlackTurn;
}
int ChessBoard::getLoopStart(int arc, int color) {
	return loopStart[arc][color];
}

inline void ChessBoard::chkLoopStart() {
	loopStart[INNER][BLACK] = loopStart[INNER][RED] = loopStart[INNER][NOCHESS] = -1;
	loopStart[OUTER][BLACK] = loopStart[OUTER][RED] = loopStart[OUTER][NOCHESS] = -1;
	for (int arc = INNER; arc <= OUTER; arc++) {
		int foundColor = 0;
		for (int index = 0; index < 24; index++) {
			if (*loop[arc][index]) {
				if (!foundColor) {
					foundColor = *loop[arc][index];
					loopStart[arc][NOCHESS] = loopStart[arc][foundColor] = index;
				}
				else if (*loop[arc][index] == 3 - foundColor) {
					loopStart[arc][3 - foundColor] = index;
					loopStart[arc][NOCHESS] = min(loopStart[arc][BLACK], loopStart[arc][RED]);
					break;
				}
			}
		}
	}
}

void ChessBoard::attachLoopList() {
	for (int i = 0; i < 24; i++) {
		for (int arc = INNER; arc <= OUTER; arc++) {
			loop[arc][i] = &(this->position[arcLoop[arc][i][PX]][arcLoop[arc][i][PY]]);
		}
	}
}
//棋盘的移动
void ChessBoard::move(int fX, int fY, int tX, int tY) {
	CHESSMOVE move(
		fX, fY, tX, tY,
		isBlackTurn ? BLACK : RED,
		!position[tX][tY]);
	this->move(move);
	isBlackTurn = !isBlackTurn;
}
//重载 棋盘的移动
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
	moves.push(move);

	chkLoopStart();
}
//返回移动前一步棋盘的状态
void ChessBoard::unMove() {
	CHESSMOVE move = moves.top();
	moves.pop();

	BYTE sColor = move.Side;
	BYTE eColor = move.isMove ? 0 : 3 - sColor;

	rawId -= idList[move.To.x][move.To.y][sColor];
	rawId += idList[move.From.x][move.From.y][sColor];
	if (eColor) {
		eColor == BLACK ? bNum++ : rNum++;
		rawId += idList[move.To.x][move.To.y][eColor];
	}

	position[move.To.x][move.To.y] = eColor;
	position[move.From.x][move.From.y] = sColor;

	isBlackTurn = !isBlackTurn;

	chkLoopStart();
}
//由走方得到棋盘的id
ID_TYPE ChessBoard::getId() {
	return isBlackTurn ? getIdRaw() | SIG_BLACK : getIdRaw();
}
//正常得到id
ID_TYPE ChessBoard::getIdRaw() {
	return rawId;
}
//直接计算出棋盘id，没有优化的情况下
ID_TYPE ChessBoard::getIdNormal() {
	ID_TYPE id = 0;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			id += idList[i][j][BLACK];
			id += idList[i][j][position[i][j]];
		}
	}
	return id;
}
//存储一共走了多少步，move为栈
int ChessBoard::getMoves() {
	return moves.size();
}
//判断游戏是否结束
int ChessBoard::isGameOver() {
	if (bNum&&rNum)
		return 0;
	return bNum ? B_WIN : R_WIN;//两个全局变量
}
//得到棋盘红黑子的个数
int ChessBoard::getNums(bool isBlack) {
	return isBlack ? bNum : rNum;
}
//存储一共走了多少步，move为栈
int ChessBoard::getSearchMoves() {
	return moves.size();
}
//将棋局形势拷贝至pos
void ChessBoard::getPosition(BYTE pos[][6]) {
	memcpy(pos, position, sizeof(BYTE) * 36);
}
//输出棋局形势
void ChessBoard::outputPosition()
{
	CString temp;
	temp.Format(
		"%d %d %d %d %d %d\n%d %d %d %d %d %d\n%d %d %d %d %d %d\n%d %d %d %d %d %d\n%d %d %d %d %d %d\n%d %d %d %d %d %d",position[0][0],position[0][1],position[0][2],position[0][3],position[0][4],position[0][5], position[1][0], position[1][1], position[1][2], position[1][3], position[1][4], position[1][5], position[2][0], position[2][1], position[2][2], position[2][3], position[2][4], position[2][5], position[3][0], position[3][1], position[3][2], position[3][3], position[3][4], position[3][5], position[4][0], position[4][1], position[4][2], position[4][3], position[4][4], position[4][5], position[5][0], position[5][1], position[5][2], position[5][3], position[5][4], position[5][5]);
	AfxMessageBox(temp);
}
//返回棋盘的行指针
BYTE * ChessBoard::operator[](int x) {
	return position[x];
}
//返回某点的棋的类型
BYTE & ChessBoard::operator[](CHESSNAMPOS pos) {
	return position[pos.x][pos.y];
}

BYTE& ChessBoard::operator()(int arc, int index) {
	return *loop[arc][index];
	// TODO: ڴ˴ return 
}

BYTE& ChessBoard::pInner(int index) {
	return *loop[INNER][index];
}

BYTE& ChessBoard::pOuter(int index) {
	return *loop[OUTER][index];
}

BYTE& ChessBoard::pArc(int arc, int index) {
	return *loop[arc][index];
}
