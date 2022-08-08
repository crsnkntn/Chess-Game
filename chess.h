#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <bitset>

namespace Chess {
    enum Pieces {
        NO_PIECE,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        KING,
        QUEEN,
        DARK = 16
    };

    enum Modes {
        NO_PLAYERS,
        SINGLE_PLAYER_LIGHT,
        SINGLE_PLAYER_DARK,
        TWO_PLAYERS,
        OPEN_BOARD
    };

    struct Board {
        uint64_t light;
        uint64_t dark;
        uint64_t pawns;
        uint64_t knights;
        uint64_t bishops;
        uint64_t rooks;
        uint64_t kings;
        uint64_t queens;
    };

    struct Game {
        Board board;
        bool board_update_flag;
        int mode;
    };

    struct bitmap {
        uint16_t first;
        uint64_t second;
        uint64_t third;
    };

    bitmap empty_bitmap = {
        static_cast<uint16_t>(0b0),
        static_cast<uint64_t>(0b0),
        static_cast<uint64_t>(0b0)
    };

    bitmap pawn_bitmap = {
        static_cast<uint16_t>(0b0),
        static_cast<uint64_t>(0b0000000000000000000000000110000000001111000000001111000000000110),
        static_cast<uint64_t>(0b0000000001100000000001100000000011110000000111111000000000000000)
    };

    bitmap knight_bitmap = {
        static_cast<uint16_t>(0b0),
        static_cast<uint64_t>(0b0001000000011111000000011101100000001111100000000111100000000111),
        static_cast<uint64_t>(0b1000000011110000000011110000000111111000001111111100000000000000)
    };

    bitmap bishop_bitmap = {
        static_cast<int16_t>(0b0),
        static_cast<uint64_t>(0b0110000000001101000000001111000000001111000000000110000000000110),
        static_cast<uint64_t>(0b0000000011110000000011110000000111111000001111111100000000000000)
    };

    bitmap rook_bitmap = {
        static_cast<int16_t>(0b1),
        static_cast<uint64_t>(0b0110100000011111100000011111100000001111000000001111000000001111),
        static_cast<uint64_t>(0b0000000011110000000111111000000111111000001111111100000000000000)
    };

    bitmap king_bitmap = {
        static_cast<int16_t>(0b0),
        static_cast<uint64_t>(0b0110000000001111000000000110000000011111100000011111100000011111),
        static_cast<uint64_t>(0b1000000011110000000111111000000111111000001111111100000000000000)
    };

    bitmap queen_bitmap = {
        static_cast<int16_t>(0b0),
        static_cast<uint64_t>(0b1001000000010110100000001001000000011111100000011111100000011111),
        static_cast<uint64_t>(0b1000000011110000000111111000000111111000001111111100000000000000)
    };

    SDL_Color color_light_square = {
        .r = 181,
        .g = 184,
        .b = 156,
        .a = 255
    };

    SDL_Color color_dark_square = {
        .r = 41,
        .g = 57,
        .b = 105,
        .a = 255
    };

    SDL_Color color_light_piece = {
        .r = 52,
        .g = 235,
        .b = 186,
        .a = 255
    };

    SDL_Color color_dark_piece = {
        .r = 122,
        .g = 52,
        .b = 235,
        .a = 255
    };

    SDL_Color color_selected = {
        .r = 240,
        .g = 10,
        .b = 10,
        .a = 150
    };

    SDL_Color color_legal_move = {
        .r = 240,
        .g = 100,
        .b = 10,
        .a = 150
    };

    void reset (Game& game);

    bool check_move_legality (Game& game, int src, int dest);

    void move (Game& game, int src, int dest);

    int auto_move (Game& game);
}
