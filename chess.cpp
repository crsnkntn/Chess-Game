#include "chess.h"

using namespace Chess;
    
void Chess::reset (Game& game) {
    game.board_update_flag = true;

    game.board.light = static_cast<uint64_t>(0b1111111111111111000000000000000000000000000000000000000000000000);
    game.board.dark = static_cast<uint64_t>(0b0000000000000000000000000000000000000000000000001111111111111111);
    game.board.pawns = static_cast<uint64_t>(0b0000000011111111000000000000000000000000000000001111111100000000);
    game.board.knights = static_cast<uint64_t>(0b0100001000000000000000000000000000000000000000000000000001000010);
    game.board.bishops = static_cast<uint64_t>(0b0010010000000000000000000000000000000000000000000000000000100100);
    game.board.rooks = static_cast<uint64_t>(0b1000000100000000000000000000000000000000000000000000000010000001);
    game.board.kings = static_cast<uint64_t>(0b0000100000000000000000000000000000000000000000000000000000001000);
    game.board.queens = static_cast<uint64_t>(0b0001000000000000000000000000000000000000000000000000000000010000);
}

bool Chess::king_in_check (Game& game) {
    return false;
}

bool Chess::check_move_legality (Game& game, int src, int dest) {
    uint64_t one = static_cast<uint64_t>(1);

    int srcx = src % 8;
    int srcy = (src - srcx) / 8;
    int destx = dest % 8;
    int desty = (dest - destx) / 8;
    
    uint64_t square = static_cast<uint64_t>(one << src);

    if (game.board.light & square) {
        if (game.board.light & (one << (destx + 8 * desty)))
            return false;
    }
    else if (game.board.dark & square) {
        if (game.board.dark & (one << (destx + 8 * desty)))
            return false;
    }

    if (game.board.pawns & square) {
        if (game.board.light & square) {
            if (srcy == 6) {
                if (srcx == destx && desty == srcy - 1)
                    return ((game.board.light | game.board.dark) & (square >> 8)) == 0;
                if (srcx == destx && desty == srcy - 2)
                    return ((game.board.light | game.board.dark) & ((square >> 8) + (square >> 16))) == 0;
            }
            if (srcy - 1 == desty && srcx + 1 == destx) {
                if (srcx != 7 && game.board.dark & (square >> 7))
                    return true;
            }
            if (srcy - 1 == desty && srcx - 1 == destx) {
                if (srcx != 0 && game.board.dark & (square >> 9))
                    return true;
            }
            if (game.board.light & (square >> 8)
                || game.board.dark & (square >> 8))
                    return false;
            return (srcy - 1 == desty) && srcx == destx;
        }
        else {
            if (srcy == 1) {
                if (srcx == destx && desty == srcy + 1)
                    return ((game.board.light | game.board.dark) & (square << 8)) == 0;
                if (srcx == destx && desty == srcy + 2)
                    return ((game.board.light | game.board.dark) & ((square << 8) + (square << 16))) == 0;
            }
            if (srcy + 1 == desty && srcx + 1 == destx) {
                if (srcx != 7 && game.board.light & (square << 9))
                    return true;
            }
            if (srcy + 1 == desty && srcx - 1 == destx) {
                if (srcx != 0 && game.board.light & (square << 7))
                    return true;
            }
            if (game.board.light & (one << (src + 8))
                || game.board.dark & (one << (src + 8)))
                    return false;

            return (srcy + 1 == desty) && srcx == destx;
        }
        return false;
    }
    else if (game.board.knights & square) {
        uint64_t pseudo_legal = static_cast<uint64_t>(0b0);
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

        uint64_t temp = one << dest;
        if (pseudo_legal & temp) {
            if (game.board.light & square)
                return !(game.board.light & temp);
            else
                return !(game.board.dark & temp);
        }
        return false;
    }
    else if ((game.board.bishops & square) || (game.board.queens & square)) {
        int xdiff = destx - srcx;
        int ydiff = desty - srcy;
        if (abs(xdiff) == abs(ydiff)) {
            uint64_t iter = square;
            uint64_t dest_square = static_cast<uint64_t>(one << dest);
            uint64_t occupied_squares = static_cast<uint64_t>(game.board.light | game.board.dark);
            int shift_amount = 0;
            bool shift_left = false;
            if (xdiff < 0 && ydiff < 0)
                shift_amount = 9;
            else if (xdiff > 0 && ydiff < 0)
                shift_amount = 7;
            else if (xdiff < 0 && ydiff > 0) {
                shift_amount = 7;
                shift_left = true;
            }
            else {
                shift_amount = 9;
                shift_left = true;
            }

            if (shift_left)
                iter = iter << shift_amount;
            else
                iter = iter >> shift_amount;

            while (iter != dest_square && iter != 0) {
                if (occupied_squares & iter)
                    return false;
                if (shift_left)
                    iter = iter << shift_amount;
                else
                    iter = iter >> shift_amount;
            }
            return true;
        }
    }
    if ((game.board.rooks & square) || (game.board.queens & square)) {
        int xdiff = destx - srcx;
        int ydiff = desty - srcy;
        if (!((xdiff == 0 && ydiff != 0) || (ydiff == 0 && xdiff != 0)))
            return false;

        uint64_t iter = square;
        uint64_t dest_square = static_cast<uint64_t>(one << dest);
        uint64_t occupied_squares = static_cast<uint64_t>(game.board.light | game.board.dark);
        int shift_amount = 0;
        bool shift_left = false;
        if (xdiff == 0)
            shift_amount = 8;
        else
            shift_amount = 1;

        if (ydiff > 0)
            shift_left = true;
        else if (xdiff > 0)
            shift_left = true;

        if (shift_left)
            iter = iter << shift_amount;
        else
            iter = iter >> shift_amount;

        while (iter != dest_square && iter != 0) {
            if (occupied_squares & iter)
                return false;
            if (shift_left)
                iter = iter << shift_amount;
            else
                iter = iter >> shift_amount;
        }
        return true;
    }
    else if (game.board.kings & square) {
        int xdiff = abs(destx - srcx);
        int ydiff = abs(desty - srcy);

        return xdiff + ydiff == 1 || (xdiff == 1 && ydiff == 1);
    }
    return true;
}

void Chess::move (Game& game, int src, int dest) {
    uint64_t one = static_cast<uint64_t>(1);
    uint64_t src_mask = (one << src);
    uint64_t dest_mask = (one << dest);

    // Clears the Destination Square of the color bitboard
    if (game.board.light & dest_mask)
        game.board.light = game.board.light - dest_mask;
    else if (game.board.dark & dest_mask)
        game.board.dark = game.board.dark - dest_mask;

    // Clears the Source Square of the color bitboard
    if (game.board.light & src_mask) {
        game.board.light = game.board.light - src_mask;
        game.board.light = game.board.light + dest_mask;
    }
    else if (game.board.dark & src_mask) {
        game.board.dark = game.board.dark - src_mask;
        game.board.dark = game.board.dark + dest_mask;
    }

    // Clears the Destination Square of the piece bitboard
    if (game.board.pawns & dest_mask)
        game.board.pawns = game.board.pawns - dest_mask;
    else if (game.board.knights & dest_mask)
        game.board.knights = game.board.knights - dest_mask;
    else if (game.board.bishops & dest_mask)
        game.board.bishops = game.board.bishops - dest_mask;
    else if (game.board.rooks & dest_mask)
        game.board.rooks = game.board.rooks - dest_mask;
    else if (game.board.kings & dest_mask)
        game.board.kings = game.board.kings - dest_mask;
    else if (game.board.queens & dest_mask)
        game.board.queens = game.board.queens - dest_mask;
    
    // Clears the Source Square of the piece bitboard
    // Also updates the Destination Square of the piece bitboard
    if (game.board.pawns & src_mask) {
        game.board.pawns = game.board.pawns - src_mask;
        game.board.pawns = game.board.pawns + dest_mask;
    }
    else if (game.board.knights & src_mask) {
        game.board.knights = game.board.knights - src_mask;
        game.board.knights = game.board.knights + dest_mask;
    }
    else if (game.board.bishops & src_mask) {
        game.board.bishops = game.board.bishops - src_mask;
        game.board.bishops = game.board.bishops + dest_mask;
    }
    else if (game.board.rooks & src_mask) {
        game.board.rooks = game.board.rooks - src_mask;
        game.board.rooks = game.board.rooks + dest_mask;
    }
    else if (game.board.kings & src_mask) {
        game.board.kings = game.board.kings - src_mask;
        game.board.kings = game.board.kings + dest_mask;
    }
    else if (game.board.queens & src_mask) {
        game.board.queens = game.board.queens - src_mask;
        game.board.queens = game.board.queens + dest_mask;
    }
    game.board_update_flag = true;
}

int Chess::auto_move (Game& game) {
    return 1;
}

int main () {
    int square_dimension = SIZE * 12;

    int pixel_size = square_dimension / 12;

    Chess::Game game;

    reset(game);

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
    int selected_square = -1;
    
    uint64_t one = static_cast<uint64_t>(1);
    uint64_t click = 0;

    while (playing == 1) {

        if (game.board_update_flag) {
            game.board_update_flag = false;

            bitmap temp_piece_bitmap;
            uint64_t bit_piece_color;
            uint64_t one = 0b1;

            int sqx = 0, sqy = 0;
            bool square_alternator = true;
            for (int bit_iterator = 0; bit_iterator < 64; bit_iterator++) {
                sqx = bit_iterator % 8;
                sqy = (bit_iterator - sqx) / 8;

                square_alternator = sqx == 0 ? square_alternator : !square_alternator;

                uint64_t bit_check = one << bit_iterator;

                square_color = square_alternator ? color_light_square : color_dark_square;

                if (game.board.light & bit_check) {
                    piece_color = color_light_piece;
                    bit_piece_color = game.board.light;
                }
                else if (game.board.dark & bit_check) {
                    piece_color = color_dark_piece;
                    bit_piece_color = game.board.dark;
                }
                else {
                    piece_color = square_color;
                    bit_piece_color = static_cast<uint64_t>(0b0);
                }

                if ((game.board.pawns & bit_piece_color) & bit_check)
                    temp_piece_bitmap = pawn_bitmap;
                else if ((game.board.knights & bit_piece_color) & bit_check)
                    temp_piece_bitmap = knight_bitmap;
                else if ((game.board.bishops & bit_piece_color) & bit_check)
                    temp_piece_bitmap = bishop_bitmap;
                else if ((game.board.rooks & bit_piece_color) & bit_check)
                    temp_piece_bitmap = rook_bitmap;
                else if ((game.board.kings & bit_piece_color) & bit_check)
                    temp_piece_bitmap = king_bitmap;
                else if ((game.board.queens & bit_piece_color) & bit_check)
                    temp_piece_bitmap = queen_bitmap;
                else
                    temp_piece_bitmap = empty_bitmap;

                bool current_is_selected_square = bit_iterator == selected_square;
                bool legal_move = (selected_square != -1)
                    && check_move_legality(game, selected_square, (sqy * 8) + sqx);

                int offsetx = 0, offsety = 0;
                int bit_iterator_inner = 15;
                int iteration = 0;
                while (iteration < 3) {
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

                    bool draw_flag = true;
                    switch (iteration) {
                        case 0:
                            draw_flag = (temp_piece_bitmap.first & (one << bit_iterator_inner)) != 0;
                            break;
                        case 1:
                            draw_flag = (temp_piece_bitmap.second & (one << bit_iterator_inner)) != 0;
                            break;
                        case 2:
                            draw_flag = (temp_piece_bitmap.third & (one << bit_iterator_inner)) != 0;
                            break;
                        default:
                            break;
                    }

                    SDL_Color draw_color;


                    if (draw_flag)
                        draw_color = piece_color;
                    else {
                        if (current_is_selected_square && (offsetx == 0 || offsety == 0))
                            draw_color = color_selected;
                        else if (legal_move && (offsetx == 0 || offsety == 0))
                            draw_color = color_legal_move;
                        else
                            draw_color = square_color;
                    }

                    SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
                    SDL_RenderFillRect(renderer, &pixel);

                    offsetx += pixel_size;

                    if (bit_iterator_inner == 0) {
                        bit_iterator_inner = 64;
                        iteration++;
                    }
                    bit_iterator_inner--;
                }
            }
            
            SDL_RenderPresent(renderer);
        }

        while (SDL_PollEvent(&e)){
            switch (e.type) {
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_r:
                            reset(game);
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    SDL_GetMouseState(&x, &y);
                    x /= square_dimension;
                    y /= square_dimension;
                    click = static_cast<uint64_t>(x + (8 * y));

                    if (click == selected_square) {
                        selected_square = -1;
                        game.board_update_flag = true;
                        break;
                    }

                    if (selected_square != -1) {
                        if (check_move_legality(game, selected_square, x + (8 * y))) {
                            Game previous_game_state = game;
                            move(game, selected_square, x + (8 * y));
                            if (king_in_check(game))
                                game = previous_game_state;
                            selected_square = -1;
                        }
                    }
                    else {
                        if ((game.board.light | game.board.dark) & static_cast<uint64_t>(one << click)) {
                            selected_square = click;
                            game.board_update_flag = true;
                        }
                    }
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