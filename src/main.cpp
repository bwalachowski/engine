#include "board.h"
#include "position.h"
#include "move_generator.h"
#include <iostream>

int main(){
    Board emptyBoard;
    emptyBoard.init();
    Position position(emptyBoard, Board::white);
    MoveGenerator generator;
    generator.generatePseudoLegalMoves(position);

}