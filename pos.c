#include "pos.h"

Pos make_pos(unsigned char r, unsigned char c) {
    Pos pos;
    pos.r = r;
    pos.c = c;
    return pos;
}

void malloc_check(void* p) {
    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

