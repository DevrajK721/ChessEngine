#include <gtest/gtest.h>
#include <vector>
#include "board.hpp"
#include "bitboard.hpp"

// Helper to build a bitboard from a list of squares
static U64 bb_from_squares(const std::vector<int>& squares) {
    U64 b = 0ULL;
    for (int sq : squares) {
        set_bit(b, sq);
    }
    return b;
}

TEST(BoardInitTest, StartingPosition) {
    Board board;
    board.init_startpos();

    EXPECT_EQ(board.bitboards[board_index(WHITE, PAWN)],
              bb_from_squares({sq_index('a','2'), sq_index('b','2'), sq_index('c','2'),
                               sq_index('d','2'), sq_index('e','2'), sq_index('f','2'),
                               sq_index('g','2'), sq_index('h','2')}));

    EXPECT_EQ(board.bitboards[board_index(BLACK, PAWN)],
              bb_from_squares({sq_index('a','7'), sq_index('b','7'), sq_index('c','7'),
                               sq_index('d','7'), sq_index('e','7'), sq_index('f','7'),
                               sq_index('g','7'), sq_index('h','7')}));


    EXPECT_EQ(board.bitboards[board_index(WHITE, KING)],
              bb_from_squares({sq_index('e','1')}));
    EXPECT_EQ(board.bitboards[board_index(BLACK, KING)],
              bb_from_squares({sq_index('e','8')}));

    U64 white_occ = 0ULL;
    U64 black_occ = 0ULL;
    for (int pt = PAWN; pt <= KING; ++pt) {
        white_occ |= board.bitboards[board_index(WHITE, static_cast<PieceType>(pt))];
        black_occ |= board.bitboards[board_index(BLACK, static_cast<PieceType>(pt))];
    }

    EXPECT_EQ(board.whiteOccupancy, white_occ);
    EXPECT_EQ(board.blackOccupancy, black_occ);
    EXPECT_EQ(board.bothOccupancy, (white_occ | black_occ));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}