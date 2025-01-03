#ifndef BOARD_H
#define BOARD_H

#include "piece.h"

typedef struct {
    Piece*** matrix;  
    Piecelist *white_pieces, *black_pieces;
} Board;
typedef enum { NEW, EXISTING
} piece_status;

Board* board_new();
Piece* board_get(Board* board, Pos pos);
void board_set(Board* board, Pos pos, Piece* piece, piece_status status);
void board_show(Board* board, player_perspective);
void board_free(Board* board);

Pos kpos_get(Board* board, side kside);
Piece* nrook_get(Board* board, move_type mt, side side);

bool void_traj_check(Board* board, Direction dir, Pos fpos, Pos tpos);
Piece* piece_traj_encounter(Board* board, Direction dir, Pos fpos, 
                          piece_kind* pk, unsigned char pklen, side side, 
                          unsigned char max_repeat);

Piece* pin(Board* board, Pos pinned_pos, side pinning_side); 
bool square_threatened(Board* board, Pos pos, side threatening);
bool legal_to_move(Board* board, Piece* op, Pos tpos, Pos captured_pos,
                                                               move_type mt);

#endif /* BOARD_H */
