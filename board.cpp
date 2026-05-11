#include "board.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cctype>

void Board::SetupBoard()
{
    std::string start[8] = 
    {
        "rnbqkbnr",
        "pppppppp",
        "........",
        "........",
        "........",
        "........",
        "PPPPPPPP",
        "RNBQKBNR"
    };

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            this->board[r][c] = start[r][c];
        }
    }
}

void Board::PrintBoard()
{
     for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            std::cout << this->board[r][c];
        }
        std::cout << std::endl;
    }
}



bool Board::IsWhitePiece(int row, int col)
{
    char piece = this->board[row][col];

    if (piece == '.') return false;

    if (std::isupper(piece)) return true;
    return false;
}

bool Board::IsBlackPiece(int row, int col)
{
    char piece = this->board[row][col];

    if (piece == '.') return false;

    if (std::islower(piece)) return true;
    return false;
}

bool Board::IsEmptySquare(int row, int col)
{
    char piece = this->board[row][col];

    if (piece == '.') return true;
    return false;
}

std::vector<Move> Board::GetPawnMoves(char color, int row, int col)
{
    std::vector<Move> moves;

    if (color == 'w')
    {
        //move one space up
        if (row - 1 >= 0 && IsEmptySquare(row - 1, col))
        {
            moves.push_back({row, col, row - 1, col});

            //move two spaces up on first move
            if (row == 6 && IsEmptySquare(row - 2, col))
            {
                moves.push_back({row, col, row - 2, col});
            }
        }

        //top left capture
        if (col - 1 >= 0 && row - 1 >= 0 && IsBlackPiece(row - 1, col - 1))
        {
            moves.push_back({row, col, row - 1, col - 1});
        }

        //top right capture
        if (col + 1 < 8 && row - 1 >= 0 && IsBlackPiece(row - 1, col + 1))
        {
            moves.push_back({row, col, row - 1, col + 1});
        }
    }
    else if (color == 'b')
    {
        //move one space down
        if (row + 1 < 8 && IsEmptySquare(row + 1, col))
        {
            moves.push_back({row, col, row + 1, col});

            if (row == 1 && IsEmptySquare(row + 2, col))
            {
                moves.push_back({row, col, row + 2, col});
            }
        }

        //bottom left capture
        if (col - 1 >= 0 && row + 1 < 8 && IsWhitePiece(row + 1, col - 1))
        {
            moves.push_back({row, col, row + 1, col - 1});
        }

        //bottom right capture
        if (col + 1 < 8 && row + 1 < 8 && IsWhitePiece(row + 1, col + 1))
        {
            moves.push_back({row, col, row + 1, col + 1});
        }
    }

    return moves;
}

std::vector<Move> Board::GetKnightMoves(char color, int row, int col)
{
    std::vector<Move> moves;

    int offsets[8][2]
    {
        {-2, -1},
        {-2,  1},
        {-1, -2},
        {-1,  2},
        { 1, -2},
        { 1,  2},
        { 2, -1},
        { 2,  1}
    };

    for (int i = 0; i < 8; i++)
    {
        int newRow = row + offsets[i][0];
        int newCol = col + offsets[i][1];
        
        if (newRow < 0 || newRow > 8) continue;
        if (newCol < 0 || newCol > 8) continue;

        if (color == 'w')
        {
            if (!IsWhitePiece(newRow, newCol))
            {
                moves.push_back({row, col, newRow, newCol});
            }
        }
        else if (color == 'b')
        {
            if (!IsBlackPiece(newRow, newCol))
            {
                moves.push_back({row, col, newRow, newCol});
            }
        }
    }

    return moves;
}

std::vector<Move> Board::GetBishopMoves(char color, int row, int col)
{
    std::vector<Move> moves;

    int dir[4][2] = {
        {1, 1},
        {1, -1},
        {-1, 1},
        {-1, -1}
    };

    for (int i = 0; i < 4; i++)
    {
        int dirRow = dir[i][0];
        int dirCol = dir[i][1];

        int newRow = row + dirRow;
        int newCol = col + dirCol;

        while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            if (color == 'w' && IsWhitePiece(newRow, newCol)) break;
            if (color == 'b' && IsBlackPiece(newRow, newCol)) break;

            moves.push_back({row, col, newRow, newCol});

            if (color == 'w' && IsBlackPiece(newRow, newCol)) break;
            if (color == 'b' && IsWhitePiece(newRow, newCol)) break;

            newRow += dirRow;
            newCol += dirCol;
        }
    }

    return moves;
}

std::vector<Move> Board::GetRookMoves(char color, int row, int col)
{
    std::vector<Move> moves;

    int dir[4][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1}
    };

    for (int i = 0; i < 4; i++)
    {
        int dirRow = dir[i][0];
        int dirCol = dir[i][1];

        int newRow = row + dirRow;
        int newCol = col + dirCol;

        while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            if (color == 'w' && IsWhitePiece(newRow, newCol)) break;
            if (color == 'b' && IsBlackPiece(newRow, newCol)) break;

            moves.push_back({row, col, newRow, newCol});

            if (color == 'w' && IsBlackPiece(newRow, newCol)) break;
            if (color == 'b' && IsWhitePiece(newRow, newCol)) break;

            newRow += dirRow;
            newCol += dirCol;
        }
    }

    return moves;
}

std::vector<Move> Board::GetQueenMoves(char color, int row, int col)
{
    std::vector<Move> moves;

    std::vector<Move> bishopMoves = this->GetBishopMoves(color, row, col);
    std::vector<Move> rookMoves = this->GetRookMoves(color, row, col);

    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());

    return moves;
}

std::vector<Move> Board::GetKingMoves(char color, int row, int col)
{
    std::vector<Move> moves;

    int dir[8][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
        {1, 1},
        {1, -1},
        {-1, 1},
        {-1, -1}
    };

    for (int i = 0; i < 8; i++)
    {
        int newRow = row + dir[i][0];
        int newCol = col + dir[i][1];

        if (newRow < 0 || newRow >= 8) continue;
        if (newCol < 0 || newCol >= 8) continue;

        if (color == 'w' && !IsWhitePiece(newRow, newCol))
        {
            moves.push_back({row, col, newRow, newCol});
        }
        else if (color == 'b' && !IsBlackPiece(newRow, newCol))
        {
            moves.push_back({row, col, newRow, newCol});
        }
    }

    return moves;
}

std::vector<Move> Board::GetLegalMoves(char color)
{
    std::vector<Move> legalMoves;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            char piece = this->board[r][c];

            if (piece == '.') continue;
            if (color == 'w' && !IsWhitePiece(r, c)) continue;
            if (color == 'b' && !IsBlackPiece(r, c)) continue;

            switch (piece)
            {
                case 'P':
                {
                    std::vector<Move> moves = this->GetPawnMoves(color, r, c);
                    legalMoves.insert(legalMoves.end(), moves.begin(), moves.end());
                    break;
                }
                case 'N':
                {  
                    std::vector<Move> moves = this->GetKnightMoves(color, r, c);
                    legalMoves.insert(legalMoves.end(), moves.begin(), moves.end());
                    break;
                }
                case 'B':
                {
                    std::vector<Move> moves = this->GetBishopMoves(color, r, c);
                    legalMoves.insert(legalMoves.end(), moves.begin(), moves.end());
                    break;
                }
                case 'R':
                {
                    std::vector<Move> moves = this->GetRookMoves(color, r, c);
                    legalMoves.insert(legalMoves.end(), moves.begin(), moves.end());
                    break;
                }
                case 'Q':
                {
                    std::vector<Move> moves = this->GetQueenMoves(color, r, c);
                    legalMoves.insert(legalMoves.end(), moves.begin(), moves.end());
                    break;
                }
                case 'K':
                {
                    std::vector<Move> moves = this->GetKingMoves(color, r, c);
                    legalMoves.insert(legalMoves.end(), moves.begin(), moves.end());
                    break;
                }
            }
        }
    }

    return legalMoves;
}