#include "../include/engine.hpp"
#include "../include/eval.hpp"
#include "../include/move.hpp"

#include <algorithm>
#include <vector>
#include <iostream>
#include <cctype>

Engine::Engine()
{
    
}

int Engine::GetMoveScore(const Move& move, Board& board)
{
    int score = 0;
    if (move.moveType == CAPTURE)
    {
        char capturedPiece = std::toupper(board.board[move.toRow][move.toCol]);
        char pieceMoved = std::toupper(move.pieceMoved);
        
        score += (this->eval.pieceValue[capturedPiece] * 10 - this->eval.pieceValue[pieceMoved]);
    }
    
    return score;
}

void Engine::OrderMoves(std::vector<Move>& moves, Board& board)
{
    for (Move& move : moves)
    {
        move.orderingScore = GetMoveScore(move, board);
    }
    std::sort(moves.begin(), moves.end(), [&board, this](const Move& move1, const Move& move2)
        {
            return move1.orderingScore > move2.orderingScore;
        }
    );
}

int Engine::Minimax(Board& board, int depth, int alpha, int beta, bool maxPlayer)
{
    this->nodes++;

    if (depth == 0) return eval.GetEvalScore(board);

    std::vector<Move> moves;
    board.GetLegalMoves(moves);
    OrderMoves(moves, board);

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
            best = std::max(best, score);
            alpha = std::max(alpha, score);

            if (alpha >= beta) break;
        }

        moves.clear();
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
            best = std::min(best, score);
            beta = std::min(beta, score);

            if (alpha >= beta) break;
        }

        moves.clear();
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
    OrderMoves(moves, board);

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

    moves.clear();
    return bestMove;
}