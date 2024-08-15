#include "search.h"
#include "uci.h"

int qsearch(Position& position, Search_stack* ss, Search_data& sd, int alpha, int beta) {
    if ((*sd.timer).stopped() || (!(sd.nodes & 4095) && (*sd.timer).check(sd.nodes, 0))) return 0;
    bool in_check = position.check();
    int static_eval = position.static_eval(*sd.nnue);
    int score = -20001;
    int best_score = -20001;
    if (!in_check) { //stand pat
        if (static_eval >= beta) return static_eval;
        if (static_eval > best_score) best_score = static_eval;
        if (static_eval > alpha) alpha = static_eval;
    }
    int legal_moves = 0;
    Move best_move{};
    Movelist movelist;
    if (in_check) {
        position.generate_stage<all>(movelist);
    } else {
        position.generate_stage<noisy>(movelist);
    }
    for (int i{}; i < movelist.size(); ++i) {
        if (!position.is_legal(movelist[i])) continue;
        position.make_move<true>(movelist[i], sd.nnue);
        ss->move = movelist[i];
        bool gives_check = position.check();
        ++sd.nodes;
        ++legal_moves;
        (ss + 1)->ply = ss->ply + 1;
        score = -qsearch(position, ss + 1, sd, -beta, -alpha);
        position.undo_move<true>(movelist[i], sd.nnue);
        if ((*sd.timer).stopped()) return 0;
        if (score > best_score) {
            best_score = score;
             if (score > alpha) {
                alpha = score;
                best_move = movelist[i];
                if (score > beta) {
                    return score;
                }
            }
        }
    }
    if (in_check && legal_moves == 0) {
        return -20000 + ss->ply;
    }
    return (*sd.timer).stopped() ? 0 : best_score;
}

int search(Position& position, Search_stack* ss, Search_data& sd, int depth, int alpha, int beta) {
    bool is_root = (ss->ply == 0);
    bool is_pv = (beta - alpha) != 1;
    if ((*sd.timer).stopped() || (!(sd.nodes & 4095) && (*sd.timer).check(sd.nodes, 0))) return 0;
    if (depth <= 0) {
        return qsearch(position, ss, sd, alpha, beta);
    }
    if (depth == 1 && is_pv) sd.pv_table[ss->ply + 1][0] = Move{};
    if (position.draw(ss->ply > 2 ? 1 : 2)) {
        sd.pv_table[ss->ply][0] = Move{};
        return 0;
    }
    bool in_check = position.check();
    int score = -20001;
    int best_score = -20001;
    int legal_moves = 0;
    Move best_move{};
    Movelist movelist;
    position.generate_stage<all>(movelist);
    for (int i{}; i < movelist.size(); ++i) {
        if (movelist[i].captured() != 12) movelist[i].add_sortkey(10000 + movelist[i].mvv_lva());
        else movelist[i].add_sortkey(0);
    }
    movelist.sort(0, movelist.size());
    for (int i{}; i < movelist.size(); ++i) {
        if (!position.is_legal(movelist[i])) continue;
        position.make_move<true>(movelist[i], sd.nnue);
        ss->move = movelist[i];
        bool gives_check = position.check();
        ++sd.nodes;
        ++legal_moves;
        (ss + 1)->ply = ss->ply + 1;
        score = -search(position, ss + 1, sd, depth - 1, -beta, -alpha);
        position.undo_move<true>(movelist[i], sd.nnue);
        if ((*sd.timer).stopped()) return 0;
        if (score > best_score) {
            best_score = score;
             if (score > alpha) {
                alpha = score;
                best_move = movelist[i];
                if (is_pv) {
                    sd.pv_table[ss->ply][0] = best_move;
                    memcpy(&sd.pv_table[ss->ply][1], &sd.pv_table[ss->ply + 1][0], sizeof(Move) * 127);
                }
                if (score > beta) {
                    return score;
                }
            }
        }
    }
    if (legal_moves == 0) {
        sd.pv_table[ss->ply][0] = Move{};
        if (in_check) return -20000 + ss->ply;
        else return 0;
    }
    return (*sd.timer).stopped() ? 0 : best_score;
}

void search_root(Position& position, Limit_timer& timer, Search_data& sd, bool output) {
    Search_stack ss[96];
    ss[4].ply = 0;
    NNUE nnue;
    nnue.refresh(position);
    sd.nnue = &nnue;
    sd.timer = &timer;
    int score;
    Movelist movelist;
    position.generate_stage<all>(movelist);
    Move best_move = movelist[0];
    int alpha = -20001;
    int beta = 20001;
    for (int depth = 1; depth < 64; ++depth) {
        if (depth > 1 && timer.check(sd.nodes, depth)) break;
        score = search(position, &ss[4], sd, depth, alpha, beta);
        if (timer.stopped()) break;
        best_move = sd.pv_table[0][0];
        if (output) print_info(score, depth, sd.nodes, static_cast<int>(sd.nodes / timer.elapsed()), static_cast<int>(timer.elapsed() * 1000), sd.pv_table[0]);
    }
    if (output) std::cout << "bestmove " << best_move << std::endl;
    sd.nnue = nullptr;
    sd.timer = nullptr;
}