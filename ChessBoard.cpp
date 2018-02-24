#include "ChessBoard.h"

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

bool ChessBoard::setChessTurn(bool isBlackTurn) {
	if (moves.size() > 0)return false;
	this->isBlackTurn = isBlackTurn;
	return true;
}

int ChessBoard::getChessTurn() {
	return isBlackTurn;
}

inline int ChessBoard::getLoopStart(int arc) {
	return loopStart[arc];
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

inline int ChessBoard::isGameOver() {
	if (bNum&&rNum) 
		return 0;
	return bNum ? B_WIN : R_WIN;
}

inline int ChessBoard::getNums(bool isBlack) {
	return isBlack ? bNum : rNum;
}

inline int ChessBoard::getSearchMoves() {
	return moves.size();
}

inline bool ChessBoard::getTurn() {
	return isBlackTurn;
}
inline void ChessBoard::getPosition(BYTE pos[][6]) {
	memcmp(pos, position, sizeof(BYTE) * 36);
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

inline BYTE& ChessBoard::pOuter(int index) {
	return *loop[OUTER][index];
}

inline BYTE& ChessBoard::pArc(int arc, int index) {
	return *loop[arc][index];
}
