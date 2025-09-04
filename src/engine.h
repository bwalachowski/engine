#ifndef ENGINE_H
#define ENGINE_H

#include "move.h"
#include "position.h"
#include "move_generator.h"
#include <random>

class Engine
{
    std::random_device dev;
    std::mt19937 rng{dev()};
    Move moves[64][256];
    MoveGenerator generator;

public:
    Engine() = default;
    Move give_move(Position pos);
    int negamax(int alpha, int beta, Position pos, int depth);
    int evaluate(Position pos);
};
#endif // ENGINE_H