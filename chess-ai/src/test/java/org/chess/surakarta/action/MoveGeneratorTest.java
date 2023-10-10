package org.chess.surakarta.action;

import org.chess.surakarta.chessai.action.MoveGenerator;
import org.chess.surakarta.chessai.entity.Board;
import org.chess.surakarta.chessai.entity.Move;
import org.junit.Assert;
import org.junit.Test;

import java.util.List;

public class MoveGeneratorTest {

    private MoveGenerator generator = new MoveGenerator();

    @org.junit.Before
    public void setUp() throws Exception {
    }

    @org.junit.After
    public void tearDown() throws Exception {
    }

    @Test
    public void availableMove() {
        List<Move> moves = generator.availableMove(new Board(new int[][] {
                {0, 0, 0, 0, 2, 0},
                {0, 1, 0, 1, 1, 0},
                {0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 1, 0},
                {0, 0, 1, 2, 0, 2},
                {2, 2, 2, 2, 0, 2}
        }, true));
        Assert.assertEquals(moves.get(0), Move.arc(1, 4, 0, 4, true));
        Assert.assertEquals(moves.get(1), Move.arc(3, 4, 4, 5, true));
        Assert.assertEquals(moves.get(2), Move.arc(4, 2, 5, 1, true));
        Assert.assertEquals(moves.get(3), Move.arc(1, 1, 5, 1, true));
        Assert.assertEquals(moves.get(4), Move.arc(3, 4, 5, 3, true));
        Assert.assertEquals(moves.get(5), Move.arc(3, 4, 5, 2, true));
        Assert.assertFalse(moves.get(6).isArc());
    }

    @Test
    public void availableMove2() {
        List<Move> moves = generator.availableMove(new Board(new int[][] {
                {0, 2, 0, 2, 2, 2},
                {2, 2, 2, 2, 2, 2},
                {2, 2, 0, 0, 0, 0},
                {0, 0, 1, 1, 1, 1},
                {1, 1, 0, 1, 1, 0},
                {1, 1, 0, 1, 1, 0}
        }, false));
        System.out.println(moves);
//        Assert.assertEquals(moves.get(0), Move.as(1, 4, 0, 4, true));
//        Assert.assertEquals(moves.get(1), Move.as(3, 4, 4, 5, true));
//        Assert.assertEquals(moves.get(2), Move.as(4, 2, 5, 1, true));
//        Assert.assertEquals(moves.get(3), Move.as(1, 1, 5, 1, true));
//        Assert.assertEquals(moves.get(4), Move.as(3, 4, 5, 3, true));
//        Assert.assertEquals(moves.get(5), Move.as(3, 4, 5, 2, true));
//        Assert.assertFalse(moves.get(6).isArc());
    }
}