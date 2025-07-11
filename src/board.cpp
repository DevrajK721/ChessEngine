#include "board.hpp"
#include "attacks.hpp"
#include <initializer_list>
#include <cassert>

// Helper function to set several bits at once 
template <size_t N> // Template to handle arrays of different sizes
static void set_bits(U64 &bb, const std::array<int, N>& squares) {
    for (int sq : squares) {
        set_bit(bb, sq);
    }
}

// Bitboard setup
void Board::init_startpos() {
    // Initialize all bitboards to zero 
    for (auto &bb : bitboards) {
        bb = 0ULL;
    }

    // White Pawns on Rank 2 
    std::array<int, 8> whitePawns = {sq_index('a', '2'), sq_index('b', '2'), sq_index('c', '2'),
                                     sq_index('d', '2'), sq_index('e', '2'), sq_index('f', '2'),
                                     sq_index('g', '2'), sq_index('h', '2')};
    int whitePawnsIndex = board_index(WHITE, PAWN);

    set_bits(bitboards[whitePawnsIndex], whitePawns);

    // Black Pawns on Rank 7
    std::array<int, 8> blackPawns = {sq_index('a', '7'), sq_index('b', '7'), sq_index('c', '7'),
                                     sq_index('d', '7'), sq_index('e', '7'), sq_index('f', '7'),
                                     sq_index('g', '7'), sq_index('h', '7')};
    int blackPawnsIndex = board_index(BLACK, PAWN);
    set_bits(bitboards[blackPawnsIndex], blackPawns);

    // White Knights on b1 and g1
    std::array<int, 2> whiteKnights = {sq_index('b', '1'), sq_index('g', '1')};
    int whiteKnightsIndex = board_index(WHITE, KNIGHT);
    set_bits(bitboards[whiteKnightsIndex], whiteKnights);

    // Black Knights on b8 and g8
    std::array<int, 2> blackKnights = {sq_index('b', '8'), sq_index('g', '8')};
    int blackKnightsIndex = board_index(BLACK, KNIGHT);
    set_bits(bitboards[blackKnightsIndex], blackKnights);

    // White Bishops on c1 and f1
    std::array<int, 2> whiteBishops = {sq_index('c', '1'), sq_index('f', '1')};
    int whiteBishopsIndex = board_index(WHITE, BISHOP);
    set_bits(bitboards[whiteBishopsIndex], whiteBishops);

    // Black Bishops on c8 and f8
    std::array<int, 2> blackBishops = {sq_index('c', '8'), sq_index('f', '8')};
    int blackBishopsIndex = board_index(BLACK, BISHOP);
    set_bits(bitboards[blackBishopsIndex], blackBishops);

    // White Rooks on a1 and h1
    std::array<int, 2> whiteRooks = {sq_index('a', '1'), sq_index('h', '1')};
    int whiteRooksIndex = board_index(WHITE, ROOK);
    set_bits(bitboards[whiteRooksIndex], whiteRooks);

    // Black Rooks on a8 and h8
    std::array<int, 2> blackRooks = {sq_index('a', '8'), sq_index('h', '8')};
    int blackRooksIndex = board_index(BLACK, ROOK);
    set_bits(bitboards[blackRooksIndex], blackRooks);

    // White Queen on d1
    int whiteQueenIndex = board_index(WHITE, QUEEN);
    set_bit(bitboards[whiteQueenIndex], sq_index('d', '1'));
    
    // Black Queen on d8
    int blackQueenIndex = board_index(BLACK, QUEEN);
    set_bit(bitboards[blackQueenIndex], sq_index('d', '8'));
    
    // White King on e1
    int whiteKingIndex = board_index(WHITE, KING);
    set_bit(bitboards[whiteKingIndex], sq_index('e', '1'));

    // Black King on e8
    int blackKingIndex = board_index(BLACK, KING);
    set_bit(bitboards[blackKingIndex], sq_index('e', '8'));

    // Update occupancy bitboards
    recompute_occupancy();

}

void Board::recompute_occupancy() {
    // Clear occupancy bitboards
    bothOccupancy = 0ULL;
    whiteOccupancy = 0ULL;
    blackOccupancy = 0ULL;

    // Recompute bothOccupancy, whiteOccupancy, and blackOccupancy
    for (int pt = PAWN; pt <= KING; pt++) {
        whiteOccupancy |= bitboards[board_index(WHITE, PieceType(pt))];
        blackOccupancy |= bitboards[board_index(BLACK, PieceType(pt))];  
    }

    bothOccupancy = whiteOccupancy | blackOccupancy;

}

bool Board::is_square_attacked(int sq, Color bySide) const {
    U64 occ = bothOccupancy; // Local copy of occupancy bitboard
    Color other = bySide == WHITE ? BLACK : WHITE; // Determine the opponent's color from who's turn it is

    // Check for pawn attacks 
    int pi = board_index(bySide, PAWN); // Get the index for the pawn for current side
    if (bySide == WHITE) {
        // Get pawn attacks from black pawns and compare to positions on white bitboards
        if (pawnAttacks[0][sq] & bitboards[pi]) {
            return true;
        } else {
            // Get pawn attacks from black pawns and compare to positions on white bitboards
            if (pawnAttacks[1][sq] & bitboards[pi]) {
                return true;
            }
        }
    }

    // Check for knight attacks 
    if (knightAttacks[sq] & bitboards[board_index(bySide, KNIGHT)]) {
        return true;
    }

    // Check for king attacks
    if (kingAttacks[sq] & bitboards[board_index(bySide, KING)]) {
        return true;
    }

    // Check for sliding piece attacks (Bishops, Rooks, Queens)
    auto occ64 = occ; // Local copy of occupancy bitboard as U64
    // Bishops/Queens 
    if (bishop_attacks(sq, occ64) & (bitboards[board_index(bySide, BISHOP)] | bitboards[board_index(bySide, QUEEN)])) {
        return true; 
    }
    // Rooks/Queens
    if (rook_attacks(sq, occ64) & (bitboards[board_index(bySide, ROOK)] | bitboards[board_index(bySide, QUEEN)])) {
        return true;
    }

    return false; // If no attacks found, return false

}

PieceType Board::piece_at(int sq, Color &color_out) const {
    U64 mask = 1ULL << sq;
    for (Color c : {WHITE, BLACK}) {
        for (int pt = PAWN; pt <= KING; ++pt) {
            if (bitboards[board_index(c, static_cast<PieceType>(pt))] & mask) {
                color_out = c;
                return static_cast<PieceType>(pt);
            }
        }
    }
    color_out = BOTH;
    return NO_PIECE;
}

int Board::king_square(Color c) const {
    U64 bb = bitboards[board_index(c, KING)];
    if (!bb)
        return -1;
    return __builtin_ctzll(bb);
}

