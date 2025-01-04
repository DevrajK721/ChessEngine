#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <string> // Fix: Include for std::string

class Bitboard {
private:
	int enPassantTarget = -1; // Index of square available for an en Passant, or -1 for none
public:
	uint64_t whitePawns = 0;
	uint64_t whiteKnights = 0;
	uint64_t whiteBishops = 0;
	uint64_t whiteRooks = 0;
	uint64_t whiteQueens = 0;
	uint64_t whiteKing = 0;

	uint64_t blackPawns = 0;
	uint64_t blackKnights = 0;
	uint64_t blackBishops = 0;
	uint64_t blackRooks = 0;
	uint64_t blackQueens = 0;
	uint64_t blackKing = 0;

	Bitboard(); // Constructor declaration

	void initialize();          // Initializes starting position
	std::string displayBoard() const; // Returns a string representation of the board
	void makeMove(const std::string& move, char promotionPiece = 'Q'); // Makes a move on the board
	int getEnPassantTarget() const; // Getter for enPassantTarget
	void clearSquare(uint64_t squareBit); // Clears a square on the board
	uint64_t getWhitePawns() const { return whitePawns; } // Getter for whitePawns
	uint64_t getBlackPawns() const { return blackPawns; } // Getter for blackPawns
	uint64_t getWhiteQueens() const { return whiteQueens; } // Getter for whiteQueens
	uint64_t getBlackQueens() const { return blackQueens; } // Getter for blackQueens
	uint64_t getWhiteBishops() const { return whiteBishops; } // Getter for whiteBishops
	uint64_t getBlackBishops() const { return blackBishops; } // Getter for blackBishops
	uint64_t getWhiteRooks() const { return whiteRooks; } // Getter for whiteRooks
	uint64_t getBlackRooks() const { return blackRooks; } // Getter for blackRooks
	uint64_t getWhiteKnights() const { return whiteKnights; } // Getter for whiteKnights
	uint64_t getBlackKnights() const { return blackKnights; } // Getter for blackKnights
	uint64_t getWhiteKing() const { return whiteKing; } // Getter for whiteKing
	uint64_t getBlackKing() const { return blackKing; } // Getter for blackKing
};

#endif // BITBOARD_H