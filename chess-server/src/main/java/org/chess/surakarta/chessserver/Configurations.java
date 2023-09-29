package org.chess.surakarta.chessserver;

import org.chess.surakarta.chessai.action.MoveGenerator;
import org.chess.surakarta.chessai.value.BasicChessABPruneEvaluator;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class Configurations {
    @Bean
    public MoveGenerator moveGenerator() {
        MoveGenerator generator = new MoveGenerator();
        generator.setPrunEvaluator(new BasicChessABPruneEvaluator(4));
        return generator;
    }
}
