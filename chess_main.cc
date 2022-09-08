#include "Chess/Chess.h"
#include "Chess/Chess.cc"
#include <SDL2/SDL.h>

#include <vector>

int main (int argc, char** argv) {
    Chess::SDL_Board_View view(0, 0);

    Chess::State* board = new Chess::State();

    std::vector<Player> players;

    Chess::Player p1(true);
    Chess::Player p2(true);

    Chess::Logic logic;

    Chess::Backpropagation backprop;
    Chess::TerminationCheck termclock;
    Chess::Scoring scoring;

    const Chess::State& boardref = Chess::State();

    players.push_back(p1);
    players.push_back(p2);

    int turn = LIGHT;

    int x = 0;
    int y = 0;
    int click = 0;

    bool playing = true;
    bool updated = true;
    while (playing) {
        if (updated) {
            view.display_board(board);
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
                                turn = DARK;
                                updated = true;
                                break;
                        }
                    case SDL_MOUSEBUTTONUP:
                        SDL_GetMouseState(&x, &y);
                        players[turn].process_click(int((y / (12 * SIZE)) * 8) + int(x / (12 * SIZE)));
                        if (players[turn].is_current_change_ready()) {
                            if (logic.isLegalChange(board, players[turn].get_current_change())) {
                                players[turn].get_current_change().execute(*board);
                                updated = true;
                            }
                            players[turn].set_current_change(Action(-1, -1));
                        }
                }
            }
        }
        else {
            //action = players[turn].get_generated_move(mcts, board, turn);
            players[turn].get_current_change().execute(*board);
            players[turn].set_current_change(Action(-1, -1));
            updated = true;
        }

        turn = (turn + 1) % 2;
    }
}
