#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "board.hpp"
#include "movegen.hpp"
#include <unordered_map>
#include <cstdint>

namespace Engine {

struct SearchResult {
    Move bestMove;
    int score;
    int nodes;
};

int evaluate(const Board &b);

SearchResult search(Board &board, int maxDepth);

} // namespace Engine

#endif // ENGINE_HPP