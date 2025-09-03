#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <stack>
#include "move.h"
#include "types.h"

class Board
{
    std::uint64_t pieces[8];
    std::stack<Types::PieceEnum> capturedPiecesStack;

public:
    void init();
    void print();

    Board(std::string fen);
    Board() { init(); };
    uint64_t getPieceSet(Types::PieceEnum color, Types::PieceEnum pieceType) { return pieces[color] & pieces[pieceType]; }
    uint64_t getPieceSet(Types::PieceEnum i) { return pieces[i]; }
    uint64_t getAllPieces() { return pieces[Types::white] | pieces[Types::black]; }
    Types::PieceEnum getCapturedPiece() { return capturedPiecesStack.top(); }
    void popCapturedPieces() { capturedPiecesStack.pop(); }
    size_t capturedPiecesStackSize() { return capturedPiecesStack.size(); }

    void makeMove(Move move, Types::PieceEnum color);
    void removePiece(uint64_t square, Types::PieceEnum color, Types::PieceEnum pieceType)
    {
        pieces[color] &= ~square;
        pieces[pieceType] &= ~square;
    }

    void addPiece(uint64_t square, Types::PieceEnum color, Types::PieceEnum pieceType)
    {
        pieces[color] |= square;
        pieces[pieceType] |= square;
    }
};

#endif // #ifndef BOARD_H