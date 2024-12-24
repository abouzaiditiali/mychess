#include "pos.h"

Pos pos_make(unsigned char r, unsigned char c) {
    Pos pos = {r, c};
    return pos;
}

Pos pos_flip(Pos pos) {
    return pos_make(board_size - 1 - pos.r, board_size - 1 - pos.c);
}

Square pos_convert(Pos pos, player_perspective perspective) {
    Pos npos = pos;
    if (perspective == BLACKS_PERSPECTIVE) {
        npos = pos_flip(pos);
    }
    return square_make('a' + npos.c, board_size - npos.r);
    //return square_make('a' + board_size - 1 - pos.c, pos.r + 1);
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

//returns pos in white's perspective
Pos square_convert(Square square) {
    Pos pos = pos_make(board_size - square.rank, square.file - 'a');
    return pos;
    //pos = pos_make(square.rank - 1, 'a' + board_size - 1 - square.file);
}

void square_show(Square square) {
    printf("%c%hhu", square.file, square.rank);
}

