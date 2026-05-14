#include "../include/board.hpp"
#include "../include/perft.hpp"
#include "../include/engine.hpp"

#include <iostream>
#include <string>

int main()
{
    Board board;
    board.SetupBoard();

    Engine engine;

    while (true)
    {
        board.PrintBoard();
        
        if (board.color == 'w')
        {
            std::string inputMove;

            std::cout << "Play a move: ";
            std::cin >> inputMove;

            Move move = board.ParseMove(inputMove);
            board.MakeMove(move);
        }
        else
        {
            Move move = engine.GetBestMove(board, 4);
            board.MakeMove(move);
        }
    }

    return 0;
}