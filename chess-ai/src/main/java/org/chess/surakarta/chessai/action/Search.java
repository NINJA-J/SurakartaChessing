package org.chess.surakarta.chessai.action;

import org.chess.surakarta.chessai.entity.Board;
import org.chess.surakarta.chessai.entity.Move;

public interface Search {
    Move bestMove(Board board, boolean isBlack);
}
