#ifndef MOVE_H
#define MOVE_H

#include "board.h"

typedef struct {
    Square from, to;
    move_type type;
    union {
        piece_kind captured; //only if en-passant, pp with capture, capture
    };
} Move;

typedef struct Move_entry {
    Move move;
    struct Move_entry *prev, *next; 
} Move_entry;

typedef struct {
    Move_entry *head, *tail;
    unsigned char len;
} Movestack;

Move move_make(Square from, Square to, move_type type, piece_kind captured);
Movestack* movestack_new();
void movestack_add(Movestack* s, Move move);
Move movestack_pop(Movestack* s);
void movestack_free(Movestack* s);

#endif /* MOVE_H */
