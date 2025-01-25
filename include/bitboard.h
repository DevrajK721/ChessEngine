#ifndef BITBOARD_H
#define BITBOARD_H

#pragma once
#include <cstdint>
#include <string> // Fix: Include for std::string
#include <vector>
#include "move.h"

class Bitboard {
private:
	int enPassantTarget = -1; // Index of square available for an en Passant, or -1 for none
public:
	// Castling Rights
	bool canCastleWhiteKing = true, canCastleWhiteQueen = true;
	bool canCastleBlackKing = true, canCastleBlackQueen = true;

	// Masks for ranks and files
	static constexpr uint64_t FileA = 0x0101010101010101ULL;
	static constexpr uint64_t FileB = FileA << 1;
	static constexpr uint64_t FileC = FileA << 2;
	static constexpr uint64_t FileD = FileA << 3;
	static constexpr uint64_t FileE = FileA << 4;
	static constexpr uint64_t FileF = FileA << 5;
	static constexpr uint64_t FileG = FileA << 6;
	static constexpr uint64_t FileH = FileA << 7;

	static constexpr uint64_t Rank1 = 0x00000000000000FFULL;
	static constexpr uint64_t Rank2 = Rank1 << 8;
	static constexpr uint64_t Rank3 = Rank1 << 16;
	static constexpr uint64_t Rank4 = Rank1 << 24;
	static constexpr uint64_t Rank5 = Rank1 << 32;
	static constexpr uint64_t Rank6 = Rank1 << 40;
	static constexpr uint64_t Rank7 = Rank1 << 48;
	static constexpr uint64_t Rank8 = Rank1 << 56;

	// Knight Precomputed Attacks
	uint64_t precomputedKnightAttacks[64];

	uint64_t whitePawns = 0;
	uint64_t whiteKnights = 0;
	uint64_t whiteBishops = 0;
	uint64_t whiteRooks = 0;
	uint64_t whiteQueens = 0;
	uint64_t whiteKing = 0;
	uint64_t whitePieces = 0;

	uint64_t blackPawns = 0;
	uint64_t blackKnights = 0;
	uint64_t blackBishops = 0;
	uint64_t blackRooks = 0;
	uint64_t blackQueens = 0;
	uint64_t blackKing = 0;
	uint64_t blackPieces = 0;

	// Zobrist Hashing Setup 
	uint64_t zobristKey; // Current Position's Hash Key
	static uint64_t zobristPiece[12][64]; 
	static uint64_t zobristSideToMove;
	void updateZobristKey(); // Updates the Zobrist key for the current position


	// Initialization
	Bitboard(); // Constructor declaration
	void initialize();          // Initializes starting position

	// Display & Board Manipulation
	std::string displayBoard() const; // Returns a string representation of the board
	MoveState createMoveState() const;
	void makeMove(const std::string& move, char promotionPiece = 'Q'); // Makes a move on the board
	void undoMove(const Move& move);
	std::string formatMove(int sourceIndex, int targetIndex, char promotion = '\0'); // Keep default in header
	void clearSquare(uint64_t squareBit); // Clears a square on the board
	std::vector<MoveState> moveHistory; // History of moves

	// Getters
	int getEnPassantTarget() const; // Getter for enPassantTarget
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
	uint64_t getKnightAttacks(int squareIndex) const; // Returns the knight attacks for a square
	uint64_t getBishopAttacks(int squareIndex) const; // Returns the bishop attacks for a square
	uint64_t getRookAttacks(int squareIndex) const; // Returns the rook attacks for a square
	uint64_t getQueenAttacks(int squareIndex) const; // Returns the queen attacks for a square
	uint64_t getKingAttacks(int squareIndex) const; // Returns the king attacks for a square
	uint64_t generateSlidingAttacks(int squareIndex, uint64_t occupied) const;

	// Move Generation
	std::vector<Move> generateLegalMoves(bool isWhiteTurn); // Generates all legal moves for a color
	std::vector<Move> generatePseudoLegalMoves(bool isWhiteTurn); // Generates all pseudo-legal moves for a color
	void generatePawnMoves(std::vector<Move>& moves, bool isWhite); // Generates pawn moves
	void generateKnightMoves(std::vector<Move>& moves, bool isWhite); // Generates knight moves
	void precomputeKnightAttacks();
	void generateSlidingPieceMoves(std::vector<Move>& moves, uint64_t pieceBitboard, bool isBishop, bool isWhite); // Correct declaration
	void generateKingMoves(std::vector<Move>& moves, uint64_t kingBitboard, bool isWhite); // Generates king moves
	void generateCastlingMoves(std::vector<Move>& moves, bool isWhite); // Generates castling moves
	uint64_t generatePawnAttacks(bool isWhite, uint64_t pawns) const; // Generates pawn attacks
	uint64_t generateKnightAttacks(uint64_t knights) const; // Generates knight attacks
	uint64_t generateKingAttacks(uint64_t king) const; // Generates king attacks
	uint64_t getKingAttacks(uint64_t kingBitboard) const; // Returns the king attacks for a square

	// Helper Functions
	bool isSquareAttacked(int squareIndex, bool byWhite) const; // Determines if a square is attacked by a color
	bool isSquareOccupied(int squareIndex) const; // Determines if a square is occupied
	bool isBoundaryCrossed(int StartIndex, int currentIndex, int direction)  const;
	bool isKingInCheck(bool isWhite) const; // Determines if a king is in check
	bool isCaptureMove(const std::string& move) const;
	int getDestinationSquare(const std::string& move) const;
	bool isSquareOccupiedByOpponent(int squareIndex, bool byWhite) const;

	// Zobrist Hashing
	uint64_t computeZobristKey() const; // Computes the Zobrist key for the current position

	// Miscellaneous
	int bitCount(uint64_t x) const;
	int bitScanForward(uint64_t bitboard) const; // Returns the index of the lowest bit
};

#endif // BITBOARD_H