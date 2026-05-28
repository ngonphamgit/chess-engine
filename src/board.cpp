#include "../include/board.hpp"
#include "../include/engine.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstring>
#include <cstdint>

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

    this->whitePieces = 0x000000000000FFFFULL;
    this->whitePawns   = 0x000000000000FF00ULL;
    this->whiteRooks   = 0x0000000000000081ULL;
    this->whiteKnights = 0x0000000000000042ULL;
    this->whiteBishops = 0x0000000000000024ULL;
    this->whiteQueens  = 0x0000000000000008ULL;
    this->whiteKing    = 0x0000000000000010ULL;

    this->blackPieces = 0xFFFF000000000000ULL;
    this->blackPawns   = 0x00FF000000000000ULL;
    this->blackRooks   = 0x8100000000000000ULL;
    this->blackKnights = 0x4200000000000000ULL;
    this->blackBishops = 0x2400000000000000ULL;
    this->blackQueens  = 0x0800000000000000ULL;
    this->blackKing    = 0x1000000000000000ULL;

    //generate pawn attacks
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            uint64_t whiteAttacks = 0ULL;
            uint64_t blackAttacks = 0ULL;
            int index = r * 8 + c;
            uint64_t whitePawn = 1ULL << index;
            uint64_t blackPawn = 1ULL << index;
            
            if (!(whitePawn & FILE_A)) whiteAttacks |= whitePawn << 7;
            if (!(whitePawn & FILE_H)) whiteAttacks |= whitePawn << 9;
            if (!(blackPawn & FILE_A)) blackAttacks |= blackPawn >> 9;
            if (!(blackPawn & FILE_H)) blackAttacks |= blackPawn >> 7;

            whitePawnAttacks[r][c] = whiteAttacks;
            blackPawnAttacks[r][c] = blackAttacks;
        }
    }

    int knightDirs[8][2] = {
        {2, 1},
        {-2, 1},
        {2, -1},
        {-2, -1},
        {1, 2},
        {-1, 2},
        {1, -2},
        {-1, -2}
    };
    //generate knight attacks
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            uint64_t attacks = 0ULL;

            for (int i = 0; i < 8; i++)
            {
                int dirRow = knightDirs[i][0];
                int dirCol = knightDirs[i][1];

                int newRow = r + dirRow;
                int newCol = c + dirCol;

                if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
                {
                    int attackSq = newRow * 8 + newCol;
                    attacks |= 1ULL << attackSq;
                }
            }

            this->knightAttacks[r][c] = attacks;
        }
    }

    int kingDirs[8][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
        {1, -1},
        {-1, 1},
        {1, 1},
        {-1, -1}
    };
    //generate king attacks
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            uint64_t attacks = 0ULL;

            for (int i = 0; i < 8; i++)
            {
                int dirRow = kingDirs[i][0];
                int dirCol = kingDirs[i][1];

                int newRow = r + dirRow;
                int newCol = c + dirCol;

                if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
                {
                    int attackSq = newRow * 8 + newCol;
                    attacks |= 1ULL << attackSq;
                }
            }

            this->kingAttacks[r][c] = attacks;
        }
    }

    this->occupiedSquares = whitePieces | blackPieces;

    this->color = 'w';
    this->enPassantRow = -1;
    this->enPassantCol = -1;
    this->whiteKingSide = true;
    this->whiteQueenSide = true;
    this->blackKingSide = true;
    this->blackQueenSide = true;
}

void Board::PrintBoard()
{
    /*
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            std::cout << this->board[r][c] << " ";
        }
        std::cout << std::endl;
    }
    */

    for (int r = 7; r >= 0; r--)
    {
        for (int c = 0; c < 8; c++)
        {
            int index = r * 8 + c;
            uint64_t mask = 1ULL << index;

            if (whitePawns & mask) std::cout << "P ";
            else if (whiteKnights & mask) std::cout << "N ";
            else if (whiteBishops & mask) std::cout << "B ";
            else if (whiteRooks & mask) std::cout << "R ";
            else if (whiteQueens & mask) std::cout << "Q ";
            else if (whiteKing & mask) std::cout << "K ";

            else if (blackPawns & mask) std::cout << "p ";
            else if (blackKnights & mask) std::cout << "n ";
            else if (blackBishops & mask) std::cout << "b ";
            else if (blackRooks & mask) std::cout << "r ";
            else if (blackQueens & mask) std::cout << "q ";
            else if (blackKing & mask) std::cout << "k ";

            else std::cout << ". ";
        }
        std::cout << std::endl;
    }
}

int Board::PieceIndex(char piece)
{
    switch (piece)
    {
        case 'P': return 0;
        case 'N': return 1;
        case 'B': return 2;
        case 'R': return 3;
        case 'Q': return 4;
        case 'K': return 5;

        case 'p': return 6;
        case 'n': return 7;
        case 'b': return 8;
        case 'r': return 9;
        case 'q': return 10;
        case 'k': return 11;
    }
    return -1;
}

std::string Board::SquareToString(int row, int col)
{
    char file = 'a' + col;
    char rank = '8' - row;

    return std::string() + file + rank;
}

Move Board::ParseMove(std::string input)
{
    Move move;
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;

    fromCol = input[0] - 'a';
    fromRow = 8 - (input[1] - '0');

    if (input == "O-O")
    {
        if (this->color == 'w')
        {
            return {7, 4, 7, 6, this->board[7][4], CASTLEKING};
        }
        else
        {
            return {0, 4, 0, 6, this->board[0][4], CASTLEKING};
        }
        
    }
    else if (input == "O-O-O")
    {
        if (this->color == 'w')
        {
            return {7, 4, 7, 2, this->board[7][4], CASTLEKING};
        }
        else
        {
            return {0, 4, 0, 2, this->board[0][4], CASTLEKING};
        }
    }

    if (input[2] == 'x')
    {
        toCol = input[3] - 'a';
        toRow =  8 - (input[4] - '0');
        move.moveType = CAPTURE;
    }
    else
    {
        toCol = input[2] - 'a';
        toRow = 8 - (input[3] - '0');
        move.moveType = NORMAL;
    }

    move.fromRow = fromRow;
    move.fromCol = fromCol;
    move.toRow = toRow;
    move.toCol = toCol;

    move.pieceMoved = this->board[fromRow][fromCol];

    //std::cout << fromRow << " " << fromCol << " " << toRow << " " << toCol << " " << move.pieceMoved << std::endl;

    return move;
}

int PopLSB(uint64_t& bb)
{
    int index = __builtin_ctzll(bb);
    bb &= bb - 1;
    return index;
}

bool Board::IsWhitePiece(int row, int col)
{
    /*
    char piece = this->board[row][col];

    if (std::isupper(piece)) return true;
    return false;
    */

    int index = row * 8 + col;
    if (whitePieces & (1ULL << index)) return true;
    return false;
}

bool Board::IsBlackPiece(int row, int col)
{
    /*
    char piece = this->board[row][col];

    if (std::islower(piece)) return true;
    return false;
    */
    
    int index = row * 8 + col;
    if (blackPieces & (1ULL << index)) return true;
    return false;
}

bool Board::IsEmptySquare(int row, int col)
{
    /*
    char piece = this->board[row][col];

    if (piece == '.') return true;
    return false;
    */

    int index = row * 8 + col;
    if (!(occupiedSquares & (1ULL))) return true;
    return false;
}

bool Board::IsSquareAttacked(int row, int col, char attackerColor) 
{
    int index = row * 8 + col; //square to check
    uint64_t sq = 1ULL << index;

    //check pawns
    if (attackerColor == 'w')
    {
        if (whitePawns & (1ULL << (index - 7)) & ~FILE_A) return true;
        if (whitePawns & (1ULL << (index - 9)) & ~FILE_H) return true;
    }
    else
    {
        if (blackPawns & (1ULL << (index + 7)) & ~FILE_A) return true;
        if (blackPawns & (1ULL << (index + 9)) & ~FILE_H) return true;
    }

    //check king and knight attacks
    uint64_t currKnightAttacks = knightAttacks[row][col];
    uint64_t currKingAttacks = kingAttacks[row][col];
    if (attackerColor == 'w')
    {
        if (currKnightAttacks & whiteKnights) return true;
        if (currKingAttacks & whiteKing) return true;
    }
    else
    {
        if (currKnightAttacks & blackKnights) return true;
        if (currKingAttacks & blackKing) return true;
    }

    //check diagonal rays
    for (int i = 0; i < 4; i++)
    {
        int dirRow = bishopRays[i][0];
        int dirCol = bishopRays[i][1];

        int newRow = row + dirRow;
        int newCol = col + dirCol;
        
        while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            int newIndex = newRow * 8 + newCol;
            uint64_t newSq = 1ULL << newIndex;

            if (attackerColor == 'w')
            {
                if ((whiteBishops | whiteQueens) & newSq) return true;
            }
            else
            {
                if ((blackBishops | blackQueens) & newSq) return true;
            }

            if (occupiedSquares & newSq) break;

            newRow += dirRow;
            newCol += dirCol;
        }
    }

    //check straight rays
    for (int i = 0; i < 4; i++)
    {
        int dirRow = rookRays[i][0];
        int dirCol = rookRays[i][1];

        int newRow = row + dirRow;
        int newCol = col + dirCol;
        
        while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            int newIndex = newRow * 8 + newCol;
            uint64_t newSq = 1ULL << newIndex;

            if (attackerColor == 'w')
            {
                if ((whiteRooks | whiteQueens) & newSq) return true;
            }
            else
            {
                if ((blackRooks | blackQueens) & newSq) return true;
            }

            newRow += dirRow;
            newCol += dirCol;
        }
    }

    return false;
}

bool Board::IsKingChecked(char color)
{
    int kingRow = -1;
    int kingCol = -1;

    if (color == 'w')
    {
        kingRow = this->whiteKingRow;
        kingCol = this->whiteKingCol;
    }
    else
    {
        kingRow = this->blackKingRow;
        kingCol = this->blackKingCol;
    }
    
    char enemyColor = (color == 'w') ? 'b' : 'w';
    return IsSquareAttacked(kingRow, kingCol, enemyColor);
}

/*
void Board::SetupControlMaps()
{
    std::memset(whiteControl, 0, sizeof(whiteControl));
    std::memset(blackControl, 0, sizeof(blackControl));
    whiteControlSquares = 0;
    blackControlSquares = 0;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c ++)
        {
            if (IsEmptySquare(r, c)) continue;

            bool isWhite = IsWhitePiece(r, c);
            char piece = std::tolower(board[r][c]);

            switch (piece)
            {
                case 'p':
                {
                    int dir = isWhite ? -1 : 1;

                    int newRow = r + dir;
                    if (newRow >= 0 && newRow < 8)
                    {
                        if (c - 1 >= 0)
                        {
                            if (isWhite)
                            {
                                whiteControl[newRow][c - 1]++;
                                whiteControlSquares++;
                            }
                            else
                            {
                                blackControl[newRow][c - 1]++;
                                blackControlSquares++;
                            }
                        }

                        if (c + 1 < 8)
                        {
                            if (isWhite)
                            {
                                whiteControl[newRow][c + 1]++;
                                whiteControlSquares++;
                            }
                            else
                            {
                                blackControl[newRow][c + 1]++;
                                blackControlSquares++;
                            } 
                        }
                    }

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
                        int newRow = r + offsets[i][0];
                        int newCol = c + offsets[i][1];

                        if (newRow < 0 || newRow >= 8) continue;
                        if (newCol < 0 || newCol >= 8) continue;

                        if (isWhite)
                        {
                            whiteControl[newRow][newCol]++;
                            whiteControlSquares++;
                        }
                        else
                        {
                            blackControl[newRow][newCol]++;
                            blackControlSquares++;
                        }
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

                    if (piece == 'b')
                    {
                        start = 4;
                    }
                    else if (piece == 'r')
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
                            if (isWhite)
                            {
                                whiteControl[curRow][curCol]++;
                                whiteControlSquares++;
                            }
                            else
                            {
                                blackControl[curRow][curCol]++;
                                blackControlSquares++;
                            }

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

                        if (isWhite)
                        {
                            whiteControl[newRow][newCol]++;
                            whiteControlSquares++;
                        }
                        else
                        {
                            blackControl[newRow][newCol]++;
                            blackControlSquares++;
                        }
                    }

                    break;
                }
            }
        }
    }
}

void Board::UpdateControlMaps(int fromRow, int fromCol, int toRow, int toCol)
{
    //fromsquare is the empty square, tosquare is the square the piece moved to
    char fromPiece = this->board[fromRow][fromCol];
    bool isWhite = std::isupper(fromPiece);
    char lower = std::tolower(fromPiece);
    
    //remove old attack squares
    switch (lower)
    {
        case 'p':
        {
            if (isWhite)
            {
                if (fromRow - 1 < 0) break;
                if (fromCol - 1 >= 0)
                {
                    whiteControl[fromRow - 1][fromCol - 1]--;
                    whiteControlSquares--;
                }
                if (fromCol + 1 < 8)
                {
                    whiteControl[fromRow - 1][fromCol + 1]--;
                    whiteControlSquares--;
                }
            }
            else
            {
                if (fromRow + 1 >= 8) break;
                if (fromCol - 1 >= 0)
                {
                    blackControl[fromRow + 1][fromCol - 1]--;
                    blackControlSquares--;
                }
                if (fromCol + 1 < 8)
                {
                    blackControl[fromRow + 1][fromCol + 1]--;
                    blackControlSquares--;
                }
            }
            break;
        }
        case 'b':
        case 'r':
        case 'q':
        {
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

                int newRow = fromRow + dirRow;
                int newCol = fromCol + dirCol;

                while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
                {
                    if (isWhite)
                    {
                        whiteControl[newRow][newCol]--;
                        whiteControlSquares--;
                    }
                    else
                    {
                        blackControl[newRow][newCol]--;
                        blackControlSquares--;
                    }

                    if (!IsEmptySquare(newRow, newCol)) break;

                    newRow += dirRow;
                    newCol += dirCol;
                }
            }

            break;
        }
        case 'n':
        case 'k':
        {
            for (int i = 0; i < 8; i++)
            {
                int newRow;
                int newCol;

                if (lower == 'n')
                {
                    newRow = fromRow + knightOffsets[i][0];
                    newCol = fromCol + knightOffsets[i][1];
                }
                else
                {
                    newRow = fromRow + dir[i][0];
                    newCol = fromCol + dir[i][1];
                }

                if (newRow < 0 || newRow >= 8) continue;
                if (newCol < 0 || newCol >= 8) continue;

                if (isWhite)
                {
                    whiteControl[newRow][newCol]--;
                    whiteControlSquares--;
                }
                else
                {
                    blackControl[newRow][newCol]--;
                    blackControlSquares--;
                }
            }
            break;
        }
    }

    //discover new attack rays passing through from-square
    for (int i = 0; i < 8; i++)
    {
        int dirRow = dir[i][0];
        int dirCol = dir[i][1];

        int newRow = fromRow + dirRow;
        int newCol = fromCol + dirCol;

        bool pieceFound = false;
        int pieceRow = -1;
        int pieceCol = -1;
        
        //scan for any sliding piece in all 8 directions, and recompute their attack rays
        while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            char piece = std::tolower(this->board[newRow][newCol]);

            switch (piece)
            {
                case 'b':
                case 'r':
                case 'q':
                {
                    pieceFound = true;

                    pieceRow = newRow;
                    pieceCol = newCol;

                    if (piece == 'b' && i < 4) break; // i < 4 are rook directions
                    if (piece == 'r' && i >= 4) break; // i >= 4 are bishop directions

                    int newDirRow = -dirRow;
                    int newDirCol = -dirCol;

                    int newNewRow = pieceRow + newDirRow;
                    int newNewCol = pieceCol + newDirCol;

                    //remove old attack squares
                    while (newNewRow != fromRow || newNewCol != fromCol)
                    {
                        if (isWhite) {
                            whiteControl[newNewRow][newNewCol]--;
                            whiteControlSquares--;
                        }
                        else
                        {
                            blackControl[newNewRow][newNewCol]--;
                            blackControlSquares--;
                        }

                        newNewRow += newDirRow;
                        newNewCol += newDirCol;
                    }

                    newNewRow = pieceRow + newDirRow;
                    newNewCol = pieceCol + newDirCol;

                    //compute new discovered attack ray
                    while (newNewRow >= 0 && newNewRow < 8 && newNewCol >= 0 && newNewCol < 8)
                    {
                        if (isWhite)
                        {
                            whiteControl[newNewRow][newNewCol]++;
                            whiteControlSquares++;
                        }
                        else
                        {
                            blackControl[newNewRow][newNewCol]++;
                            blackControlSquares++;
                        }

                        if (this->board[newNewRow][newNewCol] != '.') break;

                        newNewRow += newDirRow;
                        newNewCol += newDirCol;
                    }

                    break;
                }
            }

            if (pieceFound) break;

            newRow += dirRow;
            newCol += dirCol;
        }
    }

    //block attack rays on to-square
    for (int i = 0; i < 8; i++)
    {
        int dirRow = dir[i][0];
        int dirCol = dir[i][1];

        int newRow = fromRow + dirRow;
        int newCol = fromCol + dirCol;

        bool pieceFound = false;
        int pieceRow = -1;
        int pieceCol = -1;

        while (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            char piece = std::tolower(this->board[newRow][newCol]);

            switch (piece)
            {
                case 'b':
                case 'r':
                case 'q':
                {
                    pieceFound = true;

                    pieceRow = newRow;
                    pieceCol = newCol;

                    if (piece == 'b' && i < 4) break; // i < 4 are rook directions
                    if (piece == 'r' && i >= 4) break; // i >= 4 are bishop directions

                    int newDirRow = -dirRow;
                    int newDirCol = -dirCol;

                    int newNewRow = pieceRow + newDirRow;
                    int newNewCol = pieceCol + newDirCol;

                    while (newNewRow >= 0 && newNewRow < 8 && newNewCol >= 0 && newNewCol < 8)
                    {
                        if (isWhite) {
                            whiteControl[newNewRow][newNewCol]--;
                            whiteControlSquares--;
                        }
                        else
                        {
                            blackControl[newNewRow][newNewCol]--;
                            blackControlSquares--;
                        }

                        newNewRow += newDirRow;
                        newNewCol += newDirCol;
                    }

                    newNewRow = pieceRow + newDirRow;
                    newNewCol = pieceCol + newDirCol;

                    while (newNewRow != fromRow || newNewCol != fromCol)
                    {
                        if (isWhite)
                        {
                            whiteControl[newNewRow][newNewCol]++;
                            whiteControlSquares++;
                        }
                        else
                        {
                            blackControl[newNewRow][newNewCol]++;
                            blackControlSquares++;
                        }

                        if (this->board[newNewRow][newNewCol] != '.') break;

                        newNewRow += newDirRow;
                        newNewCol += newDirCol;
                    }

                    break;
                }
            }

            if (pieceFound) break;

            newRow += dirRow;
            newCol += dirCol;
        }
    }
}
    */

void Board::GetPawnMoves(int row, int col, std::vector<Move>& moves)
{
    int currIndex = row * 8 + col;
    uint64_t sq = 1ULL << currIndex;
    int epIndex = enPassantRow * 8 + enPassantCol;

    if (whitePieces & sq)
    {
        uint64_t singlePushes = (whitePawns << 8) & ~occupiedSquares;
        while (singlePushes)
        {
            int toIndex = PopLSB(singlePushes);
            int fromIndex = toIndex - 8;
            if (toIndex / 8 == 7)
            {
                moves.push_back({fromIndex, toIndex, PROMOTION, KNIGHT});
                moves.push_back({fromIndex, toIndex, PROMOTION, BISHOP});
                moves.push_back({fromIndex, toIndex, PROMOTION, ROOK});
                moves.push_back({fromIndex, toIndex, PROMOTION, QUEEN});
            }
            else
            {
                moves.push_back({fromIndex, toIndex, NORMAL});

                if (fromIndex / 8 == 1 && !(occupiedSquares & (1ULL << (toIndex + 8))))
                {
                    moves.push_back({fromIndex, toIndex + 8, PAWNDOUBLE});
                }
            }
        }

        uint64_t attacks = whitePawnAttacks[row][col] & blackPieces;
        while (attacks)
        {
            int toIndex = PopLSB(attacks);

            if (toIndex / 8 == 7)
            {
                moves.push_back({currIndex, toIndex, PROMOTION, KNIGHT});
                moves.push_back({currIndex, toIndex, PROMOTION, BISHOP});
                moves.push_back({currIndex, toIndex, PROMOTION, ROOK});
                moves.push_back({currIndex, toIndex, PROMOTION, QUEEN});
            }
            else
            {
                if (epIndex == toIndex)
                {
                    moves.push_back({currIndex, toIndex, ENPASSANT});
                }
                else
                {
                    moves.push_back({currIndex, toIndex, CAPTURE});
                }
            }
        }
    }
    else //black
    {
        uint64_t singlePushes = (blackPawns >> 8) & ~occupiedSquares;
        while (singlePushes)
        {
            int toIndex = PopLSB(singlePushes);
            int fromIndex = toIndex + 8;
            if (toIndex / 8 == 0)
            {
                moves.push_back({fromIndex, toIndex, PROMOTION, KNIGHT});
                moves.push_back({fromIndex, toIndex, PROMOTION, BISHOP});
                moves.push_back({fromIndex, toIndex, PROMOTION, ROOK});
                moves.push_back({fromIndex, toIndex, PROMOTION, QUEEN});
            }
            else
            {
                moves.push_back({fromIndex, toIndex, NORMAL});

                if (fromIndex / 8 == 6 && !(occupiedSquares & (1ULL << (toIndex - 8))))
                {
                    moves.push_back({fromIndex, toIndex - 8, PAWNDOUBLE});
                }
            }
        }

        uint64_t attacks = blackPawnAttacks[row][col] & whitePieces;
        while (attacks)
        {
            int toIndex = PopLSB(attacks);

            if (toIndex / 8 == 0)
            {
                moves.push_back({currIndex, toIndex, PROMOTION, KNIGHT});
                moves.push_back({currIndex, toIndex, PROMOTION, BISHOP});
                moves.push_back({currIndex, toIndex, PROMOTION, ROOK});
                moves.push_back({currIndex, toIndex, PROMOTION, QUEEN});
            }
            else
            {
                if (epIndex == toIndex)
                {
                    moves.push_back({currIndex, toIndex, ENPASSANT});
                }
                else
                {
                    moves.push_back({currIndex, toIndex, CAPTURE});
                }
            }
        }
    }
}

void Board::GetKnightMoves(int row, int col, std::vector<Move>& moves)
{
    uint64_t attacks = knightAttacks[row][col];
    int currIndex = row * 8 + col;
    uint64_t sq = 1ULL << currIndex;

    if (whitePieces & sq)
    {
        while (attacks)
        {
            int toIndex = PopLSB(attacks);
            uint64_t toSq = 1ULL << toIndex;
            
            if (!(whitePieces & toSq))
            {
                if (blackPieces & toSq)
                {
                    moves.push_back({currIndex, toIndex, CAPTURE});
                }
                else
                {
                    moves.push_back({currIndex, toIndex, NORMAL});
                }
            }
        }
    }
    else
    {
        while (attacks)
        {
            int toIndex = PopLSB(attacks);
            uint64_t toSq = 1ULL << toIndex;
            
            if (!(blackPieces & toSq))
            {
                if (whitePieces & toSq)
                {
                    moves.push_back({currIndex, toIndex, CAPTURE});
                }
                else
                {
                    moves.push_back({currIndex, toIndex, NORMAL});
                }
            }
        }
    }
}

void Board::GetBishopMoves(int row, int col, std::vector<Move>& moves)
{
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
            if (IsWhitePiece(row, col))
            {
                //stop if target square is a friendly piece
                if (IsWhitePiece(newRow, newCol)) break;
                //stop if target square is an enemy piece
                else if (IsBlackPiece(newRow, newCol))
                {
                    moves.push_back({row, col, newRow, newCol, this->board[row][col], CAPTURE});
                    break;
                }
                else
                {
                    moves.push_back({row, col, newRow, newCol, this->board[row][col], NORMAL});
                }
            }
            if (IsBlackPiece(row, col))
            {
                if (IsBlackPiece(newRow, newCol)) break;
                else if (IsWhitePiece(newRow, newCol))
                {
                    moves.push_back({row, col, newRow, newCol, this->board[row][col], CAPTURE});
                    break;
                }
                else
                {
                    moves.push_back({row, col, newRow, newCol, this->board[row][col], NORMAL});
                }
            }

            newRow += dirRow;
            newCol += dirCol;
        }
    }
}

void Board::GetRookMoves(int row, int col, std::vector<Move>& moves)
{
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
            if (IsWhitePiece(row, col))
            {
                //stop if target square is a friendly piece
                if (IsWhitePiece(newRow, newCol)) break;
                //stop if target square is an enemy piece
                else if (IsBlackPiece(newRow, newCol))
                {
                    moves.push_back({row, col, newRow, newCol, this->board[row][col], CAPTURE});
                    break;
                }
                else
                {
                    moves.push_back({row, col, newRow, newCol, this->board[row][col], NORMAL});
                }
            }
            if (IsBlackPiece(row, col))
            {
                if (IsBlackPiece(newRow, newCol)) break;
                else if (IsWhitePiece(newRow, newCol))
                {
                    moves.push_back({row, col, newRow, newCol, this->board[row][col], CAPTURE});
                    break;
                }
                else
                {
                    moves.push_back({row, col, newRow, newCol, this->board[row][col], NORMAL});
                }
            }

            newRow += dirRow;
            newCol += dirCol;
        }
    }
}

void Board::GetQueenMoves(int row, int col, std::vector<Move>& moves)
{
    this->GetBishopMoves(row, col, moves);
    this->GetRookMoves(row, col, moves);
}

void Board::GetKingMoves(int row, int col, std::vector<Move>& moves)
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
        int newRow = row + dir[i][0];
        int newCol = col + dir[i][1];

        if (newRow < 0 || newRow >= 8) continue;
        if (newCol < 0 || newCol >= 8) continue;

        if (IsWhitePiece(row, col))
        {
            if (IsBlackPiece(newRow, newCol)) moves.push_back({row, col, newRow, newCol, this->board[row][col], CAPTURE});
            else if (IsEmptySquare(newRow, newCol)) moves.push_back({row, col, newRow, newCol, this->board[row][col], NORMAL});
        }
        else
        {
            if (IsWhitePiece(newRow, newCol)) moves.push_back({row, col, newRow, newCol, this->board[row][col], CAPTURE});
            else if (IsEmptySquare(newRow, newCol)) moves.push_back({row, col, newRow, newCol, this->board[row][col], NORMAL});
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
}

void Board::GetLegalMoves(std::vector<Move>& moves)
{
    moves.clear();

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
                    GetPawnMoves(r, c, moves);
                    break;
                }
                case 'N':
                case 'n':
                {  
                    GetKnightMoves(r, c, moves);
                    break;
                }
                case 'B':
                case 'b':
                {
                    GetBishopMoves(r, c, moves);
                    break;
                }
                case 'R':
                case 'r':
                {
                    GetRookMoves(r, c, moves);
                    break;
                }
                case 'Q':
                case 'q':
                {
                    GetQueenMoves(r, c, moves);
                    break;
                }
                case 'K':
                case 'k':
                {
                    GetKingMoves(r, c, moves);
                    break;
                }
            }
        }
    }
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
    //std::cout << "Start Make Hash: " << this->hash << std::endl;
    UndoMove undo;

    char fromPiece = this->board[move.fromRow][move.fromCol];
    char toPiece = this->board[move.toRow][move.toCol];

    int fromIndex = PieceIndex(fromPiece);
    int toIndex = PieceIndex(toPiece);

    undo.pieceMoved = fromPiece;
    undo.pieceCaptured = toPiece;
    undo.lastColor = this->color;

    undo.enPassantRow = this->enPassantRow;
    undo.enPassantCol = this->enPassantCol;

    undo.whiteKingSide = whiteKingSide;
    undo.whiteQueenSide = whiteQueenSide;
    undo.blackKingSide = blackKingSide;
    undo.blackQueenSide = blackQueenSide;

    //remove capturing piece from from-square
    this->hash ^= Engine::pieceSquareVals[move.fromRow][move.fromCol][fromIndex];
    //remove captured piece from to-square
    if (toPiece != '.')
    {
        this->hash ^= Engine::pieceSquareVals[move.toRow][move.toCol][toIndex];
    }
    //add capturing piece to to-square
    this->hash ^= Engine::pieceSquareVals[move.toRow][move.toCol][fromIndex];
    this->hash ^= Engine::sideKey;

    if (this->enPassantCol != -1)
    {
        this->hash ^= Engine::epKey[this->enPassantCol];
    }

    int oldCastleIndex =
    (whiteKingSide  ? 1 : 0) |
    (whiteQueenSide ? 2 : 0) |
    (blackKingSide  ? 4 : 0) |
    (blackQueenSide ? 8 : 0);
    this->hash ^= Engine::castlingKey[oldCastleIndex];

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

    //apply normal moves
    if (move.moveType != CASTLEKING && move.moveType != CASTLEQUEEN)
    {
        this->board[move.fromRow][move.fromCol] = '.';
        this->board[move.toRow][move.toCol] = fromPiece;

        if (fromPiece == 'K')
        {
            this->whiteKingRow = move.toRow;
            this->whiteKingCol = move.toCol;
        }
        else if (fromPiece == 'k')
        {
            this->blackKingRow = move.toRow;
            this->blackKingCol = move.toCol;
        }

        this->enPassantRow = -1;
        this->enPassantCol = -1;

        if (move.moveType == ENPASSANT)
        {
            if (fromPiece == 'P')
            {
                int pawnIndex = PieceIndex('p');
                this->board[move.toRow + 1][move.toCol] = '.';
                hash ^= Engine::pieceSquareVals[move.toRow + 1][move.toCol][pawnIndex];
            }
            else if (fromPiece == 'p')
            {
                int pawnIndex = PieceIndex('P');
                this->board[move.toRow - 1][move.toCol] = '.';
                hash ^= Engine::pieceSquareVals[move.toRow - 1][move.toCol][pawnIndex];
            }
        }
        else if (move.moveType == PAWNDOUBLE)
        {
            this->enPassantRow = -1;
            this->enPassantCol = -1;

            if (fromPiece == 'P')
            {
                this->enPassantRow = move.toRow + 1;
                this->enPassantCol = move.toCol;
            }
            else if (fromPiece == 'p')
            {
                this->enPassantRow = move.toRow - 1;
                this->enPassantCol = move.toCol;
            }
        }
        else if (move.moveType == PROMOTION)
        {
            switch (move.promoteType)
            {
                case KNIGHT:
                {
                    if (fromPiece == 'P')
                    {
                        this->board[move.toRow][move.toCol] = 'N';
                    }
                    else
                    {
                        this->board[move.toRow][move.toCol] = 'n';
                    }
                    break;
                }
                case BISHOP:
                {
                    if (fromPiece == 'P')
                    {
                        this->board[move.toRow][move.toCol] = 'B';
                    }
                    else
                    {
                        this->board[move.toRow][move.toCol] = 'b';
                    }
                    break;
                }
                case ROOK:
                {
                    if (fromPiece == 'P')
                    {
                        this->board[move.toRow][move.toCol] = 'R';
                    }
                    else
                    {
                        this->board[move.toRow][move.toCol] = 'r';
                    }
                    break;
                }
                case QUEEN:
                {
                    if (fromPiece == 'P')
                    {
                        this->board[move.toRow][move.toCol] = 'Q';
                    }
                    else
                    {
                        this->board[move.toRow][move.toCol] = 'q';
                    }
                    break;
                }
            }
        }
    }
    //apply castling moves
    if (move.moveType == CASTLEKING)
    {   
        this->enPassantRow = -1;
        this->enPassantCol = -1;

        if (fromPiece == 'K')
        {
            board[7][4] = '.';
            board[7][6] = 'K';

            board[7][7] = '.';
            board[7][5] = 'R';

            int rookIndex = PieceIndex('R');
            this->hash ^= Engine::pieceSquareVals[7][7][rookIndex];
            this->hash ^= Engine::pieceSquareVals[7][5][rookIndex];

            this->whiteKingRow = 7;
            this->whiteKingCol = 6;
        }
        else
        {
            board[0][4] = '.';
            board[0][6] = 'k';

            board[0][7] = '.';
            board[0][5] = 'r';

            int rookIndex = PieceIndex('r');
            this->hash ^= Engine::pieceSquareVals[0][7][rookIndex];
            this->hash ^= Engine::pieceSquareVals[0][5][rookIndex];

            this->blackKingRow = 0;
            this->blackKingCol = 6;
        }
    }
    else if (move.moveType == CASTLEQUEEN)
    {
        this->enPassantRow = -1;
        this->enPassantCol = -1;

        if (fromPiece == 'K')
        {
            board[7][4] = '.';
            board[7][2] = 'K';

            board[7][0] = '.';
            board[7][3] = 'R';

            int rookIndex = PieceIndex('R');
            this->hash ^= Engine::pieceSquareVals[7][0][rookIndex];
            this->hash ^= Engine::pieceSquareVals[7][3][rookIndex];

            this->whiteKingRow = 7;
            this->whiteKingCol = 2;
        }
        else
        {
            board[0][4] = '.';
            board[0][2] = 'k';

            board[0][0] = '.';
            board[0][3] = 'r';

            int rookIndex = PieceIndex('r');
            this->hash ^= Engine::pieceSquareVals[0][0][rookIndex];
            this->hash ^= Engine::pieceSquareVals[0][3][rookIndex];

            this->blackKingRow = 0;
            this->blackKingCol = 2;
        }
    }

    int newCastleIndex =
    (whiteKingSide  ? 1 : 0) |
    (whiteQueenSide ? 2 : 0) |
    (blackKingSide  ? 4 : 0) |
    (blackQueenSide ? 8 : 0);
    this->hash ^= Engine::castlingKey[newCastleIndex];

    if (this->enPassantCol != -1)
    {
        this->hash ^= Engine::epKey[this->enPassantCol];
    }

    SwitchColors();

    //std::cout << "End Make Hash: " << this->hash << std::endl;

    return undo;
}

void Board::UnmakeMove(const Move& move, const UndoMove& undo)
{
    //std::cout << "Start Unmake Hash: " << this->hash << std::endl;
    char fromPiece = undo.pieceMoved;
    char toPiece = undo.pieceCaptured;

    int fromIndex = PieceIndex(fromPiece);
    int toIndex = PieceIndex(toPiece);

    int newCastleIndex =
    (whiteKingSide  ? 1 : 0) |
    (whiteQueenSide ? 2 : 0) |
    (blackKingSide  ? 4 : 0) |
    (blackQueenSide ? 8 : 0);
    this->hash ^= Engine::castlingKey[newCastleIndex];

    if (this->enPassantCol != -1)
    {
        this->hash ^= Engine::epKey[this->enPassantCol];
    }

    this->color = undo.lastColor;

    this->whiteKingSide = undo.whiteKingSide;
    this->whiteQueenSide = undo.whiteQueenSide;
    this->blackKingSide = undo.blackKingSide;
    this->blackQueenSide = undo.blackQueenSide;

    this->enPassantRow = undo.enPassantRow;
    this->enPassantCol = undo.enPassantCol;

    this->hash ^= Engine::pieceSquareVals[move.fromRow][move.fromCol][fromIndex];
    this->hash ^= Engine::pieceSquareVals[move.toRow][move.toCol][fromIndex];
    if (toPiece != '.')
    {
        this->hash ^= Engine::pieceSquareVals[move.toRow][move.toCol][toIndex];
    }
    this->hash ^= Engine::sideKey;

    if (this->enPassantCol != -1)
    {
        this->hash ^= Engine::epKey[this->enPassantCol];
    }

    int oldCastleIndex =
    (undo.whiteKingSide  ? 1 : 0) |
    (undo.whiteQueenSide ? 2 : 0) |
    (undo.blackKingSide  ? 4 : 0) |
    (undo.blackQueenSide ? 8 : 0);
    this->hash ^= Engine::castlingKey[oldCastleIndex];

    if (move.moveType == CASTLEKING)
    {
        if (fromPiece == 'K')
        {
            board[7][6] = '.';
            board[7][4] = 'K';

            board[7][5] = '.';
            board[7][7] = 'R';

            this->whiteKingRow = 7;
            this->whiteKingCol = 4;
        }
        else
        {
            board[0][6] = '.';
            board[0][4] = 'k';

            board[0][5] = '.';
            board[0][7] = 'r';

            this->blackKingRow = 0;
            this->blackKingCol = 4;
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

            this->whiteKingRow = 7;
            this->whiteKingCol = 4;
        }
        else
        {
            board[0][2] = '.';
            board[0][4] = 'k';

            board[0][3] = '.';
            board[0][0] = 'r';
            
            this->blackKingRow = 0;
            this->blackKingCol = 4;
        }
    }
    else
    {
        this->board[move.toRow][move.toCol] = toPiece;
        this->board[move.fromRow][move.fromCol] = fromPiece;

        if (fromPiece == 'K')
        {
            this->whiteKingRow = move.fromRow;
            this->whiteKingCol = move.fromCol;
        }
        else if (fromPiece == 'k')
        {
            this->blackKingRow = move.fromRow;
            this->blackKingCol = move.fromCol;
        }

        if (move.moveType == ENPASSANT)
        {
            if (fromPiece == 'P')
            {
                this->board[move.toRow + 1][move.toCol] = 'p';
            }
            else if (fromPiece == 'p')
            {
                this->board[move.toRow - 1][move.toCol] = 'P';
            }
        }
    }

    //std::cout << "End Unmake Hash: " << this->hash << std::endl;
}