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
    nodes++;

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
                // std::cerr << "move: " << moves[depth][i] << "depth: " << depth << std::endl;
                int eval = -negamax(-200000, 200000, pos, depth + 1);
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
                // std::cerr << "move: " << moves[depth][i] << "depth: " << depth << std::endl;
                int eval = -negamax(-200000, 200000, pos, depth + 1);
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
    nodes++;
    // int current_nodes = 0;
    if (pos.isDraw())
    {
        return 0;
    }

    if (depth == search_depth)
    {
        // return evaluate(pos);
        return quiesce(alpha, beta, pos, depth + 1);
    }

    int n_moves = generator.generatePseudoLegalMoves(pos, moves[depth]);
    int legalMoves = n_moves;
    int max_eval = -200000;
    for (int i = 0; i < n_moves && goBool && std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) < time; i++)
    {
        if (moves[depth][i].getFlags() == Move::capture)
        {
            // current_nodes++;
            if (pos.makeMoveCheckIfLegal(moves[depth][i]))
            {
                int eval = -negamax(-beta, -alpha, pos, depth + 1);
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
                    // nodes_pruned += n_moves - current_nodes;
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
            // current_nodes++;
            if (pos.makeMoveCheckIfLegal(moves[depth][i]))
            {
                int eval = -negamax(-beta, -alpha, pos, depth + 1);
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
                    // nodes_pruned += n_moves = current_nodes;
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
            max_eval = -100000 - search_depth + depth;
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

int Engine::quiesce(int alpha, int beta, Position pos, int depth)
{
    if (depth > seldepth)
    {
        seldepth = depth + 1;
    }
    nodes++;
    quiescent_nodes++;
    int max_eval = evaluate(pos);
    if (depth >= 63)
    {
        return max_eval;
    }
    if (max_eval >= beta)
    {
        return max_eval;
    }
    if (max_eval > alpha)
    {
        alpha = max_eval;
    }

    int n_moves = generator.generatePseudoLegalCapturesAndPromotions(pos, moves[depth]);

    for (int i = 0; i < n_moves && goBool && std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) < time; i++)
    {

        // current_nodes++;
        if (pos.makeMoveCheckIfLegal(moves[depth][i]))
        {
            // std::cerr << "move: " << moves[depth][i] << "depth: " << depth << std::endl;
            int eval = -quiesce(-beta, -alpha, pos, depth + 1);
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
                // nodes_pruned += n_moves - current_nodes;
                return max_eval;
            }
        }
        else
        {
            pos.unmakeMove(moves[depth][i]);
        }

        end = std::chrono::steady_clock::now();
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

    score += 100 * std::popcount(white_pawns) - 100 * std::popcount(black_pawns);
    score += 300 * std::popcount(white_knights) - 300 * std::popcount(black_knights);
    score += 300 * std::popcount(white_bishops) - 300 * std::popcount(black_bishops);
    score += 500 * std::popcount(white_rooks) - 500 * std::popcount(black_rooks);
    score += 900 * std::popcount(white_queens) - 900 * std::popcount(black_queens);
    score += 10 * generator.checkPseudoLegalMobility(pos, Types::white) - 10 * generator.checkPseudoLegalMobility(pos, Types::black);
    score += evaluate_piece_tables(pos);

    return (pos.getCurrentPlayer() == Types::white) ? score : -score;
}

int Engine::evaluate_piece_tables(Position pos)
{
    return evaluate_pawns_piece_tables(pos) + evaluate_bishops_piece_tables(pos) + evaluate_knights_piece_tables(pos) + evaluate_rooks_piece_tables(pos) + evaluate_queens_piece_tables(pos) + evaluate_kings_piece_tables(pos);
}

int Engine::evaluate_pawns_piece_tables(Position pos)
{
    int score = 0;
    uint64_t white_pawns = pos.getPieceSet(Types::white, Types::pawns);
    while (white_pawns)
    {
        int pawn = std::countr_zero(white_pawns);
        score += pawns[FIELD_WHITE(pawn)];
        white_pawns &= white_pawns - 1;
    }
    uint64_t black_pawns = pos.getPieceSet(Types::black, Types::pawns);
    while (black_pawns)
    {
        int pawn = std::countr_zero(black_pawns);
        score += pawns[FIELD_BLACK(pawn)];
        black_pawns &= black_pawns - 1;
    }
    return score;
}

int Engine::evaluate_knights_piece_tables(Position pos)
{
    int score = 0;
    uint64_t white_knights = pos.getPieceSet(Types::white, Types::knights);
    while (white_knights)
    {
        int knight = std::countr_zero(white_knights);
        score += knights[FIELD_WHITE(knight)];
        white_knights &= white_knights - 1;
    }
    uint64_t black_knights = pos.getPieceSet(Types::black, Types::knights);
    while (black_knights)
    {
        int knight = std::countr_zero(black_knights);
        score -= knights[FIELD_BLACK(knight)];
        black_knights &= black_knights - 1;
    }
    return score;
}

int Engine::evaluate_bishops_piece_tables(Position pos)
{
    int score = 0;
    uint64_t white_bishops = pos.getPieceSet(Types::white, Types::bishops);
    while (white_bishops)
    {
        int bishop = std::countr_zero(white_bishops);
        score += bishops[FIELD_WHITE(bishop)];
        white_bishops &= white_bishops - 1;
    }
    uint64_t black_bishops = pos.getPieceSet(Types::black, Types::bishops);
    while (black_bishops)
    {
        int bishop = std::countr_zero(black_bishops);
        score -= bishops[FIELD_BLACK(bishop)];
        black_bishops &= black_bishops - 1;
    }
    return score;
}

int Engine::evaluate_rooks_piece_tables(Position pos)
{
    int score = 0;
    uint64_t white_rooks = pos.getPieceSet(Types::white, Types::rooks);
    while (white_rooks)
    {
        int rook = std::countr_zero(white_rooks);
        score += rooks[FIELD_WHITE(rook)];
        white_rooks &= white_rooks - 1;
    }
    uint64_t black_rooks = pos.getPieceSet(Types::black, Types::rooks);
    while (black_rooks)
    {
        int rook = std::countr_zero(black_rooks);
        score -= rooks[FIELD_BLACK(rook)];
        black_rooks &= black_rooks - 1;
    }
    return score;
}

int Engine::evaluate_queens_piece_tables(Position pos)
{
    int score = 0;
    uint64_t white_queens = pos.getPieceSet(Types::white, Types::queens);
    while (white_queens)
    {
        int queen = std::countr_zero(white_queens);
        score += queens[FIELD_WHITE(queen)];
        white_queens &= white_queens - 1;
    }
    uint64_t black_queens = pos.getPieceSet(Types::black, Types::queens);
    while (black_queens)
    {
        int queen = std::countr_zero(black_queens);
        score -= queens[FIELD_BLACK(queen)];
        black_queens &= black_queens - 1;
    }
    return score;
}

int Engine::evaluate_kings_piece_tables(Position pos)
{
    int score = 0;

    if (pos.getNumberOfPieces() > 4)
    {
        uint64_t white_king = pos.getPieceSet(Types::white, Types::kings);
        while (white_king)
        {
            int king = std::countr_zero(white_king);
            score += king_mg[FIELD_WHITE(king)];
            white_king &= white_king - 1;
        }
        uint64_t black_king = pos.getPieceSet(Types::black, Types::kings);
        while (black_king)
        {
            int king = std::countr_zero(black_king);
            score -= king_mg[FIELD_BLACK(king)];
            black_king &= black_king - 1;
        }
    }
    else
    {
        uint64_t white_king = pos.getPieceSet(Types::white, Types::kings);
        while (white_king)
        {
            int king = std::countr_zero(white_king);
            score += king_eg[FIELD_WHITE(king)];
            white_king &= white_king - 1;
        }
        uint64_t black_king = pos.getPieceSet(Types::black, Types::kings);
        while (black_king)
        {
            int king = std::countr_zero(black_king);
            score -= king_eg[FIELD_BLACK(king)];
            black_king &= black_king - 1;
        }
    }
    return score;
}

void Engine::run()
{
    while (goBool)
    {
        mutex.lock();
        nodes = 0;
        // nodes_pruned = 0;
        begin = std::chrono::steady_clock::now();
        end = std::chrono::steady_clock::now();
        search_depth = 1;
        quiescent_nodes = 0;
        // int curr_seldepth = 0;
        while (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) < time && search_depth < 64)
        {
            give_move(position, 0);
            search_depth++;
            end = std::chrono::steady_clock::now();
        }
        std::cout << "info depth " << search_depth - 2 << " score cp " << stored_eval << " nodes " << nodes << std::endl;
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
        std::string plies;
        is >> pieces >> color >> castlingRights >> enPassantSquare >> plies;
        position = Position(pieces, color, castlingRights, enPassantSquare, plies);
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