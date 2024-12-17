#ifndef MOVE_H
#define MOVE_H

#include <cstdint>

class Move {
    /*
    first 6 bits: from
    second 6 bits: to
    next 4 bits: promotion
    last 16 bits: not used for now
    */
    uint32_t move;
        
};


#endif  // #ifndef MOVE_H