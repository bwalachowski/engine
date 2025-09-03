#ifndef POSITION_H
#define POSITION_H

#include "board.h"

class Position
{
    Board board;
    uint64_t enPassantSquare;
    Types::PieceEnum currentPlayer;
    uint8_t castlingRights; // last 4 bits, in order from last:  white short castle, white
                            // long castle, black short castle, black long castle
    std::stack<uint8_t> prevCastlingRightsStack;
    std::stack<uint64_t> prevEnPassantSquaresStack;

    const static uint64_t a1Square = 0x80;
    const static uint64_t h1Square = 0x1;
    const static uint64_t h8Square = 0x100000000000000;
    const static uint64_t a8Square = 0x8000000000000000;
    const static uint64_t f1Square = 0x4;
    const static uint64_t f8Square = 0x400000000000000;
    const static uint64_t d1Square = 0x10;
    const static uint64_t d8Square = 0x1000000000000000;

public:
    Position(Board board, Types::PieceEnum currentPlayer, uint8_t castlingRights)
        : board(board),
          currentPlayer(currentPlayer), castlingRights(castlingRights)
    {
        enPassantSquare = 0;
        prevCastlingRightsStack = std::stack<uint8_t>();
        prevEnPassantSquaresStack = std::stack<uint64_t>();
    };
    Position(Board board, uint64_t enPassantSquare, Types::PieceEnum currentPlayer, uint8_t castlingRights)
        : board(board), enPassantSquare(enPassantSquare), currentPlayer(currentPlayer), castlingRights(castlingRights)
    {
        prevCastlingRightsStack = std::stack<uint8_t>();
        prevEnPassantSquaresStack = std::stack<uint64_t>();
    };

    Position(Board board, uint64_t enPassantSquare, Types::PieceEnum currentPlayer, uint8_t castlingRights,
             std::stack<uint8_t> prevCastlingRightsStack, std::stack<uint64_t> prevEnPassantSquaresStack)
        : board(board), enPassantSquare(enPassantSquare), currentPlayer(currentPlayer), castlingRights(castlingRights),
          prevCastlingRightsStack(prevCastlingRightsStack), prevEnPassantSquaresStack(prevEnPassantSquaresStack) {};

    Position(std::string fen);
    Position() : board(Board()), enPassantSquare(0), currentPlayer(Types::white), castlingRights(0b1111)
    {
        prevCastlingRightsStack = std::stack<uint8_t>();
        prevEnPassantSquaresStack = std::stack<uint64_t>();
    };

    uint64_t getPieceSet(Types::PieceEnum color, Types::PieceEnum pieceType)
    {
        return board.getPieceSet(color, pieceType);
    }
    uint64_t getPieceSet(Types::PieceEnum i) { return board.getPieceSet(i); }
    uint64_t getAllPieces() { return board.getAllPieces(); }

    uint64_t getEnPassantSquare() { return enPassantSquare; }
    Types::PieceEnum getCurrentPlayer() { return currentPlayer; }
    Types::PieceEnum getOtherPlayer()
    {
        return (currentPlayer == Types::white) ? Types::black : Types::white;
    }

    uint8_t getCastlingRights() { return castlingRights; }

    Position makeMove(Move move);
    Position unmakeMove(Move move);
};
#endif // #ifndef POSITION_H
