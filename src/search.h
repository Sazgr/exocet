#ifndef EXOCET_SEARCH
#define EXOCET_SEARCH

#include "board.h"
#include "hash.h"
#include "move.h"
#include "move_order.h"
#include "nnue.h"
#include "timer.h"

struct Search_stack {
    Move move{};
    int ply{};
};

struct Search_data {
    u64 nodes{};
    NNUE* nnue = nullptr;
    Limit_timer* timer = nullptr;
    Hash_table* hash_table;
    Move_order* move_order;
    Move pv_table[128][128];
};

int search(Position& position, Search_stack* ss, Search_data& sd, int depth, int alpha, int beta);
void search_root(Position& position, Limit_timer& timer, Search_data& sd, bool output);

#endif
