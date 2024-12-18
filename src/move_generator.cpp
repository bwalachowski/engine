#include "move_generator.h"
#include <iostream>
#include <bit>

MoveGenerator::MoveGenerator()
{
    generateBishopAttacksArray();
    generateRookAttacksArray();
    generateQueenAttacksArray();
    generateKnightAttacksArray();
    generateKingAttacksArray();
    generateBehindArray();
}

std::vector<Move> MoveGenerator::generatePseudoLegalMoves(Position position)
{
    std::vector<Move> pseudo_legal_moves;
    generateBishopPseudoLegalMoves(pseudo_legal_moves, position);
    return pseudo_legal_moves;
}

void MoveGenerator::generateBishopPseudoLegalMoves(std::vector<Move>& moveVector, Position position) {
    uint64_t currentPlayerBishops = position.getPieceSet(position.getCurrentPlayer(), Board::bishops);
    while(currentPlayerBishops) {
        uint32_t bishop = std::countr_zero(currentPlayerBishops);
        uint32_t fromSquare = squareForMove(bishop);
        uint64_t bishopMoves = position.getPieceSet(position.getCurrentPlayer()) ^ singleBishopMoves(bishop, position);
        uint64_t bishopAttacks = position.getPieceSet(position.getOtherPlayer()) ^ bishopMoves;
        bishopMoves &= ~bishopAttacks;
        while (bishopAttacks){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(bishopAttacks)), Move::capture});
            bishopAttacks &= bishopAttacks - 1;
        }
        while (bishopMoves){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(bishopMoves)), Move::quiet});
            bishopMoves &= bishopMoves - 1;
        }
        currentPlayerBishops &= currentPlayerBishops - 1;
    }
}

uint64_t MoveGenerator::southFill(uint64_t square) {
    square |= (square >> 8);
    square |= (square >> 16);
    square |= (square >> 32);
    return square;
}

uint64_t MoveGenerator::northFill(uint64_t square) {
    square |= (square << 8);
    square |= (square << 16);
    square |= (square << 32);
    return square;
}

uint64_t MoveGenerator::eastFill(uint64_t square) {
    const uint64_t pr0 = notAFile;
    const uint64_t pr1 = pr0 & (pr0 << 1);
    const uint64_t pr2 = pr1 & (pr1 << 2);
    square |= pr0 & (square << 1);
    square |= pr1 & (square << 2);
    square |= pr2 & (square << 4);
    return square;
}

uint64_t MoveGenerator::westFill(uint64_t square)
{
    const uint64_t pr0 = notHFile;
    const uint64_t pr1 = pr0 & (pr0 >> 1);
    const uint64_t pr2 = pr1 & (pr1 >> 2);
    square |= pr0 & (square >> 1);
    square |= pr1 & (square >> 2);
    square |= pr2 & (square >> 4);
    return square;
}

uint64_t MoveGenerator::soEaFill(uint64_t square)
{
    const uint64_t pr0 = notAFile;
    const uint64_t pr1 = pr0 & (pr0 << 1);
    const uint64_t pr2 = pr1 & (pr1 << 2);
    square |= pr0 & (square >> 7);
    square |= pr1 & (square >> 14);
    square |= pr2 & (square >> 28);
    return square;
}

uint64_t MoveGenerator::noEaFill(uint64_t square)
{
    const uint64_t pr0 = notAFile;
    const uint64_t pr1 = pr0 & (pr0 << 1);
    const uint64_t pr2 = pr1 & (pr1 << 2);
    square |= pr0 & (square << 9);
    square |= pr1 & (square << 18);
    square |= pr2 & (square << 36);
    return square;
}

uint64_t MoveGenerator::soWeFill(uint64_t square)
{
    const uint64_t pr0 = notHFile;
    const uint64_t pr1 = pr0 & (pr0 >> 1);
    const uint64_t pr2 = pr1 & (pr1 >> 2);
    square |= pr0 & (square >> 9);
    square |= pr1 & (square >> 18);
    square |= pr2 & (square >> 36);
    return square;
}

uint64_t MoveGenerator::noWeFill(uint64_t square)
{
    const uint64_t pr0 = notHFile;
    const uint64_t pr1 = pr0 & (pr0 >> 1);
    const uint64_t pr2 = pr1 & (pr1 >> 2);
    square |= pr0 & (square << 7);
    square |= pr1 & (square << 14);
    square |= pr2 & (square << 28);
    return square;
}

uint64_t MoveGenerator::bishopAttacks(uint64_t square)
{
    return square ^ (noEaFill(square) | noWeFill(square) | soEaFill(square) | soWeFill(square));
}

uint64_t MoveGenerator::rookAttacks(uint64_t square)
{
    return square ^ (southFill(square) | northFill(square) | eastFill(square) | westFill(square));
}

uint64_t MoveGenerator::knightAttacks(uint64_t square)
{
    uint64_t result = square << 6;
    result &= square << 10;
    result &= square << 15;
    result &= square << 17;
    result &= square >> 10;
    result &= square >> 15;
    result &= square >> 17;
    result &= square >> 6;
    return result;
}

uint64_t MoveGenerator::kingAttacks(uint64_t square)
{
    uint64_t result = square << 1;
    result &= square << 7;
    result &= square << 8;
    result &= square << 9;
    result &= square >> 1;
    result &= square >> 7;
    result &= square >> 8;
    result &= square >> 9;
    return result;
}

void MoveGenerator::generateBishopAttacksArray()
{
    for(int i = 0; i < 64; ++i) {
        bishopAttacksEmptyBoard[i] = bishopAttacks(uint64_t(1) << i);
        bishopBlockersAndBeyond[i] = bishopAttacksEmptyBoard[i] & notOuterLines;
    }
}

void MoveGenerator::generateRookAttacksArray()
{
    for(int i = 0; i < 64; ++i) {
        rookAttacksEmptyBoard[i] = rookAttacks(uint64_t(1) << i);
        rookBlockersAndBeyond[i] = rookAttacksEmptyBoard[i] & notOuterLines;
    }
}

void MoveGenerator::generateQueenAttacksArray()
{
    for(int i = 0; i < 64; ++i) {
        queenAttacksEmptyBoard[i] = bishopAttacksEmptyBoard[i] | rookAttacksEmptyBoard[i];
        queenBlockersAndBeyond[i] = queenAttacksEmptyBoard[i] & notOuterLines;
    }
}

void MoveGenerator::generateKnightAttacksArray()
{
    for(int i = 0; i < 64; ++i) {
        knightAttacksEmptyBoard[i] = knightAttacks(uint64_t(1) << i);
    }
}

void MoveGenerator::generateKingAttacksArray()
{
    for(int i = 0; i < 64; ++i) {
        kingAttacksEmptyBoard[i] = kingAttacks(uint64_t(1) << i);
    }
}

void MoveGenerator::generateBehindArray()
{
    for(int i = 0; i < 64; ++i) {
        for (int j = i + 1; j < i + 8 - (i % 8); ++j) {
            behind[i][j] =  (uint64_t(1) << j) ^ (eastFill(uint64_t(1) << i) & eastFill(uint64_t(1) << j));
        }
        for (int j = i - 1; j >= i - (i % 8); --j) {
            behind[i][j] =  (uint64_t(1) << j) ^ (westFill(uint64_t(1) << i) & westFill(uint64_t(1) << j));
        }
        for (int j = i + 8; j < 64; j+=8) {
            behind[i][j] =  (uint64_t(1) << j) ^ (northFill(uint64_t(1) << i) & northFill(uint64_t(1) << j));
        }
        for (int j = i - 8; j >= 0; j-=8) {
            behind[i][j] =  (uint64_t(1) << j) ^ (southFill(uint64_t(1) << i) & southFill(uint64_t(1) << j));
        }
        for (int j = i + 9; j < 64; j+=9) {
            behind[i][j] =  (uint64_t(1) << j) ^ (noEaFill(uint64_t(1) << i) & noEaFill(uint64_t(1) << j));
        }
        for (int j = i + 7; j < 64; j+=7) {
            behind[i][j] =  (uint64_t(1) << j) ^ (noWeFill(uint64_t(1) << i) & noWeFill(uint64_t(1) << j));
        }
        for (int j = i - 7; j >= 0; j-=7) {
            behind[i][j] =  (uint64_t(1) << j) ^ (soEaFill(uint64_t(1) << i) & soEaFill(uint64_t(1) << j));
        }
        for (int j = i - 9; j >= 0; j-=9) {
            behind[i][j] =  (uint64_t(1) << j) ^ (soWeFill(uint64_t(1) << i) & soWeFill(uint64_t(1) << j));
        }
    }
}


uint64_t MoveGenerator::singleBishopMoves(uint32_t pos, Position position)
{
    uint64_t result = bishopAttacksEmptyBoard[pos];
    for(uint64_t b = position.getAllPieces() & bishopBlockersAndBeyond[pos]; b != 0; b &= (b-1)) {
        int sq = std::countr_zero(b);
        result &= ~behind[pos][sq];
    }
    return result;
}

uint64_t MoveGenerator::singleRookMoves(uint32_t pos, Position position)
{
    uint64_t result = rookAttacksEmptyBoard[pos];
    for(uint64_t b = position.getAllPieces() & rookBlockersAndBeyond[pos]; b != 0; b &= (b-1)) {
        int sq = std::countr_zero(b);
        result &= ~behind[pos][sq];
    }
    return result;
}

uint64_t MoveGenerator::singleQueenMoves(uint32_t pos, Position position)
{
    uint64_t result = queenAttacksEmptyBoard[pos];
    for(uint64_t b = position.getAllPieces() & queenBlockersAndBeyond[pos]; b != 0; b &= (b-1)) {
        int sq = std::countr_zero(b);
        result &= ~behind[pos][sq];
    }
    return result;
}

uint64_t MoveGenerator::whiteDoublePushTargets(uint64_t pawns, Position position)
{
    return northOne(whiteSinglePushTargets(pawns, position)) & fourthRank & ~position.getAllPieces();
}

uint64_t MoveGenerator::blackDoublePushTargets(uint64_t pawns, Position position)
{
    return southOne(blackSinglePushTargets(pawns, position)) & fifthRank & ~position.getAllPieces();
}

uint32_t MoveGenerator::squareForMove(uint64_t square)
{
    return ((square % 8) + 1) >> 3 | ((square / 8) + 1);
}