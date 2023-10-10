package org.chess.surakarta.chessai.action;

import org.chess.surakarta.chessai.entity.Board;
import org.chess.surakarta.chessai.entity.Pos;
import org.chess.surakarta.chessai.entity.Move;
import org.chess.surakarta.chessai.value.Evaluator;

import java.util.*;

public class MoveGenerator {

    private Evaluator<Board> prunEvaluator = null;
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
                    {17, 16, 15, 14, 10, 12},
                    {-1, 18, -1, -1, 11, -1}
            },
            { //outerArc，外弧
                    {-1, -1, 23, 6, -1, -1},
                    {-1, -1, 22, 7, -1, -1},
                    {0, 1, 2, 3, 4, 5},
                    {17, 16, 15, 9, 13, 12},
                    {-1, -1, 19, 10, -1, -1},
                    {-1, -1, 18, 11, -1, -1}
            }
    };

    public static final int[][] arcIndexPair = new int[][]{
            {
                    -2, 22, -2, -2, 7, -2,
                    -2, 4, -2, -2, 13, -2,
                    -2, 10, -2, -2, 19, -2,
                    -2, 16, -2, -2, 1, -2
            },
            {
                    -2, -2, 21, 8, -2, -2,
                    -2, -2, 3, 14, -2, -2,
                    -2, -2, 9, 20, -2, -2,
                    -2, -2, 15, 2, -2, -2
            }
    };

    public List<Move> availableMove(Board board) {
        return availableMove(board, board.getSide());
    }

    public List<Move> availableMove(Board board, int side) {
        List<Move> moves = new ArrayList<>();
        for (int arc = 0; arc <= 1; arc++) {
            int arcS = 0, arcE;
            while (arcS < 24 && board.getPos(arcChain[arc][arcS]) == Board.EMPTY) {
                arcS++;
            }
            if (arcS == 24) {
                continue;
            }

            arcE = arcS + 1;
            while (arcE < 24 && board.getPos(arcChain[arc][arcE]) == Board.EMPTY) {
                arcE++;
            }
            if (arcE == 24) {
                continue;
            }

            int indL = arcS, indS = arcE, indE;
            // Start with L=arcS, S=arcE, E=arcE_1
            for (indE = indS + 1; indE < 24; indE++) {
                if (board.getPos(arcChain[arc][indE]) == Board.EMPTY) {
                    continue;
                }

                // return true if not on cross
                chkAddArcMove(board, side, moves, arc, indL, indS, indE);
                indL = indS;
                indS = indE;
            }
            chkAddArcMove(board, side, moves, arc, indL, indS, arcS);
            chkAddArcMove(board, side, moves, arc, indS, arcS, arcE);
            // if only 2 chess on arc, may exist same move
            if (moves.size() > 1 && moves.get(moves.size() - 1).equals(moves.get(moves.size() - 2))) {
                moves.remove(moves.size() - 1);
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

    public void setPrunEvaluator(Evaluator<Board> evaluator) {
        this.prunEvaluator = evaluator;
    }

    private void chkAddArcMove(Board board, int side, List<Move> moves, int arc, int indL, int indS, int indE) {
        // indL --- indS(cross) ^^^ indE[indS(cross)]
        // Check indL->indE
        int colorL = indL == -1 ? -1 : board.getPos(arcChain[arc][indL]);
        int colorS = board.getPos(arcChain[arc][indS]);
        int colorE = board.getPos(arcChain[arc][indE]);

        if (isSamePoint(indS, indE, arc)) {
            if (colorL + colorE != 3) {
                return;
            }
            if (colorL != side) {
                moves.add(Move.arc(arcChain[arc][indS], arcChain[arc][indL]));
            } else if (indL / 6 != indS / 6 || indL > indS) {
                moves.add(Move.arc(arcChain[arc][indL], arcChain[arc][indS]));
            }
        } else if (isSamePoint(indL, indS, arc)) {
            if (colorL + colorE != 3) {
                return;
            }
            if (colorL == side) {
                moves.add(Move.arc(arcChain[arc][indL], arcChain[arc][indE]));
            } else if (indS / 6 != indE / 6 || indS > indE) {
                moves.add(Move.arc(arcChain[arc][indE], arcChain[arc][indL]));
            }
        } else if (colorS + colorE == 3 && (indS / 6 != indE / 6 || indS > indE)) {
            if (colorS == side) {
                moves.add(Move.arc(arcChain[arc][indS], arcChain[arc][indE]));
            } else {
                moves.add(Move.arc(arcChain[arc][indE], arcChain[arc][indS]));
            }
        }
    }


    private boolean isSamePoint(int arcIndex1, int arcIndex2, int arc) {
        return arcIndexPair[arc][arcIndex2] == arcIndex1;
    }

    public Move bestMove(Board board) {
        List<Move> moves = availableMove(board);
        int min = Integer.MAX_VALUE;
        Move best = null;
        for (Move m : moves) {
            board.move(m);
            int v = prunEvaluator.value(board);
            board.unmove(m);

            if (min > v) {
                min = v;
                best = m;
            }
        }
        return best;
    }
}
