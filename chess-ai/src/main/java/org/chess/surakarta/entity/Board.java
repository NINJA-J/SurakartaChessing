package org.chess.surakarta.entity;

public class Board {

    public static final int EMPTY = 0;

    public static final int BLACK = 1;

    public static final int RED = 2;
    private final int board[][] = new int[6][6];

    public static final int[][] DEFAULT_INIT = new int[][]{
            {1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1},
            {0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0},
            {2, 2, 2, 2, 2, 2},
            {2, 2, 2, 2, 2, 2}
    };

    public Board() {
        this(DEFAULT_INIT);
    }

    public Board(int[][] board) {
        for (int i = 0; i < 6; i++) {
            System.arraycopy(board, 0, this.board, 0, 6);
            for (int j = 0; j < 6; j++) {
                chessCount[board[i][j]]++;
            }
        }
    }

    private final int[] chessCount = new int[3];


    public void move(Move m) {
        if (board[m.getFrom().getY()][m.getFrom().getX()] == EMPTY) {
            throw new RuntimeException("Invalid move from argument, from is empty");
        } else if (board[m.getTo().getY()][m.getTo().getX()] == board[m.getFrom().getY()][m.getFrom().getX()]) {
            throw new RuntimeException("Invalid move to argument, to is same color");
        } else {
            chessCount[board[m.getTo().getY()][m.getTo().getX()]]--;
            board[m.getTo().getY()][m.getTo().getX()] = board[m.getFrom().getY()][m.getFrom().getX()];
            board[m.getFrom().getY()][m.getFrom().getX()] = EMPTY;
        }
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

}
