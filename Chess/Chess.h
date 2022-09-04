#pragma once
#include "../include/mini-mcts/mcts.h"
#include <SDL2/SDL.h>

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdint.h>
#include <vector>
#include <bitset>

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
    
    class State : public mcts::State {
        public:
            char en_passant[2];
            char castling_privilege[2];
            uint64_t p[8];
            uint64_t movespace[2];

            State () {
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
    };

    class Action : public mcts::Action<State> {
        public:
            int src;
            int dest;

            Action () : src(-1), dest(-1) {}

            Action (int s, int d) : src(s), dest(d) {}

            void execute(State& state) {
                int64_t one = static_cast<int64_t>(1);
                int64_t src_square = one << src;
                int64_t dest_square = one << dest;

                if (state.p[LIGHT] & src_square) {
                    state.p[LIGHT] -= src_square;
                    state.p[LIGHT] = state.p[LIGHT] | dest_square;
                    if (state.p[DARK] & dest_square)
                        state.p[DARK] -= dest_square;
                }
                else if (state.p[DARK] & src_square) {
                    state.p[DARK] -= src_square;
                    state.p[DARK] = state.p[DARK] | dest_square;
                    if (state.p[LIGHT] & dest_square)
                        state.p[LIGHT] -= dest_square;
                }

                if (state.p[PAWN] & dest_square)
                    state.p[PAWN] -= dest_square;
                else if (state.p[KNIGHT] & dest_square)
                    state.p[KNIGHT] -= dest_square;
                else if (state.p[BISHOP] & dest_square) 
                    state.p[BISHOP] -= dest_square;
                else if (state.p[ROOK] & dest_square)
                    state.p[ROOK] -= dest_square;
                else if (state.p[QUEEN] & dest_square)
                    state.p[QUEEN] -= dest_square;
                else if (state.p[KING] & dest_square)
                    state.p[KING] -= dest_square;

                if (state.p[PAWN] & src_square) {
                    state.p[PAWN] -= src_square;
                    state.p[PAWN] += dest_square;
                }
                else if (state.p[KNIGHT] & src_square) {
                    state.p[KNIGHT] -= src_square;
                    state.p[KNIGHT] += dest_square;
                }
                else if (state.p[BISHOP] & src_square) {
                    state.p[BISHOP] -= src_square;
                    state.p[BISHOP] += dest_square;
                }
                else if (state.p[ROOK] & src_square) {
                    state.p[ROOK] -= src_square;
                    state.p[ROOK] += dest_square;
                }
                else if (state.p[QUEEN] & src_square) {
                    state.p[QUEEN] -= src_square;
                    state.p[QUEEN] += dest_square;
                }
                else if (state.p[KING] & src_square) {
                    state.p[KING] -= src_square;
                    state.p[KING] += dest_square;
                }
            }

            int get_src () {
                return src;
            }

            int get_dest () {
                return dest;
            }
    };

    class ExpansionStrategy : public mcts::ExpansionStrategy<State, Action> {
        uint64_t generatedSources [2];
        uint64_t generatedDestinations [2];

        // Utility uint64_t ... 
        
        Action generateNext() {
            return Action(-1, -1);
        }

        bool canGenerateNext () const {
            return true;
        }
    };

    class PlayoutStrategy : public mcts::PlayoutStrategy<State, Action> {

        
        void generateRandom (Action& a) {
            
        }
    };

    class Backpropagation : public mcts::Backpropagation<State> {
        float updateScore(const State& state, float backpropScore) {
            return 1;
        }
    };

    class TerminationCheck : public mcts::TerminationCheck<State> {
        bool isTerminal (const State& state) {
            return true;
        }
    };

    class Scoring : public mcts::Scoring<State> {
        float score(const State& state) {
            return 1;
        }
    };

    class Node : public mcts::Node<State, Action, ExpansionStrategy> {

    };

    class MCTS : public mcts::MCTS<State, Action, ExpansionStrategy, PlayoutStrategy> {

    };

    class Player {
        private:
            bool is_human;
            Action currentSelection;

        public:
            Player (bool h);

            ~Player () = default;

            void process_click (int click);

            bool is_current_change_ready () {
                return currentSelection.src != -1 && currentSelection.dest != -1;
            }

            Action get_current_change () {
                return currentSelection;
            }

            Action get_generated_move (MCTS& mcts, State* s, int turn) {
                return Action();//mcts.calculateAction();
            }

            bool is_human_player ();
    };

    class Logic {
        public:
            bool isLegalChange (State* s, Action c);

            bool check_end_condition ();

        private:
            uint64_t movespace (State* board, int src);

            void move_piece (State* board, int src, int dest);

            bool is_king_in_check (State* board, int side);

            uint64_t side_movespace (State* board, int side);

            uint64_t pawn_movespace (int src, uint64_t ally, uint64_t enemy, int side);

            uint64_t knight_movespace (int src, uint64_t ally);

            uint64_t bishop_movespace (int src, uint64_t ally, uint64_t enemy);

            uint64_t rook_movespace (int src, uint64_t ally, uint64_t enemy);

            uint64_t queen_movespace (int src, uint64_t ally, uint64_t enemy);

            uint64_t king_movespace (int src, uint64_t ally, uint64_t enemy_movespace, int side);
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
    }

    class SDL_Board_View {
        private:
            int offsetx = 0;
            int offsety = 0;

            SDL_Window* window;
            SDL_Renderer* renderer;

            void display_piece (int sq_color, int pc_color, int piece_type, int loc) {
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

        public:
            SDL_Board_View (int x, int y) : offsetx(x), offsety(y) {
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

            ~SDL_Board_View () {
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                SDL_Quit();
            }

            // Disable copy ctor, etc.
            void display_board (State* state) {
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

                    if (i % 8 != 7)
                        sq_color = (sq_color + 1) % 2;
                    
                    iter = iter << 1;
                }

                SDL_RenderPresent(renderer);
            }
    };
}
