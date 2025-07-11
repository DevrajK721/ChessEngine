// Include guard to prevent multiple inclusions of this header file
#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include <cstdint> // For fixed-width integer types
#include <iostream> 

using U64 = uint64_t; // 64-bit unsigned integer type alias to avoid tedious type names

// Basic Bitboard Operations 
inline void set_bit(U64 &b, int sq) { b |= (1ULL << sq); }
inline void clear_bit(U64 &b, int sq) { b &= ~(1ULL << sq); }
inline bool test_bit(const U64 &b, int sq) { return (b >> sq) & 1;}

inline int pop_lsb(U64 &b) {
    int sq = __builtin_ctzll(b); // Count trailing zeros to find the least significant bit
    b &= (b - 1); // Clear the least significant bit
    return sq;
}

// Mapping File and Rank to Square Index 
inline int sq_index(int file, int rank) {
    return rank * 8 + file; // Convert file and rank to square index
}

inline int sq_index(char file_char, char rank_char) {
    int file = file_char - 'a'; // Convert file character to index (0-7)
    int rank = rank_char - '1'; // Convert rank character to index (0-7)
    return sq_index(file, rank); // Use the other sq_index function
}

// Display Bitboard 
inline void display_bitboard(const U64 &b) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int sq = sq_index(file, rank);
            std::cout << (test_bit(b, sq) ? '1' : '0') << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "-----------------\n";
}

#endif // BITBOARD_HPP