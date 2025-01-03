#ifndef PIECE_H
#define PIECE_H

#include "translation.h"

typedef enum { MOVED, NOT_MOVED 
} piece_moved;

typedef struct { piece_kind kind; side side; piece_moved moved; Pos position;
} Piece;
typedef struct Piece_entry { Piece* piece; struct Piece_entry* next;
} Piece_entry;
typedef struct { Piece_entry* head; unsigned char len;
} Piecelist;

Piece* piece_new(piece_kind kind, side side, piece_moved moved, Pos pos);
bool is_sliding_piece(piece_kind kind);
void piece_show(Piece* piece);
void piece_free(Piece* piece);

Piecelist* piecelist_new();
void piecelist_show(Piecelist* piecelist);
void piecelist_insert(Piecelist* piecelist, Piece* piece);
void piecelist_delete(Piecelist* piecelist, Piece* piece);
void piecelist_free(Piecelist* piecelist);

#endif /* PIECE_H */
