/*
    Copyright (C) 2024  Kyle Zhang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
        if (tokens[0] == "eval") {
            std::cout << uci.position << '\n';
            NNUE nnue;
            nnue.refresh(uci.position);
            std::cout << "NNUE: " << nnue.evaluate(uci.position.side_to_move) << std::endl;
        }
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
        if (tokens[0] == "setoption") {
            uci.handle_setoption(tokens);
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
