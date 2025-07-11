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

// Tunable parameters controlling the evaluation function.  They are kept
// outside of the evaluate() call so tests or front-ends can tweak the engine
// without recompiling.
struct EvalParams {
    int mobilityWeight      = 2;   // weighting for simple mobility term
    int spaceControlWeight  = 10;  // bonus per extra controlled square
    int imbalanceWeight     = 30;  // penalty for pieces that are under defended
    int outpostKnightBonus  = 25;  // reward for each protected outpost
    int openFileBonus       = 10;  // rook on open file bonus
    int seventhRankBonus    = 20;  // rook on the 7th rank bonus
    int pawnTensionBonus    = 5;   // bonus per pawn tension
    int pawnBreakBonus      = 10;  // bonus per available pawn break
    int initiativeWeight    = 5;   // side to move forcing move bonus
    int kingSafetyWeight    = 5;   // bonus for king flight squares
};

extern EvalParams evalParams;

int evaluate(const Board &b);

SearchResult search(Board &board, int maxDepth);

} // namespace Engine

#endif // ENGINE_HPP