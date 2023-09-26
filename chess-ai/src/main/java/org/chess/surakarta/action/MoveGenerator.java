package org.chess.surakarta.action;

import org.chess.surakarta.entity.Board;
import org.chess.surakarta.entity.Move;
import org.chess.surakarta.entity.Pos;

import java.util.ArrayList;
import java.util.List;

public class MoveGenerator {
    public static final int[][][] arcChain = new int[][][]{
            {   //内弧
                    {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5},
                    {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4},
                    {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0},
                    {5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 1}, {0, 1}
            },
            { //外弧
                    {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5},
                    {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3},
                    {3, 5}, {3, 4}, {3, 3}, {3, 2}, {3, 1}, {3, 0},
                    {5, 2}, {4, 2}, {3, 2}, {2, 2}, {1, 2}, {0, 2}
            }
    };

    public static final int[][][] board2arcIndex = new int[][][]{
            { //innerArc，内弧
                    {-1, 23, -1, -1, 6, -1},
                    {0, 1, 2, 3, 4, 5},
                    {-1, 21, -1, -1, 8, -1},
                    {-1, 20, -1, -1, 9, -1},
                    {17, 16, 15, 14, 13, 12},
                    {-1, 18, -1, -1, 11, -1}
            },
            { //outerArc，外弧
                    {-1, -1, 23, 6, -1, -1},
                    {-1, -1, 22, 7, -1, -1},
                    {0, 1, 2, 3, 4, 5},
                    {17, 16, 15, 14, 13, 12},
                    {-1, -1, 19, 10, -1, -1},
                    {-1, -1, 18, 11, -1, -1}
            }
    };

    public List<Move> availableMove(Board board, int side) {
        List<Move> moves = new ArrayList<>();
        for (int arc = 0; arc <= 1; arc++) {
            int arcS = -1;
            for (int i = 0; i < 24; i++) {
                int[] p = arcChain[arc][i];
                if (board.getPos(p) != Board.EMPTY) {
                    arcS = i;
                    break;
                }
            }
            if (arcS == -1) {
                continue;
            }
            int indS = arcS;
            int colorS = board.getPos(arcChain[arc][arcS]), colorE;
            board.setPos(arcChain[arc][arcS], Board.EMPTY);
            for (int i = arcS+1; i < 24; i++) {
                colorE = board.getPos(arcChain[arc][i]);
                if (colorE == Board.EMPTY) {
                    continue;
                }
                if (colorS != colorE && indS / 6 != i / 6) {
                    if (colorS == side) {
                        moves.add(Move.as(arcChain[arc][indS], arcChain[arc][i], true));
                    } else {
                        moves.add(Move.as(arcChain[arc][i], arcChain[arc][indS], true));
                    }
                }
                board.setPos(arcChain[arc][indS], colorS);
                colorS = colorE;
                board.setPos(arcChain[arc][i], Board.EMPTY);
                indS = i;
            }
            if (arcS != indS) {
                colorS = board.getPos(arcChain[arc][arcS]);
                colorE = board.getPos(arcChain[arc][indS]);
                if (colorS != colorE && indS / 6 != arcS/6) {
                    if (colorS == side) {
                        moves.add(Move.as(arcChain[arc][arcS], arcChain[arc][indS], true));
                    } else {
                        moves.add(Move.as(arcChain[arc][indS], arcChain[arc][arcS], true));
                    }
                }
            }
        }
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                if (board.getPos(i, j) == side) {
                    for (int x = i - 1; x <= i + 1; x++) {
                        for (int y = j - 1; y <= j + 1; y++) {
                            if (0 <= x && x < 6 && 0 <= y && y < 6 &&
                                    board.getPos(x, y) == Board.EMPTY) {
                                moves.add(new Move(new Pos(i, j), new Pos(x, y), false));

                            }
                        }
                    }
                }
            }
        }
        return moves;
    }
}
