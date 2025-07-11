#include <gtest/gtest.h>
#include "board.hpp"
#include "movegen.hpp"
#include "engine.hpp"
#include "attacks.hpp"

TEST(EngineEval, MaterialBalance) {
    init_attacks();
    Board b; b.bitboards.fill(0ULL);
    set_bit(b.bitboards[board_index(WHITE,KING)], sq_index('e','1'));
    set_bit(b.bitboards[board_index(BLACK,KING)], sq_index('e','8'));
    set_bit(b.bitboards[board_index(WHITE,QUEEN)], sq_index('d','1'));
    b.recompute_occupancy();
    int eval = Engine::evaluate(b);
    EXPECT_GT(eval, 800); // queen advantage should be large
}

TEST(EngineSearch, CaptureRook) {
    init_attacks();
    Board b; b.bitboards.fill(0ULL);
    set_bit(b.bitboards[board_index(WHITE,KING)], sq_index('e','1'));
    set_bit(b.bitboards[board_index(WHITE,QUEEN)], sq_index('e','2'));
    set_bit(b.bitboards[board_index(BLACK,KING)], sq_index('e','8'));
    set_bit(b.bitboards[board_index(BLACK,ROOK)], sq_index('e','5'));
    b.sideToMove = WHITE;
    b.recompute_occupancy();
    auto res = Engine::search(b,2);
    Move expected = parse_move("e2e5", b);
    EXPECT_EQ(res.bestMove.from, expected.from);
    EXPECT_EQ(res.bestMove.to, expected.to);
}