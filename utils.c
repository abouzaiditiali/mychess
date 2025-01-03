#include "utils.h"

void bounds_check(char r, char c) {
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

bool out_of_bounds(char r, char c) {
    return (r < 0 || r >= board_size || c < 0 || c >= board_size); 
}

side opp_side(side side) {
    if (side == WHITE_SIDE) {
        return BLACK_SIDE;
    }
    return WHITE_SIDE;
}

side side_from_turn(game_turn turn) {
    if (turn == WHITES_TURN) {
        return WHITE_SIDE;
    } else {
        return BLACK_SIDE;
    }
}

player_perspective perspective_from_turn(game_turn turn) {
    if (turn == WHITES_TURN) {
        return WHITES_PERSPECTIVE;
    }
    return BLACKS_PERSPECTIVE;
}

