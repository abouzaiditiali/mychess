#include "logic.h"

int main() {
    Game* game = game_new();

    board_show(game->board, WHITES_PERSPECTIVE);

    game_free(game);
    return 0;
}

