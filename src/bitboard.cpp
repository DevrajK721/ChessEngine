#include "bitboard.h"
#include <iostream>
#include <bitset>
#include <stdexcept>

// Constructor initializes all bitboards to 0
Bitboard::Bitboard()
    : whitePawns(0), whiteKnights(0), whiteBishops(0), whiteRooks(0),
      whiteQueens(0), whiteKing(0), blackPawns(0), blackKnights(0),
      blackBishops(0), blackRooks(0), blackQueens(0), blackKing(0), enPassantTarget(-1) {}

void Bitboard::initialize() {
    // Starting position
    whitePawns = 0x000000000000FF00;
    whiteRooks = 0x0000000000000081;
    whiteKnights = 0x0000000000000042;
    whiteBishops = 0x0000000000000024;
    whiteQueens = 0x0000000000000008;
    whiteKing = 0x0000000000000010;

    blackPawns = 0x00FF000000000000;
    blackRooks = 0x8100000000000000;
    blackKnights = 0x4200000000000000;
    blackBishops = 0x2400000000000000;
    blackQueens = 0x0800000000000000;
    blackKing = 0x1000000000000000;

    enPassantTarget = -1;
}

std::string Bitboard::displayBoard() const {
    std::string board;
    for (int rank = 7; rank >= 0; --rank) {
        board += std::to_string(rank + 1) + " "; // Add rank number
        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            uint64_t bit = 1ULL << index;

            // Dynamically check all bitboards to determine the piece
            if (whiteKing & bit) board += "K ";
            else if (whiteQueens & bit) board += "Q ";
            else if (whiteRooks & bit) board += "R ";
            else if (whiteBishops & bit) board += "B ";
            else if (whiteKnights & bit) board += "N ";
            else if (whitePawns & bit) board += "P ";
            else if (blackKing & bit) board += "k ";
            else if (blackQueens & bit) board += "q ";
            else if (blackRooks & bit) board += "r ";
            else if (blackBishops & bit) board += "b ";
            else if (blackKnights & bit) board += "n ";
            else if (blackPawns & bit) board += "p ";
            else board += ". "; // Empty square
        }
        board += std::to_string(rank + 1) + "\n"; // Add rank number again
    }
    board += "  a b c d e f g h\n"; // Add file letters
    return board;
}


void Bitboard::makeMove(const std::string& move, char promotionPiece) {
    if (move.length() != 4) {
        throw std::invalid_argument("Invalid move format. Use standard notation, e.g., e2e4.");
    }

    int sourceFile = move[0] - 'a';
    int sourceRank = move[1] - '1';
    int destFile   = move[2] - 'a';
    int destRank   = move[3] - '1';

    int sourceIndex = sourceRank * 8 + sourceFile;
    int destIndex   = destRank * 8 + destFile;

    uint64_t sourceBit = 1ULL << sourceIndex;
    uint64_t destBit   = 1ULL << destIndex;

    // --------------------------------------------------
    // 1. Handle En Passant
    // --------------------------------------------------
    if (destIndex == enPassantTarget + 8 || destIndex == enPassantTarget - 8) {
        uint64_t enPassantBit = 1ULL << enPassantTarget;

        if (whitePawns & sourceBit) {
            whitePawns &= ~sourceBit;
            whitePawns |= destBit;
            blackPawns &= ~enPassantBit;
        } else if (blackPawns & sourceBit) {
            blackPawns &= ~sourceBit;
            blackPawns |= destBit;
            whitePawns &= ~enPassantBit;
        }

        enPassantTarget = -1;
        return;
    }

    // --------------------------------------------------
    // 2. Identify Which Piece is Moving
    // --------------------------------------------------
    // We’ll store the pointer to whichever bitboard array is relevant
    uint64_t* pieceBitboard = nullptr;
    bool movingWhite = false;

    if (whitePawns & sourceBit) {
        pieceBitboard = &whitePawns;
        movingWhite = true;
    } else if (blackPawns & sourceBit) {
        pieceBitboard = &blackPawns;
        movingWhite = false;
    }

    if (!pieceBitboard) {
        throw std::invalid_argument("No pawn found on the source square (or piece is not a pawn).");
    }

    // --------------------------------------------------
    // 3. White Pawn Promotion
    // --------------------------------------------------
    if (movingWhite && destRank == 7 && (whitePawns & sourceBit)) {
        // Remove occupant from the destination bit, just in case
        clearSquare(destBit);

        // Remove the original white pawn
        whitePawns &= ~sourceBit;

        // Add the chosen promotion piece
        switch (promotionPiece) {
            case 'R': whiteRooks   |= destBit; break;
            case 'B': whiteBishops |= destBit; break;
            case 'N': whiteKnights |= destBit; break;
            case 'Q':
            default:  whiteQueens  |= destBit; break;
        }

        return;
    }

    // --------------------------------------------------
    // 4. Black Pawn Promotion
    // --------------------------------------------------
    if (!movingWhite && destRank == 0 && (blackPawns & sourceBit)) {
        // Remove occupant from the destination bit, just in case
        clearSquare(destBit);

        // Remove the original black pawn
        blackPawns &= ~sourceBit;

        // Add the chosen promotion piece
        switch (promotionPiece) {
            case 'R': blackRooks   |= destBit; break;
            case 'B': blackBishops |= destBit; break;
            case 'N': blackKnights |= destBit; break;
            case 'Q':
            default:  blackQueens  |= destBit; break;
        }

        return;
    }

    // --------------------------------------------------
    // 5. Non-Promotion Moves
    // --------------------------------------------------
    // Handle en passant target if it's a 2-step pawn move
    if (movingWhite) {
        if (sourceRank == 1 && destRank == 3) {
            enPassantTarget = destIndex;
        } else {
            enPassantTarget = -1;
        }
    } else {
        if (sourceRank == 6 && destRank == 4) {
            enPassantTarget = destIndex;
        } else {
            enPassantTarget = -1;
        }
    }

    // If it’s a capture, remove occupant from destination.
    // (For a more complete engine, you'd detect if there's an opponent piece.)
    clearSquare(destBit);

    // Move the piece
    *pieceBitboard &= ~sourceBit;
    *pieceBitboard |= destBit;
}

void Bitboard::clearSquare(uint64_t squareBit) {
    // Remove from white bitboards
    whitePawns   &= ~squareBit;
    whiteKnights &= ~squareBit;
    whiteBishops &= ~squareBit;
    whiteRooks   &= ~squareBit;
    whiteQueens  &= ~squareBit;
    whiteKing    &= ~squareBit;

    // Remove from black bitboards
    blackPawns   &= ~squareBit;
    blackKnights &= ~squareBit;
    blackBishops &= ~squareBit;
    blackRooks   &= ~squareBit;
    blackQueens  &= ~squareBit;
    blackKing    &= ~squareBit;
}


int Bitboard::getEnPassantTarget() const {
    return enPassantTarget;
}
