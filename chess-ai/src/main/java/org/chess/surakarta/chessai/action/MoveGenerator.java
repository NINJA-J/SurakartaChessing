package org.chess.surakarta.chessai.action;

import org.chess.surakarta.chessai.entity.Board;
import org.chess.surakarta.chessai.entity.Pos;
import org.chess.surakarta.chessai.entity.Move;
import org.chess.surakarta.chessai.value.BasicChessABPruneEvaluator;
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

    public List<Move> availableMove(Board board) {
        return availableMove(board, board.getSide());
    }

    public List<Move> availableMove(Board board, int side) {
        List<Move> moves = new ArrayList<>();
        for (int arc = 0; arc <= 1; arc++) {
            int arcS;
            for (arcS = 0; arcS < 24; arcS++) {
                if (board.getPos(arcChain[arc][arcS]) != Board.EMPTY) {
                    break;
                }
            }
            // No points on arc
            if (arcS == 24) {
                continue;
            }
            int indL = -1, indS = arcS, indE;
            for (indE = arcS + 1; indE < 24; indE++) {
                if (board.getPos(arcChain[arc][indE]) == Board.EMPTY) {
                    continue;
                }

                // return true if not on cross
                if (chkAddArcMove(board, side, moves, arc, indL, indS, indE)) {
                    indL = indS;
                    indS = indE;
                }
            }
            chkAddArcMove(board, side, moves, arc, indL, indS, arcS);
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

    private boolean chkAddArcMove(Board board, int side, List<Move> moves, int arc, int indL, int indS, int indE) {
        // indL --- indS(cross) ^^^ indE[indS(cross)]
        // Check indL->indE
        if (isSamePoint(indS, indE, arc)) {
            chkAddMove(board, side, moves, arc, indL, indE, true);
            return false;
        } else {
            chkAddMove(board, side, moves, arc, indS, indE, false);
            return true;
        }
    }

    private void chkAddMove(Board board, int side, List<Move> moves, int arc, int indS, int indE, boolean cross) {
        int colorS = board.getPos(arcChain[arc][indS]);
        if (colorS != board.getPos(arcChain[arc][indE]) && indS / 6 != indE / 6) {
            if (colorS != side) {
                moves.add(Move.as(arcChain[arc][indE], arcChain[arc][indS], true));
            } else if (!cross) {
                moves.add(Move.as(arcChain[arc][indS], arcChain[arc][indE], true));
            }
        }
    }

    private boolean isSamePoint(int arcIndex1, int arcIndex2, int arc) {
        int[] a = arcChain[arc][arcIndex1];
        int[] b = arcChain[arc][arcIndex2];
        return board2arcIndex[arc][a[0]][a[1]] == board2arcIndex[arc][b[0]][b[1]];
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
