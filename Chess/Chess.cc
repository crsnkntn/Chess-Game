#include "Chess.h"

// TODO

/*
    Implement the play function
    Implement the get_move functions for both cpu and human
    Implement the in_check logic
    Implement the game end condition
    Implement en passant
    Implement castling

    Read more about Monte Carlo Tree Search
    Implement a basic mcts for the bot

*/

using namespace Chess;

Player::Player (bool h) : is_human(h), currentSelection(Action(-1, -1)) {}

void Player::process_click (int click) {
    std::cout << "Before Click: " << click << "  (" << currentSelection.src << ", " << currentSelection.dest << ")" << std::endl;
    if (click == currentSelection.src) {
        currentSelection.src = -1;
        currentSelection.dest = -1;
    }
    else if (currentSelection.src == -1)
        currentSelection.src = click;
    else
        currentSelection.dest = click;
    std::cout << "After Click: " << click << "  (" << currentSelection.src << ", " << currentSelection.dest << ")" << std::endl;
}

bool Player::is_human_player () {
    return is_human;
}

bool Logic::isLegalChange (State* s, Action c) {
    int side = LIGHT;

    uint64_t one = static_cast<uint64_t>(1);
    uint64_t src_square = one << c.src;
    uint64_t dest_square = one << c.dest;

    if (s->p[DARK] & src_square)
        side = DARK;

    if ((s->p[PAWN] & src_square) != 0)
        return (dest_square & pawn_movespace(c.src, s->p[side], s->p[(side + 1) % 2], side)) != 0;
    else if (s->p[KNIGHT] & src_square)
        return dest_square & knight_movespace(c.src, s->p[side]);
    else if (s->p[BISHOP] & src_square)
        return dest_square & bishop_movespace(c.src, s->p[side], s->p[(side + 1) % 2]);
    else if (s->p[ROOK] & src_square)
        return dest_square & rook_movespace(c.src, s->p[side], s->p[(side + 1) % 2]);
    else if (s->p[QUEEN] & src_square)
        return dest_square & queen_movespace(c.src, s->p[side], s->p[(side + 1) % 2]);
    else if (s->p[KING] & src_square)
        return dest_square & king_movespace(c.src, s->p[side], s->movespace[(side + 1) % 2], side);

    return false;
}

bool Logic::check_end_condition () {
    return false;
}

uint64_t Logic::movespace (State* board, int src) {
    uint64_t one = static_cast<uint64_t>(1);
    uint64_t src_square = one << src;

    int side = LIGHT;
    if (src_square & board->p[LIGHT])
        side = LIGHT;
    else if (src_square & board->p[DARK])
        side = DARK;
    else
        return 0;

    if (src_square & board->p[PAWN])
        return pawn_movespace(src, board->p[side], board->p[(side + 1) % 2], side);
    else if (src_square & board->p[KNIGHT])
        return knight_movespace(src, board->p[side]);
    else if (src_square & board->p[BISHOP])
        return bishop_movespace(src, board->p[side], board->p[(side + 1) % 2]);
    else if (src_square & board->p[ROOK])
        return rook_movespace(src, board->p[side], board->p[(side + 1) % 2]);
    else if (src_square & board->p[QUEEN])
        return queen_movespace(src, board->p[side], board->p[(side + 1) % 2]);
    else if (src_square & board->p[KING])
        return king_movespace(src, board->p[side], board->p[(side + 1) % 2], side);

    return static_cast<uint64_t>(0);
}

void Logic::move_piece (State* board, int src, int dest) {
    int64_t one = static_cast<int64_t>(1);
    int64_t src_square = one << src;
    int64_t dest_square = one << dest;

    if (board->p[LIGHT] & src_square) {
        board->p[LIGHT] -= src_square;
        board->p[LIGHT] = board->p[LIGHT] | dest_square;
        if (board->p[DARK] & dest_square)
            board->p[DARK] -= dest_square;
    }
    else if (board->p[DARK] & src_square) {
        board->p[DARK] -= src_square;
        board->p[DARK] = board->p[DARK] | dest_square;
        if (board->p[LIGHT] & dest_square)
            board->p[LIGHT] -= dest_square;
    }

    if (board->p[PAWN] & dest_square)
        board->p[PAWN] -= dest_square;
    else if (board->p[KNIGHT] & dest_square)
        board->p[KNIGHT] -= dest_square;
    else if (board->p[BISHOP] & dest_square) 
        board->p[BISHOP] -= dest_square;
    else if (board->p[ROOK] & dest_square)
        board->p[ROOK] -= dest_square;
    else if (board->p[QUEEN] & dest_square)
        board->p[QUEEN] -= dest_square;
    else if (board->p[KING] & dest_square)
        board->p[KING] -= dest_square;

    if (board->p[PAWN] & src_square) {
        board->p[PAWN] -= src_square;
        board->p[PAWN] += dest_square;
    }
    else if (board->p[KNIGHT] & src_square) {
        board->p[KNIGHT] -= src_square;
        board->p[KNIGHT] += dest_square;
    }
    else if (board->p[BISHOP] & src_square) {
        board->p[BISHOP] -= src_square;
        board->p[BISHOP] += dest_square;
    }
    else if (board->p[ROOK] & src_square) {
        board->p[ROOK] -= src_square;
        board->p[ROOK] += dest_square;
    }
    else if (board->p[QUEEN] & src_square) {
        board->p[QUEEN] -= src_square;
        board->p[QUEEN] += dest_square;
    }
    else if (board->p[KING] & src_square) {
        board->p[KING] -= src_square;
        board->p[KING] += dest_square;
    }
}

bool Logic::is_king_in_check (State* board, int side) {
    board->movespace[LIGHT] = side_movespace(board, LIGHT);
    board->movespace[DARK] = side_movespace(board, DARK);
    return (board->p[KING] & board->p[side]) & board->movespace[(side + 1) % 2];
}

uint64_t Logic::side_movespace (State* board, int side) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t movespace_iter = static_cast<uint64_t>(1);

    for (int i = 0; i < 64; i++) {
        if ((board->p[PAWN] & board->p[side]) & movespace_iter)
            movespace = movespace | pawn_movespace(side, i, board->p[side], board->p[(side + 1) % 2]);
        if ((board->p[KNIGHT] & board->p[side]) & movespace_iter)
            movespace = movespace | knight_movespace(i, board->p[side]);
        if ((board->p[BISHOP] & board->p[side]) & movespace_iter)
            movespace = movespace | bishop_movespace(i, board->p[side], board->p[(side + 1) % 2]);
        if ((board->p[ROOK] & board->p[side]) & movespace_iter)
            movespace = movespace | rook_movespace(i, board->p[side], board->p[(side + 1) % 2]);
        if ((board->p[QUEEN] & board->p[side]) & movespace_iter)
            movespace = movespace | queen_movespace(i, board->p[side], board->p[(side + 1) % 2]);
        if ((board->p[KING] & board->p[side]) & movespace_iter)
            movespace = movespace | king_movespace(i, board->p[side], board->movespace[(side + 1) % 2], side);
        
        movespace_iter = movespace_iter << 1;
    }

    return movespace;
}

uint64_t Logic::pawn_movespace (int src, uint64_t ally, uint64_t enemy, int side) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t square = static_cast<uint64_t>(movespace + 1);
    square = square << src;

    if (src % 8 == src || src - (src % 8) == 56) {
        return 0;
    }
    if (src % 8 != 0) {
        if (side == LIGHT) {
            if (enemy & (square >> 9))
                movespace += (square >> 9);
        }
        else {
            if (enemy & (square << 7))
                movespace += (square << 7);
        }
    }
    if (src % 8 != 7) {
        if (side == LIGHT) {
            if (enemy & (square >> 7))
                movespace += (square >> 7);
        }
        else {
            if (enemy & (square << 9))
                movespace += (square << 9);
        }
    }
    if (side == LIGHT) {
        if ((((enemy | ally) & (square >> 8))) == 0)
            movespace += square >> 8;
        if ((src - (src % 8)) / 8 == 6 && ((enemy | ally) & (square >> 16)) == 0)
            movespace += square >> 16;
    }
    else {
        if (((enemy | ally) & (square << 8)) == 0)
            movespace += square << 8;
        if ((src - (src % 8)) / 8 == 1 && ((enemy | ally) & (square << 16)) == 0)
            movespace += square << 16;
    }
    std::cout << "Result: " << movespace << std::endl;
    return movespace;
}

uint64_t Logic::knight_movespace (int src, uint64_t ally) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t square = (movespace + 1) << src;

    int srcx = src % 8;
    int srcy = (src - srcx) / 8;

    if (srcx < 7 && srcy > 1) 
        movespace += square >> 15;
    if (srcx < 6 && srcy > 0)
        movespace += square >> 6;
    if (srcx < 6 && srcy < 7)
        movespace += square << 10;
    if (srcx < 7 && srcy < 6)
        movespace += square << 17;
    if (srcx > 0 && srcy < 6)
        movespace += square << 15;
    if (srcx > 1 && srcy < 7)
        movespace += square << 6;
    if (srcx > 1 && srcy > 0)
        movespace += square >> 10;
    if (srcx > 0 && srcy > 1)
        movespace += square >> 17;

    return movespace;
}

uint64_t Logic::bishop_movespace (int src, uint64_t ally, uint64_t enemy) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t iter = (movespace + 1) << src;

    int srcx = src % 8; // 6
    int srcy = (src - srcx) / 8; // 3

    auto min = [] (int a, int b) {
        return (a < b) ? a : b;
    };

    // Uper Left
    for (int i = 1; i <= min(srcx, srcy); i++) {
        uint64_t square = iter >> (i * 9);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Uper Right
    for (int i = 1; i <= min(8 - srcx - 1, srcy); i++) {
        uint64_t square = iter >> (i * 7);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Lower Right
    for (int i = 1; i <= min(8 - srcx - 1, 8 - srcy - 1); i++) {
        uint64_t square = iter << (i * 9);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Lower Left
    for (int i = 1; i <= min(srcx, 8 - srcy - 1); i++) {
        uint64_t square = iter << (i * 7);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    return movespace;
}

uint64_t Logic::rook_movespace (int src, uint64_t ally, uint64_t enemy) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t iter = (movespace + 1) << src;

    int srcx = src % 8;
    int srcy = (src - srcx) / 8;

    // Up
    for (int i = 1; i <= srcy; i++) {
        uint64_t square = iter >> (i * 8);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Right
    for (int i = 1; i <= 8 - srcx - 1; i++) {
        uint64_t square = iter << i;
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Down
    for (int i = 1; i <= 8 - srcy - 1; i++) {
        uint64_t square = iter << (i * 8);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Left
    for (int i = 1; i <= srcx; i++) {
        uint64_t square = iter >> i;
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    return movespace;
}

uint64_t Logic::queen_movespace (int src, uint64_t ally, uint64_t enemy) {
    return bishop_movespace(src, ally, enemy) + rook_movespace(src, ally, enemy);
}

uint64_t Logic::king_movespace (int src, uint64_t ally, uint64_t enemy_movespace, int side) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t one = static_cast<uint64_t>(1);
    uint64_t square = one << src;

    uint64_t illegal_squares = ally | enemy_movespace;

    if (src - (src % 8) != 56) {
        if ((illegal_squares & (square << 8)) == 0)
            movespace += square << 8;
    }
    if (src % 8 != 7) {
        if ((illegal_squares & (square << 1)) == 0)
            movespace += square << 1;
    }
    if (src % 8 != 0) {
        if ((illegal_squares & (square >> 1)) == 0)
            movespace += square >> 1;
    }
    if (src % 8 != src) {
        if ((illegal_squares & (square >> 8)) == 0)
            movespace += square >> 8;
    }

    if (src % 8 != 0 && src % 8 != src) {
        if ((illegal_squares & (square >> 9)) == 0)
            movespace += square >> 9;
    }
    if (src % 8 != 7 && src % 8 != src) {
        if ((illegal_squares & (square >> 7)) == 0)
            movespace += square >> 7;
    }
    if (src % 8 != 7 && src - (src % 8) != 56) {
        if ((illegal_squares & (square << 9)) == 0)
            movespace += square << 9;
    }
    if (src % 8 != 0 && src - (src % 8) != 56) {
        if ((illegal_squares & (square << 7)) == 0)
            movespace += square << 7;
    }
    return movespace;
}
