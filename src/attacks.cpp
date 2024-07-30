#include "attacks.h"
#include "bits.h"
#include "lookup.h"
#include "magics.h"

enum Directions {
    northwest,
    north,
    northeast,
    west,
    east,
    southwest,
    south,
    southeast
};

u64 positive_ray_attacks(u64 occ, int direction, int square) {
    u64 attacks = rays[direction][square];
    square = get_lsb((attacks & occ) | 0x8000000000000000ull);
    attacks ^= rays[direction][square];
    return attacks;
}

u64 negative_ray_attacks(u64 occ, int direction, int square) {
    u64 attacks = rays[direction][square];
    square = get_msb((attacks & occ) | 0x0000000000000001ull);
    attacks ^= rays[direction][square];
    return attacks;
}

u64 diagonal_attacks(u64 occ, int square) {
    return positive_ray_attacks(occ, southeast, square) | negative_ray_attacks(occ, northwest, square);
}

u64 antidiagonal_attacks(u64 occ, int square) {
    return positive_ray_attacks(occ, southwest, square) | negative_ray_attacks(occ, northeast, square);
}

u64 rank_attacks(u64 occ, int square) {
    return positive_ray_attacks(occ, east, square) | negative_ray_attacks(occ, west, square);
}

u64 file_attacks(u64 occ, int square) {
    return positive_ray_attacks(occ, south, square) | negative_ray_attacks(occ, north, square);
}

template <bool side> u64 forward_attacks(u64 occ, int square) {
    if constexpr (side) return negative_ray_attacks(occ, north, square);
    else return positive_ray_attacks(occ, south, square);
}

template u64 forward_attacks<false>(u64 occ, int square);
template u64 forward_attacks<true>(u64 occ, int square);

void init_magics() {
    for (int slider_loc{}; slider_loc < 64; ++slider_loc) { //bishops
        u64 attack_mask = bishop_premask[slider_loc];
        int num_sets = (1 << popcount(attack_mask));
        u64 temp;
        for (int index{}; index < num_sets; ++index) {
            temp = attack_mask;
            u64 occupancy = 0;
            int square{};
            for (int count = 0; count < popcount(attack_mask); count++) {
                square = pop_lsb(temp);
                if (index & (1 << count)) occupancy |= (1ull << square);
            }
            u64 magic_index = (occupancy * bishop_magics[slider_loc].magic) >> 55;
            lookup_table[bishop_magics[slider_loc].start + magic_index] = classical_bishop_attacks(occupancy, slider_loc);
        }
    }
    for (int slider_loc{}; slider_loc < 64; ++slider_loc) { //rooks
        u64 attack_mask = rook_premask[slider_loc];
        int num_sets = (1 << popcount(attack_mask));
        u64 temp;
        for (int index{}; index < num_sets; ++index) {
            temp = attack_mask;
            u64 occupancy = 0;
            int square{};
            for (int count = 0; count < popcount(attack_mask); count++) {
                square = pop_lsb(temp);
                if (index & (1 << count)) occupancy |= (1ull << square);
            }
            u64 magic_index = (occupancy * rook_magics[slider_loc].magic >> 52);
            lookup_table[rook_magics[slider_loc].start + magic_index] = classical_rook_attacks(occupancy, slider_loc);
        }
    }
}

u64 classical_rook_attacks(u64 occ, int square) {
    return file_attacks(occ, square) | rank_attacks(occ, square);
}

u64 classical_bishop_attacks(u64 occ, int square) {
    return diagonal_attacks(occ, square) | antidiagonal_attacks(occ, square);
}

u64 rook_attacks(u64 occ, int square) {
    return lookup_table[rook_magics[square].start + ((occ & rook_premask[square]) * rook_magics[square].magic >> 52)];
}

u64 bishop_attacks(u64 occ, int square) {
    return lookup_table[bishop_magics[square].start + ((occ & bishop_premask[square]) * bishop_magics[square].magic >> 55)];
}

u64 queen_attacks(u64 occ, int square) {
    return rook_attacks(occ, square) | bishop_attacks(occ, square);
}

u64 xray_rook_attacks(u64 occ, u64 blockers, int square) {
   u64 attacks = rook_attacks(occ, square);
   blockers &= attacks;
   return attacks ^ rook_attacks(occ ^ blockers, square);
}

u64 xray_bishop_attacks(u64 occ, u64 blockers, int square) {
   u64 attacks = bishop_attacks(occ, square);
   blockers &= attacks;
   return attacks ^ bishop_attacks(occ ^ blockers, square);
}
