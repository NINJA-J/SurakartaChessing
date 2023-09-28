package org.chess.surakarta.chessai.value;

import org.chess.surakarta.chessai.action.MoveGenerator;
import org.chess.surakarta.chessai.entity.Board;
import org.chess.surakarta.chessai.entity.Move;

import java.util.List;

public class BasicChessABPruneEvaluator extends AlphaBetaPruningEvaluator<Board, Move>{

    private final MoveGenerator generator = new MoveGenerator();

    private final Evaluator<Board> evaluator = new BoardEvaluator();

    public BasicChessABPruneEvaluator(int maxDepth) {
        super(maxDepth);
    }

    @Override
    protected void makeMove(Board context, Move move) {
        context.move(move);
    }

    @Override
    protected void unmakeMove(Board context, Move move) {
        context.unmove(move);
    }

    @Override
    protected List<Move> generateMoves(Board context) {
        return generator.availableMove(context);
    }

    @Override
    protected int evaluateLeaf(Board context) {
        return evaluator.value(context);
    }


}
