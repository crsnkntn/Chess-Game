#include "Header.h"

void print64 (uint64_t n) {
    uint64_t iter = static_cast<uint64_t>(1);
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0)
            std::cout << std::endl;
        if (n & iter)
            std::cout << "1";
        else
            std::cout << "0";
        iter = iter << 1;
    }
    std::cout << std::endl;
}
