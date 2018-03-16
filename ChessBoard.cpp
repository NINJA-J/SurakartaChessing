#include "stdafx.h"
#include "ChessBoard.h"
#include <string>

using namespace std;

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

void ChessBoard::initIdList() {
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
	setTurn(isBlackFirst);

	attachLoopList();
	chkLoopStart();

	moveList = NULL;
}

ChessBoard::ChessBoard(BYTE position[6][6], bool isBlackFirst) {
	setChessPosition(position);
	setTurn(isBlackFirst);

	attachLoopList();
	chkLoopStart();

	moveList = NULL;
}

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
	return true;
}

string ChessBoard::printBoard(char noChess, char black, char red) {
	string str;
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			switch (position[i][j]) {
			case NOCHESS:	str += noChess + ' '; break;
			case BLACK:		str += black + ' '; break;
			case RED:		str += red + ' '; break;
			default:		str += ". ";
			}
		}
		str += '\n';
	}
	return str;
}

bool ChessBoard::setTurn(bool isBlackTurn) {
	if (moves.size() > 0)return false;
	this->isBlackTurn = isBlackTurn;
	return true;
}

bool ChessBoard::getTurn() {
	return isBlackTurn;
}

int ChessBoard::getLoopStart(int arc) {
	return loopStart[arc];
}

void ChessBoard::chkLoopStart() {
	for (int arc = INNER; arc != OUTER; arc++) {
		for (int index = 0; index < 24; index++) {
			if (*loop[arc][index]) {
				loopStart[arc] = index;
				index = 24;
				break;
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
	moves.push(move);

	chkLoopStart();
}

void ChessBoard::cMove(CHESSMOVE move)
{
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
	cMoves.push(move);
	while (moves.size())moves.pop();
}

void ChessBoard::unMove() {
	CHESSMOVE move = moves.top();
	moves.pop();

	BYTE sColor = move.Side;
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

	chkLoopStart();
}

ID_TYPE ChessBoard::getId() {
	return isBlackTurn ? getIdRaw()|SIG_BLACK : getIdRaw();
}

ID_TYPE ChessBoard::getIdRaw() {
	return rawId;
}

int ChessBoard::getMoves() {
	return moves.size();
}

int ChessBoard::isGameOver() {
	if (bNum&&rNum) 
		return 0;
	return bNum ? B_WIN : R_WIN;
}

int ChessBoard::getNums(bool isBlack) {
	return isBlack ? bNum : rNum;
}

int ChessBoard::getSearchMoves() {
	return moves.size();
}

void ChessBoard::getPosition(BYTE pos[][6]) {
	memcpy(pos, position, sizeof(BYTE) * 36);
}

BYTE * ChessBoard::operator[](int x) {
	return position[x];
}

BYTE & ChessBoard::operator[](CHESSNAMPOS pos) {
	return position[pos.x][pos.y];
}

BYTE& ChessBoard::operator()(int arc, int index) {
	return *loop[arc][index];
	// TODO: 在此处插入 return 语句
}

BYTE& ChessBoard::pInner(int index){
	return *loop[INNER][index];
}

BYTE& ChessBoard::pOuter(int index) {
	return *loop[OUTER][index];
}

BYTE& ChessBoard::pArc(int arc, int index) {
	return *loop[arc][index];
}
