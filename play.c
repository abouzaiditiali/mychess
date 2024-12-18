#include "board.h"

int main() {
    Board* board = board_new(MOVING_UP);
    board_show(board);
    return 0;
}

