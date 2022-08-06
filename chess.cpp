#include <iostream>
#include <SDL2/SDL.h>
#include <cstring>
#include <stdint.h>
#include <cstdlib>
#include <bitset>

enum piece_bit_ids {
    NO_PIECE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    KING,
    QUEEN,
    DARK = 16
};

struct chess_bitboards {
    uint64_t light;
    uint64_t dark;
    uint64_t pawns;
    uint64_t knights;
    uint64_t bishops;
    uint64_t rooks;
    uint64_t kings;
    uint64_t queens;
};

struct chess_state {
    chess_bitboards board;
    int square_dimension;
    bool updated;
    bool needs_automatic_move;
    int selected_square;
};

struct chess_piece_bitmap {
    int16_t one;
    uint64_t two;
    uint64_t three;
};

void chess_reset (chess_state& state) {
    state.updated = true;
    state.needs_automatic_move = false;
    state.selected_square = -1;

    state.board.light = static_cast<uint64_t>(0b1111111111111111000000000000000000000000000000000000000000000000);
    state.board.dark = static_cast<uint64_t>(0b0000000000000000000000000000000000000000000000001111111111111111);
    state.board.pawns = static_cast<uint64_t>(0b0000000011111111000000000000000000000000000000001111111100000000);
    state.board.knights = static_cast<uint64_t>(0b0100001000000000000000000000000000000000000000000000000001000010);
    state.board.bishops = static_cast<uint64_t>(0b0010010000000000000000000000000000000000000000000000000000100100);
    state.board.rooks = static_cast<uint64_t>(0b1000000100000000000000000000000000000000000000000000000010000001);
    state.board.kings = static_cast<uint64_t>(0b0000100000000000000000000000000000000000000000000000000000001000);
    state.board.queens = static_cast<uint64_t>(0b0001000000000000000000000000000000000000000000000000000000010000);
}

bool chess_check_move_legality (chess_state& state, int clickx, int clicky) {
    int64_t one = static_cast<int64_t>(0b1);
    int coord = (clicky * 8) + clickx; 

    int srcx = state.selected_square % 8;
    int srcy = (state.selected_square - srcx) / 8;

    int64_t square = static_cast<int64_t>(one << state.selected_square);

    auto cln = [] (int64_t a, int64_t b) {return (a & b) != 0;}; // cln ~ collision

    if (cln(state.board.pawns, square)) {
        if (cln(state.board.light, square)) {
            if (srcy == 6) {
                if ((clicky == srcy - 1 || clicky == srcy - 2) && srcx == clickx)
                    return true;
            }
            if (srcy - 1 == clicky && srcx + 1 == clickx) {
                if (srcx != 7 && cln(state.board.dark, square >> 7))
                    return true;
            }
            if (srcy - 1 == clicky && srcx - 1 == clickx) {
                if (srcx != 0 && cln(state.board.dark, square >> 9))
                    return true;
            }
            if (cln(state.board.light, square >> 8)
                || cln(state.board.dark, square >> 8))
                    return false;
            return (srcy - 1 == clicky) && srcx == clickx;
        }
        else {
            if (srcy == 1) {
                if ((clicky == srcy + 1 || clicky == srcy + 2) && srcx == clickx)
                    return true;
            }
            if (srcy + 1 == clicky && srcx + 1 == clickx) {
                if (srcx != 7 && cln(state.board.light, square << 9))
                    return true;
            }
            if (srcy + 1 == clicky && srcx - 1 == clickx) {
                if (srcx != 0 && cln(state.board.light, square << 7))
                    return true;
            }
            if (cln(state.board.light, one << (state.selected_square + 8))
                || cln(state.board.dark, one << (state.selected_square + 8)))
                    return false;

            return (srcy + 1 == clicky) && srcx == clickx;
        }
        return false;
    }
    else if (cln(state.board.knights, square)) {
        int64_t pseudo_legal = static_cast<int64_t>(0b0);
        if (srcx < 7 && srcy > 1) 
            pseudo_legal += square >> 15;
        if (srcx < 6 && srcy > 0)
            pseudo_legal += square >> 6;
        if (srcx < 6 && srcy < 7)
            pseudo_legal += square << 10;
        if (srcx < 7 && srcy < 6)
            pseudo_legal += square << 17;
        if (srcx > 0 && srcy < 6)
            pseudo_legal += square << 15;
        if (srcx > 1 && srcy < 7)
            pseudo_legal += square << 6;
        if (srcx > 1 && srcy > 0)
            pseudo_legal += square >> 10;
        if (srcx > 0 && srcy > 1)
            pseudo_legal += square >> 17;
        if (cln(pseudo_legal, one << coord)) {
            if (cln(state.board.light, square))
                return !cln(state.board.light, one << coord);
            else
                return !cln(state.board.dark, one << coord);
        }
        return false;
    }
    else if (cln(state.board.bishops, square)) {
        int xdiff = abs(clickx - srcx);
        int ydiff = abs(clicky - srcy);
        return xdiff == ydiff;
    }
    else if (cln(state.board.rooks, square)) {
        int xdiff = abs(clickx - srcx);
        int ydiff = abs(clicky - srcy);
        return (xdiff == 0 && ydiff != 0) || (ydiff == 0 && xdiff != 0);
    }
    else if (cln(state.board.kings, square)) {

    }
    else if (cln(state.board.queens, square)) {
        int xdiff = abs(clickx - srcx);
        int ydiff = abs(clicky - srcy);
        return xdiff == ydiff || ((xdiff == 0 && ydiff != 0) || (ydiff == 0 && xdiff != 0));
    }


    return true;
}

void chess_make_move (chess_state& state, int clickx, int clicky) {
    int src_coord = state.selected_square;
    int dest_coord = (clicky * 8) + clickx;

    uint64_t one = static_cast<int64_t>(0b1);

    // Clears the Destination Square of the color bitboard
    if (state.board.light & (one << dest_coord))
        state.board.light = state.board.light - (one << dest_coord);
    else if (state.board.dark & (one << dest_coord))
        state.board.dark = state.board.dark - (one << dest_coord);

    // Clears the Source Square of the color bitboard
    if (state.board.light & (one << src_coord)) {
        state.board.light = state.board.light - (one << src_coord);
        state.board.light = state.board.light + (one << dest_coord);
    }
    else if (state.board.dark & (one << src_coord)) {
        state.board.dark = state.board.dark - (one << src_coord);
        state.board.dark = state.board.dark + (one << dest_coord);
    }

    // Clears the Destination Square of the piece bitboard
    if (state.board.pawns & (one << dest_coord))
        state.board.pawns = state.board.pawns - (one << dest_coord);
    else if (state.board.knights & (one << dest_coord))
        state.board.knights = state.board.knights - (one << dest_coord);
    else if (state.board.bishops & (one << dest_coord))
        state.board.bishops = state.board.bishops - (one << dest_coord);
    else if (state.board.rooks & (one << dest_coord))
        state.board.rooks = state.board.rooks - (one << dest_coord);
    else if (state.board.kings & (one << dest_coord))
        state.board.kings = state.board.kings - (one << dest_coord);
    else if (state.board.queens & (one << dest_coord))
        state.board.queens = state.board.queens - (one << dest_coord);
    
    // Clears the Source Square of the piece bitboard
    // Also updates the Destination Square of the piece bitboard
    if (state.board.pawns & (one << src_coord)) {
        state.board.pawns = state.board.pawns - (one << src_coord);
        state.board.pawns = state.board.pawns + (one << dest_coord);
    }
    else if (state.board.knights & (one << src_coord)) {
        state.board.knights = state.board.knights - (one << src_coord);
        state.board.knights = state.board.knights + (one << dest_coord);
    }
    else if (state.board.bishops & (one << src_coord)) {
        state.board.bishops = state.board.bishops - (one << src_coord);
        state.board.bishops = state.board.bishops + (one << dest_coord);
    }
    else if (state.board.rooks & (one << src_coord)) {
        state.board.rooks = state.board.rooks - (one << src_coord);
        state.board.rooks = state.board.rooks + (one << dest_coord);
    }
    else if (state.board.kings & (one << src_coord)) {
        state.board.kings = state.board.kings - (one << src_coord);
        state.board.kings = state.board.kings + (one << dest_coord);
    }
    else if (state.board.queens & (one << src_coord)) {
        state.board.queens = state.board.queens - (one << src_coord);
        state.board.queens = state.board.queens + (one << dest_coord);
    }
    state.selected_square = -1;
    state.updated = true;
}   

void chess_process_mouse_click (chess_state& state, int mousex, int mousey) {
    int clickx = mousex / state.square_dimension, clicky = mousey / state.square_dimension;
    int coord = static_cast<int64_t>(clickx + 8 * clicky);
    int64_t one = static_cast<int64_t>(0b1);

    // Deselection Mechanism
    if (coord == state.selected_square) {
        state.selected_square = -1;
        state.updated = true;
        return;
    }

    if (state.selected_square != -1) {
        if (chess_check_move_legality(state, clickx, clicky)) {
            chess_make_move(state, clickx, clicky);
        }
    }
    else {
        if ((state.board.light & static_cast<int64_t>(one << coord)) != 0) {
            state.selected_square = coord;
            state.updated = true;
        }
        else if ((state.board.dark & static_cast<int64_t>(one << coord)) != 0) {
            state.selected_square = coord;
            state.updated = true;
        }
    }
}   

void chess_automatic_move (chess_state& state) {

}

int main () {
    int square_dimension = 96;

    if (square_dimension % 12 != 0)
        exit(1);

    int pixel_size = square_dimension / 12;

    chess_state game;

    game.square_dimension = square_dimension;

    chess_reset(game);

    chess_piece_bitmap empty_display_bitmap = {
        static_cast<int16_t>(0b0),
        static_cast<uint64_t>(0b0),
        static_cast<uint64_t>(0b0)
    };

    chess_piece_bitmap pawn_display_bitmap = {
        static_cast<int16_t>(0b0),
        static_cast<uint64_t>(0b0000000000000000000000000110000000001111000000001111000000000110),
        static_cast<uint64_t>(0b0000000001100000000001100000000011110000000111111000000000000000)
    };

    chess_piece_bitmap knight_display_bitmap = {
        static_cast<int16_t>(0b0),
        static_cast<uint64_t>(0b0001000000011111000000011101100000001111100000000111100000000111),
        static_cast<uint64_t>(0b1000000011110000000011110000000111111000001111111100000000000000)
    };

    chess_piece_bitmap bishop_display_bitmap = {
        static_cast<int16_t>(0b0),
        static_cast<uint64_t>(0b0110000000001101000000001111000000001111000000000110000000000110),
        static_cast<uint64_t>(0b0000000011110000000011110000000111111000001111111100000000000000)
    };

    chess_piece_bitmap rook_display_bitmap = {
        static_cast<int16_t>(0b1),
        static_cast<uint64_t>(0b0110100000011111100000011111100000001111000000001111000000001111),
        static_cast<uint64_t>(0b0000000011110000000111111000000111111000001111111100000000000000)
    };

    chess_piece_bitmap king_display_bitmap = {
        static_cast<int16_t>(0b0),
        static_cast<uint64_t>(0b0110000000001111000000000110000000011111100000011111100000011111),
        static_cast<uint64_t>(0b1000000011110000000111111000000111111000001111111100000000000000)
    };

    chess_piece_bitmap queen_display_bitmap = {
        static_cast<int16_t>(0b0),
        static_cast<uint64_t>(0b1001000000010110100000001001000000011111100000011111100000011111),
        static_cast<uint64_t>(0b1000000011110000000111111000000111111000001111111100000000000000)
    };

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

    SDL_Color selection_color = {
        .r = 240,
        .g = 10,
        .b = 10,
        .a = 150
    };

    SDL_Window* window;
    SDL_Renderer* renderer;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("error: failed to initialize SDL: %s\n", SDL_GetError());
        exit(0);
    }

    window = SDL_CreateWindow("Projection", 
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        square_dimension * 8, 
        square_dimension * 8,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("error: failed to open %d x %d window: %s\n", square_dimension * 8, square_dimension * 8, SDL_GetError());
        exit(0);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        printf("error: failed to create renderer: %s\n", SDL_GetError());
        exit(0);
    }

    int playing = 1;
    SDL_Event e;
    SDL_Color piece_color, square_color;
    
    int x = 0, y = 0;

    while (playing == 1) {

        if (game.updated) {
            game.updated = false;

            chess_piece_bitmap temp_piece_bitmap;
            uint64_t bit_piece_color;
            uint64_t one = 0b1;

            int sqx = 0, sqy = 0;
            bool square_alternator = true;
            for (int bit_iterator = 0; bit_iterator < 64; bit_iterator++) {
                sqx = bit_iterator % 8;
                sqy = (bit_iterator - sqx) / 8;

                square_alternator = sqx == 0 ? square_alternator : !square_alternator;

                uint64_t bit_check = one << bit_iterator;

                square_color = square_alternator ? light_square : dark_square;

                if ((game.board.light & bit_check) != 0) {
                    piece_color = light_piece;
                    bit_piece_color = game.board.light;
                }
                else if ((game.board.dark & bit_check) != 0) {
                    piece_color = dark_piece;
                    bit_piece_color = game.board.dark;
                }
                else {
                    piece_color = square_color;
                    bit_piece_color = static_cast<uint64_t>(0b0);
                }

                if (((game.board.pawns & bit_piece_color) & bit_check) != 0) {
                    temp_piece_bitmap = pawn_display_bitmap;
                }
                else if (((game.board.knights & bit_piece_color) & bit_check) != 0) {
                    temp_piece_bitmap = knight_display_bitmap;
                }
                else if (((game.board.bishops & bit_piece_color) & bit_check) != 0) {
                    temp_piece_bitmap = bishop_display_bitmap;
                }
                else if (((game.board.rooks & bit_piece_color) & bit_check) != 0) {
                    temp_piece_bitmap = rook_display_bitmap;
                }
                else if (((game.board.kings & bit_piece_color) & bit_check) != 0) {
                    temp_piece_bitmap = king_display_bitmap;
                }
                else if (((game.board.queens & bit_piece_color) & bit_check) != 0) {
                    temp_piece_bitmap = queen_display_bitmap;
                }
                else {
                    temp_piece_bitmap = empty_display_bitmap;
                }

                int offsetx = 0, offsety = 0;

                for (int bit_iterator_inner = 15; bit_iterator_inner >= 0; bit_iterator_inner--) {
                    if (offsetx == square_dimension) {
                        offsetx = 0;
                        offsety += pixel_size;
                    }
                    
                    SDL_Rect pixel = {
                        .x = (sqx * square_dimension) + offsetx,
                        .y = (sqy * square_dimension) + offsety,
                        .w = pixel_size,
                        .h = pixel_size
                    };

                    if ((temp_piece_bitmap.one & (one << bit_iterator_inner)) != 0) {
                        SDL_SetRenderDrawColor(renderer, piece_color.r, piece_color.g, piece_color.b, piece_color.a);
                        SDL_RenderFillRect(renderer, &pixel);
                    }
                    else {
                        if ((offsetx == 0 || offsety == 0) && bit_iterator == game.selected_square) {
                            SDL_SetRenderDrawColor(renderer, selection_color.r, selection_color.g, selection_color.b, selection_color.a);
                            SDL_RenderFillRect(renderer, &pixel);
                        }
                        else {
                            SDL_SetRenderDrawColor(renderer, square_color.r, square_color.g, square_color.b, square_color.a);
                            SDL_RenderFillRect(renderer, &pixel);
                        }
                    }

                    offsetx += pixel_size;
                }

                for (int bit_iterator_inner = 63; bit_iterator_inner >= 0; bit_iterator_inner--) {
                    if (offsetx == square_dimension) {
                        offsetx = 0;
                        offsety += pixel_size;
                    }
                    SDL_Rect pixel = {
                        .x = (sqx * square_dimension) + offsetx,
                        .y = (sqy * square_dimension) + offsety,
                        .w = pixel_size,
                        .h = pixel_size
                    };

                    if ((temp_piece_bitmap.two & (one << bit_iterator_inner)) != 0) {
                        SDL_SetRenderDrawColor(renderer, piece_color.r, piece_color.g, piece_color.b, piece_color.a);
                        SDL_RenderFillRect(renderer, &pixel);
                    }
                    else {
                        if ((offsetx == 0 || offsety == 0) && bit_iterator == game.selected_square) {
                            SDL_SetRenderDrawColor(renderer, selection_color.r, selection_color.g, selection_color.b, selection_color.a);
                            SDL_RenderFillRect(renderer, &pixel);
                        }
                        else {
                            SDL_SetRenderDrawColor(renderer, square_color.r, square_color.g, square_color.b, square_color.a);
                            SDL_RenderFillRect(renderer, &pixel);
                        }
                    }

                    offsetx += pixel_size;
                }

                for (int bit_iterator_inner = 63; bit_iterator_inner >= 0; bit_iterator_inner--) {
                    if (offsetx == square_dimension) {
                        offsetx = 0;
                        offsety += pixel_size;
                    }
                    SDL_Rect pixel = {
                        .x = (sqx * square_dimension) + offsetx,
                        .y = (sqy * square_dimension) + offsety,
                        .w = pixel_size,
                        .h = pixel_size
                    };

                    if ((temp_piece_bitmap.three & (one << bit_iterator_inner)) != 0) {
                        SDL_SetRenderDrawColor(renderer, piece_color.r, piece_color.g, piece_color.b, piece_color.a);
                        SDL_RenderFillRect(renderer, &pixel);
                    }
                    else {
                        if ((offsetx == 0 || offsety == 0) && bit_iterator == game.selected_square) {
                            SDL_SetRenderDrawColor(renderer, selection_color.r, selection_color.g, selection_color.b, selection_color.a);
                            SDL_RenderFillRect(renderer, &pixel);
                        }
                        else {
                            SDL_SetRenderDrawColor(renderer, square_color.r, square_color.g, square_color.b, square_color.a);
                            SDL_RenderFillRect(renderer, &pixel);
                        }
                    }

                    offsetx += pixel_size;
                }
            }
            
            SDL_RenderPresent(renderer);
        }

        if (game.needs_automatic_move)
            chess_automatic_move(game);
        
        while (SDL_PollEvent(&e)){
            switch (e.type) {
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_r:
                            chess_reset(game);
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    SDL_GetMouseState(&x, &y);
                    chess_process_mouse_click(game, x, y);
                    break;
                case SDL_QUIT:
                    playing = 0;
                    break;
            }
        }
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}