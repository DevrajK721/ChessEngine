#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <string> // Fix: Include for std::string

class Bitboard {
public:
	uint64_t whitePawns;
	uint64_t whiteKnights;
	uint64_t whiteBishops;
	uint64_t whiteRooks;
	uint64_t whiteQueens;
	uint64_t whiteKing;

	uint64_t blackPawns;
	uint64_t blackKnights;
	uint64_t blackBishops;
	uint64_t blackRooks;
	uint64_t blackQueens;
	uint64_t blackKing;

	Bitboard(); // Constructor declaration

	void initialize();          // Initializes starting position
	std::string displayBoard(); // Returns a string representation of the board
	void makeMove(const std::string& move); // Function declaration
};

#endif // BITBOARD_H