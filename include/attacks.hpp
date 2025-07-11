#ifndef ATTACKS_HPP
#define ATTACKS_HPP

#include "board.hpp"

// External Declaration (No Memory Allocation Yet) of Knight and King Attacks 
extern U64 knightAttacks[64]; // Knight attacks for each square
extern U64 kingAttacks[64]; // King attacks for each square
extern U64 pawnAttacks[2][64]; // Pawn attacks for white and black (0 for white, 1 for black)

// Pawn Attacks
void init_pawn_attacks(); // Initialize pawn attacks for both colors
void init_leaper_attacks(); // Initialize knight and king attacks

// Sliding Piece Attacks
U64 rook_attacks(int sq, U64 occ); // Rook attacks for a given square with occupancy
U64 bishop_attacks(int sq, U64 occ); // Bishop attacks for a given square with occupancy
inline U64 queen_attacks(int sq, U64 occ) {
    return rook_attacks(sq, occ) | bishop_attacks(sq, occ); // Queen attacks are a combination of rook and bishop attacks
}

// Call once at startup to initialize the attacks
void init_attacks();

#endif // ATTACKS_HPP

