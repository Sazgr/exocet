#ifndef EXOCET_HASH
#define EXOCET_HASH

#include "bits.h"
#include "move.h"
#include "types.h"
#include <vector>

enum Transposition_entry_types {
    tt_exact,
    tt_alpha,
    tt_beta,
    tt_none
};

struct Entry {
    u64 full_hash;
    u64 data;
    Entry() {
        data = (static_cast<u64>(tt_none) << 40) | (Move{}.data & 0xFFFFFF);
    }
    Entry(u64 hash, Move move, int sc, u8 tp, u8 dp) {
        full_hash = hash;
        data = (static_cast<u64>(dp) << 48) | (static_cast<u64>(tp) << 40) | (static_cast<u64>(static_cast<u16>(static_cast<i16>(sc))) << 24) | (move.data & 0xFFFFFF);
    }
    Entry& operator=(const Entry& rhs) {
        full_hash = rhs.full_hash;
        data = rhs.data;
        return *this;
    }
    int depth() {
        return (data >> 48) & 0xFFull;
    }
    int type() {
        return (data >> 40) & 0xFFull;
    }
    int score() {
        return static_cast<i16>(static_cast<u16>((data >> 24) & 0xFFFFull));
    }
    Move move() {
        return Move(data & 0xFFFFFFull);
    }
};

class Hash_table {
public:
    explicit Hash_table(u64 sz) {
        size = 1ull << get_msb((sz * 1048576) / 16);
        table.resize(size);
    }
    void resize(u64 sz) {
        size = 1ull << get_msb((sz * 1048576) / 16);
        table.resize(size);
    }
    void prefetch(const u64 hash) const {
        __builtin_prefetch(&table[hash & (size - 1)]);
    }
    Entry probe(const u64 hash) {
        return table[hash & (size - 1)];
    }
    void clear() {
        for (int i{0}; i<size; ++i) table[i] = Entry{};
    }
    void insert(const u64 hash, int score, u8 type, Move move, u8 dp) {
        Entry previous = table[hash & (size - 1)];
        if (hash == previous.full_hash && move.is_null()) {
            move = previous.move();
        }
        table[hash & (size - 1)] = Entry{hash, move, score, type, dp};
    }
private:
    u64 size;
    std::vector<Entry> table;
};

#endif
