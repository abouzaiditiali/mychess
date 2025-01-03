#ifndef POS_H
#define POS_H

#include "utils.h"

typedef struct { unsigned char r, c; 
} Pos;
typedef struct { char r, c;
} Displacement;
typedef struct { char r, c; //all diagonals, verticals, horizontals, L
} Direction;
typedef struct { char file; unsigned char rank; //consider adding square color
} Square; 

Pos pos_make(unsigned char r, unsigned char c);
Pos pos_flip(Pos pos);
Square pos_convert(Pos pos, player_perspective perspective);
bool pos_cmp(Pos pos1, Pos pos2);
void pos_show(Pos pos);
Displacement pos_displacement(Pos from, Pos to);
Pos crossedpos_get(move_type mt, Pos tpos);
bool pos_in_traj(Pos pos, Pos e1, Pos e2);

Displacement displacement_make(char r, char c);
Direction displacement_direction(Displacement d);
void displacement_show(char r, char c);

Direction direction_make(char r, char c);
void direction_show(char r, char c);
bool direction_cmp(Direction dir1, Direction d2);

Square square_make(char file, unsigned char rank);
Pos square_convert(Square square);
void square_show(Square square);
bool square_cmp(Square s1, Square s2);

#endif /* POS_H */
