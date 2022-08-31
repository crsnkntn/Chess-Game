#include "Chess/Chess.h"
#include "Chess/Chess.cc"
#include <SDL2/SDL.h>

int main (int argc, char** argv) {
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

    Chess::Player p1(true), p2(true);

    Chess::Game_SDL chess_game(p1, p2);
    
    chess_game.init_sdl(window, renderer);

    chess_game.play();

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}
