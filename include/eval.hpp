#pragma once

#include "board.hpp"

class Eval
{
    public:
    int pieceValue[256] = {};

    Eval();
    int GetEvalScore(Board& board);
};