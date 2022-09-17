#pragma once

#include <vector>

#include "../include/mini-mcts/mcts.h"
#include "State.h"
#include "State.cc"
#include "Action.h"
#include "Action.cc"
#include "ExpansionDNN.h"
#include "ExpansionDNN.cc"

namespace Chess {
    class ExpansionStrategy : public mcts::ExpansionStrategy<State, Action> {
        std::vector<Action> generatedActions;
        bool ungenerated = true;
        
        Action generateNext() {
            if (ungenerated) {
                uint64_t generatedSources = static_cast<uint64_t>(0);
                uint64_t generatedDestinations = static_cast<uint64_t>(0);
                output_token o;

                ungenerated = false;
            }

            Action a = generatedActions[generatedActions.size() - 1];
            generatedActions.resize(generatedActions.size() - 1);
            return a;
        }

        bool canGenerateNext () const {
            return !generatedActions.empty();
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
        private:
        ExpansionDNN net;

        public:
        MCTS (const State& state, Backpropagation* backprop, TerminationCheck* termcheck, Scoring* scoring) 
            : mcts::MCTS<State, Action, ExpansionStrategy, PlayoutStrategy>(state, backprop, termcheck, scoring)
            {}

        void init_net (std::ifstream& fin) {
            net.overwrite_net_from_file(fin);
        }
    };

}