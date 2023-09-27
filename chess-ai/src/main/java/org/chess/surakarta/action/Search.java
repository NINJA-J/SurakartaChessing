package org.chess.surakarta.action;

import org.chess.surakarta.entity.Board;
import org.chess.surakarta.entity.Move;

public interface Search {
    Move bestMove(Board board, boolean isBlack);
}
