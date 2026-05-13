#include "../include/perft.hpp"
#include "../include/board.hpp"
#include "../include/move.hpp"

#include <iostream>
#include <vector>

int Perft(Board& board, int depth)
{
    if (depth == 0) return 1;

    std::vector<Move> moves = board.GetLegalMoves();
    int nodes = 0;

    for (const Move& move : moves)
    {
        char originalColor = board.color;
        UndoMove undo = board.MakeMove(move);
        if (board.IsKingChecked(originalColor))
        {
            board.UnmakeMove(move, undo);
            continue;
        }
        nodes += Perft(board, depth - 1);
        board.UnmakeMove(move, undo);
    }

    return nodes;
}

void PerftTest(Board& board, int depth)
{
    std::vector<Move> moves = board.GetLegalMoves();
    int totalNodes = 0;

    for (const Move& move : moves)
    {
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

        std::cout << board.SquareToString(move.fromRow, move.fromCol) << " " 
                  << board.SquareToString(move.toRow, move.toCol) << ": "
                  << nodes << std::endl;
    }

    std::cout << "Total nodes: " << totalNodes << std::endl;
}