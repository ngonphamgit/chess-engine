#pragma once

#include "board.hpp"
#include "eval.hpp"
#include "move.hpp"

class Engine
{
    public:
    Eval eval;

    Engine();
    int Minimax(Board& board, int depth, bool maxPlayer);
    Move GetBestMove(Board& board, int depth);
};