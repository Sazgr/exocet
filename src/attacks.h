#ifndef PEACEKEEPER_ATTACKS
#define PEACEKEEPER_ATTACKS

#include "types.h"

u64 positive_ray_attacks(u64 occ, int direction, int square);
u64 negative_ray_attacks(u64 occ, int direction, int square);
u64 diagonal_attacks(u64 occ, int square);
u64 antidiagonal_attacks(u64 occ, int square);
u64 rank_attacks(u64 occ, int square);
u64 file_attacks(u64 occ, int square);
template <bool side> u64 forward_attacks(u64 occ, int square);
void init_magics();
u64 classical_rook_attacks(u64 occ, int square);
u64 classical_bishop_attacks(u64 occ, int square);
u64 rook_attacks(u64 occ, int square);
u64 bishop_attacks(u64 occ, int square);
u64 queen_attacks(u64 occ, int square);
u64 xray_rook_attacks(u64 occ, u64 blockers, int square);
u64 xray_bishop_attacks(u64 occ, u64 blockers, int square);

#endif
