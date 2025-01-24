// bitboard.cpp
#include "bitboard.h"
#include <iostream>
#include <bitset>
#include <stdexcept>
#include <typeinfo>

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
        uint64_t emptyMask = (1ULL << 5) | (1ULL << 6);
        uint64_t occupied = whitePieces | blackPieces;
        if (canCastleWhiteKing &&
            !isSquareAttacked(4, false) &&
            !isSquareAttacked(5, false) &&
            !isSquareAttacked(6, false) &&
            ((emptyMask & occupied) == 0)) {
            whiteKing &= ~(1ULL << 4); // Remove king from e1
            whiteKing |= (1ULL << 6);  // Place king on g1
            whiteRooks &= ~(1ULL << 7); // Remove rook from h1
            whiteRooks |= (1ULL << 5);  // Place rook on f1
            canCastleWhiteKing = false;
            canCastleWhiteQueen = false; // King moved
            whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            return;
        }
    } else if (move == "e1c1" && (whiteKing & (1ULL << 4)) && (whiteRooks & (1ULL << 0))) { // White queenside castling
        uint64_t emptyMask = (1ULL << 1) | (1ULL << 2) | (1ULL << 3);
        uint64_t occupied = whitePieces | blackPieces;
        if (canCastleWhiteQueen && !isSquareAttacked(4, false) && !isSquareAttacked(3, false) && !isSquareAttacked(2, false) && (emptyMask & occupied) == 0) {
            whiteKing &= ~(1ULL << 4); // Remove king from e1
            whiteKing |= (1ULL << 2);  // Place king on c1
            whiteRooks &= ~(1ULL << 0); // Remove rook from a1
            whiteRooks |= (1ULL << 3);  // Place rook on d1
            canCastleWhiteKing = false;
            canCastleWhiteQueen = false; // King moved
            whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            return;
        }
    } else if (move == "e8g8" && (blackKing & (1ULL << 60)) && (blackRooks & (1ULL << 63))) { // Black kingside castling
        uint64_t emptyMask = (1ULL << 61) | (1ULL << 62);
        uint64_t occupied = whitePieces | blackPieces;

        if (canCastleBlackKing &&
            !isSquareAttacked(60, true) && // e8 not attacked
            !isSquareAttacked(61, true) && // f8 not attacked
            !isSquareAttacked(62, true) && // g8 not attacked
            ((emptyMask & occupied) == 0)) {
            blackKing &= ~(1ULL << 60); // Remove king from e8
            blackKing |= (1ULL << 62);  // Place king on g8
            blackRooks &= ~(1ULL << 63); // Remove rook from h8
            blackRooks |= (1ULL << 61);  // Place rook on f8
            canCastleBlackKing = false;
            canCastleBlackQueen = false; // King moved
            whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            return;
        }
    } else if (move == "e8c8" && (blackKing & (1ULL << 60)) && (blackRooks & (1ULL << 56))) { // Black queenside castling
        uint64_t emptyMask = (1ULL << 57) | (1ULL << 58) | (1ULL << 59);
        uint64_t occupied = whitePieces | blackPieces;

        if (canCastleBlackQueen &&
            !isSquareAttacked(60, true) && // e8 not attacked
            !isSquareAttacked(59, true) && // d8 not attacked
            !isSquareAttacked(58, true) && // c8 not attacked
            ((emptyMask & occupied) == 0))  {
            blackKing &= ~(1ULL << 60); // Remove king from e8
            blackKing |= (1ULL << 58);  // Place king on c8
            blackRooks &= ~(1ULL << 56); // Remove rook from a8
            blackRooks |= (1ULL << 59);  // Place rook on d8
            canCastleBlackKing = false;
            canCastleBlackQueen = false; // King moved
            whitePieces = whitePawns | whiteKnights | whiteBishops |
              whiteRooks | whiteQueens  | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops |
                          blackRooks | blackQueens  | blackKing;
            return;
        }
    }

    // -----------------------------
    // White en passant
    // -----------------------------
    if ((whitePawns & sourceBit)) {
        // White en passant captures occur if the DEST is exactly "enPassantTarget"
        // and the move is from sourceIndex => sourceIndex+7 or +9 (left or right).
        // In your engine, if the user typed "d5e6", then:
        //   sourceIndex=35, destIndex=44 => difference=+9
        //   enPassantTarget should be 44 if black just moved e7->e5
        if (destIndex == enPassantTarget &&
            (destIndex == sourceIndex + 7 || destIndex == sourceIndex + 9))
        {
            uint64_t enPassantBit = 1ULL << enPassantTarget;

            // Move the white pawn
            whitePawns &= ~sourceBit;
            whitePawns |= destBit;

            // Remove the black pawn that was "en passant" captured
            // It's on the rank behind the enPassantTarget:
            // If White captured left (source+7), that means the black pawn
            // is at enPassantTarget - 8. If White captured right (source+9),
            // that means the black pawn is also at enPassantTarget - 8.
            // Because black's original square is one rank behind enPassantTarget.
            uint64_t captureSquareBit = (1ULL << (enPassantTarget - 8));
            blackPawns &= ~captureSquareBit;

            // Clear en passant
            enPassantTarget = -1;

            // Recompute occupancy
            whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;

            return; // Done with move
        }
    }

    // -----------------------------
    // Black en passant
    // -----------------------------
    else if ((blackPawns & sourceBit)) {
        // Black en passant captures occur if the DEST is "enPassantTarget"
        // and the move is sourceIndex => sourceIndex-7 or -9 (left or right).
        if (destIndex == enPassantTarget &&
            (destIndex == sourceIndex - 7 || destIndex == sourceIndex - 9))
        {
            uint64_t enPassantBit = 1ULL << enPassantTarget;

            // Move the black pawn
            blackPawns &= ~sourceBit;
            blackPawns |= destBit;

            // Remove the white pawn that was "en passant" captured
            // It's on the rank behind the enPassantTarget:
            // For black, the captured pawn is at enPassantTarget + 8
            // because black's capture squares are downward.
            uint64_t captureSquareBit = (1ULL << (enPassantTarget + 8));
            whitePawns &= ~captureSquareBit;

            // Clear en passant
            enPassantTarget = -1;

            // Recompute occupancy
            whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
            blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;

            return; // Done with move
        }
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
        canCastleWhiteKing = false;
        canCastleWhiteQueen = false;
    } else if (sourceIndex == 60) { // Black king moved
        canCastleBlackKing = false;
        canCastleBlackQueen = false;
    } else if (sourceIndex == 7) { // White kingside rook moved
        canCastleWhiteKing = false;
    } else if (sourceIndex == 0) { // White queenside rook moved
        canCastleWhiteQueen = false;
    } else if (sourceIndex == 63) { // Black kingside rook moved
        canCastleBlackKing = false;
    } else if (sourceIndex == 56) { // Black queenside rook moved
        canCastleBlackQueen = false;
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
    // Then do:
    uint64_t occupied = (whitePieces | blackPieces);
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


