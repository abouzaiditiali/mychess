#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "globals.h"

typedef enum { WHITES_PERSPECTIVE, BLACKS_PERSPECTIVE
} player_perspective;
typedef enum { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING 
} piece_kind;
typedef enum { BLACK_SIDE, WHITE_SIDE 
} side;
typedef enum { EN_PASSANT, PAWN_PROMOTION_CAPTURE, PAWN_PROMOTION_NO_CAPTURE, 
               KINGSIDE_CASTLE, QUEENSIDE_CASTLE, PAWN_PUSH_BY_TWO, 
               NO_CAPTURE, CAPTURE, UNRECOGNIZED
} move_type;
typedef enum { BLACKS_TURN, WHITES_TURN
} game_turn;

//raising errors
void bounds_check(char r, char c);
void malloc_check(void* p);
void pointer_check(void* p);

bool out_of_bounds(char r, char c);
side opp_side(side side);
side side_from_turn(game_turn turn);
player_perspective perspective_from_turn(game_turn turn);

#endif /* UTILS_H */
