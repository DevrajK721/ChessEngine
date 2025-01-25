#include "bitboard.h"

bool Bitboard::isSquareOccupied(int squareIndex) const {
	uint64_t squareBit = 1ULL << squareIndex;
	return (whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing |
			blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing) & squareBit;
}

int Bitboard::getEnPassantTarget() const {
	return enPassantTarget;
}

bool Bitboard::isSquareAttacked(int squareIndex, bool byWhite) const {
	uint64_t squareBit = 1ULL << squareIndex;

	// 1) Pawns
	if (byWhite) {
		if ((whitePawns & (squareBit >> 7) & ~0x8080808080808080ULL) || // Attack from left
			(whitePawns & (squareBit >> 9) & ~0x0101010101010101ULL)) { // Attack from right
			return true;
			}
	} else {
		if ((blackPawns & (squareBit << 7) & ~0x0101010101010101ULL) || // Attack from left
			(blackPawns & (squareBit << 9) & ~0x8080808080808080ULL)) { // Attack from right
			return true;
			}
	}

	// 2) Knights
	uint64_t knightAttacks = getKnightAttacks(squareIndex);
	if (byWhite && (whiteKnights & knightAttacks)) return true;
	if (!byWhite && (blackKnights & knightAttacks)) return true;

	// 3) Bishops
	uint64_t bishopAttacks = getBishopAttacks(squareIndex);
	if (byWhite && (whiteBishops & bishopAttacks)) return true;
	if (!byWhite && (blackBishops & bishopAttacks)) return true;

	// 4) Rooks
	uint64_t rookAttacks = getRookAttacks(squareIndex);
	if (byWhite && (whiteRooks & rookAttacks)) return true;
	if (!byWhite && (blackRooks & rookAttacks)) return true;

	// 5) Queens
	uint64_t queenAttacks = getQueenAttacks(squareIndex);
	if (byWhite && (whiteQueens & queenAttacks)) return true;
	if (!byWhite && (blackQueens & queenAttacks)) return true;

	// 6) Kings
	uint64_t kingAttacks = getKingAttacks(squareIndex);
	if (byWhite && (whiteKing & kingAttacks)) return true;
	if (!byWhite && (blackKing & kingAttacks)) return true;

	return false;
}

int Bitboard::bitScanForward(uint64_t bitboard) const {
	if (bitboard == 0) return -1; // No bits set
	for (int i = 0; i < 64; ++i) {
		if (bitboard & (1ULL << i)) return i;
	}
	return -1; // Should never happen
}

int Bitboard::bitCount(uint64_t x) const {
    // Implement a built-in or fallback
    // For GCC/Clang: return __builtin_popcountll(x);
    // For MSVC: use _BitScanForward64 or other intrinsics, or a manual loop
    int count = 0;
    while (x) {
        x &= (x - 1);
        count++;
    }
    return count;
}

bool Bitboard::isCaptureMove(const std::string& move) const {

	// Implement the logic to check if the move is a capture

	// For example, you can check if the destination square is occupied by an opponent's piece

	int destinationSquare = std::stoi(move.substr(2, 2)); // Assuming move is in format "e2e4"

	return isSquareOccupiedByOpponent(destinationSquare, move[0] >= 'a' && move[0] <= 'h');

}

int Bitboard::getDestinationSquare(const std::string& move) const {
	// Assuming move is in format "e2e4", extract the destination square
	return std::stoi(move.substr(2, 2));
}

bool Bitboard::isSquareOccupiedByOpponent(int squareIndex, bool byWhite) const {
	uint64_t squareBit = 1ULL << squareIndex;
	if (byWhite) {
		return (blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing) & squareBit;
	} else {
		return (whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing) & squareBit;
	}
}