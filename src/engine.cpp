#include "engine.h"
#include "move_generator.h"
#include <string>
#include <iostream>
#include <regex>

void Engine::give_move(Position pos, int depth)
{
    int n_moves;
    Move best_move;
    int max_eval = -210000;

    n_moves = generator.generatePseudoLegalMoves(pos, moves[depth]);

    if (n_moves == 0)
    {
        stored_best_move = Move();
    }
    for (int i = 0; i < n_moves && goBool && std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) < time; i++)
    {
        if (moves[depth][i].getFlags() == Move::capture)
        {
            if (pos.makeMoveCheckIfLegal(moves[depth][i]))
            {
                int eval = -negamax(-200000, 200000, pos, depth - 1);
                if (eval > max_eval)
                {
                    max_eval = eval;
                    best_move = moves[depth][i];
                }
                // std::cerr << "move: " << moves[depth][i] << " eval: " << eval << "depth: " << depth << std::endl;
            }
            pos.unmakeMove(moves[depth][i]);
        }
        end = std::chrono::steady_clock::now();
    }
    for (int i = 0; i < n_moves && goBool && std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) < time; i++)
    {
        if (moves[depth][i].getFlags() != Move::capture)
        {
            if (pos.makeMoveCheckIfLegal(moves[depth][i]))
            {
                int eval = -negamax(-200000, 200000, pos, depth - 1);
                if (eval > max_eval)
                {
                    max_eval = eval;
                    best_move = moves[depth][i];
                }
                // std::cerr << "move: " << moves[depth][i] << " eval: " << eval << "depth: " << depth << std::endl;
            }
            pos.unmakeMove(moves[depth][i]);
        }
        end = std::chrono::steady_clock::now();
    }
    // std::cerr << "move: " << best_move << " eval: " << max_eval << "depth: " << depth << std::endl;
    if (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) < time)
    {
        stored_eval = max_eval;
        stored_best_move = best_move;
    }
}

int Engine::negamax(int alpha, int beta, Position pos, int depth)
{

    if (pos.isDraw())
    {
        return 0;
    }

    if (depth == 0)
    {
        return evaluate(pos);
    }

    int n_moves = generator.generatePseudoLegalMoves(pos, moves[depth]);
    int legalMoves = n_moves;
    int max_eval = -200000;
    for (int i = 0; i < n_moves && goBool && std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) < time; i++)
    {
        if (moves[depth][i].getFlags() == Move::capture)
        {
            if (pos.makeMoveCheckIfLegal(moves[depth][i]))
            {
                int eval = -negamax(-beta, -alpha, pos, depth - 1);
                if (eval > max_eval)
                {
                    max_eval = eval;
                    if (eval > alpha)
                    {
                        alpha = eval;
                    }
                }
                pos.unmakeMove(moves[depth][i]);
                if (eval >= beta)
                {
                    return max_eval;
                }
            }
            else
            {
                legalMoves--;
                pos.unmakeMove(moves[depth][i]);
            }
        }
        end = std::chrono::steady_clock::now();
    }
    for (int i = 0; i < n_moves && goBool && std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) < time; i++)
    {
        if (moves[depth][i].getFlags() != Move::capture)
        {
            if (pos.makeMoveCheckIfLegal(moves[depth][i]))
            {
                int eval = -negamax(-beta, -alpha, pos, depth - 1);
                if (eval > max_eval)
                {
                    max_eval = eval;
                    if (eval > alpha)
                    {
                        alpha = eval;
                    }
                }
                pos.unmakeMove(moves[depth][i]);
                if (eval >= beta)
                {
                    return max_eval;
                }
            }
            else
            {
                legalMoves--;
                pos.unmakeMove(moves[depth][i]);
            }
        }
        end = std::chrono::steady_clock::now();
    }
    if (legalMoves == 0)
    {
        if (generator.attacked(pos.getPieceSet(pos.getCurrentPlayer(), Types::kings), pos, pos.getOtherPlayer()))
        {
            // max_eval = (pos.getCurrentPlayer() == Types::white) ? (100000 + depth) : (-100000 - depth);
            max_eval = -100000 - depth;
        }
        else
        {
            max_eval = 0;
        }
        if (max_eval > alpha)
        {
            alpha = max_eval;
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
    score += 10 * generator.checkPseudoLegalMobility(pos, Types::white) - 10 * generator.checkPseudoLegalMobility(pos, Types::black);

    return (pos.getCurrentPlayer() == Types::white) ? score : -score;
}

void Engine::run()
{
    while (goBool)
    {
        mutex.lock();
        begin = std::chrono::steady_clock::now();
        end = std::chrono::steady_clock::now();
        int depth = 1;
        while (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) < time && depth < 64)
        {
            give_move(position, depth++);
            end = std::chrono::steady_clock::now();
        }
        std::cout << "info depth " << depth - 2 << " score cp " << stored_eval << std::endl;
        std::cout << "bestmove " << stored_best_move << std::endl;
    }
}

void Engine::set_position(std::istringstream &is)
{
    is >> std::skipws;
    std::string type;
    is >> type;

    if (type == "startpos")
    {
        position = Position();
    }
    else
    {
        std::string pieces;
        std::string color;
        std::string castlingRights;
        std::string enPassantSquare;
        is >> pieces >> color >> castlingRights >> enPassantSquare;
        position = Position(pieces, color, castlingRights, enPassantSquare);
    }
    is >> type;

    while (!type.empty())
    {
        if (type == "moves")
        {
            while (is >> type)
            {
                Move move = position.getMoveFromLongAlgebraicNotation(type);
                position = position.makeMove(move, true);
            }

            return;
        }
        type.clear();
        is >> type;
    }
}

void Engine::go(std::istringstream &is)
{

    std::string token;
    int new_time = 0;
    while (is >> token)
    {
        if (token == "wtime" && position.getCurrentPlayer() == Types::white)
        {
            is >> token;
            new_time += (std::stoi(token) / 20);
        }
        else if (token == "btime" && position.getCurrentPlayer() == Types::black)
        {
            is >> token;
            new_time += (std::stoi(token) / 20);
        }
        else if (token == "winc" && position.getCurrentPlayer() == Types::white)
        {
            is >> token;
            new_time += (std::stoi(token) / 2);
        }
        else if (token == "winc" && position.getCurrentPlayer() == Types::black)
        {
            is >> token;
            new_time += (std::stoi(token) / 2);
        }
        else if (token == "movetime")
        {
            is >> token;
            new_time += std::stoi(token);
        }
    }
    time = std::chrono::milliseconds(new_time);
    mutex.unlock();
}

void Engine::new_game()
{
    return;
}

void Engine::initialize()
{
    if (!initialized)
    {
        mutex.lock();
        thread = std::thread(&Engine::run, this);
        initialized = true;
    }
    std::cout << "readyok" << std::endl;
}

void Engine::quit()
{
    goBool = false;
    mutex.unlock();
}