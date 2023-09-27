package org.chess.surakarta.value;

import java.util.List;

public abstract class AlphaBetaPruningEvaluator<C, T> implements Evaluator<C> {


    @Override
    public int value(C board) {
        return 0;
    }

    public abstract List<T> generateNodes(C context);

    public abstract void forward(C context, T move);

    public abstract void backward(C context, T move);

    public abstract void evaluateLeaf(C context);

    public int alphaBeta(C context, int depth, int maxDepth, boolean selfTurn, int alpha, int beta) {
        boolean isMax = depth % 2 == 0;
        boolean curSelfTurn = isMax == selfTurn;
    }

}
