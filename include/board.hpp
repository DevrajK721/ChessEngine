#ifndef BOARD_HPP
#define BOARD_HPP

#include "bitboard.hpp"
#include "piece.hpp"
#include <array> 
#include <cctype>
#include <iostream>

// Simple struct holding the state to restore
struct Undo {
    int    ep_square;
    bool   w_can_castle_k, w_can_castle_q;
    bool   b_can_castle_k, b_can_castle_q;
    PieceType captured;
};

// Board Struct 
struct Board {
    std::array<U64, 12> bitboards; // 12 bitboards for each piece type and color
    
    U64 bothOccupancy; // Bitboard for all occupied squares
    U64 whiteOccupancy; // Bitboard for white occupied squares
    U64 blackOccupancy; // Bitboard for black occupied squares

    Color sideToMove = WHITE; // By default, white to move (can be changed to black)
    int enPassantSquare = -1;
    bool w_can_castle_k = true; // White can castle kingside
    bool w_can_castle_q = true; // White can castle queenside
    bool b_can_castle_k = true; // Black can castle kingside
    bool b_can_castle_q = true; // Black can castle queenside

    // Set up the initial position of the board
    void init_startpos();

    // Recompute the occupancy bitboards
    void recompute_occupancy();

    // Check square attack 
    bool is_square_attacked(int sq, Color bySide) const;

    PieceType piece_at(int sq, Color &color_out) const;

    int king_square(Color c) const;
};

inline void display_board(const Board &b) {
    const char* pieces = " PNBRQK"; // index by PieceType
    const char* white = "\033[37m"; // bright white
    const char* red   = "\033[31m"; // red for black pieces
    const char* reset = "\033[0m";

    auto horiz = [](){
        std::cout << "  +---+---+---+---+---+---+---+---+\n";
    };

    horiz();
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << " |";
        for (int file = 0; file < 8; ++file) {
            int sq = sq_index(file, rank);
            Color col;
            PieceType pt = b.piece_at(sq, col);
            char ch = ' ';
            const char* color = reset;
            if (pt != NO_PIECE) {
                ch = pieces[pt];
                color = (col == WHITE) ? white : red;
                if (col == BLACK)
                    ch = std::tolower(ch);
            }
            std::cout << ' ' << color << ch << reset << " |";
        }
        std::cout << ' ' << rank + 1 << "\n";
        horiz();
    }
    std::cout << "    a   b   c   d   e   f   g   h\n";
}

#endif // BOARD_HPP