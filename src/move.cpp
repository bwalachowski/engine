#include "move.h"

#include <string>

Move::Move(uint32_t from, uint32_t to, uint32_t flags, Types::PieceEnum piece)
    : piece(piece), pieceTaken(Types::white)
{
    move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
}

Move::Move(uint32_t from, uint32_t to, uint32_t flags, Types::PieceEnum piece, Types::PieceEnum pieceTaken)
    : piece(piece), pieceTaken(pieceTaken)
{
    move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
}

std::string Move::getLongAlgebraicNotation() const
{
    char toSquareRank = ('1' + (move & 0x7));
    char toSquareFile = ('h' - ((move & 0x3f) >> 3));
    char fromSquareRank = ('1' + ((move & 0x1ff) >> 6));
    char fromSquareFile = ('h' - ((move & 0xfff) >> 9));
    char arr[6] = {fromSquareFile, fromSquareRank, toSquareFile, toSquareRank,
                   '\0'};

    if (getFlags() >= Move::knightPromotion && getFlags() <= Move::queenPromotionCapture)
    {
        char promotionChar;
        switch (getFlags())
        {
        case Move::knightPromotion:
        case Move::knightPromotionCapture:
            promotionChar = 'n';
            break;
        case Move::bishopPromotion:
        case Move::bishopPromotionCapture:
            promotionChar = 'b';
            break;
        case Move::rookPromotion:
        case Move::rookPromotionCapture:
            promotionChar = 'r';
            break;
        case Move::queenPromotion:
        case Move::queenPromotionCapture:
            promotionChar = 'q';
            break;
        default:
            promotionChar = ' '; // Should not reach here
            break;
        }
        arr[4] = promotionChar;
        arr[5] = '\0';
    }
    return std::string(arr);
}

uint64_t Move::getFromSquare() const
{
    uint32_t fromRank = (move & 0x1ff) >> 6;
    uint32_t fromFile = (move & 0xfff) >> 9;
    return 1ULL << (fromRank * 8 + fromFile);
}

uint64_t Move::getToSquare() const
{
    uint32_t toRank = move & 0x7;
    uint32_t toFile = (move & 0x3f) >> 3;
    return 1ULL << (toRank * 8 + toFile);
}

std::ostream &operator<<(std::ostream &os, const Move &move)
{
    os << move.getLongAlgebraicNotation();
    return os;
}
