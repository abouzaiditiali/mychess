#include "move.h"

Move move_make(Square from, Square to, piece_kind moved, move_type type, 
                        piece_kind captured) {
    Move move;
    move.from = from;
    move.to = to;
    move.moved = moved;
    move.type = type;
    move.captured = captured;
    return move;
}

//Helper
char convert_kind_to_char(piece_kind kind) {
    piece_kind pk_char[6] = {'P', 'B', 'N', 'R', 'Q', 'K'};
    return pk_char[kind];
}

void move_show(Move move) {
    if (move.moved != PAWN) {
        printf("%c", convert_kind_to_char(move.moved));
    }
    if (move.moved == PAWN && (move.type == CAPTURE || 
                               move.type == EN_PASSANT)) {
        printf("%c", move.from.file);
    }
    if (move.type == CAPTURE || move.type == EN_PASSANT) {
        printf("x");
    }
    square_show(move.to);
    if (move.type == EN_PASSANT) {
        printf(" e.p.");
    }
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
    return s->tail->move;
}

void movestack_show(Movestack* s) {
    printf("MOVE COUNT: %hhu\n", s->len / 2);
    unsigned char i = 1, j = 1;
    Move_entry* curr = s->head;
    while (curr) {
        if (i % 2 != 0) {
            printf("%d. ", j++);
        }
        move_show(curr->move);
        printf(" ");
        i++;
        curr = curr->next;
    }
    printf("\n");
}

