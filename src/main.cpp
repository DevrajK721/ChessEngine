#include "board.hpp"
#include "movegen.hpp"
#include "attacks.hpp"
#include "engine.hpp"
#include <iostream>
#include <string>

int main() {
    init_attacks();
    Board board;
    board.init_startpos();

    while (true) {
        display_board(board, true);
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
                  << " to move (or 'ai'): ";
        std::string input;
        if (!(std::cin >> input))
            break;
        if (input == "quit" || input == "exit")
            break;

        if (input == "ai") {
            auto res = Engine::search(board,5);
            std::string uci;
            char f1 = 'a' + (res.bestMove.from % 8);
            char r1 = '1' + (res.bestMove.from / 8);
            char f2 = 'a' + (res.bestMove.to % 8);
            char r2 = '1' + (res.bestMove.to / 8);
            uci.push_back(f1); uci.push_back(r1);
            uci.push_back(f2); uci.push_back(r2);
            if (res.bestMove.promotion != NO_PIECE) {
                char p = 'q';
                if(res.bestMove.promotion==ROOK) p='r';
                else if(res.bestMove.promotion==BISHOP) p='b';
                else if(res.bestMove.promotion==KNIGHT) p='n';
                uci.push_back(p);
            }
            std::cout << "Engine plays: " << uci << "\n";
            make_move(board, res.bestMove);
            continue;
        }

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

