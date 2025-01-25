#ifndef SEARCH_H
#define SEARCH_H

#include <cstdint>
#include <vector>
#include "bitboard.h"
#include "search.h"
#include "zobrist.h"
#include "evaluation.h"
#include <string>
#include "move.h"

struct MoveScore {
    std::string move;
    int score;
};

struct SearchResult {
    int bestScore;
    std::string bestMove;
};

class Search {
public:
    // Basic interface: search up to a certain depth, return best move.
    static SearchResult alphaBetaSearch(Bitboard& board,
                                        int depth,
                                        bool isWhiteTurn);

    static std::vector<MoveScore> getAllMovesSorted(Bitboard& board, int depth, bool whiteToMove);
private:
    static int alphaBeta(Bitboard& board,
                         int depth,
                         int alpha,
                         int beta,
                         bool isWhiteTurn);
};

#endif // SEARCH_H