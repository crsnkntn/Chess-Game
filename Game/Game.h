#pragma once

template <typename S, typename C>
class Player {
    public:
        Player () = default;

        C get_move (S* s);
};

template <typename S, typename C>
class LogicObject {
    private:
        S* currentState;

        int finished_game_object;

    public:
        LogicObject(S* s);

        bool isLegalChange (C c) = 0;

        bool check_end_condition () = 0;

        void changeState (S* newState);

        int get_finished_game_object ();
};

template <typename S, typename C>
class Game {
    private:
        S* currentState;

        Player<S, C> p1;
        Player<S, C> p2;

        LogicObject<S, C> logicObject;

        S* make_state_change (C c) = 0;

    public:
        Game (Player<S, C> one, Player<S, C> two);

        ~Game () = default;

        S* get_state_pointer ();

        virtual void display () = 0;

        void play ();
};
