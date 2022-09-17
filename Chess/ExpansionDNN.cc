#include "ExpansionDNN.h"

Chess::output_token::output_token () : sources(static_cast<uint64_t>(0)), destinations(static_cast<uint64_t>(0)) {}

double Chess::sigmoid (double d) {
    return d;
}

Chess::ExpansionDNN::ExpansionDNN () : DNN<State, output_token, sigmoid>(Chess::DNN_INPUT_SIZE, Chess::DNN_OUTPUT_SIZE, Chess::DNN_HIDDEN_LAYER_SIZE, Chess::DNN_NUM_HIDDEN_LAYERS, 23) {}

Chess::ExpansionDNN::ExpansionDNN (std::ifstream& fin) : DNN<State, output_token, sigmoid>(fin) {}

Chess::ExpansionDNN::pq_output_obj::pq_output_obj (int i, double v) : index(i), value(v) {}

bool Chess::ExpansionDNN::pq_comp::operator() (pq_output_obj a, pq_output_obj b) {
    return a.value <  b.value;
}

void Chess::ExpansionDNN::process_input_token (State* input) {
    uint64_t one = static_cast<uint64_t>(1);
    uint64_t iter = one;

    for (int i = 0; i < 8; i++) {
        iter = one;
        for (int j = 0; j < 64; j++) {
            input_layer[(i * 64) + j] = (input->p[i] & one) ? 1.0 : 0.0;
            iter = iter << 1;
        }
    }

    for (int i = 0; i < 2; i++) {
        iter = one;
        for (int j = 0; j < 8; j++) {
            input_layer[512 + (i * 8) + j] = (input->en_passant[i] & iter) ? 1.0 : 0.0;
            iter = iter << 1;
        }
    }

    input_layer[527] = (input->castling_privilege[0] & char(0b1111)) ? 1.0 : 0.0;
    input_layer[528] = (input->castling_privilege[0] & char(0b11110000)) ? 1.0 : 0.0;
    input_layer[529] = (input->castling_privilege[1] & char(0b1111)) ? 1.0 : 0.0;
    input_layer[530] = (input->castling_privilege[1] & char(0b11110000)) ? 1.0 : 0.0;
}

Chess::output_token Chess::ExpansionDNN::extract_output_token () {
    std::priority_queue<pq_output_obj, std::vector<pq_output_obj>, pq_comp> pq_src;
    std::priority_queue<pq_output_obj, std::vector<pq_output_obj>, pq_comp> pq_dest;

    for (int i = 0; i < o_layer_sz; i++) {
        if (i < o_layer_sz / 2)
            pq_src.push(pq_output_obj(i, output_layer[i]));
        else
            pq_dest.push(pq_output_obj(i, output_layer[i]));
    }

    output_token output;
    uint64_t one = static_cast<uint64_t>(1);

    for (int i = 0; i < 13; i++) {
        pq_output_obj a = pq_src.top();
        pq_output_obj b = pq_dest.top();
        pq_src.pop();
        pq_dest.pop();

        output.sources += (one << a.index);
        output.destinations += (one << b.index);
    }

    return output;
}

void Chess::ExpansionDNN::fill_output_delta (output_token correct) {
    double errorTemp = 0.0;
    for (int h = 0; h < 2; h++) {
        uint64_t iter = static_cast<uint64_t>(1);
        uint64_t mask = (h == 0) ? correct.sources : correct.destinations;
        for (int i = 0; i < 64; i++) {
            if (mask & iter)
                errorTemp = 1 - output_layer[i];
            else
                errorTemp = -output_layer[i];
            delta_output_layer[i] = -errorTemp * sigmoid(output_layer[i]) * (1 - sigmoid(output_layer[i]));
            errorSum += errorTemp * errorTemp;
            iter = iter << 1;
        }
    }
}
