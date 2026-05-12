#pragma once

struct Move
{
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
};

struct UndoMove
{
    char pieceCaptured;
    char lastColor;
};

