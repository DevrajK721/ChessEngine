#include "bitboard.h"
#include <bitset>
#include <iostream>

void Bitboard::generatePawnMoves(std::vector<Move>& moves, bool isWhite) {
    uint64_t pawns = isWhite ? whitePawns : blackPawns;
    uint64_t ownPieces = isWhite ? whitePieces : blackPieces;
    uint64_t opponentPieces = isWhite ? blackPieces : whitePieces;
    uint64_t emptySquares = ~(whitePieces | blackPieces);
    // Debugging output
    std::cout << "Empty Squares: " << std::bitset<64>(emptySquares) << "\n";

    if (isWhite) {
        // Single-step advances
        uint64_t singlePush = (pawns << 8) & emptySquares;

        // Double-step advances
        uint64_t doublePush = ((singlePush & Rank3) << 8) & emptySquares;

        // Generate single push moves
        while (singlePush) {
            int targetIndex = bitScanForward(singlePush);
            int sourceIndex = targetIndex - 8;

            // Check for promotion
            if (targetIndex >= 56) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'Q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'R'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'B'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'N'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            singlePush &= singlePush - 1; // Clear the lowest bit
        }

        // Generate double push moves
        while (doublePush) {
            int targetIndex = bitScanForward(doublePush);
            int sourceIndex = targetIndex - 16;
            moves.emplace_back(formatMove(sourceIndex, targetIndex));
            doublePush &= doublePush - 1; // Clear the lowest bit
        }

        // Captures
        uint64_t leftCaptures = (pawns << 7) & opponentPieces & ~FileH;
        uint64_t rightCaptures = (pawns << 9) & opponentPieces & ~FileA;

        while (leftCaptures) {
            int targetIndex = bitScanForward(leftCaptures);
            int sourceIndex = targetIndex - 7;

            // Check for promotion
            if (targetIndex >= 56) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'Q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'R'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'B'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'N'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            leftCaptures &= leftCaptures - 1; // Clear the lowest bit
        }

        while (rightCaptures) {
            int targetIndex = bitScanForward(rightCaptures);
            int sourceIndex = targetIndex - 9;

            // Check for promotion
            if (targetIndex >= 56) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'Q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'R'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'B'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'N'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            rightCaptures &= rightCaptures - 1; // Clear the lowest bit
        }

        // En Passant
        if (enPassantTarget != -1) {
            uint64_t enPassantSquare = 1ULL << enPassantTarget;

            uint64_t enPassantLeft = (pawns << 7) & enPassantSquare & ~FileH;
            uint64_t enPassantRight = (pawns << 9) & enPassantSquare & ~FileA;

            if (enPassantLeft) {
                int sourceIndex = enPassantTarget - 7;
                moves.emplace_back(formatMove(sourceIndex, enPassantTarget));
            }

            if (enPassantRight) {
                int sourceIndex = enPassantTarget - 9;
                moves.emplace_back(formatMove(sourceIndex, enPassantTarget));
            }
        }
    } else {
        // Black pawn logic (mirrored for white)

        // Single-step advances
        uint64_t singlePush = (pawns >> 8) & emptySquares;

        // Double-step advances
        uint64_t doublePush = ((singlePush & Rank6) >> 8) & emptySquares;

        // Generate single push moves
        while (singlePush) {
            int targetIndex = bitScanForward(singlePush);
            int sourceIndex = targetIndex + 8;

            // Check for promotion
            if (targetIndex <= 7) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'r'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'b'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'n'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            singlePush &= singlePush - 1; // Clear the lowest bit
        }

        // Generate double push moves
        while (doublePush) {
            int targetIndex = bitScanForward(doublePush);
            int sourceIndex = targetIndex + 16;
            moves.emplace_back(formatMove(sourceIndex, targetIndex));
            doublePush &= doublePush - 1; // Clear the lowest bit
        }

        // Captures
        uint64_t leftCaptures = (pawns >> 9) & opponentPieces & ~FileH;
        uint64_t rightCaptures = (pawns >> 7) & opponentPieces & ~FileA;

        while (leftCaptures) {
            int targetIndex = bitScanForward(leftCaptures);
            int sourceIndex = targetIndex + 9;

            // Check for promotion
            if (targetIndex <= 7) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'r'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'b'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'n'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            leftCaptures &= leftCaptures - 1; // Clear the lowest bit
        }

        while (rightCaptures) {
            int targetIndex = bitScanForward(rightCaptures);
            int sourceIndex = targetIndex + 7;

            // Check for promotion
            if (targetIndex <= 7) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'r'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'b'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'n'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            rightCaptures &= rightCaptures - 1; // Clear the lowest bit
        }

        // En Passant
        if (enPassantTarget != -1) {
            uint64_t enPassantSquare = 1ULL << enPassantTarget;

            uint64_t enPassantLeft = (pawns >> 9) & enPassantSquare & ~FileH;
            uint64_t enPassantRight = (pawns >> 7) & enPassantSquare & ~FileA;

            if (enPassantLeft) {
                int sourceIndex = enPassantTarget + 9;
                moves.emplace_back(formatMove(sourceIndex, enPassantTarget));
            }

            if (enPassantRight) {
                int sourceIndex = enPassantTarget + 7;
                moves.emplace_back(formatMove(sourceIndex, enPassantTarget));
            }
        }
    }
}

uint64_t Bitboard::generatePawnAttacks(bool isWhite, uint64_t pawns) const {
    return isWhite
        ? ((pawns & ~FileA) << 7) | ((pawns & ~FileH) << 9)  // White pawn attacks
        : ((pawns & ~FileA) >> 9) | ((pawns & ~FileH) >> 7); // Black pawn attacks
}