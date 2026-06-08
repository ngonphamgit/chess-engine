#include "../include/perft.hpp"
#include "../include/board.hpp"
#include "../include/move.hpp"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <stdexcept>  // std::runtime_error
#include <exception>  // std::exception

int Perft(Board& board, int depth)
{
    if (depth == 0) return 1;

    std::vector<Move> moves;
    board.GetLegalMoves(moves);
    int nodes = 0;
    int lastNodes = nodes;

    for (const Move& move : moves)
    {   
        uint64_t originalHash = board.hash;
        try
        {
            //board.PrintBoard();
            char originalColor = board.color;
            UndoMove undo = board.MakeMove(move);

            if (board.IsKingChecked(originalColor))
            {
                board.UnmakeMove(move, undo);
                continue;
            }

            int moveNodes = Perft(board, depth - 1);
            nodes += moveNodes;

            board.UnmakeMove(move, undo);

            if (originalHash != board.hash) throw std::runtime_error("Incorrect restoration");
        }
        catch (const std::exception& e)
        {
            board.PrintBoard();
            std::cout << move.ToString(board);
            std::cout << "Before: " << originalHash << std::endl;
            std::cout << "After: " << board.hash << std::endl;
            std::abort();
        }
    }

    return nodes;
}

void PerftTest(Board& board, int depth)
{
    std::vector<Move> moves;
    board.GetLegalMoves(moves);
    int totalNodes = 0;

    for (const Move& move : moves)
    {
        uint64_t originalHash = board.hash;
        try
        {
            //board.PrintBoard();
            char originalColor = board.color;
            UndoMove undo = board.MakeMove(move);

            if (board.IsKingChecked(originalColor))
            {
                board.UnmakeMove(move, undo);
                continue;
            }

            int nodes = Perft(board, depth - 1);

            totalNodes += nodes;
            board.UnmakeMove(move, undo);

            std::cout << depth << ": " << board.SquareToString(move.fromIndex / 8, move.fromIndex % 8) << " " 
                  << board.SquareToString(move.toIndex / 8, move.toIndex % 8) << ": "
                  << nodes << std::endl;

            if (originalHash != board.hash) throw std::runtime_error("Incorrect restoration");
        }
        catch (const std::exception& e)
        {
            board.PrintBoard();
            std::cout << move.ToString(board);
            std::cout << "Before: " << originalHash << std::endl;
            std::cout << "After: " << board.hash << std::endl;
            std::abort();
        }   
    }

    std::cout << "Total nodes: " << totalNodes << std::endl;
}

void ManualTest(Board& board, int depth)
{
    Move d2d3 = {11, 19, NORMAL};
    UndoMove undo1 = board.MakeMove(d2d3);
    depth--;

    Move a7a6 = {50, 42, NORMAL}; //c7c6
    UndoMove undo2 = board.MakeMove(a7a6);
    depth--;

    //Move a7a6 = {48, 40, NORMAL};
    //UndoMove undo2 = board.MakeMove(a7a6);

    std::vector<Move> nextMoves;
    board.GetLegalMoves(nextMoves);

    for (const Move& move : nextMoves)
    {
        try
        {
            uint64_t originalHash = board.hash;
            char originalColor = board.color;
            UndoMove undo = board.MakeMove(move);

            if (board.IsKingChecked(originalColor))
            {
                std::cout << "king checked\n";
                board.UnmakeMove(move, undo);
                continue;
            }

            int nodes = Perft(board, depth - 1);

            board.UnmakeMove(move, undo);

            std::cout << depth << ": " << board.SquareToString(move.fromIndex / 8, move.fromIndex % 8) << " " 
                  << board.SquareToString(move.toIndex / 8, move.toIndex % 8) << ": "
                  << nodes << std::endl;

            if (originalHash != board.hash) throw std::runtime_error("Incorrect restoration");
        }
        catch (const std::exception& e)
        {
            std::cout << move.ToString(board);
        }
    }
}