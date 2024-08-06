#include "perft.h"
#include "uci.h"
#include <chrono>
#include <iostream>
#include <thread>

#define VERSION "0.0.0-dev"

void Uci::handle_isready() {
    std::cout << "readyok\n";
    std::cout << std::flush;
}

void Uci::handle_perft(std::vector<std::string> tokens) {
    int depth = 1;
    if (tokens.size() >= 2) {depth = stoi(tokens[1]);}
    Timer timer;
    timer.reset();
    u64 result = perft(position, depth);
    double elapsed = timer.elapsed();
    std::cout << "info nodes " << result << " time " << static_cast<int>(elapsed * 1000) << " nps " << static_cast<int>(result / elapsed) << std::endl;
}

void Uci::handle_perftsplit(std::vector<std::string> tokens) {
    int depth = 1;
    if (tokens.size() >= 2) {depth = stoi(tokens[1]);}
    std::vector<std::pair<Move, int>> list{};
    int result = perft_split(position, depth, list);
    std::cout << result << '\n';
    std::sort(list.begin(), list.end(), [] (std::pair<Move, int> entry1, std::pair<Move, int> entry2) {return (entry1.first.start() < entry2.first.start()) || (entry1.first.start() == entry2.first.start() && entry1.first.end() < entry2.first.end());});
    for (int i{0}; i<list.size(); ++i) {
        std::cout << list[i].first << ' ' << list[i].second << '\n';
    }
}

void Uci::handle_position(std::vector<std::string> tokens) {
    Move move;
    if (tokens.size() == 1) return;
    if (tokens[1] == "startpos") position.load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", "w", "KQkq", "-", "0", "1");
    else if (tokens[1] == "kiwipete") position.load_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R", "w", "KQkq", "-", "0", "1");
    else if (tokens[1] == "fen") position.load_fen(tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], tokens[7]);
    if (std::find(tokens.begin(), tokens.end(), "moves") != tokens.end()) {
        for (auto iter = ++std::find(tokens.begin(), tokens.end(), "moves"); iter != tokens.end(); ++iter) {
            position.parse_move(move, *iter);
            position.make_move(move);
        }
    }
}

void Uci::handle_quit() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Uci::handle_uci() {
    std::cout << "id name Exocet v" << VERSION << '\n';
    std::cout << "id author Kyle Zhang\n";
    std::cout << "uciok\n";
    std::cout << std::flush;
}
