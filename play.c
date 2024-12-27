#include "logic.h"

int main() {
    Game* game = game_new();
    game_set(game);

    move(game, square_make('e', 2), square_make('e', 4));

    board_show(game->board, WHITES_PERSPECTIVE);

    game_free(game);
    return 0;
}

