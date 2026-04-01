#include "board.h"
#include <iostream>
#include <bitset>
#include "move_generator.h"

void Board::init()
{
    pieces[Types::white] = 0xffff;
    pieces[Types::black] = 0xffff000000000000;
    pieces[Types::kings] = 0x800000000000008;
    pieces[Types::queens] = 0x1000000000000010;
    pieces[Types::bishops] = 0x2400000000000024;
    pieces[Types::rooks] = 0x8100000000000081;
    pieces[Types::knights] = 0x4200000000000042;
    pieces[Types::pawns] = 0xff00000000ff00;
    // pieces[Types::white] = 0x1000002000ebf9;
    // pieces[Types::black] = 0xf5cf200000000400;
    // pieces[Types::kings] = 0x400000000000008;
    // pieces[Types::queens] = 0x1000000000000010;
    // pieces[Types::bishops] = 0x2008000020000020;
    // pieces[Types::rooks] = 0x8100000000000081;
    // pieces[Types::knights] = 0x4000000000000c40;
    // pieces[Types::pawns] = 0xd720000000e300;
    nPieces = 14;
}

void Board::print()
{
    for (auto a : pieces)
    {
        std::cout << a << "\n";
    }
}

Board::Board(std::string fen)
{
    int nPieces = 0;
    // Initialize all pieces to 0
    for (int i = 0; i < 8; ++i)
    {
        pieces[i] = 0;
    }

    uint64_t square = 0x8000000000000000; // Start from a8
    for (char c : fen)
    {
        if (c == ' ')
        {
            break; // End of board representation
        }
        if (c == '/')
        {
            continue; // Move to the next rank
        }
        else if (isdigit(c))
        {
            square >>= (c - '0'); // Skip empty squares
        }
        else
        {
            switch (c)
            {
            case 'P':
                pieces[Types::white] |= square;
                pieces[Types::pawns] |= square;
                break;
            case 'N':
                pieces[Types::white] |= square;
                pieces[Types::knights] |= square;
                nPieces++;
                break;
            case 'B':
                pieces[Types::white] |= square;
                pieces[Types::bishops] |= square;
                nPieces++;
                break;
            case 'R':
                pieces[Types::white] |= square;
                pieces[Types::rooks] |= square;
                nPieces++;
                break;
            case 'Q':
                pieces[Types::white] |= square;
                pieces[Types::queens] |= square;
                nPieces++;
                break;
            case 'K':
                pieces[Types::white] |= square;
                pieces[Types::kings] |= square;
                break;
            case 'p':
                pieces[Types::black] |= square;
                pieces[Types::pawns] |= square;
                break;
            case 'n':
                pieces[Types::black] |= square;
                pieces[Types::knights] |= square;
                nPieces++;
                break;
            case 'b':
                pieces[Types::black] |= square;
                pieces[Types::bishops] |= square;
                nPieces++;
                break;
            case 'r':
                pieces[Types::black] |= square;
                pieces[Types::rooks] |= square;
                nPieces++;
                break;
            case 'q':
                pieces[Types::black] |= square;
                pieces[Types::queens] |= square;
                nPieces++;
                break;
            case 'k':
                pieces[Types::black] |= square;
                pieces[Types::kings] |= square;
                break;
            default:
                // Invalid character in FEN
                break;
            }
            square >>= 1; // Move to the next square
        }
    }
}

void Board::makeMove(Move move, Types::PieceEnum color, int depth)
{
    uint64_t fromSquare = move.getFromSquare();
    uint64_t toSquare = move.getToSquare();
    Types::PieceEnum piece = move.getPiece();
    uint32_t flags = move.getFlags();

    pieces[color] &= ~fromSquare;
    pieces[piece] &= ~fromSquare;
    pieces[color] |= toSquare;

    if (flags == Move::capture || (flags >= Move::knightPromotionCapture && flags <= Move::queenPromotionCapture))
    {
        Types::PieceEnum opponent = (color == Types::white) ? Types::black : Types::white;
        for (int i = Types::pawns; i <= Types::queens; ++i)
        {
            if (pieces[i] & toSquare)
            {
                // std::cout << "Capturing piece: " << static_cast<Types::PieceEnum>(i) << "\n";
                pieces[i] &= ~toSquare;
                if (i != Types::pawns)
                {
                    nPieces--;
                }
                capturedPieces[depth] = (static_cast<Types::PieceEnum>(i));
                break;
            }
        }
        pieces[opponent] &= ~toSquare;
    }

    pieces[piece] |= toSquare;

    if (flags >= Move::knightPromotion && flags <= Move::queenPromotionCapture)
    {
        Types::PieceEnum promotionPiece;
        switch (flags)
        {
        case Move::knightPromotion:
        case Move::knightPromotionCapture:
            promotionPiece = Types::knights;
            break;
        case Move::bishopPromotion:
        case Move::bishopPromotionCapture:
            promotionPiece = Types::bishops;
            break;
        case Move::rookPromotion:
        case Move::rookPromotionCapture:
            promotionPiece = Types::rooks;
            break;
        case Move::queenPromotion:
        case Move::queenPromotionCapture:
            promotionPiece = Types::queens;
            break;
        default:
            promotionPiece = piece; // Should not reach here
            break;
        }
        pieces[promotionPiece] |= toSquare;
        pieces[Types::pawns] &= ~toSquare;
    }

    if (flags == Move::enPassant)
    {
        Types::PieceEnum opponent = (color == Types::white) ? Types::black : Types::white;
        if (color == Types::white)
        {
            uint64_t capturedPawnSquare = toSquare >> 8;
            pieces[opponent] &= ~capturedPawnSquare;
            pieces[Types::pawns] &= ~capturedPawnSquare;
        }
        else
        {
            uint64_t capturedPawnSquare = toSquare << 8;
            pieces[opponent] &= ~capturedPawnSquare;
            pieces[Types::pawns] &= ~capturedPawnSquare;
        }
    }

    if (flags == Move::shortCastle)
    {
        if (color == Types::white)
        {
            // Move rook from h1 to f1
            pieces[Types::rooks] &= ~(1ULL);
            pieces[Types::rooks] |= (1ULL << 2);
            pieces[Types::white] &= ~(1ULL);
            pieces[Types::white] |= (1ULL << 2);
        }
        else
        {
            // Move rook from h8 to f8
            pieces[Types::rooks] &= ~(1ULL << 56);
            pieces[Types::rooks] |= (1ULL << 58);
            pieces[Types::black] &= ~(1ULL << 56);
            pieces[Types::black] |= (1ULL << 58);
        }
    }
    else if (flags == Move::longCastle)
    {
        if (color == Types::white)
        {
            // Move rook from a1 to d1
            pieces[Types::rooks] &= ~(1ULL << 7);
            pieces[Types::rooks] |= (1ULL << 4);
            pieces[Types::white] &= ~(1ULL << 7);
            pieces[Types::white] |= (1ULL << 4);
        }
        else
        {
            // Move rook from a8 to d8
            pieces[Types::rooks] &= ~(1ULL << 63);
            pieces[Types::rooks] |= (1ULL << 60);
            pieces[Types::black] &= ~(1ULL << 63);
            pieces[Types::black] |= (1ULL << 60);
        }
    }
}
