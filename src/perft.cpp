#include "bits.h"
#include "perft.h"
#include <cassert>

u64 perft(Position& position, int depth) {
    if (depth == 0) {
        return 1;
    }
    /*if (depth == 1) { //bulk counting
        u64 total{};
        Movelist movelist;
        if (position.side_to_move) position.generate_stage<all, true>(movelist);
        else position.generate_stage<all, false>(movelist);
        for (int i{}; i<movelist.size(); ++i) {
            if (position.is_legal(movelist[i])) {
                ++total;
            }
        }
        return total;
    }*/
    u64 total{};
    Movelist movelist;
    if (position.side_to_move) position.generate_stage<all, true>(movelist);
    else position.generate_stage<all, false>(movelist);
    for (int i{}; i<movelist.size(); ++i) {
        if (position.is_legal(movelist[i])) {
            position.make_move(movelist[i]);
            assert(!position.attacks_to(get_lsb(position.pieces[black_king + !position.side_to_move]), position.occupied, position.side_to_move));
            total += perft(position, depth - 1);
            position.undo_move(movelist[i]);
        }
    }
    return total;
}

u64 perft_split(Position& position, int depth, std::vector<std::pair<Move, int>>& list) {
    if (depth == 0) {
        return 1;
    } else {
        u64 total{};
        Movelist movelist;
        if (position.side_to_move) position.generate_stage<all, true>(movelist);
        else position.generate_stage<all, false>(movelist);
        for (int i{}; i < movelist.size(); ++i) {
            if (position.is_legal(movelist[i])) {
                position.make_move(movelist[i]);
                assert(!position.attacks_to(get_lsb(position.pieces[black_king + !position.side_to_move]), position.occupied, position.side_to_move));
                int result = perft(position, depth - 1);
                list.push_back({movelist[i], result});
                total += result;
                position.undo_move(movelist[i]);
            }
        }
        return total;
    }
}
