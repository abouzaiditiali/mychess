#include "logic.h"

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

