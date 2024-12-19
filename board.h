#ifndef BOARD_H
#define BOARD_H

#include "piece.h"



typedef struct {
    Piece*** matrix;  
    unsigned char size;
    move_direction direction; //in relation to white
    Piece **white_pieces, **black_pieces;
    unsigned char wpnum, bpnum, plen;
} Board;

typedef enum {
    STANDARD_POSITION,
    EMPTY_BOARD
} start_position;


Board* board_new(move_direction direction, start_position position);

void board_free(Board* board);

void board_show(Board* board);

Piece* board_get(Board* board, Pos pos);

void board_set(Board* board, Pos pos, Piece* piece);

void board_swap(Board* board, Pos pos1, Pos pos2);

void board_flip(Board* board);

bool check(Board* board, piece_side threatened);

bool checkmate(Board* board);

bool stalemate(Board* board);

bool piece_pinned(Board* board, Piece* piece);

Pos kingpos_get(Board* board, piece_side side);


#endif /* BOARD_H */
