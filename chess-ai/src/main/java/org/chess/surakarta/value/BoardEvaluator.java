package org.chess.surakarta.value;

import org.chess.surakarta.entity.Board;

public class BoardEvaluator implements Evaluator {
    private static final int[][] DEFAULT_BOARD_VALUE = new int[][]{
        {5, 20, 20, 20, 20, 5},
        {20, 30, 50, 50, 30, 20},
        {20, 50, 40, 40, 50, 50},
        {20, 50, 40, 40, 50, 20},
        {20, 30, 50, 50, 30, 20},
        {5, 20, 20, 20, 20, 5}
    };

    private final int[][] bScore = new int[6][6];

    public BoardEvaluator() {
        this(DEFAULT_BOARD_VALUE);
    }

    public BoardEvaluator(int[][] bScore) {
        for (int i = 0; i < 6; i++) {
            System.arraycopy(bScore[i], 0, this.bScore[i], 0, 6);
        }
    }

    @Override
    public int value(Board board, boolean isBlack) {
        return 0;
    }
}
