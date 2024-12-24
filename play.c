#include "logic.h"

int main() {
    Game* game = game_new();
    game_set(game);

    board_show(game->board, BLACKS_PERSPECTIVE);

    game_free(game);
    return 0;
}

