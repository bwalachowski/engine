#include "engine.h"
#include "move_generator.h"

Move Engine::give_move(Position pos)
{
    MoveGenerator generator;
    Move moves[256];
    int n_moves = 0;
    n_moves = generator.generateLegalMoves(pos, moves);
    if (n_moves == 0)
    {
        return Move();
    }

    std::vector<Move> special_moves;
    for (int i = 0; i < n_moves; i++)
    {
        if (moves[i].getFlags() != Move::quiet && moves[i].getFlags() != Move::doublePush)
        {
            special_moves.push_back(moves[i]);
        }
        std::cout << moves[i] << "\n";
    }
    std::cout << "Total legal moves: " << n_moves << "\n";

    if (!special_moves.empty())
    {
        std::uniform_int_distribution<std::mt19937::result_type> special_dist(0, special_moves.size() - 1);
        return special_moves[special_dist(rng)];
    }

    std::uniform_int_distribution<std::mt19937::result_type> dist(0, n_moves - 1);
    return moves[dist(rng)];
}