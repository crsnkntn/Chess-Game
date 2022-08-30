#include "Game.h"

template <typename S, typename C>
void Game<S, C>::play () {
    bool turn = true;
    while (logicObject.check_end_condition()) {
        display();
        C move = turn ? p1.get_move(currentState) : p2.get_move(currentState);
        if (!logicObject.is_legal_change(move))
            continue;
        currentState = make_state_change(move);
        logicObject.changeState(currentState);
        turn = !turn;
    }
    display_win_message(logicObject.get_finished_game_object());
}

template <typename S, typename C>
void LogicObject<S, C>::changeState (S* newState) {
    currentState = newState;
}

template <typename S, typename C>
int LogicObject<S, C>::get_finished_game_object () {
    return finished_game_object;
}

template <typename S, typename C>
Game<S, C>::Game (Player<S, C> one, Player<S, C> two) : p1(one), p2(two) {}

template <typename S, typename C>
void Game<S, C>::play () {
    bool turn = true;
    while (logicObject.check_end_condition()) {
        display();
        C move = turn ? p1.get_move(currentState) : p2.get_move(currentState);
        if (!logicObject.is_legal_change(move))
            continue;
        currentState = make_state_change(move);
        logicObject.changeState(currentState);
        turn = !turn;
    }
    display_win_message(logicObject.get_finished_game_object());
}

template <typename S, typename C>
S* Game<S, C>::get_state_pointer () {
    return currentState;
}
