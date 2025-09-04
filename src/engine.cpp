#include "engine.h"
#include "move_generator.h"

Move Engine::give_move(Position pos)
{
    int depth = 6;
    int n_moves;
    Move best_move;
    int max_eval = -100000;

    n_moves = generator.generateLegalMoves(pos, moves[depth]);
    if (n_moves == 0)
    {
        return Move(); // null move
    }
    for (int i = 0; i < n_moves; i++)
    {
        pos = pos.makeMove(moves[depth][i]);
        int eval = -negamax(-100000, 100000, pos, depth - 1);
        if (eval > max_eval)
        {
            max_eval = eval;
            best_move = moves[depth][i];
        }
        pos = pos.unmakeMove(moves[depth][i]);
        std::cout << "move: " << moves[depth][i] << " eval:" << eval << std::endl;
    }
    std::cout << "Best eval: " << max_eval << std::endl;
    return best_move;
}

int Engine::negamax(int alpha, int beta, Position pos, int depth)
{
    if (depth == 0)
    {
        return evaluate(pos);
    }

    int n_moves = generator.generateLegalMoves(pos, moves[depth]);
    if (n_moves == 0)
    {
        return evaluate(pos);
    }
    int max_eval = -100000;
    for (int i = 0; i < n_moves; i++)
    {
        pos = pos.makeMove(moves[depth][i]);
        int eval = -negamax(-beta, -alpha, pos, depth - 1);
        if (eval > max_eval)
        {
            max_eval = eval;
            if (eval > alpha)
            {
                alpha = eval;
            }
        }
        pos = pos.unmakeMove(moves[depth][i]);
        if (eval >= beta)
        {
            return max_eval;
        }
    }
    return max_eval;
}

int Engine::evaluate(Position pos)
{
    int score = 0;
    uint64_t white_pawns = pos.getPieceSet(Types::white, Types::pawns);
    uint64_t black_pawns = pos.getPieceSet(Types::black, Types::pawns);
    uint64_t white_knights = pos.getPieceSet(Types::white, Types::knights);
    uint64_t black_knights = pos.getPieceSet(Types::black, Types::knights);
    uint64_t white_bishops = pos.getPieceSet(Types::white, Types::bishops);
    uint64_t black_bishops = pos.getPieceSet(Types::black, Types::bishops);
    uint64_t white_rooks = pos.getPieceSet(Types::white, Types::rooks);
    uint64_t black_rooks = pos.getPieceSet(Types::black, Types::rooks);
    uint64_t white_queens = pos.getPieceSet(Types::white, Types::queens);
    uint64_t black_queens = pos.getPieceSet(Types::black, Types::queens);
    uint64_t white_kings = pos.getPieceSet(Types::white, Types::kings);
    uint64_t black_kings = pos.getPieceSet(Types::black, Types::kings);

    score += 100 * std::popcount(white_pawns) - 100 * std::popcount(black_pawns);
    score += 300 * std::popcount(white_knights) - 300 * std::popcount(black_knights);
    score += 300 * std::popcount(white_bishops) - 300 * std::popcount(black_bishops);
    score += 500 * std::popcount(white_rooks) - 500 * std::popcount(black_rooks);
    score += 900 * std::popcount(white_queens) - 900 * std::popcount(black_queens);
    score += 10000 * std::popcount(white_kings) - 10000 * std::popcount(black_kings);
    score += 10 * generator.checkMobility(pos, Types::white) - 10 * generator.checkMobility(pos, Types::black);

    return (pos.getCurrentPlayer() == Types::white) ? score : -score;
}
