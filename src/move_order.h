#ifndef EXOCET_MOVE_ORDER
#define EXOCET_MOVE_ORDER

#include "move.h"
#include <cstring>

struct Move_order {
    int history[12][64]{};
    Move killer[128][2]{};

    void reset() {
        std::memset(history, 0, sizeof(history));
        for (int i{}; i < 128; ++i) {
            killer[i][0] = Move{};
            killer[i][1] = Move{};
        }
    }

    void history_update(Move move, int bonus) {
       history[move.piece()][move.end()] += bonus - history[move.piece()][move.end()] * std::abs(bonus) / 1024;
    }

    int history_score(Move move) {
        return history[move.piece()][move.end()];
    }

    void killer_update(Move move, int ply) {
        if (killer[ply][0] != move) {
            killer[ply][1] = killer[ply][0];
            killer[ply][0] = move;
        }
    }
    Move killer_move(int ply, int index) {
        return killer[ply][index];
    }
};

#endif