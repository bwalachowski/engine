#include "move.h"

#include <string>

Move::Move(uint32_t from, uint32_t to, uint32_t flags, Types::PieceEnum piece)
    : piece(piece)
{
    move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
}

std::string Move::getLongAlgebraicNotation() const
{
    if (getFlags() == Move::shortCastle)
    {
        return "O-O";
    }
    else if (getFlags() == Move::longCastle)
    {
        return "O-O-O";
    }
    char toSquareRank = ('1' + (move & 0x7));
    char toSquareFile = ('h' - ((move & 0x3f) >> 3));
    char fromSquareRank = ('1' + ((move & 0x1ff) >> 6));
    char fromSquareFile = ('h' - ((move & 0xfff) >> 9));
    char arr[8] = {fromSquareFile, fromSquareRank, toSquareFile, toSquareRank,
                   '\0'};

    if (getFlags() == Move::capture || (getFlags() >= Move::knightPromotionCapture && getFlags() <= Move::queenPromotionCapture))
    {
        arr[2] = 'x';
        arr[3] = toSquareFile;
        arr[4] = toSquareRank;
        arr[5] = '\0';
    }
    if (getFlags() >= Move::knightPromotion && getFlags() <= Move::queenPromotionCapture)
    {
        char promotionChar;
        switch (getFlags())
        {
        case Move::knightPromotion:
        case Move::knightPromotionCapture:
            promotionChar = 'N';
            break;
        case Move::bishopPromotion:
        case Move::bishopPromotionCapture:
            promotionChar = 'B';
            break;
        case Move::rookPromotion:
        case Move::rookPromotionCapture:
            promotionChar = 'R';
            break;
        case Move::queenPromotion:
        case Move::queenPromotionCapture:
            promotionChar = 'Q';
            break;
        default:
            promotionChar = ' '; // Should not reach here
            break;
        }
        if (getFlags() >= Move::knightPromotionCapture && getFlags() <= Move::queenPromotionCapture)
        {
            arr[5] = '=';
            arr[6] = promotionChar;
            arr[7] = '\0';
        }
        else
        {
            arr[4] = '=';
            arr[5] = promotionChar;
            arr[6] = '\0';
        }
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
