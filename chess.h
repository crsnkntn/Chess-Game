#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <bitset>

namespace Chess {
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

    bitmap selection_bitmap = {
        static_cast<uint16_t>(0b11111111111111000),
        static_cast<uint64_t>(0b0000000110000000000110000000000110000000000110000000000110000000),
        static_cast<uint64_t>(0b0001100000000001100000000001100000000001100000000001111111111111)
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
}
void print_int64 (uint64_t i);

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

int min (int a, int b);

struct ChessState {
    uint64_t p[8];
    char en_passant[2];
    char castling_privilege[2];

    uint64_t attack_space[2];
    bool updated;

    ChessState ();
};

void default_chess_state (ChessState& s);

uint64_t get_pawn_attack_space (int side, int src, uint64_t ally, uint64_t enemy);

uint64_t get_knight_attack_space (int src, uint64_t ally);

uint64_t get_bishop_attack_space (int src, uint64_t ally, uint64_t enemy);

uint64_t get_rook_attack_space (int src, uint64_t ally, uint64_t enemy);

uint64_t get_queen_attack_space (int src, uint64_t ally, uint64_t enemy);

uint64_t get_king_attack_space (int src, uint64_t ally, uint64_t enemy_attack_space, int side);

void update_attack_space (ChessState& s);

bool move_legality_check (ChessState& s, int src, int dest);

uint64_t get_side_attack_space (ChessState& s, int side);

bool king_in_check (ChessState& s, int side);

int evaluate (ChessState& s);

int tree_search (ChessState& s, int d);

void display_chess_state (ChessState& s, SDL_Window& window, SDL_Renderer& renderer);

void display_selection_state (int selected_square, SDL_Window& window, SDL_Renderer& renderer);

void display_overlay (SDL_Window& window, SDL_Renderer& renderer);
