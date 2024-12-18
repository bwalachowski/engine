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
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(bishopAttacks)), Move::capture, Board::bishops});
            bishopAttacks &= bishopAttacks - 1;
        }
        while (bishopMoves){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(bishopMoves)), Move::quiet, Board::bishops});
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
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(rookAttacks)), Move::capture, Board::rooks});
            rookAttacks &= rookAttacks - 1;
        }
        while (rookMoves){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(rookMoves)), Move::quiet, Board::rooks});
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
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(queenAttacks)), Move::capture, Board::queens});
            queenAttacks &= queenAttacks - 1;
        }
        while (queenMoves){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(queenMoves)), Move::quiet, Board::queens});
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
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(knightAttacks)), Move::capture, Board::knights});
            knightAttacks &= knightAttacks - 1;
        }
        while (knightMoves){
            moveVector.push_back({fromSquare, squareForMove(std::countr_zero(knightMoves)), Move::quiet, Board::knights});
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
        moveVector.push_back({fromSquare, squareForMove(std::countr_zero(kingAttacks)), Move::capture, Board::kings});
        kingAttacks &= kingAttacks - 1;
    }
    while (kingMoves){
        moveVector.push_back({fromSquare, squareForMove(std::countr_zero(kingMoves)), Move::quiet, Board::kings});
        kingMoves &= kingMoves - 1;
    }
}

void MoveGenerator::generatePawnPseudoLegalMoves(std::vector<Move> &moveVector, Position position)
{
    uint64_t currentPlayerPawns = position.getPieceSet(position.getCurrentPlayer(), Board::pawns);
    if(position.getCurrentPlayer() == Board::white) {
        uint64_t whiteSinglePushNoPromotion = whiteSinglePushTargets(currentPlayerPawns, position) & not1Rank;
        while(whiteSinglePushNoPromotion) {
            int lastPawn = std::countr_zero(whiteSinglePushNoPromotion);
            moveVector.push_back({squareForMove(lastPawn-8), squareForMove(lastPawn), Move::quiet, Board::pawns});
            whiteSinglePushNoPromotion &= whiteSinglePushNoPromotion - 1;
        }
        uint64_t whiteSinglePushPromotion = whiteSinglePushTargets(currentPlayerPawns, position) & ~not1Rank;
        while(whiteSinglePushPromotion) {
            int lastPawn = std::countr_zero(whiteSinglePushPromotion);
            moveVector.push_back({squareForMove(lastPawn-8), squareForMove(lastPawn), Move::knightPromotion, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn-8), squareForMove(lastPawn), Move::bishopPromotion, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn-8), squareForMove(lastPawn), Move::queenPromotion, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn-8), squareForMove(lastPawn), Move::rookPromotion, Board::pawns});
            whiteSinglePushNoPromotion &= whiteSinglePushNoPromotion - 1;
        }
        uint64_t whiteDoublePush = whiteDoublePushTargets(currentPlayerPawns, position);
        while(whiteDoublePush) {
            int lastPawn = std::countr_zero(whiteDoublePush);
            moveVector.push_back({squareForMove(lastPawn-16), squareForMove(lastPawn), Move::doublePush, Board::pawns});
            whiteDoublePush &= whiteDoublePush - 1;
        }
        uint64_t whitePawnEastAttacksNoPromotion = (whitePawnEastAttackTargets(currentPlayerPawns) & position.getOtherPlayer()) & not1Rank;
        while(whitePawnEastAttacksNoPromotion) {
            int lastPawn = std::countr_zero(whitePawnEastAttacksNoPromotion);
            moveVector.push_back({squareForMove(lastPawn-9), squareForMove(lastPawn), Move::capture, Board::pawns});
            whitePawnEastAttacksNoPromotion &= whitePawnEastAttacksNoPromotion - 1;
        }
        uint64_t whitePawnEastAttacksPromotion = (whitePawnEastAttackTargets(currentPlayerPawns) & position.getOtherPlayer()) & ~not1Rank;
        while(whitePawnEastAttacksPromotion) {
            int lastPawn = std::countr_zero(whitePawnEastAttacksPromotion);
            moveVector.push_back({squareForMove(lastPawn-9), squareForMove(lastPawn), Move::knightPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn-9), squareForMove(lastPawn), Move::bishopPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn-9), squareForMove(lastPawn), Move::queenPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn-9), squareForMove(lastPawn), Move::rookPromotionCapture, Board::pawns});
            whitePawnEastAttacksPromotion &= whitePawnEastAttacksPromotion - 1;
        }
        uint64_t whitePawnEastAttackEnPassant = whitePawnEastAttackTargets(currentPlayerPawns) & position.getEnPassantSquare();
        if (whitePawnEastAttackEnPassant) {
            int lastPawn = std::countr_zero(whitePawnEastAttackEnPassant);
            moveVector.push_back({squareForMove(lastPawn-9), squareForMove(lastPawn), Move::enPassant, Board::pawns});
        }
        uint64_t whitePawnWestAttacksNoPromotion = whitePawnWestAttackTargets(currentPlayerPawns) & position.getOtherPlayer() & not1Rank;
        while(whitePawnWestAttacksNoPromotion) {
            int lastPawn = std::countr_zero(whitePawnWestAttacksNoPromotion);
            moveVector.push_back({squareForMove(lastPawn-7), squareForMove(lastPawn), Move::capture, Board::pawns});
            whitePawnWestAttacksNoPromotion &= whitePawnWestAttacksNoPromotion - 1;
        }
        uint64_t whitePawnWestAttacksPromotion = whitePawnWestAttackTargets(currentPlayerPawns) & position.getOtherPlayer() & ~not1Rank;
        while(whitePawnWestAttacksPromotion) {
            int lastPawn = std::countr_zero(whitePawnWestAttacksPromotion);
            moveVector.push_back({squareForMove(lastPawn-7), squareForMove(lastPawn), Move::knightPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn-7), squareForMove(lastPawn), Move::bishopPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn-7), squareForMove(lastPawn), Move::queenPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn-7), squareForMove(lastPawn), Move::rookPromotionCapture, Board::pawns});
            whitePawnWestAttacksPromotion &= whitePawnWestAttacksPromotion - 1;
        }
        uint64_t whitePawnWestAttackEnPassant = whitePawnWestAttackTargets(currentPlayerPawns) & position.getEnPassantSquare();
        if (whitePawnWestAttackEnPassant) {
            int lastPawn = std::countr_zero(whitePawnWestAttackEnPassant);
            moveVector.push_back({squareForMove(lastPawn-7), squareForMove(lastPawn), Move::enPassant, Board::pawns});
        }
    }
    if(position.getCurrentPlayer() == Board::black) {
        uint64_t blackSinglePushNoPromotion = blackSinglePushTargets(currentPlayerPawns, position) & not1Rank;
        while(blackSinglePushNoPromotion) {
            int lastPawn = std::countr_zero(blackSinglePushNoPromotion);
            moveVector.push_back({squareForMove(lastPawn+8), squareForMove(lastPawn), Move::quiet, Board::pawns});
            blackSinglePushNoPromotion &= blackSinglePushNoPromotion - 1;
        }
        uint64_t blackSinglePushPromotion = blackSinglePushTargets(currentPlayerPawns, position) & ~not1Rank;
        while(blackSinglePushPromotion) {
            int lastPawn = std::countr_zero(blackSinglePushPromotion);
            moveVector.push_back({squareForMove(lastPawn+8), squareForMove(lastPawn), Move::knightPromotion, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn+8), squareForMove(lastPawn), Move::bishopPromotion, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn+8), squareForMove(lastPawn), Move::queenPromotion, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn+8), squareForMove(lastPawn), Move::rookPromotion, Board::pawns});
            blackSinglePushNoPromotion &= blackSinglePushNoPromotion - 1;
        }
        uint64_t blackDoublePush = blackDoublePushTargets(currentPlayerPawns, position);
        while(blackDoublePush) {
            int lastPawn = std::countr_zero(blackDoublePush);
            moveVector.push_back({squareForMove(lastPawn+16), squareForMove(lastPawn), Move::doublePush, Board::pawns});
            blackDoublePush &= blackDoublePush - 1;
        }
        uint64_t blackPawnEastAttacksNoPromotion = (blackPawnEastAttackTargets(currentPlayerPawns) & position.getOtherPlayer()) & not1Rank;
        while(blackPawnEastAttacksNoPromotion) {
            int lastPawn = std::countr_zero(blackPawnEastAttacksNoPromotion);
            moveVector.push_back({squareForMove(lastPawn+7), squareForMove(lastPawn), Move::capture, Board::pawns});
            blackPawnEastAttacksNoPromotion &= blackPawnEastAttacksNoPromotion - 1;
        }
        uint64_t blackPawnEastAttacksPromotion = (blackPawnEastAttackTargets(currentPlayerPawns) & position.getOtherPlayer()) & ~not1Rank;
        while(blackPawnEastAttacksPromotion) {
            int lastPawn = std::countr_zero(blackPawnEastAttacksPromotion);
            moveVector.push_back({squareForMove(lastPawn+7), squareForMove(lastPawn), Move::knightPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn+7), squareForMove(lastPawn), Move::bishopPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn+7), squareForMove(lastPawn), Move::queenPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn+7), squareForMove(lastPawn), Move::rookPromotionCapture, Board::pawns});
            blackPawnEastAttacksPromotion &= blackPawnEastAttacksPromotion - 1;
        }
        uint64_t blackPawnEastAttackEnPassant = blackPawnEastAttackTargets(currentPlayerPawns) & position.getEnPassantSquare();
        if (blackPawnEastAttackEnPassant) {
            int lastPawn = std::countr_zero(blackPawnEastAttackEnPassant);
            moveVector.push_back({squareForMove(lastPawn+7), squareForMove(lastPawn), Move::enPassant, Board::pawns});
        }
        uint64_t blackPawnWestAttacksNoPromotion = blackPawnWestAttackTargets(currentPlayerPawns) & position.getOtherPlayer() & not1Rank;
        while(blackPawnWestAttacksNoPromotion) {
            int lastPawn = std::countr_zero(blackPawnWestAttacksNoPromotion);
            moveVector.push_back({squareForMove(lastPawn+9), squareForMove(lastPawn), Move::capture, Board::pawns});
            blackPawnWestAttacksNoPromotion &= blackPawnWestAttacksNoPromotion - 1;
        }
        uint64_t blackPawnWestAttacksPromotion = blackPawnWestAttackTargets(currentPlayerPawns) & position.getOtherPlayer() & ~not1Rank;
        while(blackPawnWestAttacksPromotion) {
            int lastPawn = std::countr_zero(blackPawnWestAttacksPromotion);
            moveVector.push_back({squareForMove(lastPawn+9), squareForMove(lastPawn), Move::knightPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn+9), squareForMove(lastPawn), Move::bishopPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn+9), squareForMove(lastPawn), Move::queenPromotionCapture, Board::pawns});
            moveVector.push_back({squareForMove(lastPawn+9), squareForMove(lastPawn), Move::rookPromotionCapture, Board::pawns});
            blackPawnWestAttacksPromotion &= blackPawnWestAttacksPromotion - 1;
        }
        uint64_t blackPawnWestAttackEnPassant = blackPawnWestAttackTargets(currentPlayerPawns) & position.getEnPassantSquare();
        if (blackPawnWestAttackEnPassant) {
            int lastPawn = std::countr_zero(blackPawnWestAttackEnPassant);
            moveVector.push_back({squareForMove(lastPawn+9), squareForMove(lastPawn), Move::enPassant, Board::pawns});
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