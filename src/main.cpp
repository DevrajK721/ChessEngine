#include "bitboard.h"
#include <iostream>
#include <bitset>

int main() {
    Bitboard board;
    board.initialize();

    std::cout << "Initial Chessboard:\n";
    std::cout << board.displayBoard() << std::endl;
    std::cout << "Initial Black Queens: " << std::bitset<64>(board.getBlackQueens()) << std::endl;
    std::cout << "Initial Black Bishops: " << std::bitset<64>(board.getBlackBishops()) << std::endl;

    try {
        // Promotion to Queen
        board.makeMove("e2e8", 'Q');
        std::cout << board.displayBoard() << std::endl;

        // Reset the board
        board.initialize();

        board.makeMove("b7b1", 'B');
        std::cout << board.displayBoard() << std::endl;

        board.initialize();
        board.makeMove("a2a8", 'R');
        std::cout << board.displayBoard() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
