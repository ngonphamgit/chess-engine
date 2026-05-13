#include "../include/board.hpp"
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

    this->color = 'w';
    this->whiteKingSide = true;
    this->whiteQueenSide = true;
    this->blackKingSide = true;
    this->blackQueenSide = true;
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

std::string Board::SquareToString(int row, int col)
{
    char file = 'a' + col;
    char rank = '8' - row;

    return std::string() + file + rank;
}

bool Board::IsWhitePiece(int row, int col)
{
    char piece = this->board[row][col];

    if (std::isupper(piece)) return true;
    return false;
}

bool Board::IsBlackPiece(int row, int col)
{
    char piece = this->board[row][col];

    if (std::islower(piece)) return true;
    return false;
}

bool Board::IsEmptySquare(int row, int col)
{
    char piece = this->board[row][col];

    if (piece == '.') return true;
    return false;
}

bool Board::IsSquareAttacked(int row, int col, char color)
{
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            char piece = board[r][c];
            if (piece == '.') continue;

            bool isWhite = IsWhitePiece(r, c);
            bool isWhiteTarget = (color == 'w');

            if (isWhite != isWhiteTarget) continue;

            char lower = std::tolower(piece);
            switch (lower)
            {
                case 'p':
                {
                    int dir = isWhite ? -1 : 1;

                    if (row == r + dir && (col == c - 1 || col == c + 1)) return true;

                    break;
                }
                case 'n':
                {
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
                        int targetRow = r + offsets[i][0];
                        int targetCol = c + offsets[i][1];

                        if (targetRow == row && targetCol == col) return true;
                    }

                    break;
                }
                case 'b':
                case 'r':
                case 'q':
                {
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

                    int start = 0;
                    int end = 8;

                    if (lower == 'b')
                    {
                        start = 4;
                    }
                    else if (lower == 'r')
                    {
                        end = 4;
                    }

                    for (int i = start; i < end; i++)
                    {
                        int dirRow = dir[i][0];
                        int dirCol = dir[i][1];

                        int curRow = r + dirRow;
                        int curCol = c + dirCol;

                        while (curRow >= 0 && curRow < 8 && curCol >= 0 && curCol < 8)
                        {
                            if (curRow == row && curCol == col) return true;

                            if (!IsEmptySquare(curRow, curCol)) break;

                            curRow += dirRow;
                            curCol += dirCol;
                        }
                    }

                    break;
                }
                case 'k':
                {
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
                        int newRow = r + dir[i][0];
                        int newCol = c + dir[i][1];

                        if (newRow < 0 || newRow >= 8) continue;
                        if (newCol < 0 || newCol >= 8) continue;

                        if (newRow == row && newCol == col) return true;
                    }

                    break;
                }
            }
        }
    }

    return false;
}

bool Board::IsKingChecked(char color)
{
    int kingRow = -1;
    int kingCol = -1;
    bool found = false;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (color == 'w' && this->board[r][c] == 'K')
            {
                kingRow = r;
                kingCol = c;
                found = true;
                break;
            }
            else if (color == 'b' && this->board[r][c] == 'k')
            {
                kingRow = r;
                kingCol = c;
                found = true;
                break;
            }
        }

        if (found) break;
    }

    char enemyColor;
    if (color == 'w') enemyColor = 'b'; else enemyColor = 'w';
    return IsSquareAttacked(kingRow, kingCol, enemyColor);
}

std::vector<Move> Board::GetPawnMoves(int row, int col)
{
    std::vector<Move> moves;

    if (IsWhitePiece(row, col))
    {
        //move one space up
        if (row - 1 >= 0 && IsEmptySquare(row - 1, col))
        {
            moves.push_back({row, col, row - 1, col, this->board[row][col]});

            //move two spaces up on first move
            if (row == 6 && IsEmptySquare(row - 2, col))
            {
                moves.push_back({row, col, row - 2, col, this->board[row][col]});
            }
        }

        //top left capture
        if (col - 1 >= 0 && row - 1 >= 0 && IsBlackPiece(row - 1, col - 1))
        {
            moves.push_back({row, col, row - 1, col - 1, this->board[row][col]});
        }

        //top right capture
        if (col + 1 < 8 && row - 1 >= 0 && IsBlackPiece(row - 1, col + 1))
        {
            moves.push_back({row, col, row - 1, col + 1, this->board[row][col]});
        }
    }
    else
    {
        //move one space down
        if (row + 1 < 8 && IsEmptySquare(row + 1, col))
        {
            moves.push_back({row, col, row + 1, col, this->board[row][col]});

            if (row == 1 && IsEmptySquare(row + 2, col))
            {
                moves.push_back({row, col, row + 2, col, this->board[row][col]});
            }
        }

        //bottom left capture
        if (col - 1 >= 0 && row + 1 < 8 && IsWhitePiece(row + 1, col - 1))
        {
            moves.push_back({row, col, row + 1, col - 1, this->board[row][col]});
        }

        //bottom right capture
        if (col + 1 < 8 && row + 1 < 8 && IsWhitePiece(row + 1, col + 1))
        {
            moves.push_back({row, col, row + 1, col + 1, this->board[row][col]});
        }
    }

    return moves;
}

std::vector<Move> Board::GetKnightMoves(int row, int col)
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
        
        if (newRow < 0 || newRow >= 8) continue;
        if (newCol < 0 || newCol >= 8) continue;

        if (IsWhitePiece(row, col))
        {
            if (!IsWhitePiece(newRow, newCol))
            {
                moves.push_back({row, col, newRow, newCol, this->board[row][col]});
            }
        }
        else
        {
            if (!IsBlackPiece(newRow, newCol))
            {
                moves.push_back({row, col, newRow, newCol, this->board[row][col]});
            }
        }
    }

    return moves;
}

std::vector<Move> Board::GetBishopMoves(int row, int col)
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
            //stop if target square is a friendly piece
            if (IsWhitePiece(row, col) && IsWhitePiece(newRow, newCol)) break;
            if (IsBlackPiece(row, col) && IsBlackPiece(newRow, newCol)) break;

            moves.push_back({row, col, newRow, newCol, this->board[row][col]});

            //stop if target square is an enemy piece
            if (IsWhitePiece(row, col) && IsBlackPiece(newRow, newCol)) break;
            if (IsBlackPiece(row, col) && IsWhitePiece(newRow, newCol)) break;

            newRow += dirRow;
            newCol += dirCol;
        }
    }

    return moves;
}

std::vector<Move> Board::GetRookMoves(int row, int col)
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
            if (IsWhitePiece(row, col) && IsWhitePiece(newRow, newCol)) break;
            if (IsBlackPiece(row, col) && IsBlackPiece(newRow, newCol)) break;

            moves.push_back({row, col, newRow, newCol, this->board[row][col]});

            if (IsWhitePiece(row, col) && IsBlackPiece(newRow, newCol)) break;
            if (IsBlackPiece(row, col) && IsWhitePiece(newRow, newCol)) break;

            newRow += dirRow;
            newCol += dirCol;
        }
    }

    return moves;
}

std::vector<Move> Board::GetQueenMoves(int row, int col)
{
    std::vector<Move> moves;

    std::vector<Move> bishopMoves = this->GetBishopMoves(row, col);
    std::vector<Move> rookMoves = this->GetRookMoves(row, col);

    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());

    return moves;
}

std::vector<Move> Board::GetKingMoves(int row, int col)
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

        if (IsWhitePiece(row, col) && !IsWhitePiece(newRow, newCol))
        {
            moves.push_back({row, col, newRow, newCol, this->board[row][col]});
        }
        else if (IsBlackPiece(row, col) && !IsBlackPiece(newRow, newCol))
        {
            moves.push_back({row, col, newRow, newCol, this->board[row][col]});
        }
    }

    //castling
    if (IsWhitePiece(row, col))
    {
        if (this->whiteKingSide 
            && IsEmptySquare(row, col + 1) 
            && IsEmptySquare(row, col + 2)
            && !IsKingChecked('w')
            && !IsSquareAttacked(row, col + 1, 'b')
            && !IsSquareAttacked(row, col + 2, 'b')
            && this->board[row][7] == 'R')
        {
            moves.push_back({row, col, row, col + 2, this->board[row][col], CASTLEKING});
        }
        if (this->whiteQueenSide 
            && IsEmptySquare(row, col - 1) 
            && IsEmptySquare(row, col - 2)
            && IsEmptySquare(row, col - 3)
            && !IsKingChecked('w')
            && !IsSquareAttacked(row, col - 1, 'b')
            && !IsSquareAttacked(row, col - 2, 'b')
            && this->board[row][0] == 'R')
        {
            moves.push_back({row, col, row, col - 2, this->board[row][col], CASTLEQUEEN});
        }
    }
    else
    {
        if (this->blackKingSide 
            && IsEmptySquare(row, col + 1) 
            && IsEmptySquare(row, col + 2)
            && !IsKingChecked('b')
            && !IsSquareAttacked(row, col + 1, 'w')
            && !IsSquareAttacked(row, col + 2, 'w')
            && this->board[row][7] == 'r')
        {
            moves.push_back({row, col, row, col + 2, this->board[row][col], CASTLEKING});
        }
        if (this->blackQueenSide 
            && IsEmptySquare(row, col - 1) 
            && IsEmptySquare(row, col - 2)
            && IsEmptySquare(row, col - 3)
            && !IsKingChecked('b')
            && !IsSquareAttacked(row, col - 1, 'w')
            && !IsSquareAttacked(row, col - 2, 'w')
            && this->board[row][0] == 'r')
        {
            moves.push_back({row, col, row, col - 2, this->board[row][col], CASTLEQUEEN});
        }
    }

    return moves;
}

std::vector<Move> Board::GetLegalMoves()
{
    std::vector<Move> pseudoMoves;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            char piece = this->board[r][c];

            if (piece == '.') continue;
            if (this->color == 'w' && !IsWhitePiece(r, c)) continue;
            if (this->color == 'b' && !IsBlackPiece(r, c)) continue;

            switch (piece)
            {
                case 'P':
                case 'p':
                {
                    std::vector<Move> moves = GetPawnMoves(r, c);
                    pseudoMoves.insert(pseudoMoves.end(), moves.begin(), moves.end());
                    break;
                }
                case 'N':
                case 'n':
                {  
                    std::vector<Move> moves = GetKnightMoves(r, c);
                    pseudoMoves.insert(pseudoMoves.end(), moves.begin(), moves.end());
                    break;
                }
                case 'B':
                case 'b':
                {
                    std::vector<Move> moves = GetBishopMoves(r, c);
                    pseudoMoves.insert(pseudoMoves.end(), moves.begin(), moves.end());
                    break;
                }
                case 'R':
                case 'r':
                {
                    std::vector<Move> moves = GetRookMoves(r, c);
                    pseudoMoves.insert(pseudoMoves.end(), moves.begin(), moves.end());
                    break;
                }
                case 'Q':
                case 'q':
                {
                    std::vector<Move> moves = GetQueenMoves(r, c);
                    pseudoMoves.insert(pseudoMoves.end(), moves.begin(), moves.end());
                    break;
                }
                case 'K':
                case 'k':
                {
                    std::vector<Move> moves = GetKingMoves(r, c);
                    pseudoMoves.insert(pseudoMoves.end(), moves.begin(), moves.end());
                    break;
                }
            }
        }
    }
    /*
    std::vector<Move> legalMoves;

    for (const Move& move : pseudoMoves)
    {
        char originalColor = this->color;

        UndoMove undo = MakeMove(move);

        if (!IsKingChecked(originalColor))
        {
            std::cout << "added" << std::endl;
            legalMoves.push_back(move);
        }

        UnmakeMove(move, undo);
    }
    */

    return pseudoMoves;
}

void Board::SwitchColors()
{
    if (this->color == 'w')
    {
        this->color = 'b';
    }
    else if (this->color == 'b')
    {
        this->color = 'w';
    }
}

UndoMove Board::MakeMove(const Move& move)
{
    UndoMove undo;

    char fromPiece = this->board[move.fromRow][move.fromCol];
    char toPiece = this->board[move.toRow][move.toCol];

    undo.pieceMoved = fromPiece;
    undo.pieceCaptured = toPiece;
    undo.lastColor = this->color;

    undo.whiteKingSide = whiteKingSide;
    undo.whiteQueenSide = whiteQueenSide;
    undo.blackKingSide = blackKingSide;
    undo.blackQueenSide = blackQueenSide;

    //can't castle after moving king
    if (fromPiece == 'K')
    {
        whiteKingSide = false;
        whiteQueenSide = false;
    }
    else if (fromPiece == 'k')
    {
        blackKingSide = false;
        blackQueenSide = false;
    }
    //cant castle after moving rook
    else if (fromPiece == 'R')
    {
        if (move.fromCol == 7) whiteKingSide = false;
        if (move.fromCol == 0) whiteQueenSide = false;
    }
    else if (fromPiece == 'r')
    {
        if (move.fromCol == 7) blackKingSide = false;
        if (move.fromCol == 0) blackQueenSide = false;
    }
    //cant castle after rook is taken
    if (toPiece == 'R')
    {
        if (move.toRow == 7 && move.toCol == 7) whiteKingSide = false;
        if (move.toRow == 7 && move.toCol == 0) whiteQueenSide = false;
    }
    else if (toPiece == 'r')
    {
        if (move.toRow == 0 && move.toCol == 7) blackKingSide = false;
        if (move.toRow == 0 && move.toCol == 0) blackQueenSide = false;
    }

    //apply castling moves
    if (move.moveType == CASTLEKING)
    {
        if (fromPiece == 'K')
        {
            board[7][4] = '.';
            board[7][6] = 'K';

            board[7][7] = '.';
            board[7][5] = 'R';
        }
        else
        {
            board[0][4] = '.';
            board[0][6] = 'k';

            board[0][7] = '.';
            board[0][5] = 'r';
        }
    }
    else if (move.moveType == CASTLEQUEEN)
    {
        if (fromPiece == 'K')
        {
            board[7][4] = '.';
            board[7][2] = 'K';

            board[7][0] = '.';
            board[7][3] = 'R';
        }
        else
        {
            board[0][4] = '.';
            board[0][2] = 'k';

            board[0][0] = '.';
            board[0][3] = 'r';
        }
    }
    //apply normal moves
    else
    {
        this->board[move.fromRow][move.fromCol] = '.';
        this->board[move.toRow][move.toCol] = fromPiece;
    }
    SwitchColors();

    return undo;
}

void Board::UnmakeMove(const Move& move, const UndoMove& undo)
{
    char fromPiece = undo.pieceMoved;
    char toPiece = undo.pieceCaptured;

    this->color = undo.lastColor;

    this->whiteKingSide = undo.whiteKingSide;
    this->whiteQueenSide = undo.whiteQueenSide;
    this->blackKingSide = undo.blackKingSide;
    this->blackQueenSide = undo.blackQueenSide;

    if (move.moveType == CASTLEKING)
    {
        if (fromPiece == 'K')
        {
            board[7][6] = '.';
            board[7][4] = 'K';

            board[7][5] = '.';
            board[7][7] = 'R';
        }
        else
        {
            board[0][6] = '.';
            board[0][4] = 'k';

            board[0][5] = '.';
            board[0][7] = 'r';
        }
    }
    else if (move.moveType == CASTLEQUEEN)
    {
        if (fromPiece == 'K')
        {
            board[7][2] = '.';
            board[7][4] = 'K';

            board[7][3] = '.';
            board[7][0] = 'R';
        }
        else
        {
            board[0][2] = '.';
            board[0][4] = 'k';

            board[0][3] = '.';
            board[0][0] = 'r';
        }
    }
    else
    {
        this->board[move.toRow][move.toCol] = toPiece;
        this->board[move.fromRow][move.fromCol] = fromPiece;
    }
}