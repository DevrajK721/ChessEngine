#include "bitboard.h"
#include <iostream>
#include <bitset>
#include <stdexcept>

// Constructor initializes all bitboards to 0
Bitboard::Bitboard()
    : whitePawns(0), whiteKnights(0), whiteBishops(0), whiteRooks(0),
      whiteQueens(0), whiteKing(0), blackPawns(0), blackKnights(0),
      blackBishops(0), blackRooks(0), blackQueens(0), blackKing(0) {}

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
}

std::string Bitboard::displayBoard() {
    std::string board(64, '.'); // Empty squares as '.'

    // Place pieces on the board
    for (int i = 0; i < 64; ++i) {
        uint64_t bit = 1ULL << i;

        if (whitePawns & bit) board[i] = 'P';
        else if (whiteKnights & bit) board[i] = 'N';
        else if (whiteBishops & bit) board[i] = 'B';
        else if (whiteRooks & bit) board[i] = 'R';
        else if (whiteQueens & bit) board[i] = 'Q';
        else if (whiteKing & bit) board[i] = 'K';

        else if (blackPawns & bit) board[i] = 'p';
        else if (blackKnights & bit) board[i] = 'n';
        else if (blackBishops & bit) board[i] = 'b';
        else if (blackRooks & bit) board[i] = 'r';
        else if (blackQueens & bit) board[i] = 'q';
        else if (blackKing & bit) board[i] = 'k';
    }

    // Convert the board into a visual representation with spacing
    std::string display = "  a b c d e f g h\n"; // Add column labels
    for (int rank = 7; rank >= 0; --rank) {
        display += std::to_string(rank + 1) + " "; // Add rank number
        for (int file = 0; file < 8; ++file) {
            display += board[rank * 8 + file];
            if (file < 7) display += " "; // Add space between squares
        }
        display += " " + std::to_string(rank + 1) + "\n"; // Add rank number at the end
    }
    display += "  a b c d e f g h\n"; // Add column labels again
    return display;
}

void Bitboard::makeMove(const std::string& move) {
    if (move.length() != 4) {
        throw std::invalid_argument("Invalid move format. Use standard notation, e.g., e2e4.");
    }

    // Parse the move (e.g., e2e4)
    int sourceFile = move[0] - 'a'; // Convert 'a'-'h' to 0-7
    int sourceRank = move[1] - '1'; // Convert '1'-'8' to 0-7
    int destFile = move[2] - 'a';
    int destRank = move[3] - '1';

    int sourceIndex = sourceRank * 8 + sourceFile; // Source square index
    int destIndex = destRank * 8 + destFile;       // Destination square index

    uint64_t sourceBit = 1ULL << sourceIndex;
    uint64_t destBit = 1ULL << destIndex;

    // Determine which piece is moving
    uint64_t* pieceBitboard = nullptr;
    char piece = '.';

    // Check each bitboard to find the piece
    if (whitePawns & sourceBit) {
        pieceBitboard = &whitePawns;
        piece = 'P';
    } else if (whiteKnights & sourceBit) {
        pieceBitboard = &whiteKnights;
        piece = 'N';
    } else if (whiteBishops & sourceBit) {
        pieceBitboard = &whiteBishops;
        piece = 'B';
    } else if (whiteRooks & sourceBit) {
        pieceBitboard = &whiteRooks;
        piece = 'R';
    } else if (whiteQueens & sourceBit) {
        pieceBitboard = &whiteQueens;
        piece = 'Q';
    } else if (whiteKing & sourceBit) {
        pieceBitboard = &whiteKing;
        piece = 'K';
    } else if (blackPawns & sourceBit) {
        pieceBitboard = &blackPawns;
        piece = 'p';
    } else if (blackKnights & sourceBit) {
        pieceBitboard = &blackKnights;
        piece = 'n';
    } else if (blackBishops & sourceBit) {
        pieceBitboard = &blackBishops;
        piece = 'b';
    } else if (blackRooks & sourceBit) {
        pieceBitboard = &blackRooks;
        piece = 'r';
    } else if (blackQueens & sourceBit) {
        pieceBitboard = &blackQueens;
        piece = 'q';
    } else if (blackKing & sourceBit) {
        pieceBitboard = &blackKing;
        piece = 'k';
    }

    if (!pieceBitboard) {
        throw std::invalid_argument("No piece found on the source square.");
    }

    // Update the bitboards
    *pieceBitboard &= ~sourceBit; // Remove piece from source
    *pieceBitboard |= destBit;   // Place piece on destination
}