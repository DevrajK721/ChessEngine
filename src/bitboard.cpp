// bitboard.cpp
#include "bitboard.h"
#include <iostream>
#include <bitset>
#include <stdexcept>
#include <typeinfo>
#include <cmath> 

// Constructor initializes all bitboards to 0
Bitboard::Bitboard()
    : whitePawns(0), whiteKnights(0), whiteBishops(0), whiteRooks(0),
      whiteQueens(0), whiteKing(0), blackPawns(0), blackKnights(0),
      blackBishops(0), blackRooks(0), blackQueens(0), blackKing(0), enPassantTarget(-1), whitePieces(0), blackPieces(0) {}

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

std::vector<MoveState> moveHistory;

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
    state.canCastleWhiteKing = canCastleWhiteKing;
    state.canCastleWhiteQueen = canCastleWhiteQueen;
    state.canCastleBlackKing = canCastleBlackKing;
    state.canCastleBlackQueen = canCastleBlackQueen;
    state.enPassantTarget = enPassantTarget;
    return state;
}

#include "bitboard.h"
#include <stdexcept>
#include <cmath> // for std::abs

void Bitboard::makeMove(const std::string& move, char promotionPiece) {
    if (move.length() != 4) {
        throw std::invalid_argument("Invalid move format. Use standard notation, e.g., e2e4.");
    }

    // Save current board state to history for undo
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

    // Debugging output
    std::cout << "Attempting move: " << move << "\n";
    std::cout << "Source square index: " << sourceIndex << "\n";
    std::cout << "Destination square index: " << destIndex << "\n";

    // --------------------------------------------------
    //  0. Check for Castling
    // --------------------------------------------------
    // White King: e1g1
    if (move == "e1g1" && (whiteKing & (1ULL << 4)) && (whiteRooks & (1ULL << 7))) {
        uint64_t emptyMask = (1ULL << 5) | (1ULL << 6); // squares f1,g1
        uint64_t occupied  = whitePieces | blackPieces;

        if (canCastleWhiteKing &&
            !isSquareAttacked(4, false) &&  // e1 not attacked
            !isSquareAttacked(5, false) &&  // f1 not attacked
            !isSquareAttacked(6, false) &&  // g1 not attacked
            ((emptyMask & occupied) == 0))  // f1,g1 must be empty
        {
            // Perform castling
            whiteKing  &= ~(1ULL << 4); // remove King from e1
            whiteKing  |=  (1ULL << 6); // place King on g1
            whiteRooks &= ~(1ULL << 7); // remove Rook from h1
            whiteRooks |=  (1ULL << 5); // place Rook on f1
            canCastleWhiteKing  = false; 
            canCastleWhiteQueen = false; // King moved

            // Update occupancy
            whitePieces = whitePawns | whiteKnights | whiteBishops |
                          whiteRooks | whiteQueens  | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            return;
        } else {
            throw std::invalid_argument("Invalid move for white kingside castling (e1g1).");
        }

    // White King: e1c1
    } else if (move == "e1c1" && (whiteKing & (1ULL << 4)) && (whiteRooks & (1ULL << 0))) {
        uint64_t emptyMask = (1ULL << 1) | (1ULL << 2) | (1ULL << 3); // b1,c1,d1
        uint64_t occupied  = whitePieces | blackPieces;

        if (canCastleWhiteQueen &&
            !isSquareAttacked(4, false) && // e1 not attacked
            !isSquareAttacked(3, false) && // d1 not attacked
            !isSquareAttacked(2, false) && // c1 not attacked
            ((emptyMask & occupied) == 0)) // b1,c1,d1 must be empty
        {
            // Perform castling
            whiteKing  &= ~(1ULL << 4);
            whiteKing  |=  (1ULL << 2);
            whiteRooks &= ~(1ULL << 0);
            whiteRooks |=  (1ULL << 3);
            canCastleWhiteKing  = false;
            canCastleWhiteQueen = false; // King moved

            whitePieces = whitePawns | whiteKnights | whiteBishops |
                          whiteRooks | whiteQueens  | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            return;
        } else {
            throw std::invalid_argument("Invalid move for white queenside castling (e1c1).");
        }

    // Black King: e8g8
    } else if (move == "e8g8" && (blackKing & (1ULL << 60)) && (blackRooks & (1ULL << 63))) {
        uint64_t emptyMask = (1ULL << 61) | (1ULL << 62); // f8,g8
        uint64_t occupied  = whitePieces | blackPieces;

        if (canCastleBlackKing &&
            !isSquareAttacked(60, true) && // e8
            !isSquareAttacked(61, true) && // f8
            !isSquareAttacked(62, true) && // g8
            ((emptyMask & occupied) == 0))
        {
            // Perform castling
            blackKing  &= ~(1ULL << 60);
            blackKing  |=  (1ULL << 62);
            blackRooks &= ~(1ULL << 63);
            blackRooks |=  (1ULL << 61);
            canCastleBlackKing  = false;
            canCastleBlackQueen = false; // King moved

            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            whitePieces = whitePawns | whiteKnights | whiteBishops |
                          whiteRooks | whiteQueens  | whiteKing;
            return;
        } else {
            throw std::invalid_argument("Invalid move for black kingside castling (e8g8).");
        }

    // Black King: e8c8
    } else if (move == "e8c8" && (blackKing & (1ULL << 60)) && (blackRooks & (1ULL << 56))) {
        uint64_t emptyMask = (1ULL << 57) | (1ULL << 58) | (1ULL << 59); // b8,c8,d8
        uint64_t occupied  = whitePieces | blackPieces;

        if (canCastleBlackQueen &&
            !isSquareAttacked(60, true) && 
            !isSquareAttacked(59, true) && 
            !isSquareAttacked(58, true) &&
            ((emptyMask & occupied) == 0))
        {
            // Perform castling
            blackKing  &= ~(1ULL << 60);
            blackKing  |=  (1ULL << 58);
            blackRooks &= ~(1ULL << 56);
            blackRooks |=  (1ULL << 59);
            canCastleBlackKing  = false;
            canCastleBlackQueen = false; // King moved

            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            whitePieces = whitePawns | whiteKnights | whiteBishops |
                          whiteRooks | whiteQueens  | whiteKing;
            return;
        } else {
            throw std::invalid_argument("Invalid move for black queenside castling (e8c8).");
        }
    }

    // --------------------------------------------------
    //  1. En Passant Capture
    // --------------------------------------------------
    if ((whitePawns & sourceBit)) {
        // White en passant
        if (destIndex == enPassantTarget &&
            (destIndex == sourceIndex + 7 || destIndex == sourceIndex + 9))
        {
            // White captures en passant
            whitePawns &= ~sourceBit;
            whitePawns |=  destBit;

            // Remove black pawn behind the enPassantTarget
            uint64_t captureSquareBit = (1ULL << (enPassantTarget - 8));
            blackPawns &= ~captureSquareBit;

            enPassantTarget = -1;
            whitePieces = whitePawns | whiteKnights | whiteBishops |
                          whiteRooks | whiteQueens | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens | blackKing;
            return;
        }
    }
    else if ((blackPawns & sourceBit)) {
        // Black en passant
        if (destIndex == enPassantTarget &&
            (destIndex == sourceIndex - 7 || destIndex == sourceIndex - 9))
        {
            // Black captures en passant
            blackPawns &= ~sourceBit;
            blackPawns |=  destBit;

            // Remove white pawn behind the enPassantTarget
            uint64_t captureSquareBit = (1ULL << (enPassantTarget + 8));
            whitePawns &= ~captureSquareBit;

            enPassantTarget = -1;
            whitePieces = whitePawns | whiteKnights | whiteBishops |
                          whiteRooks | whiteQueens | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens | blackKing;
            return;
        }
    }

    // --------------------------------------------------
    //  2. Identify Which Piece is Moving
    // --------------------------------------------------
    uint64_t* pieceBitboard = nullptr;
    bool movingWhite = false;

    if (whitePawns & sourceBit)       { pieceBitboard = &whitePawns;   movingWhite = true;  }
    else if (whiteKnights & sourceBit){ pieceBitboard = &whiteKnights; movingWhite = true;  }
    else if (whiteBishops & sourceBit){ pieceBitboard = &whiteBishops; movingWhite = true;  }
    else if (whiteRooks & sourceBit)  { pieceBitboard = &whiteRooks;   movingWhite = true;  }
    else if (whiteQueens & sourceBit) { pieceBitboard = &whiteQueens;  movingWhite = true;  }
    else if (whiteKing & sourceBit)   { pieceBitboard = &whiteKing;    movingWhite = true;  }
    else if (blackPawns & sourceBit)  { pieceBitboard = &blackPawns;   movingWhite = false; }
    else if (blackKnights & sourceBit){ pieceBitboard = &blackKnights; movingWhite = false; }
    else if (blackBishops & sourceBit){ pieceBitboard = &blackBishops; movingWhite = false; }
    else if (blackRooks & sourceBit)  { pieceBitboard = &blackRooks;   movingWhite = false; }
    else if (blackQueens & sourceBit) { pieceBitboard = &blackQueens;  movingWhite = false; }
    else if (blackKing & sourceBit)   { pieceBitboard = &blackKing;    movingWhite = false; }

    // Debugging output
    std::cout << "Source index: " << sourceIndex << ", Source bit: " << std::bitset<64>(sourceBit) << "\n";
    std::cout << "White Pawns: " << std::bitset<64>(whitePawns) << "\n";

    if (!pieceBitboard) {
        throw std::invalid_argument("No piece found on the source square.");
    }

    // --------------------------------------------------
    //  3. White Pawn Promotion
    // --------------------------------------------------
    if (movingWhite && (pieceBitboard == &whitePawns) && destRank == 7) {
        // Remove occupant from destination
        clearSquare(destBit);

        // Remove the original white pawn
        whitePawns &= ~sourceBit;

        // Place the promoted piece
        switch (promotionPiece) {
            case 'R': whiteRooks   |= destBit; break;
            case 'B': whiteBishops |= destBit; break;
            case 'N': whiteKnights |= destBit; break;
            case 'Q':
            default : whiteQueens  |= destBit; break;
        }

        whitePieces = whitePawns | whiteKnights | whiteBishops |
                      whiteRooks | whiteQueens  | whiteKing;
        blackPieces = blackPawns | blackKnights | blackBishops |
                      blackRooks | blackQueens  | blackKing;
        return;
    }

    // --------------------------------------------------
    //  4. Black Pawn Promotion
    // --------------------------------------------------
    if (!movingWhite && (pieceBitboard == &blackPawns) && destRank == 0) {
        clearSquare(destBit);
        blackPawns &= ~sourceBit;

        switch (promotionPiece) {
            case 'R': blackRooks   |= destBit; break;
            case 'B': blackBishops |= destBit; break;
            case 'N': blackKnights |= destBit; break;
            case 'Q':
            default : blackQueens  |= destBit; break;
        }

        whitePieces = whitePawns | whiteKnights | whiteBishops |
                      whiteRooks | whiteQueens  | whiteKing;
        blackPieces = blackPawns | blackKnights | blackBishops |
                      blackRooks | blackQueens  | blackKing;
        return;
    }

    // --------------------------------------------------
    //  5. Non-Promotion Moves
    // --------------------------------------------------
    // Set enPassantTarget if it's a 2-step pawn move
    if (pieceBitboard == &whitePawns) {
        if (sourceRank == 1 && destRank == 3) {
            enPassantTarget = sourceIndex + 8; // in-between square
        } else {
            enPassantTarget = -1;
        }
    } else if (pieceBitboard == &blackPawns) {
        if (sourceRank == 6 && destRank == 4) {
            enPassantTarget = sourceIndex - 8;
        } else {
            enPassantTarget = -1;
        }
    } else {
        // Not a pawn => no en passant
        enPassantTarget = -1;
    }

    // Update castling rights if king/rook moved
    if (sourceIndex == 4) {
        // White king
        canCastleWhiteKing  = false;
        canCastleWhiteQueen = false;
    } else if (sourceIndex == 60) {
        // Black king
        canCastleBlackKing  = false;
        canCastleBlackQueen = false;
    } else if (sourceIndex == 7) {
        // White rook h1
        canCastleWhiteKing = false;
    } else if (sourceIndex == 0) {
        // White rook a1
        canCastleWhiteQueen = false;
    } else if (sourceIndex == 63) {
        // Black rook h8
        canCastleBlackKing = false;
    } else if (sourceIndex == 56) {
        // Black rook a8
        canCastleBlackQueen = false;
    }

    // Pawn vs. Normal piece capturing logic
    bool isPawn = (pieceBitboard == &whitePawns || pieceBitboard == &blackPawns);
    if (isPawn) {
        int fileDiff = destFile - sourceFile;
        int rankDiff = destRank - sourceRank;

        // Diagonal => must capture
        if (std::abs(fileDiff) == 1 && std::abs(rankDiff) == 1) {
            if (movingWhite) {
                if ((destBit & blackPieces) == 0ULL) {
                    throw std::invalid_argument("Illegal pawn capture: no black piece on destination");
                }
            } else {
                if ((destBit & whitePieces) == 0ULL) {
                    throw std::invalid_argument("Illegal pawn capture: no white piece on destination");
                }
            }
            clearSquare(destBit);

        // Straight => must NOT capture
        } else if (fileDiff == 0 && std::abs(rankDiff) >= 1) {
            if (destBit & (whitePieces | blackPieces)) {
                throw std::invalid_argument("Illegal pawn move: cannot capture forward");
            }
            // no clearSquare(destBit) here
        } else {
            throw std::invalid_argument("Illegal pawn move shape");
        }
    } else {
        // Normal piece => can capture occupant if any
        clearSquare(destBit);
    }

    // Update occupancy
    whitePieces = whitePawns | whiteKnights | whiteBishops |
                  whiteRooks | whiteQueens  | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops |
                  blackRooks | blackQueens  | blackKing;

    // Finally move the piece from source to dest
    *pieceBitboard &= ~sourceBit;
    *pieceBitboard |=  destBit;

    whitePieces = whitePawns | whiteKnights | whiteBishops |
                  whiteRooks | whiteQueens  | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops |
                  blackRooks | blackQueens  | blackKing;
}

std::vector<Move> Bitboard::generateLegalMoves(bool isWhiteTurn) {
    std::vector<Move> legalMoves;

    // Generate pseudo-legal moves - no check-check yet
    auto pseudoLegalMoves = generatePseudoLegalMoves(isWhiteTurn);
    std::cout << "GeneratinG moves.." << std::endl; // Debugging output
    for (const auto& move : pseudoLegalMoves) {
        // Make the move
        std::cout << "Move: " << move.move << "\n"; // Debugging output
        moveHistory.push_back(createMoveState());
        makeMove(move.move);

        // If we are not in check after this move => it's legal
        if (!isKingInCheck(isWhiteTurn)) {
            legalMoves.push_back(move);
        }

        // Undo the move
        undoMove(move.move);
    }
    return legalMoves;
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

void Bitboard::undoMove(const Move& move) {
    if (moveHistory.empty()) {
        throw std::runtime_error("No moves to undo!");
    }
    // Retrieve the last saved state
    const MoveState& lastState = moveHistory.back();

    // Restore board bitboards
    whitePieces = lastState.whitePieces;
    blackPieces = lastState.blackPieces;
    whiteKing   = lastState.whiteKing;
    blackKing   = lastState.blackKing;
    whiteQueens = lastState.whiteQueens;
    blackQueens = lastState.blackQueens;
    whiteRooks  = lastState.whiteRooks;
    blackRooks  = lastState.blackRooks;
    whiteBishops= lastState.whiteBishops;
    blackBishops= lastState.blackBishops;
    whiteKnights= lastState.whiteKnights;
    blackKnights= lastState.blackKnights;
    whitePawns  = lastState.whitePawns;
    blackPawns  = lastState.blackPawns;

    // Restore castling, enPassant, etc.
    canCastleWhiteKing  = lastState.canCastleWhiteKing;
    canCastleWhiteQueen = lastState.canCastleWhiteQueen;
    canCastleBlackKing  = lastState.canCastleBlackKing;
    canCastleBlackQueen = lastState.canCastleBlackQueen;
    enPassantTarget     = lastState.enPassantTarget;

    // Pop the last state
    moveHistory.pop_back();
}