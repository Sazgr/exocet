#include "bits.h"
#include "board.h"
#include "main.h"
#include <iostream>

int main() {
    Position position;
    Move move;
    position.load_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R", "w", "KQkq", "-", "0", "1");
    //position.parse_move(move, "d5d6"); position.make_move(move);
    //position.parse_move(move, "h3g2"); position.make_move(move);
    //position.parse_move(move, "d6c7"); position.make_move(move);
    int depth = 5;

    /*std::vector<std::pair<Move, int>> list{};
    int result = perft_split(position, depth, list);
    std::cout << result << '\n';
    std::sort(list.begin(), list.end(), [] (std::pair<Move, int> entry1, std::pair<Move, int> entry2) {return (entry1.first.start() < entry2.first.start()) || (entry1.first.start() == entry2.first.start() && entry1.first.end() < entry2.first.end());});
    for (int i{0}; i<list.size(); ++i) {
        std::cout << list[i].first << ' ' << list[i].second << '\n';
    }//*/

    for (int i{1}; i <= depth; ++i) {
        u64 result = perft(position, i);
        std::cout << "perft " << i << " " << result << '\n';
    }//*/
}

u64 perft(Position& position, int depth) {
    if (depth == 0) {
        return 1;
    }
    u64 total{};
    Movelist movelist;
    if (position.side_to_move) position.generate_stage<all, true>(movelist);
    else position.generate_stage<all, false>(movelist);
    //if (depth == 6) std::cout << movelist.size() << '\n';
    for (int i{}; i<movelist.size(); ++i) {
        if (position.is_legal(movelist[i])) {
            position.make_move(movelist[i]);
            if (!position.attacks_to(get_lsb(position.pieces[black_king + !position.side_to_move]), position.occupied, position.side_to_move)) {
                total += perft(position, depth - 1);
            } else {
                //std::cout << movelist[i] << ' ' << movelist[i].flag() << '\n';
            }
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
                if (!position.attacks_to(get_lsb(position.pieces[black_king + !position.side_to_move]), position.occupied, position.side_to_move)) {
                    int result = perft(position, depth - 1);
                    list.push_back({movelist[i], result});
                    total += result;
                } else {
                    //std::cout << movelist[i] << ' ' << movelist[i].flag() << '\n';
                }
                position.undo_move(movelist[i]);
            }
        }
        return total;
    }
}
