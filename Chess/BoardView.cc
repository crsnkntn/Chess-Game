#include "BoardView.h"

void Chess::SDL_Board_View::display_piece (int sq_color, int pc_color, int piece_type, int loc) {
    int offsetx = 0, offsety = 0;
    int iter = 15;
    int iteration = 0;
    int sqx = loc % 8;
    int sqy = (loc - sqx) / 8;

    Bitmap::bitmap temp = Bitmap::empty;

    switch (piece_type) {
        case PAWN:
            temp = Bitmap::pawn;
            break;
        case KNIGHT:
            temp = Bitmap::knight;
            break;
        case BISHOP:
            temp = Bitmap::bishop;
            break;
        case ROOK:
            temp = Bitmap::rook;
            break;
        case QUEEN:
            temp = Bitmap::queen;
            break;
        case KING:
            temp = Bitmap::king;
            break;
        default:
            break;
    }

    SDL_Color square_color = (sq_color == LIGHT) ? Color::light_square : Color::dark_square;
    SDL_Color piece_color = (pc_color == LIGHT) ? Color::light_piece : Color::dark_piece;

    uint64_t one = static_cast<uint64_t>(1);

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

        int draw_flag = 0;
        switch (iteration) {
            case 0:
                if (temp.first & (one << iter))
                    draw_flag = 1;
                break;
            case 1:
                if (temp.second & (one << iter))
                    draw_flag = 1;
                break;
            case 2:
                if (temp.third & (one << iter))
                    draw_flag = 1;
                break;
            default:
                break;
        }

        SDL_Color draw_color = (draw_flag == 0) ? square_color : piece_color;

        SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
        SDL_RenderFillRect(renderer, &pixel);

        offsetx += SIZE;

        if (iter == 0) {
            iter = 64;
            iteration++;
        }
        iter--;
    }
}

Chess::SDL_Board_View::SDL_Board_View (int x, int y) : offsetx(x), offsety(y) {
    int square_dimension = SIZE * 12;

    int pixel_size = square_dimension / 12;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("error: failed to initialize SDL: %s\n", SDL_GetError());
        exit(0);
    }

    window = SDL_CreateWindow("Projection", 
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        square_dimension * 8 + offsetx, 
        square_dimension * 8 + offsety,
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
}

Chess::SDL_Board_View::~SDL_Board_View () {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

void Chess::SDL_Board_View::display_board (State* state, int selection) {
    uint64_t one = static_cast<uint64_t>(1);
    uint64_t iter = one;

    int sq_color = LIGHT, pc_color = LIGHT, piece_type = PAWN;
    for (int i = 0; i < 64; i++) {
        pc_color = (state->p[LIGHT] & iter) ? LIGHT : DARK;

        if (state->p[PAWN] & iter)
            piece_type = PAWN;
        else if (state->p[KNIGHT] & iter)
            piece_type = KNIGHT;
        else if (state->p[BISHOP] & iter)
            piece_type = BISHOP;
        else if (state->p[ROOK] & iter)
            piece_type = ROOK;
        else if (state->p[QUEEN] & iter)
            piece_type = QUEEN;
        else if (state->p[KING] & iter)
            piece_type = KING;
        else
            piece_type = NONE;

        display_piece(sq_color, pc_color, piece_type, i);

        if (selection == i) {
            int sqx = i % 8;
            int sqy = (i - sqx) / 8;
            SDL_Rect pixel = {
                .x = (sqx * SIZE * 12) + offsetx + 4,
                .y = (sqy * SIZE * 12) + offsety + 4,
                .w = 6,
                .h = 6
            };

            SDL_SetRenderDrawColor(renderer, Color::selection.r, Color::selection.g, Color::selection.b, Color::selection.a);
            SDL_RenderFillRect(renderer, &pixel);
        }

        if (i % 8 != 7)
            sq_color = (sq_color + 1) % 2;
        
        iter = iter << 1;
    }

    SDL_RenderPresent(renderer);
}
  