#ifndef POS_H
#define POS_H

#include "utils.h"

typedef enum { WHITE_MOVING_UP, BLACK_MOVING_UP 
} board_direction;
typedef struct { unsigned char r, c; 
} Pos;
typedef struct { char file; unsigned char rank; //consider adding square color
} Square; 


Pos pos_make(unsigned char r, unsigned char c);
Square pos_convert(Pos pos, board_direction direction);
bool pos_cmp(Pos pos1, Pos pos2);

Square square_make(char file, unsigned char rank);
Pos square_convert(Square square, board_direction direction);
void square_show(Square square);

#endif /* POS_H */
