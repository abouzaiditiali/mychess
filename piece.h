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
    MOVING_UP,
    MOVING_DOWN,
} move_direction;

typedef struct {
    char* transformations;
    unsigned char len;
    unsigned char max_repeat; //can either be 1, 2, or 7
} Transformation;

typedef struct {
    piece_kind kind;
    piece_side side;
    Pos position;
    bool moved;
} Piece;


Piece* piece_new(piece_kind kind, piece_side side, Pos position);

void piece_free(Piece* piece);

Transformation transformation_new(char* t, unsigned char len, 
                                                unsigned char max_repeat);

Transformation transformation_get(piece_kind kind); //any piece but pawn

Transformation ptransformation_get(move_direction direction, bool capture);

void transformation_free(Transformation t);


#endif /* PIECE_H */
