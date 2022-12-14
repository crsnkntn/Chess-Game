#pragma once

#include "../include/mini-mcts/mcts.h"
#include "Header.h"

namespace Chess {
    class State : public mcts::mctsState {
        public:
            char en_passant[2];
            char castling_privilege[2];
            uint64_t p[8];
            uint64_t movespace[2];

            State () {
                p[LIGHT] = static_cast<uint64_t>(0b1111111111111111000000000000000000000000000000000000000000000000);
                p[DARK] = static_cast<uint64_t>(0b0000000000000000000000000000000000000000000000001111111111111111);
                p[PAWN] = static_cast<uint64_t>(0b0000000011111111000000000000000000000000000000001111111100000000);
                p[KNIGHT] = static_cast<uint64_t>(0b0100001000000000000000000000000000000000000000000000000001000010);
                p[BISHOP] = static_cast<uint64_t>(0b0010010000000000000000000000000000000000000000000000000000100100);
                p[ROOK] = static_cast<uint64_t>(0b1000000100000000000000000000000000000000000000000000000010000001);
                p[KING] = static_cast<uint64_t>(0b0001000000000000000000000000000000000000000000000000000000010000);
                p[QUEEN] = static_cast<uint64_t>(0b0000100000000000000000000000000000000000000000000000000000001000);

                en_passant[LIGHT] = char(0);
                en_passant[DARK] = char(0);

                castling_privilege[LIGHT] = char(0b11111111);
                castling_privilege[DARK] = char(0b11111111);
            }
    };
}