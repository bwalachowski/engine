#ifndef POSITION_H
#define POSITION_H

#include "board.h"

class Position{
    Board board;
    uint64_t enPassantSquare;
    Board::PieceEnum currentPlayer;
    
    public:
        

        Position(Board board, Board::PieceEnum currentPlayer): board(board), currentPlayer(currentPlayer) {};

        uint64_t getPieceSet(Board::PieceEnum color, Board::PieceEnum pieceType) {return board.getPieceSet(color, pieceType);}
        uint64_t getPieceSet(Board::PieceEnum i) {return board.getPieceSet(i);}
        uint64_t getAllPieces() {return board.getAllPieces();}
        
        uint64_t getEnPassantSquare() {return enPassantSquare;}
        Board::PieceEnum getCurrentPlayer() {return currentPlayer;}
        Board::PieceEnum getOtherPlayer() {return (currentPlayer==Board::white) ? Board::black : Board::white;}
};
#endif  // #ifndef POSITION_H