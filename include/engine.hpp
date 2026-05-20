#pragma once

#include "board.hpp"
#include "eval.hpp"
#include "move.hpp"

#include <vector>
#include <limits>

struct TTEntry
{
    uint64_t zobristKey = std::numeric_limits<uint64_t>::max();
    Move bestMove;
    int depth;
    int score;
    char scoreFlag;
};

class Engine
{
    public:
    uint64_t nodes = 0;
    Eval eval;

    static uint64_t pieceSquareVals[8][8][12];
    static uint64_t sideKey;
    static uint64_t castlingKey[16];
    static uint64_t epKey[8];
    static std::vector<TTEntry> tt;

    Engine();
    static void InitZobrist();

    int GetMoveScore(const Move& move, Board& board);
    void OrderMoves(std::vector<Move>& moves, Board& board, int depth);
    int Minimax(Board& board, int depth, int alpha, int beta, bool maxPlayer);
    Move GetBestMove(Board& board, int depth, bool maxPlayer);
};