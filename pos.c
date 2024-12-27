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

Displacement pos_displacement(Pos from, Pos to) {
    Displacement d;
    d.r = to.r - from.r;
    d.c = to.c - from.c;
    return d;
}

Direction displacement_direction(Displacement d) {
    if ((abs(d.r) == 1 && abs(d.c) == 2) || (abs(d.r) == 2 && abs(d.c) == 1)) {
        Direction dir = {d.r, d.c}; //L
        return dir;
    } else if (d.r == 0 || d.c == 0) {
        if (d.c > 0) {
            Direction dir = {0, 1}; //Rank to the right
            return dir;
        } else if (d.c < 0){
            Direction dir = {0, -1}; //Rank to the left 
            return dir;
        } else if (d.r > 0){
            Direction dir = {1, 0}; //File down
            return dir;
        } else if (d.r < 0){
            Direction dir = {-1, 0}; //File up
            return dir;
        }
    } else if (abs(d.r) == abs(d.c)) {
        if (d.r < 0 && d.c < 0) {
            Direction dir = {-1, -1}; //Diagonal top left
            return dir;
        } else if (d.r < 0 && d.c > 0) {
            Direction dir = {-1, 1}; //Diagonal top right 
            return dir;
        } else if (d.r > 0 && d.c < 0) {
            Direction dir = {1, -1}; //Diagonal bottom left
            return dir;
        } else if (d.r > 0 && d.c > 0) {
            Direction dir = {1, 1}; //Diagonal bottom right
            return dir;
        }
    }
    fprintf(stderr, "Unable to retrieve direction from displacement\n");
    exit(1);
}

bool direction_cmp(Direction d1, Direction d2) {
    return d1.r == d2.r && d1.c == d2.c;
}

void tdd_show(char r, char c) {
    printf("(%d, %d)\n", r, c);
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

bool square_cmp(Square s1, Square s2) {
    return s1.file == s2.file && s1.rank == s2.rank;
}

