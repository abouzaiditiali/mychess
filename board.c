#include "board.h"

Board* board_new() {
    Board* board = (Board*)malloc(sizeof(Board));
    malloc_check(board);
    board->matrix = (Piece***)malloc(sizeof(Piece**) * board_size);
    malloc_check(board->matrix);
    for (unsigned char r = 0; r < board_size; r++) {
        board->matrix[r] = (Piece**)malloc(sizeof(Piece*) * board_size);
        malloc_check(board->matrix[r]);
        for (unsigned char c = 0; c < board_size; c++) {
            board->matrix[r][c] = NULL;
        }
    }
    board->white_pieces = piecelist_new();
    board->black_pieces = piecelist_new();
    return board;
}

void board_free(Board* board) {
    piecelist_free(board->white_pieces);
    piecelist_free(board->black_pieces);
    for (unsigned char r = 0; r < board_size; r++) {
        free(board->matrix[r]);
    }
    free(board->matrix);
    free(board);
}

//Helper 
void print_chars(player_perspective perspective) {
    printf("  ");
    if (perspective == WHITES_PERSPECTIVE) {
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
char maybe_upp(side side, char c) {
    if (side == WHITE_SIDE) {
        char new_c = c - ('a' - 'A');
        return new_c;
    }
    return c;
}

//Helper
char convert_piece_to_char(Piece* piece) {
    char pk_chars[] = {'p', 'b', 'n', 'r', 'q', 'k'};
    if (piece == NULL) {
        return '.';
    }
    return maybe_upp(piece->side, pk_chars[piece->kind]);
}

void board_show(Board* board, player_perspective perspective) {
    printf("\n");
    print_chars(perspective);
    unsigned char mu = 8, md = 1, curr = 1;
    for (unsigned char i = 0; i < 17; i++) {
        if (i % 2 == 0) {
            printf("   ");
            for (unsigned char j = 0; j < 8; j++) {
                printf(" ---");
            }
        } else {
            printf(" ");
            if (perspective == WHITES_PERSPECTIVE) {
                printf("%hhu", mu);
            } else {
                printf("%hhu", md);
            }
            printf(" |");
            for (unsigned char j = 0; j < 8; j++) {
                Pos wp_pos = pos_make(i - curr, j);
                Piece* piece;
                if (perspective == WHITES_PERSPECTIVE) {
                    piece = board_get(board, wp_pos);
                } else {
                    piece = board_get(board, pos_flip(wp_pos));
                }
                char c = convert_piece_to_char(piece);
                printf(" %c |", c);
            }
            curr++;
            printf(" ");
            if (perspective == WHITES_PERSPECTIVE) {
                printf("%hhu", mu--);
            } else {
                printf("%hhu", md++);
            }
        }
        printf("\n"); 
    }
    print_chars(perspective);
    printf("\n");
    piecelist_show(board->white_pieces);
    piecelist_show(board->black_pieces);
    printf("\n");
}

Piece* board_get(Board* board, Pos pos) {
    bounds_check(pos.r, pos.c);
    return board->matrix[pos.r][pos.c];
}

void board_set(Board* board, Pos pos, Piece* piece, piece_status status) {
    Piece* to_delete = board_get(board, pos);
    if (to_delete) {
        if (to_delete->side == WHITE_SIDE) {
            piecelist_delete(board->white_pieces, to_delete);
        } else {
            piecelist_delete(board->black_pieces, to_delete);
        }
    }
    if (piece) {
        if (status == NEW) {
            if (piece->side == WHITE_SIDE) {
                piecelist_insert(board->white_pieces, piece);
            } else {
                piecelist_insert(board->black_pieces, piece);
            }
        } else {
            Pos fpos = piece->position;
            board->matrix[fpos.r][fpos.c] = NULL;
            piece->position = pos;
            piece->moved = MOVED;
        }
    }
    board->matrix[pos.r][pos.c] = piece;
}

//void board_flip(Board* board) {
//    for (unsigned char r = 0; r < (board_size / 2); r++) {
//        for (unsigned char c = 0; c < board_size; c++) {
//            Pos pos = pos_make(r, c);
//            Pos fpos = pos_flip(pos);
//            board_swap(board, pos, fpos);
//        }
//    }
//}

Pos kpos_get(Board* board, side kside) {
    Piece_entry* head;
    if (kside == WHITE_SIDE) {
        head = board->white_pieces->head;
    } else {
        head = board->black_pieces->head;
    }
    if (head) { //king expected to be first in the linked list
        if (head->piece->kind == KING) {
            return head->piece->position;
        }
    }
    fprintf(stderr, "King Not Found\n");
    exit(1);
}

