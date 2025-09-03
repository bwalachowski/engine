#ifndef ENGINE_H
#define ENGINE_H

#include "move.h"
#include "position.h"
#include <random>

class Engine
{
    std::random_device dev;
    std::mt19937 rng{dev()};

public:
    Engine() = default;
    Move give_move(Position pos);
};
#endif // ENGINE_H