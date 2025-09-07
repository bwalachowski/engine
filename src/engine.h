#ifndef ENGINE_H
#define ENGINE_H

#include "move.h"
#include "position.h"
#include "move_generator.h"
#include <random>
#include <sstream>
#include <thread>
#include <mutex>

class Engine
{
    Move moves[64][256];
    MoveGenerator generator;
    Position position;
    std::thread thread;
    std::mutex mutex;
    bool goBool = true;
    std::chrono::milliseconds time;
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    Move stored_best_move;
    int stored_eval;
    bool initialized = false;
    int nodes = 0;
    // int nodes_pruned = 0;

public:
    Engine() = default;
    void give_move(Position pos, int depth);
    int negamax(int alpha, int beta, Position pos, int depth);
    int evaluate(Position pos);

    void run();

    void set_position(std::istringstream &is);
    void go(std::istringstream &is);
    void new_game();
    void initialize();
    void quit();
};
#endif // ENGINE_H