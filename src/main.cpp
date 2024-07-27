#include "bits.h"
#include "board.h"
#include "main.h"
#include <iostream>

int main() {
    Position position;
    int depth = 6;
    //position.make_move(Move{1, 48, 12, 32});
    //position.make_move(Move{2, 1, 12, 16});
    //position.make_move(Move{1, 32, 12, 24});
    //position.make_move(Move{0, 9, 12, 25});
    /*std::vector<std::pair<Move, int>> list{};
    int result = perft_split(position, depth, list);
    std::cout << result << '\n';
    std::sort(list.begin(), list.end(), [] (std::pair<Move, int> entry1, std::pair<Move, int> entry2) {return (entry1.first.start() < entry2.first.start()) || (entry1.first.start() == entry2.first.start() && entry1.first.end() < entry2.first.end());});
    for (int i{0}; i<list.size(); ++i) {
        std::cout << list[i].first << ' ' << list[i].second << '\n';
    }*/
    for (int i{1}; i <= 6; ++i) {
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
        position.make_move(movelist[i]);
        if (!position.attacks_to(get_lsb(position.pieces[black_king + !position.side_to_move]), position.occupied, position.side_to_move)) total += perft(position, depth - 1);
        position.undo_move(movelist[i]);
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
            position.make_move(movelist[i]);
            if (!position.attacks_to(get_lsb(position.pieces[black_king + !position.side_to_move]), position.occupied, position.side_to_move)) {
                int result = perft(position, depth - 1);
                list.push_back({movelist[i], result});
                total += result;
            }
            position.undo_move(movelist[i]);
        }
        return total;
    }
}
