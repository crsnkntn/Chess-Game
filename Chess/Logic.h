#pragma once

#include <bitset>

#include "State.h"
#include "Action.h"

namespace Chess {
    class Logic {
        public:
            bool isLegalChange (State* s, Action c, int side);

            bool check_end_condition ();

        private:
            uint64_t movespace (State* board, int src);

            void move_piece (State* board, int src, int dest);

            bool is_king_in_check (State* board, int side);

            uint64_t side_movespace (State* board, int side);

            uint64_t pawn_movespace (int src, uint64_t ally, uint64_t enemy, int side);

            uint64_t knight_movespace (int src, uint64_t ally);

            uint64_t bishop_movespace (int src, uint64_t ally, uint64_t enemy);

            uint64_t rook_movespace (int src, uint64_t ally, uint64_t enemy);

            uint64_t queen_movespace (int src, uint64_t ally, uint64_t enemy);

            uint64_t king_movespace (int src, uint64_t ally, uint64_t enemy_movespace, int side);
    };
}
