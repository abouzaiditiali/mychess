#ifndef LOGIC_H
#define LOGIC_H

#include "move.h"


typedef enum {
    BLACKS_TURN,
    WHITES_TURN
} game_turn;

typedef enum {
    IN_PROGRESS,
    DRAW,
    BLACK_WIN,
    WHITE_WIN
} game_outcome;

typedef struct Game Game;
struct Game {
    Board* board;
    Piece *white_pieces, *black_pieces;
    Movestack* moves;
    game_turn turn;
    Game* prev;
};


Game* game_new();

void game_free(Game* game);

bool move(Game* game, Move move);

move_type move_type(Game* game, Move move);

void undo(Game* game);

game_outcome outcome(Game* game);


#endif /* LOGIC_H */
