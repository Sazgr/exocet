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
    Move excluded{};
    int ply{};
    int static_eval{};
};

struct Search_data {
    u64 nodes{};
    NNUE* nnue = nullptr;
    Limit_timer* timer = nullptr;
    Hash_table* hash_table;
    Move_order* move_order;
    Move pv_table[128][128];
};

const int see_value[7] = {256, 768, 768, 1280, 2304, 20000, 0};

bool see(Position& position, Move move, const int threshold);
int qsearch(Position& position, Search_stack* ss, Search_data& sd, int alpha, int beta);
int search(Position& position, Search_stack* ss, Search_data& sd, int depth, int alpha, int beta);
void search_root(Position& position, Limit_timer& timer, Search_data& sd, bool output);

#endif
