#ifndef POSITION_H
#define POSITION_H

#include "board.h"

class Position{
    Board board;
    
    public:
        Board::PieceEnum current_player;

        Position(Board board, Board::PieceEnum current_player): board(board), current_player(current_player) {};

        uint64_t getPieceSet(Board::PieceEnum color, Board::PieceEnum pieceType) {return board.getPieceSet(color, pieceType);}
        uint64_t getPieceSet(Board::PieceEnum i) {return board.getPieceSet(i);}
        uint64_t getAllPieces() {return board.getAllPieces();}
};
#endif  // #ifndef POSITION_H