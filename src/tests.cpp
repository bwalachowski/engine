#include "tests.h"
#include <vector>
#include "move_generator.h"

uint64_t Tests::perft(int depth, Position pos)
{
    int n_moves, i;
    uint64_t nodes = 0;

    if (depth == 0)
        return 1ULL;

    MoveGenerator generator = MoveGenerator();
    n_moves = generator.generateLegalMoves(pos, moves[depth]);
    // std::cout << pos.getAllPieces() << "depth: " << depth << "\n";
    // for (auto a : moves)
    // {
    //     std::cout << a << "\n";
    // }
    // std::cout << "black pawns: " << pos.getPieceSet(Types::black, Types::pawns) << "\n";
    // std::cout << "pawns: " << pos.getPieceSet(Types::pawns) << "\n";
    // std::cout << "rooks: " << pos.getPieceSet(Types::rooks) << "\n";
    // std::cout << "knights: " << pos.getPieceSet(Types::knights) << "\n";
    // std::cout << "bishops: " << pos.getPieceSet(Types::bishops) << "\n";
    // std::cout << "queens: " << pos.getPieceSet(Types::queens) << "\n";
    // std::cout << "kings: " << pos.getPieceSet(Types::kings) << "\n";
    // std::cout << "white pieces: " << pos.getPieceSet(Types::white) << "\n";
    // std::cout << "black pieces: " << pos.getPieceSet(Types::black) << "\n";
    // std::cout << "All pieces: " << pos.getAllPieces() << "\n";
    // std::cout << "castling rights: " << (int)pos.getCastlingRights() << "\n";
    for (i = 0; i < n_moves; i++)
    {
        pos = pos.makeMove(moves[depth][i]);
        uint64_t new_nodes = perft(depth - 1, pos);
        // if (depth >= 1)
        //     std::cout << moves[depth][i] << ": " << new_nodes << "\n";
        nodes += new_nodes;
        pos = pos.unmakeMove(moves[depth][i]);
    }
    return nodes;
}