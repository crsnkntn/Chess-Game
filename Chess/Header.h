#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdint.h>
#include <vector>
#include <bitset>
#include <queue>

#include <SDL2/SDL.h>

void print64 (uint64_t n);

namespace Chess {
    enum Pieces {
        LIGHT,
        DARK,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
        NONE
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

        SDL_Color selection = {
            .r = 0,
            .g = 0,
            .b = 10,
            .a = 255
        };
    }

}