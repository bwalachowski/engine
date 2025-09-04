#include "position.h"
#include "move.h"
#include "move_generator.h"

uint8_t Position::prevCastlingRights[] = {0};
uint64_t Position::prevEnPassantSquares[] = {0};

Position::Position(Board board, Types::PieceEnum currentPlayer, uint8_t castlingRights)
    : board(board),
      currentPlayer(currentPlayer), castlingRights(castlingRights)
{
    enPassantSquare = 0;
    depth = 0;
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
}

Position::Position(Board board, uint64_t enPassantSquare, Types::PieceEnum currentPlayer, uint8_t castlingRights) : board(board), enPassantSquare(enPassantSquare), currentPlayer(currentPlayer), castlingRights(castlingRights)
{
    depth = 0;
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
}

Position::Position(Board board, uint64_t enPassantSquare, Types::PieceEnum currentPlayer, uint8_t castlingRights, int depth) : board(board), enPassantSquare(enPassantSquare), currentPlayer(currentPlayer), castlingRights(castlingRights), depth(depth)
{
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
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
    depth = 0;
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
}

Position::Position()
    : board(Board()), enPassantSquare(0), currentPlayer(Types::white), castlingRights(0b1111)
{
    depth = 0;
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
}

Position Position::makeMove(Move move, bool commit)
{
    Board newBoard = board;
    uint64_t fromSquare = move.getFromSquare();
    uint64_t toSquare = move.getToSquare();
    Types::PieceEnum piece = move.getPiece();
    uint32_t flags = move.getFlags();

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
    if (commit)
    {
        return Position(newBoard, newEnPassantSquare, nextPlayer, newCastlingRights, 0);
    }
    return Position(newBoard, newEnPassantSquare, nextPlayer, newCastlingRights, depth + 1);
}

bool Position::makeMoveCheckIfLegal(Move move)
{
    Board newBoard = board;
    uint64_t fromSquare = move.getFromSquare();
    uint64_t toSquare = move.getToSquare();
    Types::PieceEnum piece = move.getPiece();
    uint32_t flags = move.getFlags();

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
        if (toSquare & h1Square)
            newCastlingRights &= 0b1110;
        else if (toSquare & a1Square)
            newCastlingRights &= 0b1101;
        else if (toSquare & h8Square)
            newCastlingRights &= 0b1011;
        else if (toSquare & a8Square)
            newCastlingRights &= 0b0111;
    }
    MoveGenerator moveGen;
    board = newBoard;
    enPassantSquare = newEnPassantSquare;
    castlingRights = newCastlingRights;
    depth += 1;
    prevCastlingRights[depth] = castlingRights;
    prevEnPassantSquares[depth] = enPassantSquare;
    Types::PieceEnum otherPlayer = currentPlayer;
    currentPlayer = (currentPlayer == Types::white) ? Types::black : Types::white;
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

    // Handle promotions
    if (flags >= Move::knightPromotion && flags <= Move::queenPromotionCapture)
    {
        switch (flags)
        {
        case Move::knightPromotion:
        case Move::knightPromotionCapture:
            newBoard.removePiece(toSquare, opponent, Types::knights);
            break;
        case Move::bishopPromotion:
        case Move::bishopPromotionCapture:
            newBoard.removePiece(toSquare, opponent, Types::bishops);
            break;
        case Move::rookPromotion:
        case Move::rookPromotionCapture:
            newBoard.removePiece(toSquare, opponent, Types::rooks);
            break;
        case Move::queenPromotion:
        case Move::queenPromotionCapture:
            newBoard.removePiece(toSquare, opponent, Types::queens);
            break;
        default:
            break; // Should not reach here
        }
    }
    else
    {
        newBoard.removePiece(toSquare, opponent, piece);
    }

    if (flags == Move::capture || (flags >= Move::knightPromotionCapture && flags <= Move::queenPromotionCapture))
    {
        // std::cout << "Restoring captured piece: " << newBoard.getCapturedPiece() << "\n";
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

    board = newBoard;
    castlingRights = prevCastlingRightsValue;
    enPassantSquare = prevEnPassantSquare;
    depth -= 1;
    currentPlayer = opponent;
}
