#pragma once

#include <iostream>
#include <math.h>
#include <random>
#include <queue>

#include "Chess.h"

namespace Chess {
    struct DNN_int64t_output {
        uint64_t sources;
        uint64_t destinations;

        DNN_int64t_output () : sources(static_cast<uint64_t>(0)), destinations(static_cast<uint64_t>(0)) {}
    };

    class DNN_int64t {
        private:
            // Sizes, needed for iteration bounds
            const int i_layer_sz = 672; // en passant and castling can be covered by one bit for each side
            const int h_layer_sz = 1000;
            const int o_layer_sz = 128;
            const int n_hidden_layers = 5;

            // Layers
            double* input_layer;
            double* output_layer;
            double** hidden_layers;

            // Weights
            double*** weights;

            // Utility/Temporary
            double* delta_output_layer;
            double** delta_hidden_layers;
            double*** delta_weights;

            // Constants
            double A, B;

            struct pq_output_obj {
                int index;
                double value;

                pq_output_obj (int i, double v) : index(i), value(v) {}
            };

            struct pq_comp {
                public:
                    bool operator() (pq_output_obj a, pq_output_obj b) {
                        return a.value <  b.value;
                    }
            };

        public:
            DNN_int64t () {
                input_layer = (double*)malloc(i_layer_sz * sizeof(double));
                output_layer = (double*)malloc(o_layer_sz * sizeof(double));
                delta_output_layer = (double*)malloc(o_layer_sz * sizeof(double));

                hidden_layers = (double**)malloc(n_hidden_layers * sizeof(double*));
                delta_hidden_layers = (double**)malloc(n_hidden_layers * sizeof(double*));
                for (int j = 0; j < n_hidden_layers; j++) {
                    hidden_layers[j] = (double*)malloc(h_layer_sz * sizeof(double));
                    delta_hidden_layers[j] = (double*)malloc(h_layer_sz * sizeof(double));
                }

                weights = (double***)malloc((n_hidden_layers + 1) * sizeof(double**));
                delta_weights = (double***)malloc((n_hidden_layers + 1) * sizeof(double**));

                weights[0] = (double**)malloc(i_layer_sz * sizeof(double*));
                delta_weights[0] = (double**)malloc(i_layer_sz * sizeof(double*));
                for (int j = 0; j < i_layer_sz; j++) {
                    weights[0][j] = (double*)malloc(h_layer_sz * sizeof(double));
                    delta_weights[0][j] = (double*)malloc(h_layer_sz * sizeof(double));
                }

                for (int j = 1; j < n_hidden_layers; j++) {
                    weights[j] = (double**)malloc(h_layer_sz * sizeof(double*));
                    delta_weights[j] = (double**)malloc(h_layer_sz * sizeof(double*));
                    for (int k = 0; k < h_layer_sz; k++) {
                        weights[j][k] = (double*)malloc(h_layer_sz * sizeof(double));
                        delta_weights[j][k] = (double*)malloc(h_layer_sz * sizeof(double));
                    }
                }

                weights[n_hidden_layers] = (double**)malloc(h_layer_sz * sizeof(double*));
                delta_weights[n_hidden_layers] = (double**)malloc(h_layer_sz * sizeof(double*));
                for (int j = 0; j < h_layer_sz; j++) {
                    weights[n_hidden_layers][j] = (double*)malloc(o_layer_sz * sizeof(double));
                    delta_weights[n_hidden_layers][j] = (double*)malloc(o_layer_sz * sizeof(double));
                }

                A = 0.1;
                B = 0.1;
                randomize_weights(2333);
            }

            DNN_int64t (const DNN_int64t& other) {
                memcpy(&input_layer, &other.input_layer, sizeof(other.input_layer));
                memcpy(&output_layer, &other.output_layer, sizeof(output_layer));
                memcpy(&hidden_layers, &other.hidden_layers, sizeof(hidden_layers));
                memcpy(&weights, &other.weights, sizeof(weights));
                memcpy(&delta_output_layer, &other.delta_output_layer, sizeof(delta_output_layer));
                memcpy(&delta_hidden_layers, &other.delta_hidden_layers, sizeof(delta_hidden_layers));
            }

            ~DNN_int64t () {
                free(input_layer);
                free(output_layer);
                free(hidden_layers);

                // Weights
                free(weights);

                // Utility/Temporary
                free(delta_output_layer);
                free(delta_hidden_layers);
                free(delta_weights);
            }

            void randomize_weights (int seed) {
                srand(seed);

                for (int i = 0; i < i_layer_sz; i++) {
                    for (int j = 0; j < h_layer_sz; j++) {
                        weights[0][i][j] = rand() / (double)(RAND_MAX);
                        delta_weights[0][i][j] = 0.0;
                    }
                }

                for (int i = 1; i < n_hidden_layers; i++) {
                    for (int j = 0; j < h_layer_sz; j++) {
                        for (int k = 0; k < h_layer_sz; k++) {
                            weights[i][j][k] = rand() / (double)(RAND_MAX);
                            delta_weights[i][j][k] = 0.0;
                        }
                    }
                }

                for (int j = 0; j < h_layer_sz; j++) {
                    for (int k = 0; k < o_layer_sz; k++) {
                        weights[n_hidden_layers][j][k] = rand() / (double)(RAND_MAX);
                        delta_weights[n_hidden_layers][j][k] = 0.0;
                    }
                }

                for (int i = 0; i < n_hidden_layers; i++) {
                    for (int j = 0; j < h_layer_sz; j++) {
                        delta_hidden_layers[i][j] = rand() / (double)(RAND_MAX);
                    }
                }

                for (int i = 0; i < o_layer_sz; i++) {
                    delta_output_layer[i] = 0.0;
                }
            }

            void load_input_layer (const Chess::State* input, int turn) {
                uint64_t one = static_cast<uint64_t>(1);
                uint64_t iter = one;

                for (int i = 0; i < 8; i++) {
                    iter = one;
                    for (int j = 0; j < 64; j++) {
                        input_layer[(i * 64) + j] = (input->p[i] & one) ? 1.0 : 0.0;
                        iter = iter << 1;
                    }
                }

                input_layer[512] = (turn == LIGHT) ? 1.0 : 0.0;

                for (int i = 0; i < 2; i++) {
                    iter = one;
                    for (int j = 0; j < 8; j++) {
                        input_layer[513 + (i * 8) + j] = (input->en_passant[i] & iter) ? 1.0 : 0.0;
                        iter = iter << 1;
                    }
                }

                input_layer[528] = (input->castling_privilege[0] & char(0b1111)) ? 1.0 : 0.0;
                input_layer[529] = (input->castling_privilege[0] & char(0b11110000)) ? 1.0 : 0.0;
                input_layer[530] = (input->castling_privilege[1] & char(0b1111)) ? 1.0 : 0.0;
                input_layer[531] = (input->castling_privilege[1] & char(0b11110000)) ? 1.0 : 0.0;
            }

            void train_with_input (Chess::State* state, DNN_int64t_output correct, int turn) {
                load_input_layer(state, turn);

                do_feed_forward();

                do_back_propogation(correct);

                do_stochastic_gradient_descent();
            }

            DNN_int64t_output use_net (Chess::State* state, int turn) {
                load_input_layer(state, turn);

                do_feed_forward();

                return get_token_from_output_layer ();
            }

        private:
            double sigmoid (double sigma) {
                return 1.0 / (1.0 + exp(-sigma));
            }

            void do_feed_forward () {
                double sum = 0.0;
                // Feed Forward
                for (int i = 0; i < n_hidden_layers + 1; i++) {
                    sum = 0.0;
                    if (i == 0) {
                        for (int j = 0; j < h_layer_sz; j++) {
                            sum = 0.0;
                            for (int k = 0; k < i_layer_sz; k++) {
                                sum += input_layer[k] * weights[0][k][j];
                            }
                            hidden_layers[0][j] = sigmoid(sum);
                        }
                    }
                    else if (i == n_hidden_layers) {
                        for (int j = 0; j < o_layer_sz; j++) {
                            sum = 0.0;
                            for (int k = 0; k < h_layer_sz; k++) {
                                sum += hidden_layers[i - 1][k] * weights[i][k][j];
                            }
                            output_layer[j] = sigmoid(sum);
                        }
                    }
                    else {
                        for (int j = 0; j < h_layer_sz; j++) {
                            sum = 0.0;
                            for (int k = 0; k < h_layer_sz; k++) {
                                sum += hidden_layers[i - 1][k] * weights[i][j][k];
                            }
                            hidden_layers[i][j] = sigmoid(sum);
                        }
                    }
                }
            }

            void do_back_propogation (DNN_int64t_output correct) {
                double errorTemp = 0.0, errorSum = 0.0;
                
                uint64_t one = static_cast<uint64_t>(1);
                uint64_t iter = one;

                for (int i = 0; i < o_layer_sz; i++) {
                    if (i < (o_layer_sz / 2) && (correct.sources & iter))
                        errorTemp = 1 - output_layer[i];
                    else if (correct.sources & iter)
                        errorTemp = 1 - output_layer[i];
                    else
                        errorTemp = -output_layer[i];

                    delta_output_layer[i] = -errorTemp * sigmoid(output_layer[i]) * (1 - sigmoid(output_layer[i]));
                    errorSum += errorTemp * errorTemp;
                    iter = iter << 1;
                }

                for (int i = n_hidden_layers; i > 0; i--) {
                    if (i == n_hidden_layers) {
                        for (int j = 0; j < h_layer_sz; j++) {
                            errorTemp = 0.0;
                            for (int k = 0; k < o_layer_sz; k++) {
                                errorTemp += delta_output_layer[k] * weights[i][j][k];
                            }
                            delta_hidden_layers[i - 1][j] = errorTemp * 
                                (1.0 + hidden_layers[i - 1][j]) * (1.0 - hidden_layers[i - 1][j]);
                        }
                    }
                    else {
                        for (int j = 0; j < h_layer_sz; j++) {
                            errorTemp = 0.0;
                            for (int k = 0; k < h_layer_sz; k++) {
                                errorTemp += delta_hidden_layers[i][k] * weights[i][j][k];
                            }
                            delta_hidden_layers[i - 1][j] = errorTemp * 
                                (1.0 + hidden_layers[i - 1][j]) * (1.0 - hidden_layers[i - 1][j]);
                        }
                    }
                }
            }

            void do_stochastic_gradient_descent () {
                for (int i = n_hidden_layers; i > 0; i--) {
                    if (i == n_hidden_layers) {
                        for (int j = 0; j < h_layer_sz; j++) {
                            for (int k = 0; k < o_layer_sz; k++) {
                                delta_weights[i][j][k] = 
                                    A * delta_weights[i][j][k] + 
                                    B * delta_output_layer[k] * hidden_layers[i - 1][j]; // index problem?
                                weights[i][j][k] -= delta_weights[i][j][k]; // order of brackets?
                            }
                        }
                    }
                    else {
                        for (int j = 0; j < h_layer_sz; j++) {
                            for (int k = 0; k < h_layer_sz; k++) {
                                delta_weights[i][j][k] = 
                                    A * delta_weights[i][j][k] + 
                                    B * delta_hidden_layers[i - 1][k] * hidden_layers[i - 1][j]; // index problem?
                                weights[i][j][k] -= delta_weights[i][j][k]; // order of brackets?
                            }
                        }
                    }
                }
            }
        
            DNN_int64t_output get_token_from_output_layer () {
                std::priority_queue<pq_output_obj, std::vector<pq_output_obj>, pq_comp> pq_src;
                std::priority_queue<pq_output_obj, std::vector<pq_output_obj>, pq_comp> pq_dest;

                for (int i = 0; i < o_layer_sz; i++) {
                    if (i < o_layer_sz / 2)
                        pq_src.push(pq_output_obj(i, output_layer[i]));
                    else
                        pq_dest.push(pq_output_obj(i, output_layer[i]));
                }

                DNN_int64t_output output;
                uint64_t one = static_cast<uint64_t>(1);

                for (int i = 0; i < 10; i++) {
                    pq_output_obj a = pq_src.top();
                    pq_output_obj b = pq_dest.top();
                    pq_src.pop();
                    pq_dest.pop();

                    output.sources += (one << a.index);
                    output.destinations += (one << b.index);
                }

                return output;
            }
    };
}
