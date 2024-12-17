#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <vector>

#include "move.h"
#include "position.h"

class MoveGenerator {
    const uint64_t notAFile = 0xfefefefefefefefe;
    const uint64_t notHFile = 0x7f7f7f7f7f7f7f7f;
    const uint64_t notOuterLines = 0x7e7e7e7e7e7e00;

    uint64_t bishopAttacksEmptyBoard[64];
    uint64_t rookAttacksEmptyBoard[64];
    uint64_t queenAttacksEmptyBoard[64];

    uint64_t bishopBlockersAndBeyond[64];
    uint64_t rookBlockersAndBeyond[64];
    uint64_t queenBlockersAndBeyond[64];

    uint64_t behind[64][64];

    void generateBishopPseudoLegalMoves(std::vector<Move>& moveVector, Position position);

    uint64_t southFill(uint64_t square);
    uint64_t northFill(uint64_t square);
    uint64_t eastFill(uint64_t square);
    uint64_t westFill(uint64_t square);
    uint64_t soEaFill(uint64_t square);
    uint64_t noEaFill(uint64_t square);
    uint64_t soWeFill(uint64_t square);
    uint64_t noWeFill(uint64_t square);

    uint64_t bishopAttacks(uint64_t square);
    uint64_t rookAttacks(uint64_t square);

    void generateBishopAttacksArray();
    void generateRookAttacksArray();
    void generateQueenAttacksArray();

    void generateBehindArray();

    uint64_t singleBishopMoves(int pos, Position position);
    uint64_t singleRookMoves(int pos, Position position);
    uint64_t singleQueenMoves(int pos, Position position);

public:
    MoveGenerator();
    std::vector<Move> generatePseudoLegalMoves(Position position);
        
};

#endif  // #ifndef MOVE_GENERATOR_H