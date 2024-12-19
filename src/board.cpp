#include "board.h"
#include <iostream>
#include "move_generator.h"

void Board::init() {
    pieces[Board::white] = 0xffff;
    pieces[Board::black] = 0xffff000000000000;
    pieces[Board::kings] = 0x1000000000000010;
    pieces[Board::queens] = 0x800000000000008;
    pieces[Board::bishops] = 0x2400000000000024;
    pieces[Board::rooks] = 0x8100000000000081;
    pieces[Board::knights] = 0x4200000000000042;
    pieces[Board::pawns] = 0xff00000000ff00;
}

void Board::print() {
    for (auto a: pieces) {
        std::cout << a << "\n";
    }
}
