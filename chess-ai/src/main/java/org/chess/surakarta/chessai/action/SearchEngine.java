package org.chess.surakarta.chessai.action;

import org.chess.surakarta.chessai.entity.Board;
import org.chess.surakarta.chessai.value.BoardEvaluator;
import org.chess.surakarta.chessai.value.Evaluator;
import org.chess.surakarta.chessai.entity.Move;

import java.util.List;

public class SearchEngine implements Search {
    private Evaluator evaluator = new BoardEvaluator();

    private MoveGenerator moveGenerator = new MoveGenerator();

    @Override
    public Move bestMove(Board board, boolean isBlack) {
        return null;
    }

    public Move searchBestMove(Board board, int side, int depth) {
        List<Move> moves = moveGenerator.availableMove(board);
        Move best = null;
        int maxVal = Integer.MIN_VALUE;
        for (Move m : moves) {
            board.move(m);
            int val = alphaBeta(board, 1, depth, side, Integer.MIN_VALUE, Integer.MAX_VALUE);
            board.unmove(m);
            if (val > maxVal) {
                best = m;
                maxVal = val;
            }
        }
        return best;
    }

    public int isGameOver(Board board) {
        if (board.getCount(Board.BLACK) == 0) {
            return Board.RED;
        } else if (board.getCount(Board.RED) == 0) {
            return Board.BLACK;
        } else {
            return Board.EMPTY;
        }
    }

    public int alphaBeta(Board board, int depth, int maxDepth, int side, int alpha, int beta) {
        if (depth == 0) { //在第一层时自动设定alpha和beta，因为这一层肯定是maxint和minint，减少searchagoodmove里的代码量
            alpha = Integer.MIN_VALUE;
            beta = Integer.MAX_VALUE;
        }
        //0 floor is max floor
        //depth of 0 floor = m_nMaxDepth
        boolean isMax = (maxDepth - depth) % 2 == 0;//判断当前层是不是max层
        //cur floor is black playing -- side = 1
        //cur floor is red   playing -- side = 0
        //0   floor is black playing -- m_Type = 1
        //depth of 0   floor = m_nMaxDepth
        //depth of cur floor = m_nMaxDepth - depth
        int curSide = (maxDepth - depth + side) % 2;//当前层谁走子
        int best = isMax ? Integer.MIN_VALUE : Integer.MAX_VALUE;//初始化搜索的估值的最值

        int winSide;
        if ((winSide = isGameOver(board)) != 0) {
            int winValue = Integer.MAX_VALUE - depth - 2;
            return winSide == curSide ? winValue : -winValue;
        }//终局
        if (depth <= 0) {
            return evaluator.value(board);//叶结点
        }

        List<Move> moves = moveGenerator.availableMove(board);

        for (Move move : moves) {
//            x++;
            board.move(move);
            int t = isMax ?
                    alphaBeta(board, depth + 1, maxDepth, side, best, Integer.MAX_VALUE) :
                    alphaBeta(board, depth + 1, maxDepth, side, Integer.MIN_VALUE, best);
            board.unmove(move);

            if (isMax) {
                if (best < t) { //获得更大的估值
                    alpha = best = t; //更新最值和alpha
                }
                if (best >= beta) {
                    return best; //无法使上层变小，剪枝
                }
            } else {
                if (best > t) {
                    beta = best = t;
                }
                if (best <= alpha) {
                    return best;
                }
            }
        }
        return best;//返回最值
    }
}
