#ifndef MOVE_H
#define MOVE_H

#include <cstdint>

class Move {
    /*
    first 6 bits: from
    second 6 bits: to
    next 4 bits: flags
    last 16 bits: not used for now
    */
    uint32_t move;
    public:
        enum flagEnum : uint32_t
        {
            quiet = 0,
            doublePush = 1,
            shortCastle = 2,
            longCastle = 3,
            capture = 4,
            enPassant = 5,
            knightPromotion = 8,
            bishopPromotion = 9,
            rookPromotion = 10,
            queenPromotion = 11,
            knightPromotionCapture = 12,
            bishopPromotionCapture = 13,
            rookPromotionCapture = 14,
            queenPromotionCapture = 15
        };
        Move(uint32_t from, uint32_t to, uint32_t flags);
};


#endif  // #ifndef MOVE_H