#include "piece.h"

Piece* piece_new(piece_kind kind, side side, piece_moved moved, Pos pos) {
    Piece* piece = (Piece*)malloc(sizeof(Piece));
    malloc_check(piece);
    piece->kind = kind;
    piece->side = side;
    piece->position = pos;
    piece->moved = moved;
    return piece;
}

void piece_show(Piece* piece) {
    if (piece == NULL) {
        fprintf(stderr, "Cannot show an empty cell\n");
        exit(1);
    }
    char* kind;
    char* pk_strings[] = {"PAWN", "BISHOP", "KNIGHT", "ROOK", "QUEEN", "KING"};
    kind = pk_strings[piece->kind];
    printf("(%s, ", kind);
    square_show(pos_convert(piece->position, WHITES_PERSPECTIVE));
    if (piece->moved == MOVED) {
        printf(", MOVED)");
    } else {
        printf(")");
    }
}

void piece_free(Piece* piece) {
    free(piece);
}

bool move_recognized(Translation* tr, unsigned char len, Displacement d, 
                        Direction dir, unsigned char max_repeat) {
    for (unsigned char i = 0; i < len; i++) {
        Direction curr_dir;
        curr_dir.r = tr[i].r;
        curr_dir.c = tr[i].c;
        if (max_repeat == 1) {
            //here comparing displacement to translation (has to be same)
            if (d.r == tr[i].r && d.c == tr[i].c) {
                return true;
            }
        } else if (direction_cmp(dir, curr_dir)) {
            return true;
            //here comparing directions (rook, bishop, queen)
        }
    }
    return false;
}

move_type* mts_allocate(move_type* copy_from, unsigned char len) {
    move_type* mts = (move_type*)malloc(sizeof(move_type) * len);
    malloc_check(mts);
    for (unsigned char i = 0; i < len; i++) {
        mts[i] = copy_from[i];
    }
    return mts;
}

move_type* plausible_mts(piece_kind kind, Displacement d, Direction dir,
                            side side, unsigned char* tylen) {
    if (kind == PAWN) {
        Translation tr_capture[2], tr_no_cap[1], tr_push_2[1];
        if (side == WHITE_SIDE) {
            tr_capture[0] = (Translation){-1, -1};
            tr_capture[1] = (Translation){-1, 1};
            tr_no_cap[0] = (Translation){-1, 0};
            tr_push_2[0] = (Translation){-2, 0};
        } else {
            tr_capture[0] = (Translation){1, -1};
            tr_capture[1] = (Translation){1, 1};
            tr_no_cap[0] = (Translation){1, 0};
            tr_push_2[0] = (Translation){2, 0};
        }
        if (move_recognized(tr_capture, 2, d, dir, 1)) {
            move_type mt_cap[3] = {CAPTURE, PAWN_PROMOTION_CAPTURE,EN_PASSANT};
            *tylen = 3;
            return mts_allocate(mt_cap, 3);
        }
        if (move_recognized(tr_no_cap, 1, d, dir, 1)) {
            move_type mt_no_cap[2] = {NO_CAPTURE, PAWN_PROMOTION_NO_CAPTURE};
            *tylen = 2;
            return mts_allocate(mt_no_cap, 2);
        }
        if (move_recognized(tr_push_2, 1, d, dir, 1)) {
            move_type mt_push_2[1] = {PAWN_PUSH_BY_TWO};
            *tylen = 1;
            return mts_allocate(mt_push_2, 1);
        }
    } else if (kind == KING) {
        Translation tr_king[8] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}, 
                                    {1, -1}, {-1, 1}, {1, 1}, {1, -1}};
        if (move_recognized(tr_king, 8, d, dir, 1)) {
            move_type mt_king[2] = {CAPTURE, NO_CAPTURE};
            *tylen = 2;
            return mts_allocate(mt_king, 2);
        }
        Translation tr_kcastle[1] = {{0, 2}};
        if (move_recognized(tr_kcastle, 1, d, dir, 1)) {
            move_type mt_kcastle[1] = {KINGSIDE_CASTLE};
            *tylen = 1;
            return mts_allocate(mt_kcastle, 1);
        }
        Translation tr_qcastle[1] = {{0, -2}};
        if (move_recognized(tr_qcastle, 1, d, dir, 1)) {
            move_type mt_qcastle[1] = {QUEENSIDE_CASTLE};
            *tylen = 1;
            return mts_allocate(mt_qcastle, 1);
        }
    } else {
        move_type mts[2] = {CAPTURE, NO_CAPTURE};
        *tylen = 2;
        Translation tr_knight[8] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, 
                                     {1, -2}, {1, 2}, {2, -1}, {2, 1}};
        Translation tr_rook[4] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
        Translation tr_queen[8] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}, 
                                    {-1, -1}, {-1, 1}, {1, 1}, {1, -1}};
        Translation tr_bishop[4] = {{-1, -1}, {-1, 1}, {1, 1}, {1, -1}};
        if ((kind == KNIGHT && move_recognized(tr_knight, 8, d, dir, 1)) ||
            (kind == BISHOP && move_recognized(tr_bishop, 4, d, dir, 7)) ||
            (kind == QUEEN && move_recognized(tr_queen, 8, d, dir, 7)) ||
            (kind == ROOK && move_recognized(tr_rook, 4, d, dir, 7))) {
            return mts_allocate(mts, 2);
        }
    }
    return NULL;
}

Piecelist* piecelist_new() {
    Piecelist* piecelist = (Piecelist*)malloc(sizeof(Piecelist));
    malloc_check(piecelist);
    piecelist->head = NULL;
    piecelist->len = 0;
    return piecelist;
}

void piecelist_show(Piecelist* piecelist) {
    Piece_entry* head = piecelist->head;
    if (head) {
        char* side;
        if (head->piece->side == WHITE_SIDE) {
            side = "WHITE";
        } else {
            side = "BLACK";
        }
        printf("%hhu %s pieces still on board\n", piecelist->len, side);
        while (head) {
            piece_show(head->piece);
            printf(", ");
            head = head->next;
        }
    }
    printf("\n");
}

void piecelist_insert(Piecelist* piecelist, Piece* piece) {
    Piece_entry* piece_entry = (Piece_entry*)malloc(sizeof(Piece_entry));
    malloc_check(piece_entry);
    piece_entry->piece = piece;
    if (piece->kind == KING) {
        piece_entry->next = piecelist->head;
        piecelist->head = piece_entry;
    } else if (piecelist->len == 0) {
        piece_entry->next = NULL;
        piecelist->head = piece_entry;
    } else {
        Piece_entry* tmp = piecelist->head->next;
        piecelist->head->next = piece_entry;
        piece_entry->next = tmp;
    }
    (piecelist->len)++;
}

//Helper
void piece_entry_free(Piece_entry* piece_entry) {
    piece_free(piece_entry->piece);
    free(piece_entry);
}

void piecelist_delete(Piecelist* piecelist, Piece* piece) {
    if (piecelist->len == 0) {
        fprintf(stderr, "No pieces to delete from\n");
        exit(1);
    }
    Piece_entry* head = piecelist->head; 
    if (head->piece == piece) {
        Piece_entry* to_delete = head;
        piecelist->head = head->next;
        piece_entry_free(to_delete);
    } else {
        Piece_entry* curr = head;
        while (curr->next) {
            if (curr->next->piece == piece) {
                Piece_entry* to_delete = curr->next; 
                curr->next = curr->next->next;
                piece_entry_free(to_delete);
                (piecelist->len)--;
                break;
            }
            curr = curr->next;
        }
    }
}

void piecelist_free(Piecelist* piecelist) {
    Piece_entry* to_delete;
    while (piecelist->head) {
        to_delete = piecelist->head;
        piecelist->head = piecelist->head->next;
        piece_entry_free(to_delete);
    }
    free(piecelist);
}

side opp_side(side side) {
    if (side == WHITE_SIDE) {
        return BLACK_SIDE;
    }
    return WHITE_SIDE;
}

