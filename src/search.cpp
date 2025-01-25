#include "search.h"
#include "evaluation.h"  
#include "zobrist.h"
#include <algorithm>    
#include <limits>
#include "bitboard.h"
#include "move.h"

std::vector<MoveScore> Search::getAllMovesSorted(Bitboard& board, int depth, bool whiteToMove) {
    std::vector<MoveScore> result;

    auto moves = board.generateLegalMoves(whiteToMove);
    // If no moves => checkmate or stalemate, but we return an empty vector.

    for (auto &m : moves) {
        // Make the move
        board.moveHistory.push_back(board.createMoveState());
        board.makeMove(m.move);

        int score = alphaBeta(board, depth-1, -9999999, 9999999, !whiteToMove);

        // Undo
        board.undoMove(m.move);

        // If you're storing everything from White’s perspective:
        // White wants to maximize score, black wants to minimize => invert the sign if black is to move
        // Or keep it consistent with alphaBeta's definition.

        result.push_back({m.move, score});
    }

    // Sort best to worst from White’s perspective
    std::sort(result.begin(), result.end(), 
              [](const MoveScore& a, const MoveScore& b){
                  return a.score > b.score; // highest first
              });
    return result;
}

SearchResult Search::alphaBetaSearch(Bitboard& board,
                                     int depth,
                                     bool isWhiteTurn)
{
    // We'll keep track of the best move and best score from the root
    SearchResult result;
    result.bestScore = isWhiteTurn ? -999999 : 999999;
    result.bestMove  = "";

    // Generate all legal moves for side to move
    auto moves = board.generateLegalMoves(isWhiteTurn);
    // Basic move ordering: push captures to front
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        bool aCapture = board.isCaptureMove(a.move);
        bool bCapture = board.isCaptureMove(b.move);
        return (aCapture > bCapture);
    });

    // If no moves, it's checkmate or stalemate, so evaluate directly
    if (moves.empty()) {
        // Return the evaluation with sign depending on checkmate/stalemate
        int score = Evaluation::evaluate(board);
        // If in check => checkmate => huge negative or positive
        if (board.isKingInCheck(isWhiteTurn)) {
            score = isWhiteTurn ? -999999 : 999999;
        }
        result.bestScore = score;
        return result;
    }

    // Basic alpha/beta window
    int alpha = -9999999;
    int beta  =  9999999;

    // Try each move at the root
    for (auto& move : moves) {
        // Save current position to restore after we test the move
        board.moveHistory.push_back(board.createMoveState());
        board.makeMove(move.move);
        
        int score = alphaBeta(board, depth - 1, alpha, beta, !isWhiteTurn);

        // Undo
        board.undoMove(move.move);

        if (isWhiteTurn) {
            // We want to maximize the score
            if (score > result.bestScore) {
                result.bestScore = score;
                result.bestMove  = move.move;
            }
            alpha = std::max(alpha, score);
        } else {
            // Minimizing player
            if (score < result.bestScore) {
                result.bestScore = score;
                result.bestMove  = move.move;
            }
            beta = std::min(beta, score);
        }
        if (alpha >= beta) {
            // alpha-beta cutoff
            break;
        }
    }

    return result;
}

int Search::alphaBeta(Bitboard& board, int depth, int alpha, int beta, bool isWhiteTurn) {
    // 0) Transposition Table Probe
    int index = board.zobristKey % TT_SIZE; 
    TTEntry& entry = transTable[index];

    if (entry.key == board.zobristKey && entry.depth >= depth) {
        // We have a "hit"
        switch (entry.type) {
            case TT_EXACT:
                return entry.score;
            case TT_ALPHA: // fail-low
                if (entry.score <= alpha) {
                    return entry.score; 
                }
                break;
            case TT_BETA: // fail-high
                if (entry.score >= beta) {
                    return entry.score;
                }
                break;
        }
    }

    // 1) Check if depth==0 or game over
    if (depth == 0) {
        int eval = Evaluation::evaluate(board);
        // store in TT
        entry.key   = board.zobristKey;
        entry.depth = depth;
        entry.score = eval;
        entry.type  = TT_EXACT;
        return eval;
    }

    auto moves = board.generateLegalMoves(isWhiteTurn);
    if (moves.empty()) {
        // checkmate or stalemate
        int eval = Evaluation::evaluate(board);
        // store TT
        entry.key   = board.zobristKey;
        entry.depth = depth;
        entry.score = eval;
        entry.type  = TT_EXACT;
        return eval;
    }

    int oldAlpha = alpha;
    int bestScore = isWhiteTurn ? -9999999 : 9999999;

    // 2) Move ordering if you like (sort captures, etc.)

    // 3) Search each move
    for (auto &m : moves) {
        board.moveHistory.push_back(board.createMoveState());
        board.makeMove(m.move);

        int score = alphaBeta(board, depth - 1, alpha, beta, !isWhiteTurn);

        board.undoMove(m.move);

        if (isWhiteTurn) {
            if (score > bestScore) bestScore = score;
            if (bestScore > alpha) alpha = bestScore;
        } else {
            if (score < bestScore) bestScore = score;
            if (bestScore < beta) beta = bestScore;
        }

        if (alpha >= beta) {
            break; // alpha-beta cutoff
        }
    }

    // 4) Store result in TT
    entry.key   = board.zobristKey;
    entry.depth = depth;
    entry.score = bestScore;

    if (bestScore <= oldAlpha) {
        // fail-low => bestScore <= alpha => BETA node
        entry.type = TT_BETA; 
    } else if (bestScore >= beta) {
        // fail-high => bestScore >= beta => ALPHA node
        entry.type = TT_ALPHA;
    } else {
        entry.type = TT_EXACT;
    }

    return bestScore;
}
