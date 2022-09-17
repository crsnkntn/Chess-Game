#pragma once

#include "Header.h"
#include "State.h"
#include "../include/mini-mcts/mcts.h"

namespace Chess {
    class Action : public mcts::Action<State> {
        public:
            int src;
            int dest;

            Action ();

            Action (int s, int d);

            void execute(State& state);

            int get_src ();

            int get_dest ();
    };
}
