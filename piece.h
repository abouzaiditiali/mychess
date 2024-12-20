#ifndef PIECE_H
#define PIECE_H

#include "pos.h"


typedef enum {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
} piece_kind;

typedef enum {
    BLACK_SIDE, WHITE_SIDE
} side;

typedef enum {
    WHITE_MOVING_UP, BLACK_MOVING_UP
} board_direction;

typedef enum {
    MOVED, NOT_MOVED
} piece_moved;

typedef struct {
    char* transformations;
    unsigned char len, max_repeat; //can either be 1, 2, or 7
} Transformation;

typedef struct {
    piece_kind kind;
    side side;
    piece_moved moved;
    Pos position;
} Piece;

typedef enum {
    EN_PASSANT, CAPTURE, CASTLE, NO_CAPTURE
} move_type;

typedef struct Piece_entry Piece_entry;
struct Piece_entry {
    Piece* piece;
    Piece_entry* next;
};

typedef struct {
    Piece_entry* head;
    unsigned char len;
} Piecelist;


Piece* piece_new(piece_kind kind, side side, piece_moved moved, Pos pos);

void piece_free(Piece* piece);

Transformation transformation_get(piece_kind kind, move_type type, 
                                       board_direction direction);

void transformation_free(Transformation t);

Piecelist* piecelist_new();

void piecelist_insert(Piecelist* piecelist, Piece* piece);

void piecelist_delete(Piecelist* piecelist, Piece* piece);

void piecelist_free(Piecelist* piecelist);

void piece_entry_free(Piece_entry* piece_entry);


#endif /* PIECE_H */
