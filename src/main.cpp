#include "../include/board.hpp"
#include "../include/perft.hpp"
#include "../include/engine.hpp"

#include <iostream>
#include <string>
#include <chrono>
#include <cstdint>

int main()
{
    Board board;
    board.SetupBoard();

    Engine engine;

    auto start =
        std::chrono::high_resolution_clock::now();

    Move move = engine.GetBestMove(board, 5, false);

    auto end =
        std::chrono::high_resolution_clock::now();

    double seconds =
        std::chrono::duration<double>(end - start).count();

    std::cout << "Nodes: " << engine.nodes << '\n';
    std::cout << "Time: " << seconds << " sec\n";
    std::cout << "NPS: " << (engine.nodes / seconds) << '\n';

    while (!board.gameOver)
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
            Move move = engine.GetBestMove(board, 5, false);
            board.MakeMove(move);
        }
    }

    return 0;
}