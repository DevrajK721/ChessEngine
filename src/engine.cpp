#include "engine.hpp"
#include "attacks.hpp"
#include <array>
#include <limits>
#include <unordered_map>

namespace Engine {

static const int INF = 100000;

EvalParams evalParams;

// Piece values
static const int pieceValue[6] = {
    100,   // pawn
    280,   // knight
    320,   // bishop
    479,   // rook
    929,   // queen
    60000  // king (very high to discourage losing it)
};

// Simple piece-square tables (from white perspective) - Sunfish Tuned
// pawn
static const std::array<int,64> pawnTable = {
      0,   0,   0,   0,   0,   0,   0,   0,
     78,  83,  86,  73, 102,  82,  85,  90,
      7,  29,  21,  44,  40,  31,  44,   7,
    -17,  16,  -2,  15,  14,   0,  15, -13,
    -26,   3,  10,   9,   6,   1,   0, -23,
    -22,   9,   5, -11, -10,  -2,   3, -19,
    -31,   8,  -7, -37, -36, -14,   3, -31,
      0,   0,   0,   0,   0,   0,   0,   0
};

// knight
static const std::array<int,64> knightTable = {
    -66, -53, -75, -75, -10, -55, -58, -70,
     -3,  -6, 100, -36,   4,  62,  -4, -14,
     10,  67,   1,  74,  73,  27,  62,  -2,
     24,  24,  45,  37,  33,  41,  25,  17,
     -1,   5,  31,  21,  22,  35,   2,   0,
    -18,  10,  13,  22,  18,  15,  11, -14,
    -23, -15,   2,   0,   2,   0, -23, -20,
    -74, -23, -26, -24, -19, -35, -22, -69
};

// bishop
static const std::array<int,64> bishopTable = {
    -59, -78, -82, -76, -23,-107, -37, -50,
    -11,  20,  35, -42, -39,  31,   2, -22,
     -9,  39, -32,  41,  52, -10,  28, -14,
     25,  17,  20,  34,  26,  25,  15,  10,
     13,  10,  17,  23,  17,  16,   0,   7,
     14,  25,  24,  15,   8,  25,  20,  15,
     19,  20,  11,   6,   7,   6,  20,  16,
     -7,   2, -15, -12, -14, -15, -10, -10
};

// rook
static const std::array<int,64> rookTable = {
     35,  29,  33,   4,  37,  33,  56,  50,
     55,  29,  56,  67,  55,  62,  34,  60,
     19,  35,  28,  33,  45,  27,  25,  15,
      0,   5,  16,  13,  18,  -4,  -9,  -6,
    -28, -35, -16, -21, -13, -29, -46, -30,
    -42, -28, -42, -25, -25, -35, -26, -46,
    -53, -38, -31, -26, -29, -43, -44, -53,
    -30, -24, -18,   5,  -2, -18, -31, -32
};

// queen
static const std::array<int,64> queenTable = {
      6,   1,  -8, -104,  69,  24,  88,  26,
     14,  32,  60,  -10,  20,  76,  57,  24,
     -2,  43,  32,   60,  72,  63,  43,   2,
      1, -16,  22,   17,  25,  20, -13,  -6,
    -14, -15,  -2,   -5,  -1, -10, -20, -22,
    -30,  -6, -13,  -11, -16, -11, -16, -27,
    -36, -18,   0,  -19, -15, -15, -21, -38,
    -39, -30, -31,  -13, -31, -36, -34, -42
};

// king (middle game)
static const std::array<int,64> kingTable = {
      4,  54,  47,  -99, -99,  60,  83, -62,
    -32,  10,  55,   56,  56,  55,  10,   3,
    -62,  12, -57,   44, -67,  28,  37, -31,
    -55,  50,  11,   -4, -19,  13,   0, -49,
    -55, -43, -52,  -28, -51, -47,  -8, -50,
    -47, -42, -43,  -79, -64, -32, -29, -32,
     -4,   3, -14,  -50, -57, -18,  13,   4,
     17,  30,  -3,  -14,   6,  -1,  40,  18
};

static const std::array<const std::array<int,64>*,6> pst = {
    &pawnTable,&knightTable,&bishopTable,&rookTable,&queenTable,&kingTable
};

static int pst_value(PieceType pt, int sq, Color c){
    if(pt==NO_PIECE || pt==KING) return 0;
    if(c==WHITE) return (*pst[pt-1])[sq];
    else return (*pst[pt-1])[63 - sq];
}

// count how many pieces of the given color attack a particular square
static int attack_count(const Board &b, int sq, Color c){
    int cnt = 0;
    U64 occ = b.bothOccupancy;
    U64 bb;
    // pawns
    bb = b.bitboards[board_index(c, PAWN)];
    while(bb){
        int from = pop_lsb(bb);
        if(pawnAttacks[c==WHITE?0:1][from] & (1ULL<<sq)) cnt++;
    }
    // knights
    bb = b.bitboards[board_index(c, KNIGHT)];
    while(bb){
        int from = pop_lsb(bb);
        if(knightAttacks[from] & (1ULL<<sq)) cnt++;
    }
    // bishops
    bb = b.bitboards[board_index(c, BISHOP)];
    while(bb){
        int from = pop_lsb(bb);
        if(bishop_attacks(from, occ) & (1ULL<<sq)) cnt++;
    }
    // rooks
    bb = b.bitboards[board_index(c, ROOK)];
    while(bb){
        int from = pop_lsb(bb);
        if(rook_attacks(from, occ) & (1ULL<<sq)) cnt++;
    }
    // queens
    bb = b.bitboards[board_index(c, QUEEN)];
    while(bb){
        int from = pop_lsb(bb);
        if(queen_attacks(from, occ) & (1ULL<<sq)) cnt++;
    }
    // king
    bb = b.bitboards[board_index(c, KING)];
    if(bb){
        int from = __builtin_ctzll(bb);
        if(kingAttacks[from] & (1ULL<<sq)) cnt++;
    }
    return cnt;
}

// compute the attack bitboard for a side
static U64 attacks_for_side(const Board &b, Color c){
    U64 occ = b.bothOccupancy;
    U64 att = 0ULL;
    U64 bb;
    bb = b.bitboards[board_index(c, PAWN)];
    while(bb){ int from = pop_lsb(bb); att |= pawnAttacks[c==WHITE?0:1][from]; }
    bb = b.bitboards[board_index(c, KNIGHT)];
    while(bb){ int from = pop_lsb(bb); att |= knightAttacks[from]; }
    bb = b.bitboards[board_index(c, BISHOP)];
    while(bb){ int from = pop_lsb(bb); att |= bishop_attacks(from,occ); }
    bb = b.bitboards[board_index(c, ROOK)];
    while(bb){ int from = pop_lsb(bb); att |= rook_attacks(from,occ); }
    bb = b.bitboards[board_index(c, QUEEN)];
    while(bb){ int from = pop_lsb(bb); att |= queen_attacks(from,occ); }
    bb = b.bitboards[board_index(c, KING)];
    if(bb){ int from = __builtin_ctzll(bb); att |= kingAttacks[from]; }
    return att;
}

int evaluate(const Board &b){
    int score = 0;

    // material and piece-square tables
    for(Color c : {WHITE,BLACK}){
        int sign = (c==WHITE)?1:-1;
        for(int pt=PAWN; pt<=KING; ++pt){
            U64 bb = b.bitboards[board_index(c,(PieceType)pt)];
            while(bb){
                int sq = pop_lsb(bb);
                score += sign*pieceValue[pt-1];
                score += sign*pst_value((PieceType)pt,sq,c);
                // Bonus for pieces occupying the enemy half of the board
                if((c==WHITE && sq>=32) || (c==BLACK && sq<32))
                    score += sign*10;
            }
        }
    }
    // simple check threat bonus
    int wKing = b.king_square(WHITE);
    int bKing = b.king_square(BLACK);
    if(wKing != -1 && b.is_square_attacked(wKing, BLACK)) score -= 50;
    if(bKing != -1 && b.is_square_attacked(bKing, WHITE)) score += 50;

    // If the side not to move has no legal moves and is in check, favour the
    // side to move heavily (checkmate threat)
    Board copy = b;
    copy.sideToMove = (Color)(-b.sideToMove);
    auto replies = generate_legal_moves(copy);
    int ksq = copy.king_square(copy.sideToMove);
    if(replies.empty() && copy.is_square_attacked(ksq,b.sideToMove))
        score += (b.sideToMove==WHITE?100000:-100000);
    
    // Mobility: prefer positions where we have more legal moves than the
    // opponent.  This is a light heuristic to guide the search towards more
    // active play.
    Board tmp = b;
    tmp.sideToMove = WHITE;
    int whiteMoves = generate_legal_moves(tmp).size();
    tmp = b;
    tmp.sideToMove = BLACK;
    int blackMoves = generate_legal_moves(tmp).size();
    score += evalParams.mobilityWeight * (whiteMoves - blackMoves);

    // Central control: pieces occupying or attacking the center squares are
    // rewarded.  The four central squares are d4, e4, d5 and e5.
    int centers[4] = { sq_index('d','4'), sq_index('e','4'),
                       sq_index('d','5'), sq_index('e','5') };
    for(int sq : centers){
        Color col;
        PieceType pt = b.piece_at(sq, col);
        if(pt != NO_PIECE)
            score += (col==WHITE ? 10 : -10);
        if(b.is_square_attacked(sq, WHITE)) score += 3;
        if(b.is_square_attacked(sq, BLACK)) score -= 3;
    }

    // Square control: count attacked squares on the enemy side of the board
    U64 whiteAtt = attacks_for_side(b, WHITE);
    U64 blackAtt = attacks_for_side(b, BLACK);
    U64 enemyHalfWhite = 0xFFFFFFFF00000000ULL; // ranks 5-8
    U64 enemyHalfBlack = 0x00000000FFFFFFFFULL; // ranks 1-4
    int whiteControl = __builtin_popcountll(whiteAtt & enemyHalfWhite);
    int blackControl = __builtin_popcountll(blackAtt & enemyHalfBlack);
    int wKingSq = b.king_square(BLACK);
    if(wKingSq != -1)
        whiteControl += __builtin_popcountll(whiteAtt & kingAttacks[wKingSq]);
    int bKingSq = b.king_square(WHITE);
    if(bKingSq != -1)
        blackControl += __builtin_popcountll(blackAtt & kingAttacks[bKingSq]);
    score += evalParams.spaceControlWeight * (whiteControl - blackControl);

    // Attack vs defence imbalance
    for(Color c : {WHITE,BLACK}){
        Color them = (Color)(-c);
        int sign = (c==WHITE)?-1:1; // penalty for the side being attacked
        for(int pt=PAWN; pt<=KING; ++pt){
            U64 bb2 = b.bitboards[board_index(c,(PieceType)pt)];
            while(bb2){
                int sq = pop_lsb(bb2);
                int att = attack_count(b,sq,them);
                int def = attack_count(b,sq,c);
                if(att>def)
                    score += sign * evalParams.imbalanceWeight * (att-def);
            }
        }
    }

    // Outposts for knights in the enemy half not attackable by enemy pawns
    int whiteOutposts = 0, blackOutposts = 0;
    U64 whiteKnights = b.bitboards[board_index(WHITE, KNIGHT)];
    while(whiteKnights){
        int sq = pop_lsb(whiteKnights);
        if(sq >= 32){
            U64 enemyPawns = b.bitboards[board_index(BLACK, PAWN)];
            bool attacked = false;
            while(enemyPawns){
                int psq = pop_lsb(enemyPawns);
                if(pawnAttacks[1][psq] & (1ULL<<sq)) { attacked=true; break; }
            }
            if(!attacked) whiteOutposts++;
        }
    }
    U64 blackKnights = b.bitboards[board_index(BLACK, KNIGHT)];
    while(blackKnights){
        int sq = pop_lsb(blackKnights);
        if(sq < 32){
            U64 enemyPawns = b.bitboards[board_index(WHITE, PAWN)];
            bool attacked = false;
            while(enemyPawns){
                int psq = pop_lsb(enemyPawns);
                if(pawnAttacks[0][psq] & (1ULL<<sq)) { attacked=true; break; }
            }
            if(!attacked) blackOutposts++;
        }
    }
    score += evalParams.outpostKnightBonus * (whiteOutposts - blackOutposts);

    // Rook activity on open or semi-open files and on the seventh rank
    int rookOpenDiff = 0;
    int rook7thDiff = 0;
    U64 allPawns = b.bitboards[board_index(WHITE,PAWN)] |
                   b.bitboards[board_index(BLACK,PAWN)];
    for(Color c : {WHITE,BLACK}){
        int sign = (c==WHITE)?1:-1;
        U64 rooks = b.bitboards[board_index(c,ROOK)];
        while(rooks){
            int sq = pop_lsb(rooks);
            int file = sq % 8;
            U64 fileMask = 0x0101010101010101ULL << file;
            bool ourPawn = b.bitboards[board_index(c,PAWN)] & fileMask;
            bool oppPawn = b.bitboards[board_index((Color)-c,PAWN)] & fileMask;
            if(!ourPawn && !oppPawn) rookOpenDiff += sign*2;
            else if(!ourPawn) rookOpenDiff += sign;
            int rank = sq/8;
            if((c==WHITE && rank==6) || (c==BLACK && rank==1))
                rook7thDiff += sign;
        }
    }
    score += evalParams.openFileBonus * rookOpenDiff;
    score += evalParams.seventhRankBonus * rook7thDiff;

    // Pawn tension: pawns facing each other
    int whiteTension=0, blackTension=0;
    U64 wPawns = b.bitboards[board_index(WHITE, PAWN)];
    while(wPawns){
        int sq = pop_lsb(wPawns);
        if(sq+8 < 64 && (b.bitboards[board_index(BLACK, PAWN)] & (1ULL<<(sq+8))))
            whiteTension++;
    }
    U64 bPawns = b.bitboards[board_index(BLACK, PAWN)];
    while(bPawns){
        int sq = pop_lsb(bPawns);
        if(sq-8 >=0 && (b.bitboards[board_index(WHITE, PAWN)] & (1ULL<<(sq-8))))
            blackTension++;
    }
    score += evalParams.pawnTensionBonus * (whiteTension - blackTension);

    // Pawn breaks: available pawn captures or double pushes
    tmp = b; tmp.sideToMove = WHITE;
    auto wm = generate_legal_moves(tmp);
    int wBreaks=0;
    for(const auto &m: wm){
        if(m.piece==PAWN && (m.captured!=NO_PIECE || m.isDoublePush))
            wBreaks++;
    }
    tmp = b; tmp.sideToMove = BLACK;
    auto bm = generate_legal_moves(tmp);
    int bBreaks=0;
    for(const auto &m: bm){
        if(m.piece==PAWN && (m.captured!=NO_PIECE || m.isDoublePush))
            bBreaks++;
    }
    score += evalParams.pawnBreakBonus * (wBreaks - bBreaks);

    // Initiative: forcing moves for each side
    int wForcing=0, bForcing=0;
    for(const auto &m: wm){
        if(m.captured!=NO_PIECE){ wForcing++; continue; }
        Board cb = b; Undo u = make_move(cb,m);
        int ksq2 = cb.king_square(BLACK);
        if(ksq2!=-1 && cb.is_square_attacked(ksq2,WHITE)) wForcing++;
        undo_move(cb,m,u);
    }
    for(const auto &m: bm){
        if(m.captured!=NO_PIECE){ bForcing++; continue; }
        Board cb = b; Undo u = make_move(cb,m);
        int ksq2 = cb.king_square(WHITE);
        if(ksq2!=-1 && cb.is_square_attacked(ksq2,BLACK)) bForcing++;
        undo_move(cb,m,u);
    }
    score += evalParams.initiativeWeight * (wForcing - bForcing);

    // King safety: count safe flight squares around each king
    int wSafe=0, bSafe=0;
    if(bKing != -1){
        U64 area = kingAttacks[bKing];
        U64 mask = area & ~b.blackOccupancy;
        mask &= ~whiteAtt;
        bSafe = __builtin_popcountll(mask);
    }
    if(wKing != -1){
        U64 area = kingAttacks[wKing];
        U64 mask = area & ~b.whiteOccupancy;
        mask &= ~blackAtt;
        wSafe = __builtin_popcountll(mask);
    }
    score += evalParams.kingSafetyWeight * (wSafe - bSafe);

    return score;
}

// Simple hash for transposition table
static uint64_t board_hash(const Board &b){
    uint64_t h = 0xcbf29ce484222325ULL;
    auto mix=[&](uint64_t x){ h ^= x; h *= 0x100000001b3ULL; };
    for(auto bb: b.bitboards) mix(bb);
    mix(b.sideToMove==WHITE?1:2);
    mix(b.enPassantSquare+1);
    mix(b.w_can_castle_k); mix(b.w_can_castle_q);
    mix(b.b_can_castle_k); mix(b.b_can_castle_q);
    return h;
}

struct TTEntry { int depth; int flag; int score; Move best; };
static std::unordered_map<uint64_t,TTEntry> tt;

enum {EXACT, LOWER, UPPER};

static int quiescence(Board &b, int alpha, int beta, int &nodes){
    int stand_pat = evaluate(b);
    if(stand_pat>=beta) return beta;
    if(stand_pat>alpha) alpha=stand_pat;

    auto moves = generate_legal_moves(b);
    for(const auto &m: moves){
        if(m.captured==NO_PIECE && !m.isEnPassant && m.promotion==NO_PIECE)
            continue;
        Undo u = make_move(b,m);
        ++nodes;
        int score = -quiescence(b,-beta,-alpha,nodes);
        undo_move(b,m,u);
        if(score>=beta) return beta;
        if(score>alpha) alpha=score;
    }
    return alpha;
}

static int alphabeta(Board &b, int depth, int alpha, int beta, Move &best, int &nodes){
    uint64_t key = board_hash(b);
    auto it = tt.find(key);
    if(it!=tt.end() && it->second.depth>=depth){
        int flag = it->second.flag; int val = it->second.score;
        if(flag==EXACT) return val;
        if(flag==LOWER && val>alpha) alpha=val;
        else if(flag==UPPER && val<beta) beta=val;
        if(alpha>=beta) return val;
    }

    if(depth==0){
        return quiescence(b,alpha,beta,nodes);
    }

    auto moves = generate_legal_moves(b);
    if(moves.empty()){
        int ksq = b.king_square(b.sideToMove);
        Color enemy = b.sideToMove==WHITE?BLACK:WHITE;
        if(b.is_square_attacked(ksq,enemy)) return -INF+1;
        return 0; // stalemate
    }

    Move localBest{}; int origAlpha = alpha;
    for(const auto &m : moves){
        Undo u = make_move(b,m); ++nodes;
        Move dummy; int score = -alphabeta(b,depth-1,-beta,-alpha,dummy,nodes);
        undo_move(b,m,u);
        if(score>alpha){
            alpha=score; localBest=m;
            if(alpha>=beta) break;
        }
    }

    TTEntry entry; entry.depth=depth; entry.score=alpha; entry.best=localBest;
    entry.flag = (alpha<=origAlpha)?UPPER : (alpha>=beta?LOWER:EXACT);
    tt[key]=entry;

    best = localBest;
    return alpha;
}

SearchResult search(Board &board, int maxDepth){
    tt.clear();
    SearchResult result{}; result.nodes=0; result.score=0;
    Move best{}; int score=0;
    for(int d=1; d<=maxDepth; ++d){
        score = alphabeta(board,d,-INF,INF,best,result.nodes);
        result.score = score; result.bestMove = best;
    }
    return result;
}

} // namespace Engine
