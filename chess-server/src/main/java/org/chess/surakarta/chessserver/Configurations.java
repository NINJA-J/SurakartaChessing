package org.chess.surakarta.chessserver;

import org.chess.surakarta.chessai.action.MoveGenerator;
import org.chess.surakarta.chessai.value.BasicChessABPruneEvaluator;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.web.servlet.config.annotation.CorsRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;

@Configuration
public class Configurations implements WebMvcConfigurer {
    @Bean
    public MoveGenerator moveGenerator() {
        MoveGenerator generator = new MoveGenerator();
        generator.setPrunEvaluator(new BasicChessABPruneEvaluator(4));
        return generator;
    }

    @Override
    public void addCorsMappings(CorsRegistry registry) {
        registry.addMapping("/**").allowedMethods("*");
    }
}
