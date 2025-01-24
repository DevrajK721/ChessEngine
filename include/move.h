#ifndef MOVE_H
#define MOVE_H

#include "bitboard.h"

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

#endif // MOVE_H

