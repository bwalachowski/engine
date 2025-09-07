#ifndef POSITION_H
#define POSITION_H

#include "board.h"

class Position
{
    constexpr static int hashTableSize = 781;
    Board board;
    uint64_t enPassantSquare;
    Types::PieceEnum currentPlayer;
    uint8_t castlingRights; // last 4 bits, in order from last:  white short castle, white
                            // long castle, black short castle, black long castle
    static uint8_t prevCastlingRights[64];
    static uint64_t hashPieceNumbers[12][64];
    static uint64_t hashBlackToMove;
    static uint64_t hashCastlingRights[16];
    static uint64_t hashEnPassantSquare[8];
    static uint64_t prevEnPassantSquares[64];
    static uint64_t repetitionTable[120];
    static uint64_t prevHashes[64];
    int repetitionIndex = 0;
    static int prevRepetitionIndices[64];
    int depth;
    static bool hashesInitialized;
    int plySinceCaptureOrPawnMove;
    static int prevPlies[64];

    const static uint64_t a1Square = 0x80;
    const static uint64_t h1Square = 0x1;
    const static uint64_t h8Square = 0x100000000000000;
    const static uint64_t a8Square = 0x8000000000000000;
    const static uint64_t f1Square = 0x4;
    const static uint64_t f8Square = 0x400000000000000;
    const static uint64_t d1Square = 0x10;
    const static uint64_t d8Square = 0x1000000000000000;

public:
    Position(Board board, Types::PieceEnum currentPlayer, uint8_t castlingRights);
    Position(Board board, uint64_t enPassantSquare, Types::PieceEnum currentPlayer, uint8_t castlingRights);

    Position(Board board, uint64_t enPassantSquare, Types::PieceEnum currentPlayer, uint8_t castlingRights, int repetitionIndex, int depth, uint64_t hash, int plySinceCaptureOrPawnMove);

    Position(std::string fen);
    Position(std::string pieces, std::string color, std::string castlingRightsFen, std::string enPassantSquareFen, std::string plySinceCaptureOrPawnMoveFen);
    Position();

    void initializeHashNumbers();

    uint64_t getPieceSet(Types::PieceEnum color, Types::PieceEnum pieceType)
    {
        return board.getPieceSet(color, pieceType);
    }
    uint64_t getPieceSet(Types::PieceEnum i) { return board.getPieceSet(i); }
    uint64_t getAllPieces() { return board.getAllPieces(); }
    uint64_t getZobristHash();

    bool isDraw();

    Move getMoveFromLongAlgebraicNotation(std::string longAlgebraicNotation);

    uint64_t getEnPassantSquare() { return enPassantSquare; }
    Types::PieceEnum getCurrentPlayer() { return currentPlayer; }
    Types::PieceEnum getOtherPlayer()
    {
        return (currentPlayer == Types::white) ? Types::black : Types::white;
    }

    uint8_t getCastlingRights() { return castlingRights; }

    Position makeMove(Move move, bool commit = false);
    bool makeMoveCheckIfLegal(Move move);
    void unmakeMove(Move move);
    void changeCurrentPlayer()
    {
        currentPlayer = (currentPlayer == Types::white) ? Types::black : Types::white;
    }
};
#endif // #ifndef POSITION_H
