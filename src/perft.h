#ifndef PEACEKEEPER_PERFT
#define PEACEKEEPER_PERFT

#include "board.h"
#include "move.h"
#include "types.h"
#include <vector>

u64 perft(Position& position, int depth);
u64 perft_split(Position& position, int depth, std::vector<std::pair<Move, int>>& list);

#endif
