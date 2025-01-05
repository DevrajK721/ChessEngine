#include "bitboard.h"
#include <iostream>
#include <bitset>
#include <stdexcept>

// Constructor initializes all bitboards to 0
Bitboard::Bitboard()
    : whitePawns(0), whiteKnights(0), whiteBishops(0), whiteRooks(0),
      whiteQueens(0), whiteKing(0), blackPawns(0), blackKnights(0),
      blackBishops(0), blackRooks(0), blackQueens(0), blackKing(0), enPassantTarget(-1) {}

// Castling Rights
bool whiteCanCastleKingside = true;
bool whiteCanCastleQueenside = true;
bool blackCanCastleKingside = true;
bool blackCanCastleQueenside = true;

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
    // 0. Handle Castling
    // --------------------------------------------------
    if (move == "e1g1" && (whiteKing & (1ULL << 4)) && (whiteRooks & (1ULL << 7))) { // White kingside castling
        if (whiteCanCastleKingside && !isSquareAttacked(4, false) && !isSquareAttacked(5, false) && !isSquareAttacked(6, false)) {
            whiteKing &= ~(1ULL << 4); // Remove king from e1
            whiteKing |= (1ULL << 6);  // Place king on g1
            whiteRooks &= ~(1ULL << 7); // Remove rook from h1
            whiteRooks |= (1ULL << 5);  // Place rook on f1
            whiteCanCastleKingside = false;
            whiteCanCastleQueenside = false; // King moved
            return;
        } else {
            throw std::invalid_argument("Illegal kingside castling for white.");
        }
    } else if (move == "e1c1" && (whiteKing & (1ULL << 4)) && (whiteRooks & (1ULL << 0))) { // White queenside castling
        if (whiteCanCastleQueenside && !isSquareAttacked(4, false) && !isSquareAttacked(3, false) && !isSquareAttacked(2, false)) {
            whiteKing &= ~(1ULL << 4); // Remove king from e1
            whiteKing |= (1ULL << 2);  // Place king on c1
            whiteRooks &= ~(1ULL << 0); // Remove rook from a1
            whiteRooks |= (1ULL << 3);  // Place rook on d1
            whiteCanCastleKingside = false;
            whiteCanCastleQueenside = false; // King moved
            return;
        } else {
            throw std::invalid_argument("Illegal queenside castling for white.");
        }
    } else if (move == "e8g8" && (blackKing & (1ULL << 60)) && (blackRooks & (1ULL << 63))) { // Black kingside castling
        if (blackCanCastleKingside && !isSquareAttacked(60, true) && !isSquareAttacked(61, true) && !isSquareAttacked(62, true)) {
            blackKing &= ~(1ULL << 60); // Remove king from e8
            blackKing |= (1ULL << 62);  // Place king on g8
            blackRooks &= ~(1ULL << 63); // Remove rook from h8
            blackRooks |= (1ULL << 61);  // Place rook on f8
            blackCanCastleKingside = false;
            blackCanCastleQueenside = false; // King moved
            return;
        } else {
            throw std::invalid_argument("Illegal kingside castling for black.");
        }
    } else if (move == "e8c8" && (blackKing & (1ULL << 60)) && (blackRooks & (1ULL << 56))) { // Black queenside castling
        if (blackCanCastleQueenside && !isSquareAttacked(60, true) && !isSquareAttacked(59, true) && !isSquareAttacked(58, true)) {
            blackKing &= ~(1ULL << 60); // Remove king from e8
            blackKing |= (1ULL << 58);  // Place king on c8
            blackRooks &= ~(1ULL << 56); // Remove rook from a8
            blackRooks |= (1ULL << 59);  // Place rook on d8
            blackCanCastleKingside = false;
            blackCanCastleQueenside = false; // King moved
            return;
        } else {
            throw std::invalid_argument("Illegal queenside castling for black.");
        }
    }

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
    } else if (whiteKnights & sourceBit) {
        pieceBitboard = &whiteKnights;
        movingWhite = true;
    } else if (whiteBishops & sourceBit) {
        pieceBitboard = &whiteBishops;
        movingWhite = true;
    } else if (whiteRooks & sourceBit) {
        pieceBitboard = &whiteRooks;
        movingWhite = true;
    } else if (whiteQueens & sourceBit) {
        pieceBitboard = &whiteQueens;
        movingWhite = true;
    } else if (whiteKing & sourceBit) {
        pieceBitboard = &whiteKing;
        movingWhite = true;

    } else if (blackPawns & sourceBit) {
        pieceBitboard = &blackPawns;
        movingWhite = false;
    } else if (blackKnights & sourceBit) {
        pieceBitboard = &blackKnights;
        movingWhite = false;
    } else if (blackBishops & sourceBit) {
        pieceBitboard = &blackBishops;
        movingWhite = false;
    } else if (blackRooks & sourceBit) {
        pieceBitboard = &blackRooks;
        movingWhite = false;
    } else if (blackQueens & sourceBit) {
        pieceBitboard = &blackQueens;
        movingWhite = false;
    } else if (blackKing & sourceBit) {
        pieceBitboard = &blackKing;
        movingWhite = false;
    }

    if (!pieceBitboard) {
        throw std::invalid_argument("No piece found on the source square.");
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

    // Ensure castling rights are updated when king or rook moves
    if (sourceIndex == 4) { // White king moved
        whiteCanCastleKingside = false;
        whiteCanCastleQueenside = false;
    } else if (sourceIndex == 60) { // Black king moved
        blackCanCastleKingside = false;
        blackCanCastleQueenside = false;
    } else if (sourceIndex == 7) { // White kingside rook moved
        whiteCanCastleKingside = false;
    } else if (sourceIndex == 0) { // White queenside rook moved
        whiteCanCastleQueenside = false;
    } else if (sourceIndex == 63) { // Black kingside rook moved
        blackCanCastleKingside = false;
    } else if (sourceIndex == 56) { // Black queenside rook moved
        blackCanCastleQueenside = false;
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

uint64_t Bitboard::getKnightAttacks(int squareIndex) const {
    uint64_t knightBit = 1ULL << squareIndex;
    uint64_t l1 = (knightBit & ~0x8080808080808080ULL) >> 1;
    uint64_t l2 = (knightBit & ~0xC0C0C0C0C0C0C0C0ULL) >> 2;
    uint64_t r1 = (knightBit & ~0x0101010101010101ULL) << 1;
    uint64_t r2 = (knightBit & ~0x0303030303030303ULL) << 2;
    uint64_t h1 = l1 | r1;
    uint64_t h2 = l2 | r2;
    uint64_t attacks = (h1 << 8) | (h1 >> 8) | (h2 << 16) | (h2 >> 16);
    return attacks;
}

uint64_t Bitboard::getBishopAttacks(int squareIndex) const {
    uint64_t attacks = 0;
    int rank = squareIndex / 8;
    int file = squareIndex % 8;

    // Generate attacks in four diagonal directions
    for (int r = rank + 1, f = file + 1; r < 8 && f < 8; ++r, ++f) { // Up-right
        int index = r * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }
    for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; ++r, --f) { // Up-left
        int index = r * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }
    for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; --r, ++f) { // Down-right
        int index = r * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; --r, --f) { // Down-left
        int index = r * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }

    return attacks;
}

uint64_t Bitboard::getRookAttacks(int squareIndex) const {
    uint64_t attacks = 0;
    int rank = squareIndex / 8;
    int file = squareIndex % 8;

    // Generate attacks in four cardinal directions
    for (int r = rank + 1; r < 8; ++r) { // Up
        int index = r * 8 + file;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break; // Stop at the first occupied square
    }
    for (int r = rank - 1; r >= 0; --r) { // Down
        int index = r * 8 + file;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }
    for (int f = file + 1; f < 8; ++f) { // Right
        int index = rank * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }
    for (int f = file - 1; f >= 0; --f) { // Left
        int index = rank * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }

    return attacks;
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

uint64_t Bitboard::getQueenAttacks(int squareIndex) const {
    return getRookAttacks(squareIndex) | getBishopAttacks(squareIndex);
}

bool Bitboard::isSquareOccupied(int squareIndex) const {
    uint64_t squareBit = 1ULL << squareIndex;
    return (whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing |
            blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing) & squareBit;
}

int Bitboard::getEnPassantTarget() const {
    return enPassantTarget;
}
