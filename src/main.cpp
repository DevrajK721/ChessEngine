#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>   // for std::exit
#include "bitboard.h" // Ensure this declares Bitboard, etc.

static void waitForUserConfirmation() {
    std::cout << "Press \033[93mY\033[0m to continue (anything else to quit): ";
    std::string input;
    std::getline(std::cin, input);
    if (input.empty() || (input[0] != 'Y' && input[0] != 'y')) {
        std::cout << "\nAborting test sequence.\n";
        std::exit(0);
    }
}

static void displayScenarioInfo() {
    std::cout << "\033[94mWelcome to the Chess Engine Test!\033[0m\n\n"
              << "We will run a series of 4-character moves that demonstrate:\n"
              << "  - En passant (White plays e5d6)\n"
              << "  - Castling (Black e8g8, White e1g1)\n"
              << "  - Promotion (White h7h8 with promotion piece 'Q')\n\n"
              << "After each move, the board will be shown, and you must press\n"
              << "'Y' to continue. If you press anything else, the program ends,\n"
              << "so you can pause mid-test.\n\n";
}

int main() {
    Bitboard board;
    board.initialize(); // Standard chess opening position, White to move

    displayScenarioInfo();

    // Move list: each is a 4-char move string plus a promotion piece
    // (use ' ' if not a promotion). This scenario sets up e-pawn pushes
    // to allow e5d6 en passant, some normal development, castling, and
    // eventually a White pawn on h7 that promotes on h8.
    //
    // Sequence:
    //   1.  e2e4 (White)
    //   2.  c7c5 (Black)
    //   3.  e4e5 (White)
    //   4.  d7d5 (Black)
    //   5.  e5d6 (White, en passant capture)
    //   6.  b8c6 (Black)
    //   7.  g1f3 (White)
    //   8.  g8f6 (Black)
    //   9.  f1e2 (White)
    //   10. e8g8 (Black castles kingside)
    //   11. e1g1 (White castles kingside)
    //   12. a7a6 (Black)
    //   13. h2h4 (White)
    //   14. h7h5 (Black)
    //   15. h4h5 (White)
    //   16. a6a5 (Black)
    //   17. h5h6 (White)
    //   18. a5a4 (Black)
    //   19. h6h7 (White)
    //   20. g7g6 (Black)
    //   21. h7h8 (White promotes to Queen)
    //
    //   Note: We rely on your engine's internal logic to allow the en passant
    //   capture on e5d6 if the position is correct after moves 1-4.

    std::vector<std::pair<std::string, char>> moves = {
        {"e2e4", ' '},  // 1. White
        {"c7c5", ' '},  // 2. Black
        {"e4e5", ' '},  // 3. White
        {"d7d5", ' '},  // 4. Black
        {"e5d6", ' '},  // 5. White (en passant)
        {"b8c6", ' '},  // 6. Black
        {"g1f3", ' '},  // 7. White
        {"g8f6", ' '},  // 8. Black
        {"f1e2", ' '},  // 9. White
        {"e8g8", ' '},  // 10. Black castles kingside
        {"e1g1", ' '},  // 11. White castles kingside
        {"a7a6", ' '},  // 12. Black
        {"h2h4", ' '},  // 13. White
        {"h7h5", ' '},  // 14. Black
        {"h4h5", ' '},  // 15. White
        {"a6a5", ' '},  // 16. Black
        {"h5h6", ' '},  // 17. White
        {"a5a4", ' '},  // 18. Black
        {"h6h7", ' '},  // 19. White
        {"g7g6", ' '},  // 20. Black
        {"h7h8", 'Q'}   // 21. White promotes to Queen
    };

    bool whiteToMove = true;  // Standard chess starts with White

    for (size_t i = 0; i < moves.size(); i++) {
        const auto& mv = moves[i];
        std::cout << "\nMove " << (i + 1)
                  << (whiteToMove ? " (White): " : " (Black): ")
                  << mv.first;

        if (mv.second != ' ') {
            std::cout << " with promotion to " << mv.second;
        }
        std::cout << "\n";

        // Execute the move with promotion piece if specified
        board.makeMove(mv.first, mv.second);

        // Display the board
        std::cout << board.displayBoard() << "\n";

        // Check if the opponent has moves left => might be checkmate or stalemate
        auto possibleMoves = board.generateLegalMoves(!whiteToMove);
        if (possibleMoves.empty()) {
            // If no moves, check if it's checkmate or stalemate
            if (board.isKingInCheck(!whiteToMove)) {
                std::cout << (whiteToMove ? "White" : "Black")
                          << " wins by checkmate!\n";
            } else {
                std::cout << "Stalemate!\n";
            }
            break;
        }

        // Wait for user to press Y or abort
        waitForUserConfirmation();

        // Switch turn
        whiteToMove = !whiteToMove;
    }

    std::cout << "\nEnd of the test sequence.\n";
    return 0;
}
