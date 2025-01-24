#include "bitboard.h"

void Bitboard::generateKnightMoves(std::vector<Move>& moves, bool isWhite) {
	uint64_t knights = isWhite ? whiteKnights : blackKnights;
	uint64_t ownPieces = isWhite ? whitePieces : blackPieces;
	uint64_t opponentPieces = isWhite ? blackPieces : whitePieces;

	while (knights) {
		int index = bitScanForward(knights); // Get the index of the lowest knight
		uint64_t knightBit = 1ULL << index;

		// Precomputed knight attack mask
		uint64_t knightAttacks = precomputedKnightAttacks[index] & ~ownPieces;

		// Generate moves
		while (knightAttacks) {
			int targetIndex = bitScanForward(knightAttacks);
			moves.emplace_back(formatMove(index, targetIndex));
			knightAttacks &= knightAttacks - 1; // Clear the lowest bit
		}

		knights &= knights - 1; // Clear the lowest bit of knights
	}
}

void Bitboard::precomputeKnightAttacks() {
	for (int square = 0; square < 64; ++square) {
		uint64_t bit = 1ULL << square;
		uint64_t attacks = 0;

		// Generate all 8 possible knight moves
		if ((bit & ~FileH) << 17) attacks |= (bit & ~FileH) << 17; // Up-right
		if ((bit & ~FileA) << 15) attacks |= (bit & ~FileA) << 15; // Up-left
		if ((bit & ~(FileH | FileG)) << 10) attacks |= (bit & ~(FileH | FileG)) << 10; // Right-up
		if ((bit & ~(FileA | FileB)) << 6) attacks |= (bit & ~(FileA | FileB)) << 6; // Left-up
		if ((bit & ~(FileH | FileG)) >> 6) attacks |= (bit & ~(FileH | FileG)) >> 6; // Right-down
		if ((bit & ~(FileA | FileB)) >> 10) attacks |= (bit & ~(FileA | FileB)) >> 10; // Left-down
		if ((bit & ~FileH) >> 15) attacks |= (bit & ~FileH) >> 15; // Down-right
		if ((bit & ~FileA) >> 17) attacks |= (bit & ~FileA) >> 17; // Down-left

		precomputedKnightAttacks[square] = attacks;
	}
}

uint64_t Bitboard::getKnightAttacks(int squareIndex) const {
	uint64_t knightBit = 1ULL << squareIndex;
	uint64_t l1 = (knightBit & ~0x8080808080808080ULL) >> 1;
	uint64_t l2 = (knightBit & ~0xC0C0C0C0C0C0C0C0ULL) >> 2;
	uint64_t r1 = (knightBit & ~0x0101010101010101ULL) << 1;
	uint64_t r2 = (knightBit & ~0x0303030303030303ULL) << 2;
	uint64_t h1 = l1 | r1;
	uint64_t h2 = l2 | r2;
	uint64_t attacks = (h1 << 8) | (h1 >> 8) | (h2 << 16) | (h2 >> 16);
	return attacks;
}

uint64_t Bitboard::generateKnightAttacks(uint64_t knights) const {
	uint64_t left1 = (knights & ~FileA) >> 1;
	uint64_t right1 = (knights & ~FileH) << 1;
	uint64_t left2 = (knights & ~(FileA | FileB)) >> 2;
	uint64_t right2 = (knights & ~(FileH | FileG)) << 2;

	return (left2 | right2) << 16 | (left2 | right2) >> 16 | (left1 | right1) << 8 | (left1 | right1) >> 8;
}