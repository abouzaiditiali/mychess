#include "board.h"

void set_standard_position(Board* board) {
    unsigned char size = board->size;
    for (unsigned char c = 0; c < size; c++) {
        board_set(board, make_pos(1, c), 
                  piece_new(PAWN, BLACK_SIDE, make_pos(1, c), false));
        board_set(board, make_pos(6, c), 
                  piece_new(PAWN, WHITE_SIDE, make_pos(6, c), false));
    }
    piece_kind pk[8] = {ROOK, ROOK, KNIGHT, KNIGHT, 
                        BISHOP, BISHOP, QUEEN, KING};
    unsigned char positions[8] = {0, 7, 1, 6, 2, 5, 3, 4};
    for (unsigned char i = 0; i < 8; i++) {
        board_set(board, make_pos(0, positions[i]),
               piece_new(pk[i], BLACK_SIDE, make_pos(0, positions[i]), false));
        board_set(board, make_pos(7, positions[i]),
               piece_new(pk[i], WHITE_SIDE, make_pos(7, positions[i]), false));
    }
}

Board* board_new(move_direction direction, start_position position) {
    unsigned char size = 8;
    Board* board = (Board*)malloc(sizeof(Board));
    board->matrix = (Piece***)malloc(sizeof(Piece**) * size);
    for (unsigned char r = 0; r < size; r++) {
        board->matrix[r] = (Piece**)malloc(sizeof(Piece*) * size);
        for (unsigned char c = 0; c < size; c++) {
            board->matrix[r][c] = NULL;
        }
    }
    board->size = size;
    board->direction = MOVING_UP;
    board->plen = 20;
    unsigned char plen = board->plen;
    board->white_pieces = (Piece**)malloc(sizeof(Piece*) * plen);
    board->black_pieces = (Piece**)malloc(sizeof(Piece*) * plen);
    board->wpnum = 0;
    board->bpnum = 0;
    for (unsigned char i = 0; i < plen; i++) {
        board->white_pieces[i] = NULL;
        board->black_pieces[i] = NULL;
    }
    if (position == STANDARD_POSITION) {
        set_standard_position(board);
    }
    if (direction == MOVING_DOWN) {
        board_flip(board);
    }
    return board;
}

void board_free(Board* board) {
    unsigned char size = board->size;
    for (unsigned char r = 0; r < size; r++) {
        for (unsigned char c = 0; c < size; c++) {
            piece_free(board_get(board, make_pos(r, c)));
        }
        free(board->matrix[r]);
    }
    free(board->white_pieces);
    free(board->black_pieces);
    free(board->matrix);
}

//Helper 
void print_chars(move_direction direction) {
    printf("  ");
    if (direction == MOVING_UP) {
        for (char c = 'a'; c <= 'h'; c++) {
            printf("   %c", c);
        }
    } else {
        for (char c = 'h'; c >= 'a'; c--) {
            printf("   %c", c);
        }
    }
    printf("\n");
}

//Helper
char maybe_upp(piece_side side, char c) {
    if (side == WHITE_SIDE) {
        char new_c = c - ('a' - 'A');
        return new_c;
    }
    return c;
}

//Helper
char convert_piece_to_char(Piece* piece) {
    char c;
    if (piece == NULL) {
        return '.';
    }
    switch (piece->kind) {
        case PAWN:
            return maybe_upp(piece->side, 'p');
        case KNIGHT:
            return maybe_upp(piece->side, 'n');
        case BISHOP:
            return maybe_upp(piece->side, 'b');
        case ROOK:
            return maybe_upp(piece->side, 'r');
        case QUEEN:
            return maybe_upp(piece->side, 'q');
        case KING:
            return maybe_upp(piece->side, 'k');
    }
}

void board_show(Board* board) {
    printf("\n");
    print_chars(board->direction);
    unsigned char mu = 8, md = 1, curr = 1;
    for (unsigned char i = 0; i < 17; i++) {
        if (i % 2 == 0) {
            printf("   ");
            for (unsigned char j = 0; j < 8; j++) {
                printf(" ---");
            }
        } else {
            printf(" ");
            if (board->direction == MOVING_UP) {
                printf("%hhu", mu);
            } else {
                printf("%hhu", md);
            }
            printf(" |");
            for (unsigned char j = 0; j < 8; j++) {
                Piece* piece = board_get(board, make_pos(i - curr, j));
                char c = convert_piece_to_char(piece);
                printf(" %c |", c);
            }
            curr++;
            printf(" ");
            if (board->direction == MOVING_UP) {
                printf("%hhu", mu--);
            } else {
                printf("%hhu", md++);
            }
        }
        printf("\n"); 
    }
    print_chars(board->direction);
    printf("\n");
}

//Helper 
void bounds_check(unsigned char size, Pos pos) {
    if (pos.r < 0 || pos.r >= size || pos.c < 0 || pos.c >= size) {
        fprintf(stderr, "Tried to access position outside of board\n");
        exit(1);
    }
}
 
Piece* board_get(Board* board, Pos pos) {
    bounds_check(board->size, pos);
    Piece* piece = board->matrix[pos.r][pos.c];
    return piece;
}

//Helper
void board_set(Board* board, Pos pos, Piece* piece) {
    bounds_check(board->size, pos);

    if (piece == NULL) {
        if (board_get(board, pos)) {
            //make changes
            //remove piece from array by setting its spot to null
            //reduce num pieces by 1
            //free the piece
        }
        board->matrix[pos.r][pos.c] = NULL;

    } else {

    }



    unsigned char plen = board->plen;
    piece_side side = piece->side;
    if ((side == BLACK_SIDE && board->bpnum == plen) || 
        (side == WHITE_SIDE && board->wpnum == plen)) {
        fprintf(stderr, "Cannot add more pieces\n");
        exit(1);
    }
    for (unsigned char i = 0; i < plen; i++) {
        if (side == BLACK_SIDE) {
            if (board->black_pieces[i] == NULL) {
                board->black_pieces[i] = piece;
                (board->bpnum)++;
                break;
            }
        } else {
            if (board->white_pieces[i] == NULL) {
                board->white_pieces[i] = piece;
                (board->wpnum)++;
                break;
            }
        }
    }
    return true;
}

void board_swap(Board* board, Cell* cell1, Pos pos1, Cell* cell2, Pos pos2) {
    Cell* tmp_cell = cell1;
    board->matrix[pos1.r][pos1.c] = cell2;
    board->matrix[pos2.r][pos2.c] = tmp_cell;
    if (cell2->piece) {
        cell2->piece->position = pos1;
    }
    if (tmp_cell->piece) {
        tmp_cell->piece->position = pos2;
    }
}

void board_flip(Board* board) {
    unsigned char size = board->size;
    for (unsigned char r = 0; r < (size / 2); r++) {
        for (unsigned char c = 0; c < size; c++) {
            Pos pos = make_pos(r, c);
            Pos mirr_pos = make_pos(size - 1 - r, size - 1 - c);
            Cell* cell = board_get(board, pos);
            Cell* mirr_cell = board_get(board, mirr_pos);

            board_swap(board, cell, pos, mirr_cell, mirr_pos);
        }
    }
    if (board->direction == MOVING_UP) {
        board->direction = MOVING_DOWN;
    } else {
        board->direction = MOVING_UP;
    }
}



//Helper
bool encounter(Board* board, piece_kind kind, piece_side side, 
                                     Transformation transformation, Pos pos) {
    char* transformations = transformation.transformations;
    unsigned char max_repeat = transformation.max_repeat;
    for (unsigned char i = 0; i < transformation.len; i += 2) {
        char tr = transformations[i], tc = transformations[i + 1];
        Pos curr_pos = pos;
        for (unsigned char j = 0; j < max_repeat; j++) {
            curr_pos.r += tr;
            curr_pos.c += tc;
            if (out_of_bounds(board->size, curr_pos)) {
                break;
            }
            Cell* cell = board_get(board, curr_pos);
            if (cell->type == EMPTY) {
                continue;
            }
            if (cell->piece->side != side || cell->piece->kind != kind) {
                break;
            }
            printf("%hhu %hhu\n", curr_pos.r, curr_pos.c);
            printf("was here haha\n");
            return true;
        }
    }
    return false;
}

bool handle_kind(Board* board, piece_kind kind, move_direction direction, 
                                piece_side side, bool* maybe_found, Pos pos) { 
    piece_kind pk[6] = {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING};
    for (unsigned char i = 0; i < 6; i++) {
        if (kind == pk[i]) {
            if (maybe_found[i]) {
                break;
            }
            maybe_found[i] = true;
            if (kind == PAWN) {
                if (direction == MOVING_UP) {
                    if (encounter(board, kind, side, 
                               ptransformation_get(MOVING_DOWN, true), pos)) {
                        return true;
                    }
                } else {
                    if (encounter(board, kind, side, 
                                 ptransformation_get(MOVING_UP, true), pos)) {
                        return true;
                    } 
                }
            } else {
                if (encounter(board, kind, side, transformation_get(kind), 
                                                                       pos)) {
                    return true;
                }
            }
        }
    }
    return false;
}

//only check trajectories starting from the king
//also these trajectories only have to be from pieces still on board
//do not check the same piece trajectory twice
bool square_targeted(Board* board, Pos pos, piece_side targeted) {
    bool maybe_found[6] = {false}; 
    unsigned char plen = board->plen;
    move_direction direction = board->direction;
    for (unsigned char i = 0; i < plen; i++) {
        Piece* piece;
        piece_side side;
        if (targeted == WHITE_SIDE) {
            piece = board->black_pieces[i];
            side = BLACK_SIDE;
        } else {
            piece = board->white_pieces[i];
            side = WHITE_SIDE;
        }
        if (piece == NULL) {
            continue;
        }
        if (handle_kind(board, piece->kind, direction, side, maybe_found, 
                                                                       pos)) {
            return true;
        }
    }
    return false;
}

Pos king_pos_get(Board* board, piece_side side) {
    unsigned char plen = board->plen;
    Piece* piece;
    for (unsigned char i = 0; i < plen; i++) {
        if (side == WHITE_SIDE) {
            piece = board->white_pieces[i]; 
        } else {
            piece = board->black_pieces[i]; 
        }
        if (piece->kind == KING) {
            return piece->position;
        }
    }
    fprintf(stderr, "King Not Found\n");
    exit(1);
}
 
bool check(Board* board, piece_side threatened) {
    Pos king_pos;
    if (threatened == WHITE_SIDE) {
        king_pos = king_pos_get(board, WHITE_SIDE);
    } else {
        king_pos = king_pos_get(board, BLACK_SIDE);
    }
    return square_targeted(board, king_pos, threatened);
    //be careful with case of king checking
}

























