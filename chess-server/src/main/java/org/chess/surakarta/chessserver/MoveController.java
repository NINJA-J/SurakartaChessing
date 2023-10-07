package org.chess.surakarta.chessserver;

import lombok.Builder;
import lombok.Data;
import org.chess.surakarta.chessai.action.MoveGenerator;
import org.chess.surakarta.chessai.entity.Board;
import org.chess.surakarta.chessai.entity.Move;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import java.util.*;

@Controller
@CrossOrigin
@RequestMapping("/move")
public class MoveController {

    private final Map<String, Board> boardSessionMap = new HashMap<>();

    private final Map<String, List<Move>> boardMovesSessionMap = new HashMap<>();

    @Autowired
    private MoveGenerator generator;

    @RequestMapping(value = "/board", method = RequestMethod.POST)
    public @ResponseBody Response<ResponseInfo> bestMove(@RequestBody RequestInfo req) {
        Board board = new Board(req.rawBoard, req.isPlayerTurn);
        Move m = generator.bestMove(board);
        board.move(m);
        return Response.ok(
                ResponseInfo.builder()
                        .compMove(m)
                        .nextAvailableMoves(generator.availableMove(board))
                        .build()
        );
    }

    @RequestMapping("/session/register")
    @ResponseBody
    public Response<ResponseInfo> registerSession(@RequestBody RequestInfo req) {
        String id = UUID.randomUUID().toString();
        if (boardSessionMap.containsKey(id)) {
            return Response.fail(1, "UUID exist");
        }

        Board chessBoard = new Board(req.rawBoard, req.isPlayerTurn);
        boardSessionMap.put(id, chessBoard);
        return Response.ok(
                ResponseInfo.builder()
                        .sessionId(id)
                        .nextAvailableMoves(generator.availableMove(chessBoard))
                        .build()
        );
    }

    @RequestMapping("/session/{sid}/move")
    public Response<ResponseInfo> playerMove(@PathVariable("sid") String sessionId, @RequestBody RequestInfo req) {
        Board board = boardSessionMap.get(sessionId);
        if (board == null) {
            return Response.fail(1, "Invalid session ID");
        }
        if (!generator.availableMove(board).contains(req.playerMove)) {
            return Response.fail(2, "Invalid move");
        }
        List<Move> moves = boardMovesSessionMap.computeIfAbsent(sessionId, k -> new ArrayList<>());

        moves.add(req.playerMove);
        board.move(req.playerMove);

        Move m = null;
        if (req.doComputerMove) {
            m = generator.bestMove(board);
            board.move(m);
            moves.add(m);
        }

        return Response.ok(
                ResponseInfo.builder()
                        .compMove(m)
                        .nextAvailableMoves(generator.availableMove(board))
                        .build()
        );
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
        board.unmove(moves.remove(moves.size() - 1));
        return Response.ok(null);
    }

    @Data
    @Builder
    private static class ResponseInfo {
        private String sessionId;

        private Move compMove;

        private List<Move> nextAvailableMoves;
    }

    @Data
    private static class RequestInfo {
        private Move playerMove;

        private int[][] rawBoard;

        private Boolean isPlayerTurn;

        private boolean doComputerMove = true;
    }
}
