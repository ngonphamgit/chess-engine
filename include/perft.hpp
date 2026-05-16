#pragma once

#include "board.hpp"
#include "move.hpp"

#include <vector>

void PerftTest(Board& board, int depth);
int Perft(Board& board, int depth, std::vector<Move>& moves);