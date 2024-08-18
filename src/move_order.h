#ifndef EXOCET_MOVE_ORDER
#define EXOCET_MOVE_ORDER

#include "move.h"
#include <cstring>

struct Move_order {
    int history[12][64]{};

    void reset() {
        std::memset(history, 0, sizeof(history));
    }

    void history_update(Move move, int bonus) {
       history[move.piece()][move.end()] += bonus - history[move.piece()][move.end()] * std::abs(bonus) / 1024;
    }

    int history_score(Move move) {
        return history[move.piece()][move.end()];
    }
};

#endif