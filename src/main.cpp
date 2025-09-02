#include "board.h"
#include "move_generator.h"
#include "position.h"
#include <iostream>
#include <bit>

int main()
{
    Board emptyBoard;
    emptyBoard.init();
    uint8_t castlingRights = 0b1111;
    Position position(emptyBoard, Board::white, castlingRights);
    uint64_t king = position.getPieceSet(Board::white, Board::kings);
    std::cout << std::countr_zero(king) << "\n";
    MoveGenerator generator;
    std::vector<Move> moves = generator.generatePseudoLegalMoves(position);
    for (auto a : moves)
    {
        std::cout << a << "\n";
    }
}