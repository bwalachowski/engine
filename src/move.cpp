#include "move.h"

#include <bitset>
#include <string>

Move::Move(uint32_t from, uint32_t to, uint32_t flags, Board::PieceEnum piece)
    : piece(piece)
{
    move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
}

std::string Move::getLongAlgebraicNotation() const
{
    char toSquareRank = ('1' + (move & 0x7));
    char toSquareFile = ('h' - ((move & 0x3f) >> 3));
    char fromSquareRank = ('1' + ((move & 0x1ff) >> 6));
    char fromSquareFile = ('h' - ((move & 0xfff) >> 9));
    char arr[5] = {fromSquareFile, fromSquareRank, toSquareFile, toSquareRank,
                   '\0'};
    return std::string(arr);
}

std::ostream &operator<<(std::ostream &os, const Move &move)
{
    os << move.getLongAlgebraicNotation();
    return os;
}
