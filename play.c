#include "logic.h"

player_perspective perspective_from_turn(game_turn turn) {
    if (turn == WHITES_TURN) {
        return WHITES_PERSPECTIVE;
    }
    return BLACKS_PERSPECTIVE;
}

int main() {
    Game* game = game_new();
    game_set(game);
    
    game_show(game, WHITES_PERSPECTIVE);

    while (1) {
        Square from;
        Square to;

        scanf("%c%hhu%c%hhu%*c", &from.file, &from.rank, &to.file, &to.rank);
        
        if (game_move(game, from, to) == false) {
            continue;
        }

        game_show(game, perspective_from_turn(game->turn));
    }    

    game_free(game);
    return 0;
}

