#ifndef MOVE_H
#define MOVE_H

#include "board.h"

typedef struct {
    Square from, to;
    piece_kind moved;
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

Move move_make(Square from, Square to, piece_kind moved, move_type type, 
                                                        piece_kind captured);
bool is_capture_move(move_type mt);
bool is_non_capture_move(move_type mt);
void move_show(Move move);

Movestack* movestack_new();
void movestack_add(Movestack* s, Move move);
Move movestack_pop(Movestack* s);
void movestack_free(Movestack* s);
Move last_move(Movestack* s);
void movestack_show(Movestack* s);

move_type* plausible_mts(piece_kind kind, Displacement d, Direction dir,
                            side side, unsigned char* tylen);
move_type find_move_type(Movestack* s, Board* board, Piece* op, Piece* dp, 
                            move_type* mts, Pos tpos, unsigned char tylen, 
                            Pos* captured_pos);
void valid_move_check(Pos from, Pos to, Piece* op, Piece* dp, game_turn turn);

#endif /* MOVE_H */
