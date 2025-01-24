#include "bitboard.h"

void Bitboard::generateSlidingPieceMoves(std::vector<Move>& moves, uint64_t pieceBitboard, bool isBishop, bool isWhite) {
	while (pieceBitboard) {
		int sourceIndex = bitScanForward(pieceBitboard);
		uint64_t piece = 1ULL << sourceIndex;

		uint64_t attacks = generateSlidingAttacks(sourceIndex, whitePieces | blackPieces) &
						   (isWhite ? ~whitePieces : ~blackPieces);

		while (attacks) {
			int targetIndex = bitScanForward(attacks);
			attacks &= attacks - 1;

			moves.emplace_back(formatMove(sourceIndex, targetIndex));
		}

		pieceBitboard &= pieceBitboard - 1;
	}
}

uint64_t Bitboard::getBishopAttacks(int squareIndex) const {
	uint64_t attacks = 0;
	int rank = squareIndex / 8;
	int file = squareIndex % 8;

	// Generate attacks in four diagonal directions
	for (int r = rank + 1, f = file + 1; r < 8 && f < 8; ++r, ++f) { // Up-right
		int index = r * 8 + f;
		attacks |= (1ULL << index);
		if (isSquareOccupied(index)) break;
	}
	for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; ++r, --f) { // Up-left
		int index = r * 8 + f;
		attacks |= (1ULL << index);
		if (isSquareOccupied(index)) break;
	}
	for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; --r, ++f) { // Down-right
		int index = r * 8 + f;
		attacks |= (1ULL << index);
		if (isSquareOccupied(index)) break;
	}
	for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; --r, --f) { // Down-left
		int index = r * 8 + f;
		attacks |= (1ULL << index);
		if (isSquareOccupied(index)) break;
	}

	return attacks;
}

uint64_t Bitboard::getRookAttacks(int squareIndex) const {
	uint64_t attacks = 0;
	int rank = squareIndex / 8;
	int file = squareIndex % 8;

	// Generate attacks in four cardinal directions
	for (int r = rank + 1; r < 8; ++r) { // Up
		int index = r * 8 + file;
		attacks |= (1ULL << index);
		if (isSquareOccupied(index)) break; // Stop at the first occupied square
	}
	for (int r = rank - 1; r >= 0; --r) { // Down
		int index = r * 8 + file;
		attacks |= (1ULL << index);
		if (isSquareOccupied(index)) break;
	}
	for (int f = file + 1; f < 8; ++f) { // Right
		int index = rank * 8 + f;
		attacks |= (1ULL << index);
		if (isSquareOccupied(index)) break;
	}
	for (int f = file - 1; f >= 0; --f) { // Left
		int index = rank * 8 + f;
		attacks |= (1ULL << index);
		if (isSquareOccupied(index)) break;
	}

	return attacks;
}

uint64_t Bitboard::getQueenAttacks(int squareIndex) const {
	return getRookAttacks(squareIndex) | getBishopAttacks(squareIndex);
}

uint64_t Bitboard::generateSlidingAttacks(int squareIndex, uint64_t occupied) const {
	uint64_t attacks = 0;

	// Calculate attacks for each direction (N, E, S, W, NE, NW, SE, SW)
	for (int direction : {8, -8, 1, -1, 9, -9, 7, -7}) {
		int currentIndex = squareIndex;

		while (true) {
			currentIndex += direction;

			// Check boundaries and add to attack if valid
			if (currentIndex < 0 || currentIndex >= 64 || isBoundaryCrossed(squareIndex, currentIndex, direction)) {
				break;
			}

			attacks |= (1ULL << currentIndex);

			// Stop at the first occupied square
			if (occupied & (1ULL << currentIndex)) {
				break;
			}
		}
	}

	return attacks;
}

bool Bitboard::isBoundaryCrossed(int startIndex, int currentIndex, int direction) const {
	// If out of range, obviously boundary crossed
	if (currentIndex < 0 || currentIndex >= 64) {
		return true;
	}

	int startRank = startIndex / 8, startFile = startIndex % 8;
	int currRank  = currentIndex / 8, currFile  = currentIndex % 8;

	// For a single step in direction, how much do rank/file change?
	int rankDelta = currRank - startRank;
	int fileDelta = currFile - startFile;

	switch (direction) {
		case  8: // up
		case -8: // down
			// The file should not change at all
			return (fileDelta != 0);

		case  1: // right
		case -1: // left
			// The rank should not change at all
			return (rankDelta != 0);

		case  9:  // up-right
		case -9:  // down-left
			// rankDelta and fileDelta should match
			return (rankDelta != fileDelta);

		case  7:  // up-left
		case -7:  // down-right
			// rankDelta should be negative of fileDelta
			return (rankDelta != -fileDelta);

		default:
			return true; // unknown direction => treat as boundary
	}
}
