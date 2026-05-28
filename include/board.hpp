#pragma once
#include "move.hpp"

#include <vector>
#include <string>
#include <cstdint>

class Board
{

    /*
    8 56 57 58 59 60 61 62 63
    7 48 49 50 51 52 53 54 55
    6 40 41 42 43 44 45 46 47
    5 32 33 34 35 36 37 38 39
    4 24 25 26 27 28 29 30 31
    3 16 17 18 19 20 21 22 23
    2  8  9 10 11 12 13 14 15
    1  0  1  2  3  4  5  6  7
       a  b  c  d  e  f  g  h
    */
    private:
    bool whiteKingSide;
    bool whiteQueenSide;
    bool blackKingSide;
    bool blackQueenSide;

    public:
    bool gameOver = false;
    char board[8][8];

    uint64_t whitePieces;
    uint64_t whitePawns;
    uint64_t whiteKnights;
    uint64_t whiteBishops;
    uint64_t whiteRooks;
    uint64_t whiteQueens;
    uint64_t whiteKing;

    uint64_t blackPieces;
    uint64_t blackPawns;
    uint64_t blackKnights;
    uint64_t blackBishops;
    uint64_t blackRooks;
    uint64_t blackQueens;
    uint64_t blackKing;

    uint64_t occupiedSquares;

    const uint64_t FILE_A = 0x0101010101010101ULL;
    const uint64_t FILE_B = 0x0202020202020202ULL; //for checking knight attacks
    const uint64_t FILE_G = 0x4040404040404040ULL; //for checking knight attacks
    const uint64_t FILE_H = 0x8080808080808080ULL;
    const uint64_t RANK_1 = 0x00000000000000FFULL;
    const uint64_t RANK_2 = 0x000000000000FF00ULL; //for checking knight attacks
    const uint64_t RANK_6 = 0x00FF000000000000ULL; //for checking knight attacks
    const uint64_t RANK_7 = 0xFF00000000000000ULL;

    int whiteControlSquares = 0;
    int blackControlSquares = 0;
    int whiteControl[8][8] = {};
    int blackControl[8][8] = {};
    
    int whiteKingRow = 7;
    int whiteKingCol = 4;
    int blackKingRow = 0;
    int blackKingCol = 4;

    uint64_t hash;

    uint64_t whitePawnAttacks[8][8];
    uint64_t blackPawnAttacks[8][8];
    uint64_t knightAttacks[8][8];
    uint64_t kingAttacks[8][8];

    int rookRays[4][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
    };

    int bishopRays[4][2] = {
        {1, -1},
        {-1, 1},
        {1, 1},
        {-1, -1}
    };

    char color;
    int enPassantRow;
    int enPassantCol;

    void SetupBoard();
    void PrintBoard();

    int PieceIndex(char piece); //used for zobrist hashing
    std::string SquareToString(int row, int col);
    Move ParseMove(std::string input);

    int PopLSB(uint64_t& bb);
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