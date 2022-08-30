#pragma once

#include "Player.h"

/*
    This abstraction makes sense because the logic for the game is separate from the 
    physical board. 

    There is the Game State (ex.Chess/ board abstraction, castling rights, en passant)

    There is a Game Logic (ex.Chess/ move legality)


*/


namespace Game {
    template <typename S, typename C>
    class LogicObject {
        private:
            S* currentState;

            int finished_game_object;

        public:
            LogicObject(S* s) = default;

            bool isLegalChange (C c) = 0;

            bool check_end_condition () = 0;

            void changeState (S* newState) {
                currentState = newState;
            }

            int get_finished_game_object () {
                return finished_game_object;
            }

    };

    /* Abstraction of a two-player turn based game.
        The template parameter S is the Game State
        The template parameter C is a Change in the Game State

        The Game class is an Abstract Class that has four required functions:

        - display
        - start
        - make_state_change
        - check_end_condition
    */ 
    template <typename S, typename C, typename Logic = LogicObject<S, C>>
    class Game {
        private:
            S* currentState;

            Player<S, C> p1;
            Player<S, C> p2;

            Logic logicObject;

            S* make_state_change (C c) = 0;

        public:
            Game (Player<S, C> one, Player<S, C> two) : p1(one), p2(two) {}

            ~Game () = default;

            void display ();

            void display_win_message (int win_message);

            void play () {
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

    };
}

namespace mcts {

    template <class S, class T>
    class MCTS_Node {

        public:

        MCTS_Node (S* s);

        ~MCTS_Node () = default;



    };

    template <class S, class C>
    class MC_Tree_Search {
        private:
            S* root = nullptr;
        public:
            MC_Tree_Search (S* s);

            ~MC_Tree_Search () = default;


    };
}