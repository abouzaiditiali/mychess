#include "board.h"

int main() {
    Board* board = board_new(WHITE_MOVING_UP, EMPTY_BOARD);
    place_piece(board, "kc4");
    place_piece(board, "pe6");
    //place_piece(board, "rd5");
    place_piece(board, "Qg8");

    board_show(board);
    printf("%d\n", pin(board, board_get(board, make_pos(2, 4))));

    board_free(board);
    return 0;
}

