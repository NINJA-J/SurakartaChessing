package org.chess.surakarta.chessai.entity;

public class Board {

    public static final int EMPTY = 0;

    public static final int BLACK = 1;

    public static final int RED = 2;
    private final int[][] board = new int[6][6];

    private boolean black = true;

    public static final int[][] DEFAULT_INIT = new int[][]{
            {1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1},
            {0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0},
            {2, 2, 2, 2, 2, 2},
            {2, 2, 2, 2, 2, 2}
    };

    public Board() {
        this(DEFAULT_INIT, true);
    }

    public Board(int[][] board, boolean isBlackTurn) {
        for (int i = 0; i < 6; i++) {
            System.arraycopy(board[i], 0, this.board[i], 0, 6);
            for (int j = 0; j < 6; j++) {
                chessCount[board[i][j]]++;
            }
        }
        this.black = isBlackTurn;
    }

    public Board copy() {
        Board copy = new Board(this.board, this.black);
        copy.setTurn(this.isBlackTurn());
        return copy;
    }

    private final int[] chessCount = new int[3];


    public void move(Move m) {
        if (getPos(m.getFrom()) == EMPTY) {
            throw new RuntimeException("Invalid move from argument, from is empty");
        } else if (getPos(m.getTo()) == getPos(m.getFrom())) {
            throw new RuntimeException("Invalid move to argument, to is same color");
        } else {
            chessCount[board[m.getTo().getY()][m.getTo().getX()]]--;
            setPos(m.getTo(), getPos(m.getFrom()));
            setPos(m.getFrom(), EMPTY);
            black = !black;
        }
    }

    public void unmove(Move m) {
        setPos(m.getFrom(), getPos(m.getTo()));
        if (m.isArc()) {
            setPos(m.getTo(), 3 - getPos(m.getTo()));
        } else {
            setPos(m.getTo(), EMPTY);
        }
        black = !black;
    }

    public void setPos(Pos pos, int color) {
        board[pos.getX()][pos.getY()] = color;
    }

    public void setPos(int[] pos, int color) {
        board[pos[0]][pos[1]] = color;
    }

    public int getPos(Pos pos) {
        return board[pos.getX()][pos.getY()];
    }

    public int getPos(int x, int y) {
        return board[x][y];
    }

    public int getPos(int[] pos) {
        return board[pos[0]][pos[1]];
    }

    public int getCount(int color) {
        return chessCount[color];
    }

    public boolean isBlackTurn() {
        return black;
    }

    public int getSide() {
        return black ? BLACK : RED;
    }

    public void setTurn(boolean self) {
        this.black = self;
    }

}
