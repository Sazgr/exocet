#include "board.h"
#include "main.h"
#include "nnue.h"
#include "perft.h"
#include "timer.h"
#include "uci.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <sstream>

int main() {
    nnue_init();
    Position position;
    Move move;
    std::string command;
    std::string token;
    std::vector<std::string> tokens;
    while (true) {
        getline(std::cin, command);
        tokens.clear();
        std::istringstream parser(command);
        while (parser >> token) {tokens.push_back(token);}
        if (tokens.size() == 0) {continue;}
        if (tokens[0] == "isready") {std::cout << "readyok" << std::endl;}
        if (tokens[0] == "perft") {
            int depth = 1;
            if (tokens.size() >= 2) {depth = stoi(tokens[1]);}
            Timer timer;
            timer.reset();
            u64 result = perft(position, depth);
            double elapsed = timer.elapsed();
            std::cout << "info nodes " << result << " time " << static_cast<int>(elapsed * 1000) << " nps " << static_cast<int>(result / elapsed) << std::endl;
        }
        if (tokens[0] == "perftsplit") {
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
        if (tokens[0] == "quit") {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return 0;
        }
        if (tokens[0] == "uci") {
            handle_uci();
        }
        if (tokens[0] == "ucinewgame") {
        }
    }
}
