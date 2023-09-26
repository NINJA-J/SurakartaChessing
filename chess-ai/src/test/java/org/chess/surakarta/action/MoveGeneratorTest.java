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
        }), Board.BLACK);
        moves.stream().
        Assert.assertEquals(Collections.li moves, Arrays.asList(
                Move.as(1, 4, 0, 4, true),
                Move.as(3, 4, 4, 5, true),
                Move.as(4, 2, 5, 1, true),
                Move.as(1, 1, 5, 1, true)
        ));
    }
}