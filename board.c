#include "board.h"

void set_standard_position(Board* board) {
    unsigned char size = board->size;
    for (unsigned char c = 0; c < size; c++) {
        board_set(board, make_pos(1, c), 
                  piece_new(PAWN, BLACK_SIDE, NOT_MOVED, make_pos(1, c)));
        board_set(board, make_pos(6, c), 
                  piece_new(PAWN, WHITE_SIDE, NOT_MOVED, make_pos(6, c)));
    }
    piece_kind pk[8] = {ROOK, ROOK, KNIGHT, KNIGHT, 
                        BISHOP, BISHOP, QUEEN, KING};
    unsigned char positions[8] = {0, 7, 1, 6, 2, 5, 3, 4};
    for (unsigned char i = 0; i < 8; i++) {
        board_set(board, make_pos(0, positions[i]),
          piece_new(pk[i], BLACK_SIDE, NOT_MOVED, make_pos(0, positions[i])));
        board_set(board, make_pos(7, positions[i]),
          piece_new(pk[i], WHITE_SIDE, NOT_MOVED, make_pos(7, positions[i])));
    }
}

Board* board_new(board_direction direction, start_position position) {
    unsigned char size = 8;
    Board* board = (Board*)malloc(sizeof(Board));
    malloc_check(board);
    board->matrix = (Piece***)malloc(sizeof(Piece**) * size);
    malloc_check(board->matrix);
    for (unsigned char r = 0; r < size; r++) {
        board->matrix[r] = (Piece**)malloc(sizeof(Piece*) * size);
        malloc_check(board->matrix[r]);
        for (unsigned char c = 0; c < size; c++) {
            board->matrix[r][c] = NULL;
        }
    }
    board->size = size;
    board->direction = WHITE_MOVING_UP;
    board->white_pieces = piecelist_new();
    board->black_pieces = piecelist_new();
    if (position == STANDARD_POSITION) {
        set_standard_position(board);
    }
    if (direction == BLACK_MOVING_UP) {
        board_flip(board);
    }
    return board;
}

void board_free(Board* board) {
    piecelist_free(board->white_pieces);
    piecelist_free(board->black_pieces);
    unsigned char size = board->size;
    for (unsigned char r = 0; r < size; r++) {
        free(board->matrix[r]);
    }
    free(board->matrix);
    free(board);
}

//Helper 
void print_chars(board_direction direction) {
    printf("  ");
    if (direction == WHITE_MOVING_UP) {
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
            if (board->direction == WHITE_MOVING_UP) {
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
            if (board->direction == WHITE_MOVING_UP) {
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

void board_set(Board* board, Pos pos, Piece* piece) {
    bounds_check(board->size, pos);
    Piece* to_delete = board_get(board, pos);
    if (to_delete) {
        if (to_delete->side == WHITE_SIDE) {
            piecelist_delete(board->white_pieces, to_delete);
        } else {
            piecelist_delete(board->black_pieces, to_delete);
        }
    }
    if (piece) {
        if (piece->side == WHITE_SIDE) {
            piecelist_insert(board->white_pieces, piece);
        } else {
            piecelist_insert(board->black_pieces, piece);
        }
        piece->position = pos; //in case
    }
    board->matrix[pos.r][pos.c] = piece;
}

void board_swap(Board* board, Pos pos1, Pos pos2) {
    bounds_check(board->size, pos1);
    bounds_check(board->size, pos2);
    Piece* piece1 = board_get(board, pos1);
    Piece* piece2 = board_get(board, pos2);
    board->matrix[pos1.r][pos1.c] = piece2;
    board->matrix[pos2.r][pos2.c] = piece1;
    if (piece2) {
        piece2->position = pos1; //in case
    }
    if (piece1) {
        piece1->position = pos2; //in case
    }
}

void board_flip(Board* board) {
    unsigned char size = board->size;
    for (unsigned char r = 0; r < (size / 2); r++) {
        for (unsigned char c = 0; c < size; c++) {
            Pos pos = make_pos(r, c);
            Pos mirr_pos = make_pos(size - 1 - r, size - 1 - c);
            board_swap(board, pos, mirr_pos);
        }
    }
    if (board->direction == WHITE_MOVING_UP) {
        board->direction = BLACK_MOVING_UP;
    } else {
        board->direction = WHITE_MOVING_UP;
    }
}

////Helper
//bool encounter(Board* board, piece_kind kind, side side, Pos pos, 
//                                            Transformation transformation) {
//    char* transformations = transformation.transformations;
//    unsigned char max_repeat = transformation.max_repeat;
//    for (unsigned char i = 0; i < transformation.len; i += 2) {
//        char tr = transformations[i], tc = transformations[i + 1];
//        Pos curr_pos = pos;
//        for (unsigned char j = 0; j < max_repeat; j++) {
//            curr_pos.r += tr;
//            curr_pos.c += tc;
//            if (out_of_bounds(board->size, curr_pos)) {
//                break;
//            }
//            Piece* piece = board_get(board, curr_pos);
//            if (piece == NULL) {
//                continue;
//            }
//            if (piece->side != side || piece->kind != kind) {
//                break;
//            }
//            return true;
//        }
//    }
//    return false;
//}
//
//bool handle_kind(Board* board, piece_kind kind, board_direction direction, 
//                                side side, bool* maybe_found, Pos pos) { 
//    piece_kind pk[6] = {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING};
//    for (unsigned char i = 0; i < 6; i++) {
//        if (kind == pk[i]) {
//            if (maybe_found[i]) {
//                break;
//            }
//            maybe_found[i] = true;
//            if (kind == PAWN) {
//                if (direction == WHITE_MOVING_UP) {
//                    if (encounter(board, kind, side, pos,
//                        transformation_get(kind, CAPTURE, BLACK_MOVING_UP))) {
//                        return true;
//                    }
//                } else {
//                    if (encounter(board, kind, side, pos, 
//                        transformation_get(kind, CAPTURE, WHITE_MOVING_UP))) {
//                        return true;
//                    } 
//                }
//            } else {
//                if (encounter(board, kind, side, pos, 
//                        transformation_get(kind, CAPTURE, WHITE_MOVING_UP))) { 
//                    return true;
//                }
//            }
//        }
//    }
//    return false;
//}
//
////only check trajectories starting from the king
////also these trajectories only have to be from pieces still on board
////do not check the same piece trajectory twice
//bool square_targeted(Board* board, Pos pos, piece_side targeted) {
//    bool maybe_found[6] = {false}; 
//    unsigned char plen = board->plen;
//    move_direction direction = board->direction;
//    for (unsigned char i = 0; i < plen; i++) {
//        Piece* piece;
//        piece_side side;
//        if (targeted == WHITE_SIDE) {
//            piece = board->black_pieces[i];
//            side = BLACK_SIDE;
//        } else {
//            piece = board->white_pieces[i];
//            side = WHITE_SIDE;
//        }
//        if (piece == NULL) {
//            continue;
//        }
//        if (handle_kind(board, piece->kind, direction, side, maybe_found, 
//                                                                       pos)) {
//            return true;
//        }
//    }
//    return false;
//}
//
//Pos king_pos_get(Board* board, piece_side side) {
//    unsigned char plen = board->plen;
//    Piece* piece;
//    for (unsigned char i = 0; i < plen; i++) {
//        if (side == WHITE_SIDE) {
//            piece = board->white_pieces[i]; 
//        } else {
//            piece = board->black_pieces[i]; 
//        }
//        if (piece->kind == KING) {
//            return piece->position;
//        }
//    }
//    fprintf(stderr, "King Not Found\n");
//    exit(1);
//}
// 
//bool check(Board* board, piece_side threatened) {
//    Pos king_pos;
//    if (threatened == WHITE_SIDE) {
//        king_pos = king_pos_get(board, WHITE_SIDE);
//    } else {
//        king_pos = king_pos_get(board, BLACK_SIDE);
//    }
//    return square_targeted(board, king_pos, threatened);
//    //be careful with case of king checking
//}

























