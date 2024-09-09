#ifndef EXOCET_ZOBRIST
#define EXOCET_ZOBRIST

#include "types.h"
#include <array>

constexpr u64 random_step(u64 current) {
    return current * 1103515245 + 12345;
}

constexpr std::array<std::array<u64, 64>, 13> generate_zobrist_pieces() {
    u64 hash = 1;
    std::array<std::array<u64, 64>, 13> constexpr_zobrist_pieces{};
    for (int i{}; i < 12; ++i) {
        for (int j{}; j < 64; ++j) {
            hash = random_step(hash);
            constexpr_zobrist_pieces[i][j] = hash;
        }
    }
    return constexpr_zobrist_pieces;
}
constexpr std::array<std::array<u64, 64>, 13> zobrist_pieces = generate_zobrist_pieces();

constexpr u64 zobrist_black = random_step(zobrist_pieces[11][63]);

constexpr std::array<u64, 65> generate_zobrist_enpassant() {
    u64 hash = zobrist_black;
    std::array<u64, 65> constexpr_zobrist_enpassant{};
    for (int i{}; i < 64; ++i) {
        hash = random_step(hash);
        constexpr_zobrist_enpassant[i] = hash;
    }
    return constexpr_zobrist_enpassant;
}
constexpr std::array<u64, 65> zobrist_enpassant = generate_zobrist_enpassant();

constexpr std::array<u64, 65> generate_zobrist_castling() {
    u64 hash = zobrist_enpassant[63];
    std::array<u64, 65> constexpr_zobrist_castling{};
    for (int i{}; i < 8; ++i) {
        hash = random_step(hash);
        constexpr_zobrist_castling[i] = hash;
    }
    for (int i{56}; i < 64; ++i) {
        hash = random_step(hash);
        constexpr_zobrist_castling[i] = hash;
    }
    return constexpr_zobrist_castling;
}
constexpr std::array<u64, 65> zobrist_castling = generate_zobrist_castling();

#endif
