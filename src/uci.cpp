#include "uci.h"
#include <iostream>

#define VERSION "0.1.0-dev"

void handle_uci() {
    std::cout << "id name Exocet v" << VERSION << '\n';
    std::cout << "id author Kyle Zhang\n";
    std::cout << "uciok\n";
    std::cout << std::flush;
}
