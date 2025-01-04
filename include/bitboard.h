#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <string> // Fix: Include for std::string

class Bitboard {
private:
	int enPassantTarget; // Index of square available for an en Passant, or -1 for none
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
	void makeMove(const std::string& move); // Makes a move on the board
	int getEnPassantTarget() const; // Getter for enPassantTarget
	uint64_t getWhitePawns() const { return whitePawns; } // Getter for whitePawns
	uint64_t getBlackPawns() const { return blackPawns; } // Getter for blackPawns
};

#endif // BITBOARD_H