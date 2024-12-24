#ifndef LOGIC_H
#define LOGIC_H

#include "move.h"

typedef enum { BLACKS_TURN, WHITES_TURN
} game_turn;
typedef enum { IN_PROGRESS, DRAW, BLACK_WIN, WHITE_WIN
} game_outcome;

typedef struct {
    Board* board;
    Movestack* moves;
    game_turn turn;
} Game;

Game* game_new();
void game_set(Game* game);
void game_free(Game* game);

bool move(Game* game, Square from, Square to);
void undo(Game* game);

game_outcome outcome(Game* game);

#endif /* LOGIC_H */
