#include "../include/engine.hpp"
#include "../include/eval.hpp"
#include "../include/move.hpp"

#include <algorithm>
#include <vector>
#include <iostream>
#include <cctype>
#include <random>

uint64_t Engine::pieceSquareVals[8][8][12];
uint64_t Engine::sideKey;
uint64_t Engine::castlingKey[16];
uint64_t Engine::epKey[8];
std::vector<TTEntry> Engine::tt((1 << 24));

Engine::Engine()
{
}

void Engine::InitZobrist()
{
    std::mt19937_64 rng(8000000);
    std::uniform_int_distribution<uint64_t> dist;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            for (int i = 0; i < 12; i++)
            {
                pieceSquareVals[r][c][i] = dist(rng);
            }
        }
    }

    sideKey = dist(rng);

    for (int i = 0; i < 16; i++)
    {
        castlingKey[i] = dist(rng);
    }
    for (int i = 0; i < 8; i++)
    {
        epKey[i] = dist(rng);
    }
}

int Engine::GetMoveScore(const Move& move, Board& board)
{
    int score = 0;

    //MVV-LVA
    if (move.moveType == CAPTURE)
    {
        char capturedPiece = std::toupper(board.board[move.toRow][move.toCol]);
        char pieceMoved = std::toupper(move.pieceMoved);
        
        score += (this->eval.pieceValue[capturedPiece] * 10 - this->eval.pieceValue[pieceMoved]);
    }
    
    return score;
}

void Engine::OrderMoves(std::vector<Move>& moves, Board& board, int depth)
{
    Move bestMove;
    uint64_t index = board.hash & (Engine::tt.size() - 1);
    TTEntry& entry = Engine::tt[index];
    if (entry.zobristKey == board.hash && entry.depth >= depth)
    {
        bestMove = entry.bestMove;
    }

    for (Move& move : moves)
    {
        move.orderingScore = GetMoveScore(move, board);
    }

    std::sort(moves.begin(), moves.end(), [&board, this, bestMove](const Move& move1, const Move& move2)
        {
            if (move1 == bestMove) return true;
            if (move2 == bestMove) return false;

            return move1.orderingScore > move2.orderingScore;
        }
    );
}

int Engine::Minimax(Board& board, int depth, int alpha, int beta, bool maxPlayer)
{
    this->nodes++;

    uint64_t index = board.hash & (Engine::tt.size() - 1);
    TTEntry& entry = Engine::tt[index];
    if (entry.zobristKey == board.hash && entry.depth >= depth)
    {
        if (entry.scoreFlag == 'e')
        return entry.score;

        if (entry.scoreFlag == 'b' && entry.score >= beta)
        alpha = std::max(alpha, entry.score);

        if (entry.scoreFlag == 'a' && entry.score <= alpha)
        beta = std::min(beta, entry.score);

        if (alpha >= beta) return entry.score;
    }
    
    if (depth == 0) return eval.GetEvalScore(board);

    Move bestMove;
    std::vector<Move> moves;
    board.GetLegalMoves(moves);
    OrderMoves(moves, board, depth);

    int originalAlpha = alpha;
    int originalBeta = beta;

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

            int score = Minimax(board, depth - 1, alpha, beta, !maxPlayer);

            board.UnmakeMove(move, undo);
            if (score > best)
            {
                best = score;
                bestMove = move;
            }
            alpha = std::max(alpha, score);

            if (alpha >= beta) break;
        }

        TTEntry newEntry;
        newEntry.zobristKey = board.hash;
        newEntry.depth = depth;
        newEntry.score = best;

        if (best >= originalBeta) newEntry.scoreFlag = 'b';
        else if (best <= originalAlpha) newEntry.scoreFlag = 'a';
        else newEntry.scoreFlag = 'e';
        
        newEntry.bestMove = bestMove;

        Engine::tt[index] = newEntry;

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

            int score = Minimax(board, depth - 1, alpha, beta, !maxPlayer);

            board.UnmakeMove(move, undo);
            if (score < best)
            {
                best = score;
                bestMove = move;
            }
            beta = std::min(beta, score);

            if (alpha >= beta) break;
        }

        TTEntry newEntry;
        newEntry.zobristKey = board.hash;
        newEntry.depth = depth;
        newEntry.score = best;

        if (best >= originalBeta) newEntry.scoreFlag = 'b';
        else if (best <= originalAlpha) newEntry.scoreFlag = 'a';
        else newEntry.scoreFlag = 'e';
        
        newEntry.bestMove = bestMove;

        Engine::tt[index] = newEntry;

        return best;
    }
}

Move Engine::GetBestMove(Board& board, int depth, bool maxPlayer)
{
    this->nodes = 0;
    int best = 1000000;
    int alpha = -1000000;
    int beta = 1000000;
    if (maxPlayer) 
    {
        best = -1000000;
    }
    Move bestMove;

    std::vector<Move> moves;
    board.GetLegalMoves(moves);
    OrderMoves(moves, board, depth);

    for (const Move& move : moves)
    {
        char originalColor = board.color;
        UndoMove undo = board.MakeMove(move);
        if (board.IsKingChecked(originalColor))
        {
            board.UnmakeMove(move, undo);
            continue;
        }

        int score = Minimax(board, depth - 1, alpha, beta, !maxPlayer);

        board.UnmakeMove(move, undo);
        if (maxPlayer)
        {
            if (score > best)
            {
                best = score;
                bestMove = move;
            }

            alpha = std::max(alpha, score);
        }
        else
        {
            if (score < best)
            {
                best = score;
                bestMove = move;
            }

            beta = std::min(beta, score);
        }
    }

    return bestMove;
}