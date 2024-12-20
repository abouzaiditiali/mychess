#ifndef POS_H
#define POS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct {
    unsigned char r, c;
} Pos;
 

Pos make_pos(unsigned char r, unsigned char c);

//General Purpose Function
void malloc_check(void* p);


#endif /* POS_H */
