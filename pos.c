#include "pos.h"

Pos pos_make(unsigned char r, unsigned char c) {
    Pos pos;
    pos.r = r;
    pos.c = c;
    return pos;
}

Square pos_convert(Pos pos, board_direction direction) {
    bounds_check(pos.r, pos.c);
    if (direction == WHITE_MOVING_UP) {
        return square_make('a' + pos.c, board_size - pos.r);
    } else {
        return square_make('a' + board_size - 1 - pos.c, pos.r + 1);
    }
}

bool pos_cmp(Pos pos1, Pos pos2) {
    return pos1.r == pos2.r && pos1.c == pos2.c;
}

Square square_make(char file, unsigned char rank) {
    Square square;
    square.file = file;
    square.rank = rank;
    return square;
}

Pos square_convert(Square square, board_direction direction) {
    Pos pos;
    if (direction == WHITE_MOVING_UP) {
        pos = pos_make(board_size - square.rank, square.file - 'a');
    } else {
        pos = pos_make(square.rank - 1, 'a' + board_size - 1 - square.file);
    }
    bounds_check(pos.r, pos.c);
    return pos;
}

void square_show(Square square) {
    printf("%c%hhu", square.file, square.rank);
}

