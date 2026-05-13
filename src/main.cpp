#include <iostream>
#include "../include/board.hpp"
#include "../include/perft.hpp"

int main()
{
    Board board;
    board.SetupBoard();
    board.PrintBoard();
    char color = 'w';

    PerftTest(board, 6);

    return 0;
}