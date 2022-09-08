#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>

#include "Chess.h"
#include "Chess.cc"

struct training_object {
    State* state;
    output_token correct;
};

training_object get_input (std::ifstream& fin) {
    training_object object;

    object.state = new State();

    fin >> object.state->p[LIGHT];
    fin >> object.state->p[DARK];
    fin >> object.state->p[PAWN];
    fin >> object.state->p[KNIGHT];
    fin >> object.state->p[BISHOP];
    fin >> object.state->p[ROOK];
    fin >> object.state->p[QUEEN];
    fin >> object.state->p[KING];


    fin >> object.state->castling_privilege[LIGHT];
    fin >> object.state->castling_privilege[DARK];


    fin >> object.state->en_passant[LIGHT];
    fin >> object.state->en_passant[DARK];

    fin >> object.state->movespace[LIGHT];
    fin >> object.state->movespace[DARK];

    fin >> object.correct.sources;
    fin >> object.correct.destinations;
}

int main (int argc, char** argv) {
    if (argc > 2) {
        std::cerr << "Format is: ./{exe} {output_file}" << std::endl;
        exit(1);
    }

    // Create the net
    Chess::dnn net;

    // Fill the Training Object Vector
    using directory_iter = std::filesystem::recursive_directory_iterator;
    std::string training_file_path = "training_data";

    std::vector<training_object> training_sets;

    for (const auto& dirEntry : directory_iter(training_file_path)) {
        std::ifstream fin(dirEntry);

        training_sets.push_back(get_input(fin));

        fin.close();
    }

    // Train on the set
    for (training_object t : training_sets) {
        net.process_input_token(t.state);

        net.train_with_current_input(t.correct);
    }


    // Send to an output file
    std::string net_output_file = argv[1];

    net.save_current_dnn(net_output_file);
}
