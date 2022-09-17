#pragma once

#include <SDL2/SDL.h>

#include "Header.h"
#include "State.h"
#include "Action.h"

namespace Chess {
    class SDL_Board_View {
        private:
            int offsetx = 0;
            int offsety = 0;

            SDL_Window* window;
            SDL_Renderer* renderer;

            void display_piece (int sq_color, int pc_color, int piece_type, int loc);
        public:
            SDL_Board_View (int x, int y);

            ~SDL_Board_View ();
            
            void display_board (State* state, int selection);
    };

}