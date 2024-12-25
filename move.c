#include "move.h"

Move move_make(Square from, Square to, move_type type, piece_kind captured) {
    Move move;
    move.from = from;
    move.to = to;
    move.type = type;
    move.captured = captured;
    return move;
}

Movestack* movestack_new() {
    Movestack* s = (Movestack*)malloc(sizeof(Movestack));
    malloc_check(s);
    s->head = NULL;
    s->tail = NULL;
    s->len = 0;
    return s;
}

void movestack_add(Movestack* s, Move move) {
    pointer_check(s);
    Move_entry* e = (Move_entry*)malloc(sizeof(Move_entry));
    malloc_check(e);
    e->move = move;
    e->prev = s->tail;
    e->next = NULL;
    if (s->tail) {
        s->tail->next = e;
    }
    s->tail = e;
    if (s->head == NULL) {
        s->head = e;
    }
    s->len += 1;
}

Move movestack_pop(Movestack* s) {
    pointer_check(s);
    pointer_check(s->head);
    Move last = s->tail->move;
    Move_entry* tmp = s->tail;
    s->tail = s->tail->prev;
    free(tmp);
    if (s->tail == NULL) {
        s->head = NULL;
    } else {
        s->tail->next = NULL;
    }
    s->len -= 1;
    return last;
}

void movestack_free(Movestack* s) {
    Move_entry* tmp;
    while(s->head) {
        tmp = s->head;
        s->head = s->head->next;
        free(tmp);
    }
    free(s);
}

Move last_move(Movestack* s) {
    if (s->len < 2) {
        fprintf(stderr, "There is no last move\n");
        exit(1);
    }
    return s->tail->prev->move;
}

