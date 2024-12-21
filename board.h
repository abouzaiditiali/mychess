#ifndef BOARD_H
#define BOARD_H

#include "piece.h"


typedef struct {
    Piece*** matrix;  
    board_direction direction;
    Piecelist *white_pieces, *black_pieces;
} Board;

typedef enum {
    STANDARD_POSITION,
    EMPTY_BOARD
} start_position;


Board* board_new(board_direction direction, start_position position);

void board_free(Board* board);

void board_show(Board* board);

Piece* board_get(Board* board, Pos pos);

void board_set(Board* board, Pos pos, Piece* piece);

//for interactive testing only
void place_piece(Board* board, char* call);

void board_swap(Board* board, Pos pos1, Pos pos2);

void board_flip(Board* board);

Piece* check(Board* board, side threatened);

Piece* pin(Board* board, Piece* piece);

bool checkmate(Board* board);

bool stalemate(Board* board);


#endif /* BOARD_H */
