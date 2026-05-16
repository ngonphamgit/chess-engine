#include "../include/eval.hpp"

#include <cctype>

Eval::Eval()
{
    pieceValue['P'] = 100;
    pieceValue['N'] = 320;
    pieceValue['B'] = 330;
    pieceValue['R'] = 500;
    pieceValue['Q'] = 900;
    pieceValue['K'] = 20000;

    pieceValue['p'] = -100;
    pieceValue['n'] = -320;
    pieceValue['b'] = -330;
    pieceValue['r'] = -500;
    pieceValue['q'] = -900;
    pieceValue['k'] = -20000;

    mobilityValue['N'] = 6;
    mobilityValue['B'] = 7;
    mobilityValue['R'] = 3;
    mobilityValue['Q'] = 1;

    mobilityValue['n'] = -6;
    mobilityValue['b'] = -7;
    mobilityValue['r'] = -3;
    mobilityValue['q'] = -1;
}

int Eval::GetPieceMobility(Board& board, int row, int col)
{
    int score = 0;
    char piece = board.board[row][col];
    bool isWhite = std::isupper(piece);
    char lower = std::tolower(piece);

    switch (lower)
    {
        case 'n':
        {
            for (int i = 0; i < 8; i++)
            {
                int dirRow = board.knightOffsets[i][0];
                int dirCol = board.knightOffsets[i][1];

                int newRow = row + dirRow;
                int newCol = col + dirCol;

                if (newRow < 0 || newRow >= 8) continue;
                if (newCol < 0 || newCol >= 8) continue;

                if (isWhite && board.IsWhitePiece(newRow, newCol)) continue;
                else if (!isWhite && board.IsBlackPiece(newRow, newCol)) continue;

                score += mobilityValue[piece];
            }
            break;
        }
        case 'b':
        case 'r':
        case 'q':
        {
            int start = 0;
            int end = 8;

            if (lower == 'b') start = 4;
            else if (lower == 'r') end = 4;

            for (int i = start; i < end; i++)
            {
                int dirRow = board.dir[i][0];
                int dirCol = board.dir[i][1];

                int newRow = row + dirRow;
                int newCol = col + dirCol;

                while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
                {
                    score += mobilityValue[piece];

                    if (isWhite && board.IsWhitePiece(newRow, newCol)) break;
                    else if (!isWhite && board.IsBlackPiece(newRow, newCol)) break;

                    newRow += dirRow;
                    newCol += dirCol;
                }
            }
            break;
        }
    }

    return score;
}

int Eval::GetEvalScore(Board& board)
{
    int score = 0;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            char piece = board.board[r][c];

            if (piece == '.') continue;

            score += this->pieceValue[piece];
            score += GetPieceMobility(board, r, c);
        }
    }
    
    return score;
}