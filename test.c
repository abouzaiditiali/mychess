#include "logic.h"

player_perspective perspective_from_turn(game_turn turn) {
    if (turn == WHITES_TURN) {
        return WHITES_PERSPECTIVE;
    }
    return BLACKS_PERSPECTIVE;
}

void play_moves(Game* game, char* moves) {
    unsigned char i = 0;
    Square from, to;
    while (moves[i]) {
        sscanf(moves, "%c%hhu%c%hhu", &from.file, &from.rank, &to.file, &to.rank); 
        game_move(game, from, to);
        moves += 5;
    }
}

int main() {
    Game* game = game_new();
    game_set(game);
    
    char* moves = "e2e4 e7e5 d2d4 e5d4 c2c4 d4c3 ";
    play_moves(game, moves);

    game_show(game, perspective_from_turn(game->turn));

    game_free(game);
    return 0;
}
    
