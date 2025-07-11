#include "board.hpp"
#include "movegen.hpp"
#include "attacks.hpp"
#include <iostream>
#include <string>

int main() {
    init_attacks();
    Board board;
    board.init_startpos();

    while (true) {
        display_board(board);
        auto legal = generate_legal_moves(board);

        if (legal.empty()) {
            Color enemy = board.sideToMove == WHITE ? BLACK : WHITE;
            int ksq = board.king_square(board.sideToMove);
            if (board.is_square_attacked(ksq, enemy)) {
                std::cout << (enemy == WHITE ? "White" : "Black")
                          << " wins by checkmate!\n";
            } else {
                std::cout << "Stalemate!\n";
            }
            break;
        }

        std::cout << (board.sideToMove == WHITE ? "White" : "Black")
                  << " to move: ";
        std::string input;
        if (!(std::cin >> input))
            break;
        if (input == "quit" || input == "exit")
            break;

        Move m = parse_move(input, board);
        bool found = false;
        for (const auto &lm : legal) {
            if (lm.from == m.from && lm.to == m.to && lm.promotion == m.promotion &&
                lm.isCastling == m.isCastling && lm.isEnPassant == m.isEnPassant) {
                m = lm;
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "Illegal move, try again.\n";
            continue;
        }

        make_move(board, m);
    }
}

