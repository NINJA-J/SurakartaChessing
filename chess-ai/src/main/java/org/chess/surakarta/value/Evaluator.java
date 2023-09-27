package org.chess.surakarta.value;

import org.chess.surakarta.entity.Board;

public interface Evaluator {
    int value(Board board, int side);
}
