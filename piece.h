#ifndef PIECE_H
#define PIECE_H

#include "pos.h"


typedef enum {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} piece_kind;

typedef enum {
    BLACK_SIDE,
    WHITE_SIDE
} piece_side;

typedef enum {
    ONE_TRANSFORMATION,
    TWO_TRANSFORMATIONS,
    AS_MANY
} max_distance;

typedef enum {
    MOVING_UP,
    MOVING_DOWN
} move_direction;

typedef struct {
    piece_kind kind;
    piece_side side;
    Pos position;
    char* transformations;
    union {
        char* pc_transformations; //pc stands for pawn capture
        max_distance pc_travel;
        move_direction direction;
    };
    bool moved;
    max_distance travel; //highest number of transformations allowed
} Piece;


Piece* piece_new(piece_kind kind, piece_side side, Pos position);

void set_pawn_transformation(Piece* piece, move_direction direction);

void piece_free(Piece* piece);


#endif /* PIECE_H */
