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

bool Board::IsSquareAttacked(int row, int col, char attackerColor) 
{
    int index = row * 8 + col; //square to check
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

void Board::GetPawnMoves(int currIndex, std::vector<Move>& moves)
{
    int row = currIndex / 8;
    int col = currIndex % 8;
    uint64_t sq = 1ULL << currIndex;
    int epIndex = enPassantRow * 8 + enPassantCol;

    if (this->color == 'w')
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

void Board::GetKnightMoves(int currIndex, std::vector<Move>& moves)
{
    int row = currIndex / 8;
    int col = currIndex % 8;
    uint64_t attacks = knightAttacks[row][col];

    if (this->color == 'w')
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

void Board::GetBishopMoves(int currIndex, std::vector<Move>& moves)
{
    int row = currIndex / 8;
    int col = currIndex % 8;

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

            if (this->color == 'w')
            {
                //stop if target square is a friendly piece
                if (whitePieces & newSq) break;
                //stop if target square is an enemy piece
                else if (blackPieces & newSq)
                {
                    moves.push_back({currIndex, newIndex, CAPTURE});
                    break;
                }
                else moves.push_back({currIndex, newIndex, NORMAL});
            }
            else
            {
                if (blackPieces & newSq) break;
                else if (whitePieces & newSq)
                {
                    moves.push_back({currIndex, newIndex, CAPTURE});
                    break;
                }
                else moves.push_back({currIndex, newIndex, NORMAL});
            }

            newRow += dirRow;
            newCol += dirCol;
        }
    }
}

void Board::GetRookMoves(int currIndex, std::vector<Move>& moves)
{
    int row = currIndex / 8;
    int col = currIndex % 8;

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

            if (this->color == 'w')
            {
                //stop if target square is a friendly piece
                if (whitePieces & newSq) break;
                //stop if target square is an enemy piece
                else if (blackPieces & newSq)
                {
                    moves.push_back({currIndex, newIndex, CAPTURE});
                    break;
                }
                else moves.push_back({currIndex, newIndex, NORMAL});
            }
            else
            {
                if (blackPieces & newSq) break;
                else if (whitePieces & newSq)
                {
                    moves.push_back({currIndex, newIndex, CAPTURE});
                    break;
                }
                else moves.push_back({currIndex, newIndex, NORMAL});
            }

            newRow += dirRow;
            newCol += dirCol;
        }
    }
}

void Board::GetQueenMoves(int index, std::vector<Move>& moves)
{
    this->GetBishopMoves(index, moves);
    this->GetRookMoves(index, moves);
}

void Board::GetKingMoves(int currIndex, std::vector<Move>& moves)
{
    int row = currIndex / 8;
    int col = currIndex % 8;
    uint64_t attacks = kingAttacks[row][col];

    while (attacks)
    {
        int toIndex = PopLSB(attacks);
        uint64_t newSq = 1ULL << toIndex;

        if (this->color == 'w')
        {
            if (blackPieces & newSq) moves.push_back({currIndex, toIndex, CAPTURE});
        }
        else
        {
            if (whitePieces & newSq) moves.push_back({currIndex, toIndex, CAPTURE});
        }
    
        if (!(occupiedSquares & newSq)) moves.push_back({currIndex, toIndex, NORMAL});
    }

    //castling
    if (this->color == 'w')
    {
        if (this->whiteKingSide 
            && !(occupiedSquares & (1ULL << (currIndex + 1))) 
            && !(occupiedSquares & (1ULL << (currIndex + 2)))
            && !IsKingChecked('w')
            && !IsSquareAttacked(row, col + 1, 'b')
            && !IsSquareAttacked(row, col + 2, 'b')
            && (whiteRooks & (1ULL << 7)))
        {
            moves.push_back({currIndex, currIndex + 2, CASTLEKING});
        }
        if (this->whiteQueenSide 
            && !(occupiedSquares & (1ULL << (currIndex - 1))) 
            && !(occupiedSquares & (1ULL << (currIndex - 2)))
            && !(occupiedSquares & (1ULL << (currIndex - 3)))
            && !IsKingChecked('w')
            && !IsSquareAttacked(row, col - 1, 'b')
            && !IsSquareAttacked(row, col - 2, 'b')
            && (whiteRooks & (1ULL << 0)))
        {
            moves.push_back({currIndex, currIndex - 2, CASTLEQUEEN});
        }
    }
    else
    {
        if (this->blackKingSide 
            && !(occupiedSquares & (1ULL << (currIndex + 1))) 
            && !(occupiedSquares & (1ULL << (currIndex + 2)))
            && !IsKingChecked('b')
            && !IsSquareAttacked(row, col + 1, 'w')
            && !IsSquareAttacked(row, col + 2, 'w')
            && (blackRooks & (1ULL << 63)))
        {
            moves.push_back({currIndex, currIndex + 2, CASTLEKING});
        }
        if (this->blackQueenSide 
            && !(occupiedSquares & (1ULL << (currIndex - 1))) 
            && !(occupiedSquares & (1ULL << (currIndex - 2)))
            && !(occupiedSquares & (1ULL << (currIndex - 3)))
            && !IsKingChecked('b')
            && !IsSquareAttacked(row, col - 1, 'w')
            && !IsSquareAttacked(row, col - 2, 'w')
            && (blackRooks & (1ULL << 56)))
        {
            moves.push_back({currIndex, currIndex - 2, CASTLEQUEEN});
        }
    }
}

void Board::GetLegalMoves(std::vector<Move>& moves)
{
    uint64_t pawns   = (color == 'w') ? whitePawns : blackPawns;
    uint64_t knights = (color == 'w') ? whiteKnights : blackKnights;
    uint64_t bishops = (color == 'w') ? whiteBishops : blackBishops;
    uint64_t rooks   = (color == 'w') ? whiteRooks : blackRooks;
    uint64_t queens  = (color == 'w') ? whiteQueens : blackQueens;
    uint64_t king    = (color == 'w') ? whiteKing : blackKing;

    uint64_t pieces = pawns | knights | bishops | rooks | queens | king;
    while (pieces)
    {
        int index = PopLSB(pieces);
        uint64_t sq = 1ULL << index;

        if (pawns & sq) GetPawnMoves(index, moves);
        else if (knights & sq) GetKnightMoves(index, moves);
        else if (bishops & sq) GetBishopMoves(index, moves);
        else if (rooks & sq) GetRookMoves(index, moves);
        else if (queens & sq) GetQueenMoves(index, moves);
        else if (king & sq) GetKingMoves(index, moves);
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

    undo.enPassantRow = this->enPassantRow;
    undo.enPassantCol = this->enPassantCol;

    undo.whiteKingSide = whiteKingSide;
    undo.whiteQueenSide = whiteQueenSide;
    undo.blackKingSide = blackKingSide;
    undo.blackQueenSide = blackQueenSide;

    //remove capturing piece from from-square
    this->hash ^= Engine::pieceSquareVals[fromRow][fromCol][fromPieceIndex];
    //remove captured piece from to-square
    if (toPiece != '.')
    {
        this->hash ^= Engine::pieceSquareVals[toRow][toCol][toPieceIndex];
    }
    //add capturing piece to to-square
    this->hash ^= Engine::pieceSquareVals[toRow][toCol][fromPieceIndex];
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

    //apply normal moves
    if (move.moveType != CASTLEKING && move.moveType != CASTLEQUEEN)
    {
        RemovePieceAtIndex(fromPiece, move.fromIndex);
        AddPieceAtIndex(fromPiece, move.toIndex);

        if (fromPiece == 'K')
        {
            this->whiteKingRow = toRow;
            this->whiteKingCol = toCol;
        }
        else if (fromPiece == 'k')
        {
            this->blackKingRow = toRow;
            this->blackKingCol = toCol;
        }

        this->enPassantRow = -1;
        this->enPassantCol = -1;

        if (move.moveType == ENPASSANT)
        {
            if (fromPiece == 'P')
            {
                int pawnIndex = PieceZobristIndex('p');
                RemovePieceAtIndex('p', move.toIndex - 8);
                hash ^= Engine::pieceSquareVals[toRow + 1][toCol][pawnIndex];
            }
            else if (fromPiece == 'p')
            {
                int pawnIndex = PieceZobristIndex('P');
                RemovePieceAtIndex('P', move.toIndex + 8);
                hash ^= Engine::pieceSquareVals[toRow - 1][toCol][pawnIndex];
            }
        }
        else if (move.moveType == PAWNDOUBLE)
        {
            this->enPassantRow = -1;
            this->enPassantCol = -1;

            if (fromPiece == 'P')
            {
                this->enPassantRow = toRow + 1;
                this->enPassantCol = toCol;
            }
            else if (fromPiece == 'p')
            {
                this->enPassantRow = toRow - 1;
                this->enPassantCol = toCol;
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
                        RemovePieceAtIndex('P', move.toIndex);
                        AddPieceAtIndex('N', move.toIndex);
                    }
                    else
                    {
                        RemovePieceAtIndex('p', move.toIndex);
                        AddPieceAtIndex('n', move.toIndex);
                    }
                    break;
                }
                case BISHOP:
                {
                    if (fromPiece == 'P')
                    {
                        RemovePieceAtIndex('P', move.toIndex);
                        AddPieceAtIndex('B', move.toIndex);
                    }
                    else
                    {
                        RemovePieceAtIndex('p', move.toIndex);
                        AddPieceAtIndex('b', move.toIndex);
                    }
                    break;
                }
                case ROOK:
                {
                    if (fromPiece == 'P')
                    {
                        RemovePieceAtIndex('P', move.toIndex);
                        AddPieceAtIndex('R', move.toIndex);
                    }
                    else
                    {
                        RemovePieceAtIndex('p', move.toIndex);
                        AddPieceAtIndex('r', move.toIndex);
                    }
                    break;
                }
                case QUEEN:
                {
                    if (fromPiece == 'P')
                    {
                        RemovePieceAtIndex('P', move.toIndex);
                        AddPieceAtIndex('Q', move.toIndex);
                    }
                    else
                    {
                        RemovePieceAtIndex('p', move.toIndex);
                        AddPieceAtIndex('q', move.toIndex);
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
            RemovePieceAtIndex('K', 4);
            AddPieceAtIndex('K', 6);

            RemovePieceAtIndex('R', 7);
            AddPieceAtIndex('R', 5);

            int rookIndex = PieceZobristIndex('R');
            this->hash ^= Engine::pieceSquareVals[7][7][rookIndex];
            this->hash ^= Engine::pieceSquareVals[7][5][rookIndex];

            this->whiteKingRow = 0;
            this->whiteKingCol = 6;
        }
        else
        {
            RemovePieceAtIndex('k', 60);
            AddPieceAtIndex('k', 62);

            RemovePieceAtIndex('r', 63);
            AddPieceAtIndex('r', 61);

            int rookIndex = PieceZobristIndex('r');
            this->hash ^= Engine::pieceSquareVals[0][7][rookIndex];
            this->hash ^= Engine::pieceSquareVals[0][5][rookIndex];

            this->blackKingRow = 7;
            this->blackKingCol = 6;
        }
    }
    else if (move.moveType == CASTLEQUEEN)
    {
        this->enPassantRow = -1;
        this->enPassantCol = -1;

        if (fromPiece == 'K')
        {
            RemovePieceAtIndex('K', 4);
            AddPieceAtIndex('K', 2);

            RemovePieceAtIndex('R', 0);
            AddPieceAtIndex('R', 3);

            int rookIndex = PieceZobristIndex('R');
            this->hash ^= Engine::pieceSquareVals[7][0][rookIndex];
            this->hash ^= Engine::pieceSquareVals[7][3][rookIndex];

            this->whiteKingRow = 0;
            this->whiteKingCol = 2;
        }
        else
        {
            RemovePieceAtIndex('k', 60);
            AddPieceAtIndex('k', 58);

            RemovePieceAtIndex('r', 56);
            AddPieceAtIndex('r', 59);

            int rookIndex = PieceZobristIndex('r');
            this->hash ^= Engine::pieceSquareVals[0][0][rookIndex];
            this->hash ^= Engine::pieceSquareVals[0][3][rookIndex];

            this->blackKingRow = 7;
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

    this->hash ^= Engine::pieceSquareVals[fromRow][fromCol][fromPieceIndex];
    this->hash ^= Engine::pieceSquareVals[toRow][toCol][fromPieceIndex];
    if (toPiece != '.')
    {
        this->hash ^= Engine::pieceSquareVals[toRow][toCol][toPieceIndex];
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

    if (move.moveType == NORMAL)
    {
        RemovePieceAtIndex(fromPiece, move.toIndex);
        AddPieceAtIndex(fromPiece, move.fromIndex);

        if (fromPiece == 'K')
        {
            this->whiteKingRow = move.fromIndex / 8;
            this->whiteKingCol = move.fromIndex % 8;
        }
        else if (fromPiece == 'k')
        {
            this->blackKingRow = move.fromIndex / 8;
            this->blackKingCol = move.fromIndex % 8;
        }
    }
    else if (move.moveType == CAPTURE)
    {
        RemovePieceAtIndex(fromPiece, move.toIndex);
        AddPieceAtIndex(fromPiece, move.fromIndex);
        AddPieceAtIndex(toPiece, move.toIndex);

        if (fromPiece == 'K')
        {
            this->whiteKingRow = move.fromIndex / 8;
            this->whiteKingCol = move.fromIndex % 8;
        }
        else if (fromPiece == 'k')
        {
            this->blackKingRow = move.fromIndex / 8;
            this->blackKingCol = move.fromIndex % 8;
        }
    }
    else if (move.moveType == ENPASSANT)
    {
        RemovePieceAtIndex(fromPiece, move.toIndex);
        AddPieceAtIndex(fromPiece, move.fromIndex);

        if (fromPiece == 'P')
        {
            AddPieceAtIndex('p', move.toIndex - 8); 
        }
        else if (fromPiece == 'p')
        {
            AddPieceAtIndex('P', move.toIndex + 8);
        }
    }
    else if (move.moveType == PROMOTION)
    {
        char promotedPiece = GetPieceAtIndex(move.toIndex);

        RemovePieceAtIndex(promotedPiece, move.toIndex);
        if (toPiece != '.') AddPieceAtIndex(toPiece, move.toIndex);

        AddPieceAtIndex(fromPiece, move.fromIndex);
    }
    else if (move.moveType == CASTLEKING)
    {
        if (fromPiece == 'K')
        {
            RemovePieceAtIndex('K', 6);
            AddPieceAtIndex('K', 4);

            RemovePieceAtIndex('R', 5);
            AddPieceAtIndex('R', 7);

            this->whiteKingRow = 0;
            this->whiteKingCol = 4;
        }
        else
        {
            RemovePieceAtIndex('k', 62);
            AddPieceAtIndex('k', 60);

            RemovePieceAtIndex('r', 61);
            AddPieceAtIndex('r', 63);

            this->blackKingRow = 0;
            this->blackKingCol = 4;
        }
    }
    else if (move.moveType == CASTLEQUEEN)
    {
        if (fromPiece == 'K')
        {
            RemovePieceAtIndex('K', 2);
            AddPieceAtIndex('K', 4);

            RemovePieceAtIndex('R', 3);
            AddPieceAtIndex('R', 0);

            this->whiteKingRow = 7;
            this->whiteKingCol = 4;
        }
        else
        {
            RemovePieceAtIndex('k', 58);
            AddPieceAtIndex('k', 60);

            RemovePieceAtIndex('r', 59);
            AddPieceAtIndex('r', 56);
            
            this->blackKingRow = 7;
            this->blackKingCol = 4;
        }
    }
    //std::cout << "End Unmake Hash: " << this->hash << std::endl;
}