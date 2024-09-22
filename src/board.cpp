#include "attacks.h"
#include "bits.h"
#include "board.h"
#include "lookup.h"
#include "nnue.h"
#include "zobrist.h"
#include <cassert>

Position::Position() {
    init_magics();
    recalculate_zobrist();
}

template <bool side> u64 Position::promotion_rank() {
    if constexpr (side) return 0x000000000000FF00;
    else return 0x00FF000000000000;
}

u64 Position::attacks_to(int square, u64 occ, bool side) {
    return (pawn_attacks[!side][square] & pieces[black_pawn + side]) 
    | (knight_attacks[square] & pieces[black_knight + side])
    | (king_attacks[square] & pieces[black_king + side])
    | (bishop_attacks(occ, square) & (pieces[black_queen + side] | pieces[black_bishop + side]))
    | (rook_attacks(occ, square) & (pieces[black_queen + side] | pieces[black_rook + side]));
}

u64 Position::attacks_to(int square, u64 occ) {
    return (pawn_attacks[0][square] & pieces[white_pawn])
    | (pawn_attacks[1][square] & pieces[black_pawn]) 
    | (knight_attacks[square] & (pieces[black_knight] | pieces[white_knight]))
    | (king_attacks[square] & (pieces[black_king] | pieces[white_king]))
    | (bishop_attacks(occ, square) & (pieces[black_queen] | pieces[black_bishop] | pieces[white_queen] | pieces[white_bishop]))
    | (rook_attacks(occ, square) & (pieces[black_queen] | pieces[black_rook] | pieces[white_queen] | pieces[white_rook]));
}

u64 Position::checkers(u64 occ) {
    int square = get_lsb(pieces[black_king + side_to_move]);
    return attacks_to(square, occ, !side_to_move);
}

bool Position::check() {
    return checkers(occupied);
}

bool Position::draw(int num_reps) {
    if (halfmove_clock[ply] < 8) return false;
    if (halfmove_clock[ply] >= 100) return true;
    u64 curr_hash = hash[ply];
    int repeats{};
    for (int i{ply - 4}; i >= 0 && i >= ply - halfmove_clock[ply] && repeats < num_reps; i -= 2) repeats += (hash[i] == curr_hash);
    return (repeats >= num_reps);
}

template <Move_types types, bool side> void Position::generate_stage_side(Movelist& movelist) {
    assert(popcount(pieces[black_king]) == 1);
    assert(popcount(pieces[white_king]) == 1);
    constexpr bool gen_quiet = types & 1;
    constexpr bool gen_noisy = types & 2;

    movelist.clear();

    u64 own_pieces{pieces[side] | pieces[side + 2] | pieces[side + 4] | pieces[side + 6] | pieces[side + 8] | pieces[side + 10]};
    u64 opp_pieces{occupied ^ own_pieces};

    u64 targets{};
    if constexpr (gen_quiet) targets |= ~occupied;
    if constexpr (gen_noisy) targets |= opp_pieces;

    u64 curr_board, curr_moves;
    int piece_location;
    int end;
    int king_location = get_lsb(pieces[black_king + side]);

    u64 promote_mask{promotion_rank<side>()};
    int ep_square = enpassant_square[ply];

    u64 hv_pinmask{(xray_rook_attacks(occupied, own_pieces, king_location) & (pieces[black_rook + !side] | pieces[black_queen + !side]))};
    u64 dd_pinmask{(xray_bishop_attacks(occupied, own_pieces, king_location) & (pieces[black_bishop + !side] | pieces[black_queen + !side]))};
    curr_board = hv_pinmask;
    while (curr_board) {
        piece_location = pop_lsb(curr_board);
        hv_pinmask |= between[piece_location][king_location];
    }
    curr_board = dd_pinmask;
    while (curr_board) {
        piece_location = pop_lsb(curr_board);
        dd_pinmask |= between[piece_location][king_location];
    }
    u64 not_pinned{~(hv_pinmask | dd_pinmask)};

    u64 curr_checkers{checkers(occupied)};

    //king moves
    curr_moves = king_attacks[king_location] & targets;
    while (curr_moves != 0) {
        end = pop_lsb(curr_moves);
        movelist.add(Move{board[king_location], king_location, board[end], end, none});
    }

    switch (popcount(curr_checkers)) {
        case 2:
            return; //double check
        case 1: //single check
            targets &= between[get_lsb(curr_checkers)][king_location] ^ curr_checkers;
        case 0: // no check
            curr_board = pieces[side] & ~promote_mask & not_pinned;//non-pinned non-promoting pawns
            while (curr_board != 0) {
                piece_location = pop_lsb(curr_board);
                if constexpr (gen_noisy) curr_moves = pawn_attacks[side][piece_location] & opp_pieces;
                if constexpr (gen_quiet) curr_moves |= forward_attacks<side>(occupied, piece_location) & pawn_pushes[side][piece_location] & ~occupied;
                curr_moves &= targets;
                while (curr_moves != 0) {
                    end = pop_lsb(curr_moves);
                    movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
                }
            }
            curr_board = pieces[side] & promote_mask & not_pinned;//non-pinned promoting pawns
            while (curr_board != 0) {
                piece_location = pop_lsb(curr_board);
                if constexpr (gen_noisy) curr_moves = pawn_attacks[side][piece_location] & opp_pieces;
                if constexpr (gen_quiet) curr_moves |= pawn_pushes[side][piece_location] & ~occupied;
                curr_moves &= targets;
                while (curr_moves != 0) {
                    end = pop_lsb(curr_moves);
                    movelist.add(Move{board[piece_location], piece_location, board[end], end, knight_pr});
                    movelist.add(Move{board[piece_location], piece_location, board[end], end, bishop_pr});
                    movelist.add(Move{board[piece_location], piece_location, board[end], end, rook_pr});
                    movelist.add(Move{board[piece_location], piece_location, board[end], end, queen_pr});
                }
            }

            //non-pinned knights
            curr_board = pieces[side + 2] & not_pinned;
            while (curr_board != 0) {
                piece_location = pop_lsb(curr_board);
                curr_moves = knight_attacks[piece_location] & targets;
                while (curr_moves != 0) {
                    end = pop_lsb(curr_moves);
                    movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
                }
            }

            //non-pinned diagonal sliders
            curr_board = (pieces[side + 4] | pieces[side + 8]) & not_pinned;
            while (curr_board != 0) {
                piece_location = pop_lsb(curr_board);
                curr_moves = bishop_attacks(occupied, piece_location) & targets;
                while (curr_moves != 0) {
                    end = pop_lsb(curr_moves);
                    movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
                }
            }

            //non-pinned horizontal sliders
            curr_board = (pieces[side + 6] | pieces[side + 8]) & not_pinned;
            while (curr_board != 0) {
                piece_location = pop_lsb(curr_board);
                curr_moves = rook_attacks(occupied, piece_location) & targets;
                while (curr_moves != 0) {
                    end = pop_lsb(curr_moves);
                    movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
                }
            }

            //en passant
            if constexpr (gen_noisy) {
                curr_board = pawn_attacks[!side][ep_square] & pieces[side];//possible capturing pawns
                while (curr_board != 0) {
                    piece_location = pop_lsb(curr_board);
                    movelist.add(Move{board[piece_location], piece_location, 12, ep_square, enpassant});
                }
            }
            if (popcount(curr_checkers)) return; //if we are in check don't generate moves of pinned pieces and castling

            //pinned non-promoting pawns
            if constexpr (gen_quiet) {
                curr_board = pieces[side] & ~promote_mask & hv_pinmask;
                while (curr_board != 0) {
                    piece_location = pop_lsb(curr_board);
                    curr_moves |= forward_attacks<side>(occupied, piece_location) & pawn_pushes[side][piece_location] & (~occupied) & hv_pinmask;
                    while (curr_moves != 0) {
                        end = pop_lsb(curr_moves);
                        movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
                    }
                }
            }
            if constexpr (gen_noisy) {
                curr_board = pieces[side] & ~promote_mask & dd_pinmask;
                while (curr_board != 0) {
                    piece_location = pop_lsb(curr_board);
                    curr_moves = pawn_attacks[side][piece_location] & opp_pieces & dd_pinmask;
                    while (curr_moves != 0) {
                        end = pop_lsb(curr_moves);
                        movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
                    }
                }
            }

            //pinned promoting pawns
            curr_board = pieces[side] & promote_mask & dd_pinmask;
            //note: hv-pinned pawns on 7th rank cannot push (or capture) in either case
            if constexpr (gen_noisy) {
                while (curr_board != 0) {
                    piece_location = pop_lsb(curr_board);
                    curr_moves = pawn_attacks[side][piece_location] & opp_pieces & dd_pinmask;
                    while (curr_moves != 0) {
                        end = pop_lsb(curr_moves);
                        movelist.add(Move{board[piece_location], piece_location, board[end], end, knight_pr});
                        movelist.add(Move{board[piece_location], piece_location, board[end], end, bishop_pr});
                        movelist.add(Move{board[piece_location], piece_location, board[end], end, rook_pr});
                        movelist.add(Move{board[piece_location], piece_location, board[end], end, queen_pr});
                    }
                }
            }

            //note: pinned knights cannot move

            //pinned diagonal sliders
            curr_board = (pieces[side + 4] | pieces[side + 8]) & dd_pinmask;
            while (curr_board != 0) {
                piece_location = pop_lsb(curr_board);
                curr_moves = bishop_attacks(occupied, piece_location) & targets & dd_pinmask;
                while (curr_moves != 0) {
                    end = pop_lsb(curr_moves);
                    movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
                }
            }

            //pinned horizontal sliders
            curr_board = (pieces[side + 6] | pieces[side + 8]) & hv_pinmask;
            while (curr_board != 0) {
                piece_location = pop_lsb(curr_board);
                curr_moves = rook_attacks(occupied, piece_location) & targets & hv_pinmask;
                while (curr_moves != 0) {
                    end = pop_lsb(curr_moves);
                    movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
                }
            }

            if constexpr (gen_quiet) { //castling
                bool king_castle, queen_castle;
                int shift;
                if constexpr (side) {
                    king_castle = castling_rights[ply][3] != 64;
                    queen_castle = castling_rights[ply][2] != 64;
                    shift = 56;
                } else {
                    king_castle = castling_rights[ply][1] != 64;
                    queen_castle = castling_rights[ply][0] != 64;
                    shift = 0;
                }
                if (king_castle && !((occupied >> shift) & 0x60ull)) { //kingside
                    movelist.add(Move{black_king + side, king_location, empty_square, castling_rights[ply][side * 2 + 1], k_castling});
                }
                if (queen_castle && !((occupied >> shift) & 0xeull)) { //queenside
                    movelist.add(Move{black_king + side, king_location, empty_square, castling_rights[ply][side * 2], q_castling});
                }
            }
            return;
    }
}

template <Move_types types> void Position::generate_stage(Movelist& movelist) {
    if (side_to_move) generate_stage_side<types, true>(movelist);
    else generate_stage_side<types, false>(movelist);
}

template void Position::generate_stage<quiet>(Movelist& movelist);
template void Position::generate_stage<noisy>(Movelist& movelist);
template void Position::generate_stage<all>(Movelist& movelist);

template <bool update_nnue, bool update_hash> void Position::remove_piece(int sq) {
    if constexpr (update_hash) {
        hash[ply] ^= zobrist_pieces[board[sq]][sq];
        corrhist_hash[ply] ^= zobrist_corrhist[board[sq]][sq];
    }
    if constexpr (update_nnue) {
        if (board[sq] != empty_square) nnue_sub.push_back({index(board[sq], sq, 0, king_square[0]), index(board[sq], sq, 1, king_square[1])});
    }
    pieces[board[sq]] ^= (1ull << sq);
    pieces[12] ^= (1ull << sq);
    board[sq] = 12;
}

template <bool update_nnue, bool update_hash> void Position::add_piece(int sq, int piece) {
    if constexpr (update_hash) {
        hash[ply] ^= zobrist_pieces[piece][sq];
        corrhist_hash[ply] ^= zobrist_corrhist[piece][sq];
    }
    if constexpr (update_nnue) {
        if (piece != empty_square) nnue_add.push_back({index(piece, sq, 0, king_square[0]), index(piece, sq, 1, king_square[1])});
    }
    pieces[12] ^= (1ull << sq);
    pieces[piece] ^= (1ull << sq);
    board[sq] = piece;
}

template <bool update_nnue, bool update_hash> void Position::remove_add_piece(int sq, int piece) {
    if constexpr (update_hash) {
        hash[ply] ^= zobrist_pieces[board[sq]][sq] ^ zobrist_pieces[piece][sq];
        corrhist_hash[ply] ^= zobrist_corrhist[board[sq]][sq] ^ zobrist_corrhist[piece][sq];
    }
    if constexpr (update_nnue) {
        if (board[sq] != empty_square) nnue_sub.push_back({index(board[sq], sq, 0, king_square[0]), index(board[sq], sq, 1, king_square[1])});
        if (piece != empty_square) nnue_add.push_back({index(piece, sq, 0, king_square[0]), index(piece, sq, 1, king_square[1])});
    }
    pieces[board[sq]] ^= (1ull << sq);
    pieces[piece] ^= (1ull << sq);
    board[sq] = piece;
}

template <bool update_nnue> void Position::make_move(Move move, NNUE* nnue) {
    if constexpr (update_nnue) {
        assert(nnue_sub.empty());
        nnue->push();
    }
    ++ply;
    hash[ply] = hash[ply - 1];
    corrhist_hash[ply] = corrhist_hash[ply - 1];
    hash[ply] ^= zobrist_black;
    int start = move.start();
    int end = move.end();
    int piece = move.piece();
    int captured = move.captured();
    int king_end = end;
    switch (move.flag()) {
        case none:
            remove_piece<update_nnue, true>(start);
            remove_add_piece<update_nnue, true>(end, piece);
            break;
        case knight_pr:
            remove_piece<update_nnue, true>(start);
            remove_add_piece<update_nnue, true>(end, piece + 2);
            break;
        case bishop_pr:
            remove_piece<update_nnue, true>(start);
            remove_add_piece<update_nnue, true>(end, piece + 4);
            break;
        case rook_pr:
            remove_piece<update_nnue, true>(start);
            remove_add_piece<update_nnue, true>(end, piece + 6);
            break;
        case queen_pr:
            remove_piece<update_nnue, true>(start);
            remove_add_piece<update_nnue, true>(end, piece + 8);
            break;
        case k_castling:
            remove_piece<update_nnue, true>(start);
            remove_piece<update_nnue, true>(end);
            add_piece<update_nnue, true>((start & 56) + 6, piece);
            add_piece<update_nnue, true>((start & 56) + 5, piece - 4);
            king_end = (start & 56) + 6;
            break;
        case q_castling:
            remove_piece<update_nnue, true>(start);
            remove_piece<update_nnue, true>(end);
            add_piece<update_nnue, true>((start & 56) + 2, piece);
            add_piece<update_nnue, true>((start & 56) + 3, piece - 4);
            king_end = (start & 56) + 2;
            break;
        case enpassant:
            remove_piece<update_nnue, true>(start);
            remove_piece<update_nnue, true>(end ^ 8);//ep square
            add_piece<update_nnue, true>(end, piece);
            break;
    }
    enpassant_square[ply] = (!(piece & ~1) && end == (start ^ 16)) ? (end ^ 8) : 64;
    memcpy(castling_rights[ply], castling_rights[ply - 1], sizeof(int) * 4);
    if (castling_rights[ply][0] != 64 && board[castling_rights[ply][0]] != black_rook) castling_rights[ply][0] = 64;
    if (castling_rights[ply][1] != 64 && board[castling_rights[ply][1]] != black_rook) castling_rights[ply][1] = 64;
    if (castling_rights[ply][2] != 64 && board[castling_rights[ply][2]] != white_rook) castling_rights[ply][2] = 64;
    if (castling_rights[ply][3] != 64 && board[castling_rights[ply][3]] != white_rook) castling_rights[ply][3] = 64;
    if (piece == black_king) {
        castling_rights[ply][0] = 64;
        castling_rights[ply][1] = 64;
    }
    if (piece == white_king) {
        castling_rights[ply][2] = 64;
        castling_rights[ply][3] = 64;
    }
    halfmove_clock[ply] = ((!(piece & ~1) || captured != 12) ? 0 : halfmove_clock[ply - 1] + 1);
    for (int i{}; i<4; ++i) {
        hash[ply] ^= zobrist_castling[castling_rights[ply - 1][i]] ^ zobrist_castling[castling_rights[ply][i]];
    }
    hash[ply] ^= zobrist_enpassant[enpassant_square[ply - 1]] ^ zobrist_enpassant[enpassant_square[ply]];
    nnue_refresh = 0;
    if constexpr (update_nnue) if (piece == black_king + side_to_move && (((start ^ king_end) & 4) || (buckets > 1 && king_buckets[start ^ (56 * side_to_move)] != king_buckets[king_end ^ (56 * side_to_move)]))) {
        nnue_refresh = 1 + side_to_move;
    }
    king_square[0] = get_lsb(pieces[10]);
    king_square[1] = get_lsb(pieces[11]);
    side_to_move = !side_to_move;
}

template void Position::make_move<false>(Move move, NNUE* nnue);
template void Position::make_move<true>(Move move, NNUE* nnue);

template <bool update_nnue> void Position::undo_move(Move move, NNUE* nnue) {
    if constexpr (update_nnue) nnue->pop();
    side_to_move = !side_to_move;
    int start = move.start();
    int end = move.end();
    int piece = move.piece();
    int captured = move.captured();
    switch (move.flag()) {
        case none:
            add_piece<update_nnue, false>(start, piece);
            remove_add_piece<update_nnue, false>(end, captured);
            break;
        case knight_pr:
        case bishop_pr:
        case rook_pr:
        case queen_pr:
            add_piece<update_nnue, false>(start, piece);
            remove_add_piece<update_nnue, false>(end, captured);
            break;
        case k_castling:
            remove_piece<update_nnue, false>((start & 56) + 6);
            remove_piece<update_nnue, false>((start & 56) + 5);
            add_piece<update_nnue, false>(start, piece);
            add_piece<update_nnue, false>(end, piece - 4);
            break;
        case q_castling:
            remove_piece<update_nnue, false>((start & 56) + 2);
            remove_piece<update_nnue, false>((start & 56) + 3);
            add_piece<update_nnue, false>(start, piece);
            add_piece<update_nnue, false>(end, piece - 4);
            break;
        case enpassant:
            add_piece<update_nnue, false>(start, piece);
            remove_piece<update_nnue, false>(end);
            add_piece<update_nnue, false>(end ^ 8, piece ^ 1);
            break;
    }
    king_square[0] = get_lsb(pieces[10]);
    king_square[1] = get_lsb(pieces[11]);
    nnue_sub.clear();
    nnue_add.clear();
    --ply;
}

template void Position::undo_move<false>(Move move, NNUE* nnue);
template void Position::undo_move<true>(Move move, NNUE* nnue);

void Position::make_null() {
    ++ply;
    hash[ply] = hash[ply - 1];
    hash[ply] ^= zobrist_enpassant[enpassant_square[ply - 1]];
    hash[ply] ^= zobrist_black;
    side_to_move = !side_to_move;
    memcpy(castling_rights[ply], castling_rights[ply - 1], sizeof(int) * 4);
    enpassant_square[ply] = 64;
    hash[ply] ^= zobrist_enpassant[64];
}

void Position::undo_null() {
    side_to_move = !side_to_move;
    --ply;
}

bool Position::is_legal(Move move) {
    if (move.flag() == k_castling) {
        return !attacks_to((move.start() & 56) + 5, occupied, !side_to_move) && !attacks_to((move.start() & 56) + 6, occupied, !side_to_move);
    }
    if (move.flag() == q_castling) {
        return !attacks_to((move.start() & 56) + 2, occupied, !side_to_move) && !attacks_to((move.start() & 56) + 3, occupied, !side_to_move);
    }

    //king moves
    if ((move.piece() >> 1) == 5) {
        return !attacks_to(move.end(), occupied ^ (1ull << move.start()), !side_to_move);
    }

    if (move.flag() == enpassant) {
        return !(attacks_to(get_lsb(pieces[black_king + side_to_move]), occupied ^ (1ull << move.start()) ^ (1ull << move.end()) ^ (1ull << (move.end() ^ 8)), !side_to_move) & ~(1ull << (move.end() ^ 8)));
    }

    return true;
}

void Position::nnue_update_accumulator(NNUE& nnue) {
    if (nnue_refresh) nnue.refresh_side(nnue_refresh - 1, *this);
    if (nnue_sub.empty()) return;
    if (nnue_sub.size() > nnue_add.size()) {
        nnue.update_accumulator_sub_sub_add(3 - nnue_refresh, nnue_sub[0], nnue_sub[1], nnue_add[0]);
        nnue_sub.pop_back();
        nnue_sub.pop_back();
        nnue_add.pop_back();
        return;
    }
    while (!nnue_sub.empty()) {
        nnue.update_accumulator_sub_add(3 - nnue_refresh, nnue_sub.back(), nnue_add.back());
        nnue_sub.pop_back();
        nnue_add.pop_back();
    }
}

int Position::static_eval(NNUE& nnue) {
    nnue_update_accumulator(nnue);
    return nnue.evaluate(side_to_move);
}

void Position::recalculate_zobrist() {
    hash[ply] = 0;
    for (int i{0}; i < 64; ++i) hash[ply] ^= zobrist_pieces[board[i]][i];
    if (!side_to_move) hash[ply] ^= zobrist_black;
    for (int i{}; i < 4; ++i) {
        hash[ply] ^= zobrist_castling[castling_rights[ply][i]];
    }
    hash[ply] ^= zobrist_enpassant[enpassant_square[ply]];

    corrhist_hash[ply] = 0;
    for (int i{0}; i < 64; ++i) corrhist_hash[ply] ^= zobrist_corrhist[board[i]][i];
}

u64 Position::hashkey() {
    return hash[ply];
}

u64 Position::corrhist_hashkey() {
    return corrhist_hash[ply];
}

bool Position::load_fen(std::string fen_pos, std::string fen_stm, std::string fen_castling, std::string fen_ep, std::string fen_hmove_clock, std::string fen_fmove_counter) {
    int sq = 0;
    ply = 0;

    for (int i{}; i<64; ++i) remove_piece<false, false>(i);
    for (auto pos = fen_pos.begin(); pos != fen_pos.end(); ++pos) {
        switch (*pos) {
            case 'p': add_piece<false, false>(sq, black_pawn); break;
            case 'n': add_piece<false, false>(sq, black_knight); break;
            case 'b': add_piece<false, false>(sq, black_bishop); break;
            case 'r': add_piece<false, false>(sq, black_rook); break;
            case 'q': add_piece<false, false>(sq, black_queen); break;
            case 'k': add_piece<false, false>(sq, black_king); break;
            case 'P': add_piece<false, false>(sq, white_pawn); break;
            case 'N': add_piece<false, false>(sq, white_knight); break;
            case 'B': add_piece<false, false>(sq, white_bishop); break;
            case 'R': add_piece<false, false>(sq, white_rook); break;
            case 'Q': add_piece<false, false>(sq, white_queen); break;
            case 'K': add_piece<false, false>(sq, white_king); break;
            case '/': --sq; break;
            case '1': break;
            case '2': ++sq; break;
            case '3': sq += 2; break;
            case '4': sq += 3; break;
            case '5': sq += 4; break;
            case '6': sq += 5; break;
            case '7': sq += 6; break;
            case '8': sq += 7; break;
            default: return false;
        }
        ++sq;
    }
    king_square[0] = get_lsb(pieces[10]);
    king_square[1] = get_lsb(pieces[11]);

    if (fen_stm == "w") side_to_move = true;
    else if (fen_stm == "b") side_to_move = false;
    else return false;

    castling_rights[0][0] = castling_rights[0][1] = castling_rights[0][2] = castling_rights[0][3] = 64;
    for (auto pos = fen_castling.begin(); pos != fen_castling.end(); ++pos) {
        switch (*pos) {
            case '-': break;
            case 'q': castling_rights[0][0] = 0; break;
            case 'k': castling_rights[0][1] = 7; break;
            case 'Q': castling_rights[0][2] = 56; break;
            case 'K': castling_rights[0][3] = 63; break;
            default: return false;
        }
    }

    if (fen_ep == "-") enpassant_square[0] = 64;
    else if (fen_ep.size() == 2) enpassant_square[0] = (static_cast<int>(fen_ep[0]) - 97) + 8 * (56 - static_cast<int>(fen_ep[1])); //ascii 'a' = 97 '8' = 56
    else return false;

    halfmove_clock[0] = stoi(fen_hmove_clock);
    ply_counter = 2 * stoi(fen_fmove_counter) + !side_to_move;
    recalculate_zobrist();
    return true;
}

bool Position::parse_move(Move& out, std::string move) {
    if (move.size() < 4 || move.size() > 5) return false;
    int start = (static_cast<int>(move[0]) - 97) + 8 * (56 - static_cast<int>(move[1]));//ascii 'a' = 97 , '8' = 56
    int end = (static_cast<int>(move[2]) - 97) + 8 * (56 - static_cast<int>(move[3]));
    if ((~63 & start) || (~63 & end)) return false; //out-of-bound squares
    int piece = board[start];
    int captured = board[end];
    int flag{none};
    if (move.size() == 5) {
        switch (move[4]) {
            case 'n': flag = knight_pr; break;
            case 'b': flag = bishop_pr; break;
            case 'r': flag = rook_pr; break;
            case 'q': flag = queen_pr; break;
        }
    } else {
        if (piece == black_king + side_to_move && end - start == 2) {
            flag = k_castling;
            end = start + 3;
            captured = empty_square;
        }
        if (piece == black_king + side_to_move && end - start == -2) {
            flag = q_castling;
            end = start - 4;
            captured = empty_square;
        }
        if (piece == black_pawn + side_to_move && (abs(end - start) == 7 || abs(end - start) == 9) && captured == empty_square) {
            flag = enpassant;
            captured = piece ^ 1;
        }
    }
    out = Move{piece, start, captured, end, flag};
    return true;
}

std::ostream& operator<<(std::ostream& out, Position& position) {
    static std::vector<std::string> pieces{"p", "P", "n", "N", "b", "B", "r", "R", "q", "Q", "k", "K", ".", "."};
    out << "8 ";
    for (int square{0}; square < 64; ++square) {
        out << pieces[position.board[square]] << ' ';
        if ((square & 7) == 7) out << '\n' << (7 - (square >> 3)) << ' ';
    }
    out << "a b c d e f g h\n";
    return out;
}
