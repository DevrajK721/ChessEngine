#include "bitboard.h"
#include <iostream>
#include <bitset>

int main() {
    Bitboard board;
    board.initialize();

    std::cout << "Initial Chessboard:\n";
    std::cout << board.displayBoard() << std::endl;

    try {
        // Testing Sequence for En Passant, Pawn Promotion and Casling for both white and Black
        std::cout << "Playing e2e4" << std::endl;
        board.makeMove("e2e4"); // White pawn to e4
        std::cout << "Playing c7c5" << std::endl;
        board.makeMove("c7c5"); // Black pawn to c5
        std::cout << "Playing f1b5" << std::endl;
        board.makeMove("f1b5"); // White bishop to b5
        std::cout << "Playing e7e6" << std::endl;
        board.makeMove("e7e6"); // Black pawn to e6
        std::cout << "Playing e4e5" << std::endl;
        board.makeMove("e4e5"); // White pawn to e5
        std::cout << "Playing f7f5" << std::endl;
        board.makeMove("f7f5"); // Black pawn to f5
        std::cout << "Testing En Passant for White" << std::endl;
        board.makeMove("e5f6"); // White pawn captures black pawn on f6
        std::cout << board.displayBoard() << std::endl;
        std::cout << "Playing c5c4" << std::endl;
        board.makeMove("c5c4"); // Black pawn to c4
        std::cout << "Playing d2d4" << std::endl;
        board.makeMove("d2d4"); // White pawn to d4
        std::cout << "Testing En Passant for Black" << std::endl;
        board.makeMove("c4d3"); // Black pawn captures white pawn on d3
        std::cout << board.displayBoard() << std::endl;
        std::cout << "Playing g1f3" << std::endl;
        board.makeMove("g1f3"); // White knight to f3
        std::cout << "Playing g8f6" << std::endl;
        board.makeMove("g8f6"); // Black knight to f6
        std::cout << "Testing White King Side Castling" << std::endl;
        board.makeMove("e1g1"); // White king side castling
        std::cout << board.displayBoard() << std::endl;
        std::cout << "Playing f8b4" << std::endl;
        board.makeMove("f8b4"); // Black bishop to b4
        std::cout << "Playing b1c3" << std::endl;
        board.makeMove("b1c3"); // White knight to c3
        std::cout << "Testing Black King Side Castling" << std::endl;
        board.makeMove("e8g8"); // Black king side castling
        std::cout << board.displayBoard() << std::endl;
        std::cout << "Playing b5d7" << std::endl;
        board.makeMove("b5d7"); // White bishop to d7
        std::cout << "Playing d3c2" << std::endl;
        board.makeMove("d3c2"); // Black pawn to c2
        std::cout << "Playing c1g5" << std::endl;
        board.makeMove("c1g5"); // White bishop to g5
        std::cout << "Testing Pawn Promotion for Black" << std::endl;
        board.makeMove("c2d1", 'N');
        std::cout << board.displayBoard() << std::endl;
        std::cout << "Ending Testing Sequence, note any bugs or errors" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
