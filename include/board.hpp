#pragma once
#include "move.hpp"

#include <vector>
#include <string>

class Board
{
    private:
    char board[8][8];
    bool whiteKingSide;
    bool whiteQueenSide;
    bool blackKingSide;
    bool blackQueenSide;

    public:
    char color;
    void SetupBoard();
    void PrintBoard();

    std::string SquareToString(int row, int col);

    bool IsWhitePiece(int row, int col);
    bool IsBlackPiece(int row, int col);
    bool IsEmptySquare(int row, int col);
    bool IsSquareAttacked(int row, int col, char color);
    bool IsKingChecked(char color);

    std::vector<Move> GetPawnMoves(int row, int col);
    std::vector<Move> GetKnightMoves(int row, int col);
    std::vector<Move> GetBishopMoves(int row, int col);
    std::vector<Move> GetRookMoves(int row, int col);
    std::vector<Move> GetQueenMoves(int row, int col);
    std::vector<Move> GetKingMoves(int row, int col);
    std::vector<Move> GetLegalMoves();

    void SwitchColors();
    UndoMove MakeMove(const Move& move);
    void UnmakeMove(const Move& move, const UndoMove& undo);
};