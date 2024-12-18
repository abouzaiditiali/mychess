#ifndef BOARD_H
#define BOARD_H

#include "piece.h"


typedef enum {
    EMPTY,
    PIECE
} cell_type;

typedef struct {
    cell_type type;
    union {
        Piece* piece;
    };
} Cell;

typedef struct {
    Cell*** matrix;  
    unsigned char size;
    move_direction direction; //in relation to white
    Pos white_king, black_king;
    Piece **white_pieces, **black_pieces;
    unsigned char wpnum, bpnum, plen;
} Board;



Cell* cell_new(cell_type type, Piece* piece);

void cell_free(Cell* cell);

Board* board_new(move_direction direction);

void board_free(Board* board);

void board_show(Board* board);

Cell* board_get(Board* board, Pos pos);

void board_set(Board* board, Pos pos, cell_type type, Piece* piece);

void board_flip(Board* board);

bool check(Board* board);

bool checkmate(Board* board);

bool stalemate(Board* board);

bool square_protected(Board* board, Pos pos);

bool square_threatened(Board* board, Pos pos);

bool piece_pinned(Board* board, Piece* piece);


#endif /* BOARD_H */
