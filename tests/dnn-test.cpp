#include "../Chess/Chess.h"
#include "../Chess/dnn-int64t.h"

void print_int64 (uint64_t n) {
    uint64_t iter = static_cast<uint64_t>(1);

    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0)
            std::cout << std::endl;

        if (n & iter)
            std::cout << "X ";
        else
            std::cout << "- ";
        
        iter = iter << 1;
    }
    std::cout << std::endl;
}
int main (int argc, char** argv) {
    int turn = Chess::LIGHT;

    Chess::State* default_state = new Chess::State;

    Chess::DNN_int64t dnn;

    Chess::DNN_int64t_output o = dnn.use_net(default_state, turn);

    print_int64(o.sources);
    print_int64(o.destinations);
}