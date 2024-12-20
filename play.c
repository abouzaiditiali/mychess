#include "board.h"

int main() {
    Board* board = board_new(BLACK_MOVING_UP, STANDARD_POSITION);
    board_flip(board);

    board_show(board);
    //printf("%d\n", check(board, WHITE_SIDE));

    board_free(board);
    return 0;
}

