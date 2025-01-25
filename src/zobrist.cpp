#include "zobrist.h"
#include <random>
#include "bitboard.h"

Side sideToMove = WHITE;
uint64_t Zobrist::pieceSquare[ZOBRIST_PIECE_TYPES][64];
uint64_t Zobrist::sideKey;
uint64_t Zobrist::castleKeys[16];
uint64_t Zobrist::enPassantKeys[8];

void Zobrist::initZobrist() {
    std::mt19937_64 rng(1234567ULL); 
    auto rand64 = [&]() -> uint64_t {
        // Return a random 64-bit value
        return ((uint64_t)rng() << 32) ^ rng();
    };

    // Fill pieceSquare[][]
    for(int pt = 0; pt < ZOBRIST_PIECE_TYPES; pt++){
        for(int sq = 0; sq < 64; sq++){
            pieceSquare[pt][sq] = rand64();
        }
    }

    // sideKey
    sideKey = rand64();

    // 16 possible castling states (4 bits for White-King, White-Queen, Black-King, Black-Queen)
    for(int i = 0; i < 16; i++){
        castleKeys[i] = rand64();
    }

    // enPassant for files a..h (0..7)
    for(int f = 0; f < 8; f++){
        enPassantKeys[f] = rand64();
    }
}

uint64_t Bitboard::computeZobristKey() const {
    uint64_t key = 0ULL;

    // For each piece type on each square:
    // e.g. if whitePawns has a bit set at square s => XOR Zobrist::pieceSquare[PIECE_WHITE_PAWN][s]

    // White Pawns
    uint64_t wp = whitePawns;
    while (wp) {
        int s = bitScanForward(wp);
        wp &= (wp - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_PAWN][s];
    }

    // White Knights
    uint64_t wn = whiteKnights;
    while (wn) {
        int s = bitScanForward(wn);
        wn &= (wn - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_KNIGHT][s];
    }

    // White Bishops
    uint64_t wb = whiteBishops;
    while (wb) {
        int s = bitScanForward(wb);
        wb &= (wb - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_BISHOP][s];
    }

    // White Rooks
    uint64_t wr = whiteRooks;
    while (wr) {
        int s = bitScanForward(wr);
        wr &= (wr - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_ROOK][s];
    }

    // White Queens
    uint64_t wq = whiteQueens;
    while (wq) {
        int s = bitScanForward(wq);
        wq &= (wq - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_QUEEN][s];
    }

    // White King
    uint64_t wk = whiteKing;
    while (wk) {
        int s = bitScanForward(wk);
        wk &= (wk - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_KING][s];
    }

    // Black Pawns
    uint64_t bp = blackPawns;
    while (bp) {
        int s = bitScanForward(bp);
        bp &= (bp - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_PAWN][s];
    }

    // Black Knights
    uint64_t bn = blackKnights;
    while (bn) {
        int s = bitScanForward(bn);
        bn &= (bn - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_KNIGHT][s];
    }

    // Black Bishops
    uint64_t bb = blackBishops;
    while (bb) {
        int s = bitScanForward(bb);
        bb &= (bb - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_BISHOP][s];
    }

    // Black Rooks
    uint64_t br = blackRooks;
    while (br) {
        int s = bitScanForward(br);
        br &= (br - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_ROOK][s];
    }

    // Black Queens
    uint64_t bq = blackQueens;
    while (bq) {
        int s = bitScanForward(bq);
        bq &= (bq - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_QUEEN][s];
    }

    // Black King
    uint64_t bk = blackKing;
    while (bk) {
        int s = bitScanForward(bk);
        bk &= (bk - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_KING][s];
    }

    // Side to move
    if (sideToMove == WHITE) { // or however you store it
        key ^= Zobrist::sideKey; 
    }

    // Castling rights
    // Suppose you store them in an int from 0..15
    int cr = 0;
    if (canCastleWhiteKing)  cr |= 0x1;
    if (canCastleWhiteQueen) cr |= 0x2;
    if (canCastleBlackKing)  cr |= 0x4;
    if (canCastleBlackQueen) cr |= 0x8;
    key ^= Zobrist::castleKeys[cr];

    // En passant
    // If there's an enPassantTarget square
    if (enPassantTarget != -1) {
        int file = enPassantTarget % 8; 
        key ^= Zobrist::enPassantKeys[file];
    }

    return key;
}

void Bitboard::updateZobristKey() {
    uint64_t key = 0ULL;

    // White Pawns
    uint64_t wp = whitePawns;
    while (wp) {
        int sq = bitScanForward(wp);
        wp &= (wp - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_PAWN][sq];
    }

    // White Knights
    uint64_t wn = whiteKnights;
    while (wn) {
        int sq = bitScanForward(wn);
        wn &= (wn - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_KNIGHT][sq];
    }

    // White Bishops
    uint64_t wb = whiteBishops;
    while (wb) {
        int sq = bitScanForward(wb);
        wb &= (wb - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_BISHOP][sq];
    }

    // White Rooks
    uint64_t wr = whiteRooks;
    while (wr) {
        int sq = bitScanForward(wr);
        wr &= (wr - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_ROOK][sq];
    }

    // White Queens
    uint64_t wq = whiteQueens;
    while (wq) {
        int sq = bitScanForward(wq);
        wq &= (wq - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_QUEEN][sq];
    }

    // White King
    uint64_t wk = whiteKing;
    while (wk) {
        int sq = bitScanForward(wk);
        wk &= (wk - 1);
        key ^= Zobrist::pieceSquare[PIECE_WHITE_KING][sq];
    }

    // Black Pawns
    uint64_t bp = blackPawns;
    while (bp) {
        int sq = bitScanForward(bp);
        bp &= (bp - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_PAWN][sq];
    }

    // Black Knights
    uint64_t bn = blackKnights;
    while (bn) {
        int sq = bitScanForward(bn);
        bn &= (bn - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_KNIGHT][sq];
    }

    // Black Bishops
    uint64_t bb = blackBishops;
    while (bb) {
        int sq = bitScanForward(bb);
        bb &= (bb - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_BISHOP][sq];
    }

    // Black Rooks
    uint64_t br = blackRooks;
    while (br) {
        int sq = bitScanForward(br);
        br &= (br - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_ROOK][sq];
    }

    // Black Queens
    uint64_t bq = blackQueens;
    while (bq) {
        int sq = bitScanForward(bq);
        bq &= (bq - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_QUEEN][sq];
    }

    // Black King
    uint64_t bk = blackKing;
    while (bk) {
        int sq = bitScanForward(bk);
        bk &= (bk - 1);
        key ^= Zobrist::pieceSquare[PIECE_BLACK_KING][sq];
    }

    // Side to move
    if (sideToMove == WHITE) {
        key ^= Zobrist::sideKey;
    }

    // Castling rights
    int cr = 0;
    if (canCastleWhiteKing)  cr |= 0x1;
    if (canCastleWhiteQueen) cr |= 0x2;
    if (canCastleBlackKing)  cr |= 0x4;
    if (canCastleBlackQueen) cr |= 0x8;
    key ^= Zobrist::castleKeys[cr];

    // En passant
    if (enPassantTarget != -1) {
        int file = enPassantTarget % 8;
        key ^= Zobrist::enPassantKeys[file];
    }

    zobristKey = key;
}

inline int ttIndex(uint64_t key) {
    return (int)(key % TT_SIZE);
}

void Zobrist::initTransTable() {
    for (int i = 0; i < TT_SIZE; i++) {
        transTable[i].key = 0ULL;
        transTable[i].depth = -1;
        transTable[i].score = 0;
        transTable[i].type  = TT_ALPHA; // default
    }
}