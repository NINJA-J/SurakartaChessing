package org.chess.surakarta.chessai.value;

public interface Evaluator<C> {
    int value(C board);
}
