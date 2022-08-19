#include "new.h"

void print_int64 (uint64_t i) {
    uint64_t iter = static_cast<uint64_t>(1);

    std::cout << "Bit Mask of Integer Value: " << i << std::endl;
    for (int _ = 0; _ < 64; _++) {
        if (_!=0 && _%8 == 0)
            std::cout << std::endl;
        if (iter & i)
            std::cout << "X";
        else
            std::cout << "-";
        iter = iter << 1;
    }
    std::cout << std::endl;
}

int min (int a, int b) {
    return (a < b) ? a : b;
}

ChessState::ChessState () {
    p[LIGHT] = static_cast<uint64_t>(0);
    p[DARK] = static_cast<uint64_t>(0);
    p[PAWN] = static_cast<uint64_t>(0);
    p[KNIGHT] = static_cast<uint64_t>(0);
    p[BISHOP] = static_cast<uint64_t>(0);
    p[ROOK] = static_cast<uint64_t>(0);
    p[KING] = static_cast<uint64_t>(0);
    p[QUEEN] = static_cast<uint64_t>(0);

    en_passant[0] = char(0);
    en_passant[1] = char(0);

    castling_privilege[0] = char(0);
    castling_privilege[1] = char(0);

    updated = false;
}

void default_chess_state (ChessState& s) {
    s.p[LIGHT] = static_cast<uint64_t>(0b1111111111111111000000000000000000000000000000000000000000000000);
    s.p[DARK] = static_cast<uint64_t>(0b0000000000000000000000000000000000000000000000001111111111111111);
    s.p[PAWN] = static_cast<uint64_t>(0b0000000011111111000000000000000000000000000000001111111100000000);
    s.p[KNIGHT] = static_cast<uint64_t>(0b0100001000000000000000000000000000000000000000000000000001000010);
    s.p[BISHOP] = static_cast<uint64_t>(0b0010010000000000000000000000000000000000000000000000000000100100);
    s.p[ROOK] = static_cast<uint64_t>(0b1000000100000000000000000000000000000000000000000000000010000001);
    s.p[KING] = static_cast<uint64_t>(0b0001000000000000000000000000000000000000000000000000000000010000);
    s.p[QUEEN] = static_cast<uint64_t>(0b0000100000000000000000000000000000000000000000000000000000001000);

    s.en_passant[0] = char(0);
    s.en_passant[1] = char(1);

    s.castling_privilege[0] = char(0b11111111);
    s.castling_privilege[1] = char(0b11111111);

    update_attack_space(s);
}

uint64_t get_pawn_attack_space (int side, int src, uint64_t ally, uint64_t enemy) {
    uint64_t attack_space = static_cast<uint64_t>(0);
    uint64_t square = static_cast<uint64_t>(attack_space + 1);
    square = square << src;

    if (src % 8 == src || src - (src % 8) == 56)
        return 0;
    if (src % 8 != 0) {
        if (side == LIGHT) {
            if (enemy & (square >> 9))
                attack_space += (square >> 9);
        }
        else {
            if (enemy & (square << 7))
                attack_space += (square << 7);
        }
    }
    if (src % 8 != 7) {
        if (side == LIGHT) {
            if (enemy & (square >> 7))
                attack_space += (square >> 7);
        }
        else {
            if (enemy & (square << 9))
                attack_space += (square << 9);
        }
    }
    if (side == LIGHT) {
        if ((((enemy | ally) & (square >> 8))) == 0)
            attack_space += square >> 8;
        if ((src - (src % 8)) / 8 == 6 && ((enemy | ally) & (square >> 16)) == 0)
            attack_space += square >> 16;
    }
    else {
        if (((enemy | ally) & (square << 8)) == 0)
            attack_space += square << 8;
        if ((src - (src % 8)) / 8 == 1 && ((enemy | ally) & (square << 16)) == 0)
            attack_space += square << 16;
    }
    return attack_space;
}

uint64_t get_knight_attack_space (int src, uint64_t ally) {
    uint64_t attack_space = static_cast<uint64_t>(0);
    uint64_t square = (attack_space + 1) << src;

    int srcx = src % 8;
    int srcy = (src - srcx) / 8;

    if (srcx < 7 && srcy > 1) 
        attack_space += square >> 15;
    if (srcx < 6 && srcy > 0)
        attack_space += square >> 6;
    if (srcx < 6 && srcy < 7)
        attack_space += square << 10;
    if (srcx < 7 && srcy < 6)
        attack_space += square << 17;
    if (srcx > 0 && srcy < 6)
        attack_space += square << 15;
    if (srcx > 1 && srcy < 7)
        attack_space += square << 6;
    if (srcx > 1 && srcy > 0)
        attack_space += square >> 10;
    if (srcx > 0 && srcy > 1)
        attack_space += square >> 17;

    return attack_space;
}

uint64_t get_bishop_attack_space (int src, uint64_t ally, uint64_t enemy) {
    uint64_t attack_space = static_cast<uint64_t>(0);
    uint64_t iter = (attack_space + 1) << src;

    int srcx = src % 8; // 6
    int srcy = (src - srcx) / 8; // 3

    // Upper Left
    for (int i = 1; i <= min(srcx, srcy); i++) {
        uint64_t square = iter >> (i * 9);
        if (ally & square)
            break;

        attack_space += square;

        if (enemy & square)
            break;
    }

    // Upper Right
    for (int i = 1; i <= min(8 - srcx - 1, srcy); i++) {
        uint64_t square = iter >> (i * 7);
        if (ally & square)
            break;

        attack_space += square;

        if (enemy & square)
            break;
    }

    // Lower Right
    for (int i = 1; i <= min(8 - srcx - 1, 8 - srcy - 1); i++) {
        uint64_t square = iter << (i * 9);
        if (ally & square)
            break;

        attack_space += square;

        if (enemy & square)
            break;
    }

    // Lower Left
    for (int i = 1; i <= min(srcx, 8 - srcy - 1); i++) {
        uint64_t square = iter << (i * 7);
        if (ally & square)
            break;

        attack_space += square;

        if (enemy & square)
            break;
    }

    return attack_space;
}

uint64_t get_rook_attack_space (int src, uint64_t ally, uint64_t enemy) {
    uint64_t attack_space = static_cast<uint64_t>(0);
    uint64_t iter = (attack_space + 1) << src;

    int srcx = src % 8;
    int srcy = (src - srcx) / 8;

    // Up
    for (int i = 1; i <= srcy; i++) {
        uint64_t square = iter >> (i * 8);
        if (ally & square)
            break;

        attack_space += square;

        if (enemy & square)
            break;
    }

    // Right
    for (int i = 1; i <= 8 - srcx - 1; i++) {
        uint64_t square = iter << i;
        if (ally & square)
            break;

        attack_space += square;

        if (enemy & square)
            break;
    }

    // Down
    for (int i = 1; i <= 8 - srcy - 1; i++) {
        uint64_t square = iter << (i * 8);
        if (ally & square)
            break;

        attack_space += square;

        if (enemy & square)
            break;
    }

    // Left
    for (int i = 1; i <= srcx; i++) {
        uint64_t square = iter >> i;
        if (ally & square)
            break;

        attack_space += square;

        if (enemy & square)
            break;
    }

    return attack_space;
}

uint64_t get_queen_attack_space (int src, uint64_t ally, uint64_t enemy) {
    return get_bishop_attack_space(src, ally, enemy) + get_rook_attack_space(src, ally, enemy);
}

uint64_t get_king_attack_space (int src, uint64_t ally, uint64_t enemy_attack_space, int side) {
    uint64_t attack_space = static_cast<uint64_t>(0);
    uint64_t one = static_cast<uint64_t>(1);
    uint64_t square = one << src;

    uint64_t illegal_squares = ally | enemy_attack_space;

    if (src - (src % 8) != 56) {
        if ((illegal_squares & (square << 8)) == 0)
            attack_space += square << 8;
    }
    if (src % 8 != 7) {
        if ((illegal_squares & (square << 1)) == 0)
            attack_space += square << 1;
    }
    if (src % 8 != 0) {
        if ((illegal_squares & (square >> 1)) == 0)
            attack_space += square >> 1;
    }
    if (src % 8 != src) {
        if ((illegal_squares & (square >> 8)) == 0)
            attack_space += square >> 8;
    }

    if (src % 8 != 0 && src % 8 != src) {
        if ((illegal_squares & (square >> 9)) == 0)
            attack_space += square >> 9;
    }
    if (src % 8 != 7 && src % 8 != src) {
        if ((illegal_squares & (square >> 7)) == 0)
            attack_space += square >> 7;
    }
    if (src % 8 != 7 && src - (src % 8) != 56) {
        if ((illegal_squares & (square << 9)) == 0)
            attack_space += square << 9;
    }
    if (src % 8 != 0 && src - (src % 8) != 56) {
        if ((illegal_squares & (square << 7)) == 0)
            attack_space += square << 7;
    }
    return attack_space;
}

void update_attack_space (ChessState& s) {
    s.attack_space[LIGHT] = get_side_attack_space(s, LIGHT);
    s.attack_space[DARK] = get_side_attack_space(s, DARK);
    s.updated = true;
}

uint64_t get_side_attack_space (ChessState& s, int side) {
    uint64_t attack_space = static_cast<uint64_t>(0);
    uint64_t attack_space_iter = static_cast<uint64_t>(1);

    for (int i = 0; i < 64; i++) {
        if ((s.p[PAWN] & s.p[side]) & attack_space_iter)
            attack_space = attack_space | get_pawn_attack_space(side, i, s.p[side], s.p[(side + 1) % 2]);
        if ((s.p[KNIGHT] & s.p[side]) & attack_space_iter)
            attack_space = attack_space | get_knight_attack_space(i, s.p[side]);
        if ((s.p[BISHOP] & s.p[side]) & attack_space_iter)
            attack_space = attack_space | get_bishop_attack_space(i, s.p[side], s.p[(side + 1) % 2]);
        if ((s.p[ROOK] & s.p[side]) & attack_space_iter)
            attack_space = attack_space | get_rook_attack_space(i, s.p[side], s.p[(side + 1) % 2]);
        if ((s.p[QUEEN] & s.p[side]) & attack_space_iter)
            attack_space = attack_space | get_queen_attack_space(i, s.p[side], s.p[(side + 1) % 2]);
        if ((s.p[KING] & s.p[side]) & attack_space_iter)
            attack_space = attack_space | get_king_attack_space(i, s.p[side], s.attack_space[(side + 1) % 2], side);
        
        attack_space_iter = attack_space_iter << 1;
    }

    return attack_space;
}

bool king_in_check (ChessState& s, int side) {
    update_attack_space(s);
    return (s.p[KING] & s.p[side]) & s.attack_space[(side + 1) % 2];
}

void normal_piece_move (ChessState& s, int src, int dest) {
    int64_t one = static_cast<int64_t>(1);
    int64_t src_square = one << src;
    int64_t dest_square = one << dest;

    if (s.p[LIGHT] & src_square) {
        s.p[LIGHT] -= src_square;
        s.p[LIGHT] = s.p[LIGHT] | dest_square;
        if (s.p[DARK] & dest_square)
            s.p[DARK] -= dest_square;
    }
    else if (s.p[DARK] & src_square) {
        s.p[DARK] -= src_square;
        s.p[DARK] = s.p[DARK] | dest_square;
        if (s.p[LIGHT] & dest_square)
            s.p[LIGHT] -= dest_square;
    }

    if (s.p[PAWN] & dest_square)
        s.p[PAWN] -= dest_square;
    else if (s.p[KNIGHT] & dest_square)
        s.p[KNIGHT] -= dest_square;
    else if (s.p[BISHOP] & dest_square) 
        s.p[BISHOP] -= dest_square;
    else if (s.p[ROOK] & dest_square)
        s.p[ROOK] -= dest_square;
    else if (s.p[QUEEN] & dest_square)
        s.p[QUEEN] -= dest_square;
    else if (s.p[KING] & dest_square)
        s.p[KING] -= dest_square;

    if (s.p[PAWN] & src_square) {
        s.p[PAWN] -= src_square;
        s.p[PAWN] += dest_square;
    }
    else if (s.p[KNIGHT] & src_square) {
        s.p[KNIGHT] -= src_square;
        s.p[KNIGHT] += dest_square;
    }
    else if (s.p[BISHOP] & src_square) {
        s.p[BISHOP] -= src_square;
        s.p[BISHOP] += dest_square;
    }
    else if (s.p[ROOK] & src_square) {
        s.p[ROOK] -= src_square;
        s.p[ROOK] += dest_square;
    }
    else if (s.p[QUEEN] & src_square) {
        s.p[QUEEN] -= src_square;
        s.p[QUEEN] += dest_square;
    }
    else if (s.p[KING] & src_square) {
        s.p[KING] -= src_square;
        s.p[KING] += dest_square;
    }
}

bool move_legality_check (ChessState& s, int src, int dest) {
    int side = LIGHT;

    uint64_t one = static_cast<uint64_t>(1);
    uint64_t src_square = one << src;
    uint64_t dest_square = one << dest;

    if (s.p[DARK] & src_square)
        side = DARK;

    if (s.p[PAWN] & src_square)
        return dest_square & get_pawn_attack_space(side, src, s.p[side], s.p[(side + 1) % 2]);
    else if (s.p[KNIGHT] & src_square)
        return dest_square & get_knight_attack_space(src, s.p[side]);
    else if (s.p[BISHOP] & src_square)
        return dest_square & get_bishop_attack_space(src, s.p[side], s.p[(side + 1) % 2]);
    else if (s.p[ROOK] & src_square)
        return dest_square & get_rook_attack_space(src, s.p[side], s.p[(side + 1) % 2]);
    else if (s.p[QUEEN] & src_square)
        return dest_square & get_queen_attack_space(src, s.p[side], s.p[(side + 1) % 2]);
    else if (s.p[KING] & src_square)
        return dest_square & get_king_attack_space(src, s.p[side], s.attack_space[(side + 1) % 2], side);

    return false;
}

int evaluate (ChessState& s) {
    return 1;
}

int tree_search (ChessState& s, int d) {
    if (d == 0)
        return evaluate (s);

    int sum = 0;

    return sum;
}

void display_chess_state (ChessState& s, SDL_Window* window, SDL_Renderer* renderer) {
    SDL_Color piece_color, square_color;
    s.updated = false;

    Chess::bitmap temp_piece_bitmap;
    int square_piece_color = LIGHT;
    uint64_t one = 0b1;

    int sqx = 0, sqy = 0;
    bool square_alternator = true;
    for (int bit_iterator = 0; bit_iterator < 64; bit_iterator++) {
        sqx = bit_iterator % 8;
        sqy = (bit_iterator - sqx) / 8;

        square_alternator = sqx == 0 ? square_alternator : !square_alternator;

        uint64_t bit_check = one << bit_iterator;

        square_color = square_alternator ? Chess::color_light_square : Chess::color_dark_square;

        if (s.p[LIGHT] & bit_check) {
            piece_color = Chess::color_light_piece;
            square_piece_color = LIGHT;
        }
        else if (s.p[DARK] & bit_check) {
            piece_color = Chess::color_dark_piece;
            square_piece_color = DARK;
        }
        else {
            piece_color = square_color;
            square_piece_color = static_cast<uint64_t>(0b0);
        }

        if ((s.p[PAWN] & s.p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess::pawn_bitmap;
        else if ((s.p[KNIGHT] & s.p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess::knight_bitmap;
        else if ((s.p[BISHOP] & s.p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess::bishop_bitmap;
        else if ((s.p[ROOK] & s.p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess::rook_bitmap;
        else if ((s.p[KING] & s.p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess::king_bitmap;
        else if ((s.p[QUEEN] & s.p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess::queen_bitmap;
        else
            temp_piece_bitmap = Chess::empty_bitmap;

        int offsetx = 0, offsety = 0;
        int bit_iterator_inner = 15;
        int iteration = 0;
        while (iteration < 3) {
            if (offsetx == SIZE * 12) {
                offsetx = 0;
                offsety += SIZE;
            }
            
            SDL_Rect pixel = {
                .x = (sqx * SIZE * 12) + offsetx,
                .y = (sqy * SIZE * 12) + offsety,
                .w = SIZE,
                .h = SIZE
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
            else
                draw_color = square_color;

            SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
            SDL_RenderFillRect(renderer, &pixel);

            offsetx += SIZE;

            if (bit_iterator_inner == 0) {
                bit_iterator_inner = 64;
                iteration++;
            }
            bit_iterator_inner--;
        }
    }
}

void display_selection_state (int selected_square, SDL_Window* window, SDL_Renderer* renderer) {
    int sqx = selected_square % 8;
    int sqy = (selected_square - sqx) / 8;

    uint64_t one = static_cast<uint64_t>(1);

    int offsetx = 0, offsety = 0;
    int bit_iterator = 15;
    int iteration = 0;
    while (iteration < 3) {
        if (offsetx == SIZE * 12) {
            offsetx = 0;
            offsety += SIZE;
        }
        
        SDL_Rect pixel = {
            .x = (sqx * SIZE * 12) + offsetx,
            .y = (sqy * SIZE * 12) + offsety,
            .w = SIZE,
            .h = SIZE
        };

        bool draw_flag = true;
        switch (iteration) {
            case 0:
                draw_flag = (Chess::selection_bitmap.first & (one << bit_iterator)) != 0;
                break;
            case 1:
                draw_flag = (Chess::selection_bitmap.second & (one << bit_iterator)) != 0;
                break;
            case 2:
                draw_flag = (Chess::selection_bitmap.third & (one << bit_iterator)) != 0;
                break;
            default:
                break;
        }

        SDL_Color draw_color;


        if (draw_flag) {
            SDL_SetRenderDrawColor(renderer, Chess::color_selected.r, Chess::color_selected.g, Chess::color_selected.b, Chess::color_selected.a);
            SDL_RenderFillRect(renderer, &pixel);
        }

        offsetx += SIZE;

        if (bit_iterator == 0) {
            bit_iterator = 64;
            iteration++;
        }
        bit_iterator--;
    }
}

void display_overlay (SDL_Window* window, SDL_Renderer* renderer) {

}


int main () {
    int square_dimension = SIZE * 12;

    int pixel_size = square_dimension / 12;

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

    ChessState game;
    default_chess_state(game);

    int selected_square = -1;
    int playing = 1;
    int turn = LIGHT;

    uint64_t one = static_cast<uint64_t>(1);

    int x = 0, y = 0;

    while (playing == 1) {
        if (game.updated) {
            update_attack_space(game);
            display_chess_state(game, window, renderer);
            if (selected_square != -1)
                display_selection_state (selected_square, window, renderer);
            SDL_RenderPresent(renderer);
            game.updated = false;
        }

        SDL_Event e;
        uint64_t click;
        uint64_t attack_space = 0;

        while (SDL_PollEvent(&e)){
            switch (e.type) {
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_r:
                            default_chess_state(game);
                            selected_square = -1;
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    SDL_GetMouseState(&x, &y);
                    x /= square_dimension;
                    y /= square_dimension;
                    click = static_cast<uint64_t>(x + (8 * y));

                    if (selected_square == -1) {
                        if (game.p[turn] & (one << click)) {
                            selected_square = click;
                            game.updated = true;
                        }
                        break;
                    }

                    if (click == selected_square) {
                        selected_square = -1;
                        game.updated = true;
                        break;
                    }

                    if (selected_square != -1) {
                        attack_space = game.attack_space[turn];
                        if (attack_space & (one << click)) {
                            if (move_legality_check(game, selected_square, click)) {
                                std::cout << "checking legality" << std::endl;
                                ChessState copy = game;
                                normal_piece_move(game, selected_square, click);
                                update_attack_space(game);
                                if (king_in_check(game, turn)) {
                                    game = copy;
                                    update_attack_space(game);
                                    std::cout << "KING WAS IN CHECK" << std::endl;
                                    break;
                                }
                                game.updated = true;
                                selected_square = -1;
                                turn = (turn + 1) % 2;
                            }
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