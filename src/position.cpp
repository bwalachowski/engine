#include "position.h"
#include "move.h"
#include "move_generator.h"
#include <random>
#include <cassert>

uint8_t Position::prevCastlingRights[64] = {0};
uint64_t Position::hashPieceNumbers[12][64] = {{0}};
uint64_t Position::hashBlackToMove = 0;
uint64_t Position::hashCastlingRights[16] = {0};
uint64_t Position::hashEnPassantSquare[8] = {0};
uint64_t Position::prevEnPassantSquares[64] = {0};
uint64_t Position::prevHashes[64] = {0};
uint64_t Position::repetitionTable[120] = {0};
int Position::prevRepetitionIndices[64] = {0};
bool Position::hashesInitialized = false;
int Position::prevPlies[64] = {0};

Position::Position(Board board, Types::PieceEnum currentPlayer, uint8_t castlingRights)
    : board(board),
      currentPlayer(currentPlayer), castlingRights(castlingRights)
{
    plySinceCaptureOrPawnMove = 0;
    enPassantSquare = 0;
    depth = 0;
    repetitionIndex = 0;
    initializeHashNumbers();
    repetitionTable[repetitionIndex] = getZobristHash();

    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
    prevRepetitionIndices[depth] = repetitionIndex;
}

Position::Position(Board board, uint64_t enPassantSquare, Types::PieceEnum currentPlayer, uint8_t castlingRights) : board(board), enPassantSquare(enPassantSquare), currentPlayer(currentPlayer), castlingRights(castlingRights)
{
    plySinceCaptureOrPawnMove = 0;
    depth = 0;
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
    repetitionIndex = 0;
    prevRepetitionIndices[depth] = repetitionIndex;
    initializeHashNumbers();
    repetitionTable[repetitionIndex] = getZobristHash();
}

Position::Position(Board board, uint64_t enPassantSquare, Types::PieceEnum currentPlayer, uint8_t castlingRights, int repetitionIndex, int depth, uint64_t hash, int plySinceCaptureOrPawnMove) : board(board),
                                                                                                                                                                                                  enPassantSquare(enPassantSquare),
                                                                                                                                                                                                  currentPlayer(currentPlayer),
                                                                                                                                                                                                  castlingRights(castlingRights),
                                                                                                                                                                                                  repetitionIndex(repetitionIndex),
                                                                                                                                                                                                  depth(depth),
                                                                                                                                                                                                  plySinceCaptureOrPawnMove(plySinceCaptureOrPawnMove)

{
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
    initializeHashNumbers();
    repetitionTable[repetitionIndex] = hash;
    prevRepetitionIndices[depth] = repetitionIndex;
}

Position::Position(std::string fen)
    : board(fen)
{
    currentPlayer = (fen.find(" w ") != std::string::npos) ? Types::white : Types::black;
    size_t castlingPos = fen.find(" ") + 1;
    castlingPos = fen.find(" ", castlingPos) + 1;
    std::string castlingRightsStr = fen.substr(castlingPos, fen.find(" ", castlingPos) - castlingPos);
    castlingRights = 0;
    if (castlingRightsStr.find('K') != std::string::npos)
        castlingRights |= 0b0001;
    if (castlingRightsStr.find('Q') != std::string::npos)
        castlingRights |= 0b0010;
    if (castlingRightsStr.find('k') != std::string::npos)
        castlingRights |= 0b0100;
    if (castlingRightsStr.find('q') != std::string::npos)
        castlingRights |= 0b1000;

    size_t enPassantPos = fen.find(" ", castlingPos) + 1;
    std::string enPassantStr = fen.substr(enPassantPos, fen.find(" ", enPassantPos) - enPassantPos);
    if (enPassantStr == "-")
    {
        enPassantSquare = 0;
    }
    else
    {
        char file = enPassantStr[0];
        char rank = enPassantStr[1];
        int fileIndex = 'h' - file;
        int rankIndex = rank - '1';
        enPassantSquare = 1ULL << (rankIndex * 8 - fileIndex);
    }
    size_t plySinceCaptureOrPawnMovePos = fen.find(" ", castlingPos) + 1;
    std::string plySinceCaptureOrPawnMoveStr = fen.substr(plySinceCaptureOrPawnMovePos, fen.find(" ", enPassantPos) - enPassantPos);
    plySinceCaptureOrPawnMove = std::stoi(plySinceCaptureOrPawnMoveStr);

    depth = 0;
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
    repetitionIndex = 0;
    prevRepetitionIndices[depth] = repetitionIndex;
    initializeHashNumbers();
    repetitionTable[repetitionIndex] = getZobristHash();
}

Position::Position(std::string pieces, std::string color, std::string castlingRightsFen, std::string enPassantSquareFen, std::string plySinceCaptureOrPawnMoveFen) : board(pieces)
{
    currentPlayer = (color == "w") ? Types::white : Types::black;
    castlingRights = 0;
    if (castlingRightsFen.find('K') != std::string::npos)
        castlingRights |= 0b0001;
    if (castlingRightsFen.find('Q') != std::string::npos)
        castlingRights |= 0b0010;
    if (castlingRightsFen.find('k') != std::string::npos)
        castlingRights |= 0b0100;
    if (castlingRightsFen.find('q') != std::string::npos)
        castlingRights |= 0b1000;

    if (enPassantSquareFen == "-")
    {
        enPassantSquare = 0;
    }
    else
    {
        char file = enPassantSquareFen[0];
        char rank = enPassantSquareFen[1];
        int fileIndex = 'h' - file;
        int rankIndex = rank - '1';
        enPassantSquare = 1ULL << (rankIndex * 8 - fileIndex);
    }
    depth = 0;
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
    repetitionIndex = 0;
    prevRepetitionIndices[depth] = repetitionIndex;
    initializeHashNumbers();
    repetitionTable[repetitionIndex] = getZobristHash();
    plySinceCaptureOrPawnMove = std::stoi(plySinceCaptureOrPawnMoveFen);
}

Position::Position()
    : board(Board()), enPassantSquare(0), currentPlayer(Types::white), castlingRights(0b1111)
{
    depth = 0;
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
    repetitionIndex = 0;
    prevRepetitionIndices[depth] = repetitionIndex;
    initializeHashNumbers();
    repetitionTable[repetitionIndex] = getZobristHash();
    plySinceCaptureOrPawnMove = 0;
}

void Position::initializeHashNumbers()
{
    if (!hashesInitialized)
    {
        std::random_device rd;
        std::mt19937_64 e2(rd());
        std::uniform_int_distribution<uint64_t> dist;
        for (int i = 0; i < 12; ++i)
        {
            for (int j = 0; j < 64; ++j)
            {
                hashPieceNumbers[i][j] = dist(e2);
            }
        }
        for (int i = 0; i < 16; ++i)
        {
            hashCastlingRights[i] = dist(e2);
        }
        for (int i = 0; i < 8; ++i)
        {
            hashEnPassantSquare[i] = dist(e2);
        }
        hashBlackToMove = dist(e2);
        hashesInitialized = true;
    }
}

uint64_t Position::getZobristHash()
{
    uint64_t hash = 0;
    for (int i = Types::white; i <= Types::black; ++i)
    {
        for (int j = Types::pawns; j <= Types::queens; ++j)
        {
            uint64_t pieceSet = getPieceSet(static_cast<Types::PieceEnum>(i), static_cast<Types::PieceEnum>(j));
            while (pieceSet)
            {
                uint32_t piece = std::countr_zero(pieceSet);
                hash ^= hashPieceNumbers[i * 6 + (j - 2)][piece];
                pieceSet &= pieceSet - 1;
            }
        }
    }
    if (currentPlayer == Types::black)
    {
        hash ^= hashBlackToMove;
    }
    hash ^= hashCastlingRights[castlingRights];
    int enPassantRank = std::countr_zero(enPassantSquare) % 8;
    hash ^= hashEnPassantSquare[enPassantRank];
    // std::cerr << hash << " 0" << std::endl;
    return hash;
}

bool Position::isDraw()
{
    if (plySinceCaptureOrPawnMove >= 100)
    {
        return true;
    }
    int repetitionCount = 1;
    for (int i = repetitionIndex % 2; i < repetitionIndex; i += 2)
    {
        // std::cerr << repetitionTable[i] << " " << i << std::endl;
        if (repetitionTable[i] == repetitionTable[repetitionIndex])
        {
            repetitionCount++;
            if (repetitionCount == 3)
            {
                return true;
            }
        }
    }
    return false;
}

Move Position::getMoveFromLongAlgebraicNotation(std::string longAlgebraicNotation)
{
    uint32_t fromFile = 'h' - (longAlgebraicNotation[0]);
    uint32_t fromRank = longAlgebraicNotation[1] - '1';
    uint32_t toFile = 'h' - (longAlgebraicNotation[2]);
    uint32_t toRank = longAlgebraicNotation[3] - '1';
    uint64_t fromSquare = 1ULL << (fromRank * 8 + fromFile);
    uint64_t toSquare = 1ULL << (toRank * 8 + toFile);
    uint32_t flags = Move::quiet;
    Types::PieceEnum piece = Types::pawns;

    for (int i = Types::pawns; i <= Types::queens; ++i)
    {
        if (fromSquare & getPieceSet(static_cast<Types::PieceEnum>(i)))
        {
            piece = static_cast<Types::PieceEnum>(i);
        }
    }
    if (toSquare & getAllPieces())
    {
        if (piece == Types::pawns && ((currentPlayer == Types::white && longAlgebraicNotation[3] == '8') ||
                                      (currentPlayer == Types::black && longAlgebraicNotation[3] == '1')))
        {
            if (longAlgebraicNotation[4] == 'q')
            {
                flags = Move::queenPromotionCapture;
            }
            else if (longAlgebraicNotation[4] == 'r')
            {
                flags = Move::rookPromotionCapture;
            }
            else if (longAlgebraicNotation[4] == 'b')
            {
                flags = Move::bishopPromotionCapture;
            }
            else if (longAlgebraicNotation[4] == 'n')
            {
                flags = Move::knightPromotionCapture;
            }
        }
        else
        {
            flags = Move::capture;
        }
    }
    else if (piece == Types::pawns && ((currentPlayer == Types::white && longAlgebraicNotation[3] == '8') ||
                                       (currentPlayer == Types::black && longAlgebraicNotation[3] == '1')))
    {
        if (longAlgebraicNotation[4] == 'q')
        {
            flags = Move::queenPromotion;
        }
        else if (longAlgebraicNotation[4] == 'r')
        {
            flags = Move::rookPromotion;
        }
        else if (longAlgebraicNotation[4] == 'b')
        {
            flags = Move::bishopPromotion;
        }
        else if (longAlgebraicNotation[4] == 'n')
        {
            flags = Move::knightPromotion;
        }
    }
    else if (piece == Types::pawns && ((longAlgebraicNotation[3] == '4' && longAlgebraicNotation[1] == '2') || (longAlgebraicNotation[3] == '5' && longAlgebraicNotation[1] == '7')))
    {
        flags = Move::doublePush;
    }
    else if (piece == Types::kings && (longAlgebraicNotation[2] == 'g' && longAlgebraicNotation[0] == 'e'))
    {
        flags = Move::shortCastle;
    }
    else if (piece == Types::kings && (longAlgebraicNotation[2] == 'c' && longAlgebraicNotation[0] == 'e'))
    {
        flags = Move::longCastle;
    }
    else if (piece == Types::pawns && (toSquare & enPassantSquare))
    {
        flags = Move::enPassant;
    }
    MoveGenerator generator;
    return Move(generator.squareForMove(std::countr_zero(fromSquare)), generator.squareForMove(std::countr_zero(toSquare)), flags, piece);
}

Position Position::makeMove(Move move, bool commit)
{
    Board newBoard = board;
    uint64_t fromSquare = move.getFromSquare();
    uint64_t toSquare = move.getToSquare();
    Types::PieceEnum piece = move.getPiece();
    uint32_t flags = move.getFlags();
    uint64_t hash = repetitionTable[repetitionIndex];
    int pieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(piece) - 2);
    // std::cerr << "hash number for piece: " << piece << " color: " << currentPlayer << " square: " << fromSquare << " : " << hashPieceNumbers[pieceIndex][std::countr_zero(fromSquare)] << std::endl;
    hash ^= hashPieceNumbers[pieceIndex][std::countr_zero(fromSquare)];
    // std::cerr << "new hash: " << hash << std::endl;

    if (flags >= Move::knightPromotion && flags <= Move::queenPromotionCapture)
    {
        int promotionPieceIndex;
        switch (flags)
        {
        case Move::knightPromotion:
        case Move::knightPromotionCapture:
            // std::cerr << "hash number for piece: " << Types::knights << " color: " << currentPlayer << " square: " << toSquare << " :" << hashPieceNumbers[pieceIndex][std::countr_zero(toSquare)] << std::endl;
            promotionPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(Types::knights) - 2);
            break;
        case Move::bishopPromotion:
        case Move::bishopPromotionCapture:
            // std::cerr << "hash number for piece: " << Types::bishops << " color: " << currentPlayer << " square: " << toSquare << " :" << hashPieceNumbers[pieceIndex][std::countr_zero(toSquare)] << std::endl;
            promotionPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(Types::bishops) - 2);
            break;
        case Move::rookPromotion:
        case Move::rookPromotionCapture:
            // std::cerr << "hash number for piece: " << Types::rooks << " color: " << currentPlayer << " square: " << toSquare << " :" << hashPieceNumbers[pieceIndex][std::countr_zero(toSquare)] << std::endl;
            promotionPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(Types::rooks) - 2);
            break;
        case Move::queenPromotion:
        case Move::queenPromotionCapture:
            // std::cerr << "hash number for piece: " << Types::queens << " color: " << currentPlayer << " square: " << toSquare << " :" << hashPieceNumbers[pieceIndex][std::countr_zero(toSquare)] << std::endl;
            promotionPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(Types::queens) - 2);
            break;
        default:
            promotionPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(piece) - 2); // Should not reach here
            break;
        }
        hash ^= hashPieceNumbers[promotionPieceIndex][std::countr_zero(toSquare)];
        // std::cerr << "new hash: " << hash << std::endl;
    }
    else
    {
        // std::cerr << "hash number for piece: " << piece << " color: " << currentPlayer << " square: " << toSquare << " :" << hashPieceNumbers[pieceIndex][std::countr_zero(toSquare)] << std::endl;
        hash ^= hashPieceNumbers[pieceIndex][std::countr_zero(toSquare)];
        // std::cerr << "new hash: " << hash << std::endl;
    }
    // std::cerr << "hash number for blackToMove: " << hashBlackToMove << std::endl;
    hash ^= hashBlackToMove;
    // std::cerr << "new hash: " << hash << std::endl;

    newBoard.makeMove(move, currentPlayer, depth);
    uint64_t newEnPassantSquare = 0;
    uint8_t newCastlingRights = castlingRights;
    if (flags == Move::doublePush)
    {
        if (currentPlayer == Types::white)
        {
            newEnPassantSquare = fromSquare << 8;
        }
        else
        {
            newEnPassantSquare = fromSquare >> 8;
        }
    }

    if (piece == Types::kings)
    {
        if (currentPlayer == Types::white)
        {
            newCastlingRights &= 0b1100;
        }
        else
        {
            newCastlingRights &= 0b0011;
        }
    }
    else if (piece == Types::rooks)
    {
        if (currentPlayer == Types::white)
        {
            if (fromSquare & h1Square)
                newCastlingRights &= 0b1110;
            else if (fromSquare & a1Square)
                newCastlingRights &= 0b1101;
        }
        else
        {
            if (fromSquare & h8Square)
                newCastlingRights &= 0b1011;
            else if (fromSquare & a8Square)
                newCastlingRights &= 0b0111;
        }
    }
    if (flags == Move::capture || (flags >= Move::knightPromotionCapture && flags <= Move::queenPromotionCapture))
    {
        int capturedPieceIndex = static_cast<int>(getOtherPlayer()) * 6 + (static_cast<int>(newBoard.getCapturedPiece(depth)) - 2);
        // std::cerr << "hash number for piece: " << newBoard.getCapturedPiece(depth) << " color: " << getOtherPlayer() << " square: " << toSquare << " :" << hashPieceNumbers[pieceIndex][std::countr_zero(fromSquare)] << std::endl;
        int toSquareIndex = std::countr_zero(toSquare);
        assert(toSquareIndex < 64);
        hash ^= hashPieceNumbers[capturedPieceIndex][toSquareIndex];
        // std::cerr << "new hash: " << hash << std::endl;
        if (toSquare & h1Square)
            newCastlingRights &= 0b1110;
        else if (toSquare & a1Square)
            newCastlingRights &= 0b1101;
        else if (toSquare & h8Square)
            newCastlingRights &= 0b1011;
        else if (toSquare & a8Square)
            newCastlingRights &= 0b0111;
    }
    Types::PieceEnum nextPlayer = (currentPlayer == Types::white) ? Types::black : Types::white;

    int enPassantRank = std::countr_zero(enPassantSquare) % 8;
    int newEnPassantRank = std::countr_zero(newEnPassantSquare) % 8;
    // std::cerr << "hash number for enPassantRank: " << enPassantRank << " : " << hashEnPassantSquare[enPassantRank] << std::endl;
    hash ^= hashEnPassantSquare[enPassantRank];
    // std::cerr << "new hash: " << hash << std::endl;
    // std::cerr << "hash number for enPassantRank: " << newEnPassantRank << " : " << hashEnPassantSquare[newEnPassantRank] << std::endl;
    hash ^= hashEnPassantSquare[newEnPassantRank];
    // std::cerr << "new hash: " << hash << std::endl;
    // std::cerr << "hash number for castlingRights: " << castlingRights << " : " << hashCastlingRights[castlingRights] << std::endl;
    hash ^= hashCastlingRights[castlingRights];
    // std::cerr << "new hash: " << hash << std::endl;
    // std::cerr << "hash number for castlingRights: " << newCastlingRights << " : " << hashCastlingRights[newCastlingRights] << std::endl;
    hash ^= hashCastlingRights[newCastlingRights];
    // std::cerr << "new hash: " << hash << std::endl;
    if (piece == Types::pawns || flags > Move::doublePush)
    {
        repetitionIndex = 0;
    }
    else
    {
        repetitionIndex++;
    }

    // std::cerr << hash << " " << repetitionIndex << std::endl;
    int newPlySinceCaptureOrPawnMove = plySinceCaptureOrPawnMove + 1;
    if (piece == Types::pawns || flags == Move::capture)
    {
        newPlySinceCaptureOrPawnMove = 0;
    }
    prevPlies[depth] = plySinceCaptureOrPawnMove;

    if (commit)
    {
        prevHashes[0] = repetitionTable[repetitionIndex];
        return Position(newBoard, newEnPassantSquare, nextPlayer, newCastlingRights, repetitionIndex, 0, hash, newPlySinceCaptureOrPawnMove);
    }
    prevHashes[depth + 1] = repetitionTable[repetitionIndex];
    return Position(newBoard, newEnPassantSquare, nextPlayer, newCastlingRights, repetitionIndex, depth + 1, hash, newPlySinceCaptureOrPawnMove);
}

bool Position::makeMoveCheckIfLegal(Move move)
{
    Board newBoard = board;
    uint64_t fromSquare = move.getFromSquare();
    uint64_t toSquare = move.getToSquare();
    Types::PieceEnum piece = move.getPiece();
    uint32_t flags = move.getFlags();

    uint64_t hash = repetitionTable[repetitionIndex];
    int pieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(piece) - 2);
    hash ^= hashPieceNumbers[pieceIndex][std::countr_zero(fromSquare)];

    if (flags >= Move::knightPromotion && flags <= Move::queenPromotionCapture)
    {
        int promotionPieceIndex;
        switch (flags)
        {
        case Move::knightPromotion:
        case Move::knightPromotionCapture:
            promotionPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(Types::knights) - 2);
            break;
        case Move::bishopPromotion:
        case Move::bishopPromotionCapture:
            promotionPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(Types::bishops) - 2);
            break;
        case Move::rookPromotion:
        case Move::rookPromotionCapture:
            promotionPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(Types::rooks) - 2);
            break;
        case Move::queenPromotion:
        case Move::queenPromotionCapture:
            promotionPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(Types::queens) - 2);
            break;
        default:
            promotionPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(piece) - 2); // Should not reach here
            break;
        }
        hash ^= hashPieceNumbers[promotionPieceIndex][std::countr_zero(toSquare)];
    }
    else
    {
        hash ^= hashPieceNumbers[pieceIndex][std::countr_zero(toSquare)];
    }

    hash ^= hashBlackToMove;

    newBoard.makeMove(move, currentPlayer, depth);
    uint64_t newEnPassantSquare = 0;
    uint8_t newCastlingRights = castlingRights;
    if (flags == Move::doublePush)
    {
        if (currentPlayer == Types::white)
        {
            newEnPassantSquare = fromSquare << 8;
        }
        else
        {
            newEnPassantSquare = fromSquare >> 8;
        }
    }

    if (piece == Types::kings)
    {
        if (currentPlayer == Types::white)
        {
            newCastlingRights &= 0b1100;
        }
        else
        {
            newCastlingRights &= 0b0011;
        }
    }
    else if (piece == Types::rooks)
    {
        if (currentPlayer == Types::white)
        {
            if (fromSquare & h1Square)
                newCastlingRights &= 0b1110;
            else if (fromSquare & a1Square)
                newCastlingRights &= 0b1101;
        }
        else
        {
            if (fromSquare & h8Square)
                newCastlingRights &= 0b1011;
            else if (fromSquare & a8Square)
                newCastlingRights &= 0b0111;
        }
    }
    if (flags == Move::capture || (flags >= Move::knightPromotionCapture && flags <= Move::queenPromotionCapture))
    {
        int capturedPieceIndex = static_cast<int>(getOtherPlayer()) * 6 + (static_cast<int>(newBoard.getCapturedPiece(depth)) - 2);
        int toSquareIndex = std::countr_zero(toSquare);
        assert(toSquareIndex < 64);
        hash ^= hashPieceNumbers[capturedPieceIndex][toSquareIndex];
        if (toSquare & h1Square)
            newCastlingRights &= 0b1110;
        else if (toSquare & a1Square)
            newCastlingRights &= 0b1101;
        else if (toSquare & h8Square)
            newCastlingRights &= 0b1011;
        else if (toSquare & a8Square)
            newCastlingRights &= 0b0111;
    }

    int enPassantRank = std::countr_zero(enPassantSquare) % 8;
    int newEnPassantRank = std::countr_zero(newEnPassantSquare) % 8;
    hash ^= hashEnPassantSquare[enPassantRank];
    hash ^= hashEnPassantSquare[newEnPassantRank];
    hash ^= hashCastlingRights[castlingRights];
    hash ^= hashCastlingRights[newCastlingRights];

    prevPlies[depth] = plySinceCaptureOrPawnMove;

    if (piece == Types::pawns || flags == Move::capture)
    {
        plySinceCaptureOrPawnMove = 0;
    }
    else
    {
        plySinceCaptureOrPawnMove = plySinceCaptureOrPawnMove + 1;
    }

    board = newBoard;
    enPassantSquare = newEnPassantSquare;
    castlingRights = newCastlingRights;
    depth += 1;
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
    Types::PieceEnum otherPlayer = currentPlayer;
    currentPlayer = (currentPlayer == Types::white) ? Types::black : Types::white;

    if (piece == Types::pawns || flags > Move::doublePush)
    {
        repetitionIndex = 0;
    }
    else
    {
        repetitionIndex++;
    }

    prevHashes[depth] = repetitionTable[repetitionIndex];
    repetitionTable[repetitionIndex] = hash;
    prevRepetitionIndices[depth] = repetitionIndex;

    MoveGenerator moveGen;
    if (moveGen.attacked(newBoard.getPieceSet(otherPlayer, Types::kings), *this, currentPlayer))
    {
        return false;
    }

    return true;
}

void Position::unmakeMove(Move move)
{
    Board newBoard = board;
    uint64_t fromSquare = move.getFromSquare();
    uint64_t toSquare = move.getToSquare();
    Types::PieceEnum piece = move.getPiece();
    uint32_t flags = move.getFlags();
    Types::PieceEnum opponent = getOtherPlayer();

    newBoard.addPiece(fromSquare, opponent, piece);

    // uint64_t hash = repetitionTable[repetitionIndex];
    // int pieceIndex = static_cast<int>(opponent) * 6 + (static_cast<int>(piece) - 2);
    // hash ^= hashPieceNumbers[pieceIndex][std::countr_zero(fromSquare)];

    // hash ^= hashBlackToMove;

    // Handle promotions
    if (flags >= Move::knightPromotion && flags <= Move::queenPromotionCapture)
    {
        // int promotionPieceIndex;
        switch (flags)
        {
        case Move::knightPromotion:
        case Move::knightPromotionCapture:
            newBoard.removePiece(toSquare, opponent, Types::knights);
            // promotionPieceIndex = static_cast<int>(opponent) * 6 + (static_cast<int>(Types::knights) - 2);
            break;
        case Move::bishopPromotion:
        case Move::bishopPromotionCapture:
            newBoard.removePiece(toSquare, opponent, Types::bishops);
            // promotionPieceIndex = static_cast<int>(opponent) * 6 + (static_cast<int>(Types::bishops) - 2);
            break;
        case Move::rookPromotion:
        case Move::rookPromotionCapture:
            newBoard.removePiece(toSquare, opponent, Types::rooks);
            // promotionPieceIndex = static_cast<int>(opponent) * 6 + (static_cast<int>(Types::rooks) - 2);
            break;
        case Move::queenPromotion:
        case Move::queenPromotionCapture:
            newBoard.removePiece(toSquare, opponent, Types::queens);
            // promotionPieceIndex = static_cast<int>(opponent) * 6 + (static_cast<int>(Types::queens) - 2);
            break;
        default:
            break; // Should not reach here
        }
        // hash ^= hashPieceNumbers[promotionPieceIndex][std::countr_zero(toSquare)];
    }
    else
    {
        newBoard.removePiece(toSquare, opponent, piece);
        // hash ^= hashPieceNumbers[pieceIndex][std::countr_zero(toSquare)];
    }

    if (flags == Move::capture || (flags >= Move::knightPromotionCapture && flags <= Move::queenPromotionCapture))
    {
        // std::cout << "Restoring captured piece: " << newBoard.getCapturedPiece() << "\n";
        // int capturedPieceIndex = static_cast<int>(currentPlayer) * 6 + (static_cast<int>(newBoard.getCapturedPiece(depth - 1)) - 2);
        // std::cout << static_cast<int>(currentPlayer) << " " << newBoard.getCapturedPiece(depth - 1) << "\n";
        // std::cout << capturedPieceIndex << std::endl;
        // hash ^= hashPieceNumbers[capturedPieceIndex][std::countr_zero(toSquare)];
        newBoard.addPiece(toSquare, currentPlayer, newBoard.getCapturedPiece(depth - 1));
    }

    // Handle en passant
    if (flags == Move::enPassant)
    {
        if (currentPlayer == Types::white)
        {
            newBoard.addPiece(toSquare << 8, currentPlayer, Types::pawns);
        }
        else
        {
            newBoard.addPiece(toSquare >> 8, currentPlayer, Types::pawns);
        }
    }

    if (flags == Move::shortCastle)
    {
        if (currentPlayer == Types::black)
        {
            newBoard.removePiece(f1Square, opponent, Types::rooks);
            newBoard.addPiece(h1Square, opponent, Types::rooks);
        }
        else
        {
            newBoard.removePiece(f8Square, opponent, Types::rooks);
            newBoard.addPiece(h8Square, opponent, Types::rooks);
        }
    }
    else if (flags == Move::longCastle)
    {
        if (currentPlayer == Types::black)
        {
            newBoard.removePiece(d1Square, opponent, Types::rooks);
            newBoard.addPiece(a1Square, opponent, Types::rooks);
        }
        else
        {
            newBoard.removePiece(d8Square, opponent, Types::rooks);
            newBoard.addPiece(a8Square, opponent, Types::rooks);
        }
    }

    // std::cout << "Unmaking move: " << move << "\n";
    // std::cout << "Board after unmaking:\n";
    // std::cout << "Flags: " << flags << "\n";
    // std::cout << newBoard.getPieceSet(Types::white, Types::pawns) << "\n";
    // std::cout << newBoard.getPieceSet(Types::black, Types::pawns) << "\n";
    // std::cout << newBoard.getPieceSet(Types::white, Types::knights) << "\n";
    // std::cout << newBoard.getPieceSet(Types::black, Types::knights) << "\n";
    // std::cout << newBoard.getPieceSet(Types::white, Types::bishops) << "\n";
    // std::cout << newBoard.getPieceSet(Types::black, Types::bishops) << "\n";
    // std::cout << newBoard.getPieceSet(Types::white, Types::rooks) << "\n";
    // std::cout << newBoard.getPieceSet(Types::black, Types::rooks) << "\n";
    // std::cout << newBoard.getPieceSet(Types::white, Types::queens) << "\n";
    // std::cout << newBoard.getPieceSet(Types::black, Types::queens) << "\n";
    // std::cout << newBoard.getPieceSet(Types::white, Types::kings) << "\n";
    // std::cout << newBoard.getPieceSet(Types::black, Types::kings) << "\n";

    uint8_t prevCastlingRightsValue = prevCastlingRights[depth - 1];
    uint64_t prevEnPassantSquare = prevEnPassantSquares[depth - 1];

    // int enPassantRank = std::countr_zero(enPassantSquare) % 8;
    // int prevEnPassantRank = std::countr_zero(prevEnPassantSquare) % 8;
    // hash ^= hashEnPassantSquare[enPassantRank];
    // hash ^= hashEnPassantSquare[prevEnPassantRank];
    // hash ^= hashCastlingRights[castlingRights];
    // hash ^= hashCastlingRights[prevCastlingRightsValue];
    // std::cerr << "unmake move " << move << " hash: " << hash << std::endl;
    board = newBoard;
    castlingRights = prevCastlingRightsValue;
    enPassantSquare = prevEnPassantSquare;
    depth -= 1;
    currentPlayer = opponent;

    repetitionTable[repetitionIndex] = prevHashes[depth + 1];
    repetitionIndex = prevRepetitionIndices[depth];
    plySinceCaptureOrPawnMove = prevPlies[depth];
}