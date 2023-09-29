package org.chess.surakarta.chessserver;

import lombok.AllArgsConstructor;
import lombok.Data;

@Data
@AllArgsConstructor
public class Response<D> {
    private D data;

    private int code;

    private String message;

    public static <D> Response<D> ok(D resp) {
        return new Response<D>(resp, 0, "");
    }

    public static <D> Response<D> fail(int code, String message) {
        return new Response<D>(null, code, message);
    }
}
