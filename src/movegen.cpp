#include "movegen.hpp"
#include "attacks.hpp"
#include <cassert>

static int square_from_string(const std::string &s) {
    assert(s.size() == 2);
    return sq_index(s[0]-'a', s[1]-'1');
}

Move parse_move(const std::string &uci, const Board &board) {
    Move m{};
    m.from = square_from_string(uci.substr(0,2));
    m.to = square_from_string(uci.substr(2,2));
    m.promotion = NO_PIECE;
    m.isDoublePush = false;
    m.isEnPassant = false;
    m.isCastling = false;

    Color c;
    m.piece = board.piece_at(m.from, c);
    Color captured_color;
    m.captured = board.piece_at(m.to, captured_color);

    if(m.piece == PAWN && std::abs(m.to - m.from) == 16) {
        m.isDoublePush = true;
    }
    if(m.piece == KING && std::abs(m.to - m.from) == 2) {
        m.isCastling = true;
    }
    if(m.piece == PAWN && m.to == board.enPassantSquare && m.captured == NO_PIECE) {
        m.isEnPassant = true;
        m.captured = PAWN;
    }
    if(uci.size() == 5) {
        switch(uci[4]) {
            case 'q': m.promotion = QUEEN; break;
            case 'r': m.promotion = ROOK; break;
            case 'b': m.promotion = BISHOP; break;
            case 'n': m.promotion = KNIGHT; break;
            default: m.promotion = QUEEN; break;
        }
    }
    return m;
}

Undo make_move(Board &b, const Move &m) {
    Undo u{b.enPassantSquare, b.w_can_castle_k, b.w_can_castle_q,
            b.b_can_castle_k, b.b_can_castle_q, NO_PIECE};

    Color mover = b.sideToMove;
    Color capColor;
    PieceType pieceAtDest = b.piece_at(m.to, capColor);

    if(m.isEnPassant) {
        int capSq = m.to + (mover == WHITE ? -8 : 8);
        Color dummy;
        pieceAtDest = b.piece_at(capSq, dummy);
        clear_bit(b.bitboards[board_index((Color)(-mover), PAWN)], capSq);
        u.captured = PAWN;
    } else if(pieceAtDest != NO_PIECE) {
        clear_bit(b.bitboards[board_index(capColor, pieceAtDest)], m.to);
        u.captured = pieceAtDest;
    }

    // move piece
    clear_bit(b.bitboards[board_index(mover, m.piece)], m.from);

    PieceType finalPiece = m.promotion != NO_PIECE ? m.promotion : m.piece;
    set_bit(b.bitboards[board_index(mover, finalPiece)], m.to);
    if(m.promotion != NO_PIECE)
        ; // pawn bitboard bit already cleared above

    if(m.isCastling) {
        if(m.to == sq_index('g','1')) { // white king side
            clear_bit(b.bitboards[board_index(WHITE, ROOK)], sq_index('h','1'));
            set_bit(b.bitboards[board_index(WHITE, ROOK)], sq_index('f','1'));
        } else if(m.to == sq_index('c','1')) {
            clear_bit(b.bitboards[board_index(WHITE, ROOK)], sq_index('a','1'));
            set_bit(b.bitboards[board_index(WHITE, ROOK)], sq_index('d','1'));
        } else if(m.to == sq_index('g','8')) {
            clear_bit(b.bitboards[board_index(BLACK, ROOK)], sq_index('h','8'));
            set_bit(b.bitboards[board_index(BLACK, ROOK)], sq_index('f','8'));
        } else if(m.to == sq_index('c','8')) {
            clear_bit(b.bitboards[board_index(BLACK, ROOK)], sq_index('a','8'));
            set_bit(b.bitboards[board_index(BLACK, ROOK)], sq_index('d','8'));
        }
    }

    b.enPassantSquare = -1;
    if(m.isDoublePush) {
        b.enPassantSquare = m.from + (mover==WHITE?8:-8);
    }

    if(m.piece == KING) {
        if(mover==WHITE) {
            b.w_can_castle_k = b.w_can_castle_q = false;
        } else {
            b.b_can_castle_k = b.b_can_castle_q = false;
        }
    }
    if(m.piece == ROOK) {
        if(mover==WHITE) {
            if(m.from == sq_index('h','1')) b.w_can_castle_k = false;
            if(m.from == sq_index('a','1')) b.w_can_castle_q = false;
        } else {
            if(m.from == sq_index('h','8')) b.b_can_castle_k = false;
            if(m.from == sq_index('a','8')) b.b_can_castle_q = false;
        }
    }

    b.sideToMove = (Color)(-mover);
    b.recompute_occupancy();
    return u;
}

void undo_move(Board &b, const Move &m, const Undo &u) {
    Color mover = (Color)(-b.sideToMove); // side who made the move
    b.sideToMove = mover;
    b.enPassantSquare = u.ep_square;
    b.w_can_castle_k = u.w_can_castle_k;
    b.w_can_castle_q = u.w_can_castle_q;
    b.b_can_castle_k = u.b_can_castle_k;
    b.b_can_castle_q = u.b_can_castle_q;

    clear_bit(b.bitboards[board_index(mover, m.promotion!=NO_PIECE?m.promotion:m.piece)], m.to);
    set_bit(b.bitboards[board_index(mover, m.piece)], m.from);

    if(m.isCastling) {
        if(m.to == sq_index('g','1')) {
            clear_bit(b.bitboards[board_index(WHITE, ROOK)], sq_index('f','1'));
            set_bit(b.bitboards[board_index(WHITE, ROOK)], sq_index('h','1'));
        } else if(m.to == sq_index('c','1')) {
            clear_bit(b.bitboards[board_index(WHITE, ROOK)], sq_index('d','1'));
            set_bit(b.bitboards[board_index(WHITE, ROOK)], sq_index('a','1'));
        } else if(m.to == sq_index('g','8')) {
            clear_bit(b.bitboards[board_index(BLACK, ROOK)], sq_index('f','8'));
            set_bit(b.bitboards[board_index(BLACK, ROOK)], sq_index('h','8'));
        } else if(m.to == sq_index('c','8')) {
            clear_bit(b.bitboards[board_index(BLACK, ROOK)], sq_index('d','8'));
            set_bit(b.bitboards[board_index(BLACK, ROOK)], sq_index('a','8'));
        }
    }

    if(u.captured != NO_PIECE) {
        int capSq = m.to;
        if(m.isEnPassant) capSq = m.to + (mover==WHITE? -8:8);
        set_bit(b.bitboards[board_index((Color)(-mover), u.captured)], capSq);
    }

    b.recompute_occupancy();
}

static void add_move(std::vector<Move> &moves, Move m, const Board &b) {
    if(m.piece == PAWN && (m.to < 8 || m.to >= 56)) {
        // generate promotions to queen only to keep things simple
        m.promotion = QUEEN;
        m.isDoublePush = false;
    }
    moves.push_back(m);
}

static void generate_pseudo(const Board &b, std::vector<Move> &moves) {
    Color us = b.sideToMove;
    Color them = (Color)(-us);
    U64 usOcc = (us==WHITE)?b.whiteOccupancy:b.blackOccupancy;
    U64 themOcc = (us==WHITE)?b.blackOccupancy:b.whiteOccupancy;

    // Pawns
    U64 pawns = b.bitboards[board_index(us, PAWN)];
    while(pawns) {
        int from = pop_lsb(pawns);
        int step = us==WHITE?8:-8;
        int to = from + step;
        if(!(b.bothOccupancy & (1ULL<<to))) {
            Move m{from,to,PAWN,NO_PIECE,NO_PIECE,false,false,false};
            add_move(moves,m,b);
            int startRank = us==WHITE?1:6;
            if(from/8==startRank && !(b.bothOccupancy & (1ULL<<(to+step)))) {
                Move dm{from,to+step,PAWN,NO_PIECE,NO_PIECE,true,false,false};
                add_move(moves,dm,b);
            }
        }
        U64 caps = pawnAttacks[us==WHITE?0:1][from] & themOcc;
        while(caps) {
            int capSq = pop_lsb(caps);
            Move m{from,capSq,PAWN,NO_PIECE,NO_PIECE,false,false,false};
            add_move(moves,m,b);
        }
        if(b.enPassantSquare != -1 && (pawnAttacks[us==WHITE?0:1][from] & (1ULL<<b.enPassantSquare))) {
            Move m{from,b.enPassantSquare,PAWN,PAWN,NO_PIECE,false,true,false};
            add_move(moves,m,b);
        }
    }

    // Knights
    U64 knights = b.bitboards[board_index(us, KNIGHT)];
    while(knights) {
        int from = pop_lsb(knights);
        U64 targets = knightAttacks[from] & ~usOcc;
        while(targets) {
            int to = pop_lsb(targets);
            Move m{from,to,KNIGHT,NO_PIECE,NO_PIECE,false,false,false};
            add_move(moves,m,b);
        }
    }

    // Bishops
    U64 bishops = b.bitboards[board_index(us,BISHOP)];
    while(bishops) {
        int from = pop_lsb(bishops);
        U64 targets = bishop_attacks(from,b.bothOccupancy) & ~usOcc;
        while(targets) {
            int to = pop_lsb(targets);
            Move m{from,to,BISHOP,NO_PIECE,NO_PIECE,false,false,false};
            add_move(moves,m,b);
        }
    }

    // Rooks
    U64 rooks = b.bitboards[board_index(us,ROOK)];
    while(rooks) {
        int from = pop_lsb(rooks);
        U64 targets = rook_attacks(from,b.bothOccupancy) & ~usOcc;
        while(targets) {
            int to = pop_lsb(targets);
            Move m{from,to,ROOK,NO_PIECE,NO_PIECE,false,false,false};
            add_move(moves,m,b);
        }
    }

    // Queens
    U64 queens = b.bitboards[board_index(us,QUEEN)];
    while(queens) {
        int from = pop_lsb(queens);
        U64 targets = queen_attacks(from,b.bothOccupancy) & ~usOcc;
        while(targets) {
            int to = pop_lsb(targets);
            Move m{from,to,QUEEN,NO_PIECE,NO_PIECE,false,false,false};
            add_move(moves,m,b);
        }
    }

    // King
    U64 kingBB = b.bitboards[board_index(us,KING)];
    if(kingBB) {
        int from = __builtin_ctzll(kingBB);
        U64 targets = kingAttacks[from] & ~usOcc;
        while(targets) {
            int to = pop_lsb(targets);
            Move m{from,to,KING,NO_PIECE,NO_PIECE,false,false,false};
            add_move(moves,m,b);
        }
    }

    // Castling
    if(us==WHITE) {
        if(b.w_can_castle_k && !(b.bothOccupancy & ((1ULL<<sq_index('f','1'))|(1ULL<<sq_index('g','1'))))) {
            if(!b.is_square_attacked(sq_index('e','1'), BLACK) &&
               !b.is_square_attacked(sq_index('f','1'), BLACK) &&
               !b.is_square_attacked(sq_index('g','1'), BLACK)) {
                Move m{sq_index('e','1'), sq_index('g','1'), KING, NO_PIECE, NO_PIECE, false,false,true};
                moves.push_back(m);
            }
        }
        if(b.w_can_castle_q && !(b.bothOccupancy & ((1ULL<<sq_index('b','1'))|(1ULL<<sq_index('c','1'))|(1ULL<<sq_index('d','1'))))) {
            if(!b.is_square_attacked(sq_index('e','1'), BLACK) &&
               !b.is_square_attacked(sq_index('d','1'), BLACK) &&
               !b.is_square_attacked(sq_index('c','1'), BLACK)) {
                Move m{sq_index('e','1'), sq_index('c','1'), KING, NO_PIECE, NO_PIECE, false,false,true};
                moves.push_back(m);
            }
        }
    } else {
        if(b.b_can_castle_k && !(b.bothOccupancy & ((1ULL<<sq_index('f','8'))|(1ULL<<sq_index('g','8'))))) {
            if(!b.is_square_attacked(sq_index('e','8'), WHITE) &&
               !b.is_square_attacked(sq_index('f','8'), WHITE) &&
               !b.is_square_attacked(sq_index('g','8'), WHITE)) {
                Move m{sq_index('e','8'), sq_index('g','8'), KING, NO_PIECE, NO_PIECE, false,false,true};
                moves.push_back(m);
            }
        }
        if(b.b_can_castle_q && !(b.bothOccupancy & ((1ULL<<sq_index('b','8'))|(1ULL<<sq_index('c','8'))|(1ULL<<sq_index('d','8'))))) {
            if(!b.is_square_attacked(sq_index('e','8'), WHITE) &&
               !b.is_square_attacked(sq_index('d','8'), WHITE) &&
               !b.is_square_attacked(sq_index('c','8'), WHITE)) {
                Move m{sq_index('e','8'), sq_index('c','8'), KING, NO_PIECE, NO_PIECE, false,false,true};
                moves.push_back(m);
            }
        }
    }
}

std::vector<Move> generate_legal_moves(Board &b) {
    std::vector<Move> pseudo;
    generate_pseudo(b, pseudo);
    std::vector<Move> legal;
    for(const Move &m : pseudo) {
        Undo u = make_move(b, m);
        int ksq = b.king_square((Color)(-b.sideToMove));
        bool inCheck = b.is_square_attacked(ksq, b.sideToMove);
        if(!inCheck) legal.push_back(m);
        undo_move(b, m, u);
    }
    return legal;
}