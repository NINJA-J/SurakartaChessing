package org.chess.surakarta.chessai.entity;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Objects;

public class Move {
    private final Pos from;

    private final Pos to;

    private final boolean arc;

    public static Move arc(int[] from, int[] to) {
        return arc(from[0], from[1], to[0], to[1], true);
    }

    public static Move move(int[] from, int[] to) {
        return arc(from[0], from[1], to[0], to[1], false);
    }

    public static Move arc(int xFrom, int yFrom, int xTo, int yTo, boolean arc) {
        new ArrayList<>().subList()
        return new Move(
                new Pos(xFrom, yFrom),
                new Pos(xTo, yTo),
                arc
        );
    }

    public Move(Pos from, Pos to, boolean arc) {
        this.from = from;
        this.to = to;
        this.arc = arc;
    }

    public Pos getFrom() {
        return from;
    }

    public Pos getTo() {
        return to;
    }

    public boolean isArc() {
        return arc;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Move move = (Move) o;
        return arc == move.arc && Objects.equals(from, move.from) && Objects.equals(to, move.to);
    }

    @Override
    public int hashCode() {
        return Objects.hash(from, to, arc);
    }

    @Override
    public String toString() {
        return "Move{" +
                "from=" + from +
                ", to=" + to +
                ", arc=" + arc +
                '}';
    }
}
