#include <iostream>
#include "bitboard.h"
#include <vector>
#include <cctype>

void displayWelcomeMessage() {
    std::cout << "\033[94mWelcome to Chess!\033[0m\n";
    std::cout << "Type your moves in standard chess notation (e.g., e2e4).\n";
    std::cout << "Type 'moves' to list all legal moves.\n";
    std::cout << "Type 'moves <square>' (e.g., moves e2) to list moves for a specific piece.\n";
    std::cout << "Type 'exit' to quit.\n";
}

int squareToIndex(const std::string& square) {
    if (square.length() != 2 || square[0] < 'a' || square[0] > 'h' || square[1] < '1' || square[1] > '8') {
        return -1;
    }
    int file = square[0] - 'a';
    int rank = square[1] - '1';
    return rank * 8 + file;
}

void displayMovesForSquare(Bitboard& board, bool isWhiteTurn, const std::string& square) {
    int squareIndex = squareToIndex(square);
    if (squareIndex == -1) {
        std::cout << "Invalid square. Please use valid chess notation (e.g., e2).\n";
        return;
    }

    std::vector<Move> legalMoves = board.generateLegalMoves(isWhiteTurn);
    bool found = false;

    std::cout << "\033[93mLegal moves for square " << square << ":\033[0m\n";
    for (const auto& move : legalMoves) {
        if (move.move.substr(0, 2) == square) {
            found = true;
            std::cout << move.move;
            if (move.move.find('x') != std::string::npos) {
                std::cout << " (Capture)";
            } else if (move.move.find('O') != std::string::npos) {
                std::cout << " (Castling)";
            }
            std::cout << "\n";
        }
    }

    if (!found) {
        std::cout << "No legal moves available for this square.\n";
    }
}

int main() {
    Bitboard board;
    board.initialize();

    bool isWhiteTurn = true;
    std::string input;

    displayWelcomeMessage();

    while (true) {
        std::cout << "\n";
        std::cout << (isWhiteTurn ? "\033[97mWhite\033[0m" : "\033[90mBlack\033[0m") << " to move:\n";
        std::cout << board.displayBoard();
        std::cout << "Enter your move: ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        } else if (input.substr(0, 5) == "moves") {
            if (input.length() == 5) {
                auto legalMoves = board.generateLegalMoves(isWhiteTurn);
                std::cout << "\033[92mTotal legal moves: \033[0m" << legalMoves.size() << "\n";
                for (const auto& move : legalMoves) {
                    std::cout << move.move;
                    if (move.move.find('x') != std::string::npos) {
                        std::cout << " (Capture)";
                    } else if (move.move.find('O') != std::string::npos) {
                        std::cout << " (Castling)";
                    }
                    std::cout << "\n";
                }
            } else {
                std::string square = input.substr(6); // Extract square after "moves "
                displayMovesForSquare(board, isWhiteTurn, square);
            }
            continue;
        }

        // Validate and make move
        auto legalMoves = board.generateLegalMoves(isWhiteTurn);
        bool validMove = false;

        for (const auto& move : legalMoves) {
            if (move.move == input) {
                board.makeMove(input);
                validMove = true;
                break;
            }
        }

        if (!validMove) {
            std::cout << "\033[91mInvalid move. Try again.\033[0m\n";
            continue;
        }

        // Check for game end conditions
        if (board.generateLegalMoves(!isWhiteTurn).empty()) {
            if (board.isKingInCheck(!isWhiteTurn)) {
                std::cout << (isWhiteTurn ? "White" : "Black") << " wins by checkmate!\n";
            } else {
                std::cout << "Stalemate!\n";
            }
            break;
        }

        // Switch turn
        isWhiteTurn = !isWhiteTurn;
    }

    return 0;
}
