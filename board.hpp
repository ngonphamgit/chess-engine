#pragma once
#include "move.hpp"

#include <vector>

class Board
{
    private:
    char board[8][8];

    public:
    void SetupBoard();
    void PrintBoard();

    bool IsWhitePiece(int row, int col);
    bool IsBlackPiece(int row, int col);
    bool IsEmptySquare(int row, int col);
    std::vector<Move> GetPawnMoves(char color, int row, int col);
    std::vector<Move> GetKnightMoves(char color, int row, int col);
    std::vector<Move> GetBishopMoves(char color, int row, int col);
    std::vector<Move> GetRookMoves(char color, int row, int col);
    std::vector<Move> GetQueenMoves(char color, int row, int col);
    std::vector<Move> GetKingMoves(char color, int row, int col);
    std::vector<Move> GetLegalMoves(char color);
};