#pragma once

#include "Action.h"
#include "ExpansionDNN.h"
#include "Logic.h"
#include "Action.cc"
#include "ExpansionDNN.cc"
#include "Logic.cc"

namespace Chess {
    class Player {
        private:
            bool is_human;
            Action currentSelection;

            ExpansionDNN* net;

        public:
            Player (bool h);

            ~Player () = default;

            void process_click (int click, uint64_t ally);

            bool is_current_change_ready ();

            void set_current_change (Action a);

            void set_dest (int n);
            
            int get_src ();
            
            Action get_current_change ();

            void generate_move (State* s, int turn);

            void generate_naive_move (State* s, int turn);
            
            bool is_human_player ();
        };
}