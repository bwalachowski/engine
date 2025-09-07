#include "move_generator.h"
#include <bit>
#include <iostream>

MoveGenerator::MoveGenerator()
{
    generateBishopAttacksArray();
    generateRookAttacksArray();
    generateQueenAttacksArray();
    generateKnightAttacksArray();
    generateKingAttacksArray();
    generateBehindArray();
}

int MoveGenerator::generateLegalMoves(Position position, Move *moves)
{
    int n_moves = generatePseudoLegalMoves(position, moves);
    // std::cout << "Pseudo legal moves: " << n_moves << "\n";
    // for (int i = 0; i < n_moves; i++)
    // {
    //     std::cout << moves[i] << "\n";
    // }

    for (int i = 0; i < n_moves; i++)
    {
        Position new_position = position.makeMove(moves[i]);
        uint64_t king_square = new_position.getPieceSet(position.getCurrentPlayer(), Types::kings);
        if (attacked(king_square, new_position, position.getOtherPlayer()))
        {
            moves[i] = moves[n_moves - 1];
            n_moves--;
            i--;
        }
    }
    return n_moves;
}

int MoveGenerator::checkMobility(Position position, Types::PieceEnum color)
{
    Types::PieceEnum currentPlayer = position.getCurrentPlayer();
    bool changedPlayer = false;
    if (currentPlayer != color)
    {
        changedPlayer = true;
        position.changeCurrentPlayer();
    }
    Move moves[256];
    int n_moves = generatePseudoLegalMoves(position, moves);
    for (int i = 0; i < n_moves; i++)
    {
        Position new_position = position.makeMove(moves[i]);
        uint64_t king_square = new_position.getPieceSet(position.getCurrentPlayer(), Types::kings);
        if (attacked(king_square, new_position, position.getOtherPlayer()))
        {
            moves[i] = moves[n_moves - 1];
            n_moves--;
            i--;
        }
    }
    if (changedPlayer)
    {
        position.changeCurrentPlayer();
    }
    return n_moves;
}

int MoveGenerator::checkPseudoLegalMobility(Position position, Types::PieceEnum color)
{
    int n_moves = 0;
    n_moves += generateBishopPseudoLegalMobility(position, color);
    n_moves += generateRookPseudoLegalMobility(position, color);
    n_moves += generateQueenPseudoLegalMobility(position, color);
    n_moves += generateKnightPseudoLegalMobility(position, color);
    n_moves += generateKingPseudoLegalMobility(position, color);
    n_moves += generatePawnPseudoLegalMobility(position, color);
    return n_moves;
}

int MoveGenerator::generatePseudoLegalMoves(Position position, Move *moves)
{
    int n_moves = 0;
    generatePawnPseudoLegalMoves(moves, position, &n_moves);
    generateKnightPseudoLegalMoves(moves, position, &n_moves);
    generateBishopPseudoLegalMoves(moves, position, &n_moves);
    generateRookPseudoLegalMoves(moves, position, &n_moves);
    generateQueenPseudoLegalMoves(moves, position, &n_moves);
    generateKingPseudoLegalMoves(moves, position, &n_moves);
    return n_moves;
}

int MoveGenerator::generateBishopPseudoLegalMobility(Position position, Types::PieceEnum color)
{
    uint64_t playerBishops = position.getPieceSet(color, Types::bishops);
    int n_moves = 0;
    while (playerBishops)
    {
        uint32_t bishop = std::countr_zero(playerBishops);
        uint64_t bishopMoves = ~position.getPieceSet(color) & singleBishopMoves(bishop, position);
        n_moves += std::popcount(bishopMoves);
        playerBishops &= playerBishops - 1;
    }
    return n_moves;
}

int MoveGenerator::generateRookPseudoLegalMobility(Position position, Types::PieceEnum color)
{
    uint64_t playerRooks = position.getPieceSet(color, Types::rooks);
    int n_moves = 0;
    while (playerRooks)
    {
        uint32_t rook = std::countr_zero(playerRooks);
        uint64_t rookMoves = ~position.getPieceSet(color) & singleRookMoves(rook, position);
        n_moves += std::popcount(rookMoves);
        playerRooks &= playerRooks - 1;
    }
    return n_moves;
}

int MoveGenerator::generateQueenPseudoLegalMobility(Position position, Types::PieceEnum color)
{
    uint64_t playerQueens = position.getPieceSet(color, Types::queens);
    int n_moves = 0;
    while (playerQueens)
    {
        uint32_t queen = std::countr_zero(playerQueens);
        uint64_t queenMoves = ~position.getPieceSet(color) & (singleBishopMoves(queen, position) | singleRookMoves(queen, position));
        n_moves += std::popcount(queenMoves);
        playerQueens &= playerQueens - 1;
    }
    return n_moves;
}

int MoveGenerator::generateKnightPseudoLegalMobility(Position position, Types::PieceEnum color)
{
    uint64_t playerKnights = position.getPieceSet(color, Types::knights);
    int n_moves = 0;
    while (playerKnights)
    {
        uint32_t knight = std::countr_zero(playerKnights);
        uint64_t knightMoves = ~position.getPieceSet(color) & knightAttacksEmptyBoard[knight];
        n_moves += std::popcount(knightMoves);
        playerKnights &= playerKnights - 1;
    }
    return n_moves;
}

int MoveGenerator::generateKingPseudoLegalMobility(Position position, Types::PieceEnum color)
{
    uint64_t playerKing = position.getPieceSet(color, Types::kings);
    int n_moves = 0;
    uint32_t king = std::countr_zero(playerKing);
    uint64_t kingMoves = ~position.getPieceSet(color) & kingAttacksEmptyBoard[king];
    n_moves += std::popcount(kingMoves);
    if (shortCastleLegal(position, color))
    {
        n_moves++;
    }
    if (longCastleLegal(position, color))
    {
        n_moves++;
    }

    return n_moves;
}

int MoveGenerator::generatePawnPseudoLegalMobility(Position position, Types::PieceEnum color)
{
    uint64_t playerPawns = position.getPieceSet(color, Types::pawns);
    Types::PieceEnum opponent = (color == Types::white) ? Types::black : Types::white;
    int n_moves = 0;
    if (color == Types::white)
    {
        uint64_t singlePushTargets = whiteSinglePushTargets(playerPawns, position);
        n_moves += std::popcount(singlePushTargets);
        uint64_t doublePushTargets = whiteDoublePushTargets(playerPawns, position);
        n_moves += std::popcount(doublePushTargets);
        uint64_t eastAttackTargets = whitePawnEastAttackTargets(playerPawns) & position.getPieceSet(opponent);
        n_moves += std::popcount(eastAttackTargets);
        uint64_t westAttackTargets = whitePawnWestAttackTargets(playerPawns) & position.getPieceSet(opponent);
        n_moves += std::popcount(westAttackTargets);
        if (position.getEnPassantSquare() & whitePawnEastAttackTargets(playerPawns))
        {
            n_moves++;
        }
        if (position.getEnPassantSquare() & whitePawnWestAttackTargets(playerPawns))
        {
            n_moves++;
        }
    }
    else
    {
        uint64_t singlePushTargets = blackSinglePushTargets(playerPawns, position);
        n_moves += std::popcount(singlePushTargets);
        uint64_t doublePushTargets = blackDoublePushTargets(playerPawns, position);
        n_moves += std::popcount(doublePushTargets);
        uint64_t eastAttackTargets = blackPawnEastAttackTargets(playerPawns) & position.getPieceSet(opponent);
        n_moves += std::popcount(eastAttackTargets);
        uint64_t westAttackTargets = blackPawnWestAttackTargets(playerPawns) & position.getPieceSet(opponent);
        n_moves += std::popcount(westAttackTargets);
        if (position.getEnPassantSquare() & blackPawnEastAttackTargets(playerPawns))
        {
            n_moves++;
        }
        if (position.getEnPassantSquare() & blackPawnWestAttackTargets(playerPawns))
        {
            n_moves++;
        }
    }
    return n_moves;
}

// void MoveGenerator::generatePseudoLegalMoves(uint32_t fromSquare, uint64_t movesToMake, Move *moves, int *n_moves, Types::PieceEnum movedPiece, Move::flagEnum flag, Types::PieceEnum capturedPiece)
// {
//     while (moves)
//     {
//         moves[*n_moves] = {fromSquare,
//                            squareForMove(std::countr_zero(movesToMake)),
//                            flag, capturedPiece};
//         movesToMake &= movesToMake - 1;
//         (*n_moves)++;
//     }
// }

// void MoveGenerator::generateAttacksOnPiece(Types::PieceEnum pieceAttacked, Move *moves, int *n_moves, uint64_t knights, uint64_t bishops, uint64_t rooks, uint64_t queens){

// }

void MoveGenerator::generateBishopPseudoLegalMoves(Move *moves,
                                                   Position position, int *n_moves)
{
    uint64_t currentPlayerBishops = position.getPieceSet(position.getCurrentPlayer(), Types::bishops);
    while (currentPlayerBishops)
    {
        uint32_t bishop = std::countr_zero(currentPlayerBishops);
        uint32_t fromSquare = squareForMove(bishop);
        uint64_t bishopMoves = ~position.getPieceSet(position.getCurrentPlayer()) & singleBishopMoves(bishop, position);
        uint64_t bishopAttacks = position.getPieceSet(position.getOtherPlayer()) & bishopMoves;
        bishopMoves &= ~bishopAttacks;
        while (bishopAttacks)
        {
            moves[*n_moves] = {fromSquare,
                               squareForMove(std::countr_zero(bishopAttacks)),
                               Move::capture, Types::bishops};
            bishopAttacks &= bishopAttacks - 1;
            (*n_moves)++;
        }
        while (bishopMoves)
        {
            moves[*n_moves] = {fromSquare,
                               squareForMove(std::countr_zero(bishopMoves)),
                               Move::quiet, Types::bishops};
            bishopMoves &= bishopMoves - 1;
            (*n_moves)++;
        }
        currentPlayerBishops &= currentPlayerBishops - 1;
    }
}

void MoveGenerator::generateRookPseudoLegalMoves(Move *moves,
                                                 Position position, int *n_moves)
{
    uint64_t currentPlayerRooks = position.getPieceSet(position.getCurrentPlayer(), Types::rooks);
    while (currentPlayerRooks)
    {
        uint32_t rook = std::countr_zero(currentPlayerRooks);
        uint32_t fromSquare = squareForMove(rook);
        uint64_t rookMoves = ~position.getPieceSet(position.getCurrentPlayer()) & singleRookMoves(rook, position);
        uint64_t rookAttacks = position.getPieceSet(position.getOtherPlayer()) & rookMoves;
        // std::cout << "Rook attacks: " << rookAttacks << "\n";
        // std::cout << "Rook moves: " << rookMoves << "\n";
        rookMoves &= ~rookAttacks;
        while (rookAttacks)
        {
            moves[*n_moves] = {fromSquare,
                               squareForMove(std::countr_zero(rookAttacks)),
                               Move::capture, Types::rooks};
            rookAttacks &= rookAttacks - 1;
            (*n_moves)++;
        }
        while (rookMoves)
        {
            moves[*n_moves] = {fromSquare,
                               squareForMove(std::countr_zero(rookMoves)),
                               Move::quiet, Types::rooks};
            rookMoves &= rookMoves - 1;
            (*n_moves)++;
        }
        currentPlayerRooks &= currentPlayerRooks - 1;
    }
}

void MoveGenerator::generateQueenPseudoLegalMoves(Move *moves,
                                                  Position position, int *n_moves)
{
    uint64_t currentPlayerQueens = position.getPieceSet(position.getCurrentPlayer(), Types::queens);
    while (currentPlayerQueens)
    {
        uint32_t queen = std::countr_zero(currentPlayerQueens);
        uint32_t fromSquare = squareForMove(queen);
        uint64_t queenMoves = ~position.getPieceSet(position.getCurrentPlayer()) & (singleBishopMoves(queen, position) | singleRookMoves(queen, position));
        uint64_t queenAttacks = position.getPieceSet(position.getOtherPlayer()) & queenMoves;
        queenMoves &= ~queenAttacks;
        while (queenAttacks)
        {
            moves[*n_moves] = {fromSquare,
                               squareForMove(std::countr_zero(queenAttacks)),
                               Move::capture, Types::queens};
            queenAttacks &= queenAttacks - 1;
            (*n_moves)++;
        }
        while (queenMoves)
        {
            moves[*n_moves] = {fromSquare,
                               squareForMove(std::countr_zero(queenMoves)),
                               Move::quiet, Types::queens};
            queenMoves &= queenMoves - 1;
            (*n_moves)++;
        }
        currentPlayerQueens &= currentPlayerQueens - 1;
    }
}

void MoveGenerator::generateKnightPseudoLegalMoves(Move *moves,
                                                   Position position, int *n_moves)
{
    uint64_t currentPlayerKnights = position.getPieceSet(position.getCurrentPlayer(), Types::knights);
    while (currentPlayerKnights)
    {
        uint32_t knight = std::countr_zero(currentPlayerKnights);
        uint32_t fromSquare = squareForMove(knight);
        uint64_t knightMoves = ~position.getPieceSet(position.getCurrentPlayer()) & knightAttacksEmptyBoard[knight];
        uint64_t knightAttacks = position.getPieceSet(position.getOtherPlayer()) & knightMoves;
        // std::cout << "Knight attacks: " << knightAttacks << "\n";
        // std::cout << "Knight moves: " << knightMoves << "\n";
        knightMoves &= ~knightAttacks;
        while (knightAttacks)
        {
            moves[*n_moves] = {fromSquare,
                               squareForMove(std::countr_zero(knightAttacks)),
                               Move::capture, Types::knights};
            knightAttacks &= knightAttacks - 1;
            (*n_moves)++;
        }
        while (knightMoves)
        {
            moves[*n_moves] = {fromSquare,
                               squareForMove(std::countr_zero(knightMoves)),
                               Move::quiet, Types::knights};

            knightMoves &= knightMoves - 1;
            (*n_moves)++;
        }
        currentPlayerKnights &= currentPlayerKnights - 1;
    }
}

void MoveGenerator::generateKingPseudoLegalMoves(Move *moves,
                                                 Position position, int *n_moves)
{
    uint64_t currentPlayerKing = position.getPieceSet(position.getCurrentPlayer(), Types::kings);
    uint32_t king = std::countr_zero(currentPlayerKing);
    uint32_t fromSquare = squareForMove(king);
    uint64_t kingMoves = ~position.getPieceSet(position.getCurrentPlayer()) & kingAttacksEmptyBoard[king];
    uint64_t kingAttacks = position.getPieceSet(position.getOtherPlayer()) & kingMoves;
    // std::cout << "King attacks: " << kingAttacks << "\n";
    // std::cout << "King moves: " << kingMoves << "\n";
    kingMoves &= ~kingAttacks;
    while (kingAttacks)
    {
        moves[*n_moves] = {fromSquare,
                           squareForMove(std::countr_zero(kingAttacks)),
                           Move::capture, Types::kings};
        kingAttacks &= kingAttacks - 1;
        (*n_moves)++;
    }
    while (kingMoves)
    {
        moves[*n_moves] = {fromSquare,
                           squareForMove(std::countr_zero(kingMoves)),
                           Move::quiet, Types::kings};
        kingMoves &= kingMoves - 1;
        (*n_moves)++;
    }
    if (shortCastleLegal(position, position.getCurrentPlayer()))
    {
        if (position.getCurrentPlayer() == Types::white)
        {
            moves[*n_moves] = {fromSquare, squareForMove(1), Move::shortCastle, Types::kings};
            (*n_moves)++;
        }
        else
        {
            moves[*n_moves] = {fromSquare, squareForMove(57), Move::shortCastle, Types::kings};
            (*n_moves)++;
        }
    }
    if (longCastleLegal(position, position.getCurrentPlayer()))
    {
        if (position.getCurrentPlayer() == Types::white)
        {
            moves[*n_moves] = {fromSquare, squareForMove(5), Move::longCastle, Types::kings};
            (*n_moves)++;
        }
        else
        {
            moves[*n_moves] = {fromSquare, squareForMove(61), Move::longCastle, Types::kings};
            (*n_moves)++;
        }
    }
}

void MoveGenerator::generatePawnPseudoLegalMoves(Move *moves,
                                                 Position position, int *n_moves)
{
    uint64_t currentPlayerPawns = position.getPieceSet(position.getCurrentPlayer(), Types::pawns);
    if (position.getCurrentPlayer() == Types::white)
    {
        uint64_t whiteSinglePushNoPromotion = whiteSinglePushTargets(currentPlayerPawns, position) & not8Rank;
        while (whiteSinglePushNoPromotion)
        {
            int lastPawn = std::countr_zero(whiteSinglePushNoPromotion);
            moves[*n_moves] = {squareForMove(lastPawn - 8),
                               squareForMove(lastPawn), Move::quiet,
                               Types::pawns};
            (*n_moves)++;
            whiteSinglePushNoPromotion &= whiteSinglePushNoPromotion - 1;
        }
        uint64_t whiteSinglePushPromotion = whiteSinglePushTargets(currentPlayerPawns, position) & ~not8Rank;
        while (whiteSinglePushPromotion)
        {
            int lastPawn = std::countr_zero(whiteSinglePushPromotion);
            moves[*n_moves] = {squareForMove(lastPawn - 8),
                               squareForMove(lastPawn), Move::knightPromotion,
                               Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn - 8),
                               squareForMove(lastPawn), Move::bishopPromotion,
                               Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn - 8),
                               squareForMove(lastPawn), Move::queenPromotion,
                               Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn - 8),
                               squareForMove(lastPawn), Move::rookPromotion,
                               Types::pawns};
            (*n_moves)++;
            whiteSinglePushPromotion &= whiteSinglePushPromotion - 1;
        }
        uint64_t whiteDoublePush = whiteDoublePushTargets(currentPlayerPawns, position);
        while (whiteDoublePush)
        {
            int lastPawn = std::countr_zero(whiteDoublePush);
            moves[*n_moves] = {squareForMove(lastPawn - 16),
                               squareForMove(lastPawn), Move::doublePush,
                               Types::pawns};
            (*n_moves)++;
            whiteDoublePush &= whiteDoublePush - 1;
        }
        uint64_t whitePawnEastAttacksNoPromotion = (whitePawnEastAttackTargets(currentPlayerPawns) & position.getPieceSet(position.getOtherPlayer())) & not8Rank;
        // std::cout << "White pawn east attacks no promotion: " << whitePawnEastAttacksNoPromotion << "\n";
        // std::cout << "WhitePawnEastAttackTargets: " << whitePawnEastAttackTargets(currentPlayerPawns) << "\n";
        while (whitePawnEastAttacksNoPromotion)
        {
            int lastPawn = std::countr_zero(whitePawnEastAttacksNoPromotion);
            moves[*n_moves] = {squareForMove(lastPawn - 7),
                               squareForMove(lastPawn), Move::capture,
                               Types::pawns};
            (*n_moves)++;
            whitePawnEastAttacksNoPromotion &= whitePawnEastAttacksNoPromotion - 1;
        }
        uint64_t whitePawnEastAttacksPromotion = (whitePawnEastAttackTargets(currentPlayerPawns) & position.getPieceSet(position.getOtherPlayer())) & ~not8Rank;
        // std::cout << "White pawn east attacks promotion: " << whitePawnEastAttacksPromotion << "\n";
        while (whitePawnEastAttacksPromotion)
        {
            int lastPawn = std::countr_zero(whitePawnEastAttacksPromotion);
            moves[*n_moves] = {squareForMove(lastPawn - 7),
                               squareForMove(lastPawn),
                               Move::knightPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn - 7),
                               squareForMove(lastPawn),
                               Move::bishopPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn - 7),
                               squareForMove(lastPawn),
                               Move::queenPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn - 7),
                               squareForMove(lastPawn), Move::rookPromotionCapture,
                               Types::pawns};
            (*n_moves)++;
            whitePawnEastAttacksPromotion &= whitePawnEastAttacksPromotion - 1;
        }
        uint64_t whitePawnEastAttackEnPassant = whitePawnEastAttackTargets(currentPlayerPawns) & position.getEnPassantSquare();
        if (whitePawnEastAttackEnPassant)
        {
            int lastPawn = std::countr_zero(whitePawnEastAttackEnPassant);
            moves[*n_moves] = {squareForMove(lastPawn - 7),
                               squareForMove(lastPawn), Move::enPassant,
                               Types::pawns};
            (*n_moves)++;
        }
        uint64_t whitePawnWestAttacksNoPromotion = whitePawnWestAttackTargets(currentPlayerPawns) & position.getPieceSet(position.getOtherPlayer()) & not8Rank;
        // std::cout << "WhitePawnWestAttackTargets: " << whitePawnWestAttackTargets(currentPlayerPawns) << "\n";
        while (whitePawnWestAttacksNoPromotion)
        {
            int lastPawn = std::countr_zero(whitePawnWestAttacksNoPromotion);
            moves[*n_moves] = {squareForMove(lastPawn - 9),
                               squareForMove(lastPawn), Move::capture,
                               Types::pawns};
            (*n_moves)++;
            whitePawnWestAttacksNoPromotion &= whitePawnWestAttacksNoPromotion - 1;
        }
        uint64_t whitePawnWestAttacksPromotion = whitePawnWestAttackTargets(currentPlayerPawns) & position.getPieceSet(position.getOtherPlayer()) & ~not8Rank;
        while (whitePawnWestAttacksPromotion)
        {
            int lastPawn = std::countr_zero(whitePawnWestAttacksPromotion);
            moves[*n_moves] = {squareForMove(lastPawn - 9),
                               squareForMove(lastPawn),
                               Move::knightPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn - 9),
                               squareForMove(lastPawn),
                               Move::bishopPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn - 9),
                               squareForMove(lastPawn),
                               Move::queenPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn - 9),
                               squareForMove(lastPawn), Move::rookPromotionCapture,
                               Types::pawns};
            (*n_moves)++;
            whitePawnWestAttacksPromotion &= whitePawnWestAttacksPromotion - 1;
        }
        uint64_t whitePawnWestAttackEnPassant = whitePawnWestAttackTargets(currentPlayerPawns) & position.getEnPassantSquare();
        if (whitePawnWestAttackEnPassant)
        {
            int lastPawn = std::countr_zero(whitePawnWestAttackEnPassant);
            moves[*n_moves] = {squareForMove(lastPawn - 9),
                               squareForMove(lastPawn), Move::enPassant,
                               Types::pawns};
            (*n_moves)++;
        }
    }
    if (position.getCurrentPlayer() == Types::black)
    {
        uint64_t blackSinglePushNoPromotion = blackSinglePushTargets(currentPlayerPawns, position) & not1Rank;
        while (blackSinglePushNoPromotion)
        {
            int lastPawn = std::countr_zero(blackSinglePushNoPromotion);
            moves[*n_moves] = {squareForMove(lastPawn + 8),
                               squareForMove(lastPawn), Move::quiet,
                               Types::pawns};
            (*n_moves)++;
            blackSinglePushNoPromotion &= blackSinglePushNoPromotion - 1;
        }
        uint64_t blackSinglePushPromotion = blackSinglePushTargets(currentPlayerPawns, position) & ~not1Rank;
        while (blackSinglePushPromotion)
        {
            int lastPawn = std::countr_zero(blackSinglePushPromotion);
            moves[*n_moves] = {squareForMove(lastPawn + 8),
                               squareForMove(lastPawn), Move::knightPromotion,
                               Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn + 8),
                               squareForMove(lastPawn), Move::bishopPromotion,
                               Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn + 8),
                               squareForMove(lastPawn), Move::queenPromotion,
                               Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn + 8),
                               squareForMove(lastPawn), Move::rookPromotion,
                               Types::pawns};
            (*n_moves)++;
            blackSinglePushPromotion &= blackSinglePushPromotion - 1;
        }
        uint64_t blackDoublePush = blackDoublePushTargets(currentPlayerPawns, position);
        while (blackDoublePush)
        {
            int lastPawn = std::countr_zero(blackDoublePush);
            moves[*n_moves] = {squareForMove(lastPawn + 16),
                               squareForMove(lastPawn), Move::doublePush,
                               Types::pawns};
            (*n_moves)++;
            blackDoublePush &= blackDoublePush - 1;
        }
        uint64_t blackPawnEastAttacksNoPromotion = (blackPawnEastAttackTargets(currentPlayerPawns) & position.getPieceSet(position.getOtherPlayer())) & not1Rank;
        while (blackPawnEastAttacksNoPromotion)
        {
            int lastPawn = std::countr_zero(blackPawnEastAttacksNoPromotion);
            moves[*n_moves] = {squareForMove(lastPawn + 9),
                               squareForMove(lastPawn), Move::capture,
                               Types::pawns};
            (*n_moves)++;
            blackPawnEastAttacksNoPromotion &= blackPawnEastAttacksNoPromotion - 1;
        }
        uint64_t blackPawnEastAttacksPromotion = (blackPawnEastAttackTargets(currentPlayerPawns) & position.getPieceSet(position.getOtherPlayer())) & ~not1Rank;
        while (blackPawnEastAttacksPromotion)
        {
            int lastPawn = std::countr_zero(blackPawnEastAttacksPromotion);
            moves[*n_moves] = {squareForMove(lastPawn + 9),
                               squareForMove(lastPawn),
                               Move::knightPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn + 9),
                               squareForMove(lastPawn),
                               Move::bishopPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn + 9),
                               squareForMove(lastPawn),
                               Move::queenPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn + 9),
                               squareForMove(lastPawn), Move::rookPromotionCapture,
                               Types::pawns};
            (*n_moves)++;
            blackPawnEastAttacksPromotion &= blackPawnEastAttacksPromotion - 1;
        }
        uint64_t blackPawnEastAttackEnPassant = blackPawnEastAttackTargets(currentPlayerPawns) & position.getEnPassantSquare();
        if (blackPawnEastAttackEnPassant)
        {
            int lastPawn = std::countr_zero(blackPawnEastAttackEnPassant);
            moves[*n_moves] = {squareForMove(lastPawn + 9),
                               squareForMove(lastPawn), Move::enPassant,
                               Types::pawns};
            (*n_moves)++;
        }
        uint64_t blackPawnWestAttacksNoPromotion = blackPawnWestAttackTargets(currentPlayerPawns) & position.getPieceSet(position.getOtherPlayer()) & not1Rank;
        while (blackPawnWestAttacksNoPromotion)
        {
            int lastPawn = std::countr_zero(blackPawnWestAttacksNoPromotion);
            moves[*n_moves] = {squareForMove(lastPawn + 7),
                               squareForMove(lastPawn), Move::capture,
                               Types::pawns};
            (*n_moves)++;
            blackPawnWestAttacksNoPromotion &= blackPawnWestAttacksNoPromotion - 1;
        }
        uint64_t blackPawnWestAttacksPromotion = blackPawnWestAttackTargets(currentPlayerPawns) & position.getPieceSet(position.getOtherPlayer()) & ~not1Rank;
        while (blackPawnWestAttacksPromotion)
        {
            int lastPawn = std::countr_zero(blackPawnWestAttacksPromotion);
            moves[*n_moves] = {squareForMove(lastPawn + 7),
                               squareForMove(lastPawn),
                               Move::knightPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn + 7),
                               squareForMove(lastPawn),
                               Move::bishopPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn + 7),
                               squareForMove(lastPawn),
                               Move::queenPromotionCapture, Types::pawns};
            (*n_moves)++;
            moves[*n_moves] = {squareForMove(lastPawn + 7),
                               squareForMove(lastPawn), Move::rookPromotionCapture,
                               Types::pawns};
            (*n_moves)++;
            blackPawnWestAttacksPromotion &= blackPawnWestAttacksPromotion - 1;
        }
        uint64_t blackPawnWestAttackEnPassant = blackPawnWestAttackTargets(currentPlayerPawns) & position.getEnPassantSquare();
        if (blackPawnWestAttackEnPassant)
        {
            int lastPawn = std::countr_zero(blackPawnWestAttackEnPassant);
            moves[*n_moves] = {squareForMove(lastPawn + 7),
                               squareForMove(lastPawn), Move::enPassant,
                               Types::pawns};
            (*n_moves)++;
        }
    }
}

uint64_t MoveGenerator::southFill(uint64_t square)
{
    square |= (square >> 8);
    square |= (square >> 16);
    square |= (square >> 32);
    return square;
}

uint64_t MoveGenerator::northFill(uint64_t square)
{
    square |= (square << 8);
    square |= (square << 16);
    square |= (square << 32);
    return square;
}

uint64_t MoveGenerator::westFill(uint64_t square)
{
    const uint64_t pr0 = notHFile;
    const uint64_t pr1 = pr0 & (pr0 << 1);
    const uint64_t pr2 = pr1 & (pr1 << 2);
    square |= pr0 & (square << 1);
    square |= pr1 & (square << 2);
    square |= pr2 & (square << 4);
    return square;
}

uint64_t MoveGenerator::eastFill(uint64_t square)
{
    const uint64_t pr0 = notAFile;
    const uint64_t pr1 = pr0 & (pr0 >> 1);
    const uint64_t pr2 = pr1 & (pr1 >> 2);
    square |= pr0 & (square >> 1);
    square |= pr1 & (square >> 2);
    square |= pr2 & (square >> 4);
    return square;
}

uint64_t MoveGenerator::soWeFill(uint64_t square)
{
    const uint64_t pr0 = notHFile;
    const uint64_t pr1 = pr0 & (pr0 << 1);
    const uint64_t pr2 = pr1 & (pr1 << 2);
    square |= pr0 & (square >> 7);
    square |= pr1 & (square >> 14);
    square |= pr2 & (square >> 28);
    return square;
}

uint64_t MoveGenerator::noWeFill(uint64_t square)
{
    const uint64_t pr0 = notHFile;
    const uint64_t pr1 = pr0 & (pr0 << 1);
    const uint64_t pr2 = pr1 & (pr1 << 2);
    square |= pr0 & (square << 9);
    square |= pr1 & (square << 18);
    square |= pr2 & (square << 36);
    return square;
}

uint64_t MoveGenerator::soEaFill(uint64_t square)
{
    const uint64_t pr0 = notAFile;
    const uint64_t pr1 = pr0 & (pr0 >> 1);
    const uint64_t pr2 = pr1 & (pr1 >> 2);
    square |= pr0 & (square >> 9);
    square |= pr1 & (square >> 18);
    square |= pr2 & (square >> 36);
    return square;
}

uint64_t MoveGenerator::noEaFill(uint64_t square)
{
    const uint64_t pr0 = notAFile;
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
    uint64_t result = notABFile & (square << 6);
    // std::cout << "Knight attack step 1: " << result << "\n";
    result |= notGHFile & (square << 10);
    // std::cout << "Knight attack step 2: " << result << "\n";
    result |= notAFile & (square << 15);
    // std::cout << "Knight attack step 3: " << result << "\n";
    result |= notHFile & (square << 17);
    // std::cout << "Knight attack step 4: " << result << "\n";
    result |= notABFile & (square >> 10);
    // std::cout << "Knight attack step 5: " << result << "\n";
    result |= notHFile & (square >> 15);
    // std::cout << "Knight attack step 6: " << result << "\n";
    result |= notAFile & (square >> 17);
    // std::cout << "Knight attack step 7: " << result << "\n";
    result |= notGHFile & (square >> 6);
    // std::cout << "Knight attack step 8: " << result << "\n";
    return result;
}

uint64_t MoveGenerator::kingAttacks(uint64_t square)
{
    uint64_t result = notHFile & (square << 1);
    result |= (notAFile & (square << 7));
    result |= (square << 8);
    result |= (notHFile & (square << 9));
    result |= (notAFile & (square >> 1));
    result |= (notHFile & (square >> 7));
    result |= (square >> 8);
    result |= (notAFile & (square >> 9));
    return result;
}

void MoveGenerator::generateBishopAttacksArray()
{
    for (int i = 0; i < 64; ++i)
    {
        bishopAttacksEmptyBoard[i] = bishopAttacks(uint64_t(1) << i);
        // std::cout << "Bishop attacks for square " << i << ": " << bishopAttacksEmptyBoard[i] << "\n";
        bishopBlockersAndBeyond[i] = bishopAttacksEmptyBoard[i] & notOuterLines;
    }
}

void MoveGenerator::generateRookAttacksArray()
{
    for (int i = 0; i < 64; ++i)
    {
        rookAttacksEmptyBoard[i] = rookAttacks(uint64_t(1) << i);
        // std::cout << "Rook attacks for square " << i << ": " << rookAttacksEmptyBoard[i] << "\n";

        if (i == 0 || i == 7 || i == 56 || i == 63)
        {
            rookBlockersAndBeyond[i] = rookAttacksEmptyBoard[i] & notCorners;
        }
        else if (i < 8)
        {
            rookBlockersAndBeyond[i] = rookAttacksEmptyBoard[i] & notOuterLinesBut1Rank;
        }
        else if (i > 55)
        {
            rookBlockersAndBeyond[i] = rookAttacksEmptyBoard[i] & notOuterLinesBut8Rank;
        }
        else if (i % 8 == 0)
        {
            rookBlockersAndBeyond[i] = rookAttacksEmptyBoard[i] & notOuterLinesButHFile;
        }
        else if (i % 8 == 7)
        {
            rookBlockersAndBeyond[i] = rookAttacksEmptyBoard[i] & notOuterLinesButAFile;
        }
        else
        {
            rookBlockersAndBeyond[i] = rookAttacksEmptyBoard[i] & notOuterLines;
        }
    }
}

void MoveGenerator::generateQueenAttacksArray()
{
    for (int i = 0; i < 64; ++i)
    {
        queenAttacksEmptyBoard[i] = bishopAttacksEmptyBoard[i] | rookAttacksEmptyBoard[i];
        queenBlockersAndBeyond[i] = queenAttacksEmptyBoard[i] & notOuterLines;
    }
}

void MoveGenerator::generateKnightAttacksArray()
{
    for (int i = 0; i < 64; ++i)
    {
        knightAttacksEmptyBoard[i] = knightAttacks(uint64_t(1) << i);
        // std::cout << "Knight attacks for square " << i << ": " << knightAttacksEmptyBoard[i] << "\n";
    }
}

void MoveGenerator::generateKingAttacksArray()
{
    for (int i = 0; i < 64; ++i)
    {
        kingAttacksEmptyBoard[i] = kingAttacks(uint64_t(1) << i);
    }
}

void MoveGenerator::generateBehindArray()
{
    for (int i = 0; i < 64; ++i)
    {
        for (int j = i + 1; j < i + 8 - (i % 8); ++j)
        {
            behind[i][j] = (uint64_t(1) << j) ^ (westFill(uint64_t(1) << i) & westFill(uint64_t(1) << j));
        }
        for (int j = i - 1; j >= i - (i % 8); --j)
        {
            behind[i][j] = (uint64_t(1) << j) ^ (eastFill(uint64_t(1) << i) & eastFill(uint64_t(1) << j));
        }
        for (int j = i + 8; j < 64; j += 8)
        {
            behind[i][j] = (uint64_t(1) << j) ^ (northFill(uint64_t(1) << i) & northFill(uint64_t(1) << j));
        }
        for (int j = i - 8; j >= 0; j -= 8)
        {
            behind[i][j] = (uint64_t(1) << j) ^ (southFill(uint64_t(1) << i) & southFill(uint64_t(1) << j));
        }
        for (int j = i + 9; j < 64; j += 9)
        {
            behind[i][j] = (uint64_t(1) << j) ^ (noWeFill(uint64_t(1) << i) & noWeFill(uint64_t(1) << j));
        }
        for (int j = i + 7; j < 64; j += 7)
        {
            behind[i][j] = (uint64_t(1) << j) ^ (noEaFill(uint64_t(1) << i) & noEaFill(uint64_t(1) << j));
        }
        for (int j = i - 7; j >= 0; j -= 7)
        {
            behind[i][j] = (uint64_t(1) << j) ^ (soWeFill(uint64_t(1) << i) & soWeFill(uint64_t(1) << j));
        }
        for (int j = i - 9; j >= 0; j -= 9)
        {
            behind[i][j] = (uint64_t(1) << j) ^ (soEaFill(uint64_t(1) << i) & soEaFill(uint64_t(1) << j));
        }
    }
}

uint64_t MoveGenerator::singleBishopMoves(uint32_t pos, Position position)
{
    uint64_t result = bishopAttacksEmptyBoard[pos];
    for (uint64_t b = position.getAllPieces() & bishopBlockersAndBeyond[pos];
         b != 0; b &= (b - 1))
    {
        int sq = std::countr_zero(b);
        result &= ~behind[pos][sq];
    }
    return result;
}

uint64_t MoveGenerator::singleRookMoves(uint32_t pos, Position position)
{
    uint64_t result = rookAttacksEmptyBoard[pos];
    // std::cout << "Rook attacks empty board from " << pos << ": " << result << "\n";
    for (uint64_t b = position.getAllPieces() & rookBlockersAndBeyond[pos];
         b != 0; b &= (b - 1))
    {
        int sq = std::countr_zero(b);
        result &= ~behind[pos][sq];
    }
    return result;
}

uint64_t MoveGenerator::singleQueenMoves(uint32_t pos, Position position)
{
    uint64_t result = queenAttacksEmptyBoard[pos];
    for (uint64_t b = position.getAllPieces() & queenBlockersAndBeyond[pos];
         b != 0; b &= (b - 1))
    {
        int sq = std::countr_zero(b);
        result &= ~behind[pos][sq];
    }
    return result;
}

uint64_t MoveGenerator::whiteDoublePushTargets(uint64_t pawns,
                                               Position position)
{
    return northOne(whiteSinglePushTargets(pawns, position)) & fourthRank & ~position.getAllPieces();
}

uint64_t MoveGenerator::blackDoublePushTargets(uint64_t pawns,
                                               Position position)
{
    return southOne(blackSinglePushTargets(pawns, position)) & fifthRank & ~position.getAllPieces();
}

uint32_t MoveGenerator::squareForMove(int square)
{
    return ((square % 8) << 3) | (square / 8);
}

bool MoveGenerator::attacked(uint64_t square,
                             Position position,
                             Types::PieceEnum byColor)
{
    uint64_t pawns = position.getPieceSet(Types::pawns, byColor);
    if (byColor == Types::white)
    {
        if (blackPawnEastAttackTargets(square) & pawns || blackPawnWestAttackTargets(square) & pawns)
            return true;
    }
    else
    {
        if (whitePawnEastAttackTargets(square) & pawns || whitePawnWestAttackTargets(square) & pawns)
            return true;
    }
    int square_int = std::countr_zero(square);
    if (position.getPieceSet(Types::knights, byColor) & knightAttacksEmptyBoard[square_int])
        return true;
    if (position.getPieceSet(Types::kings, byColor) & kingAttacksEmptyBoard[square_int])
        return true;
    uint64_t bishopMoves = singleBishopMoves(square_int, position);
    uint64_t queens = position.getPieceSet(Types::queens, byColor);
    if (bishopMoves & (position.getPieceSet(Types::bishops, byColor) | queens))
        return true;
    uint64_t rookMoves = singleRookMoves(square_int, position);
    if (rookMoves & (position.getPieceSet(Types::rooks, byColor) | queens))
        return true;
    return false;
}

bool MoveGenerator::shortCastleLegal(Position position, Types::PieceEnum color)
{
    uint8_t castlingRights = position.getCastlingRights();
    Types::PieceEnum opponent = (color == Types::white) ? Types::black : Types::white;
    if (color == Types::white)
    {
        if (!(castlingRights & 0b0001))
        {
            return false;
        }
        uint64_t square_checked = position.getPieceSet(color, Types::kings);
        if (attacked(square_checked, position, opponent))
        {
            return false;
        }
        square_checked >>= 1;
        while (square_checked != 0b1)
        {
            if (attacked(square_checked, position, opponent))
            {
                return false;
            }
            if (position.getAllPieces() & square_checked)
            {
                return false;
            }
            square_checked >>= 1;
        }
    }
    else
    {
        if (!(castlingRights & 0b0100))
            return false;
        uint64_t square_checked = position.getPieceSet(color, Types::kings);
        // std::cout << square_checked << std::endl;
        // std::cout << position.getPieceSet(Types::kings) << std::endl;
        // std::cout << position.getPieceSet(color) << std::endl;
        if (attacked(square_checked, position, opponent))
            return false;
        square_checked >>= 1;
        while (square_checked != 0x100000000000000)
        {
            if (attacked(square_checked, position, opponent))
                return false;
            if (position.getAllPieces() & square_checked)
                return false;
            square_checked >>= 1;
        }
    }
    return true;
}

bool MoveGenerator::longCastleLegal(Position position, Types::PieceEnum color)
{
    uint8_t castlingRights = position.getCastlingRights();
    Types::PieceEnum opponent = (color == Types::white) ? Types::black : Types::white;
    if (color == Types::white)
    {
        if (!(castlingRights & 0b0010))
            return false;
        uint64_t square_checked = position.getPieceSet(color, Types::kings);
        if (attacked(square_checked, position, opponent))
            return false;
        square_checked <<= 1;
        while (square_checked != 0b1000000)
        {
            if (attacked(square_checked, position, opponent))
                return false;
            if (position.getAllPieces() & square_checked)
                return false;
            square_checked <<= 1;
        }
        if (position.getAllPieces() & square_checked)
            return false;
    }
    else
    {
        if (!(castlingRights & 0b1000))
            return false;
        uint64_t square_checked = position.getPieceSet(color, Types::kings);
        if (attacked(square_checked, position, opponent))
            return false;
        square_checked <<= 1;
        while (square_checked != 0x4000000000000000)
        {
            if (attacked(square_checked, position, opponent))
                return false;
            if (position.getAllPieces() & square_checked)
                return false;
            square_checked <<= 1;
        }
        if (position.getAllPieces() & square_checked)
            return false;
    }
    return true;
}
