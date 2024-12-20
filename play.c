#include "board.h"

int main() {
    Board* board = board_new(WHITE_MOVING_UP, EMPTY_BOARD);
    place_piece(board, "Pc3");
    place_piece(board, "pc5");
    place_piece(board, "Ra5");
    place_piece(board, "kd5");
    place_piece(board, "Nb6");

    board_show(board);
    printf("%d\n", check(board, BLACK_SIDE));

    board_free(board);
    return 0;
}

