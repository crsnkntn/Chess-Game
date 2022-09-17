#include "Action.h"

Chess::Action::Action () : src(-1), dest(-1) {}

Chess::Action::Action (int s, int d) : src(s), dest(d) {}

void Chess::Action::execute(State& state) {
    uint64_t one = static_cast<uint64_t>(1);
    uint64_t src_square = one << src;
    uint64_t dest_square = one << dest;

    if (state.p[LIGHT] & src_square) {
        state.p[LIGHT] -= src_square;
        state.p[LIGHT] = state.p[LIGHT] | dest_square;
        if (state.p[DARK] & dest_square)
            state.p[DARK] -= dest_square;
    }
    else if (state.p[DARK] & src_square) {
        state.p[DARK] -= src_square;
        state.p[DARK] = state.p[DARK] | dest_square;
        if (state.p[LIGHT] & dest_square)
            state.p[LIGHT] -= dest_square;
    }

    if (state.p[PAWN] & dest_square)
        state.p[PAWN] -= dest_square;
    else if (state.p[KNIGHT] & dest_square)
        state.p[KNIGHT] -= dest_square;
    else if (state.p[BISHOP] & dest_square) 
        state.p[BISHOP] -= dest_square;
    else if (state.p[ROOK] & dest_square)
        state.p[ROOK] -= dest_square;
    else if (state.p[QUEEN] & dest_square)
        state.p[QUEEN] -= dest_square;
    else if (state.p[KING] & dest_square)
        state.p[KING] -= dest_square;

    if ((state.p[PAWN] & src_square) != 0) {
        state.p[PAWN] -= src_square;
        state.p[PAWN] += dest_square;
    }
    else if (state.p[KNIGHT] & src_square) {
        state.p[KNIGHT] -= src_square;
        state.p[KNIGHT] += dest_square;
    }
    else if (state.p[BISHOP] & src_square) {
        state.p[BISHOP] -= src_square;
        state.p[BISHOP] += dest_square;
    }
    else if (state.p[ROOK] & src_square) {
        state.p[ROOK] -= src_square;
        state.p[ROOK] += dest_square;
    }
    else if (state.p[QUEEN] & src_square) {
        state.p[QUEEN] -= src_square;
        state.p[QUEEN] += dest_square;
    }
    else if (state.p[KING] & src_square) {
        state.p[KING] -= src_square;
        state.p[KING] += dest_square;
    }
}

int Chess::Action::get_src () {
    return src;
}

int Chess::Action::get_dest () {
    return dest;
}
