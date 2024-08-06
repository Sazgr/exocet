#ifndef EXOCET_UCI
#define PEACEKEEPER_UCI

#include "board.h"
#include "timer.h"
#include <string>
#include <vector>

class Uci {
    Position position;

public:
    void handle_isready();
    void handle_perft(std::vector<std::string> tokens);
    void handle_perftsplit(std::vector<std::string> tokens);
    void handle_quit();
    void handle_uci();
};

#endif
