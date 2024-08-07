#include "main.h"
#include "nnue.h"
#include "search.h"
#include "uci.h"
#include <cassert>
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
    nnue_init();
    Uci uci;
    if (argc > 1 && std::string{argv[1]} == "bench") {
        uci.handle_bench();
        return 0;
    } else if (argc > 1) {
        std::cout << "unsupported command-line argument \"" << argv[1] << "\"" << std::endl;
    }
    std::string command;
    std::string token;
    std::vector<std::string> tokens;
    while (true) {
        getline(std::cin, command);
        tokens.clear();
        std::istringstream parser(command);
        while (parser >> token) {tokens.push_back(token);}
        if (tokens.size() == 0) {continue;}
        if (tokens[0] == "go") {
            uci.handle_go(tokens);
        }
        if (tokens[0] == "isready") {
            uci.handle_isready();
        }
        if (tokens[0] == "perft") {
            uci.handle_perft(tokens);
        }
        if (tokens[0] == "perftsplit") {
            uci.handle_perftsplit(tokens);
        }
        if (tokens[0] == "position") {
            uci.handle_position(tokens);
        }
        if (tokens[0] == "quit") {
            uci.handle_quit();
            return 0;
        }
        if (tokens[0] == "stop") {
            uci.handle_stop();
        }
        if (tokens[0] == "uci") {
            uci.handle_uci();
        }
        if (tokens[0] == "ucinewgame") {
        }
    }
}
