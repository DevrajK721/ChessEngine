#ifndef PIECE_HPP
#define PIECE_HPP

// Setting up index scheme 
enum Color {
    WHITE = 1, 
    BLACK = -1,
    BOTH = 0
};

enum PieceType {
    PAWN = 1, 
    KNIGHT = 2,
    BISHOP = 3,
    ROOK = 4,
    QUEEN = 5,
    KING = 6,
    NO_PIECE = 0
};

inline int piece_index(Color c, PieceType pt) {
    return pt * c; 
}

inline int board_index(Color c, PieceType pt) {
    return (c == WHITE ? (pt - 1) : ((pt - 1) + 6));
}

#endif // PIECE_HPP