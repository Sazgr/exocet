#ifndef EXOCET_MOVE_ORDER
#define EXOCET_MOVE_ORDER

#include "move.h"
#include "params.h"
#include <cstring>

struct Move_order {
    int history[12][64]{};
    int butterfly[2][64][64]{};
    int caphist[13][12][64]{};
    int* continuation;
    Move killer[128][2]{};
    int correction[65536][2]{};
    u64 move_nodes[64][65]{};

    Move_order() {
        continuation = new int[12 * 64 * 12 * 64];
    }
    ~Move_order() {
        delete[] continuation;
    }
    void reset() {
        std::memset(history, 0, sizeof(history));
        std::memset(continuation, 0, sizeof(continuation));
        for (int i{}; i < 128; ++i) {
            killer[i][0] = Move{};
            killer[i][1] = Move{};
        }
    }

    void history_update(Move move, int bonus) {
        history[move.piece()][move.end()] += bonus - history[move.piece()][move.end()] * std::abs(bonus) / hst_gravity;
    }

    int history_score(Move move) {
        return history[move.piece()][move.end()];
    }

    void butterfly_update(Move move, int bonus) {
        butterfly[move.piece() & 1][move.start()][move.end()] += bonus - butterfly[move.piece() & 1][move.start()][move.end()] * std::abs(bonus) / bfh_gravity;
    }

    int butterfly_score(Move move) {
        return butterfly[move.piece() & 1][move.start()][move.end()];
    }

    void caphist_update(Move move, int bonus) {
       caphist[move.captured()][move.piece()][move.end()] += bonus - caphist[move.captured()][move.piece()][move.end()] * std::abs(bonus) / cph_gravity;
    }

    int caphist_score(Move move) {
        return caphist[move.captured()][move.piece()][move.end()];
    }

    void continuation_update(Move previous, Move current, int bonus) {
        if (previous.is_null()) return;
        int index = previous.piece() * 64 * 12 * 64 + previous.end() * 12 * 64 + current.piece() * 64 + current.end();
        continuation[index] += bonus - continuation[index] * std::abs(bonus) / cth_gravity;
    }

    int continuation_score(Move previous, Move current) {
        if (previous.is_null()) return 0;
        int index = previous.piece() * 64 * 12 * 64 + previous.end() * 12 * 64 + current.piece() * 64 + current.end();
        return continuation[index];
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

    void correction_update(u64 pawn_hash, bool side_to_move, int correction_diff, int bonus) {
        correction[pawn_hash & 0xffffull][side_to_move] = ((crh_gravity - bonus) * correction[pawn_hash & 0xffffull][side_to_move] + bonus * correction_diff) / crh_gravity;
    }

    int correction_value(u64 pawn_hash, bool side_to_move) {
        return correction[pawn_hash & 0xffffull][side_to_move];
    }
};

#endif
