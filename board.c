#include "board.h"

Cell* cell_new(cell_type type, Piece* piece) {
    Cell* res = (Cell*)malloc(sizeof(Cell));
    res->type = type;
    res->piece = piece;
    return res;
}

void cell_free(Cell* cell) {
    if (cell->piece) {
        piece_free(cell->piece);
    }
    free(cell);
}

//Helper
void place_piece(Board* board, piece_kind kind, piece_side side, Pos pos) {
    unsigned char plen = board->plen;
    if (side == BLACK_SIDE) {
        if (board->bpnum == plen) {
            exit(1);
        }
    } else {
        if (board->wpnum == plen) {
            exit(1);
        }
    }
    Piece* piece = piece_new(kind, side, pos);
    board_set(board, pos, PIECE, piece);
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
}

void set_standard_position(Board* board) {
    unsigned char size = board->size;
    for (unsigned char c = 0; c < size; c++) {
        place_piece(board, PAWN, BLACK_SIDE, make_pos(1, c));
        place_piece(board, PAWN, WHITE_SIDE, make_pos(6, c));
    }
    place_piece(board, ROOK, BLACK_SIDE, make_pos(0, 0));           
    place_piece(board, ROOK, BLACK_SIDE, make_pos(0, 7));           
    place_piece(board, KNIGHT, BLACK_SIDE, make_pos(0, 1));       
    place_piece(board, KNIGHT, BLACK_SIDE, make_pos(0, 6));       
    place_piece(board, BISHOP, BLACK_SIDE, make_pos(0, 2));       
    place_piece(board, BISHOP, BLACK_SIDE, make_pos(0, 5));       
    place_piece(board, QUEEN, BLACK_SIDE, make_pos(0, 3));       
    place_piece(board, KING, BLACK_SIDE, make_pos(0, 4));       

    place_piece(board, ROOK, WHITE_SIDE, make_pos(7, 0));       
    place_piece(board, ROOK, WHITE_SIDE, make_pos(7, 7));       
    place_piece(board, KNIGHT, WHITE_SIDE, make_pos(7, 1));       
    place_piece(board, KNIGHT, WHITE_SIDE, make_pos(7, 6));       
    place_piece(board, BISHOP, WHITE_SIDE, make_pos(7, 2));       
    place_piece(board, BISHOP, WHITE_SIDE, make_pos(7, 5));       
    place_piece(board, QUEEN, WHITE_SIDE, make_pos(7, 3));       
    place_piece(board, KING, WHITE_SIDE, make_pos(7, 4));
}

Board* board_new(move_direction direction) {
    unsigned char size = 8;
    Board* board = (Board*)malloc(sizeof(Board));
    board->matrix = (Cell***)malloc(sizeof(Cell**) * size);
    for (unsigned char r = 0; r < size; r++) {
        board->matrix[r] = (Cell**)malloc(sizeof(Cell*) * size);
        for (unsigned char c = 0; c < size; c++) {
            board->matrix[r][c] = cell_new(EMPTY, NULL);
        }
    }
    board->size = size;
    board->direction = direction;
    board->plen = 20;
    unsigned char plen = board->plen;
    board->white_pieces = (Piece**)malloc(sizeof(Piece*) * plen);
    //4 spots added considering pawn promotion
    board->black_pieces = (Piece**)malloc(sizeof(Piece*) * plen);
    board->wpnum = 0;
    board->bpnum = 0;
    for (unsigned char i = 0; i < plen; i++) {
        board->white_pieces[i] = NULL;
        board->black_pieces[i] = NULL;
    }
    set_standard_position(board);
    if (direction == MOVING_DOWN) {
        board_flip(board);
        board->black_king = make_pos(7, 3);
        board->white_king = make_pos(0, 3);
    } else {
        board->black_king = make_pos(0, 4);
        board->white_king = make_pos(7, 4);
    }
    return board;
}

void board_free(Board* board) {
    unsigned char size = board->size;
    for (unsigned char r = 0; r < size; r++) {
        for (unsigned char c = 0; c < size; c++) {
            cell_free(board_get(board, make_pos(r, c)));
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
char convert_cell_to_char(Cell* cell) {
    char res;
    if (cell->type == EMPTY) {
        res = '.';
        return res;
    }
    switch (cell->piece->kind) {
        case PAWN:
            return maybe_upp(cell->piece->side, 'p');
        case KNIGHT:
            return maybe_upp(cell->piece->side, 'n');
        case BISHOP:
            return maybe_upp(cell->piece->side, 'b');
        case ROOK:
            return maybe_upp(cell->piece->side, 'r');
        case QUEEN:
            return maybe_upp(cell->piece->side, 'q');
        case KING:
            return maybe_upp(cell->piece->side, 'k');
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
                Cell* cell = board_get(board, make_pos(i - curr, j));
                char c = convert_cell_to_char(cell);
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
 
Cell* board_get(Board* board, Pos pos) {
    Cell* res = board->matrix[pos.r][pos.c];
    return res;
}

void board_set(Board* board, Pos pos, cell_type type, Piece* piece) {
    Cell* cell = board_get(board, pos);
    cell->type = type;
    cell->piece = piece;
    if (cell->piece) {
        cell->piece->position = pos;
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
            if (cell->type == EMPTY) {
                if (mirr_cell->type == EMPTY) {
                    continue;
                }
                board_set(board, pos, PIECE, mirr_cell->piece);
                board_set(board, mirr_pos, EMPTY, NULL);
            } else {
                if (mirr_cell->type == EMPTY) {
                    board_set(board, mirr_pos, PIECE, cell->piece);
                    board_set(board, pos, EMPTY, NULL);
                } else {
                    Piece* tmp_piece = cell->piece;
                    board_set(board, pos, PIECE, mirr_cell->piece);
                    board_set(board, mirr_pos, PIECE, tmp_piece);
                }
            }
        }
    }
}

bool check(Board* board, piece_side maybe_threatened) {
    Pos king_pos;
    if (maybe_threatened == WHITE_SIDE) {
        king_pos = board->white_king;
    } else {
        king_pos = board->black_king;
    }
    return square_maybe_targeted(board, king_pos, maybe_threatened);
    //be careful with case of king checking
}

bool out_of_bounds(unsigned char size, Pos pos) {
    return pos.r < 0 || pos.r >= size || pos.c < 0 || pos.c >= size;
}

//Helper
bool encounter(Board* board, piece_kind kind, piece_side side, 
                                              Transformation transformation) {
}

bool handle_kind(Board* board, piece_kind kind, move_direction direction, 
                                         piece_side side, bool* maybe_found) { 
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
                                    ptransformation_get(MOVING_DOWN, true))) {
                        return true;
                    }
                } else {
                    if (encounter(board, kind, side, 
                                      ptransformation_get(MOVING_UP, true))) {
                        return true;
                    } 
                }
            } else {
                if (encounter(board, kind, side, transformation_get(kind))) {
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
bool square_maybe_targeted(Board* board, Pos pos, piece_side targeted) {
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
        if (handle_kind(board, piece->kind, direction, side, maybe_found)) {
            return true;
        }
    }
    return false;
}



























