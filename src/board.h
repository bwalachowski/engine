#ifndef BOARD_H
#define BOARD_H

#include <cstdint>

class Board
{
    std::uint64_t pieces[8];

public:
    enum PieceEnum
    {
        white,
        black,
        pawns,
        knights,
        bishops,
        rooks,
        kings,
        queens
    };

    void init();
    void print();

    uint64_t getPieceSet(PieceEnum color, PieceEnum pieceType) { return pieces[color] & pieces[pieceType]; }
    uint64_t getPieceSet(PieceEnum i) { return pieces[i]; }
    uint64_t getAllPieces() { return pieces[white] | pieces[black]; }
};

#endif // #ifndef BOARD_H