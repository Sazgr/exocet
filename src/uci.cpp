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

void Uci::handle_quit() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Uci::handle_uci() {
    std::cout << "id name Exocet v" << VERSION << '\n';
    std::cout << "id author Kyle Zhang\n";
    std::cout << "uciok\n";
    std::cout << std::flush;
}
