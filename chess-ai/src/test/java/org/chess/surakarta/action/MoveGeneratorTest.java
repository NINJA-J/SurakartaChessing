package org.chess.surakarta.action;

import org.chess.surakarta.entity.Board;
import org.chess.surakarta.entity.Move;
import org.junit.Assert;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import static org.junit.Assert.*;

public class MoveGeneratorTest {

    private MoveGenerator generator = new MoveGenerator();

    @org.junit.Before
    public void setUp() throws Exception {
    }

    @org.junit.After
    public void tearDown() throws Exception {
    }

    @org.junit.Test
    public void availableMove() {
        List<Move> moves = generator.availableMove(new Board(new int[][] {
                {0, 0, 0, 0, 2, 0},
                {0, 1, 0, 1, 1, 0},
                {0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 1, 0},
                {0, 0, 1, 2, 0, 2},
                {2, 2, 2, 2, 0, 2}
        }){{
            setTurn(true);
        }});
        Assert.assertEquals(moves.get(0), Move.as(1, 4, 0, 4, true));
        Assert.assertEquals(moves.get(1), Move.as(3, 4, 4, 5, true));
        Assert.assertEquals(moves.get(2), Move.as(4, 2, 5, 1, true));
        Assert.assertEquals(moves.get(3), Move.as(1, 1, 5, 1, true));
        Assert.assertEquals(moves.get(4), Move.as(3, 4, 5, 3, true));
        Assert.assertEquals(moves.get(5), Move.as(3, 4, 5, 2, true));
        Assert.assertFalse(moves.get(6).isArc());
    }
}