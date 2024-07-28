#ifndef PEACEKEEPER_BOARD
#define PEACEKEEPER_BOARD

#include "fixed_vector.h"
#include "magics.h"
#include "types.h"
#include <string>

enum Piece_types {
    black_pawn,
    white_pawn,
    black_knight,
    white_knight,
    black_bishop,
    white_bishop,
    black_rook,
    white_rook,
    black_queen,
    white_queen,
    black_king,
    white_king,
    empty_square
};

enum Move_types {
    quiet = 1,
    noisy,
    all
};

class Position {
public:
    u64 pieces[13] {
        0x000000000000ff00,
        0x00ff000000000000,
        0x0000000000000042,
        0x4200000000000000,
        0x0000000000000024,
        0x2400000000000000,
        0x0000000000000081,
        0x8100000000000000,
        0x0000000000000008,
        0x0800000000000000,
        0x0000000000000010,
        0x1000000000000000,
        0xffff00000000ffff,
    };
    const u64& occupied{pieces[12]};
    int board[64] {
        6,  2,  4,  8, 10,  4,  2,  6,
        0,  0,  0,  0,  0,  0,  0,  0,
        12, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 12,
        1,  1,  1,  1,  1,  1,  1,  1,
        7,  3,  5,  9, 11,  5,  3,  7
    };
    bool side_to_move{true};
    int ply{};
    int enpassant_square[1024]{64};
    int castling_rights[1024][4]{0, 7, 56, 63};
    int halfmove_clock[1024]{0};
    u64 hash[1024]{};

    Position();
    template <bool side> u64 pawns_forward_one(u64 pawns);
    template <bool side> u64 pawns_backward_one(u64 pawns);
    template <bool side> u64 pawns_forward_two(u64 pawns);
    template <bool side> u64 pawns_backward_two(u64 pawns);
    template <bool side> u64 pawns_forward_left(u64 pawns);
    template <bool side> u64 pawns_backward_left(u64 pawns);
    template <bool side> u64 pawns_forward_right(u64 pawns);
    template <bool side> u64 pawns_backward_right(u64 pawns);
    template <bool side> u64 promotion_rank();
    template <bool side> u64 second_rank();
    u64 attacks_to(int square, u64 occ, bool side);
    u64 checkers(u64 occ);
    template <Move_types types, bool side> void generate_pawn(Movelist& movelist, u64 opp_pieces);
    template <Move_types types, bool side> void generate_stage(Movelist& movelist);
    template <bool update_hash> void remove_piece(int sq);
    template <bool update_hash> void add_piece(int sq, int piece);
    template <bool update_hash> void remove_add_piece(int sq, int piece);
    void make_move(Move move);
    void undo_move(Move move);
    bool is_legal(Move move);
    void recalculate_zobrist();
    bool load_fen(std::string fen_pos, std::string fen_stm, std::string fen_castling, std::string fen_ep, std::string fen_hmove_clock, std::string fen_fmove_counter);
    bool parse_move(Move& out, std::string move);
};

#endif
