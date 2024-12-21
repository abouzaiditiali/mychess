#include "pos.h"

Pos pos_make(unsigned char r, unsigned char c) {
    Pos pos;
    pos.r = r;
    pos.c = c;
    return pos;
}

Square square_make(char c, unsigned char r) {
    Square square;
    square.r = r;
    square.c = c;
    return square;
}

Square pos_convert(Pos pos, board_direction direction) {
    bounds_check(pos);
    if (direction == WHITE_MOVING_UP) {
        return square_make('a' + pos.c, board_size - pos.r);
    } else {
        return square_make('a' + board_size - 1 - pos.c, pos.r + 1);
    }
}

Pos square_convert(Square square, board_direction direction) {
    Pos pos;
    if (direction == WHITE_MOVING_UP) {
        pos = pos_make(board_size - square.r, square.c - 'a');
    } else {
        pos = pos_make(square.r - 1, 'a' + board_size - 1 - square.c);
    }
    bounds_check(pos);
    return pos;
}

void square_show(Square square) {
    printf("%c%hhu", square.c, square.r);
}

void bounds_check(Pos pos) {
    if (pos.r < 0 || pos.r >= board_size || pos.c < 0 || pos.c >= board_size) {
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

