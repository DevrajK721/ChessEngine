#include <gtest/gtest.h>
#include "board.hpp"
#include "movegen.hpp"
#include "attacks.hpp"

static bool contains_move(const std::vector<Move>& moves, const std::string& uci, const Board& b){
    Move cmp = parse_move(uci,b);
    for(const auto& m : moves){
        if(m.from==cmp.from && m.to==cmp.to && m.promotion==cmp.promotion && m.isCastling==cmp.isCastling && m.isEnPassant==cmp.isEnPassant)
            return true;
    }
    return false;
}

TEST(MoveGen, StartPosition) {
    init_attacks();
    Board b; b.init_startpos();
    auto moves = generate_legal_moves(b);
    EXPECT_EQ(moves.size(), 20);
}

TEST(MoveGen, KingMoves) {
    init_attacks();
    Board b; b.bitboards.fill(0ULL); b.recompute_occupancy();
    set_bit(b.bitboards[board_index(WHITE,KING)], sq_index('e','1'));
    set_bit(b.bitboards[board_index(BLACK,KING)], sq_index('e','3'));
    b.sideToMove = WHITE; b.recompute_occupancy();
    auto moves = generate_legal_moves(b);
    EXPECT_EQ(moves.size(), 2);
    EXPECT_TRUE(contains_move(moves,"e1d1",b));
    EXPECT_TRUE(contains_move(moves,"e1f1",b));
}

TEST(MoveGen, Checkmate) {
    init_attacks();
    Board b; b.bitboards.fill(0ULL);
    set_bit(b.bitboards[board_index(WHITE,KING)], sq_index('h','1'));
    set_bit(b.bitboards[board_index(BLACK,QUEEN)], sq_index('g','2'));
    set_bit(b.bitboards[board_index(BLACK,ROOK)], sq_index('h','2'));
    set_bit(b.bitboards[board_index(BLACK,KING)], sq_index('a','8'));
    b.sideToMove = WHITE; b.recompute_occupancy();
    auto moves = generate_legal_moves(b);
    EXPECT_EQ(moves.size(), 0);
}

TEST(MoveGen, EnPassant) {
    init_attacks();
    Board b; b.bitboards.fill(0ULL);
    set_bit(b.bitboards[board_index(WHITE,KING)], sq_index('a','1'));
    set_bit(b.bitboards[board_index(BLACK,KING)], sq_index('h','8'));
    set_bit(b.bitboards[board_index(WHITE,PAWN)], sq_index('e','5'));
    set_bit(b.bitboards[board_index(BLACK,PAWN)], sq_index('d','5'));
    b.sideToMove = WHITE;
    b.enPassantSquare = sq_index('d','6');
    b.recompute_occupancy();
    auto moves = generate_legal_moves(b);
    EXPECT_TRUE(contains_move(moves,"e5d6",b));
}

TEST(MoveGen, Stalemate) {
    init_attacks();
    Board b; b.bitboards.fill(0ULL);
    set_bit(b.bitboards[board_index(WHITE,KING)], sq_index('f','6'));
    set_bit(b.bitboards[board_index(WHITE,QUEEN)], sq_index('g','6'));
    set_bit(b.bitboards[board_index(BLACK,KING)], sq_index('h','8'));
    b.sideToMove = BLACK;
    b.recompute_occupancy();
    auto moves = generate_legal_moves(b);
    EXPECT_EQ(moves.size(),0);
}

TEST(MoveGen, CastlingLegal) {
    init_attacks();
    Board b; b.bitboards.fill(0ULL);
    set_bit(b.bitboards[board_index(WHITE,KING)], sq_index('e','1'));
    set_bit(b.bitboards[board_index(WHITE,ROOK)], sq_index('h','1'));
    set_bit(b.bitboards[board_index(BLACK,KING)], sq_index('a','8'));
    b.w_can_castle_k = true;
    b.sideToMove = WHITE;
    b.recompute_occupancy();
    auto moves = generate_legal_moves(b);
    EXPECT_TRUE(contains_move(moves,"e1g1",b));
}

TEST(MoveGen, CastlingIllegal) {
    init_attacks();
    Board b; b.bitboards.fill(0ULL);
    set_bit(b.bitboards[board_index(WHITE,KING)], sq_index('e','1'));
    set_bit(b.bitboards[board_index(WHITE,ROOK)], sq_index('h','1'));
    set_bit(b.bitboards[board_index(WHITE,KNIGHT)], sq_index('g','1'));
    set_bit(b.bitboards[board_index(BLACK,KING)], sq_index('a','8'));
    b.w_can_castle_k = true;
    b.sideToMove = WHITE;
    b.recompute_occupancy();
    auto moves = generate_legal_moves(b);
    EXPECT_FALSE(contains_move(moves,"e1g1",b));
}

TEST(MoveGen, PlaySequence) {
    init_attacks();
    Board b; b.init_startpos();
    std::vector<std::string> seq = {"e2e4","e7e5","g1f3","b8c6","f1c4","f8c5","b1c3","g8f6","e1g1","e8g8"};
    for(const auto& mv : seq){
        auto legal = generate_legal_moves(b);
        ASSERT_TRUE(contains_move(legal,mv,b)) << "Illegal move in sequence: " << mv;
        Move m = parse_move(mv,b);
        // replace with the fully defined move from legal list
        for(const auto& l : legal){
            if(l.from==m.from && l.to==m.to && l.promotion==m.promotion && l.isCastling==m.isCastling && l.isEnPassant==m.isEnPassant){
                m = l; break;
            }
        }
        make_move(b,m);
    }
}