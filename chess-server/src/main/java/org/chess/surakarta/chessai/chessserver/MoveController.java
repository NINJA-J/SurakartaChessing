package org.chess.surakarta.chessai.chessserver;

import org.chess.surakarta.chessai.action.MoveGenerator;
import org.chess.surakarta.chessai.entity.Board;
import org.chess.surakarta.chessai.entity.Move;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.servlet.mvc.support.DefaultHandlerExceptionResolver;

import java.util.*;

@Controller
@RequestMapping("/move")
public class MoveController {

    private final Map<String, Board> boardSessionMap = new HashMap<>();

    private final Map<String, List<Move>> boardMovesSessionMap = new HashMap<>();

    @Autowired
    private MoveGenerator generator;

    @RequestMapping(value = "/board", method = RequestMethod.POST)
    public @ResponseBody Response<Move> nextMove(@RequestBody Board board) {
        return Response.ok(generator.bestMove(board));
    }

    @RequestMapping("/session/register")
    public Response<String> registerSession(int[][] board, boolean isBlackTurn) {
        String id = UUID.randomUUID().toString();
        if (boardSessionMap.containsKey(id)) {
            return Response.fail(1,"UUID exist");
        }
        boardSessionMap.put(id, new Board(board, isBlackTurn));
        return Response.ok(id);
    }

    @RequestMapping("/session/{sid}/move")
    public Response<Move> nextMove(@PathVariable("sid") String sessionId, Move move) {
        Board board = boardSessionMap.get(sessionId);
        if (board == null) {
            return Response.fail(1, "Invalid session ID");
        }
        List<Move> moves = boardMovesSessionMap.computeIfAbsent(sessionId, k -> new ArrayList<>());

        moves.add(move);
        board.move(move);

        Move m = generator.bestMove(board);
        board.move(m);
        moves.add(m);

        return Response.ok(m);
    }

    @RequestMapping("/session/{sid}/unmove")
    public Response<Void> unmove(@PathVariable("sid") String sessionId) {
        Board board = boardSessionMap.get(sessionId);
        if (board == null) {
            return Response.fail(1, "Invalid session ID");
        }

        List<Move> moves = boardMovesSessionMap.get(sessionId);
        if (moves == null || moves.size() == 0) {
            return Response.fail(2, "No moves to withdraw");
        }
        board.unmove(moves.remove(moves.size()-1));
        return Response.ok(null);
    }
}
