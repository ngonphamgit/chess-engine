#include "../include/eval.hpp"

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
        }
    }

    if (board.whiteControl[3][3] == 1) score += 20;
    if (board.whiteControl[3][4] == 1) score += 20;
    if (board.whiteControl[4][3] == 1) score += 20;
    if (board.whiteControl[4][4] == 1) score += 20;

    if (board.blackControl[3][3] == 1) score -= 20;
    if (board.blackControl[3][4] == 1) score -= 20;
    if (board.blackControl[4][3] == 1) score -= 20;
    if (board.blackControl[4][4] == 1) score -= 20;

    score += (board.whiteControlSquares - board.blackControlSquares) * 10;
    
    return score;
}