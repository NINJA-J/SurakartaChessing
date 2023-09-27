package org.chess.surakarta.value;

import org.chess.surakarta.action.MoveGenerator;
import org.chess.surakarta.entity.Board;
import org.chess.surakarta.entity.Move;

public class BoardEvaluator implements Evaluator<Board> {
    private static final int[][] DEFAULT_BOARD_VALUE = new int[][]{
            {5, 20, 20, 20, 20, 5},
            {20, 30, 50, 50, 30, 20},
            {20, 50, 40, 40, 50, 50},
            {20, 50, 40, 40, 50, 20},
            {20, 30, 50, 50, 30, 20},
            {5, 20, 20, 20, 20, 5}
    };

    private final MoveGenerator moveGenerator = new MoveGenerator();

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
    public int value(Board board) {
        int posValue = 0, moveValue = 0, attachValue = 0, arcValue = 0;
        int numValue = board.getCount(Board.BLACK) - board.getCount(Board.RED);

        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                int c = board.getPos(i, j);
                int w = c - (c / 2) * 3; // 0>0,1>1,2>-1
                posValue += w * bScore[i][j];
                if (i == 0 || i == 5 || j == 0 || j == 5) {
                    arcValue += w;
                }
            }
        }

        for (Move m : moveGenerator.availableMove(board, Board.BLACK)) {
            if (m.isArc()) {
                attachValue++;
            } else {
                moveValue++;
            }
        }
        for (Move m : moveGenerator.availableMove(board, Board.RED)) {
            if (m.isArc()) {
                attachValue--;
            } else {
                moveValue--;
            }
        }

        int value = numValue * 6 + moveValue + attachValue * 2 + posValue + arcValue;
        return self == Board.BLACK ? value : -value;
    }
}
