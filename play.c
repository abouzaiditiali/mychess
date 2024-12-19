#include "board.h"

int main() {
    Board* board = board_new(MOVING_UP, EMPTY_BOARD);
    place_piece(

    board_show(board);
    printf("%d\n", check(board, WHITE_SIDE));

    return 0;
}

