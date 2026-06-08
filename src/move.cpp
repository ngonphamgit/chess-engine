#include "../include/move.hpp"
#include "../include/board.hpp"

#include <sstream>

std::string Move::ToString(const Board& board) const
{
    std::ostringstream oss;

    oss << "fromSq: " << board.IndexToSquare(fromIndex) << ", toSq: " << board.IndexToSquare(toIndex)
        << ", moveType: " << moveType << std::endl;

    return oss.str();
}