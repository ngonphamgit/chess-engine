#pragma once

#include <cstring>
#include <sstream>

class Board;

enum PromoteType
{
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN
};

enum MoveType
{
    NORMAL,
    CAPTURE,
    ENPASSANT,
    CASTLEKING,
    CASTLEQUEEN,
    PAWNDOUBLE,
    PROMOTION
};

struct Move
{
    /*
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
    */

    int fromIndex;
    int toIndex;

    //char pieceMoved;
    MoveType moveType;
    PromoteType promoteType;

    int orderingScore;

    bool operator==(const Move& other) const
    {
        return fromIndex == other.fromIndex &&
               toIndex == other.toIndex &&
               moveType == other.moveType &&
               promoteType == other.promoteType;
    }

    std::string ToString(const Board& board) const;
};

struct UndoMove
{
    char pieceMoved;
    char pieceCaptured;
    char lastColor;

    int epIndex;

    bool whiteKingSide;
    bool whiteQueenSide;
    bool blackKingSide;
    bool blackQueenSide;
};

