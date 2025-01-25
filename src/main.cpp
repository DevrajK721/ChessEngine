#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>   
#include <limits>      
#include "bitboard.h"   
#include "move.h"
#include "evaluation.h"
#include "zobrist.h"
#include "search.h"     
#include <bitset>

// ----------------------------------------------------------------------
// Helper to prompt user for an integer (the search depth).
// ----------------------------------------------------------------------
int askSearchDepth() {
    std::cout << "Enter desired search depth (e.g. 4, 5, 6...): ";
    int depth;
    std::cin >> depth;
    // Clean up leftover newline
    if (std::cin.fail() || depth < 1) {
        depth = 4;
        std::cin.clear();
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return depth;
}

// ----------------------------------------------------------------------
// Displays the top 5 moves for the current side, using the chosen search depth
// We assume your "Search" class has a method "getAllMovesSorted(Bitboard&, int, bool)" 
// that does a root search and returns a sorted list of (move, score).
// We'll just show the top 5 from that sorted list.
// ----------------------------------------------------------------------
void displayBestMoves(Bitboard& board, bool whiteToMove, int depth) {
    std::vector<MoveScore> allMoves = Search::getAllMovesSorted(board, depth, whiteToMove);

    std::cout << "\nTop 5 moves (depth " << depth << "):\n";
    for (size_t i = 0; i < allMoves.size() && i < 5; i++) {
        auto& ms = allMoves[i];
        // Score sign depends on perspective, so you can interpret 
        // it as "positive good for White, negative good for Black".
        std::cout << "   " << (i+1) << ". " << ms.move 
                  << "   score: " << ms.score << "\n";
    }
    std::cout << "\n";
}

// ----------------------------------------------------------------------
// The main game loop: 
// 1) Display board
// 2) Show best 5 moves for side to move
// 3) Prompt user for a move or "exit"
// 4) Make the move, check game status
// 5) Switch side
// ----------------------------------------------------------------------
int main() {
    Zobrist::initZobrist(); // Initialize Zobrist hashing
    Zobrist::initTransTable();
    Bitboard board;
    board.initialize(); 

    // Debugging output
    std::cout << "White Pawns: " << std::bitset<64>(board.getWhitePawns()) << "\n";
    std::cout << "Black Pawns: " << std::bitset<64>(board.getBlackPawns()) << "\n"; 

    bool whiteToMove = true;

    // Ask user what depth to search for best moves
    int searchDepth = askSearchDepth();

    // Clear the input buffer in case there's leftover newline
    // (We already did a .ignore, but just to be safe.)
    std::cin.sync(); 
    try {
    while (true) {
        // Display the board
        std::cout << "\n" 
                  << (whiteToMove ? "[White to move]" : "[Black to move]") 
                  << "\n";
        std::cout << board.displayBoard() << "\n";

        // Generate legal moves
        auto legalMoves = board.generateLegalMoves(whiteToMove);
        if (legalMoves.empty()) {
            // No moves => check if it's checkmate or stalemate
            if (board.isKingInCheck(whiteToMove)) {
                std::cout << (whiteToMove ? "Black" : "White")
                          << " wins by checkmate!\n";
            } else {
                std::cout << "Stalemate!\n";
            }
            break;
        }

        // Show the best 5 moves from the engine's perspective
        displayBestMoves(board, whiteToMove, searchDepth);

        // Prompt user for a move
        std::string userMove;
        std::cout << "Enter your move in 4-char format (e.g. e2e4) or 'exit' to quit: ";
        std::getline(std::cin, userMove);

        // Check for exit
        if (userMove == "exit") {
            std::cout << "Exiting game.\n";
            break;
        }
        if (userMove.size() != 4) {
            std::cout << "Invalid move format. Please try again.\n";
            continue;
        }

        // Attempt to make the move
        bool valid = false;
        // We'll check if userMove is in the legalMoves
        for (auto &m : legalMoves) {
            if (m.move == userMove) {
                // Make the move on the board
                board.makeMove(userMove, ' '); 
                valid = true;
                break;
            }
        }
        if (!valid) {
            std::cout << "That move wasn't found in the legal moves list. Try again.\n";
            continue;
        }

        // If we made a valid move, check if the other side has moves or if game ended
        auto opponentMoves = board.generateLegalMoves(!whiteToMove);
        if (opponentMoves.empty()) {
            if (board.isKingInCheck(!whiteToMove)) {
                std::cout << (whiteToMove ? "White" : "Black")
                          << " wins by checkmate!\n";
            } else {
                std::cout << "Stalemate!\n";
            }
            break;
        }

        // Switch side
        whiteToMove = !whiteToMove;
    }} catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}