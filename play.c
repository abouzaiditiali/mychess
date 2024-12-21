#include "board.h"

int main() {
    Board* board = board_new(WHITE_MOVING_UP, 
                             STANDARD_POSITION);
    //place_piece(board, "ke4");
    //place_piece(board, "Bc6");

    board_show(board);

    board_free(board);
    return 0;
}

