#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include "board.hpp"
#include <vector>
#include <string>

struct Move {
    int from;
    int to;
    PieceType piece;
    PieceType captured;
    PieceType promotion;
    bool isDoublePush;
    bool isEnPassant;
    bool isCastling;
};

// Parse a UCI-style move string (e2e4, e7e8q, etc.) using the current board state
// to determine move attributes.
Move parse_move(const std::string &uci, const Board &board);

// Apply a move, returning an Undo structure for later restoration.
Undo make_move(Board &board, const Move &m);

// Undo a previously made move using the Undo info.
void undo_move(Board &board, const Move &m, const Undo &u);

// Generate all legal moves for the current side to move.
std::vector<Move> generate_legal_moves(Board &board);

#endif // MOVEGEN_HPP