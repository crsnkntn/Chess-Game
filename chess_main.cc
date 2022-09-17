#include "Chess/BoardView.h"
#include "Chess/Header.h"
#include "Chess/Player.h"
#include "Chess/Logic.h"
#include "Chess/BoardView.cc"
#include "Chess/Header.cc"
#include "Chess/Player.cc"
#include "Chess/Logic.cc"

#include <SDL2/SDL.h>

#include <vector>

int main (int argc, char** argv) {
    Chess::SDL_Board_View view(0, 0);

    Chess::State* board = new Chess::State();

    std::vector<Chess::Player> players;

    Chess::Player p1(true);
    Chess::Player p2(false);

    Chess::Logic logic;

    players.push_back(p1);
    players.push_back(p2);

    int turn = Chess::LIGHT;

    int x = 0;
    int y = 0;
    int click = 0;
    int selection = -1;

    bool playing = true;
    bool updated = true;
    while (playing) {
        if (updated) {
            view.display_board(board, selection);
            updated = false;
        }

        if (players[turn].is_human_player()) {
            SDL_Event e;

            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        playing = false;
                        break;
                    case SDL_KEYUP:
                        switch (e.key.keysym.sym) {
                            case SDLK_r:
                                delete board;
                                board = new Chess::State();
                                turn = Chess::LIGHT;
                                updated = true;
                                players[Chess::LIGHT].set_current_change(Chess::Action(-1, -1));
                                players[Chess::DARK].set_current_change(Chess::Action(-1, -1));
                                break;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        SDL_GetMouseState(&x, &y);
                        players[turn].process_click(int((y / (12 * SIZE)) * 8) + int(x / (12 * SIZE)), board->p[turn]);
                        selection = players[turn].get_src();
                        if (players[turn].is_current_change_ready()) {
                            if (logic.isLegalChange(board, players[turn].get_current_change(), turn)) {
                                players[turn].get_current_change().execute(*board);
                                players[turn].set_current_change(Chess::Action(-1, -1));
                                turn = (turn + 1) % 2;
                                selection = -1;
                            }
                            else
                                players[turn].set_dest(-1);
                        }
                        updated = true;
                        break;
                }
            }
        }
        else {
            std::cout << "Generating move..." << std::endl;
            SDL_Delay(1000);
            players[turn].generate_move(board, turn);
            if (!players[turn].is_current_change_ready()) {
                players[turn].generate_naive_move(board, turn);
                std::cout << "Generated NAIVE move: " << players[turn].get_current_change().src 
                    << players[turn].get_current_change().dest << std::endl;
            }

            players[turn].get_current_change().execute(*board);
            players[turn].set_current_change(Chess::Action(-1, -1));
            updated = true;
            turn = (turn + 1) % 2;
        }
    }
}
