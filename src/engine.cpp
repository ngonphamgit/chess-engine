#include "../include/engine.hpp"
#include "../include/eval.hpp"
#include "../include/move.hpp"

#include <algorithm>
#include <vector>

Engine::Engine()
{
    
}

int Engine::Minimax(Board& board, int depth, bool maxPlayer)
{
    if (depth == 0) return eval.GetEvalScore(board);

    std::vector<Move> moves = board.GetLegalMoves();

    if (maxPlayer)
    {
        int best = -1000000;

        for (const Move& move : moves)
        {
            char originalColor = board.color;
            UndoMove undo = board.MakeMove(move);
            if (board.IsKingChecked(originalColor))
            {
                board.UnmakeMove(move, undo);
                continue;
            }

            int score = Minimax(board, depth - 1, !maxPlayer);

            board.UnmakeMove(move, undo);
            best = std::max(best, score);
        }

        return best;
    }
    else
    {
        int best = 1000000;

        for (const Move& move : moves)
        {
            char originalColor = board.color;
            UndoMove undo = board.MakeMove(move);
            if (board.IsKingChecked(originalColor))
            {
                board.UnmakeMove(move, undo);
                continue;
            }

            int score = Minimax(board, depth - 1, !maxPlayer);

            board.UnmakeMove(move, undo);
            best = std::min(best, score);
        }

        return best;
    }
}

Move Engine::GetBestMove(Board& board, int depth)
{
    int best = -1000000;
    Move bestMove;

    std::vector<Move> moves = board.GetLegalMoves();

    bool isMaximizer = (board.color == 'w') ? true : false;

    for (const Move& move : moves)
    {
        char originalColor = board.color;
        UndoMove undo = board.MakeMove(move);
        if (board.IsKingChecked(originalColor))
        {
            board.UnmakeMove(move, undo);
            continue;
        }

        int score = -Minimax(board, depth - 1, !isMaximizer);

        board.UnmakeMove(move, undo);
        if (score > best)
        {
            best = score;
            bestMove = move;
        }
    }

    return bestMove;
}