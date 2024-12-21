#include "board.h"

int main() {
    Board* board = board_new(BLACK_MOVING_UP, 
                             EMPTY_BOARD);
    place_piece(board, "ke4");
    place_piece(board, "Bc6");
    place_piece(board, "nd5");

    board_show(board);

    board_free(board);
    return 0;
}

