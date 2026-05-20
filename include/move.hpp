#pragma once

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
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;

    char pieceMoved;
    MoveType moveType;
    PromoteType promoteType;

    int orderingScore;

    bool operator==(const Move& other) const
    {
        return fromRow == other.fromRow &&
               fromCol == other.fromCol &&
               toRow == other.toRow &&
               toCol == other.toCol &&
               moveType == other.moveType &&
               promoteType == other.promoteType;
    }
};

struct UndoMove
{
    char pieceMoved;
    char pieceCaptured;
    char lastColor;

    int enPassantRow;
    int enPassantCol;

    bool whiteKingSide;
    bool whiteQueenSide;
    bool blackKingSide;
    bool blackQueenSide;
};

