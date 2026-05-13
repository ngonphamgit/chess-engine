#pragma once

enum MoveType
{
    NORMAL,
    CAPTURE,
    ENPASSANT,
    CASTLEKING,
    CASTLEQUEEN,
    PAWNDOUBLE
};

struct Move
{
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;

    char pieceMoved;
    MoveType moveType;
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

