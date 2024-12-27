#ifndef PIECE_H
#define PIECE_H

#include "pos.h"

typedef enum { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING 
} piece_kind;
typedef enum { BLACK_SIDE, WHITE_SIDE 
} side;
typedef enum { MOVED, NOT_MOVED 
} piece_moved;
typedef enum { 
    EN_PASSANT, 
    PAWN_PROMOTION_CAPTURE, 
    PAWN_PROMOTION_NO_CAPTURE, 
    KINGSIDE_CASTLE,
    QUEENSIDE_CASTLE,
    PAWN_PUSH_BY_TWO,
    CAPTURE, //event is disjoint (i.e. no en-passant, promotion)
    NO_CAPTURE, //event is disjoint (i.e. no castle, push, promotion)
} move_type;
typedef struct { piece_kind kind; side side; piece_moved moved; Pos position;
} Piece;
typedef struct Piece_entry { Piece* piece; struct Piece_entry* next;
} Piece_entry;
typedef struct { Piece_entry* head; unsigned char len;
} Piecelist;

typedef struct {
    Direction direction;
    char r, c;
} Translation;

typedef struct {
    Translation* translations;
    unsigned char trlen, max_repeat;
    move_type* type; //returns all plausible move types
    unsigned char tylen;
    side side; //trivial for queen, bishop, rook
} MoveTypeTranslation;

typedef struct { 
    piece_kind kind;
    MoveTypeTranslation** mtt; 
    unsigned char len;
} Translationlist;

Piece* piece_new(piece_kind kind, side side, piece_moved moved, Pos pos);
void piece_show(Piece* piece);
void piece_free(Piece* piece);

Translationlist* translationlist_new(piece_kind kind); 
void translationlist_free(Translationlist* t);
move_type* translationlist_retrieve(Translationlist* tl, Displacement d, 
                               Direction dir, side side, unsigned char* tylen);

Piecelist* piecelist_new();
void piecelist_show(Piecelist* piecelist);
void piecelist_insert(Piecelist* piecelist, Piece* piece);
void piecelist_delete(Piecelist* piecelist, Piece* piece);
void piecelist_free(Piecelist* piecelist);

side opp_side(side side);

#endif /* PIECE_H */
