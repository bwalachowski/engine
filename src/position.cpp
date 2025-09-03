#include "position.h"
#include "move.h"

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
    prevCastlingRightsStack = std::stack<uint8_t>();
    prevEnPassantSquaresStack = std::stack<uint64_t>();
}

Position Position::makeMove(Move move)
{
    Board newBoard = board;
    uint64_t fromSquare = move.getFromSquare();
    uint64_t toSquare = move.getToSquare();
    Types::PieceEnum piece = move.getPiece();
    uint32_t flags = move.getFlags();

    newBoard.makeMove(move, currentPlayer);
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
    if (flags == Move::capture)
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
    std::stack<uint64_t> newPrevEnPassantSquaresStack = prevEnPassantSquaresStack;
    newPrevEnPassantSquaresStack.push(enPassantSquare);
    std::stack<uint8_t> newPrevCastlingRightsStack = prevCastlingRightsStack;
    newPrevCastlingRightsStack.push(castlingRights);
    return Position(newBoard, newEnPassantSquare, nextPlayer, newCastlingRights, newPrevCastlingRightsStack, newPrevEnPassantSquaresStack);
}

Position Position::unmakeMove(Move move)
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
        newBoard.addPiece(toSquare, currentPlayer, newBoard.getCapturedPiece());
        newBoard.popCapturedPieces();
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

    uint8_t prevCastlingRights = prevCastlingRightsStack.top();
    uint64_t prevEnPassantSquare = prevEnPassantSquaresStack.top();
    prevEnPassantSquaresStack.pop();
    prevCastlingRightsStack.pop();

    return Position(newBoard, prevEnPassantSquare, opponent, prevCastlingRights, prevCastlingRightsStack, prevEnPassantSquaresStack);
}
