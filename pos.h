#ifndef POS_H
#define POS_H

#include "globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct {
    unsigned char r, c; //from 0 to 7
} Pos;

typedef struct {
    unsigned char r; //from 1 to 8
    char c; //from a to h
} Square; 

typedef enum {
    WHITE_MOVING_UP, BLACK_MOVING_UP
} board_direction;

Pos pos_make(unsigned char r, unsigned char c);

Square square_make(char c, unsigned char r);

Square pos_convert(Pos pos, board_direction direction);

Pos square_convert(Square square, board_direction direction);

void square_show(Square square);


//General-purpose Functions
void bounds_check(Pos pos);
void malloc_check(void* p);


#endif /* POS_H */
