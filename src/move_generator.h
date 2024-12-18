#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <vector>

#include "move.h"
#include "position.h"

class MoveGenerator {
    const uint64_t notAFile = 0xfefefefefefefefe;
    const uint64_t notHFile = 0x7f7f7f7f7f7f7f7f;
    const uint64_t notOuterLines = 0xff00000000;
    const uint64_t fourthRank = 0xff000000;
    const uint64_t fifthRank = 0x7e7e7e7e7e7e00;

    uint64_t bishopAttacksEmptyBoard[64];
    uint64_t rookAttacksEmptyBoard[64];
    uint64_t queenAttacksEmptyBoard[64];
    uint64_t knightAttacksEmptyBoard[64];
    uint64_t kingAttacksEmptyBoard[64];

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
    uint64_t knightAttacks(uint64_t square);
    uint64_t kingAttacks(uint64_t square);

    void generateBishopAttacksArray();
    void generateRookAttacksArray();
    void generateQueenAttacksArray();
    void generateKnightAttacksArray();
    void generateKingAttacksArray();

    void generateBehindArray();

    uint64_t singleBishopMoves(uint32_t pos, Position position);
    uint64_t singleRookMoves(uint32_t pos, Position position);
    uint64_t singleQueenMoves(uint32_t pos, Position position);

    uint64_t whiteSinglePushTargets(uint64_t pawns, Position position) {return northOne(pawns) & ~position.getAllPieces();}
    uint64_t whiteDoublePushTargets(uint64_t pawns, Position position);
    uint64_t blackSinglePushTargets(uint64_t pawns, Position position) {return southOne(pawns) & ~position.getAllPieces();}
    uint64_t blackDoublePushTargets(uint64_t pawns, Position position);

    uint64_t whitePawnEastAttacks(uint64_t pawns) {return noEaOne(pawns);}
    uint64_t whitePawnWestAttacks(uint64_t pawns) {return noWeOne(pawns);}
    uint64_t blackPawnEastAttacks(uint64_t pawns) {return soEaOne(pawns);}
    uint64_t blackPawnWestAttacks(uint64_t pawns) {return soWeOne(pawns);}

    uint64_t northOne(uint64_t board) {return board >> 8;}
    uint64_t southOne(uint64_t board) {return board << 8;}
    uint64_t noWeOne(uint64_t board) {return (board << 7) & notHFile;}
    uint64_t noEaOne(uint64_t board) {return (board << 9) & notAFile;}
    uint64_t soWeOne(uint64_t board) {return (board >> 9) & notHFile;}
    uint64_t soEaOne(uint64_t board) {return (board >> 7) & notAFile;}

    uint32_t squareForMove(uint64_t square);

public:
    MoveGenerator();
    std::vector<Move> generatePseudoLegalMoves(Position position);
        
};

#endif  // #ifndef MOVE_GENERATOR_H