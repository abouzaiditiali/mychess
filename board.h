#ifndef BOARD_H
#define BOARD_H

#include "piece.h"

typedef struct {
    Piece*** matrix;  
    Piecelist *white_pieces, *black_pieces;
} Board;

Board* board_new();
Piece* board_get(Board* board, Pos pos);
void board_set(Board* board, Pos pos, Piece* piece);
void board_show(Board* board, player_perspective);
void board_free(Board* board);

Pos kpos_get(Board* board, side kside);
Piece* pin(Board* board, Pos pinned_pos, side pinning_side);
void place_piece(Board* board, char* call);

#endif /* BOARD_H */
