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
    generateRookPseudoLegalMoves(pseudo_legal_moves, position);
    generateQueenPseudoLegalMoves(pseudo_legal_moves, position);
    generateKnightPseudoLegalMoves(pseudo_legal_moves, position);
    generateKingPseudoLegalMoves(pseudo_legal_moves, position);
    generatePawnPseudoLegalMoves(pseudo_legal_moves, position);
    return pseudo_legal_moves;
}

void MoveGenerator::generateBishopPseudoLegalMoves(std::vector<Move>& moveVector, Position position) {
    uint64_t currentPlayerBishops = position.getPieceSet(position.getCurrentPlayer(), Board::bishops);
    while(currentPlayerBishops) {
        uint32_t bishop = std::countr_zero(currentPlayerBishops);
        uint32_t fromSquare = squareForMove(bishop);
        uint64_t bishopMoves = ~position.getPieceSet(position.getCurrentPlayer()) & singleBishopMoves(bishop, position);
        uint64_t bishopAttacks = position.getPieceSet(position.getOtherPlayer()) & bishopMoves;
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

void MoveGenerator::generateRookPseudoLegalMoves(std::vector<Move>& moveVector, Position position) {
    uint64_t currentPlayerRooks = position.getPieceSet(position.getCurrentPlayer(), Board::rooks);
    while(currentPlayerRooks) {
        uint32_t rook = std::countr_zero(currentPlayerRooks);
        uint32_t fromSquare = squareForMove(rook);
        uint64_t rookMoves = ~position.getPieceSet(position.getCurrentPlayer()) & singleBishopMoves(rook, position);
        uint64_t rookAttacks = position.getPieceSet(position.getOtherPlayer()) & rookMoves;
        rookMoves &= ~rookAttacks;
        while (rookAttacks){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(rookAttacks)), Move::capture});
            rookAttacks &= rookAttacks - 1;
        }
        while (rookMoves){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(rookMoves)), Move::quiet});
            rookMoves &= rookMoves - 1;
        }
        currentPlayerRooks &= currentPlayerRooks - 1;
    }
}

void MoveGenerator::generateQueenPseudoLegalMoves(std::vector<Move>& moveVector, Position position) {
    uint64_t currentPlayerQueens = position.getPieceSet(position.getCurrentPlayer(), Board::queens);
    while(currentPlayerQueens) {
        uint32_t queen = std::countr_zero(currentPlayerQueens);
        uint32_t fromSquare = squareForMove(queen);
        uint64_t queenMoves = ~position.getPieceSet(position.getCurrentPlayer()) & singleBishopMoves(queen, position);
        uint64_t queenAttacks = position.getPieceSet(position.getOtherPlayer()) & queenMoves;
        queenMoves &= ~queenAttacks;
        while (queenAttacks){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(queenAttacks)), Move::capture});
            queenAttacks &= queenAttacks - 1;
        }
        while (queenMoves){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(queenMoves)), Move::quiet});
            queenMoves &= queenMoves - 1;
        }
        currentPlayerQueens &= currentPlayerQueens - 1;
    }
}

void MoveGenerator::generateKnightPseudoLegalMoves(std::vector<Move>& moveVector, Position position) {
    uint64_t currentPlayerKnights = position.getPieceSet(position.getCurrentPlayer(), Board::knights);
    while(currentPlayerKnights) {
        uint32_t knight = std::countr_zero(currentPlayerKnights);
        uint32_t fromSquare = squareForMove(knight);
        uint64_t knightMoves = ~position.getPieceSet(position.getCurrentPlayer()) & knightAttacksEmptyBoard[knight];
        uint64_t knightAttacks = position.getPieceSet(position.getOtherPlayer()) & knightMoves;
        knightMoves &= ~knightAttacks;
        while (knightAttacks){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(knightAttacks)), Move::capture});
            knightAttacks &= knightAttacks - 1;
        }
        while (knightMoves){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(knightMoves)), Move::quiet});
            knightMoves &= knightMoves - 1;
        }
        currentPlayerKnights &= currentPlayerKnights - 1;
    }
}

void MoveGenerator::generateKingPseudoLegalMoves(std::vector<Move>& moveVector, Position position) {
    uint64_t currentPlayerKing = position.getPieceSet(position.getCurrentPlayer(), Board::kings);
    uint32_t king = std::countr_zero(currentPlayerKing);
    uint32_t fromSquare = squareForMove(king);
    uint64_t kingMoves = ~position.getPieceSet(position.getCurrentPlayer()) & kingAttacksEmptyBoard[king];
    uint64_t kingAttacks = position.getPieceSet(position.getOtherPlayer()) & king;
    kingMoves &= ~kingAttacks;
    while (kingAttacks){
        moveVector.push_back({fromSquare, squareForMove(std::countr_zero(kingAttacks)), Move::capture});
        kingAttacks &= kingAttacks - 1;
    }
    while (kingMoves){
        moveVector.push_back({fromSquare, squareForMove(std::countr_zero(kingMoves)), Move::quiet});
        kingMoves &= kingMoves - 1;
    }
}

void MoveGenerator::generatePawnPseudoLegalMoves(std::vector<Move> &moveVector, Position position)
{
    uint64_t currentPlayerPawns = position.getPieceSet(position.getCurrentPlayer(), Board::pawns);
    if(position.getCurrentPlayer() == Board::white) {
        uint64_t whiteSinglePush = whiteSinglePushTargets(currentPlayerPawns, position);
        while(whiteSinglePush) {
            int lastPawn = std::countr_zero(whiteSinglePush);
            moveVector.push_back({squareForMove(lastPawn-8), squareForMove(lastPawn), Move::quiet});
            whiteSinglePush &= whiteSinglePush - 1;
        }
        uint64_t whiteDoublePush = whiteDoublePushTargets(currentPlayerPawns, position);
        while(whiteDoublePush) {
            int lastPawn = std::countr_zero(whiteDoublePush);
            moveVector.push_back({squareForMove(lastPawn-16), squareForMove(lastPawn), Move::doublePush});
            whiteDoublePush &= whiteDoublePush - 1;
        }
        uint64_t whitePawnEastAttacks = whitePawnEastAttackTargets(currentPlayerPawns) & position.getOtherPlayer();
        while(whitePawnEastAttacks) {
            int lastPawn = std::countr_zero(whitePawnEastAttacks);
            moveVector.push_back({squareForMove(lastPawn-9), squareForMove(lastPawn), Move::doublePush});
            whitePawnEastAttacks &= whitePawnEastAttacks - 1;
        }
        uint64_t whitePawnWestAttacks = whitePawnWestAttackTargets(currentPlayerPawns) & position.getOtherPlayer();
        while(whitePawnWestAttacks) {
            int lastPawn = std::countr_zero(whitePawnWestAttacks);
            moveVector.push_back({squareForMove(lastPawn-7), squareForMove(lastPawn), Move::doublePush});
            whitePawnWestAttacks &= whitePawnWestAttacks - 1;
        }
    }
    if(position.getCurrentPlayer() == Board::black) {
        uint64_t blackSinglePush = blackSinglePushTargets(currentPlayerPawns, position);
        while(blackSinglePush) {
            int lastPawn = std::countr_zero(blackSinglePush);
            moveVector.push_back({squareForMove(lastPawn+8), squareForMove(lastPawn), Move::quiet});
            blackSinglePush &= blackSinglePush - 1;
        }
        uint64_t blackDoublePush = blackDoublePushTargets(currentPlayerPawns, position);
        while(blackDoublePush) {
            int lastPawn = std::countr_zero(blackDoublePush);
            moveVector.push_back({squareForMove(lastPawn+16), squareForMove(lastPawn), Move::doublePush});
            blackDoublePush &= blackDoublePush - 1;
        }
        uint64_t blackPawnEastAttacks = blackPawnEastAttackTargets(currentPlayerPawns) & position.getOtherPlayer();
        while(blackPawnEastAttacks) {
            int lastPawn = std::countr_zero(blackPawnEastAttacks);
            moveVector.push_back({squareForMove(lastPawn+7), squareForMove(lastPawn), Move::doublePush});
            blackPawnEastAttacks &= blackPawnEastAttacks - 1;
        }
        uint64_t blackPawnWestAttacks = blackPawnWestAttackTargets(currentPlayerPawns) & position.getOtherPlayer();
        while(blackPawnWestAttacks) {
            int lastPawn = std::countr_zero(blackPawnWestAttacks);
            moveVector.push_back({squareForMove(lastPawn+9), squareForMove(lastPawn), Move::doublePush});
            blackPawnWestAttacks &= blackPawnWestAttacks - 1;
        }
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
    uint64_t result = notHFile & (square << 6);
    result |= notAFile & (square << 10);
    result |= notGHFile & (square << 15);
    result |= notABFile & (square << 17);
    result |= notHFile & (square >> 10);
    result |= notABFile & (square >> 15);
    result |= notGHFile & (square >> 17);
    result |= notAFile & (square >> 6);
    return result;
}

uint64_t MoveGenerator::kingAttacks(uint64_t square)
{
    uint64_t result = notAFile & (square << 1);
    result |= (notHFile & (square << 7));
    result |= (square << 8);
    result |= (notAFile & (square << 9));
    result |= (notHFile & (square >> 1));
    result |= (notAFile & (square >> 7));
    result |= (square >> 8);
    result |= (notHFile & (square >> 9));
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

uint32_t MoveGenerator::squareForMove(int square)
{   
    return ((square % 8) << 3) | (square / 8);
}