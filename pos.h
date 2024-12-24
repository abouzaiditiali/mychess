#ifndef POS_H
#define POS_H

#include "utils.h"

typedef enum { WHITES_PERSPECTIVE, BLACKS_PERSPECTIVE
} player_perspective;

typedef struct { unsigned char r, c; 
} Pos;
typedef struct { char file; unsigned char rank; //consider adding square color
} Square; 

Pos pos_make(unsigned char r, unsigned char c);
Pos pos_flip(Pos pos);
Square pos_convert(Pos pos, player_perspective perspective);
bool pos_cmp(Pos pos1, Pos pos2);

Square square_make(char file, unsigned char rank);
Pos square_convert(Square square);
void square_show(Square square);

#endif /* POS_H */
