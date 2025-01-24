#include "bitboard.h"

void Bitboard::generateKingMoves(std::vector<Move>& moves, uint64_t kingBitboard, bool isWhite) {
    int sourceIndex = bitScanForward(kingBitboard);
    uint64_t attacks = getKingAttacks(sourceIndex) & (isWhite ? ~whitePieces : ~blackPieces);

    while (attacks) {
        int targetIndex = bitScanForward(attacks);
        moves.emplace_back(formatMove(sourceIndex, targetIndex));
        attacks &= attacks - 1; // Remove the processed target bit
    }
}

uint64_t Bitboard::generateKingAttacks(uint64_t kingBitboard) const {
    // Assume kingBitboard has exactly one bit set.
    // We'll call that 'square'.
    int square = bitScanForward(kingBitboard);

    // Convert square to rank & file
    int rank = square / 8;
    int file = square % 8;

    // We'll build up a 64-bit mask of possible moves
    uint64_t attacks = 0;

    // Offsets for the 8 surrounding squares
    // (rowOffset, colOffset) pairs
    static const int offsets[8][2] = {
        {1, 0},   // up
        {-1, 0},  // down
        {0, 1},   // right
        {0, -1},  // left
        {1, 1},   // up-right
        {1, -1},  // up-left
        {-1, 1},  // down-right
        {-1, -1}  // down-left
    };

    for (auto& off : offsets) {
        int newRank = rank + off[0];
        int newFile = file + off[1];
        if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
            int newSquare = newRank * 8 + newFile;
            attacks |= (1ULL << newSquare);
        }
    }

    return attacks;
}

void Bitboard::generateCastlingMoves(std::vector<Move>& moves, bool isWhite) {
    // 1) Which king bitboard?
    uint64_t kingBitboard = isWhite ? whiteKing : blackKing;

    // 2) The king’s starting square for White = e1 (bit 4), Black = e8 (bit 60).
    uint64_t kingStart = isWhite ? (1ULL << 4) : (1ULL << 60);

    // 3) If the king isn’t on its start square, no castling
    if ((kingBitboard & kingStart) == 0) {
        return;
    }

    // 4) Occupancy bitmask
    uint64_t occupied = whitePieces | blackPieces;

    // We’ll define a helper lambda to add the move if squares are not attacked
    auto addCastlingMoveIfNotAttacked = [&](int eSq, int fSq, int gSq, uint64_t emptyMask,
                                            bool canCastleKingSide, bool canCastleQueenSide,
                                            bool checkingKingSide) {
        // If we’re checking kingside, ensure canCastleKingSide == true;
        // if queenside, ensure canCastleQueenSide == true.
        if (!checkingKingSide && !canCastleQueenSide) return;
        if (checkingKingSide && !canCastleKingSide) return;

        // Are these squares physically empty (besides the king on eSq)?
        // For kingside: typically fSq/gSq must be empty.
        // For queenside: typically bSq/cSq/dSq or so, depending on the side.
        if ((emptyMask & occupied) != 0) {
            // Some squares are occupied => no castling
            return;
        }

        // Also check squares eSq, fSq, gSq aren’t attacked by the opponent
        // eSq = king’s start, fSq/gSq = path squares
        // For black, we pass byWhite = true
        // For white, we pass byWhite = false
        bool eAtt = isSquareAttacked(eSq, !isWhite);
        bool fAtt = isSquareAttacked(fSq, !isWhite);
        bool gAtt = isSquareAttacked(gSq, !isWhite);
        if (!eAtt && !fAtt && !gAtt) {
            // e1->g1 or e8->g8, e1->c1 or e8->c8, etc.
            moves.emplace_back(formatMove(eSq, gSq));
        }
    };

    // 5) Actually do the kingside and queenside checks

    // ------------------------------
    // A) Kingside
    // ------------------------------
    if (isWhite) {
        // White: e1 => 4, f1 => 5, g1 => 6
        // squares f1,g1 must be empty
        uint64_t ksEmptyMask = (1ULL << 5) | (1ULL << 6);

        addCastlingMoveIfNotAttacked(
            4, 5, 6,           // eSq=4, fSq=5, gSq=6
            ksEmptyMask,       // squares to be empty
            canCastleWhiteKing,
            canCastleWhiteQueen,
            /*checkingKingSide=*/true
        );
    } else {
        // Black: e8 => 60, f8 => 61, g8 => 62
        // squares f8,g8 must be empty
        uint64_t ksEmptyMask = (1ULL << 61) | (1ULL << 62);

        addCastlingMoveIfNotAttacked(
            60, 61, 62,
            ksEmptyMask,
            canCastleBlackKing,
            canCastleBlackQueen,
            /*checkingKingSide=*/true
        );
    }

    // ------------------------------
    // B) Queenside
    // ------------------------------
    if (isWhite) {
        // White: e1 => 4, c1 => 2, squares b1(1), c1(2), d1(3) must be empty
        // Typically we only need c1,d1,b1 to be empty, but the standard approach is b1,c1,d1
        uint64_t qsEmptyMask = (1ULL << 1) | (1ULL << 2) | (1ULL << 3);

        // We want eSq=4, fSq=3, gSq=2 in the sense of e1->(c1).
        // But for simplicity, call them (eSq=4, fSq=3, gSq=2).
        // So we check e1=4, d1=3, c1=2 for attacks.
        addCastlingMoveIfNotAttacked(
            4, 3, 2,           // eSq=4, fSq=3, gSq=2
            qsEmptyMask,
            canCastleWhiteKing,
            canCastleWhiteQueen,
            /*checkingKingSide=*/false
        );
    } else {
        // Black: e8 => 60, c8 => 58, squares b8(57), c8(58), d8(59) must be empty
        uint64_t qsEmptyMask = (1ULL << 57) | (1ULL << 58) | (1ULL << 59);

        // eSq=60, fSq=59, gSq=58 for e8->c8
        addCastlingMoveIfNotAttacked(
            60, 59, 58,
            qsEmptyMask,
            canCastleBlackKing,
            canCastleBlackQueen,
            /*checkingKingSide=*/false
        );
    }
}

uint64_t Bitboard::getKingAttacks(int squareIndex) const {
    uint64_t kingBit = 1ULL << squareIndex;
    uint64_t attacks = ((kingBit << 8) | (kingBit >> 8) |
                        ((kingBit & ~0x0101010101010101ULL) << 1) |
                        ((kingBit & ~0x0101010101010101ULL) >> 7) |
                        ((kingBit & ~0x8080808080808080ULL) >> 1) |
                        ((kingBit & ~0x8080808080808080ULL) << 7) |
                        ((kingBit & ~0x0101010101010101ULL) >> 9) |
                        ((kingBit & ~0x8080808080808080ULL) << 9));
    return attacks;
}

bool Bitboard::isKingInCheck(bool isWhite) const {
    // 1) Locate the actual king's square for the color "isWhite"
    int kingIndex = bitScanForward(isWhite ? whiteKing : blackKing);
    if (kingIndex == -1) {
        // No king found (should never happen in normal chess)
        return false;
    }

    // 2) Check if that king square is attacked by the opponent
    return isSquareAttacked(kingIndex, !isWhite);
}