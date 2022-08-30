#include "Chess.h"

ChessState::ChessState () {
        p[LIGHT] = static_cast<uint64_t>(0b1111111111111111000000000000000000000000000000000000000000000000);
        p[DARK] = static_cast<uint64_t>(0b0000000000000000000000000000000000000000000000001111111111111111);
        p[PAWN] = static_cast<uint64_t>(0b0000000011111111000000000000000000000000000000001111111100000000);
        p[KNIGHT] = static_cast<uint64_t>(0b0100001000000000000000000000000000000000000000000000000001000010);
        p[BISHOP] = static_cast<uint64_t>(0b0010010000000000000000000000000000000000000000000000000000100100);
        p[ROOK] = static_cast<uint64_t>(0b1000000100000000000000000000000000000000000000000000000010000001);
        p[KING] = static_cast<uint64_t>(0b0001000000000000000000000000000000000000000000000000000000010000);
        p[QUEEN] = static_cast<uint64_t>(0b0000100000000000000000000000000000000000000000000000000000001000);

        en_passant[LIGHT] = char(0);
        en_passant[DARK] = char(0);

        castling_privilege[LIGHT] = char(0b11111111);
        castling_privilege[DARK] = char(0b11111111);
}

ChessStateChange::ChessStateChange (int s, int d) : src(s), dest(d) {}

HumanChessPlayer::HumanChessPlayer () {}

ChessStateChange HumanChessPlayer::get_move (ChessState* s) {
    int x = 0;
    int y = 0;
    SDL_GetMouseState(&x, &y);
    int click = (y * 8) + x;
    if (click == src)
        src = -1;
    else if (src == -1)
        src = click;
    else
        dest = click;

    ChessStateChange csc(src, dest);
    return csc;
}

ChessState ChessLogicObject::utility_chess_move (ChessState* s, ChessStateChange c) {
    ChessState r = *s;
    move_piece(&r, c.src, c.dest);
    return r;
}

bool ChessLogicObject::isLegalChange (ChessState* s, ChessStateChange c) {

    ChessState new_s = utility_chess_move (s, c);

    int side = LIGHT;

    uint64_t one = static_cast<uint64_t>(1);
    uint64_t src_square = one << c.src;
    uint64_t dest_square = one << c.dest;

    if (new_s.p[DARK] & src_square)
        side = DARK;

    if (new_s.p[PAWN] & src_square)
        return dest_square & pawn_movespace(side, c.src, new_s.p[side], new_s.p[(side + 1) % 2]);
    else if (new_s.p[KNIGHT] & src_square)
        return dest_square & knight_movespace(c.src, new_s.p[side]);
    else if (new_s.p[BISHOP] & src_square)
        return dest_square & bishop_movespace(c.src, new_s.p[side], new_s.p[(side + 1) % 2]);
    else if (new_s.p[ROOK] & src_square)
        return dest_square & rook_movespace(c.src, new_s.p[side], new_s.p[(side + 1) % 2]);
    else if (new_s.p[QUEEN] & src_square)
        return dest_square & queen_movespace(c.src, new_s.p[side], new_s.p[(side + 1) % 2]);
    else if (new_s.p[KING] & src_square)
        return dest_square & king_movespace(c.src, new_s.p[side], new_s.movespace[(side + 1) % 2], side);

    return false;
}

bool ChessLogicObject::check_end_condition () {
    return false;
}

uint64_t ChessLogicObject::movespace (ChessState* board, int src) {
    uint64_t one = static_cast<uint64_t>(1);
    uint64_t src_square = one << src;

    int side = LIGHT;
    if (src_square & board->p[LIGHT])
        side = LIGHT;
    else if (src_square & board->p[DARK])
        side = DARK;
    else
        return 0;

    if (src_square & board->p[PAWN])
        return pawn_movespace(src, board->p[side], board->p[(side + 1) % 2], side);
    else if (src_square & board->p[KNIGHT])
        return knight_movespace(src, board->p[side]);
    else if (src_square & board->p[BISHOP])
        return bishop_movespace(src, board->p[side], board->p[(side + 1) % 2]);
    else if (src_square & board->p[ROOK])
        return rook_movespace(src, board->p[side], board->p[(side + 1) % 2]);
    else if (src_square & board->p[QUEEN])
        return queen_movespace(src, board->p[side], board->p[(side + 1) % 2]);
    else if (src_square & board->p[KING])
        return king_movespace(src, board->p[side], board->p[(side + 1) % 2], side);

    return static_cast<uint64_t>(0);
}

void ChessLogicObject::move_piece (ChessState* board, int src, int dest) {
    int64_t one = static_cast<int64_t>(1);
    int64_t src_square = one << src;
    int64_t dest_square = one << dest;

    if (board->p[LIGHT] & src_square) {
        board->p[LIGHT] -= src_square;
        board->p[LIGHT] = board->p[LIGHT] | dest_square;
        if (board->p[DARK] & dest_square)
            board->p[DARK] -= dest_square;
    }
    else if (board->p[DARK] & src_square) {
        board->p[DARK] -= src_square;
        board->p[DARK] = board->p[DARK] | dest_square;
        if (board->p[LIGHT] & dest_square)
            board->p[LIGHT] -= dest_square;
    }

    if (board->p[PAWN] & dest_square)
        board->p[PAWN] -= dest_square;
    else if (board->p[KNIGHT] & dest_square)
        board->p[KNIGHT] -= dest_square;
    else if (board->p[BISHOP] & dest_square) 
        board->p[BISHOP] -= dest_square;
    else if (board->p[ROOK] & dest_square)
        board->p[ROOK] -= dest_square;
    else if (board->p[QUEEN] & dest_square)
        board->p[QUEEN] -= dest_square;
    else if (board->p[KING] & dest_square)
        board->p[KING] -= dest_square;

    if (board->p[PAWN] & src_square) {
        board->p[PAWN] -= src_square;
        board->p[PAWN] += dest_square;
    }
    else if (board->p[KNIGHT] & src_square) {
        board->p[KNIGHT] -= src_square;
        board->p[KNIGHT] += dest_square;
    }
    else if (board->p[BISHOP] & src_square) {
        board->p[BISHOP] -= src_square;
        board->p[BISHOP] += dest_square;
    }
    else if (board->p[ROOK] & src_square) {
        board->p[ROOK] -= src_square;
        board->p[ROOK] += dest_square;
    }
    else if (board->p[QUEEN] & src_square) {
        board->p[QUEEN] -= src_square;
        board->p[QUEEN] += dest_square;
    }
    else if (board->p[KING] & src_square) {
        board->p[KING] -= src_square;
        board->p[KING] += dest_square;
    }
}

bool ChessLogicObject::is_king_in_check (ChessState* board, int side) {
    board->movespace[LIGHT] = side_movespace(board, LIGHT);
    board->movespace[DARK] = side_movespace(board, DARK);
    return (board->p[KING] & board->p[side]) & board->movespace[(side + 1) % 2];
}

uint64_t ChessLogicObject::side_movespace (ChessState* board, int side) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t movespace_iter = static_cast<uint64_t>(1);

    for (int i = 0; i < 64; i++) {
        if ((board->p[PAWN] & board->p[side]) & movespace_iter)
            movespace = movespace | pawn_movespace(side, i, board->p[side], board->p[(side + 1) % 2]);
        if ((board->p[KNIGHT] & board->p[side]) & movespace_iter)
            movespace = movespace | knight_movespace(i, board->p[side]);
        if ((board->p[BISHOP] & board->p[side]) & movespace_iter)
            movespace = movespace | bishop_movespace(i, board->p[side], board->p[(side + 1) % 2]);
        if ((board->p[ROOK] & board->p[side]) & movespace_iter)
            movespace = movespace | rook_movespace(i, board->p[side], board->p[(side + 1) % 2]);
        if ((board->p[QUEEN] & board->p[side]) & movespace_iter)
            movespace = movespace | queen_movespace(i, board->p[side], board->p[(side + 1) % 2]);
        if ((board->p[KING] & board->p[side]) & movespace_iter)
            movespace = movespace | king_movespace(i, board->p[side], board->movespace[(side + 1) % 2], side);
        
        movespace_iter = movespace_iter << 1;
    }

    return movespace;
}

uint64_t ChessLogicObject::pawn_movespace (int src, uint64_t ally, uint64_t enemy, int side) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t square = static_cast<uint64_t>(movespace + 1);
    square = square << src;

    if (src % 8 == src || src - (src % 8) == 56)
        return 0;
    if (src % 8 != 0) {
        if (side == LIGHT) {
            if (enemy & (square >> 9))
                movespace += (square >> 9);
        }
        else {
            if (enemy & (square << 7))
                movespace += (square << 7);
        }
    }
    if (src % 8 != 7) {
        if (side == LIGHT) {
            if (enemy & (square >> 7))
                movespace += (square >> 7);
        }
        else {
            if (enemy & (square << 9))
                movespace += (square << 9);
        }
    }
    if (side == LIGHT) {
        if ((((enemy | ally) & (square >> 8))) == 0)
            movespace += square >> 8;
        if ((src - (src % 8)) / 8 == 6 && ((enemy | ally) & (square >> 16)) == 0)
            movespace += square >> 16;
    }
    else {
        if (((enemy | ally) & (square << 8)) == 0)
            movespace += square << 8;
        if ((src - (src % 8)) / 8 == 1 && ((enemy | ally) & (square << 16)) == 0)
            movespace += square << 16;
    }
    return movespace;
}

uint64_t ChessLogicObject::knight_movespace (int src, uint64_t ally) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t square = (movespace + 1) << src;

    int srcx = src % 8;
    int srcy = (src - srcx) / 8;

    if (srcx < 7 && srcy > 1) 
        movespace += square >> 15;
    if (srcx < 6 && srcy > 0)
        movespace += square >> 6;
    if (srcx < 6 && srcy < 7)
        movespace += square << 10;
    if (srcx < 7 && srcy < 6)
        movespace += square << 17;
    if (srcx > 0 && srcy < 6)
        movespace += square << 15;
    if (srcx > 1 && srcy < 7)
        movespace += square << 6;
    if (srcx > 1 && srcy > 0)
        movespace += square >> 10;
    if (srcx > 0 && srcy > 1)
        movespace += square >> 17;

    return movespace;
}

uint64_t ChessLogicObject::bishop_movespace (int src, uint64_t ally, uint64_t enemy) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t iter = (movespace + 1) << src;

    int srcx = src % 8; // 6
    int srcy = (src - srcx) / 8; // 3

    auto min = [] (int a, int b) {
        return (a < b) ? a : b;
    };

    // Uper Left
    for (int i = 1; i <= min(srcx, srcy); i++) {
        uint64_t square = iter >> (i * 9);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Uper Right
    for (int i = 1; i <= min(8 - srcx - 1, srcy); i++) {
        uint64_t square = iter >> (i * 7);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Lower Right
    for (int i = 1; i <= min(8 - srcx - 1, 8 - srcy - 1); i++) {
        uint64_t square = iter << (i * 9);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Lower Left
    for (int i = 1; i <= min(srcx, 8 - srcy - 1); i++) {
        uint64_t square = iter << (i * 7);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    return movespace;
}

uint64_t ChessLogicObject::rook_movespace (int src, uint64_t ally, uint64_t enemy) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t iter = (movespace + 1) << src;

    int srcx = src % 8;
    int srcy = (src - srcx) / 8;

    // Up
    for (int i = 1; i <= srcy; i++) {
        uint64_t square = iter >> (i * 8);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Right
    for (int i = 1; i <= 8 - srcx - 1; i++) {
        uint64_t square = iter << i;
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Down
    for (int i = 1; i <= 8 - srcy - 1; i++) {
        uint64_t square = iter << (i * 8);
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    // Left
    for (int i = 1; i <= srcx; i++) {
        uint64_t square = iter >> i;
        if (ally & square)
            break;

        movespace += square;

        if (enemy & square)
            break;
    }

    return movespace;
}

uint64_t ChessLogicObject::queen_movespace (int src, uint64_t ally, uint64_t enemy) {
    return bishop_movespace(src, ally, enemy) + rook_movespace(src, ally, enemy);
}

uint64_t ChessLogicObject::king_movespace (int src, uint64_t ally, uint64_t enemy_movespace, int side) {
    uint64_t movespace = static_cast<uint64_t>(0);
    uint64_t one = static_cast<uint64_t>(1);
    uint64_t square = one << src;

    uint64_t illegal_squares = ally | enemy_movespace;

    if (src - (src % 8) != 56) {
        if ((illegal_squares & (square << 8)) == 0)
            movespace += square << 8;
    }
    if (src % 8 != 7) {
        if ((illegal_squares & (square << 1)) == 0)
            movespace += square << 1;
    }
    if (src % 8 != 0) {
        if ((illegal_squares & (square >> 1)) == 0)
            movespace += square >> 1;
    }
    if (src % 8 != src) {
        if ((illegal_squares & (square >> 8)) == 0)
            movespace += square >> 8;
    }

    if (src % 8 != 0 && src % 8 != src) {
        if ((illegal_squares & (square >> 9)) == 0)
            movespace += square >> 9;
    }
    if (src % 8 != 7 && src % 8 != src) {
        if ((illegal_squares & (square >> 7)) == 0)
            movespace += square >> 7;
    }
    if (src % 8 != 7 && src - (src % 8) != 56) {
        if ((illegal_squares & (square << 9)) == 0)
            movespace += square << 9;
    }
    if (src % 8 != 0 && src - (src % 8) != 56) {
        if ((illegal_squares & (square << 7)) == 0)
            movespace += square << 7;
    }
    return movespace;
}

void ChessGame::display () {
    SDL_Color piece_color, square_color;

    Chess_Display_12_12::Bitmap::bitmap temp_piece_bitmap;

    int square_piece_color = LIGHT;
    uint64_t one = static_cast<uint64_t>(1);

    int sqx = 0, sqy = 0;
    bool square_alternator = true;

    uint64_t bit_check = one;

    ChessState* state_ptr = get_state_pointer();

    for (int bit_iterator = 0; bit_iterator < 64; bit_iterator++) {
        sqx = bit_iterator % 8;
        sqy = (bit_iterator - sqx) / 8;

        square_alternator = sqx == 0 ? square_alternator : !square_alternator;

        square_color = square_alternator ? Chess_Display_12_12::Color::light_square : Chess_Display_12_12::Color::dark_square;

        if (state_ptr->p[LIGHT] & bit_check) {
            piece_color = Chess_Display_12_12::Color::light_piece;
            square_piece_color = LIGHT;
        }
        else if (state_ptr->p[DARK] & bit_check) {
            piece_color = Chess_Display_12_12::Color::dark_piece;
            square_piece_color = DARK;
        }
        else {
            piece_color = square_color;
            square_piece_color = static_cast<uint64_t>(0b0);
        }

        if ((state_ptr->p[PAWN] & state_ptr->p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess_Display_12_12::Bitmap::pawn;
        else if ((state_ptr->p[KNIGHT] & state_ptr->p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess_Display_12_12::Bitmap::knight;
        else if ((state_ptr->p[BISHOP] & state_ptr->p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess_Display_12_12::Bitmap::bishop;
        else if ((state_ptr->p[ROOK] & state_ptr->p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess_Display_12_12::Bitmap::rook;
        else if ((state_ptr->p[KING] & state_ptr->p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess_Display_12_12::Bitmap::king;
        else if ((state_ptr->p[QUEEN] & state_ptr->p[square_piece_color]) & bit_check)
            temp_piece_bitmap = Chess_Display_12_12::Bitmap::queen;
        else
            temp_piece_bitmap = Chess_Display_12_12::Bitmap::empty;

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

            int draw_flag = 0;
            switch (iteration) {
                case 0:
                    if ((temp_piece_bitmap.first & (one << bit_iterator_inner)) != 0)
                        draw_flag = 1;
                    break;
                case 1:
                    if ((temp_piece_bitmap.second & (one << bit_iterator_inner)) != 0)
                        draw_flag = 1;
                    break;
                case 2:
                    if ((temp_piece_bitmap.third & (one << bit_iterator_inner)) != 0)
                        draw_flag = 1;
                    break;
                default:
                    break;
            }

            SDL_Color draw_color;

            if (draw_flag == 0)
                draw_color = square_color;
            else
                draw_color = piece_color;
                
            SDL_SetRenderDrawColor(renderer, draw_color.r, draw_color.g, draw_color.b, draw_color.a);
            SDL_RenderFillRect(renderer, &pixel);

            offsetx += SIZE;

            if (bit_iterator_inner == 0) {
                bit_iterator_inner = 64;
                iteration++;
            }
            bit_iterator_inner--;
        }
        bit_check = bit_check << 1;
    }
}

void ChessGame::display_win_message() {
    return;
}

void ChessGame::init_sdl(SDL_Window* w, SDL_Renderer* r) {
    window = w;
    renderer = r;
}
