#include "attacks.hpp"

U64 knightAttacks[64];
U64 kingAttacks[64];
U64 pawnAttacks[2][64];

// Masks to prevent wrap around on board edges
constexpr U64 FILE_A_MASK = 0xFEFEFEFEFEFEFEFEULL; // ~file A
constexpr U64 FILE_H_MASK = 0x7F7F7F7F7F7F7F7FULL; // ~file H
constexpr U64 FILE_AB_MASK = 0xFCFCFCFCFCFCFCFCULL; // ~files A&B
constexpr U64 FILE_GH_MASK = 0x3F3F3F3F3F3F3F3FULL; // ~files G&H

void init_attacks() {
    init_leaper_attacks(); // Initialize knight and king attacks
    init_pawn_attacks();    // Initialize pawn attacks
}

void init_leaper_attacks() {
    for (int sq = 0; sq < 64; sq++) {
        U64 b = 1ULL << sq; // Bitboard for the square
        U64 knight = 0, king = 0;

        // Knight Jumps (Done by shifting the bitboard left and right)
        knight |= (b & FILE_H_MASK) << 17; // 2 up, 1 right
        knight |= (b & FILE_A_MASK) << 15; // 2 up, 1 left
        knight |= (b & FILE_GH_MASK) << 10; // 1 up, 2 right
        knight |= (b & FILE_AB_MASK) << 6;  // 1 up, 2 left
        knight |= (b & FILE_GH_MASK) >> 6;  // 1 down, 2 right
        knight |= (b & FILE_AB_MASK) >> 10; // 1 down, 2 left
        knight |= (b & FILE_H_MASK) >> 15; // 2 down, 1 right
        knight |= (b & FILE_A_MASK) >> 17; // 2 down, 1 left
        knightAttacks[sq] = knight;

        // King Moves (Done by shifting the bitboard left and right)
        king |= (b & FILE_H_MASK) << 1;  // 1 right
        king |= (b & FILE_A_MASK) >> 1;  // 1 left
        king |= (b & FILE_H_MASK) << 9;  // 1 up, 1 right
        king |= (b & FILE_A_MASK) << 7;  // 1 up, 1 left
        king |= (b & FILE_H_MASK) >> 7;  // 1 down, 1 right
        king |= (b & FILE_A_MASK) >> 9;  // 1 down, 1 left
        if (sq + 8 < 64) king |= 1ULL << (sq + 8);   // 1 up
        if (sq >= 8)     king |= 1ULL << (sq - 8);   // 1 down
        kingAttacks[sq] = king;
    }
}

void init_pawn_attacks() {
    for (int sq = 0; sq < 64; sq++) {
        U64 b = 1ULL << sq; 

        // White Pawn Attacks (North-East and North-West)
        pawnAttacks[0][sq] = ((b & FILE_H_MASK) << 9) | ((b & FILE_A_MASK) << 7);

        // Black Pawn Attacks (South-East and South-West)
        pawnAttacks[1][sq] = ((b & FILE_H_MASK) >> 7) | ((b & FILE_A_MASK) >> 9);
    }
}

static U64 sliding_attacks(int sq, U64 occ,
                           const int dr[], const int df[], int dirCount) {
    U64 attacks = 0ULL;
    int r0 = sq / 8;
    int f0 = sq % 8;

    for (int i = 0; i < dirCount; ++i) {
        int r = r0 + dr[i];
        int f = f0 + df[i];
        int t = sq;
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            t += dr[i] * 8 + df[i];
            attacks |= (1ULL << t);
            if (occ & (1ULL << t))
                break;
            r += dr[i];
            f += df[i];
        }
    }
    return attacks;
}

U64 rook_attacks(int sq, U64 occ) {
    // Directions for Rook: N, E, S, W
    static const int dr[4] = {1, 0, -1, 0};
    static const int df[4] = {0, 1, 0, -1};
    return sliding_attacks(sq, occ, dr, df, 4);
}

U64 bishop_attacks(int sq, U64 occ) {
    // Directions for Bishop: NE, SE, SW, NW
    static const int dr[4] = {1, -1, -1, 1};
    static const int df[4] = {1, 1, -1, -1};
    return sliding_attacks(sq, occ, dr, df, 4);
}
