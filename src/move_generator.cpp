#include "move_generator.h"
#include <iostream>
#include <bit>

MoveGenerator::MoveGenerator()
{
    generateBishopAttacksArray();
    generateRookAttacksArray();
    generateQueenAttacksArray();
    generateBehindArray();
}

std::vector<Move> MoveGenerator::generatePseudoLegalMoves(Position position)
{
    std::vector<Move> pseudo_legal_moves;
    generateBishopPseudoLegalMoves(pseudo_legal_moves, position);
    uint64_t a = eastFill(0);
    return pseudo_legal_moves;
}

void MoveGenerator::generateBishopPseudoLegalMoves(std::vector<Move>& moveVector, Position position) {
    uint64_t current_player_bishops = position.getPieceSet(position.current_player, Board::bishops);
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


uint64_t MoveGenerator::singleBishopMoves(int pos, Position position)
{
    uint64_t result = bishopAttacksEmptyBoard[pos];
    for(uint64_t b = position.getAllPieces() & bishopBlockersAndBeyond[pos]; b != 0; b &= (b-1)) {
        int sq = std::countr_zero(b);
        result &= ~behind[pos][sq];
    }
    return result;
}

uint64_t MoveGenerator::singleRookMoves(int pos, Position position)
{
    uint64_t result = rookAttacksEmptyBoard[pos];
    for(uint64_t b = position.getAllPieces() & rookBlockersAndBeyond[pos]; b != 0; b &= (b-1)) {
        int sq = std::countr_zero(b);
        result &= ~behind[pos][sq];
    }
    return result;
}

uint64_t MoveGenerator::singleQueenMoves(int pos, Position position)
{
    uint64_t result = queenAttacksEmptyBoard[pos];
    for(uint64_t b = position.getAllPieces() & queenBlockersAndBeyond[pos]; b != 0; b &= (b-1)) {
        int sq = std::countr_zero(b);
        result &= ~behind[pos][sq];
    }
    return result;
}
