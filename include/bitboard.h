#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <string> // Fix: Include for std::string
#include <vector>

class Move {
public:
	std::string move;  // The move string in algebraic notation (e.g., "e2e4")
	char promotion;    // The promotion piece ('Q', 'R', 'B', 'N', or '\0' for none)

	// Constructor
	Move(const std::string& moveString, char promotionPiece = '\0')
		: move(moveString), promotion(promotionPiece) {}
};

struct MoveState {
	uint64_t whitePieces, blackPieces;
	uint64_t whiteKing, blackKing;
	uint64_t whiteQueens, blackQueens;
	uint64_t whiteRooks, blackRooks;
	uint64_t whiteBishops, blackBishops;
	uint64_t whiteKnights, blackKnights;
	uint64_t whitePawns, blackPawns;
	bool canCastleWhiteKing, canCastleWhiteQueen;
	bool canCastleBlackKing, canCastleBlackQueen;
	int enPassantTarget;
};

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
	uint64_t whitePieces = 0;

	uint64_t blackPawns = 0;
	uint64_t blackKnights = 0;
	uint64_t blackBishops = 0;
	uint64_t blackRooks = 0;
	uint64_t blackQueens = 0;
	uint64_t blackKing = 0;
	uint64_t blackPieces = 0;

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
	bool isSquareAttacked(int squareIndex, bool byWhite) const; // Determines if a square is attacked by a color
	bool isSquareOccupied(int squareIndex) const; // Determines if a square is occupied
	uint64_t getKnightAttacks(int squareIndex) const; // Returns the knight attacks for a square
	uint64_t getBishopAttacks(int squareIndex) const; // Returns the bishop attacks for a square
	uint64_t getRookAttacks(int squareIndex) const; // Returns the rook attacks for a square
	uint64_t getQueenAttacks(int squareIndex) const; // Returns the queen attacks for a square
	uint64_t getKingAttacks(int squareIndex) const; // Returns the king attacks for a square
	uint64_t generateSlidingAttacks(int squareIndex, uint64_t occupied) const;
	std::vector<Move> generateLegalMoves(bool isWhiteTurn); // Generates all legal moves for a color
	std::vector<Move> generatePseudoLegalMoves(bool isWhiteTurn); // Generates all pseudo-legal moves for a color
	void generatePawnMoves(std::vector<Move>& moves, bool isWhite); // Generates pawn moves
	void generateKnightMoves(std::vector<Move>& moves, bool isWhite); // Generates knight moves
	void precomputeKnightAttacks();
	void generateSlidingPieceMoves(std::vector<Move>& moves, uint64_t pieceBitboard, bool isBishop, bool isWhite); // Correct declaration
	void generateKingMoves(std::vector<Move>& moves, uint64_t kingBitboard, bool isWhite); // Generates king moves
	void generateCastlingMoves(std::vector<Move>& moves, bool isWhite); // Generates castling moves
	bool isBoundaryCrossed(int StartIndex, int currentIndex, int direction)  const;
	bool isKingInCheck(bool isWhite) const; // Determines if a king is in check
	void undoMove(const Move& move);
	std::string formatMove(int sourceIndex, int targetIndex, char promotion = '\0'); // Keep default in header
	uint64_t generatePawnAttacks(bool isWhite, uint64_t pawns) const; // Generates pawn attacks
	uint64_t generateKnightAttacks(uint64_t knights) const; // Generates knight attacks
	uint64_t generateKingAttacks(uint64_t king) const; // Generates king attacks
	uint64_t getKingAttacks(uint64_t kingBitboard) const; // Returns the king attacks for a square
	MoveState createMoveState() const;
};

#endif // BITBOARD_H