package org.chess.surakarta.value;

import org.chess.surakarta.chessai.value.AlphaBetaPruningEvaluator;
import org.junit.Assert;
import org.junit.Test;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

public class AlphaBetaPruningEvaluatorTest {

    @Test
    public void alphaBeta() {
        BTreeABPruningEval eval = new BTreeABPruningEval(new HashMap<Integer, Object>() {{
            put(0, Arrays.asList(1, 2));
            put(1, Arrays.asList(3, 4));
            put(2, Arrays.asList(5, 6));
            put(3, Arrays.asList(3, 5));
            put(4, Arrays.asList(6, 9));
            put(5, Arrays.asList(1, 2));
            put(6, Arrays.asList(0, -1));
        }});
        Assert.assertEquals(5, eval.value(new int[]{0}));
    }

    private static class BTreeABPruningEval extends AlphaBetaPruningEvaluator<int[], int[]> {
        public BTreeABPruningEval(Map<Integer, Object> tree) {
            super(3);
            this.tree = tree;
        }

        private final Map<Integer, Object> tree;


        @Override
        protected void makeMove(int[] context, int[] move) {
            context[0] = move[1];
        }

        @Override
        protected void unmakeMove(int[] context, int[] move) {
            context[0] = move[0];
        }

        @Override
        protected List<int[]> generateMoves(int[] context) {
            return ((List<Integer>) tree.get(context[0])).stream().map(v -> new int[]{context[0], v}).collect(Collectors.toList());
        }

        @Override
        protected int evaluateLeaf(int[] context) {
            return context[0];
        }
    }
}