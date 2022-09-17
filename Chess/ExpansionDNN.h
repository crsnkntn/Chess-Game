#pragma once

#include <bitset>
#include <queue>
#include <dnn/dnn.h>

#include "State.h"

namespace Chess {
    int DNN_INPUT_SIZE = 531;
    int DNN_OUTPUT_SIZE = 128;
    int DNN_NUM_HIDDEN_LAYERS = 100;
    int DNN_HIDDEN_LAYER_SIZE = 100;

    struct output_token {
        uint64_t sources;
        uint64_t destinations;

        output_token ();
    };

    double sigmoid (double d);

    class ExpansionDNN : public DNN<State, output_token, sigmoid> {
        public:
            ExpansionDNN ();

            ExpansionDNN (std::ifstream& fin);
        private:
            struct pq_output_obj {
                int index;
                double value;

                pq_output_obj (int i, double v);
            };

            struct pq_comp {
                public:
                    bool operator() (pq_output_obj a, pq_output_obj b);
            };
        
        public:
            void process_input_token (State* input);

            output_token extract_output_token ();

            void fill_output_delta (output_token correct);
    };
}
