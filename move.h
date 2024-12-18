#ifndef MOVE_H
#define MOVE_H

#include "board.h"

typedef enum {
    EN_PASSANT,
    CAPTURE,
    CASTLE,
    NO_CAPTURE
} move_type;

typedef struct {
    Pos from, to;
} Move;

typedef struct Move_entry Move_entry;
struct Move_entry {
    Move move;
    Move_entry *prev, *next; 
};

typedef struct {
    Move_entry *head, *tail;
    unsigned char len;
} Movestack;


Movestack* movestack_new();

void movestack_add(Movestack* movestack, Move move); //add at the end

void movestack_pop(Movestack* movestack); //pop the last

Move last_move(Movestack* movestack);

#endif /* MOVE_H */
