#include "move.h"

Move::Move(uint32_t from, uint32_t to, uint32_t flags)
{
    move = ((flags & 0xf)<<12) | ((from & 0x3f)<<6) | (to & 0x3f);
}
