#pragma once

#include "board.hpp"

class Eval
{
    public:
    int pieceValue[256] = {};
    int mobilityValue[256] = {};

    Eval();
    int GetPieceMobility(Board& board, int index);
    int GetEvalScore(Board& board);
};