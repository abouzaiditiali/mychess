#ifndef LOGIC_H
#define LOGIC_H

#include "move.h"

typedef enum { IN_PROGRESS, DRAW, BLACK_WIN, WHITE_WIN
} game_outcome;
typedef enum { NO_CHECK, CHECK, DOUBLE_CHECK
} check_status;

typedef struct {
    Pos checking;
    check_status status;
    side threatened;
} Check;

typedef struct {
    Board* board;
    Movestack* moves;
    game_turn turn;
    Check* check; //for efficiency
} Game;

Game* game_new();
void game_set(Game* game);
void game_free(Game* game);
void game_show(Game* game, player_perspective perspective);
bool game_move(Game* game, Square from, Square to);

void undo(Game* game);
game_outcome outcome(Game* game);

#endif /* LOGIC_H */
