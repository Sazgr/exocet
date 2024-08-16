#ifndef EXOCET_UCI
#define PEACEKEEPER_UCI

#include "board.h"
#include "hash.h"
#include "timer.h"
#include <string>
#include <vector>

class Uci {
public:
    Position position;
    Hash_table hash_table{1};
    Limit_timer timer;

    void handle_bench();
    void handle_go(std::vector<std::string> tokens);
    void handle_isready();
    void handle_perft(std::vector<std::string> tokens);
    void handle_perftsplit(std::vector<std::string> tokens);
    void handle_position(std::vector<std::string> tokens);
    void handle_quit();
    void handle_stop();
    void handle_uci();
};

void print_score(int score);
void print_pv(Move pv[]);
void print_info(int score, int depth, u64 nodes, int nps, int time, Move pv[]);

#endif
