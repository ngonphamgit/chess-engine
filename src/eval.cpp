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

int Eval::GetPieceMobility(Board& board, int currIndex)
{
    int score = 0;
    char piece = board.GetPieceAtIndex(currIndex);
    uint64_t sq = 1ULL << currIndex;
    bool isWhite = std::isupper(piece);
    uint64_t friendlyPieces = (board.whitePieces & sq) ? board.whitePieces : board.blackPieces;
    uint64_t enemyPieces = (board.whitePieces & sq) ? board.blackPieces : board.whitePieces;
    char lower = std::tolower(piece);

    int row = currIndex / 8;
    int col = currIndex % 8;
    
    switch (lower)
    {
        case 'n':
        {
            uint64_t attacks = board.knightAttacks[row][col];

            while (attacks)
            {
                int newIndex = board.PopLSB(attacks);
                int newSq = 1ULL << newIndex;

                if (friendlyPieces & newSq) continue;

                score += mobilityValue[piece];
            }

            break;
        }
        case 'b':
        {
            for (int i = 0; i < 4; i++)
            {
                int dirRow = board.bishopRays[i][0];
                int dirCol = board.bishopRays[i][1];

                int newRow = row + dirRow;
                int newCol = col + dirCol;

                while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
                {
                    int newIndex = newRow * 8 + newCol;
                    uint64_t newSq = 1ULL << newIndex;

                    if (friendlyPieces & newSq) break;

                    score += mobilityValue[piece];

                    if (enemyPieces & newSq) break;

                    newRow += dirRow;
                    newCol += dirCol;
                }
            }

            break;
        }
        case 'r':
        {
            for (int i = 0; i < 4; i++)
            {
                int dirRow = board.rookRays[i][0];
                int dirCol = board.rookRays[i][1];

                int newRow = row + dirRow;
                int newCol = col + dirCol;

                while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
                {
                    int newIndex = newRow * 8 + newCol;
                    uint64_t newSq = 1ULL << newIndex;

                    if (friendlyPieces & newSq) break;
                    
                    score += mobilityValue[piece];

                    if (enemyPieces & newSq) break;

                    newRow += dirRow;
                    newCol += dirCol;
                }
            }

            break;
        }
        case 'q':
        {
            for (int i = 0; i < 4; i++)
            {
                int dirRow = board.bishopRays[i][0];
                int dirCol = board.bishopRays[i][1];

                int newRow = row + dirRow;
                int newCol = col + dirCol;

                while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
                {
                    int newIndex = newRow * 8 + newCol;
                    uint64_t newSq = 1ULL << newIndex;

                    if (friendlyPieces & newSq) break;
                    
                    score += mobilityValue[piece];

                    if (enemyPieces & newSq) break;

                    newRow += dirRow;
                    newCol += dirCol;
                }
            }

            for (int i = 0; i < 4; i++)
            {
                int dirRow = board.rookRays[i][0];
                int dirCol = board.rookRays[i][1];

                int newRow = row + dirRow;
                int newCol = col + dirCol;

                while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
                {
                    int newIndex = newRow * 8 + newCol;
                    uint64_t newSq = 1ULL << newIndex;

                    if (friendlyPieces & newSq) break;
                    
                    score += mobilityValue[piece];

                    if (enemyPieces & newSq) break;

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

    uint64_t tempOccupied = board.occupiedSquares;

    while (tempOccupied)
    {
        int index = board.PopLSB(tempOccupied);
        char piece = board.GetPieceAtIndex(index);
        
        if (piece == '.') continue;

        score += this->pieceValue[piece];
        score += GetPieceMobility(board, index); 
    }
    
    return score;
}