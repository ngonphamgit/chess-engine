#include "../include/board.hpp"
#include "../include/engine.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <cassert>

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

    this->whitePieces  = 0x000000000000FFFFULL;
    this->whitePawns   = 0x000000000000FF00ULL;
    this->whiteRooks   = 0x0000000000000081ULL;
    this->whiteKnights = 0x0000000000000042ULL;
    this->whiteBishops = 0x0000000000000024ULL;
    this->whiteQueens  = 0x0000000000000008ULL;
    this->whiteKing    = 0x0000000000000010ULL;

    this->blackPieces  = 0xFFFF000000000000ULL;
    this->blackPawns   = 0x00FF000000000000ULL;
    this->blackRooks   = 0x8100000000000000ULL;
    this->blackKnights = 0x4200000000000000ULL;
    this->blackBishops = 0x2400000000000000ULL;
    this->blackQueens  = 0x0800000000000000ULL;
    this->blackKing    = 0x1000000000000000ULL;

    this->occupiedSquares = whitePieces | blackPieces;

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

    this->color = 'w';
    this->epIndex = -1;
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

std::string Board::IndexToSquare(int index) const
{
    std::string sq;
    sq.resize(2);

    int rank = index / 8;
    int file = index % 8;

    char rankStr = '1' + rank;
    char fileStr = 'a' + file;

    sq[0] = fileStr;
    sq[1] = rankStr;

    return sq;
}

int Board::PieceZobristIndex(char piece)
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

char Board::GetPieceAtIndex(int index)
{
    uint64_t sq = 1ULL << index;

    if (whitePawns   & sq) return 'P';
    if (whiteKnights & sq) return 'N';
    if (whiteBishops & sq) return 'B';
    if (whiteRooks   & sq) return 'R';
    if (whiteQueens  & sq) return 'Q';
    if (whiteKing    & sq) return 'K';

    if (blackPawns   & sq) return 'p';
    if (blackKnights & sq) return 'n';
    if (blackBishops & sq) return 'b';
    if (blackRooks   & sq) return 'r';
    if (blackQueens  & sq) return 'q';
    if (blackKing    & sq) return 'k';

    return '.';
}

std::string Board::SquareToString(int row, int col)
{
    char file = 'a' + col;
    char rank = '1' + row;

    return std::string() + file + rank;
}

Move Board::ParseMove(std::string input)
{
    Move move;
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;

    fromRow = input[1] - '1';
    fromCol = input[0] - 'a';

    if (input == "O-O")
    {
        if (this->color == 'w')
        {
            return {4, 6, CASTLEKING};
        }
        else
        {
            return {60, 62, CASTLEKING};
        }
        
    }
    else if (input == "O-O-O")
    {
        if (this->color == 'w')
        {
            return {4, 2, CASTLEQUEEN};
        }
        else
        {
            return {60, 58, CASTLEQUEEN};
        }
    }

    if (input[2] == 'x')
    {
        toRow =  input[4] - '1';
        toCol = input[3] - 'a';
        move.moveType = CAPTURE;
    }
    else
    {
        toRow = input[3] - '1';
        toCol = input[2] - 'a';
        move.moveType = NORMAL;
    }

    move.fromIndex = fromRow * 8 + fromCol;
    move.toIndex = toRow * 8 + toCol;

    std::cout << move.fromIndex << " " << move.toIndex << std::endl;
    //std::cout << fromRow << " " << fromCol << " " << toRow << " " << toCol << " " << move.pieceMoved << std::endl;

    return move;
}

void Board::RemovePieceAtIndex(char piece, int index)
{
    uint64_t sq = 1ULL << index;

    *pieceBoards[piece] &= ~sq;

    if (std::isupper(piece)) whitePieces &= ~sq;
    else blackPieces &= ~sq;
}

void Board::AddPieceAtIndex(char piece, int index)
{
    uint64_t sq = 1ULL << index;

    *pieceBoards[piece] |= sq;

    if (std::isupper(piece)) whitePieces |= sq;
    else blackPieces |= sq;
}

int Board::PopLSB(uint64_t& bb)
{
    int index = __builtin_ctzll(bb);
    bb &= bb - 1;
    return index;
}

bool Board::IsSquareAttacked(int index, char attackerColor) 
{
    int row = index / 8;
    int col = index % 8;
    uint64_t sq = 1ULL << index;

    //check pawns
    if (attackerColor == 'w')
    {
        if (whitePawns & blackPawnAttacks[row][col]) return true;
    }
    else
    {
        if (blackPawns & whitePawnAttacks[row][col]) return true;
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

            if (occupiedSquares & newSq) break;

            newRow += dirRow;
            newCol += dirCol;
        }
    }

    return false;
}

bool Board::IsKingChecked(char kingColor)
{
    int kingIndex = -1;
    uint64_t temp;
    if (kingColor == 'w') temp = whiteKing;
    else temp = blackKing;

    kingIndex = PopLSB(temp);
    
    char enemyColor = (kingColor == 'w') ? 'b' : 'w';
    return IsSquareAttacked(kingIndex, enemyColor);
}

void Board::GetPawnMoves(std::vector<Move>& moves)
{
    if (color == 'w')
    {
        uint64_t epSquare = (epIndex != -1) ? (1ULL << epIndex) : 0ULL;

        uint64_t singlePushes = (whitePawns << 8) & ~occupiedSquares;
        uint64_t doublePushes = (singlePushes << 8) & ~occupiedSquares & RANK_4;
        uint64_t leftCaptures = ((whitePawns & ~FILE_A) << 7) & (blackPieces | epSquare);
        uint64_t rightCaptures = ((whitePawns & ~FILE_H) << 9) & (blackPieces | epSquare);

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
            }
        }

        while (doublePushes)
        {
            int toIndex = PopLSB(doublePushes);
            int fromIndex = toIndex - 16;

            moves.push_back({fromIndex, toIndex, PAWNDOUBLE});
        }

        while (leftCaptures)
        {
            int toIndex = PopLSB(leftCaptures);
            int fromIndex = toIndex - 7;

            if (toIndex / 8 == 7)
            {
                moves.push_back({fromIndex, toIndex, PROMOTION, KNIGHT});
                moves.push_back({fromIndex, toIndex, PROMOTION, BISHOP});
                moves.push_back({fromIndex, toIndex, PROMOTION, ROOK});
                moves.push_back({fromIndex, toIndex, PROMOTION, QUEEN});
            }
            else
            {
                if (toIndex == epIndex)
                {
                    moves.push_back({fromIndex, toIndex, ENPASSANT});

                    assert(GetPieceAtIndex(toIndex - 8) == 'p');
                }
                else
                {
                    moves.push_back({fromIndex, toIndex, CAPTURE});
                }
            }
        }

        while (rightCaptures)
        {
            int toIndex = PopLSB(rightCaptures);
            int fromIndex = toIndex - 9;

            if (toIndex / 8 == 7)
            {
                moves.push_back({fromIndex, toIndex, PROMOTION, KNIGHT});
                moves.push_back({fromIndex, toIndex, PROMOTION, BISHOP});
                moves.push_back({fromIndex, toIndex, PROMOTION, ROOK});
                moves.push_back({fromIndex, toIndex, PROMOTION, QUEEN});
            }
            else
            {
                if (toIndex == epIndex)
                {
                    moves.push_back({fromIndex, toIndex, ENPASSANT});

                    assert(GetPieceAtIndex(toIndex - 8) == 'p');
                }
                else
                {
                    moves.push_back({fromIndex, toIndex, CAPTURE});
                }
            }
        }
    }
    else
    {
        uint64_t epSquare = (epIndex != -1) ? (1ULL << epIndex) : 0ULL;

        uint64_t singlePushes = (blackPawns >> 8) & ~occupiedSquares;
        uint64_t doublePushes = (singlePushes >> 8) & ~occupiedSquares & RANK_5;
        uint64_t leftCaptures = ((blackPawns & ~FILE_A) >> 9) & (whitePieces | epSquare);
        uint64_t rightCaptures = ((blackPawns & ~FILE_H) >> 7) & (whitePieces | epSquare);

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
            }
        }

        while (doublePushes)
        {
            int toIndex = PopLSB(doublePushes);
            int fromIndex = toIndex + 16;

            moves.push_back({fromIndex, toIndex, PAWNDOUBLE});
        }

        while (leftCaptures)
        {
            int toIndex = PopLSB(leftCaptures);
            int fromIndex = toIndex + 9;

            if (toIndex / 8 == 0)
            {
                moves.push_back({fromIndex, toIndex, PROMOTION, KNIGHT});
                moves.push_back({fromIndex, toIndex, PROMOTION, BISHOP});
                moves.push_back({fromIndex, toIndex, PROMOTION, ROOK});
                moves.push_back({fromIndex, toIndex, PROMOTION, QUEEN});
            }
            else
            {
                if (toIndex == epIndex)
                {
                    moves.push_back({fromIndex, toIndex, ENPASSANT});

                    assert(GetPieceAtIndex(toIndex + 8) == 'P');
                }
                else
                {
                    moves.push_back({fromIndex, toIndex, CAPTURE});
                }
            }
        }

        while (rightCaptures)
        {
            int toIndex = PopLSB(rightCaptures);
            int fromIndex = toIndex + 7;

            if (toIndex / 8 == 0)
            {
                moves.push_back({fromIndex, toIndex, PROMOTION, KNIGHT});
                moves.push_back({fromIndex, toIndex, PROMOTION, BISHOP});
                moves.push_back({fromIndex, toIndex, PROMOTION, ROOK});
                moves.push_back({fromIndex, toIndex, PROMOTION, QUEEN});
            }
            else
            {
                if (toIndex == epIndex)
                {
                    moves.push_back({fromIndex, toIndex, ENPASSANT});

                    assert(GetPieceAtIndex(toIndex + 8) == 'P');
                }
                else
                {
                    moves.push_back({fromIndex, toIndex, CAPTURE});
                }
            }
        }
    }
}

void Board::GetKnightMoves(std::vector<Move>& moves)
{
    uint64_t knights = (this->color == 'w') ? whiteKnights : blackKnights;
    uint64_t friendlyPieces = (this->color == 'w') ? whitePieces : blackPieces;
    uint64_t enemyPieces = (this->color == 'w') ? blackPieces : whitePieces;

    while (knights)
    {
        int fromIndex = PopLSB(knights);
        int row = fromIndex / 8;
        int col = fromIndex % 8;
        uint64_t attacks = knightAttacks[row][col] & ~friendlyPieces;

        while (attacks)
        {
            int toIndex = PopLSB(attacks);
            uint64_t toSq = 1ULL << toIndex;

            if (enemyPieces & toSq) moves.push_back({fromIndex, toIndex, CAPTURE});
            else moves.push_back({fromIndex, toIndex, NORMAL});
        }
    }
}

void Board::GetBishopMoves(std::vector<Move>& moves)
{
    uint64_t bishops = (this->color == 'w') ? whiteBishops : blackBishops;
    uint64_t friendlyPieces = (this->color == 'w') ? whitePieces : blackPieces;
    uint64_t enemyPieces = (this->color == 'w') ? blackPieces : whitePieces;

    while (bishops)
    {
        int fromIndex = PopLSB(bishops);
        int row = fromIndex / 8;
        int col = fromIndex % 8;

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

                if (friendlyPieces & newSq) break;

                if (enemyPieces & newSq)
                {
                    moves.push_back({fromIndex, newIndex, CAPTURE});
                    break;
                }
                else
                {
                    moves.push_back({fromIndex, newIndex, NORMAL});
                } 

                newRow += dirRow;
                newCol += dirCol;
            }
        }
    }
}

void Board::GetRookMoves(std::vector<Move>& moves)
{
    uint64_t rooks = (this->color == 'w') ? whiteRooks : blackRooks;
    uint64_t friendlyPieces = (this->color == 'w') ? whitePieces : blackPieces;
    uint64_t enemyPieces = (this->color == 'w') ? blackPieces : whitePieces;

    while (rooks)
    {
        int fromIndex = PopLSB(rooks);
        int row = fromIndex / 8;
        int col = fromIndex % 8;

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

                if (friendlyPieces & newSq) break;

                if (enemyPieces & newSq)
                {
                    moves.push_back({fromIndex, newIndex, CAPTURE});
                    break;
                }
                else
                {
                    moves.push_back({fromIndex, newIndex, NORMAL});
                } 
                
                newRow += dirRow;
                newCol += dirCol;
            }
        }
    }
}

void Board::GetQueenMoves(std::vector<Move>& moves)
{
    uint64_t queens = (this->color == 'w') ? whiteQueens : blackQueens;
    uint64_t friendlyPieces = (this->color == 'w') ? whitePieces : blackPieces;
    uint64_t enemyPieces = (this->color == 'w') ? blackPieces : whitePieces;

    while (queens)
    {
        int fromIndex = PopLSB(queens);
        int row = fromIndex / 8;
        int col = fromIndex % 8;

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

                if (friendlyPieces & newSq) break;

                if (enemyPieces & newSq)
                {
                    moves.push_back({fromIndex, newIndex, CAPTURE});
                    break;
                }
                else
                {
                    moves.push_back({fromIndex, newIndex, NORMAL});
                } 

                newRow += dirRow;
                newCol += dirCol;
            }
        }

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

                if (friendlyPieces & newSq) break;

                if (enemyPieces & newSq)
                {
                    moves.push_back({fromIndex, newIndex, CAPTURE});
                    break;
                }
                else
                {
                    moves.push_back({fromIndex, newIndex, NORMAL});
                } 
                
                newRow += dirRow;
                newCol += dirCol;
            }
        }
    }
}

void Board::GetKingMoves(std::vector<Move>& moves)
{
    uint64_t king = (this->color == 'w') ? whiteKing : blackKing;
    uint64_t friendlyPieces = (this->color == 'w') ? whitePieces : blackPieces;
    uint64_t enemyPieces = (this->color == 'w') ? blackPieces : whitePieces;

    int fromIndex = PopLSB(king);
    int row = fromIndex / 8;
    int col = fromIndex % 8;
    
    uint64_t attacks = kingAttacks[row][col] & ~friendlyPieces;

    while (attacks)
    {
        int toIndex = PopLSB(attacks);
        uint64_t newSq = 1ULL << toIndex;

        if (friendlyPieces & newSq) continue;
        if (enemyPieces & newSq) moves.push_back({fromIndex, toIndex, CAPTURE});
        else moves.push_back({fromIndex, toIndex, NORMAL});
    }

    //castling
    if (this->color == 'w')
    {
        if (this->whiteKingSide 
            && !(occupiedSquares & (1ULL << (fromIndex + 1))) 
            && !(occupiedSquares & (1ULL << (fromIndex + 2)))
            && !IsKingChecked('w')
            && !IsSquareAttacked(fromIndex + 1, 'b')
            && !IsSquareAttacked(fromIndex + 2, 'b')
            && (whiteRooks & (1ULL << 7)))
        {
            moves.push_back({fromIndex, fromIndex + 2, CASTLEKING});
        }
        if (this->whiteQueenSide 
            && !(occupiedSquares & (1ULL << (fromIndex - 1))) 
            && !(occupiedSquares & (1ULL << (fromIndex - 2)))
            && !(occupiedSquares & (1ULL << (fromIndex - 3)))
            && !IsKingChecked('w')
            && !IsSquareAttacked(fromIndex - 1, 'b')
            && !IsSquareAttacked(fromIndex - 2, 'b')
            && (whiteRooks & (1ULL << 0)))
        {
            moves.push_back({fromIndex, fromIndex - 2, CASTLEQUEEN});
        }
    }
    else
    {
        if (this->blackKingSide 
            && !(occupiedSquares & (1ULL << (fromIndex + 1))) 
            && !(occupiedSquares & (1ULL << (fromIndex + 2)))
            && !IsKingChecked('b')
            && !IsSquareAttacked(fromIndex + 1, 'w')
            && !IsSquareAttacked(fromIndex + 2, 'w')
            && (blackRooks & (1ULL << 63)))
        {
            moves.push_back({fromIndex, fromIndex + 2, CASTLEKING});
        }
        if (this->blackQueenSide 
            && !(occupiedSquares & (1ULL << (fromIndex - 1))) 
            && !(occupiedSquares & (1ULL << (fromIndex - 2)))
            && !(occupiedSquares & (1ULL << (fromIndex - 3)))
            && !IsKingChecked('b')
            && !IsSquareAttacked(fromIndex - 1, 'w')
            && !IsSquareAttacked(fromIndex - 2, 'w')
            && (blackRooks & (1ULL << 56)))
        {
            moves.push_back({fromIndex, fromIndex - 2, CASTLEQUEEN});
        }
    }
}

void Board::GetLegalMoves(std::vector<Move>& moves)
{
    moves.clear();

    uint64_t pawns   = (this->color == 'w') ? whitePawns : blackPawns;
    uint64_t knights = (this->color == 'w') ? whiteKnights : blackKnights;
    uint64_t bishops = (this->color == 'w') ? whiteBishops : blackBishops;
    uint64_t rooks   = (this->color == 'w') ? whiteRooks : blackRooks;
    uint64_t queens  = (this->color == 'w') ? whiteQueens : blackQueens;
    uint64_t king    = (this->color == 'w') ? whiteKing : blackKing;
    
    GetPawnMoves(moves);
    GetKnightMoves(moves);
    GetBishopMoves(moves);
    GetRookMoves(moves);
    GetQueenMoves(moves);
    GetKingMoves(moves);
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

    //std::cout << "Move: fromIndex = " << move.fromIndex << ", toIndex = " << move.toIndex << " moveType: " << move.moveType << std::endl;

    int fromRow = move.fromIndex / 8;
    int fromCol = move.fromIndex % 8;
    int toRow = move.toIndex / 8;
    int toCol = move.toIndex % 8;

    char fromPiece = GetPieceAtIndex(move.fromIndex);
    char toPiece = GetPieceAtIndex(move.toIndex);

    int fromPieceIndex = PieceZobristIndex(fromPiece);
    int toPieceIndex = PieceZobristIndex(toPiece);

    undo.pieceMoved = fromPiece;
    undo.pieceCaptured = toPiece;
    undo.lastColor = this->color;

    undo.epIndex = this->epIndex;

    undo.whiteKingSide = whiteKingSide;
    undo.whiteQueenSide = whiteQueenSide;
    undo.blackKingSide = blackKingSide;
    undo.blackQueenSide = blackQueenSide;

    //remove capturing piece from from-square
    this->hash ^= Engine::pieceSquareVals[move.fromIndex][fromPieceIndex];
    //remove captured piece from to-square
    if (toPiece != '.')
    {
        this->hash ^= Engine::pieceSquareVals[move.toIndex][toPieceIndex];
    }
    //add capturing piece to to-square
    this->hash ^= Engine::pieceSquareVals[move.toIndex][fromPieceIndex];
    this->hash ^= Engine::sideKey;

    if (this->epIndex != -1)
    {
        this->hash ^= Engine::epKey[this->epIndex % 8];
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
        if (fromCol == 7) whiteKingSide = false;
        if (fromCol == 0) whiteQueenSide = false;
    }
    else if (fromPiece == 'r')
    {
        if (fromCol == 7) blackKingSide = false;
        if (fromCol == 0) blackQueenSide = false;
    }
    //cant castle after rook is taken
    if (toPiece == 'R')
    {
        if (toRow == 7 && toCol == 7) whiteKingSide = false;
        if (toRow == 7 && toCol == 0) whiteQueenSide = false;
    }
    else if (toPiece == 'r')
    {
        if (toRow == 0 && toCol == 7) blackKingSide = false;
        if (toRow == 0 && toCol == 0) blackQueenSide = false;
    }

    this->epIndex = -1;

    switch(move.moveType)
    {
        case NORMAL:
        {
            RemovePieceAtIndex(fromPiece, move.fromIndex);
            AddPieceAtIndex(fromPiece, move.toIndex);
            break;
        }
        case CAPTURE:
        {
            //std::cout << "Capturing piece: " << GetPieceAtIndex(move.fromIndex) << ", Captured piece: " << GetPieceAtIndex(move.toIndex) << std::endl;
            RemovePieceAtIndex(toPiece, move.toIndex);
            RemovePieceAtIndex(fromPiece, move.fromIndex);
            AddPieceAtIndex(fromPiece, move.toIndex);
            break;
        }
        case PAWNDOUBLE:
        {
            RemovePieceAtIndex(fromPiece, move.fromIndex);
            AddPieceAtIndex(fromPiece, move.toIndex);

            if (fromPiece == 'P')
            {
                this->epIndex = move.toIndex - 8;
            }
            else if (fromPiece == 'p')
            {
                this->epIndex = move.toIndex + 8;
            }

            //std::cout << "New epIndex: " << this->epIndex << ", fromRow: " << fromRow << ", fromCol: " << fromCol
            //          << ", toRow: " << toRow << ", toCol: " << toCol << ", fromPiece: " << fromPiece << std::endl;

            break;
        }
        case PROMOTION:
        {
            RemovePieceAtIndex(fromPiece, move.fromIndex);

            if (toPiece != '.')
            {
                RemovePieceAtIndex(toPiece, move.toIndex);
            }

            switch (move.promoteType)
            {
                case KNIGHT:
                {
                    if (fromPiece == 'P')
                    {
                        AddPieceAtIndex('N', move.toIndex);
                    }
                    else
                    {
                        AddPieceAtIndex('n', move.toIndex);
                    }
                    break;
                }
                case BISHOP:
                {
                    if (fromPiece == 'P')
                    {
                        AddPieceAtIndex('B', move.toIndex);
                    }
                    else
                    {
                        AddPieceAtIndex('b', move.toIndex);
                    }
                    break;
                }
                case ROOK:
                {
                    if (fromPiece == 'P')
                    {
                        AddPieceAtIndex('R', move.toIndex);
                    }
                    else
                    {
                        AddPieceAtIndex('r', move.toIndex);
                    }
                    break;
                }
                case QUEEN:
                {
                    if (fromPiece == 'P')
                    {
                        AddPieceAtIndex('Q', move.toIndex);
                    }
                    else
                    {
                        AddPieceAtIndex('q', move.toIndex);
                    }
                    break;
                }
            }

            break;
        }
        case ENPASSANT:
        {
            RemovePieceAtIndex(fromPiece, move.fromIndex);
            AddPieceAtIndex(fromPiece, move.toIndex);

            if (fromPiece == 'P')
            {
                int pawnIndex = PieceZobristIndex('p');
                RemovePieceAtIndex('p', move.toIndex - 8);
                hash ^= Engine::pieceSquareVals[move.toIndex - 8][pawnIndex];
            }
            else if (fromPiece == 'p')
            {
                int pawnIndex = PieceZobristIndex('P');
                RemovePieceAtIndex('P', move.toIndex + 8);
                hash ^= Engine::pieceSquareVals[move.toIndex + 8][pawnIndex];
            }

            break;
        }
        case CASTLEKING:
        {
            if (fromPiece == 'K')
            {
                RemovePieceAtIndex('K', 4);
                AddPieceAtIndex('K', 6);

                RemovePieceAtIndex('R', 7);
                AddPieceAtIndex('R', 5);

                int rookIndex = PieceZobristIndex('R');
                int kingIndex = PieceZobristIndex('K');

                this->hash ^= Engine::pieceSquareVals[7][rookIndex];
                this->hash ^= Engine::pieceSquareVals[5][rookIndex];
            }
            else
            {
                RemovePieceAtIndex('k', 60);
                AddPieceAtIndex('k', 62);

                RemovePieceAtIndex('r', 63);
                AddPieceAtIndex('r', 61);

                int rookIndex = PieceZobristIndex('r');
                int kingIndex = PieceZobristIndex('k');

                this->hash ^= Engine::pieceSquareVals[63][rookIndex];
                this->hash ^= Engine::pieceSquareVals[61][rookIndex];
            }

            break;
        }
        case CASTLEQUEEN:
        {
            if (fromPiece == 'K')
            {
                RemovePieceAtIndex('K', 4);
                AddPieceAtIndex('K', 2);

                RemovePieceAtIndex('R', 0);
                AddPieceAtIndex('R', 3);

                int rookIndex = PieceZobristIndex('R');
                int kingIndex = PieceZobristIndex('K');

                //this->hash ^= Engine::pieceSquareVals[7][4][kingIndex];
                //this->hash ^= Engine::pieceSquareVals[7][2][kingIndex];
                this->hash ^= Engine::pieceSquareVals[0][rookIndex];
                this->hash ^= Engine::pieceSquareVals[3][rookIndex];
            }
            else
            {
                RemovePieceAtIndex('k', 60);
                AddPieceAtIndex('k', 58);

                RemovePieceAtIndex('r', 56);
                AddPieceAtIndex('r', 59);

                int rookIndex = PieceZobristIndex('r');
                int kingIndex = PieceZobristIndex('k');

                //this->hash ^= Engine::pieceSquareVals[0][4][kingIndex];
                //this->hash ^= Engine::pieceSquareVals[0][2][kingIndex];
                this->hash ^= Engine::pieceSquareVals[56][rookIndex];
                this->hash ^= Engine::pieceSquareVals[59][rookIndex];
            }

            break;
        }
    }

    int newCastleIndex =
    (whiteKingSide  ? 1 : 0) |
    (whiteQueenSide ? 2 : 0) |
    (blackKingSide  ? 4 : 0) |
    (blackQueenSide ? 8 : 0);
    this->hash ^= Engine::castlingKey[newCastleIndex];

    if (this->epIndex != -1)
    {
        this->hash ^= Engine::epKey[this->epIndex % 8];
    }

    occupiedSquares = whitePieces | blackPieces;

    //if (move.moveType == CASTLEKING) std::cout << "End Make Hash: " << this->hash << std::endl;

    SwitchColors();

    return undo;
}

void Board::UnmakeMove(const Move& move, const UndoMove& undo)
{
    //if (move.moveType == CASTLEKING) std::cout << "Start Unmake Hash: " << this->hash << std::endl;
    char fromPiece = undo.pieceMoved;
    char toPiece = undo.pieceCaptured;

    int fromPieceIndex = PieceZobristIndex(fromPiece);
    int toPieceIndex = PieceZobristIndex(toPiece);

    int fromRow = move.fromIndex / 8;
    int fromCol = move.fromIndex % 8;
    int toRow = move.toIndex / 8;
    int toCol = move.toIndex % 8;

    int newCastleIndex =
    (whiteKingSide  ? 1 : 0) |
    (whiteQueenSide ? 2 : 0) |
    (blackKingSide  ? 4 : 0) |
    (blackQueenSide ? 8 : 0);
    this->hash ^= Engine::castlingKey[newCastleIndex];

    if (this->epIndex != -1)
    {
        this->hash ^= Engine::epKey[this->epIndex % 8];
    }

    this->color = undo.lastColor;

    this->whiteKingSide = undo.whiteKingSide;
    this->whiteQueenSide = undo.whiteQueenSide;
    this->blackKingSide = undo.blackKingSide;
    this->blackQueenSide = undo.blackQueenSide;

    this->epIndex = undo.epIndex;

    this->hash ^= Engine::pieceSquareVals[move.fromIndex][fromPieceIndex];
    this->hash ^= Engine::pieceSquareVals[move.toIndex][fromPieceIndex];
    if (toPiece != '.')
    {
        this->hash ^= Engine::pieceSquareVals[move.toIndex][toPieceIndex];
    }
    this->hash ^= Engine::sideKey;

    if (this->epIndex != -1)
    {
        this->hash ^= Engine::epKey[this->epIndex % 8];
    }

    int oldCastleIndex =
    (undo.whiteKingSide  ? 1 : 0) |
    (undo.whiteQueenSide ? 2 : 0) |
    (undo.blackKingSide  ? 4 : 0) |
    (undo.blackQueenSide ? 8 : 0);
    this->hash ^= Engine::castlingKey[oldCastleIndex];

    switch (move.moveType)
    {
        case NORMAL:
        case PAWNDOUBLE:
        {
            RemovePieceAtIndex(fromPiece, move.toIndex);
            AddPieceAtIndex(fromPiece, move.fromIndex);

            break;
        }
        case CAPTURE:
        {
            RemovePieceAtIndex(fromPiece, move.toIndex);
            AddPieceAtIndex(fromPiece, move.fromIndex);
            AddPieceAtIndex(toPiece, move.toIndex);
            
            break;
        }
        case ENPASSANT:
        {
            RemovePieceAtIndex(fromPiece, move.toIndex);
            AddPieceAtIndex(fromPiece, move.fromIndex);

            if (fromPiece == 'P')
            {
                AddPieceAtIndex('p', move.toIndex - 8);
                this->hash ^= Engine::pieceSquareVals[move.toIndex - 8][PieceZobristIndex('p')]; 
            }
            else if (fromPiece == 'p')
            {
                AddPieceAtIndex('P', move.toIndex + 8);
                this->hash ^= Engine::pieceSquareVals[move.toIndex + 8][PieceZobristIndex('P')]; 
            }

            break;
        }
        case PROMOTION:
        {
            char promotedPiece = GetPieceAtIndex(move.toIndex);

            RemovePieceAtIndex(promotedPiece, move.toIndex);
            if (toPiece != '.') AddPieceAtIndex(toPiece, move.toIndex);

            AddPieceAtIndex(fromPiece, move.fromIndex);

            break;
        }
        case CASTLEKING:
        {
            if (fromPiece == 'K')
            {
                RemovePieceAtIndex('K', 6);
                AddPieceAtIndex('K', 4);

                RemovePieceAtIndex('R', 5);
                AddPieceAtIndex('R', 7);

                int rookIndex = PieceZobristIndex('R');
                int kingIndex = PieceZobristIndex('K');
                
                this->hash ^= Engine::pieceSquareVals[7][rookIndex];
                this->hash ^= Engine::pieceSquareVals[5][rookIndex];
            }
            else
            {
                RemovePieceAtIndex('k', 62);
                AddPieceAtIndex('k', 60);

                RemovePieceAtIndex('r', 61);
                AddPieceAtIndex('r', 63);

                int rookIndex = PieceZobristIndex('r');
                int kingIndex = PieceZobristIndex('k');

                this->hash ^= Engine::pieceSquareVals[63][rookIndex];
                this->hash ^= Engine::pieceSquareVals[61][rookIndex];
            }

            break;
        }
        case CASTLEQUEEN:
        {
            if (fromPiece == 'K')
            {
                RemovePieceAtIndex('K', 2);
                AddPieceAtIndex('K', 4);

                RemovePieceAtIndex('R', 3);
                AddPieceAtIndex('R', 0);

                int rookIndex = PieceZobristIndex('R');
                int kingIndex = PieceZobristIndex('K');

                //this->hash ^= Engine::pieceSquareVals[7][2][kingIndex];
                //this->hash ^= Engine::pieceSquareVals[7][4][kingIndex];
                this->hash ^= Engine::pieceSquareVals[3][rookIndex];
                this->hash ^= Engine::pieceSquareVals[0][rookIndex];
            }
            else
            {
                RemovePieceAtIndex('k', 58);
                AddPieceAtIndex('k', 60);

                RemovePieceAtIndex('r', 59);
                AddPieceAtIndex('r', 56);

                int rookIndex = PieceZobristIndex('r');
                int kingIndex = PieceZobristIndex('k');

                //this->hash ^= Engine::pieceSquareVals[0][2][kingIndex];
                //this->hash ^= Engine::pieceSquareVals[0][4][kingIndex];
                this->hash ^= Engine::pieceSquareVals[56][rookIndex];
                this->hash ^= Engine::pieceSquareVals[59][rookIndex];
            }

            break;
        }
    }

    occupiedSquares = whitePieces | blackPieces;
    //std::cout << "End Unmake Hash: " << this->hash << std::endl;
}