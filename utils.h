#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "globals.h"

void bounds_check(unsigned char r, unsigned char c);

void malloc_check(void* p);

void pointer_check(void* p);

#endif /* UTILS_H */
