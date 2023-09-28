package org.chess.surakarta.chessai.value;

import java.util.List;

public abstract class AlphaBetaPruningEvaluator<C, T> implements Evaluator<C> {

    private static final int DEFAULT_INF = Integer.MAX_VALUE - 1;

    private int maxDepth;

    private int initAlpha;

    private int initBeta;

    public AlphaBetaPruningEvaluator(int maxDepth) {
        this(maxDepth, -DEFAULT_INF, DEFAULT_INF);
    }

    public AlphaBetaPruningEvaluator(int maxDepth, int alpha, int beta) {
        this.maxDepth = maxDepth;
        this.initAlpha = alpha;
        this.initBeta = beta;
    }

    @Override
    public int value(C board) {
        return alphaBeta(board, 0, maxDepth, initAlpha, initBeta);
    }

    protected abstract void makeMove(C context, T move);

    protected abstract void unmakeMove(C context, T move);

    protected abstract List<T> generateMoves(C context);

    protected abstract int evaluateLeaf(C context);

    protected void postProcess(C context, T move, int value) {
    }

    public int alphaBeta(C context, int depth, int maxDepth, int alpha, int beta) {
        int best;
        if (depth == maxDepth) {
            return evaluateLeaf(context);
        }
        if (depth % 2 == 0) {
            best = -DEFAULT_INF;
            for (T move : generateMoves(context)) {
                makeMove(context, move);
                int val = alphaBeta(context, depth + 1, maxDepth, alpha, beta);
                unmakeMove(context, move);
                postProcess(context, move, val);

                best = Math.max(best, val);
                alpha = Math.max(alpha, best);
                if (alpha >= beta) {
                    break;
                }
            }
        } else {
            best = DEFAULT_INF;
            for (T move : generateMoves(context)) {
                makeMove(context, move);
                int val = alphaBeta(context, depth + 1, maxDepth, alpha, beta);
                unmakeMove(context, move);
                postProcess(context, move, val);

                best = Math.min(best, val);
                beta = Math.min(beta, best);
                if (alpha >= beta) {
                    break;
                }
            }
        }
        return best;
    }

}
