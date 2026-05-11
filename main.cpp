#include <iostream>
#include "board.hpp"

int main()
{
    Board board;
    board.SetupBoard();
    board.PrintBoard();
    char color = 'w';

    while (true)
    {
        board.GetLegalMoves(color);
    }

    return 0;
}