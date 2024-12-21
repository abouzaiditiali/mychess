#ifndef PIECE_H
#define PIECE_H

#include "pos.h"


typedef enum {
    PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING
} piece_kind;

typedef enum {
    BLACK_SIDE, WHITE_SIDE
} side;

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
    EN_PASSANT, CAPTURE, CASTLE, PAWN_PROMOTION, NO_CAPTURE
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

void piece_show(Piece* piece, board_direction direction);

void piece_free(Piece* piece);

Transformation transformation_get(piece_kind kind, move_type type, 
                                       board_direction direction);

void transformation_free(Transformation t);

Piecelist* piecelist_new();

void piecelist_show(Piecelist* piecelist, board_direction direction);

void piecelist_insert(Piecelist* piecelist, Piece* piece);

void piecelist_delete(Piecelist* piecelist, Piece* piece);

void piecelist_free(Piecelist* piecelist);

void piece_entry_free(Piece_entry* piece_entry);


#endif /* PIECE_H */
