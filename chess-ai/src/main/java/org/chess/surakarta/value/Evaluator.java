package org.chess.surakarta.value;

public interface Evaluator<C> {
    int value(C board);
}
