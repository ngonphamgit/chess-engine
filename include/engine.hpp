#pragma once

#include "board.hpp"
#include "eval.hpp"
#include "move.hpp"

#include <vector>

class Engine
{
    public:
    uint64_t nodes = 0;
    Eval eval;

    Engine();

    int GetMoveScore(const Move& move, Board& board);
    void OrderMoves(std::vector<Move>& moves, Board& board);
    int Minimax(Board& board, int depth, int alpha, int beta, bool maxPlayer);
    Move GetBestMove(Board& board, int depth, bool maxPlayer);
};