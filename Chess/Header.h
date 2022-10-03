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
        struct bitmap_layer {
            uint16_t first;
            uint64_t second;
            uint64_t third;
        };

        struct bitmap {
            bitmap_layer layer1;
            bitmap_layer layer2;
        };

        bitmap empty = {
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0),
                static_cast<uint64_t>(0b0)
            },
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0),
                static_cast<uint64_t>(0b0)
            }
        };

        bitmap pawn = {
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0000000000000010000000000111000000000111000000000110000000000111),
                static_cast<uint64_t>(0b0000000001110000000001111000000001111000000011111100000000000000)
            },
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0000000000000100000000001000000000001000000000000110000000001000),
                static_cast<uint64_t>(0b0000000010000000000110000000000110000000001100000000000000000000)
            },
        };

        bitmap knight = {
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0101000000000101000000000111100000001111100000001111100000000011),
                static_cast<uint64_t>(0b1000000001111000000001111000000001111000000011111100000000000000)
            },
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0000000000000010000000001000000000010000000000010000000000000100),
                static_cast<uint64_t>(0b0000000010000000000110000000000110000000001100000000000000000000)
            }
        };

        bitmap bishop = {
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0000000000000010000000000111000000000111000000000011100000000011),
                static_cast<uint64_t>(0b1000000001111000000001110000000011111100000011111100000000000000)
            },
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0000000000000100000000001000000000000000000000010000000000110000),
                static_cast<uint64_t>(0b0000000110000000000010000000001100000000001100000000000000000000)
            }
        };

        bitmap rook = {
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0000000000000010100000000011100000000011100000000111000000000111),
                static_cast<uint64_t>(0b0000000001110000000001110000000001111000000001111100000000000000)
            },
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0000000000010100000000011100000000011100000000001000000000001000),
                static_cast<uint64_t>(0b0000000010000000000010000000000110000000001110000000000000000000)
            }
        };

        bitmap king = {
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0010000000000111000000000010000000000111000000001111110000011111),
                static_cast<uint64_t>(0b1100000111111100000111111000000011110000000011111100000000000000)
            },
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0100000000001000000000000100000000001000000000110000000000100000),
                static_cast<uint64_t>(0b0000001000000000000000000000000000000000001100000000000000000000)
            }
        };

        bitmap queen = {
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0001000000000001010000000011010000000111110000001111110000001111),
                static_cast<uint64_t>(0b1100000011111000000001110000000011111100000011111100000000000000)
            },
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b1000000000101000000000101100000000111000000000110000000000010000),
                static_cast<uint64_t>(0b0000000100000000000010000000001100000000001100000000000000000000)
            },
        };

        bitmap selection = {
            {
                static_cast<uint16_t>(0b1111111111111000),
                static_cast<uint64_t>(0b0000000110000000000110000000000110000000000110000000000110000000),
                static_cast<uint64_t>(0b0001100000000001100000000001100000000001100000000001111111111111)
            },
            {
                static_cast<uint16_t>(0b0),
                static_cast<uint64_t>(0b0),
                static_cast<uint64_t>(0b0)
            }
        };
    }

    namespace Color {
        SDL_Color light_square = {
            .r = 222,
            .g = 222,
            .b = 222,
            .a = 255
        };

        SDL_Color dark_square = {
            .r = 42,
            .g = 42,
            .b = 42,
            .a = 255
        };

        SDL_Color light_piece_layer1 = {
            .r = 52,
            .g = 235,
            .b = 186,
            .a = 255
        };

        SDL_Color light_piece_layer2 = {
            .r = 52,
            .g = 135,
            .b = 86,
            .a = 255
        };

        SDL_Color dark_piece_layer1 = {
            .r = 122,
            .g = 52,
            .b = 235,
            .a = 255
        };

        SDL_Color dark_piece_layer2 = {
            .r = 22,
            .g = 52,
            .b = 135,
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