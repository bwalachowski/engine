#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <stack>
#include "move.h"
#include "types.h"

class Board
{
    std::uint64_t pieces[8];
    Types::PieceEnum capturedPieces[64];
    int nPieces;

public:
    void init();
    void print();

    Board(std::string fen);
    Board() { init(); };
    uint64_t getPieceSet(Types::PieceEnum color, Types::PieceEnum pieceType) { return pieces[color] & pieces[pieceType]; }
    uint64_t getPieceSet(Types::PieceEnum i) { return pieces[i]; }
    uint64_t getAllPieces() { return pieces[Types::white] | pieces[Types::black]; }
    int getNumberOfPieces() { return nPieces; }
    Types::PieceEnum getCapturedPiece(int depth) { return capturedPieces[depth]; }

    void makeMove(Move move, Types::PieceEnum color, int depth);
    void removePiece(uint64_t square, Types::PieceEnum color, Types::PieceEnum pieceType)
    {
        pieces[color] &= ~square;
        pieces[pieceType] &= ~square;
        if (pieceType != Types::pawns)
        {
            nPieces--;
        }
    }

    void addPiece(uint64_t square, Types::PieceEnum color, Types::PieceEnum pieceType)
    {
        pieces[color] |= square;
        pieces[pieceType] |= square;
        if (pieceType != Types::pawns)
        {
            nPieces++;
        }
    }
};

#endif // #ifndef BOARD_H