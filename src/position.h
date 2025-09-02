#ifndef POSITION_H
#define POSITION_H

#include "board.h"

class Position
{
    Board board;
    uint64_t enPassantSquare;
    Board::PieceEnum currentPlayer;
    uint8_t castlingRights; // last 4 bits, in order from last:  white short castle, white
                            // long castle, black short castle, black long castle

public:
    Position(Board board, Board::PieceEnum currentPlayer, uint8_t castlingRights)
        : board(board), currentPlayer(currentPlayer), castlingRights(castlingRights)
    {
        enPassantSquare = 0;
    };
    Position(Board board, uint64_t enPassantSquare, Board::PieceEnum currentPlayer, uint8_t castlingRights)
        : board(board), enPassantSquare(enPassantSquare), currentPlayer(currentPlayer), castlingRights(castlingRights) {};

    uint64_t getPieceSet(Board::PieceEnum color, Board::PieceEnum pieceType)
    {
        return board.getPieceSet(color, pieceType);
    }
    uint64_t getPieceSet(Board::PieceEnum i) { return board.getPieceSet(i); }
    uint64_t getAllPieces() { return board.getAllPieces(); }

    uint64_t getEnPassantSquare() { return enPassantSquare; }
    Board::PieceEnum getCurrentPlayer() { return currentPlayer; }
    Board::PieceEnum getOtherPlayer()
    {
        return (currentPlayer == Board::white) ? Board::black : Board::white;
    }

    uint8_t getCastlingRights() { return castlingRights; }
};
#endif // #ifndef POSITION_H
