#ifndef PIECE_H
#define PIECE_H

#include "pos.h"

typedef enum { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING 
} piece_kind;
typedef enum { BLACK_SIDE, WHITE_SIDE 
} side;
typedef enum { MOVED, NOT_MOVED 
} piece_moved;
typedef enum { EN_PASSANT, CAPTURE, CASTLE, PAWN_PROMOTION, NO_CAPTURE
} move_type;
typedef struct { char* transformations; unsigned char len, max_repeat;
} Transformation;
typedef struct { piece_kind kind; side side; piece_moved moved; Pos position;
} Piece;
typedef struct Piece_entry { Piece* piece; struct Piece_entry* next;
} Piece_entry;
typedef struct { Piece_entry* head; unsigned char len;
} Piecelist;

Piece* piece_new(piece_kind kind, side side, piece_moved moved, Pos pos);
void piece_show(Piece* piece);
void piece_free(Piece* piece);

//Transformation transformation_get(piece_kind kind, board_direction direction,  
//                                 piece_moved moved, move_type type);
//void transformation_free(Transformation t);

Piecelist* piecelist_new();
void piecelist_show(Piecelist* piecelist);
void piecelist_insert(Piecelist* piecelist, Piece* piece);
void piecelist_delete(Piecelist* piecelist, Piece* piece);
void piecelist_free(Piecelist* piecelist);

#endif /* PIECE_H */
