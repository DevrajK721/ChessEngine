#include "bitboard.h"
#include <iostream>
#include <bitset>
#include <stdexcept>



std::vector<MoveState> moveHistory;

// Constructor initializes all bitboards to 0
Bitboard::Bitboard()
    : whitePawns(0), whiteKnights(0), whiteBishops(0), whiteRooks(0),
      whiteQueens(0), whiteKing(0), blackPawns(0), blackKnights(0),
      blackBishops(0), blackRooks(0), blackQueens(0), blackKing(0), enPassantTarget(-1), whitePieces(0), blackPieces(0) {}

// Castling Rights
bool whiteCanCastleKingside = true;
bool whiteCanCastleQueenside = true;
bool blackCanCastleKingside = true;
bool blackCanCastleQueenside = true;

bool canCastleWhiteKing, canCastleWhiteQueen;
bool canCastleBlackKing, canCastleBlackQueen;

// Masks for ranks and files
constexpr uint64_t FileA = 0x0101010101010101ULL;
constexpr uint64_t FileB = FileA << 1;
constexpr uint64_t FileC = FileA << 2;
constexpr uint64_t FileD = FileA << 3;
constexpr uint64_t FileE = FileA << 4;
constexpr uint64_t FileF = FileA << 5;
constexpr uint64_t FileG = FileA << 6;
constexpr uint64_t FileH = FileA << 7;

constexpr uint64_t Rank1 = 0x00000000000000FFULL;
constexpr uint64_t Rank2 = Rank1 << 8;
constexpr uint64_t Rank3 = Rank1 << 16;
constexpr uint64_t Rank4 = Rank1 << 24;
constexpr uint64_t Rank5 = Rank1 << 32;
constexpr uint64_t Rank6 = Rank1 << 40;
constexpr uint64_t Rank7 = Rank1 << 48;
constexpr uint64_t Rank8 = Rank1 << 56;

void Bitboard::initialize() {
    // Starting position
    whitePawns = 0x000000000000FF00;
    whiteRooks = 0x0000000000000081;
    whiteKnights = 0x0000000000000042;
    whiteBishops = 0x0000000000000024;
    whiteQueens = 0x0000000000000008;
    whiteKing = 0x0000000000000010;

    blackPawns = 0x00FF000000000000;
    blackRooks = 0x8100000000000000;
    blackKnights = 0x4200000000000000;
    blackBishops = 0x2400000000000000;
    blackQueens = 0x0800000000000000;
    blackKing = 0x1000000000000000;

    precomputeKnightAttacks();

    // Combine all white and black pieces
    whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;

    enPassantTarget = -1;
}

std::string Bitboard::displayBoard() const {
    std::string board;
    for (int rank = 7; rank >= 0; --rank) {
        board += std::to_string(rank + 1) + " "; // Add rank number
        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            uint64_t bit = 1ULL << index;

            // Dynamically check all bitboards to determine the piece
            if (whiteKing & bit) board += "K ";
            else if (whiteQueens & bit) board += "Q ";
            else if (whiteRooks & bit) board += "R ";
            else if (whiteBishops & bit) board += "B ";
            else if (whiteKnights & bit) board += "N ";
            else if (whitePawns & bit) board += "P ";
            else if (blackKing & bit) board += "k ";
            else if (blackQueens & bit) board += "q ";
            else if (blackRooks & bit) board += "r ";
            else if (blackBishops & bit) board += "b ";
            else if (blackKnights & bit) board += "n ";
            else if (blackPawns & bit) board += "p ";
            else board += ". "; // Empty square
        }
        board += std::to_string(rank + 1) + "\n"; // Add rank number again
    }
    board += "  a b c d e f g h\n"; // Add file letters
    return board;
}

std::vector<Move> Bitboard::generateLegalMoves(bool isWhiteTurn) {
    std::vector<Move> legalMoves;

    // Generate pseudo-legal moves - all moves without considering check
    auto pseudoLegalMoves = generatePseudoLegalMoves(isWhiteTurn);

    for (const auto& move : pseudoLegalMoves) {
        // Make the move on the board
        moveHistory.push_back(createMoveState());
        makeMove(move.move);

        // Check if the king is in check
        if (!isKingInCheck(isWhiteTurn)) {
            legalMoves.push_back(move);
        }

        // Undo the move
        undoMove(move.move);
    }
    return legalMoves;
}

MoveState Bitboard::createMoveState() const {
    MoveState state;
    state.whitePieces = whitePieces;
    state.blackPieces = blackPieces;
    state.whiteKing = whiteKing;
    state.blackKing = blackKing;
    state.whiteQueens = whiteQueens;
    state.blackQueens = blackQueens;
    state.whiteRooks = whiteRooks;
    state.blackRooks = blackRooks;
    state.whiteBishops = whiteBishops;
    state.blackBishops = blackBishops;
    state.whiteKnights = whiteKnights;
    state.blackKnights = blackKnights;
    state.whitePawns = whitePawns;
    state.blackPawns = blackPawns;
    state.canCastleWhiteKing = whiteCanCastleKingside;
    state.canCastleWhiteQueen = whiteCanCastleQueenside;
    state.canCastleBlackKing = blackCanCastleKingside;
    state.canCastleBlackQueen = blackCanCastleQueenside;
    state.enPassantTarget = enPassantTarget;
    return state;
}

std::vector<Move> Bitboard::generatePseudoLegalMoves(bool isWhiteTurn) {
    std::vector<Move> moves;

    // Generate moves for each piece type
    if (isWhiteTurn) {
        generatePawnMoves(moves, true);
        generateKnightMoves(moves, true);
        generateSlidingPieceMoves(moves, whiteBishops, true, true);
        generateSlidingPieceMoves(moves, whiteRooks, false, true);
        generateSlidingPieceMoves(moves, whiteQueens, true, true);
        generateKingMoves(moves, whiteKing, true);
        generateCastlingMoves(moves, true);
    } else {
        generatePawnMoves(moves, false);
        generateKnightMoves(moves, false);
        generateSlidingPieceMoves(moves, blackBishops, true, false);
        generateSlidingPieceMoves(moves, blackRooks, false, false);
        generateSlidingPieceMoves(moves, blackQueens, true, false);
        generateKingMoves(moves, blackKing, false);
        generateCastlingMoves(moves, false);
    }

    return moves;
}

std::string Bitboard::formatMove(int sourceIndex, int targetIndex, char promotion) {
    char sourceFile = 'a' + (sourceIndex % 8);
    char sourceRank = '1' + (sourceIndex / 8);
    char targetFile = 'a' + (targetIndex % 8);
    char targetRank = '1' + (targetIndex / 8);

    std::string move = {sourceFile, sourceRank, targetFile, targetRank};
    if (promotion != '\0') {
        move += promotion; // Append promotion character
    }
    return move;
}

int bitScanForward(uint64_t bitboard) {
    if (bitboard == 0) return -1; // No bits set
    for (int i = 0; i < 64; ++i) {
        if (bitboard & (1ULL << i)) return i;
    }
    return -1; // Should never happen
}

void Bitboard::generatePawnMoves(std::vector<Move>& moves, bool isWhite) {
    uint64_t pawns = isWhite ? whitePawns : blackPawns;
    uint64_t ownPieces = isWhite ? whitePieces : blackPieces;
    uint64_t opponentPieces = isWhite ? blackPieces : whitePieces;
    uint64_t emptySquares = ~(whitePieces | blackPieces);

    if (isWhite) {
        // Single-step advances
        uint64_t singlePush = (pawns << 8) & emptySquares;

        // Double-step advances
        uint64_t doublePush = ((singlePush & Rank3) << 8) & emptySquares;

        // Generate single push moves
        while (singlePush) {
            int targetIndex = bitScanForward(singlePush);
            int sourceIndex = targetIndex - 8;

            // Check for promotion
            if (targetIndex >= 56) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'Q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'R'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'B'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'N'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            singlePush &= singlePush - 1; // Clear the lowest bit
        }

        // Generate double push moves
        while (doublePush) {
            int targetIndex = bitScanForward(doublePush);
            int sourceIndex = targetIndex - 16;
            moves.emplace_back(formatMove(sourceIndex, targetIndex));
            doublePush &= doublePush - 1; // Clear the lowest bit
        }

        // Captures
        uint64_t leftCaptures = (pawns << 7) & opponentPieces & ~FileH;
        uint64_t rightCaptures = (pawns << 9) & opponentPieces & ~FileA;

        while (leftCaptures) {
            int targetIndex = bitScanForward(leftCaptures);
            int sourceIndex = targetIndex - 7;

            // Check for promotion
            if (targetIndex >= 56) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'Q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'R'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'B'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'N'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            leftCaptures &= leftCaptures - 1; // Clear the lowest bit
        }

        while (rightCaptures) {
            int targetIndex = bitScanForward(rightCaptures);
            int sourceIndex = targetIndex - 9;

            // Check for promotion
            if (targetIndex >= 56) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'Q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'R'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'B'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'N'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            rightCaptures &= rightCaptures - 1; // Clear the lowest bit
        }

        // En Passant
        if (enPassantTarget != -1) {
            uint64_t enPassantSquare = 1ULL << enPassantTarget;

            uint64_t enPassantLeft = (pawns << 7) & enPassantSquare & ~FileH;
            uint64_t enPassantRight = (pawns << 9) & enPassantSquare & ~FileA;

            if (enPassantLeft) {
                int sourceIndex = enPassantTarget - 7;
                moves.emplace_back(formatMove(sourceIndex, enPassantTarget));
            }

            if (enPassantRight) {
                int sourceIndex = enPassantTarget - 9;
                moves.emplace_back(formatMove(sourceIndex, enPassantTarget));
            }
        }
    } else {
        // Black pawn logic (mirrored for white)

        // Single-step advances
        uint64_t singlePush = (pawns >> 8) & emptySquares;

        // Double-step advances
        uint64_t doublePush = ((singlePush & Rank6) >> 8) & emptySquares;

        // Generate single push moves
        while (singlePush) {
            int targetIndex = bitScanForward(singlePush);
            int sourceIndex = targetIndex + 8;

            // Check for promotion
            if (targetIndex <= 7) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'r'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'b'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'n'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            singlePush &= singlePush - 1; // Clear the lowest bit
        }

        // Generate double push moves
        while (doublePush) {
            int targetIndex = bitScanForward(doublePush);
            int sourceIndex = targetIndex + 16;
            moves.emplace_back(formatMove(sourceIndex, targetIndex));
            doublePush &= doublePush - 1; // Clear the lowest bit
        }

        // Captures
        uint64_t leftCaptures = (pawns >> 9) & opponentPieces & ~FileH;
        uint64_t rightCaptures = (pawns >> 7) & opponentPieces & ~FileA;

        while (leftCaptures) {
            int targetIndex = bitScanForward(leftCaptures);
            int sourceIndex = targetIndex + 9;

            // Check for promotion
            if (targetIndex <= 7) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'r'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'b'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'n'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            leftCaptures &= leftCaptures - 1; // Clear the lowest bit
        }

        while (rightCaptures) {
            int targetIndex = bitScanForward(rightCaptures);
            int sourceIndex = targetIndex + 7;

            // Check for promotion
            if (targetIndex <= 7) {
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'q'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'r'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'b'));
                moves.emplace_back(formatMove(sourceIndex, targetIndex, 'n'));
            } else {
                moves.emplace_back(formatMove(sourceIndex, targetIndex));
            }

            rightCaptures &= rightCaptures - 1; // Clear the lowest bit
        }

        // En Passant
        if (enPassantTarget != -1) {
            uint64_t enPassantSquare = 1ULL << enPassantTarget;

            uint64_t enPassantLeft = (pawns >> 9) & enPassantSquare & ~FileH;
            uint64_t enPassantRight = (pawns >> 7) & enPassantSquare & ~FileA;

            if (enPassantLeft) {
                int sourceIndex = enPassantTarget + 9;
                moves.emplace_back(formatMove(sourceIndex, enPassantTarget));
            }

            if (enPassantRight) {
                int sourceIndex = enPassantTarget + 7;
                moves.emplace_back(formatMove(sourceIndex, enPassantTarget));
            }
        }
    }
}

uint64_t precomputedKnightAttacks[64];

void Bitboard::generateKnightMoves(std::vector<Move>& moves, bool isWhite) {
    uint64_t knights = isWhite ? whiteKnights : blackKnights;
    uint64_t ownPieces = isWhite ? whitePieces : blackPieces;
    uint64_t opponentPieces = isWhite ? blackPieces : whitePieces;

    while (knights) {
        int index = bitScanForward(knights); // Get the index of the lowest knight
        uint64_t knightBit = 1ULL << index;

        // Precomputed knight attack mask
        uint64_t knightAttacks = precomputedKnightAttacks[index] & ~ownPieces;

        // Generate moves
        while (knightAttacks) {
            int targetIndex = bitScanForward(knightAttacks);
            moves.emplace_back(formatMove(index, targetIndex));
            knightAttacks &= knightAttacks - 1; // Clear the lowest bit
        }

        knights &= knights - 1; // Clear the lowest bit of knights
    }
}

void Bitboard::precomputeKnightAttacks() {
    for (int square = 0; square < 64; ++square) {
        uint64_t bit = 1ULL << square;
        uint64_t attacks = 0;

        // Generate all 8 possible knight moves
        if ((bit & ~FileH) << 17) attacks |= (bit & ~FileH) << 17; // Up-right
        if ((bit & ~FileA) << 15) attacks |= (bit & ~FileA) << 15; // Up-left
        if ((bit & ~(FileH | FileG)) << 10) attacks |= (bit & ~(FileH | FileG)) << 10; // Right-up
        if ((bit & ~(FileA | FileB)) << 6) attacks |= (bit & ~(FileA | FileB)) << 6; // Left-up
        if ((bit & ~(FileH | FileG)) >> 6) attacks |= (bit & ~(FileH | FileG)) >> 6; // Right-down
        if ((bit & ~(FileA | FileB)) >> 10) attacks |= (bit & ~(FileA | FileB)) >> 10; // Left-down
        if ((bit & ~FileH) >> 15) attacks |= (bit & ~FileH) >> 15; // Down-right
        if ((bit & ~FileA) >> 17) attacks |= (bit & ~FileA) >> 17; // Down-left

        precomputedKnightAttacks[square] = attacks;
    }
}

void Bitboard::generateSlidingPieceMoves(std::vector<Move>& moves, uint64_t pieceBitboard, bool isBishop, bool isWhite) {
    while (pieceBitboard) {
        int sourceIndex = bitScanForward(pieceBitboard);
        uint64_t piece = 1ULL << sourceIndex;

        uint64_t attacks = generateSlidingAttacks(sourceIndex, whitePieces | blackPieces) &
                           (isWhite ? ~whitePieces : ~blackPieces);

        while (attacks) {
            int targetIndex = bitScanForward(attacks);
            attacks &= attacks - 1;

            moves.emplace_back(formatMove(sourceIndex, targetIndex));
        }

        pieceBitboard &= pieceBitboard - 1;
    }
}

uint64_t Bitboard::generateSlidingAttacks(int squareIndex, uint64_t occupied) const {
    uint64_t attacks = 0;

    // Calculate attacks for each direction (N, E, S, W, NE, NW, SE, SW)
    for (int direction : {8, -8, 1, -1, 9, -9, 7, -7}) {
        int currentIndex = squareIndex;

        while (true) {
            currentIndex += direction;

            // Check boundaries and add to attack if valid
            if (currentIndex < 0 || currentIndex >= 64 || isBoundaryCrossed(squareIndex, currentIndex, direction)) {
                break;
            }

            attacks |= (1ULL << currentIndex);

            // Stop at the first occupied square
            if (occupied & (1ULL << currentIndex)) {
                break;
            }
        }
    }

    return attacks;
}

// Helper function to check board boundaries
bool Bitboard::isBoundaryCrossed(int startIndex, int currentIndex, int direction) const {
    // If out of range, obviously boundary crossed
    if (currentIndex < 0 || currentIndex >= 64) {
        return true;
    }

    int startRank = startIndex / 8, startFile = startIndex % 8;
    int currRank  = currentIndex / 8, currFile  = currentIndex % 8;

    // For a single step in direction, how much do rank/file change?
    int rankDelta = currRank - startRank;
    int fileDelta = currFile - startFile;

    switch (direction) {
        case  8: // up
        case -8: // down
            // The file should not change at all
            return (fileDelta != 0);

        case  1: // right
        case -1: // left
            // The rank should not change at all
            return (rankDelta != 0);

        case  9:  // up-right
        case -9:  // down-left
            // rankDelta and fileDelta should match
            return (rankDelta != fileDelta);

        case  7:  // up-left
        case -7:  // down-right
            // rankDelta should be negative of fileDelta
            return (rankDelta != -fileDelta);

        default:
            return true; // unknown direction => treat as boundary
    }
}

void Bitboard::makeMove(const std::string& move, char promotionPiece) {
    if (move.length() != 4) {
        throw std::invalid_argument("Invalid move format. Use standard notation, e.g., e2e4.");
    }

    MoveState currentState = createMoveState();
    moveHistory.push_back(currentState);

    int sourceFile = move[0] - 'a';
    int sourceRank = move[1] - '1';
    int destFile   = move[2] - 'a';
    int destRank   = move[3] - '1';

    int sourceIndex = sourceRank * 8 + sourceFile;
    int destIndex   = destRank * 8 + destFile;

    uint64_t sourceBit = 1ULL << sourceIndex;
    uint64_t destBit   = 1ULL << destIndex;

    // --------------------------------------------------
    // 0. Handle Castling
    // --------------------------------------------------
    if (move == "e1g1" && (whiteKing & (1ULL << 4)) && (whiteRooks & (1ULL << 7))) { // White kingside castling
        if (whiteCanCastleKingside && !isSquareAttacked(4, false) && !isSquareAttacked(5, false) && !isSquareAttacked(6, false)) {
            whiteKing &= ~(1ULL << 4); // Remove king from e1
            whiteKing |= (1ULL << 6);  // Place king on g1
            whiteRooks &= ~(1ULL << 7); // Remove rook from h1
            whiteRooks |= (1ULL << 5);  // Place rook on f1
            whiteCanCastleKingside = false;
            whiteCanCastleQueenside = false; // King moved
            whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            return;
        } else {
            throw std::invalid_argument("Illegal kingside castling for white.");
        }
    } else if (move == "e1c1" && (whiteKing & (1ULL << 4)) && (whiteRooks & (1ULL << 0))) { // White queenside castling
        if (whiteCanCastleQueenside && !isSquareAttacked(4, false) && !isSquareAttacked(3, false) && !isSquareAttacked(2, false)) {
            whiteKing &= ~(1ULL << 4); // Remove king from e1
            whiteKing |= (1ULL << 2);  // Place king on c1
            whiteRooks &= ~(1ULL << 0); // Remove rook from a1
            whiteRooks |= (1ULL << 3);  // Place rook on d1
            whiteCanCastleKingside = false;
            whiteCanCastleQueenside = false; // King moved
            whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            return;
        } else {
            throw std::invalid_argument("Illegal queenside castling for white.");
        }
    } else if (move == "e8g8" && (blackKing & (1ULL << 60)) && (blackRooks & (1ULL << 63))) { // Black kingside castling
        if (blackCanCastleKingside && !isSquareAttacked(60, true) && !isSquareAttacked(61, true) && !isSquareAttacked(62, true)) {
            blackKing &= ~(1ULL << 60); // Remove king from e8
            blackKing |= (1ULL << 62);  // Place king on g8
            blackRooks &= ~(1ULL << 63); // Remove rook from h8
            blackRooks |= (1ULL << 61);  // Place rook on f8
            blackCanCastleKingside = false;
            blackCanCastleQueenside = false; // King moved
            whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            return;
        } else {
            throw std::invalid_argument("Illegal kingside castling for black.");
        }
    } else if (move == "e8c8" && (blackKing & (1ULL << 60)) && (blackRooks & (1ULL << 56))) { // Black queenside castling
        if (blackCanCastleQueenside && !isSquareAttacked(60, true) && !isSquareAttacked(59, true) && !isSquareAttacked(58, true)) {
            blackKing &= ~(1ULL << 60); // Remove king from e8
            blackKing |= (1ULL << 58);  // Place king on c8
            blackRooks &= ~(1ULL << 56); // Remove rook from a8
            blackRooks |= (1ULL << 59);  // Place rook on d8
            blackCanCastleKingside = false;
            blackCanCastleQueenside = false; // King moved
            whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            return;
        } else {
            throw std::invalid_argument("Illegal queenside castling for black.");
        }
    }

    // --------------------------------------------------
    // 1. Handle En Passant
    // --------------------------------------------------
    if (destIndex == enPassantTarget + 8 || destIndex == enPassantTarget - 8) {
        uint64_t enPassantBit = 1ULL << enPassantTarget;

        if (whitePawns & sourceBit) {
            whitePawns &= ~sourceBit;
            whitePawns |= destBit;
            blackPawns &= ~enPassantBit;
        } else if (blackPawns & sourceBit) {
            blackPawns &= ~sourceBit;
            blackPawns |= destBit;
            whitePawns &= ~enPassantBit;
        }

        enPassantTarget = -1;
        whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
        blackPieces = blackPawns | blackKnights | blackBishops |
                      blackRooks | blackQueens  | blackKing;
        return;
    }

    // --------------------------------------------------
    // 2. Identify Which Piece is Moving
    // --------------------------------------------------
    // We’ll store the pointer to whichever bitboard array is relevant
    uint64_t* pieceBitboard = nullptr;
    bool movingWhite = false;

    if (whitePawns & sourceBit) {
        pieceBitboard = &whitePawns;
        movingWhite = true;
    } else if (whiteKnights & sourceBit) {
        pieceBitboard = &whiteKnights;
        movingWhite = true;
    } else if (whiteBishops & sourceBit) {
        pieceBitboard = &whiteBishops;
        movingWhite = true;
    } else if (whiteRooks & sourceBit) {
        pieceBitboard = &whiteRooks;
        movingWhite = true;
    } else if (whiteQueens & sourceBit) {
        pieceBitboard = &whiteQueens;
        movingWhite = true;
    } else if (whiteKing & sourceBit) {
        pieceBitboard = &whiteKing;
        movingWhite = true;

    } else if (blackPawns & sourceBit) {
        pieceBitboard = &blackPawns;
        movingWhite = false;
    } else if (blackKnights & sourceBit) {
        pieceBitboard = &blackKnights;
        movingWhite = false;
    } else if (blackBishops & sourceBit) {
        pieceBitboard = &blackBishops;
        movingWhite = false;
    } else if (blackRooks & sourceBit) {
        pieceBitboard = &blackRooks;
        movingWhite = false;
    } else if (blackQueens & sourceBit) {
        pieceBitboard = &blackQueens;
        movingWhite = false;
    } else if (blackKing & sourceBit) {
        pieceBitboard = &blackKing;
        movingWhite = false;
    }

    if (!pieceBitboard) {
        throw std::invalid_argument("No piece found on the source square.");
    }

    // --------------------------------------------------
    // 3. White Pawn Promotion
    // --------------------------------------------------
    if (movingWhite && destRank == 7 && (whitePawns & sourceBit)) {
        // Remove occupant from the destination bit, just in case
        clearSquare(destBit);

        // Remove the original white pawn
        whitePawns &= ~sourceBit;

        // Add the chosen promotion piece
        switch (promotionPiece) {
            case 'R': whiteRooks   |= destBit; break;
            case 'B': whiteBishops |= destBit; break;
            case 'N': whiteKnights |= destBit; break;
            case 'Q':
            default:  whiteQueens  |= destBit; break;
        }
        whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
        blackPieces = blackPawns | blackKnights | blackBishops |
                      blackRooks | blackQueens  | blackKing;

        return;
    }

    // --------------------------------------------------
    // 4. Black Pawn Promotion
    // --------------------------------------------------
    if (!movingWhite && destRank == 0 && (blackPawns & sourceBit)) {
        // Remove occupant from the destination bit, just in case
        clearSquare(destBit);

        // Remove the original black pawn
        blackPawns &= ~sourceBit;

        // Add the chosen promotion piece
        switch (promotionPiece) {
            case 'R': blackRooks   |= destBit; break;
            case 'B': blackBishops |= destBit; break;
            case 'N': blackKnights |= destBit; break;
            case 'Q':
            default:  blackQueens  |= destBit; break;
        }
        whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
        blackPieces = blackPawns | blackKnights | blackBishops |
                      blackRooks | blackQueens  | blackKing;

        return;
    }

    // --------------------------------------------------
    // 5. Non-Promotion Moves
    // --------------------------------------------------
    // Handle en passant target if it's a 2-step pawn move
    if (movingWhite) {
        if (sourceRank == 1 && destRank == 3) {
            enPassantTarget = destIndex;
        } else {
            enPassantTarget = -1;
        }
    } else {
        if (sourceRank == 6 && destRank == 4) {
            enPassantTarget = destIndex;
        } else {
            enPassantTarget = -1;
        }
    }

    // Ensure castling rights are updated when king or rook moves
    if (sourceIndex == 4) { // White king moved
        whiteCanCastleKingside = false;
        whiteCanCastleQueenside = false;
    } else if (sourceIndex == 60) { // Black king moved
        blackCanCastleKingside = false;
        blackCanCastleQueenside = false;
    } else if (sourceIndex == 7) { // White kingside rook moved
        whiteCanCastleKingside = false;
    } else if (sourceIndex == 0) { // White queenside rook moved
        whiteCanCastleQueenside = false;
    } else if (sourceIndex == 63) { // Black kingside rook moved
        blackCanCastleKingside = false;
    } else if (sourceIndex == 56) { // Black queenside rook moved
        blackCanCastleQueenside = false;
    }

    // If it’s a capture, remove occupant from destination.
    // (For a more complete engine, you'd detect if there's an opponent piece.)
    clearSquare(destBit);
    whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops |
                  blackRooks | blackQueens  | blackKing;
    // Move the piece
    *pieceBitboard &= ~sourceBit;
    *pieceBitboard |= destBit;
    whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops |
                  blackRooks | blackQueens  | blackKing;
}

void Bitboard::clearSquare(uint64_t squareBit) {
    // Remove from white bitboards
    whitePawns   &= ~squareBit;
    whiteKnights &= ~squareBit;
    whiteBishops &= ~squareBit;
    whiteRooks   &= ~squareBit;
    whiteQueens  &= ~squareBit;
    whiteKing    &= ~squareBit;

    // Remove from black bitboards
    blackPawns   &= ~squareBit;
    blackKnights &= ~squareBit;
    blackBishops &= ~squareBit;
    blackRooks   &= ~squareBit;
    blackQueens  &= ~squareBit;
    blackKing    &= ~squareBit;
}

bool Bitboard::isSquareAttacked(int squareIndex, bool byWhite) const {
    uint64_t squareBit = 1ULL << squareIndex;

    // 1) Pawns
    if (byWhite) {
        if ((whitePawns & (squareBit >> 7) & ~0x8080808080808080ULL) || // Attack from left
            (whitePawns & (squareBit >> 9) & ~0x0101010101010101ULL)) { // Attack from right
            return true;
            }
    } else {
        if ((blackPawns & (squareBit << 7) & ~0x0101010101010101ULL) || // Attack from left
            (blackPawns & (squareBit << 9) & ~0x8080808080808080ULL)) { // Attack from right
            return true;
            }
    }

    // 2) Knights
    uint64_t knightAttacks = getKnightAttacks(squareIndex);
    if (byWhite && (whiteKnights & knightAttacks)) return true;
    if (!byWhite && (blackKnights & knightAttacks)) return true;

    // 3) Bishops
    uint64_t bishopAttacks = getBishopAttacks(squareIndex);
    if (byWhite && (whiteBishops & bishopAttacks)) return true;
    if (!byWhite && (blackBishops & bishopAttacks)) return true;

    // 4) Rooks
    uint64_t rookAttacks = getRookAttacks(squareIndex);
    if (byWhite && (whiteRooks & rookAttacks)) return true;
    if (!byWhite && (blackRooks & rookAttacks)) return true;

    // 5) Queens
    uint64_t queenAttacks = getQueenAttacks(squareIndex);
    if (byWhite && (whiteQueens & queenAttacks)) return true;
    if (!byWhite && (blackQueens & queenAttacks)) return true;

    // 6) Kings
    uint64_t kingAttacks = getKingAttacks(squareIndex);
    if (byWhite && (whiteKing & kingAttacks)) return true;
    if (!byWhite && (blackKing & kingAttacks)) return true;

    return false;
}

uint64_t Bitboard::getKnightAttacks(int squareIndex) const {
    uint64_t knightBit = 1ULL << squareIndex;
    uint64_t l1 = (knightBit & ~0x8080808080808080ULL) >> 1;
    uint64_t l2 = (knightBit & ~0xC0C0C0C0C0C0C0C0ULL) >> 2;
    uint64_t r1 = (knightBit & ~0x0101010101010101ULL) << 1;
    uint64_t r2 = (knightBit & ~0x0303030303030303ULL) << 2;
    uint64_t h1 = l1 | r1;
    uint64_t h2 = l2 | r2;
    uint64_t attacks = (h1 << 8) | (h1 >> 8) | (h2 << 16) | (h2 >> 16);
    return attacks;
}

uint64_t Bitboard::getBishopAttacks(int squareIndex) const {
    uint64_t attacks = 0;
    int rank = squareIndex / 8;
    int file = squareIndex % 8;

    // Generate attacks in four diagonal directions
    for (int r = rank + 1, f = file + 1; r < 8 && f < 8; ++r, ++f) { // Up-right
        int index = r * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }
    for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; ++r, --f) { // Up-left
        int index = r * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }
    for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; --r, ++f) { // Down-right
        int index = r * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; --r, --f) { // Down-left
        int index = r * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }

    return attacks;
}

uint64_t Bitboard::getRookAttacks(int squareIndex) const {
    uint64_t attacks = 0;
    int rank = squareIndex / 8;
    int file = squareIndex % 8;

    // Generate attacks in four cardinal directions
    for (int r = rank + 1; r < 8; ++r) { // Up
        int index = r * 8 + file;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break; // Stop at the first occupied square
    }
    for (int r = rank - 1; r >= 0; --r) { // Down
        int index = r * 8 + file;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }
    for (int f = file + 1; f < 8; ++f) { // Right
        int index = rank * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }
    for (int f = file - 1; f >= 0; --f) { // Left
        int index = rank * 8 + f;
        attacks |= (1ULL << index);
        if (isSquareOccupied(index)) break;
    }

    return attacks;
}

uint64_t Bitboard::getKingAttacks(int squareIndex) const {
    uint64_t kingBit = 1ULL << squareIndex;
    uint64_t attacks = ((kingBit << 8) | (kingBit >> 8) |
                        ((kingBit & ~0x0101010101010101ULL) << 1) |
                        ((kingBit & ~0x0101010101010101ULL) >> 7) |
                        ((kingBit & ~0x8080808080808080ULL) >> 1) |
                        ((kingBit & ~0x8080808080808080ULL) << 7) |
                        ((kingBit & ~0x0101010101010101ULL) >> 9) |
                        ((kingBit & ~0x8080808080808080ULL) << 9));
    return attacks;
}

uint64_t Bitboard::getQueenAttacks(int squareIndex) const {
    return getRookAttacks(squareIndex) | getBishopAttacks(squareIndex);
}

uint64_t Bitboard::generatePawnAttacks(bool isWhite, uint64_t pawns) const {
    return isWhite
        ? ((pawns & ~FileA) << 7) | ((pawns & ~FileH) << 9)  // White pawn attacks
        : ((pawns & ~FileA) >> 9) | ((pawns & ~FileH) >> 7); // Black pawn attacks
}

uint64_t Bitboard::generateKnightAttacks(uint64_t knights) const {
    uint64_t left1 = (knights & ~FileA) >> 1;
    uint64_t right1 = (knights & ~FileH) << 1;
    uint64_t left2 = (knights & ~(FileA | FileB)) >> 2;
    uint64_t right2 = (knights & ~(FileH | FileG)) << 2;

    return (left2 | right2) << 16 | (left2 | right2) >> 16 | (left1 | right1) << 8 | (left1 | right1) >> 8;
}

uint64_t Bitboard::generateKingAttacks(uint64_t kingBitboard) const {
    // Assume kingBitboard has exactly one bit set.
    // We'll call that 'square'.
    int square = bitScanForward(kingBitboard);

    // Convert square to rank & file
    int rank = square / 8;
    int file = square % 8;

    // We'll build up a 64-bit mask of possible moves
    uint64_t attacks = 0;

    // Offsets for the 8 surrounding squares
    // (rowOffset, colOffset) pairs
    static const int offsets[8][2] = {
        {1, 0},   // up
        {-1, 0},  // down
        {0, 1},   // right
        {0, -1},  // left
        {1, 1},   // up-right
        {1, -1},  // up-left
        {-1, 1},  // down-right
        {-1, -1}  // down-left
    };

    for (auto& off : offsets) {
        int newRank = rank + off[0];
        int newFile = file + off[1];
        if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
            int newSquare = newRank * 8 + newFile;
            attacks |= (1ULL << newSquare);
        }
    }

    return attacks;
}

bool Bitboard::isKingInCheck(bool isWhite) const {
    int kingIndex = bitScanForward(isWhite ? whiteKing : blackKing); // Locate the king's position
    uint64_t kingBit = 1ULL << kingIndex;

    // Check if the king's position is under attack by any opponent's piece
    uint64_t opponentPieces = isWhite ? blackPieces : whitePieces;

    // Combine all possible attacks
    uint64_t attacks = generatePawnAttacks(!isWhite, kingBit) |
                       generateKnightAttacks(kingBit) |
                       generateSlidingAttacks(kingIndex, whitePieces | blackPieces) |
                       generateKingAttacks(kingIndex);

    // Return true if any opponent piece attacks the king's square
    return (attacks & kingBit) != 0;
}

void Bitboard::undoMove(const Move& move) {
    // Restore previous state using a stack or saved state
    if (moveHistory.empty()) {
        throw std::runtime_error("No moves to undo!");
    }

    // Retrieve the last move
    const MoveState& lastState = moveHistory.back();

    // Restore the board state
    whitePieces = lastState.whitePieces;
    blackPieces = lastState.blackPieces;
    whiteKing = lastState.whiteKing;
    blackKing = lastState.blackKing;
    whiteQueens = lastState.whiteQueens;
    blackQueens = lastState.blackQueens;
    whiteRooks = lastState.whiteRooks;
    blackRooks = lastState.blackRooks;
    whiteBishops = lastState.whiteBishops;
    blackBishops = lastState.blackBishops;
    whiteKnights = lastState.whiteKnights;
    blackKnights = lastState.blackKnights;
    whitePawns = lastState.whitePawns;
    blackPawns = lastState.blackPawns;

    // Restore castling, en passant, and other metadata
    canCastleWhiteKing = lastState.canCastleWhiteKing;
    canCastleWhiteQueen = lastState.canCastleWhiteQueen;
    canCastleBlackKing = lastState.canCastleBlackKing;
    canCastleBlackQueen = lastState.canCastleBlackQueen;
    enPassantTarget = lastState.enPassantTarget;

    // Pop the last state
    moveHistory.pop_back();
}

void Bitboard::generateKingMoves(std::vector<Move>& moves, uint64_t kingBitboard, bool isWhite) {
    int sourceIndex = bitScanForward(kingBitboard);
    uint64_t attacks = getKingAttacks(kingBitboard) & (isWhite ? ~whitePieces : ~blackPieces);

    while (attacks) {
        int targetIndex = bitScanForward(attacks);
        moves.emplace_back(formatMove(sourceIndex, targetIndex));
        attacks &= attacks - 1; // Remove the processed target bit
    }
}

void Bitboard::generateCastlingMoves(std::vector<Move>& moves, bool isWhite) {
    uint64_t kingBitboard = isWhite ? whiteKing : blackKing;

    // Check if the king is in its starting position
    if ((kingBitboard & (isWhite ? 0x10ULL : 0x1000000000000000ULL)) == 0) {
        return;
    }

    // Kingside Castling
    if ((isWhite && canCastleWhiteKing) || (!isWhite && canCastleBlackKing)) {
        uint64_t castleMask = isWhite ? 0x60ULL : 0x6000000000000000ULL;
        if ((castleMask & (whitePieces | blackPieces)) == 0 && !isKingInCheck(isWhite)) {
            if (!isSquareAttacked(isWhite, isWhite ? 5 : 61) && !isSquareAttacked(isWhite, isWhite ? 6 : 62)) {
                moves.emplace_back(formatMove(isWhite ? 4 : 60, isWhite ? 6 : 62));
            }
        }
    }

    // Queenside Castling
    if ((isWhite && canCastleWhiteQueen) || (!isWhite && canCastleBlackQueen)) {
        uint64_t castleMask = isWhite ? 0xEULL : 0xE00000000000000ULL;
        if ((castleMask & (whitePieces | blackPieces)) == 0 && !isKingInCheck(isWhite)) {
            if (!isSquareAttacked(isWhite, isWhite ? 3 : 59) && !isSquareAttacked(isWhite, isWhite ? 2 : 58)) {
                moves.emplace_back(formatMove(isWhite ? 4 : 60, isWhite ? 2 : 58));
            }
        }
    }
}

uint64_t Bitboard::getKingAttacks(uint64_t kingBitboard) const {
    // Assume kingBitboard has exactly one bit set.
    // We'll call that 'square'.
    int square = bitScanForward(kingBitboard);

    // Convert square to rank & file
    int rank = square / 8;
    int file = square % 8;

    // We'll build up a 64-bit mask of possible moves
    uint64_t attacks = 0;

    // Offsets for the 8 surrounding squares
    // (rowOffset, colOffset) pairs
    static const int offsets[8][2] = {
        {1, 0},   // up
        {-1, 0},  // down
        {0, 1},   // right
        {0, -1},  // left
        {1, 1},   // up-right
        {1, -1},  // up-left
        {-1, 1},  // down-right
        {-1, -1}  // down-left
    };

    for (auto& off : offsets) {
        int newRank = rank + off[0];
        int newFile = file + off[1];
        if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
            int newSquare = newRank * 8 + newFile;
            attacks |= (1ULL << newSquare);
        }
    }

    return attacks;
}



bool Bitboard::isSquareOccupied(int squareIndex) const {
    uint64_t squareBit = 1ULL << squareIndex;
    return (whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing |
            blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing) & squareBit;
}

int Bitboard::getEnPassantTarget() const {
    return enPassantTarget;
}
