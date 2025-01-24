// bitboard_display.cpp
#include "bitboard.h"
#include <string>
#include <iostream>

std::string Bitboard::displayBoard() const {
    const std::string reset = "\033[0m";
    const std::string red = "\033[91m";    // Red for rank and file labels
    const std::string white = "\033[97m"; // White for borders and separators
    const std::string blackPiece = "\033[90m"; // Light gray for black pieces
    const std::string whitePiece = "\033[37m"; // White for white pieces

    std::string board;

    // Top Border
    board += white + "   +---+---+---+---+---+---+---+---+\n" + reset;

    for (int rank = 7; rank >= 0; --rank) {
        // Add rank number in red
        board += red + " " + std::to_string(rank + 1) + " " + reset;

        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            uint64_t bit = 1ULL << index;

            board += white + "| " + reset;

            // Dynamically check all bitboards to determine the piece
            if (whiteKing & bit) board += whitePiece + "K " + reset;
            else if (whiteQueens & bit) board += whitePiece + "Q " + reset;
            else if (whiteRooks & bit) board += whitePiece + "R " + reset;
            else if (whiteBishops & bit) board += whitePiece + "B " + reset;
            else if (whiteKnights & bit) board += whitePiece + "N " + reset;
            else if (whitePawns & bit) board += whitePiece + "P " + reset;
            else if (blackKing & bit) board += blackPiece + "k " + reset;
            else if (blackQueens & bit) board += blackPiece + "q " + reset;
            else if (blackRooks & bit) board += blackPiece + "r " + reset;
            else if (blackBishops & bit) board += blackPiece + "b " + reset;
            else if (blackKnights & bit) board += blackPiece + "n " + reset;
            else if (blackPawns & bit) board += blackPiece + "p " + reset;
            else board += "  "; // Empty square
        }

        board += white + "|\n" + reset; // Close row with a border

        // Add rank number again in red
        board += white + "   +---+---+---+---+---+---+---+---+\n" + reset;
    }

    // Add file letters in red
    board += red + "     a   b   c   d   e   f   g   h\n" + reset;

    return board;
}

// basic display
// std::string Bitboard::displayBoard() const {
//     std::string board;
//     for (int rank = 7; rank >= 0; --rank) {
//         board += std::to_string(rank + 1) + " "; // Add rank number
//         for (int file = 0; file < 8; ++file) {
//             int index = rank * 8 + file;
//             uint64_t bit = 1ULL << index;
//
//             // Dynamically check all bitboards to determine the piece
//             if (whiteKing & bit) board += "K ";
//             else if (whiteQueens & bit) board += "Q ";
//             else if (whiteRooks & bit) board += "R ";
//             else if (whiteBishops & bit) board += "B ";
//             else if (whiteKnights & bit) board += "N ";
//             else if (whitePawns & bit) board += "P ";
//             else if (blackKing & bit) board += "k ";
//             else if (blackQueens & bit) board += "q ";
//             else if (blackRooks & bit) board += "r ";
//             else if (blackBishops & bit) board += "b ";
//             else if (blackKnights & bit) board += "n ";
//             else if (blackPawns & bit) board += "p ";
//             else board += ". "; // Empty square
//         }
//         board += std::to_string(rank + 1) + "\n"; // Add rank number again
//     }
//     board += "  a b c d e f g h\n"; // Add file letters
//     return board;
// }

