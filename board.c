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

Piece* nrook_get(Board* board, move_type mt, side side) {
    if (mt == KINGSIDE_CASTLE && side == WHITE_SIDE) {
        return board_get(board, pos_make(7, 7));
    } else if (mt == KINGSIDE_CASTLE && side == BLACK_SIDE) {
        return board_get(board, pos_make(0, 7));
    } else if (mt == QUEENSIDE_CASTLE && side == WHITE_SIDE) {
        return board_get(board, pos_make(7, 0));
    } else {
        return board_get(board, pos_make(0, 0));
    }
}

bool void_traj_check(Board* board, Direction dir, Pos fpos, Pos tpos) {
    Pos curr_pos = pos_make(fpos.r + dir.r, fpos.c + dir.c);
    Piece* curr_piece;
    while (!pos_cmp(curr_pos, tpos)) {
        curr_piece = board_get(board, curr_pos);
        if (curr_piece) {
            return false;
        }
        curr_pos.r += dir.r;
        curr_pos.c += dir.c;
    }
    return true;
}

Piece* piece_traj_encounter(Board* board, Direction dir, Pos fpos, 
                          piece_kind* pk, unsigned char pklen, side side, 
                          unsigned char max_repeat) {
    char curr_r = (char)fpos.r + dir.r, curr_c = (char)fpos.c + dir.c;
    Piece* curr_piece;
    for (unsigned char i = 0; i < max_repeat; i++) {
        if (out_of_bounds(curr_r, curr_c)) {
            return NULL;
        } 
        curr_piece = board_get(board, pos_make(curr_r, curr_c)); //cannot fail
        if (!curr_piece) {
            continue;
        }
        if (curr_piece->side != side) {
            return NULL;
        }
        for (unsigned char i = 0; i < pklen; i++) {
            if (curr_piece->kind == pk[i]) {
                return curr_piece;
            }
        }
        curr_r += dir.r;
        curr_c += dir.c;
    }
    return NULL;
}

//there is always at most one piece that ever pins in each case
Piece* pin(Board* board, Pos pinned_pos, side pinning_side) {
    Pos kpos = kpos_get(board, opp_side(pinning_side));
    Pos fpos = pinned_pos;
    Displacement d = pos_displacement(kpos, fpos);
    if ((abs(d.r) == 1 && abs(d.c) == 2) || (abs(d.r) == 2 && abs(d.c) == 1)) {
        return NULL;
    }
    Direction dir = displacement_direction(d); 
    if (dir.r == 0 && dir.c == 0) {
        return NULL;
    }
    if (!void_traj_check(board, dir, kpos, fpos)) {
        return NULL;
    }
    piece_kind pk[2];
    if (dir.r == 0 || dir.c == 0) {
        pk[0] = ROOK;
        pk[1] = QUEEN;
    } else {
        pk[0] = BISHOP;
        pk[1] = QUEEN;
    }
    return piece_traj_encounter(board, dir, fpos, pk, 2, pinning_side, 7);
}

//find a way to keep track of which kinds are left on board (count +-)
bool square_threatened(Board* board, Pos pos, side threatening) {
    Direction dr1[4] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}}; //along file/rank
    piece_kind pk1[2] = {QUEEN, ROOK};
    Direction dr2[4] = {{-1, -1}, {-1, 1}, {1, 1}, {1, -1}}; //diagonals
    piece_kind pk2[2] = {QUEEN, BISHOP};
    Direction dr3[8] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, 
                            {1, -2}, {1, 2}, {2, -1}, {2, 1}}; //L
    piece_kind pk3[1] = {KNIGHT};
    Direction dr4[8] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}, 
                            {1, -1}, {-1, 1}, {1, 1}, {1, -1}}; //cing
    piece_kind pk4[1] = {KING};
    Direction dr5[2];
    if (threatening == WHITE_SIDE) {
        dr5[0] = (Direction){1, -1};
        dr5[1] = (Direction){1, 1};
    } else {
        dr5[0] = (Direction){-1, -1};
        dr5[1] = (Direction){-1, 1};
    }
    piece_kind pk5[1] = {PAWN};
    for (unsigned char i = 0; i < 4; i++) {
        if (piece_traj_encounter(board, dr1[i], pos, pk1, 2, threatening, 7)) {
            return true;
        }
    }
    for (unsigned char i = 0; i < 4; i++) {
        if (piece_traj_encounter(board, dr2[i], pos, pk2, 2, threatening, 7)) {
            return true;
        }
    }
    for (unsigned char i = 0; i < 8; i++) {
        if (piece_traj_encounter(board, dr3[i], pos, pk3, 1, threatening, 1)) {
            return true;
        }
    }
    for (unsigned char i = 0; i < 8; i++) {
        if (piece_traj_encounter(board, dr4[i], pos, pk4, 1, threatening, 1)) {
            return true;
        }
    }
    for (unsigned char i = 0; i < 2; i++) {
        if (piece_traj_encounter(board, dr5[i], pos, pk5, 1, threatening, 1)) {
            return true;
        }
    }
    return false;
}

bool legal_to_move(Board* board, Piece* op, Pos tpos, Pos captured_pos,
                                                               move_type mt) {
    switch (op->kind) {
        case QUEEN:
        case BISHOP:
        case ROOK:
        case KNIGHT:
        case PAWN: {
            Piece* pinning = pin(board, op->position, opp_side(op->side));
            if (!pinning) {
                return true;
            }
            Pos pinning_pos = pinning->position;
            if (mt == CAPTURE || mt == PAWN_PROMOTION_CAPTURE) {
                return pos_cmp(captured_pos, pinning_pos);
            }
            Pos kpos = kpos_get(board, op->side);
            Displacement d1 = pos_displacement(tpos, pinning_pos); 
            Displacement d2 = pos_displacement(kpos, pinning_pos); 
            Direction dir1 = displacement_direction(d1);
            Direction dir2 = displacement_direction(d2);
            return direction_cmp(dir1, dir2);
            //tpos in trajectory between king and pinning piece;
        }
        case KING: {
            side opp = opp_side(op->side);
            if (square_threatened(board, tpos, opp)) {
                return false;
            }
            Pos crossed = crossedpos_get(mt, tpos);
            return !square_threatened(board, crossed, opp);
        }
    }
} 


