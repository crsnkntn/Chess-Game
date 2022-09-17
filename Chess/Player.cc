#include "Player.h"

Chess::Player::Player (bool h) : is_human(h), currentSelection(Action(-1, -1)) {
    net = h ? nullptr : new ExpansionDNN();
}

bool Chess::Player::is_current_change_ready () {
    return currentSelection.src != -1 && currentSelection.dest != -1;
}

void Chess::Player::set_current_change (Action a) {
    currentSelection = a;
}

void Chess::Player::set_dest (int n) {
    currentSelection.dest = n;
}

int Chess::Player::get_src () {
    return currentSelection.src;
}

Chess::Action Chess::Player::get_current_change () {
    return currentSelection;
}

void Chess::Player::generate_move (State* s, int turn) {
    net->process_input_token(s);

    net->use_net_with_current_input();

    output_token o = net->extract_output_token();

    Logic logic;
    uint64_t iter = static_cast<uint64_t>(1);
    for (int i = 0; i < 64; i++) {
        if (o.sources & iter) {
            currentSelection.src = i;
            break;
        }
        else
            iter = iter << 1;
    }

    iter = static_cast<uint64_t>(1);
    int dest = 0;
    for (int i = 0; i < 64; i++) {
        if (o.destinations & iter) {
            currentSelection.dest = i;
            if (logic.isLegalChange(s, currentSelection, turn))
                break;
            currentSelection.dest = -1;
        }
        else
            iter = iter << 1;
    }
}

void Chess::Player::generate_naive_move (State* s, int turn) {
    Logic logic;
    uint64_t iter1, iter2 = static_cast<uint64_t>(1);
    for (int i = 0; i < 64; i++) {
        if (s->p[turn] & iter1) {
            currentSelection.src = i;
            iter2 = static_cast<uint64_t>(1);
            for (int i = 0; i < 64; i++) {
                currentSelection.dest = i;
                if (logic.isLegalChange(s, currentSelection, turn))
                    break;
                currentSelection.dest = -1;
                iter2 = iter2 << 1;
            }
            if (currentSelection.dest == -1) {
                iter1 = iter1 << 1;
                currentSelection.src = -1;
                continue;
            }
            else
                break;
        }
        else
            iter1 = iter1 << 1;
    }
}

bool Chess::Player::is_human_player () {
    return is_human;
}
