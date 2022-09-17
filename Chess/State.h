#pragma once

#include "../include/mini-mcts/mcts.h"

namespace Chess {
    class State : public mcts::State {
        public:
            char en_passant[2];
            char castling_privilege[2];
            uint64_t p[8];
            uint64_t movespace[2];

            State ();
    };
}