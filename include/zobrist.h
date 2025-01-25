#pragma once
#include <cstdint>
#include <vector>

static const int PIECE_WHITE_PAWN   = 0;
static const int PIECE_WHITE_KNIGHT = 1;
static const int PIECE_WHITE_BISHOP = 2;
static const int PIECE_WHITE_ROOK   = 3;
static const int PIECE_WHITE_QUEEN  = 4;
static const int PIECE_WHITE_KING   = 5;
static const int PIECE_BLACK_PAWN   = 6;
static const int PIECE_BLACK_KNIGHT = 7;
static const int PIECE_BLACK_BISHOP = 8;
static const int PIECE_BLACK_ROOK   = 9;
static const int PIECE_BLACK_QUEEN  = 10;
static const int PIECE_BLACK_KING   = 11;

static const int ZOBRIST_PIECE_TYPES = 12; 
// e.g. 0=WhitePawn,1=WhiteKnight,2=WhiteBishop,3=WhiteRook,4=WhiteQueen,5=WhiteKing
//      6=BlackPawn,7=BlackKnight,8=BlackBishop,9=BlackRook,10=BlackQueen,11=BlackKing

enum Side { WHITE, BLACK };
extern Side sideToMove;

enum NodeType {
    TT_ALPHA, // fail-low
    TT_BETA,  // fail-high
    TT_EXACT
};

struct TTEntry {
    uint64_t key;    // Full Zobrist key
    int depth;       // The depth this entry was searched to
    int score;       // The stored evaluation
    NodeType type;   // EXACT, ALPHA, or BETA
};

static const int TT_SIZE = 1 << 20; // ~1 million
static std::vector<TTEntry> transTable(TT_SIZE);

namespace Zobrist {
    extern uint64_t pieceSquare[ZOBRIST_PIECE_TYPES][64];
    extern uint64_t sideKey;            // White to move or black to move
    extern uint64_t castleKeys[16];     // For 0..15 castling right states
    extern uint64_t enPassantKeys[8];   // For en passant on file 0..7

    void initZobrist();  
    void initTransTable(); 
}