#ifndef EVALUATION_H
#define EVALUATION_H

#include "bitboard.h"

class Evaluation {
public:
    static int evaluate(const Bitboard& board);
};

#endif // EVALUATION_H