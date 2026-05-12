#pragma once

#include "board.hpp"
#include "move.hpp"

void PerftTest(Board& board, int depth);
int Perft(Board& board, int depth);