#ifndef TESTS_H
#define TESTS_H

#include <cstdint>
#include "position.h"

class Tests
{
    Move moves[10][256];

public:
    uint64_t perft(int depth, Position pos);
};

#endif // #ifndef TESTS_H