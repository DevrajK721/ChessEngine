#include "attacks.hpp"

// Masks to prevent wrap around on file A/H
constexpr U64 FILE_A_MASK = 0xFEFEFEFEFEFEFEFEULL; // Mask for file A
constexpr U64 FILE_H_MASK = 0x7F7F7F7F7F7F7F7FULL; // Mask for file H

void init_attacks() {
    init_leaper_attacks(); // Initialize knight and king attacks
    init_pawn_attacks();    // Initialize pawn attacks
}

void init_leaper_attacks() {
    for (int sq = 0; sq < 64; sq++) {
        U64 b = 1ULL << sq; // Bitboard for the square
        U64 knight = 0, king = 0;

        // Knight Jumps (Done by shifting the bitboard left and right)
        knight |= (b << 17) & FILE_A_MASK; // 2 up, 1 right
        knight |= (b << 15) & FILE_H_MASK; // 2 up, 1 left
        knight |= (b << 10) & FILE_A_MASK; // 1 up, 2 right
        knight |= (b << 6) & FILE_H_MASK;  // 1 up, 2 left
        knight |= (b >> 6) & FILE_A_MASK;  // 1 down, 2 right
        knight |= (b >> 10) & FILE_H_MASK; // 1 down, 2 left
        knight |= (b >> 15) & FILE_A_MASK; // 2 down, 1 right
        knight |= (b >> 17) & FILE_H_MASK; // 2 down, 1 left
        knightAttacks[sq] = knight;

        // King Moves (Done by shifting the bitboard left and right)
        king |= (b << 8); // 1 up
        king |= (b >> 8); // 1 down
        king |= (b << 1) & FILE_H_MASK; // 1 right
        king |= (b >> 1) & FILE_A_MASK; // 1 left
        king |= (b << 9) & FILE_H_MASK; // 1 up, 1 right
        king |= (b << 7) & FILE_A_MASK; // 1 up, 1 left
        king |= (b >> 7) & FILE_H_MASK; // 1 down, 1 right
        king |= (b >> 9) & FILE_A_MASK; // 1 down, 1 left
        kingAttacks[sq] = king;
    }
}

void init_pawn_attacks() {
    for (int sq = 0; sq < 64; sq++) {
        U64 b = 1ULL << sq; 

        // White Pawn Attacks (North-East and North-West)
        pawnAttacks[0][sq] = ((b << 9) & not FILE_A_MASK) | ((b << 7) & FILE_H_MASK);

        // Black Pawn Attacks (South-East and South-West)
        pawnAttacks[1][sq] = ((b >> 7) & not FILE_A_MASK) | ((b >> 9) & FILE_H_MASK);
    }
}

static U64 slding_attacks(int sq, U64 occ, const int directions[], int dirCount) {
    U64 attacks = 0ULL; // Initialize attacks
    
    int ri = sq / 8; // Row Index 
    int fi = sq & 8; // File Index

    for (int i = 0; i < dirCount; i++) {
        int d = directions[i];

        int t_sq = sq; // Temporary Square Index for directional iteration
        while (true) {
            int tr = (t_sq / 8) + (d / 8); // Target Row Index
            int tf = (t_sq % 8) + (d % 8); // Target File Index

            if (tr < 0 || tr >= 8 || tf < 0 || tf >= 8) {
                break; // Out of bounds
            }

            t_sq += d; // Move to the next square
            attacks |= (1ULL << t_sq); 
            if (occ & (1ULL << t_sq)) {
                break; // Stop if we hit an occupied square
            }
        }
    }
    return attacks;
}

U64 rook_attacks(int sq, U64 occ) {
    // Directions for Rook: N, E, S, W
    static const int rookDirections[4] = {+8, +1, -8, -1};

    return slding_attacks(sq, occ, rookDirections, 4);
}

U64 bishop_attacks(int sq, U64 occ) {
    // Directions for Bishop: NE, SE, SW, NW
    static const int bishopDirections[4] = {+9, -7, -9, +7};

    return slding_attacks(sq, occ, bishopDirections, 4);
}
