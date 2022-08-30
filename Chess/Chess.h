#pragma once

#include "../Game/Game.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <bitset>

namespace Chess {
    enum Pieces {
        LIGHT,
        DARK,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };
    
    struct State;
    struct StateChange;
    
    struct State {
        uint64_t p[8];
        char en_passant[2];
        char castling_privilege[2];
        uint64_t movespace[2];

        State ();
    };

    struct StateChange {
        int src;
        int dest;

        StateChange (int s, int d);
    };

    void utility_move_piece (State* s, StateChange c);

    class Player {
        public:
            Player () = default;
            ~Player () = default;

            StateChange get_move (State* s);
    };

    class HumanPlayer : public Player {
        private:
            int src;
            int dest;

        public:
            HumanPlayer () = default;

            int get_src ();

            int get_dest ();

            StateChange get_move (State* s);
    };

    class CompPlayer : public Player {
        private:
            int src;
            int dest;

        public:
            CompPlayer () = default;

            int get_src ();

            int get_dest ();

            StateChange get_move (State* s);
    };

    class LogicObject {
        public:
            bool isLegalChange (State* s, StateChange c);

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

    class Game_SDL {
        private:
            SDL_Window* window;
            SDL_Renderer* renderer;

            Chess::State* currentState;

            Player p1;
            Player p2;

            LogicObject logicObject;

            void display ();

        public:
            Game_SDL (Player one, Player two);

            void init_sdl (SDL_Window* w, SDL_Renderer* r);

            void play ();
    };

    namespace Bitmap {
        struct bitmap {
            uint16_t first;
            uint64_t second;
            uint64_t third;
        };

        bitmap empty = {
            static_cast<uint16_t>(0b0),
            static_cast<uint64_t>(0b0),
            static_cast<uint64_t>(0b0)
        };

        bitmap pawn = {
            static_cast<uint16_t>(0b0),
            static_cast<uint64_t>(0b0000000000000000000000000110000000001111000000001111000000000110),
            static_cast<uint64_t>(0b0000000001100000000001100000000011110000000111111000000000000000)
        };

        bitmap knight = {
            static_cast<uint16_t>(0b0),
            static_cast<uint64_t>(0b0001000000011111000000011101100000001111100000000111100000000111),
            static_cast<uint64_t>(0b1000000011110000000011110000000111111000001111111100000000000000)
        };

        bitmap bishop = {
            static_cast<int16_t>(0b0),
            static_cast<uint64_t>(0b0110000000001101000000001111000000001111000000000110000000000110),
            static_cast<uint64_t>(0b0000000011110000000011110000000111111000001111111100000000000000)
        };

        bitmap rook = {
            static_cast<int16_t>(0b1),
            static_cast<uint64_t>(0b0110100000011111100000011111100000001111000000001111000000001111),
            static_cast<uint64_t>(0b0000000011110000000111111000000111111000001111111100000000000000)
        };

        bitmap king = {
            static_cast<int16_t>(0b0),
            static_cast<uint64_t>(0b0110000000001111000000000110000000011111100000011111100000011111),
            static_cast<uint64_t>(0b1000000011110000000111111000000111111000001111111100000000000000)
        };

        bitmap queen = {
            static_cast<int16_t>(0b0),
            static_cast<uint64_t>(0b1001000000010110100000001001000000011111100000011111100000011111),
            static_cast<uint64_t>(0b1000000011110000000111111000000111111000001111111100000000000000)
        };

        bitmap selection = {
            static_cast<uint16_t>(0b11111111111111000),
            static_cast<uint64_t>(0b0000000110000000000110000000000110000000000110000000000110000000),
            static_cast<uint64_t>(0b0001100000000001100000000001100000000001100000000001111111111111)
        };
    }

    namespace Color {
        SDL_Color light_square = {
            .r = 181,
            .g = 184,
            .b = 156,
            .a = 255
        };

        SDL_Color dark_square = {
            .r = 41,
            .g = 57,
            .b = 105,
            .a = 255
        };

        SDL_Color light_piece = {
            .r = 52,
            .g = 235,
            .b = 186,
            .a = 255
        };

        SDL_Color dark_piece = {
            .r = 122,
            .g = 52,
            .b = 235,
            .a = 255
        };

        SDL_Color selected = {
            .r = 240,
            .g = 10,
            .b = 10,
            .a = 150
        };

        SDL_Color legal_move = {
            .r = 240,
            .g = 100,
            .b = 10,
            .a = 150
        };
    }
}
