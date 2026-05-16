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

    Move move = engine.GetBestMove(board, 5, true);

    auto end =
        std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    double seconds = elapsed.count();

    std::cout << "Nodes: " << engine.nodes << '\n';
    std::cout << "Time: " << seconds << " sec\n";
    std::cout << "NPS: " << (engine.nodes / seconds) << '\n';

    while (!board.gameOver)
    {
        board.PrintBoard();
        
        if (board.color == 'b')
        {
            std::string inputMove;

            std::cout << "Play a move: ";
            std::cin >> inputMove;

            Move move = board.ParseMove(inputMove);
            board.MakeMove(move);
        }
        else
        {
            auto start = std::chrono::high_resolution_clock::now();
            Move move = engine.GetBestMove(board, 6, true);
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> elapsed = end - start;
            double seconds = elapsed.count();

            std::cout << "Nodes: " << engine.nodes << '\n';
            std::cout << "Time: " << seconds << " sec\n";
            std::cout << "NPS: " << (engine.nodes / seconds) << '\n';
            board.MakeMove(move);
        }
    }

    return 0;
}