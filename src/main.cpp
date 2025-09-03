#include "board.h"
#include "move.h"
#include "move_generator.h"
#include "position.h"
#include "engine.h"
#include <iostream>
#include <bit>
#include <ctime>
#include "tests.h"

// int main()
// {
//     Board emptyBoard;
//     emptyBoard.init();
//     uint8_t castlingRights = 0b0011;
//     Position position(emptyBoard, Types::white, castlingRights);
//     uint64_t king = position.getPieceSet(Types::white, Types::kings);
//     std::cout << std::countr_zero(king) << "\n";
//     Engine engine;
//     Move move = engine.give_move(position);
//     int j = 0;
//     while (!move.isNull() > 0 && j++ < 200)
//     {
//         std::cout << "Move: " << move << "\n";
//         position = position.makeMove(move);
//         std::cout << position.getAllPieces() << "\n";
//         move = engine.give_move(position);
//     }
// }

// perft test
int main(int argc, char **argv)
{
    Tests tests;
    Position position = Position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    // std::cout << "Starting position:\n";
    // std::cout << "black pawns: " << position.getPieceSet(Types::black, Types::pawns) << "\n";
    // std::cout << "pawns: " << position.getPieceSet(Types::pawns) << "\n";
    // std::cout << "rooks: " << position.getPieceSet(Types::rooks) << "\n";
    // std::cout << "knights: " << position.getPieceSet(Types::knights) << "\n";
    // std::cout << "bishops: " << position.getPieceSet(Types::bishops) << "\n";
    // std::cout << "queens: " << position.getPieceSet(Types::queens) << "\n";
    // std::cout << "kings: " << position.getPieceSet(Types::kings) << "\n";
    // std::cout << "white pieces: " << position.getPieceSet(Types::white) << "\n";
    // std::cout << "black pieces: " << position.getPieceSet(Types::black) << "\n";
    // std::cout << "All pieces: " << position.getAllPieces() << "\n";
    clock_t start = clock();
    int depth = 5;
    if (argc > 1)
    {
        depth = (int)(argv[1][0] - '0');
    }
    uint64_t nodes = tests.perft(depth, position);
    clock_t end = clock();
    double time_spent = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Perft to depth " << depth << ": " << nodes << "\n";
    std::cout << "Time taken: " << time_spent << " seconds\n";
    return 0;
}