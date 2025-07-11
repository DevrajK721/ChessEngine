#include "engine.hpp"
#include "attacks.hpp"
#include <array>
#include <limits>
#include <unordered_map>

namespace Engine {

static const int INF = 100000;

// Piece values
static const int pieceValue[6] = {100, 320, 330, 500, 900, 0};

// Simple piece-square tables (from white perspective)
static const std::array<int,64> pawnTable = {
      0,  0,  0,  0,  0,  0,  0,  0,
     50, 50, 50, 50, 50, 50, 50, 50,
     10, 10, 20, 30, 30, 20, 10, 10,
      5,  5, 10, 25, 25, 10,  5,  5,
      0,  0,  0, 20, 20,  0,  0,  0,
      5, -5,-10,  0,  0,-10, -5,  5,
      5, 10, 10,-20,-20, 10, 10,  5,
      0,  0,  0,  0,  0,  0,  0,  0
};
static const std::array<int,64> knightTable = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};
static const std::array<int,64> bishopTable = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};
static const std::array<int,64> rookTable = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};
static const std::array<int,64> queenTable = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};
static const std::array<int,64> kingTable = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

static const std::array<const std::array<int,64>*,6> pst = {
    &pawnTable,&knightTable,&bishopTable,&rookTable,&queenTable,&kingTable
};

static int pst_value(PieceType pt, int sq, Color c){
    if(pt==NO_PIECE || pt==KING) return 0;
    if(c==WHITE) return (*pst[pt-1])[sq];
    else return (*pst[pt-1])[63 - sq];
}

int evaluate(const Board &b){
    int score = 0;
    for(Color c : {WHITE,BLACK}){
        int sign = (c==WHITE)?1:-1;
        for(int pt=PAWN; pt<=KING; ++pt){
            U64 bb = b.bitboards[board_index(c,(PieceType)pt)];
            while(bb){
                int sq = pop_lsb(bb);
                score += sign*pieceValue[pt-1];
                score += sign*pst_value((PieceType)pt,sq,c);
            }
        }
    }
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
