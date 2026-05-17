#pragma once
#include "move.hpp"

#include <vector>
#include <string>

class Board
{
    private:
    bool whiteKingSide;
    bool whiteQueenSide;
    bool blackKingSide;
    bool blackQueenSide;

    public:
    bool gameOver = false;
    char board[8][8];

    int whiteControlSquares = 0;
    int blackControlSquares = 0;
    int whiteControl[8][8] = {};
    int blackControl[8][8] = {};
    
    int whiteKingRow = 7;
    int whiteKingCol = 4;
    int blackKingRow = 0;
    int blackKingCol = 4;

    uint64_t hash;

    int dir[8][2] = {
        {1, 0}, //s
        {0, 1}, //e
        {-1, 0}, //n
        {0, -1}, //w
        {1, 1}, //se
        {1, -1}, //sw
        {-1, 1}, //ne
        {-1, -1} //nw
    };
    int knightOffsets[8][2] = {
        {-2, -1},
        {-2,  1},
        {-1, -2},
        {-1,  2},
        { 1, -2},
        { 1,  2},
        { 2, -1},
        { 2,  1}
    };

    char color;
    int enPassantRow;
    int enPassantCol;

    void SetupBoard();
    void PrintBoard();

    int PieceIndex(char piece); //used for zobrist hashing
    std::string SquareToString(int row, int col);
    Move ParseMove(std::string input);

    bool IsWhitePiece(int row, int col);
    bool IsBlackPiece(int row, int col);
    bool IsEmptySquare(int row, int col);
    bool IsSquareAttacked(int row, int col, char color);
    bool IsKingChecked(char color);
    void SetupControlMaps();
    void UpdateControlMaps(int fromRow, int fromCol, int toRow, int toCol);

    void GetPawnMoves(int row, int col, std::vector<Move>& moves);
    void GetKnightMoves(int row, int col, std::vector<Move>& moves);
    void GetBishopMoves(int row, int col, std::vector<Move>& moves);
    void GetRookMoves(int row, int col, std::vector<Move>& moves);
    void GetQueenMoves(int row, int col, std::vector<Move>& moves);
    void GetKingMoves(int row, int col, std::vector<Move>& moves);
    void GetLegalMoves(std::vector<Move>& moves);

    void SwitchColors();
    UndoMove MakeMove(const Move& move);
    void UnmakeMove(const Move& move, const UndoMove& undo);
};