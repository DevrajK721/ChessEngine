#include "evaluation.h"

// Simple piece values in centipawns
static const int PAWN_VALUE   = 100;
static const int KNIGHT_VALUE = 320;
static const int BISHOP_VALUE = 330;
static const int ROOK_VALUE   = 500;
static const int QUEEN_VALUE  = 900;

int Evaluation::evaluate(const Bitboard& board) {
    // We'll do: totalWhite - totalBlack
    int whiteScore = 0;
    int blackScore = 0;

    // Pawns
    whiteScore += PAWN_VALUE * board.bitCount(board.whitePawns);
    blackScore += PAWN_VALUE * board.bitCount(board.blackPawns);

    // Knights
    whiteScore += KNIGHT_VALUE * board.bitCount(board.whiteKnights);
    blackScore += KNIGHT_VALUE * board.bitCount(board.blackKnights);

    // Bishops
    whiteScore += BISHOP_VALUE * board.bitCount(board.whiteBishops);
    blackScore += BISHOP_VALUE * board.bitCount(board.blackBishops);

    // Rooks
    whiteScore += ROOK_VALUE * board.bitCount(board.whiteRooks);
    blackScore += ROOK_VALUE * board.bitCount(board.blackRooks);

    // Queens
    whiteScore += QUEEN_VALUE * board.bitCount(board.whiteQueens);
    blackScore += QUEEN_VALUE * board.bitCount(board.blackQueens);

    int score = whiteScore - blackScore;
    // Optionally add a bit of piece-square scoring or king safety here...

    return score;
}