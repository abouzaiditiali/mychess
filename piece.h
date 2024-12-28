#ifndef PIECE_H
#define PIECE_H

#include "pos.h"

typedef enum { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING 
} piece_kind;
typedef enum { BLACK_SIDE, WHITE_SIDE 
} side;
typedef enum { MOVED, NOT_MOVED 
} piece_moved;
typedef enum { EN_PASSANT, PAWN_PROMOTION_CAPTURE, PAWN_PROMOTION_NO_CAPTURE, 
               KINGSIDE_CASTLE, QUEENSIDE_CASTLE, PAWN_PUSH_BY_TWO, 
               NO_CAPTURE, CAPTURE, UNRECOGNIZED
} move_type;
typedef struct { piece_kind kind; side side; piece_moved moved; Pos position;
} Piece;
typedef struct Piece_entry { Piece* piece; struct Piece_entry* next;
} Piece_entry;
typedef struct { Piece_entry* head; unsigned char len;
} Piecelist;

typedef struct { char r, c;
} Translation;

Piece* piece_new(piece_kind kind, side side, piece_moved moved, Pos pos);
void piece_show(Piece* piece);
void piece_free(Piece* piece);

move_type* plausible_mts(piece_kind kind, Displacement d, Direction dir,
                            side side, unsigned char* tylen);

Piecelist* piecelist_new();
void piecelist_show(Piecelist* piecelist);
void piecelist_insert(Piecelist* piecelist, Piece* piece);
void piecelist_delete(Piecelist* piecelist, Piece* piece);
void piecelist_free(Piecelist* piecelist);

side opp_side(side side);

#endif /* PIECE_H */
