#include "attacks.h"
#include "bits.h"
#include "board.h"
#include "lookup.h"
#include "zobrist.h"
#include <cassert>

Position::Position() {
    init_magics();
}

template <bool side> u64 Position::pawns_forward_one(u64 pawns) {
    if constexpr (side) return pawns >> 8;
    else return pawns << 8;
}

template <bool side> u64 Position::pawns_backward_one(u64 pawns) {
    if constexpr (side) return pawns << 8;
    else return pawns >> 8;
}

template <bool side> u64 Position::pawns_forward_two(u64 pawns) {
    if constexpr (side) return pawns >> 16;
    else return pawns << 16;
}

template <bool side> u64 Position::pawns_backward_two(u64 pawns) {
    if constexpr (side) return pawns << 16;
    else return pawns >> 16;
}

template <bool side> u64 Position::pawns_forward_left(u64 pawns) {
    if constexpr (side) return (pawns & ~0x0101010101010101) >> 9;
    else return (pawns & ~0x0101010101010101) << 7;
}

template <bool side> u64 Position::pawns_backward_left(u64 pawns) {
    if constexpr (side) return (pawns & ~0x0101010101010101) << 7;
    else return (pawns & ~0x0101010101010101) >> 9;
}

template <bool side> u64 Position::pawns_forward_right(u64 pawns) {
    if constexpr (side) return (pawns & ~0x8080808080808080) >> 7;
    else return (pawns & ~0x8080808080808080) << 9;
}

template <bool side> u64 Position::pawns_backward_right(u64 pawns) {
    if constexpr (side) return (pawns & ~0x8080808080808080) << 9;
    else return (pawns & ~0x8080808080808080) >> 7;
}

template <bool side> u64 Position::promotion_rank() {
    if constexpr (side) return 0x000000000000FF00;
    else return 0x00FF000000000000;
}

template <bool side> u64 Position::second_rank() {
    if constexpr (side) return 0x00FF000000000000;
    else return 0x000000000000FF00;
}

u64 Position::attacks_to(int square, u64 occ, bool side) {
    return (pawn_attacks[!side][square] & pieces[black_pawn + side]) 
    | (knight_attacks[square] & pieces[black_knight + side])
    | (king_attacks[square] & pieces[black_king + side])
    | (bishop_attacks(occ, square) & (pieces[black_queen + side] | pieces[black_bishop + side]))
    | (rook_attacks(occ, square) & (pieces[black_queen + side] | pieces[black_rook + side]));
}

u64 Position::checkers(u64 occ) {
    int square = get_lsb(pieces[black_king + side_to_move]);
    return attacks_to(square, occ, !side_to_move);
}

template <Move_types types, bool side> void Position::generate_pawn(Movelist& movelist, u64 opp_pieces) {
    constexpr bool gen_quiet = types & 1;
    constexpr bool gen_noisy = types & 2;

    u64 promote_mask{promotion_rank<side>()};
    int ep_square = enpassant_square[ply];
    u64 ep_bb = 1ull << ep_square;

    u64 curr_board, curr_moves;
    int piece_location, end;

    if constexpr (gen_noisy) {
        //left capture
        curr_board = pieces[side] & pawns_backward_right<side>(opp_pieces);
        curr_moves = curr_board & promote_mask;
        while (curr_moves != 0) {
            piece_location = pop_lsb(curr_moves);
            if constexpr (side) end = piece_location - 9;
            else end = piece_location + 7;
            movelist.add(Move{board[piece_location], piece_location, board[end], end, knight_pr});
            movelist.add(Move{board[piece_location], piece_location, board[end], end, bishop_pr});
            movelist.add(Move{board[piece_location], piece_location, board[end], end, rook_pr});
            movelist.add(Move{board[piece_location], piece_location, board[end], end, queen_pr});
        }
        curr_moves = curr_board & ~promote_mask;
        while (curr_moves != 0) {
            piece_location = pop_lsb(curr_moves);
            if constexpr (side) end = piece_location - 9;
            else end = piece_location + 7;
            movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
        }

        //right capture
        curr_board = pieces[side] & pawns_backward_left<side>(opp_pieces);
        curr_moves = curr_board & promote_mask;
        while (curr_moves != 0) {
            piece_location = pop_lsb(curr_moves);
            if constexpr (side) end = piece_location - 9;
            else end = piece_location + 7;
            movelist.add(Move{board[piece_location], piece_location, board[end], end, knight_pr});
            movelist.add(Move{board[piece_location], piece_location, board[end], end, bishop_pr});
            movelist.add(Move{board[piece_location], piece_location, board[end], end, rook_pr});
            movelist.add(Move{board[piece_location], piece_location, board[end], end, queen_pr});
        }
        curr_moves = curr_board & ~promote_mask;
        while (curr_moves != 0) {
            piece_location = pop_lsb(curr_moves);
            if constexpr (side) end = piece_location - 7;
            else end = piece_location + 9;
            movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
        }
        curr_board = pawn_attacks[!side][ep_square] & pieces[side];//possible capturing pawns
        while (curr_board != 0) {
            piece_location = pop_lsb(curr_board);
            movelist.add(Move{board[piece_location], piece_location, 12, ep_square, enpassant});
        }
    }
    if constexpr (gen_quiet) {
        curr_board = pieces[side] & pawns_backward_one<side>(~occupied);
        curr_moves = curr_board & promote_mask;
        while (curr_moves != 0) {
            piece_location = pop_lsb(curr_moves);
            if constexpr (side) end = piece_location - 8;
            else end = piece_location + 8;
            movelist.add(Move{board[piece_location], piece_location, board[end], end, knight_pr});
            movelist.add(Move{board[piece_location], piece_location, board[end], end, bishop_pr});
            movelist.add(Move{board[piece_location], piece_location, board[end], end, rook_pr});
            movelist.add(Move{board[piece_location], piece_location, board[end], end, queen_pr});
        }
        curr_moves = curr_board & ~promote_mask;
        while (curr_moves != 0) {
            piece_location = pop_lsb(curr_moves);
            if constexpr (side) end = piece_location - 8;
            else end = piece_location + 8;
            movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
        }
        //double push
        curr_moves = curr_board & second_rank<side>() & pawns_backward_two<side>(~occupied);
        while (curr_moves != 0) {
            piece_location = pop_lsb(curr_moves);
            if constexpr (side) end = piece_location - 16;
            else end = piece_location + 16;
            movelist.add(Move{board[piece_location], piece_location, board[end], end, none});
        }
    }
}

template <Move_types types, bool side> void Position::generate_stage(Movelist& movelist) {
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
            generate_pawn<types, side>(movelist, opp_pieces);

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

            if (popcount(curr_checkers)) return; //if we are in check don't generate moves of pinned pieces and castling

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
                if (king_castle && !((occupied >> shift) & 0x90ull)) { //kingside
                    movelist.add(Move{black_king + side, king_location, empty_square, castling_rights[ply][side * 2 + 1], k_castling});
                }
                if (queen_castle && !((occupied >> shift) & 0x11ull)) { //queenside
                    movelist.add(Move{black_king + side, king_location, empty_square, castling_rights[ply][side * 2], q_castling});
                }
            }
            return;
    }
}

template void Position::generate_stage<all, false>(Movelist& movelist);
template void Position::generate_stage<all, true>(Movelist& movelist);

template <bool update_hash> void Position::remove_piece(int sq) {
    if constexpr (update_hash) {
        hash[ply] ^= zobrist_pieces[board[sq]][sq];
    }
    pieces[board[sq]] ^= (1ull << sq);
    pieces[12] ^= (1ull << sq);
    board[sq] = 12;
}

template <bool update_hash> void Position::add_piece(int sq, int piece) {
    if constexpr (update_hash) {
        hash[ply] ^= zobrist_pieces[piece][sq];
    }
    pieces[12] ^= (1ull << sq);
    pieces[piece] ^= (1ull << sq);
    board[sq] = piece;
}

template <bool update_hash> void Position::remove_add_piece(int sq, int piece) {
    if constexpr (update_hash) {
        hash[ply] ^= zobrist_pieces[board[sq]][sq] ^ zobrist_pieces[piece][sq];
    }
    pieces[board[sq]] ^= (1ull << sq);
    pieces[piece] ^= (1ull << sq);
    board[sq] = piece;
}

void Position::make_move(Move move) {
    ++ply;
    int start = move.start();
    int end = move.end();
    int piece = move.piece();
    int captured = move.captured();
    switch (move.flag()) {
        case none:
            remove_piece<true>(start);
            remove_add_piece<true>(end, piece);
            break;
        case knight_pr:
            remove_piece<true>(start);
            remove_add_piece<true>(end, piece + 2);
            break;
        case bishop_pr:
            remove_piece<true>(start);
            remove_add_piece<true>(end, piece + 4);
            break;
        case rook_pr:
            remove_piece<true>(start);
            remove_add_piece<true>(end, piece + 6);
            break;
        case queen_pr:
            remove_piece<true>(start);
            remove_add_piece<true>(end, piece + 8);
            break;
        case k_castling:
            remove_piece<true>(start);
            remove_piece<true>(end);
            add_piece<true>((start & 56) + 6, piece);
            add_piece<true>((start & 56) + 5, piece - 4);
            break;
        case q_castling:
            remove_piece<true>(start);
            remove_piece<true>(end);
            add_piece<true>((start & 56) + 2, piece);
            add_piece<true>((start & 56) + 3, piece - 4);
            break;
        case enpassant:
            remove_piece<true>(start);
            remove_piece<true>(end ^ 8);//ep square
            add_piece<true>(end, piece);
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
    side_to_move = !side_to_move;
}

void Position::undo_move(Move move) {
    side_to_move = !side_to_move;
    int start = move.start();
    int end = move.end();
    int piece = move.piece();
    int captured = move.captured();
    switch (move.flag()) {
        case none:
            add_piece<false>(start, piece);
            remove_add_piece<false>(end, captured);
            break;
        case knight_pr:
        case bishop_pr:
        case rook_pr:
        case queen_pr:
            add_piece<false>(start, piece);
            remove_add_piece<false>(end, captured);
            break;
        case k_castling:
            remove_piece<false>((start & 56) + 6);
            remove_piece<false>((start & 56) + 5);
            add_piece<false>(start, piece);
            add_piece<false>(end, piece - 4);
            break;
        case q_castling:
            remove_piece<false>((start & 56) + 2);
            remove_piece<false>((start & 56) + 3);
            add_piece<false>(start, piece);
            add_piece<false>(end, piece - 4);
            break;
        case enpassant:
            add_piece<false>(start, piece);
            remove_piece<false>(end);
            add_piece<false>(end ^ 8, piece ^ 1);
            break;
    }
    --ply;
}

/*bool Position::is_legal(Move move) {
    if (move.type() == k_castling) {
        if (move.piece() == black_king) {
            return !attackersTo(5, WHITE) && !attackersTo(6, WHITE);
        } else {
            return !attackersTo(61, BLACK) && !attackersTo(62, BLACK);
        }
    if (move.type() == q_castling) {
        if (move.piece() == black_king) {
            return !attackersTo(2, WHITE) && !attackersTo(3, WHITE);
        } else {
            return !attackersTo(58, BLACK) && !attackersTo(59, BLACK);
        }
    }

    if ((move.piece() >> 1) == 5)
        return !attackers_to(move.end(), !side_to_move, occupied ^ move.start());

    if (!checkers) {
        if (LINE_BB[from][to] & kingSquare(sideToMove))
            return true;
    }

    if (move_type(move) == MT_EN_PASSANT) {
        Square capSq = (sideToMove == WHITE ? epSquare - 8 : epSquare + 8);
        return !slidingAttackersTo(kingSquare(sideToMove), ~sideToMove, pieces() ^ from ^ capSq ^ to);
    }

    if (piece_type(movedPc) == pawn)
        return !(blockersForKing[sideToMove] & from);

    return true;
}*/
