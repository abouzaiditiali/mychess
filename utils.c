#include "utils.h"

void bounds_check(unsigned char r, unsigned char c) {
    if (r < 0 || r >= board_size || c < 0 || c >= board_size) {
        fprintf(stderr, "Position is out of bounds\n");
        exit(1);
    }
}

void malloc_check(void* p) {
    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void pointer_check(void* p) {
    if (p == NULL) {
        fprintf(stderr, "Function cannot accept null pointer\n");
        exit(1);
    }
}

