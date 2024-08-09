#include "perft.h"
#include "search.h"
#include "uci.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#define VERSION "0.0.0-dev"

void Uci::handle_bench() {
    const static std::array<std::string, 20> fens = {
        "r3k2r/2pb1ppp/2pp1q2/p7/1nP1B3/1P2P3/P2N1PPP/R2QK2R w KQkq a6 0 14",
    };
    u64 total_nodes = 0;
    double total_time = 0.0;
    std::string token;
    std::vector<std::string> tokens;
    Search_data sd;
    for (std::string fen : fens) {
        tokens.clear();
        std::istringstream parser(fen);
        while (parser >> token) {tokens.push_back(token);}
        timer.reset(0, 0, 0, 0, 1);
        position.load_fen(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5]);
        search_root(position, timer, sd, false);
        total_nodes += sd.nodes;
        total_time += timer.elapsed();
    }
    std::cout << total_nodes << " nodes " << static_cast<int>(total_nodes / total_time) << " nps" << std::endl;
}

void Uci::handle_go(std::vector<std::string> tokens) {
    Search_data sd;
    if (std::find(tokens.begin(), tokens.end(), "infinite") != tokens.end()) {
        timer.reset();
        std::thread search_thread{search_root, std::ref(position), std::ref(timer), std::ref(sd), true};
        search_thread.detach();
        return;
    }
    int movetime = 0;
    int depth = 0;
    int nodes = 0;
    bool calculate = false;
    int wtime = 0;
    int btime = 0;
    int winc = 0;
    int binc = 0;
    int movestogo = 0;
    if (std::find(tokens.begin(), tokens.end(), "movetime") != tokens.end()) {movetime = stoi(*(++std::find(tokens.begin(), tokens.end(), "movetime")));}
    if (std::find(tokens.begin(), tokens.end(), "depth") != tokens.end()) {depth = stoi(*(++std::find(tokens.begin(), tokens.end(), "depth")));}
    if (std::find(tokens.begin(), tokens.end(), "nodes") != tokens.end()) {nodes = stoi(*(++std::find(tokens.begin(), tokens.end(), "nodes")));}
    if (std::find(tokens.begin(), tokens.end(), "wtime") != tokens.end()) {
        calculate = true;
        wtime = stoi(*(++std::find(tokens.begin(), tokens.end(), "wtime")));
    }
    if (std::find(tokens.begin(), tokens.end(), "btime") != tokens.end()) {
        calculate = true;
        btime = stoi(*(++std::find(tokens.begin(), tokens.end(), "btime")));
    }
    if (std::find(tokens.begin(), tokens.end(), "winc") != tokens.end()) {winc = stoi(*(++std::find(tokens.begin(), tokens.end(), "winc")));}
    if (std::find(tokens.begin(), tokens.end(), "binc") != tokens.end()) {binc = stoi(*(++std::find(tokens.begin(), tokens.end(), "binc")));}
    if (std::find(tokens.begin(), tokens.end(), "movestogo") != tokens.end()) {movestogo = stoi(*(++std::find(tokens.begin(), tokens.end(), "movestogo"))) + 1;}
    int mytime;
    if (movetime == 0 && calculate == true) {
        mytime = position.side_to_move ? wtime : btime;
        int myinc{position.side_to_move ? winc : binc};
        if (movestogo == 0) {movestogo = 20;}
        movetime = (mytime / movestogo + myinc * 3 / 4);
        movetime = std::max(1, movetime);
    }
    timer.reset(calculate ? std::max(1, std::min(mytime * 3 / 4, 4 * movetime)) : movetime, calculate ? movetime : 0, nodes, 0, depth);
    std::thread search_thread{search_root, std::ref(position), std::ref(timer), std::ref(sd), true};
    search_thread.detach();
}

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
    timer.stop = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Uci::handle_stop() {
    timer.stop = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Uci::handle_uci() {
    std::cout << "id name Exocet v" << VERSION << '\n';
    std::cout << "id author Kyle Zhang\n";
    std::cout << "option name Hash type spin default 1 min 1 max 1048576\n";
    std::cout << "option name Threads type spin default 1 min 1 max 1\n";
    std::cout << "uciok\n";
    std::cout << std::flush;
}

void print_score(int score) {
    if (abs(score) <= 18000) std::cout << "cp " << score / 2;
    else if (score < 0) std::cout << "mate -" << (20001 + score) / 2;
    else std::cout << "mate " << (20001 - score) / 2;
}

void print_pv(Move pv[]) {
    for (int i{}; !pv[i].is_null(); ++i) std::cout << ' ' << pv[i];
}

void print_info(int score, int depth, u64 nodes, int nps, int time, Move pv[]) {
    std::cout << "info score ";
    print_score(score);
    std::cout << " depth " << depth << " nodes " << nodes << " nps " << nps << " time " << time << " pv";
    print_pv(pv);
    std::cout << std::endl;
}
